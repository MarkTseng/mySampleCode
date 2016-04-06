all:
	gcc -shared -fPIC -I/usr/include/python2.7/ -lpython2.7 -o myModule.so myModule.c
	gcc -shared -fPIC -I/usr/include/python2.7/ -lpython2.7 -o GTO.so GTO.c

clean:
	rm -rf *.so *.o
