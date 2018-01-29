#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void *thread_func(void *param)
{
	cpu_set_t cpuset;

	CPU_ZERO(&cpuset);
	CPU_SET(1, &cpuset); // on proccessor 1;
 
	if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) <0) {
        perror("pthread_setaffinity_np");
    }

	// your code here
	while(1);
}


int main(int argc, char *argv[])
{

    pthread_t my_thread;

    if (pthread_create(&my_thread, NULL, thread_func, NULL) != 0) {
        perror("pthread_create");
    }
    pthread_exit(NULL);
}


