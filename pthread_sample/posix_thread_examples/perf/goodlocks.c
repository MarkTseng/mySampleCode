/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * goodlocks.c
 *
 * Showing better choice for lock placement.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <pthread.h>

#define THREADS_MAX      5
#define LOCAL_COUNT_MAX  1000
#define THRESHOLD        1


pthread_mutex_t count_lock=PTHREAD_MUTEX_INITIALIZER;
int count=0;

void r1(char *fname, int x, char **bufp)
{
   double temp;
   int local_count=0;
   int fd;

   for (local_count = 0; local_count < LOCAL_COUNT_MAX; local_count++) {
      temp = sqrt(x);
      fd = open(fname,O_CREAT | O_RDWR, 0666);     
     
      pthread_mutex_lock(&count_lock);
      count++; 
      pthread_mutex_unlock(&count_lock);

      *bufp = (char *)malloc(256);
      sprintf(*bufp, "%e is the square root of %d", temp, x);
      write(fd, *bufp, strlen(*bufp)); 
      close(fd); 
      free(*bufp);
   }
}


void r2(char *fname, int x, char **bufp)
{
   double temp;
   int i, reads;
   int start=0, end=LOCAL_COUNT_MAX;
   int fd;
 
   for (i = start; i < end; i++) {
         fd = open(fname,O_CREAT | O_RDWR, 0666);
         pthread_mutex_lock(&count_lock);
         x = x + count;
         temp = sqrt(x);
         if (temp == THRESHOLD)
            count++;
         pthread_mutex_unlock(&count_lock);
	 *bufp = (char *)malloc(256);
         read(fd, *bufp, 24);
         free(*bufp);
         close(fd);
   }
}

void *base(void *not_used)
{
  int x = 10;
  char *buf;
  char fname[22];

  sprintf(fname, ".%lX", (long) pthread_self());
/*
  r1(fname, x, &buf);
*/
  r2(fname, x, &buf);
  unlink(fname); 

  return(NULL);
}
  
extern int
main(void)
{
   pthread_t threads[THREADS_MAX];
   int i;

   for (i = 0; i < THREADS_MAX; i++) {
      pthread_create(&(threads[i]), NULL, base, NULL);
   }

   for (i = 0; i < THREADS_MAX; i++) {
      pthread_join(threads[i], NULL);
   }
   return 0;
}
