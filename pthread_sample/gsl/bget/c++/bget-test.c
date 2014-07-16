
/**************************************************
 * 
 * bget interface test file
 * alex,2010-11
 *
 *************************************************/
#include "bget-api.h"
#include <stdio.h>

int pool1[10000];
int pool2[50000];

int main(void)
{
    bget_handle_t handle1;
    bget_handle_t handle2;
    
    handle1 = bget_create_pool(pool1, 1000);
    if (handle1 < 0)
        printf ("create pool1 err\n");
    else
        printf ("create pool1 success, handle1: %d\n", handle1);
    
    handle2 = bget_create_pool(pool2, 5000);
    if (handle2 < 0)
        printf ("create pool2 err\n");
    else
        printf ("create pool2 success, handle2: %d\n", handle2);

    bget_test(handle1);
    bget_test(handle2);
    

    
    return 0;
    
}

    
