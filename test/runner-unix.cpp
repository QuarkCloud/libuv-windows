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

#include "runner-unix.h"
#include "runner.h"

#include <limits.h>
#include <stdint.h> /* uintptr_t */

#include <errno.h>
#include <unistd.h> /* readlink, usleep */
#include <string.h> /* strdup */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <assert.h>

#include <sys/select.h>
#include <sys/time.h>
#include <pthread.h>


/* Do platform-specific initialization. */
int platform_init(int argc, char **argv) {
  /* Disable stdio output buffering. */
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
  signal(SIGPIPE, SIG_IGN);

  if (realpath(argv[0], executable_path) == NULL) {
    perror("realpath");
    return -1;
  }

  return 0;
}


void * thread_routine(void * param)
{
    thread_info_t * thread_info = (thread_info_t *)param ;

    if(thread_info->main != NULL)
        thread_info->main() ;

    return NULL ;
}

int thread_start(char *name, char* part, int (*test_routine)(void) , thread_info_t *p, int is_helper)
{
  FILE* stdout_file;
  char* args[16];
  int n;

  stdout_file = tmpfile();
  if(stdout_file == NULL)
  {
    perror("tmpfile");
    return -1;
  }

  p->terminated = 0;
  p->status = 0;
  p->main = test_routine ;
  p->name = strdup(name);
  p->stdout_file = stdout_file;

  n = 0;

  /* Disable valgrind for helpers, it complains about helpers leaking memory.
   * They're killed after the test and as such never get a chance to clean up.
   */
  args[n++] = executable_path;
  args[n++] = name;
  args[n++] = part;
  args[n++] = NULL;

  ::pthread_create(&p->tid , NULL , thread_routine , p) ;

  return 0;
}

int thread_wait(thread_info_t *vec, int n, int timeout)
{
    for(int idx = 0 ; idx < n ; ++idx)
    {
        thread_info_t& info = vec[idx] ;
        ::pthread_join(info.tid , NULL) ;
    }

    return 0 ;
}

long int thread_output_size(thread_info_t *p)
{
  /* Size of the p->stdout_file */
  struct stat buf;

  int r = fstat(fileno(p->stdout_file), &buf);
  if (r < 0) {
    return -1;
  }

  return (long)buf.st_size;
}

int thread_copy_output(thread_info_t* p, FILE* stream)
{
  char buf[1024];
  int r;

  r = fseek(p->stdout_file, 0, SEEK_SET);
  if (r < 0) {
    perror("fseek");
    return -1;
  }

  /* TODO: what if the line is longer than buf */
  while (fgets(buf, sizeof(buf), p->stdout_file) != NULL)
    print_lines(buf, strlen(buf), stream);

  if (ferror(p->stdout_file)) {
    perror("read");
    return -1;
  }

  return 0;
}

int thread_read_last_line(thread_info_t *p,char * buffer,size_t buffer_len)
{
  char* ptr;

  int r = fseek(p->stdout_file, 0, SEEK_SET);
  if (r < 0) {
    perror("fseek");
    return -1;
  }

  buffer[0] = '\0';

  while (fgets(buffer, buffer_len, p->stdout_file) != NULL)
  {
    for (ptr = buffer; *ptr && *ptr != '\r' && *ptr != '\n'; ptr++);
    *ptr = '\0';
  }

  if (ferror(p->stdout_file)) {
    perror("read");
    buffer[0] = '\0';
    return -1;
  }
  return 0;
}

char* thread_get_name(thread_info_t *p)
{
    return p->name ;
}

int thread_terminate(thread_info_t *p)
{
  return kill(p->tid, SIGTERM);
}


/* Return the exit code of process p. On error, return -1. */
int thread_reap(thread_info_t *p)
{
  if (WIFEXITED(p->status))
  {
    return WEXITSTATUS(p->status);
  } 
  else
  {
    return p->status; /* ? */
  }
}


/* Clean up after terminating process `p` (e.g. free the output buffer etc.). */
void thread_cleanup(thread_info_t *p)
{
  fclose(p->stdout_file);
  free(p->name);
}


/* Move the console cursor one line up and back to the first column. */
void rewind_cursor(void)
{
  fprintf(stderr, "\033[2K\r");
}


/* Pause the calling thread for a number of milliseconds. */
void uv_sleep(int msec) {
  int sec;
  int usec;

  sec = msec / 1000;
  usec = (msec % 1000) * 1000;
  if (sec > 0)
    sleep(sec);
  if (usec > 0)
    usleep(usec);
}
