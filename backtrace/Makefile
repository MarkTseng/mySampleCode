all:
	gcc -o libunwind_backtrace -Wall -g libunwind_backtrace.c -lunwind
	gcc -rdynamic -o backtrace_symbols -Wall -g backtrace_symbols.c -lunwind
