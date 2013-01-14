#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define tokenpaster(STRING) printf ("token" #STRING " = %d\n", token##STRING)

#define CMPFUNC_PROTO(TYPE) int TYPE##cmp(void *, void *, size_t)
CMPFUNC_PROTO(char);
CMPFUNC_PROTO(float);
CMPFUNC_PROTO(double);
CMPFUNC_PROTO(int8_t);


int main(int argc, char *argv[])
{
	int token34  = 34;
	tokenpaster(34);
	CMPFUNC_PROTO(char);
	return 0;
}
