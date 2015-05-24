#/* **************** LPD:1.0 s_03/nomake.sh **************** */
#/*
# * The code herein is: Copyright Jerry Cooperstein, 2009
# *
# * This Copyright is retained for the purpose of protecting free
# * redistribution of source.
# *
# *     URL:    http://www.coopj.com
# *     email:  coop@coopj.com
# *
# * The primary maintainer for this code is Jerry Cooperstein
# * The CONTRIBUTORS file (distributed with this
# * file) lists those known to have contributed to the source.
# *
# * This code is distributed under Version 2 of the GNU General Public
# * License, which you should have received with the source.
# *
# */
#!/bin/bash

rm -f Makefile

cat <<EOF > Makefile

all: hello_static hello_sharedA hello_sharedB lab2_dll_main
# static library:

hello_static: lab1_hello_main.c lab1_hello_sub.c lab1_hello.h
	      gcc -O2 -Wall -pedantic  -I. -c lab1_hello_sub.c
	      ar rv libhello_static.a lab1_hello_sub.o
	      gcc -O2 -Wall -pedantic -I. -o hello_static lab1_hello_main.c -L. -lhello_static

#method one for shared library (two steps to ld)

hello_sharedA: lab1_hello_main.c lab1_hello_sub.c lab1_hello.h
	       gcc -pedantic -Wall -O2 -fPIC -c -I. lab1_hello_sub.c
	       ld -shared -soname=libhello_sharedA.so.1 -o libhello_sharedA.so.1.0 \
-O2 -I . lab1_hello_sub.o -lc
	       ln -s libhello_sharedA.so.1.0 libhello_sharedA.so
	       ln -s libhello_sharedA.so.1.0 libhello_sharedA.so.1
	       gcc -pedantic -O2 -Wall -o hello_sharedA -I. lab1_hello_main.c -L. -lhello_sharedA -lc

#method two (one step)

hello_sharedB: lab1_hello_main.c lab1_hello_sub.c lab1_hello.h
	       gcc -pedantic -O2 -Wall -fPIC -I. -shared -Wl,-soname=libhello_sharedB.so.1 \
-o libhello_sharedB.so.1.0  lab1_hello_sub.c
	       ln -s libhello_sharedB.so.1.0 libhello_sharedB.so
	       ln -s libhello_sharedB.so.1.0 libhello_sharedB.so.1
	       gcc -pedantic -O2 -Wall -fPIC -o hello_sharedB -I. lab1_hello_main.c \
-L. -lhello_sharedB -lc

# SECOND LAB

lab2_dll_main:	lab2_dll_main.c
		gcc -O2 -Wall -pedantic -fPIC -shared -o libhello_dl.so lab2_dll_lib.c
		gcc -O2 -Wall -pedantic -o lab2_dll_main lab2_dll_main.c -ldl

clean:
	rm -f hello_static hello_shared* *libhello* *.o lab2_dll_main

EOF

make

