###########################################################
# An example source module to accompany...
#
# "Using POSIX Threads: Programming with Pthreads"
#     by Brad nichols, Dick Buttlar, Jackie Farrell
#     O'Reilly & Associates, Inc.
###########################################################
# list/makefile
#
# makefile for the example programs 
#

# Tru64 UNIX compile flags
T64CC = cc
T64CFLAGS = -g -std1 -warnprotos -pthread
T64LDFLAGS = -non_shared
T64LDFLAGS_T = ${T64LDFLAGS} -pthread

# GCC/Linux compile flags
GNUCC = gcc
GNUCFLAGS = -g -ansi -Wstrict-prototypes
GNULDFLAGS = -static
GNULDFLAGS_T = ${GNULDFLAGS} -pthread

#####################################################
CC = ${GNUCC}
CFLAGS = ${GNUCFLAGS}
LDFLAGS = ${GNULDFLAGS}
LDFLAGS_T = ${GNULDFLAGS_T}

all : llist_test llist_test_threads llist_test_threads_rw

llist_test: llist_test.o llist.o llist.h
	${CC} ${CFLAGS} ${LDFLAGS} llist.o llist_test.o -o llist_test

llist_test_threads: llist_test_threads.o llist_threads.o llist_threads.h
	${CC} ${CFLAGS} ${LDFLAGS_T} llist_threads.o llist_test_threads.o -o llist_test_threads

llist_test_threads_rw: llist_test_threads_rw.o llist_threads_rw.o rdwr.o llist_threads_rw.h
	${CC} ${CFLAGS} ${LDFLAGS_T} llist_threads_rw.o llist_test_threads_rw.o rdwr.o -o llist_test_threads_rw

clean :
	rm -f *.o *~ *# core  \
	llist llist_test llist_test_threads \
	llist_test_threads_rw
