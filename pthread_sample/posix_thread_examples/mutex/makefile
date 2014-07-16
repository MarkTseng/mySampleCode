###########################################################
# An example source module to accompany...
#
# "Using POSIX Threads: Programming with Pthreads"
#     by Brad nichols, Dick Buttlar, Jackie Farrell
#     O'Reilly & Associates, Inc.
#
###########################################################
# mutex/makefile
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

all : process_shared_mutex mutex_priority mutex_ceiling

# SVID and XOPEN flags require with GCC on Linux
process_shared_mutex.o: process_shared_mutex.c
	${CC} -c ${CFLAGS} ${SFLAGS} -o process_shared_mutex.o process_shared_mutex.c

process_shared_mutex: process_shared_mutex.o
	${CC} ${CFLAGS} ${LDFLAGS_T}  process_shared_mutex.o -o process_shared_mutex

mutex_priority: mutex_priority.o
	${CC} ${CFLAGS} ${LDFLAGS_T} mutex_priority.o -o mutex_priority

mutex_ceiling: mutex_ceiling.o
	${CC} ${CFLAGS} ${LDFLAGS_T} mutex_ceiling.o -o mutex_ceiling

clean :
	rm -f *.o *~ *# core  \
	process_shared_mutex mutex_priority mutex_ceiling
