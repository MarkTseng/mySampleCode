#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#include <signal.h>

int main()
{
	clock_t mytime;
	
	printf("_SC_CLK_TCK:%lu\n",sysconf(_SC_CLK_TCK));

	signal(SIGCHLD, SIG_IGN);

	while(1)
	{
		mytime = times(NULL);
		printf("times: %10lx\n",mytime);
		printf("times: %10lx\n",mytime);
		printf("times: %10lx\n",mytime);
		printf("times: %10lx\n",mytime);
		printf("times: %10lx\n",mytime);
		printf("times: %10lx\n",mytime);
		printf("times: %10lx\n",mytime);
		printf("times: %10lx\n",mytime);
		printf("times: %10lx\n",mytime);
		printf("times: %10lx\n",mytime);
		//usleep(10);
	}
	return 0;
}
