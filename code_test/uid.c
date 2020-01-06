#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
	uid_t myuid, myeuid;

	setreuid(1000, 1000);
	setresuid(1000,1000,1000);
	while(1)
	{	
		myuid = getuid();
		myeuid = geteuid();
		printf("uid: %d, euid: %d\n",myuid, myeuid);
		sleep(1);
	}
	return 0;
}
