#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	unsigned long v1 = 0x00001111;
	unsigned long v2 = 0x00001202;
	unsigned long v;
	v = v1&(~v2);
	printf("v1&(~v2): 0x%lx\n",v);
	v = v | v2;
	printf("v | v2: 0x%lx\n",v);

	int a[5] ={1,2,3,4,5};
	int *p = (int *)(&a + 1);
	int *p1 = (int *)(&a);

	printf("*(a+1): %d, (*p-1): %d\n",*(a+1), (*p-1));
	printf("&a: %p, p: %p, p1:%p %lx\n",a, p, p1, p1 -p);

	return 0;
}
