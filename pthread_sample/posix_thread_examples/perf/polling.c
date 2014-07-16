/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * polling.c
 * 
 * Showing polling of condition versus proper condition
 * variable use.
 */

#include <pthread.h> 

pthread_mutex_t db_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  db_inited=PTHREAD_COND_INITIALIZER;

int db_initialized=0;

void r1(void)
{
   pthread_mutex_lock(&db_lock);
   while(!db_initialized) {
      pthread_mutex_unlock(&db_lock);
      sleep(1);
      pthread_mutex_lock(&db_lock);
   }
   pthread_mutex_unlock(&db_lock);   
   /* go do something */
}

void r2(void)
{
   pthread_mutex_lock(&db_lock);
   while(!db_initialized)
      pthread_cond_wait(&db_inited, &db_lock);
   pthread_mutex_unlock(&db_lock);
   /* go do something */
}

int
main(void)
{


}
