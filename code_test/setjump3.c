#include <stdio.h>
#include <setjmp.h>

jmp_buf jump_buffer;

void a(int count) 
{
    printf("a(%d) called\n", count);
    longjmp(jump_buffer, count+1);  // setjump() will return count+1
}
 
int main()
{
    volatile int count = 0; // modified locals in setjmp scope must be volatile
    setjmp(jump_buffer);  // equality against constant expression in an if
    //if (setjmp(jump_buffer) != 9) { // equality against constant expression in an if
		printf("back to main\n");
        a(++count);  // This will cause setjmp() to exit
    //}
}
