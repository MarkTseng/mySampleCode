#include <stdlib.h>
#include <unistd.h>

void *a()
{
  return malloc(1000);
}

void *b()
{
  void *result = a();
  free (malloc(1000));

  return result;
}

int main()
{
  char *block;

  free (malloc(1000));
  block = b();
  block = malloc(1000);
  block = b();
  free(block);

  return 0;
}
