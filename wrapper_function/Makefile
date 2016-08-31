all:
	arm-linux-gnueabihf-gcc -c malloc_wrapper.c 
	arm-linux-gnueabihf-gcc -c testapp.c
	arm-linux-gnueabihf-gcc -Wl,-wrap,malloc -rdynamic testapp.o malloc_wrapper.o -o testapp -ldl
	cp testapp ~/nfs/

clean:
	rm -rf *.o testapp
