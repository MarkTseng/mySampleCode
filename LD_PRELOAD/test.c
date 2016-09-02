/*******************************************
* File Name : test.c
* Purpose :
* Creation Date : 01-09-2016
* Last Modified : Thu 01 Sep 2016 03:15:16 PM CST
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
#include "liba.h"

int main(int argc, char * argv[])
{
	printf("malloc addr: %p\n", malloc);
	printf("calloc addr: %p\n", calloc);
	printf("free addr: %p\n", free);
	printf("main addr: %p\n", main);
	free(calloc(1,1024));
    write(0, "Hello, Kernel!\n", 15);
    printf("Hello, World!\n");
	TEST_A();
	return 0;
}
