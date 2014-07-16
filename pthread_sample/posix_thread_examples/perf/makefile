###########################################################
# An example source module to accompany...
#
# "Using POSIX Threads: Programming with Pthreads"
#     by Brad nichols, Dick Buttlar, Jackie Farrell
#     O'Reilly & Associates, Inc.
#
###########################################################
# perf/makefile
#
# makefile for the example programs
#

# Tru64 UNIX compile flags
T64CC = cc
T64CFLAGS = -g -std1 -warnprotos -pthread
T64LDFLAGS = -non_shared -lm
T64LDFLAGS_T = ${T64LDFLAGS} -pthread

# GCC/Linux compile flags
GNUCC = gcc
GNUCFLAGS = -g -ansi -Wstrict-prototypes
GNULDFLAGS = -static -lm
GNULDFLAGS_T = ${GNULDFLAGS} -pthread

#####################################################

CC = ${GNUCC}
CFLAGS = ${GNUCFLAGS}
LDFLAGS = ${GNULDFLAGS}
LDFLAGS_T = ${GNULDFLAGS_T}

all : goodlocks badlocks polling

polling: polling.o
	${CC} ${CFLAGS} -o polling polling.o ${LDFLAGS_T}

goodlocks: goodlocks.o
	${CC} ${CFLAGS} -o goodlocks goodlocks.o ${LDFLAGS_T}

badlocks: badlocks.o
	${CC} ${CFLAGS} -o badlocks badlocks.o ${LDFLAGS_T}

clean :
	rm -f *.o *~ *# core  \
	goodlocks badlocks polling
