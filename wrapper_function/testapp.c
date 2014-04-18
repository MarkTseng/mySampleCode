#include <stdio.h>
#include <stdlib.h>
int main()
{
    free(malloc(1024)); // malloc will resolve to __wrap_malloc
    return 0;
}

