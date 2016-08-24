#include <stdio.h>
#include <stdlib.h>

#define BIT0	0x01
#define BIT1	0x02
#define BIT2	0x04
#define BIT3	0x08
#define BIT4	0x10
#define BIT5	0x20
#define BIT6	0x40
#define BIT7	0x80

int main()
{
	int A = 0; 
	
	A |= BIT7;

	if(A & BIT7)
		printf("Bit 7 is set\n");
	
	printf("A=%x\n",A);
	
	A &= ~(BIT7);
	printf("A=%x\n",A);
	A |= ~(BIT7);
	printf("A=%x\n",A);
	A ^= ~(BIT7);
	printf("A=%x\n",A);

}
