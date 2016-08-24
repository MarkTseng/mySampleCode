#include <stdio.h>

char str1[] = "This is a string";

char *p = str1;
char **pp;

int main(int argc, char** argv) {

	//*pp =  p;
	
	pp =  &p;

	printf("p = %s\n" , p);
	printf("*pp = %s\n", *pp);

	return 0;
}
