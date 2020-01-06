/* leak.c */
#include <mcheck.h>
func()
{
  char *str[2];
  mtrace();
  str[0] = (char *)malloc(sizeof("memory leak start\n"));
  str[1] = (char *)malloc(sizeof("memory leak end\n"));
  strcpy(str[0] ,"memory leak start\n");
  strcpy(str[1] ,"memory leak end\n");
  printf("%s",str[0]);
  printf("%s",str[1]);
  free(str[1]);
  muntrace();
  return;
}

main()
{
  func();
}
