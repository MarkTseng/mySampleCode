#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ReverseString(char *str)
{
	int len = strlen(str);
	int i = 0;
	char tmp;
	while(len-- > 0)
	{
		if(i > (len/2))
			break;
		tmp = str[i];
		str[i] = str[len];
		str[len] = tmp;
		i++;
	}
}

int main(int artc, char *argv[])
{
	char MyString1[] = "1234567890abcdefg";

	printf("MyString1: %s\n", MyString1);
	ReverseString(MyString1);
	printf("Reverse String: %s\n", MyString1);

	return 0;
}
