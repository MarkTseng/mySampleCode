/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * static_init.c -- Demonstrating static initialization of 
 *            mutex and condition variable.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#include <pthread.h>


pthread_mutex_t m1=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t *m2p;

pthread_cond_t c1=PTHREAD_COND_INITIALIZER;

extern int
main(void)
{
  int rtn; 

  if ((rtn = pthread_mutex_lock(&m1)) != 0) 
    fprintf(stderr,"pthread_mutex_lock: %s\n",strerror(rtn)), exit(1);

  if ((rtn = pthread_mutex_unlock(&m1)) != 0) 
    fprintf(stderr,"pthread_mutex_unlock: %s\n",strerror(rtn)), exit(1);

  if ((m2p = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)))!=NULL) {
    if ((rtn = pthread_mutex_init(m2p,NULL)) != 0) 
      fprintf(stderr,"pthread_mutex_unlock: %s\n",strerror(rtn)), exit(1);
  }
  if ((rtn = pthread_cond_signal(&c1)) != 0)
    fprintf(stderr,"pthread_cond_signal: %s\n",strerror(rtn)), exit(1);

  return 0;
}

