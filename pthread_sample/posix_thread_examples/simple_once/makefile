###########################################################
# An example source module to accompany...
#
# "Using POSIX Threads: Programming with Pthreads"
#     by Brad nichols, Dick Buttlar, Jackie Farrell
#     O'Reilly & Associates, Inc.
#
###########################################################
# simple_once/makefile
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


all : once

once: once.o
	${CC} ${CFLAGS} once.o -o once ${LDFLAGS_T}

clean :
	rm -f *.o *~ *# core  \
	once

