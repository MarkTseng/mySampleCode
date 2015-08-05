all:
	gcc -Wall -Werror -fPIC -c foo.c
	gcc foo.o -o libfoo.so -shared -Wl,-soname=libfoo.so 
	ar rcv libfoo.a foo.o 
	gcc -L./ -Wall -static -o test main.c -lfoo

clean:
	rm -rf *.o *.so
