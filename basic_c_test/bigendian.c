#include <stdio.h>

typedef union
{
		int i;
		char c[4];
}u;

int main()
{
		u temp;
		temp.i = 0x12345678;

		printf("%x\n", temp.i);
		printf("%x %x %x %x\n", temp.c[0], temp.c[1], temp.c[2], temp.c[3]);
		//If you get the output of the second line as "78 56 34 12", the machine is little endian.
		//Instead if the output is "12 34 56 78", it's a big endian machine. 
}
