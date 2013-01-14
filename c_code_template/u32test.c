#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/types.h>

int main(int argc, char *argv)
{
	__u32 A;
	__u8 B;

	A = 0x4000;
	B = 0xFF; 
	printf("A=%d\n",A);
	printf("B=%d\n",B);

	return 0;
}
