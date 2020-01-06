#define _GNU_SOURCE
#include <dlfcn.h>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void myfunc3(void)
{
    void* callstack[128];
    int frames = backtrace(callstack, 128);
    int i;

    for(i=0;i<frames;++i)
    {
        Dl_info info;
        int res = dladdr(callstack[i], &info);
        if(res!=0)
        {
            printf ("%s + %#x %s\n", info.dli_sname, (unsigned int)((unsigned int)callstack[i] - (unsigned int)info.dli_saddr), info.dli_fname);
        }
    }
}

static void myfunc2(void)
{
	myfunc3();
}

void myfunc(int ncalls)
{
	if (ncalls > 1)
		myfunc(ncalls - 1);
	else
		myfunc2();
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "%s num-calls\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	myfunc(atoi(argv[1]));
	exit(EXIT_SUCCESS);
}

