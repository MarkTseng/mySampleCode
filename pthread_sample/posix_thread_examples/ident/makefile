###########################################################
# An example source module to accompany...
#
# "Using POSIX Threads: Programming with Pthreads"
#     by Brad nichols, Dick Buttlar, Jackie Farrell
#     O'Reilly & Associates, Inc.
#
###########################################################
# ident/makefile
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

all : tident exit_status exit_status_alternative


tident: tident.o
	${CC} ${CFLAGS} ${LDFLAGS_T} tident.o -o tident

exit_status: exit_status.o
	${CC} ${CFLAGS} ${LDFLAGS_T} exit_status.o -o exit_status

exit_status_alternative: exit_status_alternative.o
	${CC} ${CFLAGS} ${LDFLAGS_T} exit_status_alternative.o -o exit_status_alternative

clean :
	rm -f *.o *~ *# core  \
	tident exit_status exit_status_alternative

