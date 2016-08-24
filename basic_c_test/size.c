#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	char a1;
	char a2;
	short a3;
	int a4;
}MyStruct1;

typedef struct
{
	char a1;
	short a2;
	char a3;
	int a4;
} MyStruct2;

int main()
{
	union Test {
		int i;
		char j[20];
	};

	union Test uniVar1;
	uniVar1.i = 20;

	printf("int szie:%i\n",sizeof(int));
	printf("short szie:%d\n",sizeof(short));
	printf("float szie:%d\n",sizeof(float));
	printf("char szie:%d\n",sizeof(char));
	printf("short int szie:%d\n",sizeof(short int));
	printf("unsigned int szie:%d\n",sizeof(unsigned int));
	printf("unsigned char szie:%d\n",sizeof(unsigned char));

	printf("int * szie:%i\n",sizeof(int *));
	printf("short * szie:%d\n",sizeof(short *));
	printf("float * szie:%d\n",sizeof(float *));
	printf("char * szie:%d\n",sizeof(char *));
	printf("short * int szie:%d\n",sizeof(short int *));
	printf("unsigned int * szie:%d\n",sizeof(unsigned int *));
	printf("unsigned char *  szie:%d\n",sizeof(unsigned char *));


	printf("Union Test szie:%d\n",sizeof(union Test));
	
	printf("Union Test.i:%p\n",&uniVar1.i);
	printf("Union Test.j:%p\n",&uniVar1.j[0]);
	printf("MyStruct1 size:%p\n",sizeof(MyStruct1));
	printf("MyStruct2 size:%p\n",sizeof(MyStruct2));
	return 0;
}
/*
int szie:4
short szie:2
float szie:4
char szie:1
short int szie:2
unsigned int szie:4
unsigned char szie:1
double : 8 bytes
Union Test szie:20
Union Test.i:0xbf91dad8
Union Test.j:0xbf91dad8
*/
