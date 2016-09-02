#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "memleak.h"

int main()
{
  void *a,*b,*c;
  char *s;
  dbg_init(10);
  dbg_catch_sigsegv();
  a = malloc(1000);
  b = malloc(30);
  a = realloc(a, 150);
  c = calloc(90, 3);
  b = realloc(b, 0);

  malloc(0);
  calloc(0, 10);
  realloc(0, 10);

  realloc(a, 0);

  free(0);

  s = strdup("A string.");
  s = strndup(s, 5);

  puts(s);

  dbg_mem_stat();
  dbg_heap_dump("");
  return 0;
}
