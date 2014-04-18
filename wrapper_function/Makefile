all:
	gcc -c malloc_wrapper.c
	gcc -c testapp.c
	gcc -Wl,-wrap,malloc testapp.o malloc_wrapper.o -o testapp

clean:
	rm -rf *.o testapp
