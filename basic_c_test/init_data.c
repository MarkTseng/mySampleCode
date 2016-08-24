#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define	MAX_BUF_SIZE	256

int Init_Data(char **ppStrBuff)
{
	char *ret = NULL;

	*ppStrBuff = malloc(MAX_BUF_SIZE);
	memset(*ppStrBuff, 0, MAX_BUF_SIZE);

	ret = strcpy(*ppStrBuff,"Init My Data in SZIE 256");
	if(ret != NULL)
	{
		printf("ret = %s\n",ret);
	
		return 0;
	}
}

int main(int argc, char *argv[])
{
	char *MyInitData = NULL;

	Init_Data(&MyInitData);

	printf("## MyInitData : %s\n",MyInitData);

	free(MyInitData);
	return 0;
}
