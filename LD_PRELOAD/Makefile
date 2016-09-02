all:
	arm-linux-gnueabihf-gcc -fPIC -shared -o wrap-libpreload.so -Wl,-wrap,calloc wrap-preload.c -ldl	
	arm-linux-gnueabihf-gcc -fPIC -shared -o mymalloc.so mymalloc.c -ldl	
	arm-linux-gnueabihf-gcc -fPIC -shared -o liba.so liba.c 
	arm-linux-gnueabihf-gcc -L./ -la -rdynamic -o test test.c -L.
	cp test preload_test.sh *.so ~/nfs/ ; sync

clean:
	rm -rf *.o *.so test 
	
