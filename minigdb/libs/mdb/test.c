#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int say_hello(int x, int y, int z)
{
    printf("Hello, RA:%#x \n", __builtin_return_address(0));
    return 0;
}

int main(int argc, char **argv)
{
    int x = 0x55;
    int y = 0x56;
    int z = 0x57;
    char *mem;
    while(1)
    {
        printf("pid: %d\n", getpid());
        say_hello(0x255, 0x256, 0x257);
        mem = malloc(10);
        sleep(1);
        //printf("mem: %p, malloc:%p \n", mem, malloc);
        free(mem);
    }
    return 0;
}
