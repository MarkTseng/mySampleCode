
all:
	gcc -g -rdynamic func.c -fPIC -shared -o libfunc.so -Wl,-Map,add.map
	gcc -g -rdynamic test.c -L. -lfunc -Wl,-rpath=. -o test


clean:
	rm -rf test *.so
