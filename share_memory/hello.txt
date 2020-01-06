#include <stdio.h>

#define __USE_GNU
#include <dlfcn.h>

void identify_function_ptr( void *func)  {
  Dl_info info;
  int rc;

  rc = dladdr(func, &info);

  if (!rc)  {
        printf("Problem retrieving program information for %x:  %s\n", func, dlerror());
    }

  printf("Address located in function %s within the program %s\n", info.dli_fname, info.dli_sname);

}

void *__real_malloc (size_t);

/* This function wraps the real malloc */
void * __wrap_malloc (size_t size)
{
	identify_function_ptr(__builtin_return_address(0));
    void *lptr = __real_malloc(size);
    printf("Malloc: %lu bytes @%p\n", size, lptr);
    return lptr;
}

