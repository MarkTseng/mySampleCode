#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
    system("ls -l /proc/self/");

    return 0;
}
