###########################################################
# An example source module to accompany...
#
# "Using POSIX Threads: Programming with Pthreads"
#     by Brad nichols, Dick Buttlar, Jackie Farrell
#     O'Reilly & Associates, Inc.
#
###########################################################
# signals/makefile
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


all : sig stat_sigwait

sig.o: sig.c
	${CC} -c ${NOANSI} -o sig.o sig.c 

sig : sig.o
	${CC} ${CFLAGS} ${LDFLAGS_T} sig.o -o sig 

stat_sigwait.o: stat_sigwait.c
	${CC} -c  ${NOANSI} -o stat_sigwait.o stat_sigwait.c

stat_sigwait : stat_sigwait.o
	${CC} ${CFLAGS} ${LDFLAGS_T} stat_sigwait.o -o stat_sigwait

clean :
	rm -f *.o *~ *# core  \
	sig stat_sigwait 
