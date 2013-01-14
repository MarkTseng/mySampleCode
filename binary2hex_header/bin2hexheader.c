#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFSIZE	4096
unsigned char BUF[MAX_BUFSIZE];

int main(int argc, char * argv[])
{
	int i;
	FILE *FileRead,*FileWrite;
	size_t ReadSize;
	
	FileRead = fopen(argv[1],"rb");
	FileWrite = fopen("hello_header.h","w");
	if((FileRead == NULL) || (FileWrite == NULL))
	{
		perror("Open file fail\n");
		exit(1);
	}

	// write FileWrite header
	fprintf(FileWrite,"const unsigned char helloworld_bin [] = { \n");
	
	while(!feof(FileRead))
	{
		ReadSize = fread(BUF,1,sizeof(BUF),FileRead);

		for(i=0; i< ReadSize;i++)
		{
			fprintf(FileWrite,"0x%02x, ",BUF[i]);
			if(i%16 == 15)
				fprintf(FileWrite,"\n",BUF[i]);
		}

		printf("\n");
	}

	fclose(FileRead);
	fprintf(FileWrite,"};\n");
	fclose(FileWrite);
	
	return 0;
}
