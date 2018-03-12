all:
	swig -python example.i
	gcc -c example.c example_wrap.c -I/usr/include/python2.7 -fPIC
	ld -shared example.o example_wrap.o -o _example.so 

clean:
	rm -rf *.so *.pyc *.o

test:
	python test.py
