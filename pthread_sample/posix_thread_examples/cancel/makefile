###########################################################
# An example source module to accompany...
#
# "Using POSIX Threads: Programming with Pthreads"
#     by Brad nichols, Dick Buttlar, Jackie Farrell
#     O'Reilly & Associates, Inc.
#
###########################################################
# cancel/makefile
#
# makefile for the example program
#

# Tru64 UNIX compile flags
T64CC = cc
T64CFLAGS = -g -std1 -warnprotos -pthread
T64LDFLAGS = -non_shared
T64LDFLAGS_T = ${T64LDFLAGS} -pthread

# GCC/Linux compile flags
GNUCC = gcc
GNUCFLAGS = -ansi  -Wstrict-prototypes -g 
GNULDFLAGS = -static
GNULDFLAGS_T = ${GNULDFLAGS} -pthread

#####################################################
CC = ${GNUCC}
CFLAGS = ${GNUCFLAGS}
LDFLAGS = ${GNULDFLAGS}
LDFLAGS_T = ${GNULDFLAGS_T}

all : cancel

cancel: cancel.o
	${CC} ${CFLAGS} ${LDFLAGS_T} cancel.o -o cancel

clean :
	rm -f *.o *~ *# core  \
	cancel

