#include "test.h"


int main(int argc, char *argv[])
{
	if(argc<2){
		printf("Please give the test target name\n");
		return 0;
	}

	printf("========================== test  begin ==========================\n");
	printf("argc=%d, argv=%s\n", argc, argv[1]);

	if(0 == strcmp( argv[1], (char*)"sem")){
		Test_GL_Semaphore();
	}else if(0 == strcmp( argv[1], (char*)"mutex")){
		Test_GL_Mutex();
	}else if(0 == strcmp( argv[1], (char*)"queue")){
		Test_GL_Queue();
	}else if(0 == strcmp( argv[1], (char*)"list")){
		Test_GL_List();
	}else if(0 == strcmp( argv[1], (char*)"flag")){
		Test_GL_Flag();
	}else if(0 == strcmp( argv[1], (char*)"cross")){
		Test_GL_Cross();
	}
	else{
		printf("please give a param as test target!\n");
	}

	printf("========================== test  eixt  ==========================\n");
	return 0;
}

