/*******************************************
* File Name : test_efence.c
* Purpose :
* Creation Date : 02-09-2016
* Last Modified : Fri 02 Sep 2016 10:12:35 AM CST
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
	char *arr;    int i;
	arr = (char *)malloc(sizeof(char)*5);
	strcpy(arr,"amee is my name");

	return 0;
}


