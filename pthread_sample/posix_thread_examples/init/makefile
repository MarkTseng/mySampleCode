###########################################################
# An example source module to accompany...
#
# "Using POSIX Threads: Programming with Pthreads"
#     by Brad nichols, Dick Buttlar, Jackie Farrell
#     O'Reilly & Associates, Inc.
#
###########################################################
# init/makefile
#
# makefile for the example programs
#

# Tru64 UNIX compile flags
T64CC = cc
T64CFLAGS = -g -std1 -warnprotos -pthread
T64LDFLAFS = -non_shared
T64LDFLAGS_T = -pthread

# GCC/Linux compile flags
GNUCC = gcc
GNUCFLAGS = -ansi  -Wstrict-prototypes -g
GNULDFLAGS = -static
GNULDFLAGS_T = ${GNULDFLAGS} -pthread

#####################################################
CC = ${GNUCC}
CFLAGS = ${GNUCFLAGS}
LDFLAGS = ${GNUCLDFLAGS}
LDFLAGS_T = ${GNULDFLAGS_T}

all : static_init mattr

static_init: static_init.o
	${CC} ${CFLAGS} ${LDFLAGS_T} static_init.o -o static_init

mattr: mattr.o
	${CC} ${CFLAGS} ${LDFLAGS_T} mattr.o -o mattr

clean :
	rm -f *.o *~ *# core  \
	static_init mattr
