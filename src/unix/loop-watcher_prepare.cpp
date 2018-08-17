
#include "uv.h"
#include "internal.h"

  int uv_prepare_init(uv_loop_t* loop, uv_prepare_t* handle) {              
    uv__handle_init(loop, (uv_handle_t*)handle, UV_PREPARE);                   
    handle->prepare_cb = NULL;                                                 
    return 0;                                                                 
  }                                                                           

  int uv_prepare_start(uv_prepare_t* handle, uv_prepare_cb cb) {           
    if (uv__is_active(handle)) return 0;                                      
    if (cb == NULL) return -EINVAL;                                           
    QUEUE_INSERT_HEAD(&handle->loop->prepare_handles, &handle->queue);         
    handle->prepare_cb = cb;                                                   
    uv__handle_start(handle);                                                 
    return 0;                                                                 
  }                                                                           

  int uv_prepare_stop(uv_prepare_t* handle) {                               
    if (!uv__is_active(handle)) return 0;                                     
    QUEUE_REMOVE(&handle->queue);                                             
    uv__handle_stop(handle);                                                  
    return 0;                                                                 
  }                                                                           

  void uv__run_prepare(uv_loop_t* loop) {                                      
    uv_prepare_t* h;                                                         
    QUEUE queue;                                                              
    QUEUE* q;                                                                 
    QUEUE_MOVE(&loop->prepare_handles, &queue);                                
    while (!QUEUE_EMPTY(&queue)) {                                            
      q = QUEUE_HEAD(&queue);                                                 
      h = QUEUE_DATA(q, uv_prepare_t, queue);                                
      QUEUE_REMOVE(q);                                                        
      QUEUE_INSERT_TAIL(&loop->prepare_handles, q);                            
      h->prepare_cb(h);                                                        
    }                                                                         
  }                                                                           

  void uv__prepare_close(uv_prepare_t* handle) {                            
    uv_prepare_stop(handle);                                                 
  }
