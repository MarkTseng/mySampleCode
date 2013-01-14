#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <unistd.h>

void handler(int signo)
{
	printf("timeout by %d\n",signo);
	// reset timer
	alarm(5);
}

int main(int argc, char *argv[])
{
	int i;
	signal(SIGALRM,handler);
	alarm(5);
	for(i=1;i<100;i++)
	{
		printf("sleep %d ...\n",i);
		sleep(1);
	}

	return 0;
}
