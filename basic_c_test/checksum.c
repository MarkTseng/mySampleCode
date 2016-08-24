#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Checksum(unsigned char *aData)
{
	int i;
	unsigned char checksum = 0;

	for(i = 0; i < sizeof(aData) ;i++)
		checksum += *(aData + i);
	
	checksum = ~(checksum) + 1;
	
	aData[sizeof(aData)-1] = checksum;
	printf("fun: %s - line: %d\n",__func__,__LINE__);
	printf("checksum = 0x%x\n",checksum );
	printf("aData checksum byte = 0x%x\n", aData[sizeof(aData)-1] );

}


int main (int argc, char *argv[])
{
	int i;
	unsigned char data[]={0xFF,0x08,0x02,0x68,0x00,0xFF,0x00,0x00};
	
	Checksum(data);
	
	for(i=0;i<sizeof(data);i++)
	{
		printf("data[%d] = 0x%x\n",i,data[i]);
	}

	return 0;
}
