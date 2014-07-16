###########################################################
# An example source module to accompany...
#
# "Using POSIX Threads: Programming with Pthreads"
#     by Brad nichols, Dick Buttlar, Jackie Farrell
#     O'Reilly & Associates, Inc.
#
###########################################################
# simple/Makefile
#
# makefile for the example programs 
#

# Tru64 UNIX compile flags
T64CC = cc
T64CFLAGS = -g -std1 -warnprotos -pthread
T64LDFLAGS = -non_shared
T64LDFLAGS_T = ${T64LDFLAGS} -pthread
T64SFLAGS =
T64NOANSI = ${T64CFLAGS}


# GCC/Linux compile flags
GNUCC = gcc
GNUCFLAGS = -g -ansi -Wstrict-prototypes
GNULDFLAGS = -static
GNULDFLAGS_T = ${GNULDFLAGS} -pthread
GNUSFLAGS = -D_SVID_SOURCE -D_XOPEN_SOURCE
GNUNOANSI = -g -Wstrict-prototypes

#####################################################
CC = ${GNUCC}
CFLAGS = ${GNUCFLAGS}
LDFLAGS = ${GNULDFLAGS}
LDFLAGS_T = ${GNULDFLAGS_T}
SFLAGS = ${GNUSFLAGS}
NOANSI = ${GNUNOANSI}


all : simple simple_threads simple_processes simple_mutex 


simple : simple.o
	${CC} ${CFLAGS} simple.o -o simple

simple_threads : simple_threads.o
	${CC} ${CFLAGS} ${LDFLAGS_T} simple_threads.o -o simple_threads 

# SVID and XOPEN flags required with GCC on Linux
simple_processes.o : simple_processes.c
	${CC} -c ${CFLAGS} ${SFLAGS} -o simple_processes.o simple_processes.c

simple_processes : simple_processes.o
	${CC} ${CFLAGS} simple_processes.o -o simple_processes

simple_mutex : simple_mutex.o
	${CC} ${CFLAGS} ${LDFLAGS_T} simple_mutex.o -o simple_mutex

clean :
	rm -f *.o *~ *# core  a.out\
	simple simple_threads simple_processes simple_mutex

run_test :
	./test_simple.script

