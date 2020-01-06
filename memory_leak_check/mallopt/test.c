/*******************************************
* File Name : test.c
* Purpose :
* Creation Date : 05-09-2016
* Last Modified : Mon 05 Sep 2016 08:09:42 PM CST
* Created By : Mark Tseng  
**********************************************/

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	char *p;

	if (argc > 1) {
		if (mallopt(M_CHECK_ACTION, atoi(argv[1])) != 1) {
			fprintf(stderr, "mallopt() failed");
			exit(EXIT_FAILURE);
		}
	}

	p = malloc(1000);
	if (p == NULL) {
		fprintf(stderr, "malloc() failed");
		exit(EXIT_FAILURE);
	}

	free(p);
	printf("main(): returned from first free() call\n");

	free(p);
	printf("main(): returned from second free() call\n");

	exit(EXIT_SUCCESS);
}
