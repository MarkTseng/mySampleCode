#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int div3(unsigned int n, unsigned int d)
{
	unsigned int q = 0, t = d, r = n;
	while (t <= n)
		t <<= 1;
	while ((t >>= 1) >= d) {
		if (t <= r) {
			q = 1 + (q << 1);
			r -= t;
		}
		else
			q <<= 1;
	}

	return r;
}
int main()
{
	int n,r1;
	printf("enter number");
	scanf("%d",&n);
	r1=div3(n,3);
	if(r1==0)
		printf("number is divisible by 3 ");
	else
		printf("number is not divisible ny 3");
	return 0;
}
