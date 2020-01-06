all:
	arm-linux-gnueabihf-gcc -c testapp.c
	arm-linux-gnueabihf-gcc testapp.o -o testapp 
	cp testapp hello.txt ~/nfs/

clean:
	rm -rf *.o testapp
