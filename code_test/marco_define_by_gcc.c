// gcc -DHASHCODE=\"`git rev-list HEAD -1`\" marco_define_by_gcc.c -o marco_define_by_gcc
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	printf("git version: %s \n", HASHCODE);
	return 0;
}
