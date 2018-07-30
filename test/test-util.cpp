
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include "test-util.h"

#include "uv.h"
#include "runner.h"
#include "task.h"


int maybe_run_test(int argc, char **argv)
{
  if (strcmp(argv[1], "--list") == 0) {
    print_tests(stdout);
    return 0;
  }

  if (strcmp(argv[1], "ipc_helper_listen_before_write") == 0) {
    return ipc_helper(0);
  }

  if (strcmp(argv[1], "ipc_helper_listen_after_write") == 0) {
    return ipc_helper(1);
  }

  if (strcmp(argv[1], "ipc_send_recv_helper") == 0) {
    return ipc_send_recv_helper();
  }

  if (strcmp(argv[1], "ipc_helper_tcp_connection") == 0) {
    return ipc_helper_tcp_connection();
  }

  if (strcmp(argv[1], "ipc_helper_bind_twice") == 0) {
    return ipc_helper_bind_twice();
  }

  if (strcmp(argv[1], "stdio_over_pipes_helper") == 0) {
    return stdio_over_pipes_helper();
  }

  if (strcmp(argv[1], "spawn_helper1") == 0) {
    return 1;
  }

  if (strcmp(argv[1], "spawn_helper2") == 0) {
    printf("hello world\n");
    return 1;
  }

  if (strcmp(argv[1], "spawn_helper3") == 0) {
    char buffer[256];
    ASSERT(buffer == fgets(buffer, sizeof(buffer) - 1, stdin));
    buffer[sizeof(buffer) - 1] = '\0';
    fputs(buffer, stdout);
    return 1;
  }

  if (strcmp(argv[1], "spawn_helper4") == 0) {
    /* Never surrender, never return! */
    while (1) uv_sleep(10000);
  }

  if (strcmp(argv[1], "spawn_helper5") == 0) {
    const char out[] = "fourth stdio!\n";
    {
      ssize_t r;

      do
        r = write(3, out, sizeof(out) - 1);
      while (r == -1 && errno == EINTR);

      fsync(3);
    }
    return 1;
  }

  if (strcmp(argv[1], "spawn_helper6") == 0) {
    int r;

    r = fprintf(stdout, "hello world\n");
    ASSERT(r > 0);

    r = fprintf(stderr, "hello errworld\n");
    ASSERT(r > 0);

    return 1;
  }

  if (strcmp(argv[1], "spawn_helper7") == 0) {
    int r;
    char *test;
    /* Test if the test value from the parent is still set */
    test = getenv("ENV_TEST");
    ASSERT(test != NULL);

    r = fprintf(stdout, "%s", test);
    ASSERT(r > 0);

    return 1;
  }

  if (strcmp(argv[1], "spawn_helper8") == 0) {
    int fd;
    ASSERT(sizeof(fd) == read(0, &fd, sizeof(fd)));
    ASSERT(fd > 2);
    ASSERT(-1 == write(fd, "x", 1));

    return 1;
  }

  if (strcmp(argv[1], "spawn_helper9") == 0) {
    return spawn_stdin_stdout();
  }

  if (strcmp(argv[1], "spawn_helper_setuid_setgid") == 0) {
    uv_uid_t uid = atoi(argv[2]);
    uv_gid_t gid = atoi(argv[3]);

    ASSERT(uid == getuid());
    ASSERT(gid == getgid());

    return 1;
  }

  return run_test(argv[1], 0, 1);
}
