#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum MyEMU{
	A,
	B,
	C,
	D,
	E,
	Total,
};

int main(int argc, char *argv[])
{
	enum MyEMU enumTYPE;

	enumTYPE = D;

	switch(enumTYPE)
	{
		case A:
			printf("A\n");
			break;
		case B:
			printf("B\n");
			break;
		case C:
			printf("C\n");
			break;
		case D:
			printf("D\n");
			break;
		case E:
			printf("E\n");
			break;
		case Total:
			printf("Total\n");
			break;
	}



	return 0;
}
