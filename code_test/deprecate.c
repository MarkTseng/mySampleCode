#include <stdio.h>
#include <stdlib.h>

#ifdef __GNUC__
#    define GCC_VERSION_AT_LEAST(x,y) (__GNUC__ > (x) || __GNUC__ == (x) && __GNUC_MINOR__ >= (y))
#else
#    define GCC_VERSION_AT_LEAST(x,y) 0
#endif

#if GCC_VERSION_AT_LEAST(3,1)
#    define attribute_deprecated __attribute__((deprecated))
#elif defined(_MSC_VER)
#    define attribute_deprecated __declspec(deprecated)
#else
#    define attribute_deprecated
#endif


/* Variable Attribute */
attribute_deprecated int  variable_old = 0;

/* Function Attribute */
attribute_deprecated void function_old(void);

void function_old(void)
{
	printf("old function.\n");
	return;
}

int main(void)
{
	variable_old++;

	function_old();

	return EXIT_SUCCESS;
}
