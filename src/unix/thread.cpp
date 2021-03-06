/* Copyright Joyent, Inc. and other Node contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "uv.h"
#include "internal.h"

#include <pthread.h>
#include <assert.h>
#include <errno.h>

#include <sys/time.h>
#include <sys/resource.h>  /* getrlimit() */
#include <unistd.h>  /* getpagesize() */

#include <limits.h>

#undef NANOSEC
#define NANOSEC ((uint64_t) 1e9)


int uv_thread_create(uv_thread_t *tid, void (*entry)(void *arg), void *arg) {
  int err;
  pthread_attr_t* attr;

  /* On OSX threads other than the main thread are created with a reduced stack
   * size by default, adjust it to RLIMIT_STACK.
   */
  attr = NULL;

  err = pthread_create(tid, attr, (void*(*)(void*)) entry, arg);

  if (attr != NULL)
    pthread_attr_destroy(attr);

  return -err;
}


uv_thread_t uv_thread_self(void) {
  return pthread_self();
}

int uv_thread_join(uv_thread_t *tid) {
  return -pthread_join(*tid, NULL);
}


int uv_thread_equal(const uv_thread_t* t1, const uv_thread_t* t2) {
  return pthread_equal(*t1, *t2);
}


int uv_mutex_init(uv_mutex_t* mutex) {
#if defined(NDEBUG) || !defined(PTHREAD_MUTEX_ERRORCHECK)
  return -pthread_mutex_init(mutex, NULL);
#else
  pthread_mutexattr_t attr;
  int err;

  if (pthread_mutexattr_init(&attr))
    abort();

  if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK))
    abort();

  err = pthread_mutex_init(mutex, &attr);

  if (pthread_mutexattr_destroy(&attr))
    abort();

  return -err;
#endif
}


void uv_mutex_destroy(uv_mutex_t* mutex) {
  if (pthread_mutex_destroy(mutex))
    abort();
}


void uv_mutex_lock(uv_mutex_t* mutex) {
  if (pthread_mutex_lock(mutex))
    abort();
}


int uv_mutex_trylock(uv_mutex_t* mutex) {
  int err;

  err = pthread_mutex_trylock(mutex);
  if (err) {
    if (err != EBUSY && err != EAGAIN)
      abort();
    return -EBUSY;
  }

  return 0;
}


void uv_mutex_unlock(uv_mutex_t* mutex) {
  if (pthread_mutex_unlock(mutex))
    abort();
}


int uv_rwlock_init(uv_rwlock_t* rwlock) {
  return -pthread_rwlock_init(rwlock, NULL);
}


void uv_rwlock_destroy(uv_rwlock_t* rwlock) {
  if (pthread_rwlock_destroy(rwlock))
    abort();
}


void uv_rwlock_rdlock(uv_rwlock_t* rwlock) {
  if (pthread_rwlock_rdlock(rwlock))
    abort();
}


int uv_rwlock_tryrdlock(uv_rwlock_t* rwlock) {
  int err;

  err = pthread_rwlock_tryrdlock(rwlock);
  if (err) {
    if (err != EBUSY && err != EAGAIN)
      abort();
    return -EBUSY;
  }

  return 0;
}


void uv_rwlock_rdunlock(uv_rwlock_t* rwlock) {
  if (pthread_rwlock_unlock(rwlock))
    abort();
}


void uv_rwlock_wrlock(uv_rwlock_t* rwlock) {
  if (pthread_rwlock_wrlock(rwlock))
    abort();
}


int uv_rwlock_trywrlock(uv_rwlock_t* rwlock) {
  int err;

  err = pthread_rwlock_trywrlock(rwlock);
  if (err) {
    if (err != EBUSY && err != EAGAIN)
      abort();
    return -EBUSY;
  }

  return 0;
}


void uv_rwlock_wrunlock(uv_rwlock_t* rwlock) {
  if (pthread_rwlock_unlock(rwlock))
    abort();
}


void uv_once(uv_once_t* guard, void (*callback)(void)) {
  if (pthread_once(guard, callback))
    abort();
}

int uv_sem_init(uv_sem_t* sem, unsigned int value) {
  if (sem_init(sem, 0, value))
    return -errno;
  return 0;
}


void uv_sem_destroy(uv_sem_t* sem) {
  if (sem_destroy(sem))
    abort();
}


void uv_sem_post(uv_sem_t* sem) {
  if (sem_post(sem))
    abort();
}


void uv_sem_wait(uv_sem_t* sem) {
  int r;

  do
    r = sem_wait(sem);
  while (r == -1 && errno == EINTR);

  if (r)
    abort();
}


int uv_sem_trywait(uv_sem_t* sem) {
  int r;

  do
    r = sem_trywait(sem);
  while (r == -1 && errno == EINTR);

  if (r) {
    if (errno == EAGAIN)
      return -EAGAIN;
    abort();
  }

  return 0;
}


int uv_cond_init(uv_cond_t* cond) {
  pthread_condattr_t attr;
  int err;

  err = pthread_condattr_init(&attr);
  if (err)
    return -err;

  err = pthread_cond_init(cond, &attr);
  if (err)
    goto error2;

  err = pthread_condattr_destroy(&attr);
  if (err)
    goto error;

  return 0;

error:
  pthread_cond_destroy(cond);
error2:
  pthread_condattr_destroy(&attr);
  return -err;
}

void uv_cond_destroy(uv_cond_t* cond) {

  if (pthread_cond_destroy(cond))
    abort();
}

void uv_cond_signal(uv_cond_t* cond) {
  if (pthread_cond_signal(cond))
    abort();
}

void uv_cond_broadcast(uv_cond_t* cond) {
  if (pthread_cond_broadcast(cond))
    abort();
}

void uv_cond_wait(uv_cond_t* cond, uv_mutex_t* mutex) {
  if (pthread_cond_wait(cond, mutex))
    abort();
}


int uv_cond_timedwait(uv_cond_t* cond, uv_mutex_t* mutex, uint64_t timeout) {
  int r;
  struct timespec ts;

  timeout += uv__hrtime(UV_CLOCK_PRECISE);
  ts.tv_sec = timeout / NANOSEC;
  ts.tv_nsec = timeout % NANOSEC;
  r = pthread_cond_timedwait(cond, mutex, &ts);


  if (r == 0)
    return 0;

  if (r == ETIMEDOUT)
    return -ETIMEDOUT;

  abort();
  return -EINVAL;  /* Satisfy the compiler. */
}


int uv_barrier_init(uv_barrier_t* barrier, unsigned int count) {
  return -pthread_barrier_init(barrier, NULL, count);
}


void uv_barrier_destroy(uv_barrier_t* barrier) {
  if (pthread_barrier_destroy(barrier))
    abort();
}


int uv_barrier_wait(uv_barrier_t* barrier) {
  int r = pthread_barrier_wait(barrier);
  if (r && r != PTHREAD_BARRIER_SERIAL_THREAD)
    abort();
  return r == PTHREAD_BARRIER_SERIAL_THREAD;
}


int uv_key_create(uv_key_t* key) {
  return -pthread_key_create(key, NULL);
}


void uv_key_delete(uv_key_t* key) {
  if (pthread_key_delete(*key))
    abort();
}


void* uv_key_get(uv_key_t* key) {
  return pthread_getspecific(*key);
}


void uv_key_set(uv_key_t* key, void* value) {
  if (pthread_setspecific(*key, value))
    abort();
}
