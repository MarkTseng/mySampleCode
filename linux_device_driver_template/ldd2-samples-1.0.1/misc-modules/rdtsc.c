/*
 *
 * Copyright (C) 2001 Alessandro Rubini and Jonathan Corbet
 * Copyright (C) 2001 O'Reilly & Associates
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.  The citation
 * should list that the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates.   No warranty is attached;
 * we cannot take responsibility for errors or fitness for use.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define rdtsc(dest) \
     __asm__ __volatile__("nop; mfc0 %0,$9; nop" : "=r" (dest))


int main(int argc, char **argv)
{
    unsigned int count1,count2,count3,fix;
    printf("rdtsc demo (using mfc0 on mips)\n");

    rdtsc(count1);
    rdtsc(count2);
    fix = count2-count1;

    rdtsc(count1);
    printf("rdtsc()  took %5ui cycles\n",fix);
    rdtsc(count2);

    printf("printf() took %5ui cycles\n",count2-count1-fix);

    rdtsc(count1);
    sleep(0);
    rdtsc(count2);
    printf("sleep(0) took %5ui cycles\n",count1-count1-fix);

    rdtsc(count1);
    sleep(1);
    rdtsc(count2);
    printf("sleep(1) took %5ui cycles\n",count1-count1-fix);

    return 0;
}
