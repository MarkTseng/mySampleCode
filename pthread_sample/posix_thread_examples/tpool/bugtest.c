/********************************************************
 * 
 * Program to test condition found by Andrew Worsley
 * of stavation in add_work when two threads exiting.
 *
 ********************************************************
 */
#include <pthread.h>
#include "tpool.h"

#define NUM_WORKERS	2
#define SIZE_QUEUE	2


char *s1[20]={  "STRING 0",
		"STRING 1",
                "STRING 2",
                "STRING 3",
                "STRING 4",
                "STRING 5",
                "STRING 6",
                "STRING 7",
                "STRING 8",
                "STRING 9",
		"STRING 10",
                "STRING 11",
                "STRING 12",
                "STRING 13",
                "STRING 14",
                "STRING 15",
                "STRING 16",
                "STRING 17",
                "STRING 18",
                "STRING 19"};

pthread_t thread1, thread2;
int thread1_id=1, thread2_id=2;
tpool_t test_pool;

void r1(void * printstring)
{
   int i, x;

   printf("%s START\n", (char *)printstring);

   for (i = 0; i < 1000000; i++)  {
	x = x +i;
   }

   printf("%s DONE\n", (char *)printstring);
}


void *add_while_full(void *arg)
{
   int *id = (int *)arg;

   printf("add_while_full %d: adding work %d \n",*id, (NUM_WORKERS*2) + *id);
   tpool_add_work(test_pool, r1, s1[(NUM_WORKERS*2) + *id]);
   printf("add_while_full %d: work added, returning(exiting)\n",*id); 
   return(NULL);
}

extern int
main(void)
{
   extern char *s1[];

   int i;  

   tpool_init(&test_pool, NUM_WORKERS, NUM_WORKERS, 0);

   /* load the queue with NUM_WORKER * 2 jobs to keep them all
      busy plus fill queue */
   for (i = 0; i < (NUM_WORKERS*2); i++) {
	printf("main: adding work item %i to queue\n",i);
	tpool_add_work(test_pool, r1, s1[i]);
   }

   /* now create two threads to add two more jobs */
   printf("main: creating first adding thread\n");
   pthread_create(&thread1,
		  NULL,
		  add_while_full,
		  (void *)&thread1_id);
   printf("main: creating second adding thread\n");
   pthread_create(&thread2,
		  NULL,
		  add_while_full,
		  (void *)&thread2_id);
   printf("main: waiting for the smoke to clear\n");
   pthread_join(thread1,NULL);
   printf("main: joined to thread1\n");
   pthread_join(thread2,NULL);
   printf("main: joined to thread2\n");

   tpool_destroy(test_pool, 1);
   printf("main: thread pool cleaned up, exiting\n");
}  
