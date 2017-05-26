#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>



int main() {
	char *a1 = malloc(45);
	char *a2 = malloc(10);
	char *a3 = malloc(12);
	char *a4 = malloc(41);
	char *a5 = malloc(200);
	char *a6 = malloc(10);
	char *a7 = malloc(1);
	char *a8 = malloc(20);
	
	free(a2);

}
