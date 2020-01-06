#include <stdio.h>
#include <setjmp.h>
jmp_buf jmpbuffer;
jmp_buf jmpbuffer2;
static int g;  // global
void f1() {
  longjmp(jmpbuffer, 1);
}
void f2(int l, int g, int r, int v, int s) {
  setjmp(jmpbuffer2);
  printf("local = %d, global = %d, register = %d, "
      "volatile = %d, static = %d\n",
      l, g, r, v, s);
  f1();
}
int main() {
  int l;           // local
  register int r;  // register
  volatile int v;  // volatile
  static int s;    // static
  l = 1, g = 2, r = 3, v = 4, s = 5;

  if ( setjmp(jmpbuffer) != 9 ) {
    l = 1, g = 2, r = 3, v = 4, s = 5;
    printf("back to here local = %d, global = %d, register = %d, "
        "volatile = %d, static = %d\n",
        l, g, r, v, s);
    //return 0;
  }

  l = 101, g = 102, r = 103, v = 104, s = 105;

  f2(l, g, r, v, s);
  longjmp(jmpbuffer2, 1);
  return 0;
}

