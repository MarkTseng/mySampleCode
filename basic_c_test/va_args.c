#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MYPRINT(...)	printf(__VA_ARGS__)

int main(int argc, char *argv[])
{
	MYPRINT("Hello world\n");
	return 0;
}
