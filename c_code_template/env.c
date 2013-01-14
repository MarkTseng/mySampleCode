#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	char *env;

	env = getenv("HOME");
	printf("HOME: %s\n",env);
	
	setenv("MYENV_VARIABLE","XDDXDD",1);
	
	env = getenv("MYENV_VARIABLE");
	printf("MYENV_VARIABLE: %s\n",env);
	
	return 0;
}
