/*******************************************
* File Name : malloc_test.c
* Purpose :
* Creation Date : 30-09-2016
* Last Modified : Wed 14 Dec 2016 10:07:25 PM CST
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

static int ddd;
int sss;
int qqq;

int main(int argc, char * argv[])
{
	unsigned long mmSize = 0;
	char *pMemtest;
	if(argc < 2)
		exit(0);
	mmSize = strtol(argv[1], NULL,16);
	printf("mmSize: %ld\n", mmSize);
	void *top_of_heap = sbrk(0);
	pMemtest = malloc(mmSize);
	void *top_of_heap2 = sbrk(0);
	printf("The top of heap went from %p to %p \n", top_of_heap, top_of_heap2);
	free(top_of_heap);
	return 0;
}
