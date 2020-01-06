all:
	gcc -o libunwind_backtrace -Wall -g libunwind_backtrace.c -lunwind
	arm-linux-gnueabihf-gcc -o libunwind_backtrace_arm -Wall -g libunwind_backtrace.c -I/home/mark/myResearch/libunwind/LIBUNWIND_ARM/include -L/home/mark/myResearch/libunwind/LIBUNWIND_ARM/lib -lunwind 
	arm-linux-gnueabihf-gcc -rdynamic -o backtrace_symbols -Wall -g backtrace_symbols.c 
	arm-linux-gnueabihf-gcc -rdynamic -o dladdr_backtrace -Wall -g dladdr_backtrace.c -ldl
	arm-linux-gnueabihf-gcc -o buitin_frame -Wall -g buitin_frame.c -ldl -rdynamic 
	

clean:
	rm -rf *.o libunwind_backtrace backtrace_symbols dladdr_backtrace
