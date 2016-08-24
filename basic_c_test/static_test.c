#include <stdio.h>

int globalVar;

void foo1()
{
	int localVar = 0;
	static int staticVar = 0;

	localVar++;
	staticVar++;
	globalVar++;

	printf("[foo1] local: %d, static: %d, global: %d\n",
			localVar, staticVar, globalVar);
}

void foo2()
{
	int localVar = 0;
	static int staticVar = 0;

	localVar++;
	staticVar++;
	globalVar++;

	printf("[foo2] local: %d, static: %d, global: %d\n",
			localVar, staticVar, globalVar);

}

int main()
{
	foo1();
	foo2();

	foo1();
	foo2();
}
