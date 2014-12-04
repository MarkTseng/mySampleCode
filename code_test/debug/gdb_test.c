#include <stdio.h>
#include <stdlib.h>

int division(int a, int b);

int m;

int main(void)
{
	int i;
	int j;

	printf("vou setar i\n");
	i = 10;

	printf("vou setar j\n");
	j = 1;

	printf ("i = %d, j = %d\n", i, j);
	m = division(i, j);

	printf("m = %d / %d = %d\n", i, j, m);

	return 0;
}

int division(int a, int b)
{
	return a / b;
}


