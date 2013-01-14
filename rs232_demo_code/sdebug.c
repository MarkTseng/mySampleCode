#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

#include "sdebug_api.h"

int main(int argc, char *argv[]) 
{
	int opt; 
	int Daemon_mode;
	struct SerialPortConfigProfile pTTYConfig;

	memset(&pTTYConfig,0,sizeof(struct SerialPortConfigProfile));

	// parser option setting
	while((opt = getopt(argc, argv, "n:dh")) != -1) { 
		switch(opt) { 
			case 'd': 
				Daemon_mode = 1;
				break;
			case 'h':
				SDEBUG_Usage();
				break; 
			case 'n': 
				if(strlen(optarg) == 0)
				{
					exit(1);
				}else{
					printf("tty name: %s\n", optarg); 
					strcpy(pTTYConfig.ttyName,optarg);
				}
				break; 
			case ':': 
				printf("option needs a value\n"); 
				break; 
			case '?': 
				SDEBUG_Usage();
				exit(1);
				break; 
		} 
	} 

	// default tty name is /dev/ttyS0
	if(strlen(pTTYConfig.ttyName) == 0)
	{
		strcpy(pTTYConfig.ttyName,TTYNAME);
	}

	if( SDEBUG_Init(&pTTYConfig) == -1)
	{	
		perror("Initial fail\n");
		exit(1);
	}

	// open device
	if(SDEBUG_OpenDevice(&pTTYConfig) == -1)
	{
		perror("open device fail\n");
		exit(1);	
	}

	// get tty config 
	if(SDEBUG_GetTTYConfig(&pTTYConfig) == -1)
	{
		perror("get tty config fail\n");
		exit(1);
	}
	
	// set tty config 
	if(SDEBUG_SetTTYConfig(&pTTYConfig) == -1)
	{
		perror("set tty config fail\n");
		exit(1);
	}

	// uio device open
	SDEBUG_UioOpen(&pTTYConfig);

	// Sdebug CMD parser
	while(1) {

		if(SDEBUG_ReadRequestCMD(&pTTYConfig, pTTYConfig.ttyFD) == -1)
			break;

		if(SDEBUG_CMDParser(&pTTYConfig) == -1)
		{
			printf("cmd parser fail\n");
		} 
	
	}

	// restore ttyS0 for console
	SDEBUG_RestoreTTYSetting(&pTTYConfig);

	// close device
	if(SDEBUG_CloseDevice(&pTTYConfig) == -1)
	{
		perror("open device fail\n");
		exit(1);	
	}

	// uio device close
	SDEBUG_UioClose(&pTTYConfig);

	// release memory
	SDEBUG_Release(&pTTYConfig);
	return 0;
}
