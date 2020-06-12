#include "stacktrace.h"
#include <map>
#include <signal.h>

namespace Nu {

template<typename Type>
struct Alpha
{
    struct Beta
    {
    void func() {
    }
    void func(Type) {
    }
    };
};

struct Gamma
{
    template <int N>
    void unroll(double d) {
    unroll<N-1>(d);
    }
};

template<>
void Gamma::unroll<0>(double) {
 *((volatile char *)0x0) = 0x9999;
}

} // namespace Nu

void signal_handler(int signo)
{
        printf("\n=========>>>catch signal %d <<<=========\n", signo);
        printf("Dump stack start...\n");
        print_stacktrace();
        printf("Dump stack end...\n");
        signal(signo, SIG_DFL);
        raise(signo);
}

int main()
{
    signal(SIGSEGV, signal_handler);  
    Nu::Alpha<int>::Beta().func(42);
    Nu::Alpha<char*>::Beta().func("42");
    Nu::Alpha< Nu::Alpha< std::map<int, double> > >::Beta().func();
    Nu::Gamma().unroll<5>(42.0);
}
