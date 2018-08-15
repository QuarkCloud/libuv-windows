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

#include "linux-syscalls.h"
#include <unistd.h>
#include <signal.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <errno.h>

#if defined(__i386__)
# ifndef __NR_socketcall
#  define __NR_socketcall 102
# endif
#endif

#ifndef __NR_accept4
# if defined(__x86_64__)
#  define __NR_accept4 288
# elif defined(__i386__)
   /* Nothing. Handled through socketcall(). */
# endif
#endif /* __NR_accept4 */

#ifndef __NR_eventfd
# if defined(__x86_64__)
#  define __NR_eventfd 284
# elif defined(__i386__)
#  define __NR_eventfd 323
# endif
#endif /* __NR_eventfd */

#ifndef __NR_eventfd2
# if defined(__x86_64__)
#  define __NR_eventfd2 290
# elif defined(__i386__)
#  define __NR_eventfd2 328
# endif
#endif /* __NR_eventfd2 */

#ifndef __NR_epoll_create
# if defined(__x86_64__)
#  define __NR_epoll_create 213
# elif defined(__i386__)
#  define __NR_epoll_create 254
# endif
#endif /* __NR_epoll_create */

#ifndef __NR_epoll_create1
# if defined(__x86_64__)
#  define __NR_epoll_create1 291
# elif defined(__i386__)
#  define __NR_epoll_create1 329
# endif
#endif /* __NR_epoll_create1 */

#ifndef __NR_epoll_ctl
# if defined(__x86_64__)
#  define __NR_epoll_ctl 233 /* used to be 214 */
# elif defined(__i386__)
#  define __NR_epoll_ctl 255
# endif
#endif /* __NR_epoll_ctl */

#ifndef __NR_epoll_wait
# if defined(__x86_64__)
#  define __NR_epoll_wait 232 /* used to be 215 */
# elif defined(__i386__)
#  define __NR_epoll_wait 256
# endif
#endif /* __NR_epoll_wait */

#ifndef __NR_epoll_pwait
# if defined(__x86_64__)
#  define __NR_epoll_pwait 281
# elif defined(__i386__)
#  define __NR_epoll_pwait 319
# endif
#endif /* __NR_epoll_pwait */

#ifndef __NR_inotify_init
# if defined(__x86_64__)
#  define __NR_inotify_init 253
# elif defined(__i386__)
#  define __NR_inotify_init 291
# endif
#endif /* __NR_inotify_init */

#ifndef __NR_inotify_init1
# if defined(__x86_64__)
#  define __NR_inotify_init1 294
# elif defined(__i386__)
#  define __NR_inotify_init1 332
# endif
#endif /* __NR_inotify_init1 */

#ifndef __NR_inotify_add_watch
# if defined(__x86_64__)
#  define __NR_inotify_add_watch 254
# elif defined(__i386__)
#  define __NR_inotify_add_watch 292
# endif
#endif /* __NR_inotify_add_watch */

#ifndef __NR_inotify_rm_watch
# if defined(__x86_64__)
#  define __NR_inotify_rm_watch 255
# elif defined(__i386__)
#  define __NR_inotify_rm_watch 293
# endif
#endif /* __NR_inotify_rm_watch */

#ifndef __NR_pipe2
# if defined(__x86_64__)
#  define __NR_pipe2 293
# elif defined(__i386__)
#  define __NR_pipe2 331
# endif
#endif /* __NR_pipe2 */

#ifndef __NR_recvmmsg
# if defined(__x86_64__)
#  define __NR_recvmmsg 299
# elif defined(__i386__)
#  define __NR_recvmmsg 337
# endif
#endif /* __NR_recvmsg */

#ifndef __NR_sendmmsg
# if defined(__x86_64__)
#  define __NR_sendmmsg 307
# elif defined(__i386__)
#  define __NR_sendmmsg 345
# endif
#endif /* __NR_sendmmsg */

#ifndef __NR_utimensat
# if defined(__x86_64__)
#  define __NR_utimensat 280
# elif defined(__i386__)
#  define __NR_utimensat 320
# endif
#endif /* __NR_utimensat */

#ifndef __NR_preadv
# if defined(__x86_64__)
#  define __NR_preadv 295
# elif defined(__i386__)
#  define __NR_preadv 333
# endif
#endif /* __NR_preadv */

#ifndef __NR_pwritev
# if defined(__x86_64__)
#  define __NR_pwritev 296
# elif defined(__i386__)
#  define __NR_pwritev 334
# endif
#endif /* __NR_pwritev */

#ifndef __NR_dup3
# if defined(__x86_64__)
#  define __NR_dup3 292
# elif defined(__i386__)
#  define __NR_dup3 330
# endif
#endif /* __NR_pwritev */


int uv__accept4(int fd, struct sockaddr* addr, socklen_t* addrlen, int flags) {
#if defined(__i386__)
  unsigned long args[4];
  int r;

  args[0] = (unsigned long) fd;
  args[1] = (unsigned long) addr;
  args[2] = (unsigned long) addrlen;
  args[3] = (unsigned long) flags;

  r = syscall(__NR_socketcall, 18 /* SYS_ACCEPT4 */, args);

  /* socketcall() raises EINVAL when SYS_ACCEPT4 is not supported but so does
   * a bad flags argument. Try to distinguish between the two cases.
   */
  if (r == -1)
    if (errno == EINVAL)
      if ((flags & ~(UV__SOCK_CLOEXEC|UV__SOCK_NONBLOCK)) == 0)
        errno = ENOSYS;

  return r;
#elif defined(__NR_accept4)
  return syscall(__NR_accept4, fd, addr, addrlen, flags);
#endif
}


int uv__eventfd(unsigned int count)
{
  return syscall(__NR_eventfd, count);
}


int uv__eventfd2(unsigned int count, int flags)
{
  return syscall(__NR_eventfd2, count, flags);
}


int uv__epoll_create(int size)
{
  return syscall(__NR_epoll_create, size);
}


int uv__epoll_create1(int flags)
{
  return syscall(__NR_epoll_create1, flags);
}


int uv__epoll_ctl(int epfd, int op, int fd, struct uv__epoll_event* events)
{
  return syscall(__NR_epoll_ctl, epfd, op, fd, events);
}


int uv__epoll_wait(int epfd,struct uv__epoll_event* events,int nevents,int timeout)
{
  return syscall(__NR_epoll_wait, epfd, events, nevents, timeout);
}


int uv__epoll_pwait(int epfd,struct uv__epoll_event* events,
                    int nevents,int timeout,uint64_t sigmask)
{
  return syscall(__NR_epoll_pwait,epfd,events,nevents,timeout,&sigmask,sizeof(sigmask));
}


int uv__inotify_init(void)
{
  return syscall(__NR_inotify_init);
}

int uv__inotify_init1(int flags)
{
  return syscall(__NR_inotify_init1, flags);
}


int uv__inotify_add_watch(int fd, const char* path, uint32_t mask)
{
  return syscall(__NR_inotify_add_watch, fd, path, mask);
}


int uv__inotify_rm_watch(int fd, int32_t wd)
{
  return syscall(__NR_inotify_rm_watch, fd, wd);
}

int uv__pipe2(int pipefd[2], int flags)
{
  return syscall(__NR_pipe2, pipefd, flags);
}

int uv__sendmmsg(int fd,struct uv__mmsghdr* mmsg,unsigned int vlen,unsigned int flags)
{
  return syscall(__NR_sendmmsg, fd, mmsg, vlen, flags);
}

int uv__recvmmsg(int fd,struct uv__mmsghdr* mmsg,unsigned int vlen,unsigned int flags,struct timespec* timeout)
{
  return syscall(__NR_recvmmsg, fd, mmsg, vlen, flags, timeout);
}

int uv__utimesat(int dirfd,const char* path,const struct timespec times[2],int flags)
{
  return syscall(__NR_utimensat, dirfd, path, times, flags);
}


ssize_t uv__preadv(int fd, const struct iovec *iov, int iovcnt, int64_t offset)
{
  return syscall(__NR_preadv, fd, iov, iovcnt, (long)offset, (long)(offset >> 32));
}


ssize_t uv__pwritev(int fd, const struct iovec *iov, int iovcnt, int64_t offset)
{
  return syscall(__NR_pwritev, fd, iov, iovcnt, (long)offset, (long)(offset >> 32));
}


int uv__dup3(int oldfd, int newfd, int flags)
{
  return syscall(__NR_dup3, oldfd, newfd, flags);
}
