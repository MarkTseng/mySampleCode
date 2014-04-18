#include <stdio.h>
#include <stdlib.h>

int c()
{
	printf("Called C\n");
	return 0;
}

int b()
{
	c();
	printf("Called B\n");
	
	return 0;
}

int a()
{
	b();
	printf("Called A\n");
	return 0;
}

int main()
{
	printf("hello C\n");
	a();
	return 0;
}
