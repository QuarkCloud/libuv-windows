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

#ifndef UV_ERRNO_H_
#define UV_ERRNO_H_

#include <errno.h>

#define UV__EOF     (-4095)
#define UV__UNKNOWN (-4094)

#define UV__EAI_ADDRFAMILY  (-3000)
#define UV__EAI_AGAIN       (-3001)
#define UV__EAI_BADFLAGS    (-3002)
#define UV__EAI_CANCELED    (-3003)
#define UV__EAI_FAIL        (-3004)
#define UV__EAI_FAMILY      (-3005)
#define UV__EAI_MEMORY      (-3006)
#define UV__EAI_NODATA      (-3007)
#define UV__EAI_NONAME      (-3008)
#define UV__EAI_OVERFLOW    (-3009)
#define UV__EAI_SERVICE     (-3010)
#define UV__EAI_SOCKTYPE    (-3011)
#define UV__EAI_BADHINTS    (-3013)
#define UV__EAI_PROTOCOL    (-3014)

/* Only map to the system errno on non-Windows platforms. It's apparently
 * a fairly common practice for Windows programmers to redefine errno codes.
 */
# define UV__E2BIG (-E2BIG)
# define UV__EACCES (-EACCES)
# define UV__EADDRINUSE (-EADDRINUSE)
# define UV__EADDRNOTAVAIL (-EADDRNOTAVAIL)
# define UV__EAFNOSUPPORT (-EAFNOSUPPORT)
# define UV__EAGAIN (-EAGAIN)
# define UV__EALREADY (-EALREADY)
# define UV__EBADF (-EBADF)
# define UV__EBUSY (-EBUSY)
# define UV__ECANCELED (-ECANCELED)
# define UV__ECHARSET (-ECHARSET)
# define UV__ECONNABORTED (-ECONNABORTED)
# define UV__ECONNREFUSED (-ECONNREFUSED)
# define UV__ECONNRESET (-ECONNRESET)
# define UV__EDESTADDRREQ (-EDESTADDRREQ)
# define UV__EEXIST (-EEXIST)
# define UV__EFAULT (-EFAULT)
# define UV__EHOSTUNREACH (-EHOSTUNREACH)
# define UV__EINTR (-EINTR)
# define UV__EINVAL (-EINVAL)
# define UV__EIO (-EIO)
# define UV__EISCONN (-EISCONN)
# define UV__EISDIR (-EISDIR)
# define UV__ELOOP (-ELOOP)
# define UV__EMFILE (-EMFILE)
# define UV__EMSGSIZE (-EMSGSIZE)
# define UV__ENAMETOOLONG (-ENAMETOOLONG)
# define UV__ENETDOWN (-ENETDOWN)
# define UV__ENETUNREACH (-ENETUNREACH)
# define UV__ENFILE (-ENFILE)
# define UV__ENOBUFS (-ENOBUFS)
# define UV__ENODEV (-ENODEV)
# define UV__ENOENT (-ENOENT)
# define UV__ENOMEM (-ENOMEM)
# define UV__ENONET (-ENONET)
# define UV__ENOSPC (-ENOSPC)
# define UV__ENOSYS (-ENOSYS)
# define UV__ENOTCONN (-ENOTCONN)
# define UV__ENOTDIR (-ENOTDIR)
# define UV__ENOTEMPTY (-ENOTEMPTY)
# define UV__ENOTSOCK (-ENOTSOCK)
# define UV__ENOTSUP (-ENOTSUP)
# define UV__EPERM (-EPERM)
# define UV__EPIPE (-EPIPE)
# define UV__EPROTO (-EPROTO)
# define UV__EPROTONOSUPPORT (-EPROTONOSUPPORT)
# define UV__EPROTOTYPE (-EPROTOTYPE)
# define UV__EROFS (-EROFS)
# define UV__ESHUTDOWN (-ESHUTDOWN)
# define UV__ESPIPE (-ESPIPE)
# define UV__ESRCH (-ESRCH)
# define UV__ETIMEDOUT (-ETIMEDOUT)
# define UV__ETXTBSY (-ETXTBSY)
# define UV__EXDEV (-EXDEV)
# define UV__EFBIG (-EFBIG)
# define UV__ENOPROTOOPT (-ENOPROTOOPT)
# define UV__ERANGE (-ERANGE)
# define UV__ENXIO (-ENXIO)
# define UV__EMLINK (-EMLINK)

/* EHOSTDOWN is not visible on BSD-like systems when _POSIX_C_SOURCE is
 * defined. Fortunately, its value is always 64 so it's possible albeit
 * icky to hard-code it.
 */
# define UV__EHOSTDOWN (-EHOSTDOWN)

#endif /* UV_ERRNO_H_ */
