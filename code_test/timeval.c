#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>   /* Needed for struct timespec */

int main()
{
	struct timeval current_time1, current_time2;
   	struct timespec req, rem;
	int i=50;

    req.tv_sec = 0; 
    req.tv_nsec = 10; 

	while(i-->0) {
		gettimeofday(&current_time1,0);
		//usleep(1);
   		nanosleep(&req , &rem);
		gettimeofday(&current_time2,0);
		printf("%dus: emulationbegins\n",current_time2.tv_usec - current_time1.tv_usec);
	}

	return 0;
}
