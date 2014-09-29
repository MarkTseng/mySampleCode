#include <stdio.h>

#define paster( n ) printf( "token" #n " = %d\n", token##n )
int token9 = 9;

int main()
{
	paster(9);  // preprocess to printf_s( "token" "9" " = %d", token9 );
	return 0;
}
