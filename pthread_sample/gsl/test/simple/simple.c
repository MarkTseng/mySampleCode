#include <unistd.h>
#include <stdio.h>
#include <gsl.h>

//extern void GL_Init(void);
void GL_Uninit(void);

int main(int argc, char *argv[])
{
	int i;
	unsigned int *p;

	printf("Do GL_Init() in the begin\n");

	GL_Init();

	p = (unsigned int *)GL_MemAlloc(1024);
	printf("p = 0x%08X\n", (unsigned int)p);
	*p = 0xDEADBEEF;
	if (*p != 0xDEADBEEF) {
		printf("%s: simple test failed!\n", __FUNCTION__);
	} else {
		printf("%s: simple test ok.\n", __FUNCTION__);
	}
	GL_MemFree(p);

	printf(">>> Do STC test, regs0 = 0x%08X\n", (unsigned int)regs0);
	printf("GL_SetStc(1000000);\n");
	GL_SetStc(1000000);
	printf("GL_SetUsrRtc(3000000);\n");
	GL_SetUsrRtc(3000000);
	for (i = 0; i < 5; ++i) {
		printf("%2d GL_GetStc32() = %u, GL_GetStc64() = %llu\n", i, GL_GetStc32(), GL_GetStc64());
		printf("%2d GL_GetUsrRtc() = %u\n", i, GL_GetUsrRtc());
		printf("%2d GL_GetCpuCnt() = %u\n", i, GL_GetCpuCnt());
//		printf("%2d regs0 read stc = 0x%04X%04X\n", i, regs0->stc_31_16, regs0->stc_15_0);
		sleep(1);
	}

	printf("Do GL_Uninit() in the end\n");
	GL_Uninit();

	return 0;
}

