all:
	arm-linux-gnueabihf-gcc -rdynamic -o test test.c
	arm-linux-gnueabihf-gcc -fPIC -shared -o wrap-libpreload.so wrap-preload.c -ldl	
	cp test preload_test.sh wrap-libpreload.so ~/nfs/ ; sync

clean:
	rm -rf *.o *.so test 
	
