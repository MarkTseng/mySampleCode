/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * process_shared_mutex.c -- 
 *
 * Demonstrating the shared process mutex
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>


#include <pthread.h>

#ifndef _POSIX_THREAD_PROCESS_SHARED
#error This system does not support process shared mutex
#endif

int   shared_mem_id;
int   *shared_mem_ptr;

pthread_mutexattr_t mutex_shared_attr;
pthread_mutex_t	*mptr;

extern int
main(void)
{
  pid_t  child_pid;
  int  status, rtn;

  /* initialize shared memory segment */
  if ((shared_mem_id = shmget(IPC_PRIVATE, 1*sizeof(pthread_mutex_t), 0660)) < 0)
    perror("shmget"), exit(1) ;

  if ((shared_mem_ptr = (int *)shmat(shared_mem_id, (void *)0, 0)) == NULL)
    perror("shmat"), exit(1);

  mptr = (pthread_mutex_t *)shared_mem_ptr;


  if (( rtn = pthread_mutexattr_init(&mutex_shared_attr)) != 0) 
    fprintf(stderr,"pthreas_mutexattr_init: %s",strerror(rtn)),exit(1);

  if (( rtn = pthread_mutexattr_setpshared(&mutex_shared_attr,PTHREAD_PROCESS_SHARED)) != 0)
    fprintf(stderr,"pthread_mutexattr_setpshared %s",strerror(rtn)),exit(1);

  if (( rtn = pthread_mutex_init(mptr, &mutex_shared_attr)) != 0)
    fprintf(stderr,"pthread_mutex_init %s",strerror(rtn)), exit(1);


  if ((child_pid = fork()) == 0) {
    /* first child */
    if ((rtn = pthread_mutex_lock(mptr)) != 0)
      fprintf(stderr,"child:pthread_mutex_lock %s",strerror(rtn)),exit(1);
    sleep(1);
    if ((rtn = pthread_mutex_unlock(mptr)) != 0)
      fprintf(stderr,"child:pthread_unmutex_lock %s",strerror(rtn)),exit(1);
    exit(0);
  } else {

    /* parent */ 
    sleep(1); 
    if ((rtn = pthread_mutex_lock(mptr)) != 0)
      fprintf(stderr,"parent:pthread_mutex_lock %d",strerror(rtn)),exit(1);
    if ((rtn = pthread_mutex_unlock(mptr)) != 0)
      fprintf(stderr,"child:pthread_unmutex_lock %d",strerror(rtn)),exit(1);
    wait(&status);
  }

  return 0;
}
