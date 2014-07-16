/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * trace.c
 *
 * Debug trace macro
 */

#include <stdio.h>

#define DEBUG 1


#ifdef DEBUG
#define DPRINTF(x)	printf x
#else
#define DPRINTF(x)
#endif

extern int
main(void)
{
   int x = 1; 

   DPRINTF(("x = %d\n", x));

   return 0;
}



