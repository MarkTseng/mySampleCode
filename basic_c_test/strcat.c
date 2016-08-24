#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void strconnect(char *dest, char *src)
{
	int destlen = strlen(dest);
	int srclen = strlen(src);
	int i;
	for(i = 0; i < srclen; i++)
		dest[destlen+i] = src[i];

	dest[destlen+srclen+1] = '\0';
}

int main() 
{
	char str1[] = "Str1";
	char str2[] = "Str2";

	strcat(str1,str2);
	printf("str1:%s\n",str1);
	strconnect(str1,str2);
	printf("str1:%s\n",str1);
	
	return 0;
}
