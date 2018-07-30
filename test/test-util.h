#ifndef __LIBUV_TEST_UTIL_H
#define __LIBUV_TEST_UTIL_H 1

#ifdef __cplusplus
extern "C" {
#endif

int ipc_helper(int listen_after_write);
int ipc_helper_tcp_connection(void);
int ipc_send_recv_helper(void);
int ipc_helper_bind_twice(void);
int stdio_over_pipes_helper(void);
int spawn_stdin_stdout(void);

int maybe_run_test(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif  /** __LIBUV_TEST_UTIL_H */
