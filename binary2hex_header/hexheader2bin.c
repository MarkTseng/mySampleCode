#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hello_header.h"

int main(int argc, char * argv[])
{
	int i,len = sizeof(helloworld_bin);
	FILE *FileWrite;
	size_t ReadSize;
	
	FileWrite = fopen("hello_bin2","wb");
	if(FileWrite == NULL)
	{
		perror("Open file fail\n");
		exit(1);
	}

	printf("array size = %d\n", len);
	
	fwrite(helloworld_bin,1,len,FileWrite);
	
	printf("\n");

	fclose(FileWrite);

	return 0;
}
