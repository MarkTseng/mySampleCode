/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * mutex_ceiling.c
 */

#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
#ifndef _POSIX_THREAD_PRIO_PROTECT
#error "This system does not support priority ceiling protection in mutex"
#endif
*/

pthread_mutex_t      m1;
pthread_mutexattr_t  mutexattr_prioceiling;

extern int
main(void)
{
  int rtn;
  int mutex_protocol, high_prio;

  /* What is the default protocol on the host? */

  if ((rtn = pthread_mutexattr_init(&mutexattr_prioceiling)) != 0)
    fprintf(stderr,"pthread_mutexattr_init: %s",strerror(rtn)),exit(1);
/*
  if ((rtn = pthread_mutexattr_getprotocol(&mutexattr_prioceiling, 
					   &mutex_protocol)) != 0)
    fprintf(stderr,"pthread_mutexattr_getprotocol: %s",strerror(rtn)),exit(1);

  if (mutex_protocol == PTHREAD_PRIO_PROTECT)
    printf("Default mutex protocol is PTHREAD_PRIO_PROTECT\n");
  else if (mutex_protocol == PTHREAD_PRIO_INHERIT)
    printf("Default mutex protocol is PTHREAD_PRIO_INHERIT\n");
  else if (mutex_protocol == PTHREAD_PRIO_NONE)
    printf("Default mutex protocol is PTHREAD_PRIO_NONE\n");
  else 
    printf("Default mutex protocol is unrecognized: %d\n");
*/
  /* Set this mutex attribute to PROTECT with high priority */  
/* 
  if ((rtn = pthread_mutexattr_setprotocol(&mutexattr_prioceiling, 
					   PTHREAD_PRIO_PROTECT)) != 0)
    fprintf(stderr,"pthread_mutexattr_setprotocol: %s",strerror(rtn)),exit(1);
  if ((high_prio = sched_get_priority_max(SCHED_RR)) < 0)
    fprintf(stderr,"sched_get_priority_max: %s",strerror(rtn)),exit(1);
  if ((rtn = pthread_mutexattr_setprioceiling(&mutexattr_prioceiling, 
					      high_prio)) != 0)
    fprintf(stderr,"pthread_mutexattr_setprioceiling: %s",strerror(rtn)),exit(1);
*/ 
  /* Initialize a mutex with the attribute object */
  if ((rtn = pthread_mutex_init(&m1,&mutexattr_prioceiling)) != 0)
    fprintf(stderr,"pthread_mutexattr_init: %s",strerror(rtn)),exit(1);

  if ((rtn = pthread_mutex_lock(&m1)) != 0)
    fprintf(stderr,"pthread_mutex_lock: %s",strerror(rtn)),exit(1);

  if ((rtn = pthread_mutex_unlock(&m1)) != 0)
    fprintf(stderr,"pthread_mutex_unlock: %s",strerror(rtn)),exit(1);

  return 0;
}


