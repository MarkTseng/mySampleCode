/*
 * gcc -rdynamic  -finstrument-functions
 *
 * */

#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>



//this func is called when your program has func is called
//thiz is enter func address
//callsite is func back address 
void __cyg_profile_func_enter(void *thiz, void *callsite)__attribute__((no_instrument_function));

void __cyg_profile_func_exit(void *thiz, void *callsite)__attribute__((no_instrument_function));

void display(void *enter, void *callsite, int flag)__attribute__((no_instrument_function));

void display(void *enter, void *callsite, int flag)
{
    Dl_info info;
    char cmd[1024] = "addr2line  -e ";
    char *ptr = cmd + strlen(cmd);
    int strsize = readlink("/proc/self/exe",ptr,sizeof(cmd) - (ptr - cmd) - 1);

    if(strsize == -1)
    {
	fprintf(stderr,"readlink is failure!\n");
	exit(-1);
    }

    FILE *fp = popen(cmd,"w");

    if(!fp)
    {
	perror("popen\n");
	exit(-1);
    }
    
    if(dladdr(enter,&info) == 0)
    {
	fprintf(stderr,"addr to funcname is failure\n");
	exit(-1);
    }

    //flag=1,enter or flag=0,exit
    if(flag)
    {
	char address[256];
	printf("\n-------------------------------------------\n");
	printf("your program has entered a function!\n");
	printf("this run filename is %s\n",info.dli_fname);
	printf("this function name is %s\n",info.dli_sname);
	printf("this function's address is %p\n",enter);
	printf("this function back row is \n");
	sprintf(address,"%p\0",callsite);
	fwrite(address,1,strlen(address) + 1,fp);
    }
    else
    {
	printf("\n-------------------------------------------\n");
	printf("your program will leave this function %s\n",info.dli_sname);
	printf("-------------------------------------------\n");
    }

    pclose(fp);
}

void __cyg_profile_func_enter(void *thiz, void *callsite)
{
    display(thiz,callsite,1);
} 

void __cyg_profile_func_exit(void *thiz, void *callsite)
{
    display(thiz,callsite,0);
}



