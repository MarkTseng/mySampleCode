/*******************************************
* File Name : strchr.c
* Purpose :
* Creation Date : 21-09-2016
* Last Modified : Wed 21 Sep 2016 03:54:42 PM CST
* Created By : Mark Tseng  
**********************************************/

#include <stdio.h>  
#include <unistd.h>  
#include <sys/mman.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

int main(int argc, char * argv[])
{
	char str[] = "rc/rv_ms_isp_hal.c:4800";
	char * pch;
	pch=strrchr(str,'/');
	if (pch != NULL)
		printf ("Last occurence of 's' found at %d, %s \n",pch-str+1, pch + 1);
	return 0;
}
