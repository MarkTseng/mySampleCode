/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * mattr.c -- 
 *
 * Practice creating and modifying attributes
 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#include <pthread.h>

#define MIN_REQ_SSIZE 10*8192

void *r1(void *not_used)
{
  int i;

  for (i=0;i<10000;i++) {
    if (i%21 == 0) printf(".");
  } 
  printf("\n");
  return(NULL);
}


extern int
main(void)
{
   int rtn;
   size_t def_stack_size;
   pthread_t thread;
   pthread_attr_t custom_attr;

   pthread_attr_init(&custom_attr);

/*
   pthread_attr_setdetachstate(&custom_attr,
			PTHREAD_CREATE_DETACHED);
*/
   pthread_attr_getstacksize(&custom_attr,
			     &def_stack_size);
   if (def_stack_size < MIN_REQ_SSIZE) {
     pthread_attr_setstacksize(&custom_attr,
			       MIN_REQ_SSIZE);
   }
   pthread_create(&thread,
		&custom_attr,
		r1,
		NULL);

   if ((rtn = pthread_join(thread, NULL)) != 0)
     fprintf(stderr,"pthread_join: %s\n",strerror(rtn)), exit(1);

   return 0;
}
     


