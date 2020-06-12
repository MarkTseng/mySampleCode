#include <stdio.h>
void func_c()
{
 *((volatile char *)0x0) = 0x9999;
}
