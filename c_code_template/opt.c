/*
 * ./opt -i -lr 'hi there' -f fred.c -q
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{

	int opt; 

	while((opt = getopt(argc, argv, "if:lr")) != -1) { 
		switch(opt) { 
			case 'i': 
			case 'l': 
			case 'r': 
				printf("option: %c\n", opt); 
				break; 
			case 'f': 
				printf("filename: %s\n", optarg); 
				break; 
			case ':': 
				printf("option needs a value\n"); 
				break; 
			case '?': 
				printf("unknown option: %c\n", optopt); 
				break; 
		} 
	} 
	for(; optind < argc; optind++) 
		printf("argument: %s\n", argv[optind]); 
	exit(0); 

	return 0;
}
