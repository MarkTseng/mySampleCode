/* arm-linux-gnueabihf-gcc -fsanitize=address -fno-omit-frame-pointer -g -lasan memleak_aSan.c -o memleak_aSan
 */
#include <stdlib.h>
int main() {
#if 0
  char *x = (char*)malloc(10 * sizeof(char*));
  free(x);
  return x[5];
#else
return 0;
#endif
}

