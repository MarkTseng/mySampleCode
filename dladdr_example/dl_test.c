#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define __USE_GNU
#include <dlfcn.h>

int main(void);
#if 1
void identify_function_ptr( void *func)  {   
  Dl_info info;  
  int rc;      

  rc = dladdr(func, &info);   

  if (!rc)  {    
      printf("Problem retrieving program information for %x:  %s\n", func, dlerror()); 
  }  
  
  printf("Address located in function %s within the program %s\n", info.dli_fname, info.dli_sname);  

}

int A_TEST()
{
	printf("A_TEST %p\n",__builtin_return_address(0));
	B_TEST();
	return 0;
}

int B_TEST()
{
	printf("B_TEST %p\n",__builtin_return_address(0));
	identify_function_ptr(A_TEST);
	return 0;
}

int main()
{
	printf("A_TEST %p\n", A_TEST);
	printf("B_TEST %p\n", B_TEST);
	printf("identify_function_ptr %p\n", identify_function_ptr);
	A_TEST();
}
#endif

#if 0
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main(int argc, char **argv)
{
    void *handle;
    double (*cosine)(double);
    char *error;

   handle = dlopen("libm.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }

   dlerror();    /* Clear any existing error */

   /* Writing: cosine = (double (*)(double)) dlsym(handle, "cos");
       would seem more natural, but the C99 standard leaves
       casting from "void *" to a function pointer undefined.
       The assignment used below is the POSIX.1-2003 (Technical
       Corrigendum 1) workaround; see the Rationale for the
       POSIX specification of dlsym(). */

   *(void **) (&cosine) = dlsym(handle, "cos");

   if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }

   printf("%f\n", (*cosine)(2.0));
    dlclose(handle);
    exit(EXIT_SUCCESS);
}
#endif
