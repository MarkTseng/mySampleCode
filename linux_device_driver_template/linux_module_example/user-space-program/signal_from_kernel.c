#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#define SIG_TEST 44 /* we define our own signal, hard coded since SIGRTMIN is different in user and in kernel space */ 

void receiveData(int n, siginfo_t *info, void *unused)
{
	printf("received value %i\n", info->si_int);
}

int main(int argc, char **argv)
{
	/* setup the signal handler for SIG_TEST 
 	 * SA_SIGINFO -> we want the signal handler function with 3 arguments
 	 */
	struct sigaction sig;
	sig.sa_sigaction = receiveData;
	sig.sa_flags = SA_SIGINFO;
	sigaction(SIG_TEST, &sig, NULL);

	while(1);
	
	return 0;
}
