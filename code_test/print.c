#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char a[32]="01234567890123456789";
    char str[10];
    printf("%s\n",a);
    snprintf(str, 10,"%s",a);
    printf("%10s\n",str);
}
