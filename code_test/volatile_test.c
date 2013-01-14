#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "regmap_9200.h"

int main()
{
	//printf("stamp:%d \n",HWREG_R(stamp));
	printf("stamp:%lx \n",HWREG_ADDR(stamp));
	return 0;
}
