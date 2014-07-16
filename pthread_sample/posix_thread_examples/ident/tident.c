/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * ident.c
 *
 * Demonstrate use of pthread_self() and pthread_equal()
 */

#include <stdio.h>
#include <pthread.h>

pthread_t io_thread;

void * io_routine(void *notused)
{
  pthread_t thread;

  thread = pthread_self();
  if (pthread_equal(io_thread, thread)) 
    printf("hey it is me!\n");
  else 
    printf("thats\' not me!\n");
	
  return(NULL);
}

main(void)
{
  extern pthread_t io_thread;

  pthread_create(&io_thread,
		 NULL,
		 io_routine,
		 NULL);

  pthread_join(io_thread, NULL);
}



