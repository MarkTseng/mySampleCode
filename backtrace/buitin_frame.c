#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>

#define CALLSTACK_MAXLEN 64

//
// We use this macro instead of a for loop in backtrace() because the 
// documentation says that you have to use a constant, not a variable.
//
#define BT(X) {                                                         \
        case X:                                                         \
                if (!__builtin_frame_address(X)) {                      \
                        return X;                                       \
                }                                                       \
                                                                        \
                trace[X].address = __builtin_return_address(X);         \
                break;                                                  \
}

struct call {
        const void *address;
        const char *function;
        const char *object;
};

struct call trace[CALLSTACK_MAXLEN];

int
backtrace(int depth) {
        int         i;
        Dl_info     dlinfo;

        for (i = 0; i < depth; i++) {
                switch (i) {
                        BT(0);  
                        BT(1);
                        BT(2);
                        BT(3);
                        BT(4);
                        BT(5);
                        BT(6);
                        BT(7);
                        BT(8);
                        BT(9);
                        BT(10);
                        BT(11);
                        BT(12);
                        BT(13);
                        BT(14);
                        BT(15);
                        BT(16);
                        BT(17);
                        BT(18);
                        BT(19);
                        default:  return i;
                }

                if (dladdr(trace[i].address, &dlinfo) != 0) {
                        trace[i].function = dlinfo.dli_sname;
                        trace[i].object = dlinfo.dli_fname;
                }
        }

        return i;
}

void
f0() {
        int i;
        int depth;

    printf("FRAME0:%p, RA:%p\n", __builtin_frame_address(0), __builtin_return_address(0));
    printf("FRAME1:%p, RA:%p\n", __builtin_frame_address(1), __builtin_return_address(1));
    printf("FRAME2:%p, RA:%p\n", __builtin_frame_address(2), __builtin_return_address(2));
    printf("FRAME3:%p, RA:%p\n", __builtin_frame_address(3), __builtin_return_address(3));
    printf("FRAME4:%p, RA:%p\n", __builtin_frame_address(4), __builtin_return_address(4));
    printf("FRAME5:%p, RA:%p\n", __builtin_frame_address(5), __builtin_return_address(5));
#if 0
        depth = backtrace(CALLSTACK_MAXLEN);
        printf("DEPTH=%d\n", depth);

        for (i = 0 ; trace[i].object != NULL; i++) {
                printf("%s: %s (%p)\n", trace[i].object, trace[i].function, trace[i].address);
        }
#endif
}

void __attribute__ ((noinline)) f1() { f0(); }
void __attribute__ ((noinline)) f2() { f1(); }
void __attribute__ ((noinline)) f3() { f2(); }
void __attribute__ ((noinline)) f4() { f3(); }
void __attribute__ ((noinline)) f5() { f4(); }
void __attribute__ ((noinline)) f6() { f5(); }
void __attribute__ ((noinline)) f7() { f6(); }

int main(int argc, char **argv) {
        f2();
        return 0;
}

