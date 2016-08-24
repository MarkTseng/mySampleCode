#include <stdio.h>

void test2()
{
	printf("test fp:%x\n",__builtin_frame_address(0));	
	printf("test fp:%x\n",__builtin_return_address(0));	
}

void test()
{

	printf("function address = 0x%p\n", test);
	test2();
}

int main()
{
	test();
}

