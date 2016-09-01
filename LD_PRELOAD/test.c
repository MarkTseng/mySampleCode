/*******************************************
* File Name : test.c
* Purpose :
* Creation Date : 01-09-2016
* Last Modified : Thu 01 Sep 2016 09:13:15 AM CST
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
	printf("malloc addr: %p\n", malloc);
	printf("calloc addr: %p\n", calloc);
	printf("free addr: %p\n", free);
	printf("main addr: %p\n", main);
	free(malloc(1024));
    write(0, "Hello, Kernel!\n", 15);
    printf("Hello, World!\n");
	return 0;
}
