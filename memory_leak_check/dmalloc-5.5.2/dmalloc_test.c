/* dmalloc_test.c */
#include <stdio.h>
#include <stdlib.h>

#ifdef USE_DMALLOC
#include <dmalloc.h>
#endif

int main()
{
  char *test[5];
  unsigned int i;

  for (i=0; i < 5; i++)
  {
    unsigned int size = rand()%1024;
    test[i] = (char *)malloc(size);
    printf ("Allocated memory of size %d\n",size);
  }
  for (i=0; i<2; i++)
    free(test[i*2]);
}

