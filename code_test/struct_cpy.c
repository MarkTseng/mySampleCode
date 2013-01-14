#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct ABC{
	int a;
	int b;
	int c;
}ABC_st;

int ArrayA[5]={1,2,3,4,5};
int ArrayB[5];

void ArrayParam(int *TmpArray)
{
	printf("0: %d, 1: %d, 2: %d, 3: %d, 4: %d\n",*(TmpArray+0),
												*(TmpArray+1),
												*(TmpArray+2),
												*(TmpArray+3),
												*(TmpArray+4));
}

int main(int argc, char *argv[])
{
	ABC_st srcABC = {1,2,3};
	ABC_st dstABC = srcABC;
	memset(ArrayB,0,sizeof(ArrayB)); 
	memcpy(ArrayB,ArrayA,sizeof(ArrayA));

	printf("a: %d, b: %d, c: %d\n",dstABC.a,dstABC.b,dstABC.c);
	printf("0: %d, 1: %d, 2: %d, 3: %d, 4: %d\n",ArrayB[0],ArrayB[1],ArrayB[2],ArrayB[3],ArrayB[4]);

	ArrayParam(ArrayB);
	return 0;
}
