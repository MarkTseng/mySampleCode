#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
		char str[]="Hello";
		char *p=str;
		int n=10;
		printf("str:%d, *p:%d, n:%d\n",sizeof(str), sizeof(p), sizeof(n));

int a[]={6,7,8,9,10};
int *ip=a;
*(ip++)+=123;
*(++ip)+=123;
printf("a=%d,%d\n",a[0],a[1]);
		return 0;
}
