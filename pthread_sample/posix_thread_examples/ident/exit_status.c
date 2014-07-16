/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * exit_status.c
 *
 * Simple code to demonstrate thread exit status
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>

pthread_t thread;

static int arg;
static const int real_bad_error = -12;
static const int normal_error = -10;
static const int success = 1;

void * routine_x(void *arg_in)
{
  int *arg = (int *)arg_in;

  if (*arg == 1) {
    pthread_exit((void *)&real_bad_error);

  } else if (*arg == 2) {
    return ((void *)&normal_error);

  } else {
    return ((void *)&success);
  }
}

extern int
main(int argc, char **argv)
{
  extern int arg;
  pthread_t thread;
  void *statusp;

  if (argc < 2)
    printf("usage: %s [1-3]\n", argv[0]), exit(1);

  arg = atoi(argv[1]);

  pthread_create(&thread,
		 NULL,
		 routine_x ,
		 &arg);

  pthread_join(thread, &statusp);
  if (statusp == PTHREAD_CANCELED) {
    printf("Thread was canceled.\n");
  } else {
    printf("Thread completed and exit status is %ld.\n", *(int *)statusp);
  }

  return 0;
}



