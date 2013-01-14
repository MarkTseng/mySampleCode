#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void A();
void B();
void C();
void D();

void A()
{
	B();
	printf("call %s\n",__func__);
}

void B()
{
	C();
	printf("call %s\n",__func__);
}

void C()
{
	D();
	printf("call %s\n",__func__);
}

void D()
{
	printf("call %s\n",__func__);
}

int main(int argc, char *argv[])
{

	A();
	printf("Hello\n");

	return 0;
}
