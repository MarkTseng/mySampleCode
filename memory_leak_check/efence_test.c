/* efence-test.c */
#include <stdio.h>

main()
{
  int i,j;
  char * c = (char *)malloc(20);
  printf("start of efence test\n");
  for(i=0; i < 24; i++)
    c[i] = 'c';
    free(c);
    printf("end of efence test\n");
  }

