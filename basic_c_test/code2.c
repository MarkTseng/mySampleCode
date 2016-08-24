#include <stdio.h>

char str1[] = "This is a string";

int main(int argc, char** argv) {

	char *p = str1;
	char **pp;
	//*pp = p;
	pp = &p;

	printf("p = %s\n" , p);
	printf("*pp = %s\n", *pp);

	return 0;
}
