#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	int A[2][10] = {{0,1,2,3,4},{10,11,12,13,14,15,16,17,18,19}};

	int (*pA)[10] = &A[0];

	printf("pA:%d\n",*pA[0]);
	printf("pA++:%d\n",*(pA+1)[0]);
	printf("A[1][5]:%d\n",A[1][5]);
	printf("A[0][5]:%d\n",A[0][5]);
	return 0;
}
/*
pA:0
pA++:10
A[1][5]:15
A[0][5]:0
*/

