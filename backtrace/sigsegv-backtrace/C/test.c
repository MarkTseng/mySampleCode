#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <execinfo.h>
#include <signal.h>

#define BACKTRACE_SIZE   16

void func_c();
void dump(void)
{
        int j, nptrs;
        void *buffer[BACKTRACE_SIZE];
        char **strings;

        nptrs = backtrace(buffer, BACKTRACE_SIZE);

        printf("backtrace() returned %d addresses\n", nptrs);

        strings = backtrace_symbols(buffer, nptrs);
        if (strings == NULL) {
                perror("backtrace_symbols");
                exit(EXIT_FAILURE);
        }

        for (j = 0; j < nptrs; j++)
                printf("  [%02d] %s\n", j, strings[j]);

        free(strings);
}

void signal_handler(int signo)
{


        printf("\n=========>>>catch signal %d <<<=========\n", signo);

        printf("Dump stack start...\n");
        dump();
        printf("Dump stack end...\n");

        signal(signo, SIG_DFL);
        raise(signo);
}

void func_b()
{
 func_c();
}

void func_a()
{
 func_b();
}

int main()
{
    signal(SIGSEGV, signal_handler);  
    func_a();
    return 0;
}
