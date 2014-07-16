###########################################################
# An example source module to accompany...
#
# "Using POSIX Threads: Programming with Pthreads"
#     by Brad nichols, Dick Buttlar, Jackie Farrell
#     O'Reilly & Associates, Inc.
#
###########################################################
# tpool/Makefile
#
# makefile for the example programs for pthreads programming book
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

all :  tpooltest

tpooltest: tpooltest.o tpool.o
	${CC} ${CFLAGS} ${LDFLAGS_T} tpooltest.o tpool.o -o tpooltest 

bugtest: bugtest.o tpool.o
	${CC} ${CFLAGS}  ${LDFLAGS_T} bugtest.o tpool.o -o bugtest

clean :
	rm -f *.o *~ *# core  \
	tpooltest
