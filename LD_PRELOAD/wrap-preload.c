/*******************************************
* File Name : wrap-preload.c
* Purpose :
* Creation Date : 01-09-2016
* Last Modified : Thu 01 Sep 2016 09:44:53 AM CST
* Created By : Mark Tseng  
**********************************************/

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

/* reslove caller function name */
void identify_function_ptr( void *func)  {
  Dl_info info;
  int rc;

  rc = dladdr(func, &info);

  if (!rc)  {
        printf("Problem retrieving program information for %x:  %s\n", func, dlerror());
    }

  printf("Address located in function %s within the program %s\n", info.dli_fname, info.dli_sname);

}


/* Function pointers to hold the value of the glibc functions */
void *(*__real_malloc) (size_t) = NULL;
void *(*__real_calloc) (size_t num,size_t size) = NULL;
void (*__real_free) (void *ptr) = NULL;

/* wrapping write function call */
void * malloc (size_t size)
{

	identify_function_ptr(__builtin_return_address(0));
    __real_malloc = dlsym(RTLD_NEXT, "malloc");
    void *lptr = __real_malloc(size);
    printf("Malloc: %lu bytes @%p\n", size, lptr);
    return lptr;
}

/* This function wraps the real malloc */
void * calloc (size_t num, size_t size)
{
	identify_function_ptr(__builtin_return_address(0));
    __real_calloc = dlsym(RTLD_NEXT, "calloc");
    void *lptr = __real_calloc(num, size);
    printf("Calloc: %lu bytes @%p\n", size, lptr);
    return lptr;
}

void free(void *ptr)
{
	identify_function_ptr(__builtin_return_address(0));
    __real_free = dlsym(RTLD_NEXT, "free");
    __real_free(ptr);
    printf("Free: @%p\n", ptr);
}


