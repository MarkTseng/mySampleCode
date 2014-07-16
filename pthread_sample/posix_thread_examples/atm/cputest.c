/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * cputest.c 
 */
#include <sys/types.h>
#include <stdio.h>

void main(int argc, char **argv)
{
  int i, size;
  long count;

  if (argc >= 1) sscanf(argv[1], "%d", &size);

  for (i = 0; i < size; i++){
    count+=i;
  }

}
