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

#ifndef TASK_H_
#define TASK_H_

#include "uv.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

# include <stdint.h>

# include <sys/time.h>
# include <sys/resource.h>  /* setrlimit() */

#define TEST_PORT 9123
#define TEST_PORT_2 9124

# define TEST_PIPENAME "/tmp/uv-test-sock"
# define TEST_PIPENAME_2 "/tmp/uv-test-sock2"
# define TEST_PIPENAME_3 "/tmp/uv-test-sock3"


#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define container_of(ptr, type, member) \
  ((type *) ((char *) (ptr) - offsetof(type, member)))

typedef enum {
  TCP = 0,
  UDP,
  PIPE
} stream_type;

/* Die with fatal error. */
#define FATAL(msg)                                        \
  do {                                                    \
    fprintf(stderr,                                       \
            "Fatal error in %s on line %d: %s\n",         \
            __FILE__,                                     \
            __LINE__,                                     \
            msg);                                         \
    fflush(stderr);                                       \
    abort();                                              \
  } while (0)

/* Have our own assert, so we are sure it does not get optimized away in
 * a release build.
 */
#define ASSERT(expr)                                      \
 do {                                                     \
  if (!(expr)) {                                          \
    fprintf(stderr,                                       \
            "Assertion failed in %s on line %d: %s\n",    \
            __FILE__,                                     \
            __LINE__,                                     \
            #expr);                                       \
    abort();                                              \
  }                                                       \
 } while (0)

/* This macro cleans up the main loop. This is used to avoid valgrind
 * warnings about memory being "leaked" by the main event loop.
 */
#define MAKE_VALGRIND_HAPPY()                       \
  do {                                              \
    close_loop(uv_default_loop());                  \
    ASSERT(0 == uv_loop_close(uv_default_loop()));  \
  } while (0)

/* Just sugar for wrapping the main() for a task or helper. */
#define TEST_IMPL(name)                                                       \
  int run_test_##name(void);                                                  \
  int run_test_##name(void)

#define BENCHMARK_IMPL(name)                                                  \
  int run_benchmark_##name(void);                                             \
  int run_benchmark_##name(void)

#define HELPER_IMPL(name)                                                     \
  int run_helper_##name(void);                                                \
  int run_helper_##name(void)

/* Pause the calling thread for a number of milliseconds. */
void uv_sleep(int msec);

/* Format big numbers nicely. WARNING: leaks memory. */
const char* fmt(double d);

/* Reserved test exit codes. */
enum test_status {
  TEST_OK = 0,
  TEST_SKIP
};

#define RETURN_OK()                                                           \
  do {                                                                        \
    return TEST_OK;                                                           \
  } while (0)

#define RETURN_SKIP(explanation)                                              \
  do {                                                                        \
    fprintf(stderr, "%s\n", explanation);                                     \
    fflush(stderr);                                                           \
    return TEST_SKIP;                                                         \
  } while (0)

# define TEST_FILE_LIMIT(num)                                                 \
    do {                                                                      \
      struct rlimit lim;                                                      \
      lim.rlim_cur = (num);                                                   \
      lim.rlim_max = lim.rlim_cur;                                            \
      if (setrlimit(RLIMIT_NOFILE, &lim))                                     \
        RETURN_SKIP("File descriptor limit too low.");                        \
    } while (0)


/* Fully close a loop */
static void close_walk_cb(uv_handle_t* handle, void* arg)
{
  if (!uv_is_closing(handle))
    uv_close(handle, NULL);
}

static void close_loop(uv_loop_t* loop)
{
  uv_walk(loop, close_walk_cb, NULL);
  uv_run(loop, UV_RUN_DEFAULT);
}

static int can_ipv6(void) 
{
  uv_interface_address_t* addr;
  int supported;
  int count;
  int i;

  if (uv_interface_addresses(&addr, &count))
    return 0;  /* Assume no IPv6 support on failure. */

  supported = 0;
  for (i = 0; supported == 0 && i < count; i += 1)
    supported = (AF_INET6 == addr[i].address.address6.sin6_family);

  uv_free_interface_addresses(addr, count);
  return supported;
}

#endif /* TASK_H_ */
