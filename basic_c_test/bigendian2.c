#include <stdio.h>
#include <stdlib.h>

#define LITTLEENDIAN 0
#define BIGENDIAN    1

int machineEndianness()
{
   int i = 1;
   char *p = (char *) &i;
   if (p[0] == 1) // Lowest address contains the least significant byte
      return LITTLEENDIAN;
   else
      return BIGENDIAN;
}

int main()
{
	printf("%d\n",machineEndianness());
	return 0;
}
