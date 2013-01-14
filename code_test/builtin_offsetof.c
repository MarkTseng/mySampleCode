#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

typedef struct MyREG {
	unsigned int A;
	unsigned int B;
	unsigned int C;
	unsigned int D;
	unsigned int E;
	unsigned int F;
	unsigned int G;
	unsigned char H;
	unsigned char I;
	unsigned char I2;
	unsigned char I3;
	unsigned int J;
	unsigned long K;
	unsigned char L;
	unsigned char M;
	unsigned char M1;
	unsigned char M2;
}MyREG;

#define OFFSETOF(type, field)    ((unsigned int) &(((type *) 0)->field))

#define REG0 ((MyREG *)(0)) 
#define REG0_OFFSET(MEM) (unsigned int)(&(MEM))

int main()
{
	printf("REG C offset: %d\n",__builtin_offsetof(MyREG, C));
	printf("REG D offset: %d\n",__builtin_offsetof(MyREG, D));
	printf("REG F offset: %d\n",__builtin_offsetof(MyREG, F));
	printf("REG H offset: %d\n",__builtin_offsetof(MyREG, H));
	printf("REG J offset: %d\n",__builtin_offsetof(MyREG, J));
	printf("REG M offset: %d\n",__builtin_offsetof(MyREG, M));

	printf("REG M offset: %d\n",OFFSETOF(MyREG,M));
	printf("REG M offset: %d\n",offsetof(MyREG,M));
	printf("REG M offset: %d\n",REG0_OFFSET(REG0->M));

	return 0;
}
