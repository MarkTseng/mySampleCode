#/* **************** LPD:1.0 s_02/nomake.sh **************** */
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

echo CFLAGS= -O2 -Wall -pedantic > Makefile
echo CPPFLAGS = -I. >> Makefile
echo "" >> Makefile
echo all: lab1_hello lab2_hello >> Makefile
echo "" >> Makefile
echo lab1_hello: lab1_hello.c >> Makefile
echo "" >> Makefile
echo lab2_hello: lab2_hello_main.c lab2_hello_sub.c lab2_hello.h >> Makefile
echo -e '\tgcc $(CPPFLAGS) $(CFLAGS) lab2*.c -o lab2_hello' >> Makefile
echo "" >> Makefile
echo clean:  >> Makefile
echo -e '\t'rm -f lab1_hello lab2_hello *.o >> Makefile

make
