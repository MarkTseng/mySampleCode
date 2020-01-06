#include <stdio.h>
#include <setjmp.h>

// 儲存程式跳躍時所需之資訊
jmp_buf jmpbuffer;

int fun_a(int v) {
  int r = v * 2 - 1;
  if (r < 0) {
    // 跳躍至 main 函數
    longjmp(jmpbuffer, 1);
  }
  return r;
}
int fun_b(int v) {
  int r = fun_a(v) + 6;
  if (r > 10) {
    // 跳躍至 main 函數
    longjmp(jmpbuffer, 2);
  }
  return r;
}
int fun_c(int v) {
  int r = fun_b(v) * 5 - 21;
  return r;
}
int main() {
  // 設定跳躍目標位置
  int jmpVal = setjmp(jmpbuffer);
  if ( jmpVal == 1 ) {
    printf("fun_a error\n");
  } else if ( jmpVal == 2 ) {
    printf("fun_b error\n");
  } else {  // jmpVal == 0
    int x = -5;
    int result = fun_c(x);
    printf("Result = %d\n", result);
  }
  return 0;
}

