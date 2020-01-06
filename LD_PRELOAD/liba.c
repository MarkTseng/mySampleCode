/*******************************************
* File Name : liba.c
* Purpose :
* Creation Date : 01-09-2016
* Last Modified : Thu 01 Sep 2016 10:58:56 AM CST
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

int TEST_A()
{
	printf("TEST_A\n");	
	free(malloc(1024));
	printf("TEST_A done\n");	
	return 0;
}
