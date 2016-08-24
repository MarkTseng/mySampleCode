#include <stdio.h>                                                                                       

typedef int (*func_t)(int);
static func_t f(int arg) {
        int nested(int nested_arg) {
                return (arg + nested_arg);
        }
        return &nested;
}
int main()
{
        {
                func_t g = f(0x1ab);
                printf("%d\n", (*g)(20));
        }
        printf("%d\n", f(400)(27));
        printf("%d\n", f(f(400)(27))(20));
        return 0;
}
