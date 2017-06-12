#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "thpool.h"

thpool_t* workQueue;
thpool_t* workQueueHigh;
int quitJob = 0;

void taskthread(void* new) {
    int i = (int)new;
    printf("[%s] called %d\n",__func__, i );
}

void taskthreadHigh(void* new) {
    int i = (int)new;
    printf("[%s] called %d\n",__func__, i );
}

void sighandler(int signum)
{
   printf("Caught signal %d, coming out...\n", signum);
   quitJob = 1;
}

int main(int argv, char *argc[])
{
    int i;
    workQueue = thpool_init(1, SCHED_RR, 20, "JobQueue");
    workQueueHigh = thpool_init(1, SCHED_RR, 50, "HiJobQueue");

    signal(SIGINT, sighandler);
    if (!workQueue) {
        fprintf(stderr, "could not initialize workQueue\n");
        exit(1);
    }

    if (!workQueueHigh) {
        fprintf(stderr, "could not initialize workQueueHigh\n");
        exit(1);
    }

    for(i=0;i<1000;i++)
    {
        if(i%20==0)
        { 
            if (!thpool_add_job(workQueueHigh, (void*) taskthreadHigh, (void*)i)) {
                fprintf(stderr, "audio_play: Could not add file reading job for \n");
            }
        }else{
            if (!thpool_add_job(workQueue, (void*) taskthread, (void*)i)) {
                fprintf(stderr, "audio_play: Could not add file reading job for \n");
            }
        }
    }

    while(!quitJob)
    {
        if(!thpool_jobqueue_is_empty(workQueue))
        {
            printf("queue size = %d\n", thpool_jobqueue_size(workQueue));
        }

        if(!thpool_jobqueue_is_empty(workQueueHigh))
        {
            printf("queueHigh size = %d\n", thpool_jobqueue_size(workQueueHigh));
        }
        if(thpool_jobqueue_is_empty(workQueueHigh) && thpool_jobqueue_is_empty(workQueue))
            break;

        sleep(1);
    }
    if (workQueue) thpool_destroy(workQueue);
    printf("destory workQueue\n");
    if (workQueueHigh) thpool_destroy(workQueueHigh);
    printf("destory workQueueHight\n");
    return 0;
}
