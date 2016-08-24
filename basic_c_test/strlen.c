#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int StringLength(char *str)
{
	int len = 0;
	while(str[len] != '\0')
		len++;

	return len;
}

int main(int artc, char *argv[])
{
	char MyString1[] = "1234567890abcdefg";

	printf("MyString1: %d\n", strlen(MyString1));
	printf("String length: %d\n", StringLength(MyString1));
	printf("String size: %d\n", sizeof(MyString1));

	return 0;
}
