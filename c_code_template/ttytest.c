#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/types.h>
#include <unistd.h>

int main(int argc, char *argv)
{
	if(isatty(fileno(stdout)))
	{
		printf("you are using terminal\n");
	}

	return 0;
}
