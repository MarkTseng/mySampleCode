/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * sched.c
 *
 * Examples in setting scheduling policy
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <sys/types.h>

#include <pthread.h>

#define NUM_THREADS           3
#define LOOP_COUNT          100000
#define TOTAL_END_COUNT    1000000

#define DEFAULT                 0
#define TIME_SLICE              1
#define FIFO                    2
#define PRIOR                   3
#define PTIME_SLICE             4
#define PFIFO                   5
#define INHERIT                 6

/*
 * Thread initialization 
 */
int threads_ready_count=0;
pthread_cond_t    threads_ready=PTHREAD_COND_INITIALIZER; 
pthread_mutex_t   threads_ready_lock=PTHREAD_MUTEX_INITIALIZER; 

/*
 * Thread progress info
 */
int               total_count=0;
pthread_mutex_t   total_count_lock=PTHREAD_MUTEX_INITIALIZER;
int               thread_counts[NUM_THREADS];


void print_count(int id, int i)
{
  switch(id) {
  case 0:
    printf("print_count()\tthread %d \t%4d\r",id, i++);
    break;
  case 1:
    printf("print_count()\tthread %d \t\t%4d\r",id, i++);
    break;
  case 2:
    printf("print_count()\tthread %d \t\t\t%4d\r",id, i++);
    break;
  case 3:
    printf("print_count()\tthread %d \t\t\t\t%4d\r",id, i++);
    break;
  case 4:
    printf("print_count()\tthread %d \t\t\t\t\t%4d\r",id, i++);
    break;
  case 5:
    printf("print_count()\tthread %d \t\t\t\t\t\t%4d\r",id, i++);
    break;
  default:
    break;
  }
}
void *work_routine(void *arg)
{
  int *my_id=(int *)arg;
  int i=0, not_done=1;
  
  /*
   * The worker threads all wait to begin "together"
   */
  printf("\nwork_routine()\tthread %d\tI'm Alive\n", *my_id);

  pthread_mutex_lock(&threads_ready_lock);
  threads_ready_count++;
  if (threads_ready_count == NUM_THREADS) {
    /* I was the last thread to become ready.  Tell the rest. */
    pthread_cond_broadcast(&threads_ready); 
  }
  else {
    /* At least one thread isn't ready.  Wait. */
    while (threads_ready_count != NUM_THREADS) {
      pthread_cond_wait(&threads_ready, &threads_ready_lock);
    }
  }
  pthread_mutex_unlock(&threads_ready_lock);
  
  /*
   * Enter work loop
   */
  printf("\nwork_routine()\tthread %d\tI'm Proceeding\n", *my_id);

  while(not_done) {
    
    for (i=0;i < LOOP_COUNT;i++) {
      print_count(*my_id, i++);
    }
    
    thread_counts[*my_id]+=LOOP_COUNT;
    
    /* Check if done */
    pthread_mutex_lock(&total_count_lock);
    if (total_count < TOTAL_END_COUNT)
      total_count+=LOOP_COUNT;
    else
      not_done=0;
    printf("\nwork_routine()\tthread %d\tmy count %d, total_count %d\n",
	   *my_id, thread_counts[*my_id], total_count);
	   
    pthread_mutex_unlock(&total_count_lock);
        
  }
  printf("\nworkroutine()\tthread %d\tI'm done\n", *my_id);

  return(NULL);
}

extern int
main(int argc, char **argv)
{
  int       rtn, t, i, *id_arg, prio_min, prio_max;
  pthread_t threads[NUM_THREADS];
  int       sched = DEFAULT;
  pthread_attr_t pthread_custom_attr;
  struct sched_param priority_param;


  if ((argc == 2) && (argv[1][0] == 'd')) {
    sched = DEFAULT;
    printf("main()\t\t\t\tTesting Default Scheduling\n");
  }
  if ((argc == 2) && (argv[1][0] == 'r'))  {
    sched = TIME_SLICE;
    printf("main()\t\t\t\tTesting Round-Robin Scheduling\n");
  }
  if ((argc == 2) && (argv[1][0] == 'f')) {
    sched = FIFO;
    printf("main()\t\t\t\tTesting FIFO Scheduling\n");
  }
  if ((argc == 2) && (argv[1][0] == 'p')) {
    sched = PFIFO;
    printf("main()\t\t\t\tTesting FIFO/Priority Scheduling\n");
  }
  if ((argc == 2) && (argv[1][0] == 't'))  {
    sched = PTIME_SLICE;
    printf("main()\t\t\t\tTesting Round-Robin/Priority Scheduling\n");
  }
  if ((argc == 2) && (argv[1][0] == 'i'))  {
    sched = INHERIT;
    printf("main()\t\t\t\tTesting Inherited FIFO Scheduling\n");
  }
  if (argc != 2) {
    printf("Argument required: d=default,r=roundrobin,t=roundrobin/priority,f=fifo,i=inherit,p=fifo/priority\n");
    exit(1);
  }

  id_arg = (int *)malloc(NUM_THREADS*sizeof(int));

  if (sched != DEFAULT) {
    printf("main() creating a thread with non-default scheduling attributes may\n");
    printf("      require running as root on some platforms.\n");
  }
  
  /* spawn the threads */
  for (i = 0; i < NUM_THREADS; i++) {
    
    id_arg[i] = i;
    thread_counts[i] = 0;

    /* set sceduling attributes */

    if (sched == DEFAULT) {
      if (pthread_create(&(threads[i]), 
		     NULL,
		     work_routine,
		     (void *) &(id_arg[i])) !=0)
	perror("main() pthread create with NULL attr obj failed"),exit(1);

    } else {

      /*
       * Just for the fun of it, let's up our priority, and 
       * run with policy FIFO
       */

      priority_param.sched_priority = sched_get_priority_max(SCHED_FIFO);
      pthread_setschedparam(pthread_self(), SCHED_FIFO, &priority_param);

      if (sched == INHERIT) {
	pthread_attr_init(&pthread_custom_attr);
	pthread_attr_setinheritsched(&pthread_custom_attr, 
				     PTHREAD_INHERIT_SCHED);
      } else {
	/*
	 * Now, for the rest of the threads
	 */
	pthread_attr_init(&pthread_custom_attr);
	pthread_attr_setinheritsched(&pthread_custom_attr, PTHREAD_EXPLICIT_SCHED);
	if (sched == FIFO) {
	  pthread_attr_setschedpolicy(&pthread_custom_attr, SCHED_FIFO);
	  prio_min = sched_get_priority_max(SCHED_FIFO); /* PRI_FIFO_MIN;*/
	  prio_max = prio_min; /* force to all same priority */
	}
	if (sched == TIME_SLICE) {
	  pthread_attr_setschedpolicy(&pthread_custom_attr, SCHED_RR);
	  prio_min = sched_get_priority_max(SCHED_RR); /* PRI_RR_MIN; */
	  prio_max = prio_min; /* force to all same priority */
	}
	if (sched == PFIFO) {
	  pthread_attr_setschedpolicy(&pthread_custom_attr, SCHED_FIFO);
	  prio_max = sched_get_priority_max(SCHED_FIFO); /* PRI_FIFO_MAX; */
	  prio_min = sched_get_priority_min(SCHED_FIFO); /* PRI_FIFO_MIN; */
	}
	if (sched == PTIME_SLICE) {
	  pthread_attr_setschedpolicy(&pthread_custom_attr, SCHED_RR);
	  prio_max = sched_get_priority_max(SCHED_RR); /* PRI_RR_MAX; */
	  prio_min = sched_get_priority_min(SCHED_RR); /* PRI_RR_MIN; */
	}
	priority_param.sched_priority = prio_min + 
	  (i)*(prio_max - prio_min)/(NUM_THREADS-1);
	printf("main()\t\t\t\tsetting priority(%d-%d) of thread %d to %d\n", 
	       prio_min, prio_max, i, priority_param.sched_priority);
	if (pthread_attr_setschedparam(&pthread_custom_attr, 
				       &priority_param)!=0)
	  fprintf(stderr,"pthread_attr_setschedparam failed\n");
      } /* else not inherit */

     
    if (pthread_create(&(threads[i]), 
		   &pthread_custom_attr,
		   work_routine,
		   (void *) &(id_arg[i])) !=0)
	perror("main() pthread create with attr obj failed"),exit(1);


    } /* else not default */
  } /* For Num Threads */ 

  printf("\nmain()\t\t\t\t%d threads created. Main running fifo at max\n", 
	 NUM_THREADS);

  /* wait until all threads have finished */
  for (i = 0; i < NUM_THREADS; i++) {
    if (pthread_join(threads[i], NULL) !=0)
	perror("main() pthread_join failed"),exit(1);
    printf("\nmain()\t\t\t\tjoined to thread %d \n", i);
  }
  
  printf("main()\t\t\t\tall %d threads have finished. \n", NUM_THREADS);

  /* Process results */
  printf("main()\t\t\t\ttotal_count %d\n", total_count);
  for (i = 0; i < NUM_THREADS; i++) {
    if (total_count != 0) {
      printf("main()\t\t\t\tthread %d, count %d percent %f\n", i, thread_counts[i],
	     (float)thread_counts[i]/(float)total_count);
    }
  }

  return 0;
}
