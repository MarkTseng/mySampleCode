#include "test.h"

int main(int argc, char *argv[])
{
	if(argc<2){
		printf("Please give the test target name\n");
		return 0;
	}

	printf("========================== test  begin ==========================\n");
	printf("argc=%d, argv=%s\n", argc, argv[1]);

	if(0 == strcmp( argv[1], (char*)"gslos")){
		Test_Gsl_OS();
	}
	else{
		printf("please give a param as test target!\n");
	}

	printf("========================== test  eixt  ==========================\n");

	return 0;
}

