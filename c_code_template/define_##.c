#include <stdio.h>

#define paster( n, mm ) printf( "token" #n " = %d\n", token##mm )
int token100 = 100;

int main()
{
	paster(9, 100);  // preprocess to printf_s( "token" "9" " = %d", token9 );
	return 0;
}
