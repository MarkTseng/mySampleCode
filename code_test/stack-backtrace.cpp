/*
## Compile
g++ -Wall -std=c++11 -rdynamic -g stack-backtrace.cpp -lbacktrace -o stack-backtrace &&  ./stack-backtrace
clang++ -Wall -std=c++11 -rdynamic -g stack-backtrace.cpp -lbacktrace -o stack-backtrace && ./stack-backtrace
## Output
Backtrace:
0x401678 Baz::foo() 	/home/tsaarni/backtrace/stack-backtrace.cpp:73
0x401449 bar() 	/home/tsaarni/backtrace/stack-backtrace.cpp:83
0x401458 foo 	/home/tsaarni/backtrace/stack-backtrace.cpp:89
0x40147a main 	/home/tsaarni/backtrace/stack-backtrace.cpp:95  Backtrace:
## References:
https://github.com/gcc-mirror/gcc/blob/master/libbacktrace/backtrace.h
https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.3/a01696.html
http://stackoverflow.com/questions/4939636/function-to-mangle-demangle-functions
*/


#include <iostream>

#include <string.h>
#include <backtrace.h>
#include <cxxabi.h>

	static int
full_callback(void *data __attribute__((unused)), uintptr_t pc, 
		const char *filename, int lineno, const char *function)
{
	char *realname = nullptr;
	int demangle_status;

	realname = abi::__cxa_demangle(function, 0, 0, &demangle_status);

	if (demangle_status != 0)
	{
		realname = ::strdup(function);
	}

	printf("0x%lx %s \t%s:%d\n", (unsigned long) pc, 
			realname == nullptr ? "???" : realname,
			filename == nullptr ? "???" : filename, lineno);

	free(realname);

	return strcmp(function, "main") == 0 ? 1 : 0;
}


	static void
error_callback(void *data, const char *msg, int errnum)
{
	printf("Something went wrong in libbacktrace: %s\n", msg);
}


class Baz
{
	public:
		Baz() {}

		void foo()
		{
			struct backtrace_state *lbstate;

			printf ("Backtrace:\n");
			lbstate = backtrace_create_state (nullptr, 1, error_callback, nullptr);      
			backtrace_full(lbstate, 0, full_callback, error_callback, 0);
		}
};


	void
bar()
{
	Baz b;

	return b.foo();
}

	extern "C" void
foo()
{
	return bar();
}

	int
main(int argc, char *argv[])
{
	foo();
	return 0;
}

