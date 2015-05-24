#/* **************** LPD:1.0 s_04/Makefile **************** */
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
CFLAGS= -O2 -Wall -pedantic -I.

# static library:

all: lab1_static lab1_shared

lab1_sub.o:  lab1.h

liblab1_static.a:	lab1_sub.o
	ar rv liblab1_static.a lab1_sub.o

lab1_static:	liblab1_static.a lab1_main.c
	$(CC) $(CFLAGS) -o lab1_static lab1_main.c \
	-L. -llab1_static


liblab1_shared.so.1.0:  lab1_sub.c lab1.h
	$(CC) $(CFLAGS) -fPIC -I. -shared -Wl,-soname,liblab1_shared.so.1 \
	   -o liblab1_shared.so.1.0  lab1_sub.c
	rm -f liblab1_shared.so liblab1_shared.so.1
	ln -s liblab1_shared.so.1.0 liblab1_shared.so
	ln -s liblab1_shared.so.1.0 liblab1_shared.so.1

lab1_shared: liblab1_shared.so.1.0 lab1_main.c
	$(CC) $(CFLAGS) -fPIC -o lab1_shared -I. lab1_main.c \
	    -L. -llab1_shared -lc

clean:
	rm -f *.o *.a *.so* lab1_static lab1_shared

