#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void errexit(char *errMsg){
	printf("\n About to exit: %s", errMsg);
	fflush(stdout);
	exit(1);
}

int main()
{
	int ret;
	int value;
	char fifoName[]="/tmp/isp_cmdq";
	char errMsg[1000];
	FILE *pfp;

	ret = mknod(fifoName, S_IFIFO | 0600, 0); 
	/* 0600 gives read, write permissions to user and none to group and world */
	if(ret < 0){
		sprintf(errMsg,"Unable to create fifo: %s",fifoName);
		//errexit(errMsg);
	}

	while(1) 
	{
	    pfp = fopen(fifoName,"r");
		ret=fscanf(pfp,"%d",&value);
		printf("This is the parent. Received value %d from child on fifo \n", value);
        fflush(pfp);
	}
		fclose(pfp);
		unlink(fifoName); /* Delete the created fifo */
		exit(0);
}
