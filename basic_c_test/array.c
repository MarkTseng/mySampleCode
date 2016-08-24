#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	int A[5] = {0,1,2,3,4};
	int *pA = (int *)&A;
	pA++;
	printf("A[1]: %d\n",*pA);
	return 0;
}
