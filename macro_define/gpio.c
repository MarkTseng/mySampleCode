#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
#ifdef GPIO_PIN
	printf("GPIO_PIN = %f\n",GPIO_PIN);	
#endif
#ifdef GPIO_PIN_STR 
	printf("GPIO_PIN_STR = %s\n",GPIO_PIN_STR);	
#endif
	return 0;
}
