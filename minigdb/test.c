#include <stdio.h>
#include <string.h>

int say_hello(int x, int y, int z)
{
    printf("Hello\n");
    return 0;
}

int main(int argc, char **argv)
{
    int x = 0x55;
    int y = 0x56;
    int z = 0x57;

    say_hello(0x255, 0x256, 0x257);
    return 0;
}
