
#include "uv.h"
#include "internal.h"

  int uv_check_init(uv_loop_t* loop, uv_check_t* handle) {              
    uv__handle_init(loop, (uv_handle_t*)handle, UV_PREPARE);                   
    handle->check_cb = NULL;                                                 
    return 0;                                                                 
  }                                                                           

  int uv_check_start(uv_check_t* handle, uv_check_cb cb) {           
    if (uv__is_active(handle)) return 0;                                      
    if (cb == NULL) return -EINVAL;                                           
    QUEUE_INSERT_HEAD(&handle->loop->check_handles, &handle->queue);         
    handle->check_cb = cb;                                                   
    uv__handle_start(handle);                                                 
    return 0;                                                                 
  }                                                                           

  int uv_check_stop(uv_check_t* handle) {                               
    if (!uv__is_active(handle)) return 0;                                     
    QUEUE_REMOVE(&handle->queue);                                             
    uv__handle_stop(handle);                                                  
    return 0;                                                                 
  }                                                                           

  void uv__run_check(uv_loop_t* loop) {                                      
    uv_check_t* h;                                                         
    QUEUE queue;                                                              
    QUEUE* q;                                                                 
    QUEUE_MOVE(&loop->check_handles, &queue);                                
    while (!QUEUE_EMPTY(&queue)) {                                            
      q = QUEUE_HEAD(&queue);                                                 
      h = QUEUE_DATA(q, uv_check_t, queue);                                
      QUEUE_REMOVE(q);                                                        
      QUEUE_INSERT_TAIL(&loop->check_handles, q);                            
      h->check_cb(h);                                                        
    }                                                                         
  }                                                                           

  void uv__check_close(uv_check_t* handle) {                            
    uv_check_stop(handle);                                                 
  }