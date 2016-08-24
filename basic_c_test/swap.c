#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void swap(int *a, int *b)
{
	int tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}

int main()
{
	int var1=10,var2=20;

	printf("var1=%d, var2=%d\n",var1,var2);
	swap(&var1,&var2);
	printf("var1=%d, var2=%d\n",var1,var2);
	
	return 0;
}
