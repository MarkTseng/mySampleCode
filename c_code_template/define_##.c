#include <stdio.h>

#define paster(mm , val)\
	int token##mm = val; \
	printf( "token" #mm " = %d\n", token##mm ) \

int main()
{
	paster(100, 1000);  // preprocess to printf_s( "token" "9" " = %d", token9 );
	return 0;
}
