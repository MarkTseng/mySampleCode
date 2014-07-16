#
# makefile
#
# makefile for automated teller machine (ATM) sr example program
#
# Initially setup for a DEC OSF/1 environment. Use on other platforms
# may require changes to LOADLIBS, CLAGS etc.
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
SFLAGS =  ${GNUSFLAGS}
NOANSI = ${GNUNOANSI} 

DB = atm_db.o

# Serial Server - No threads, no processes
SSOBJS   = atm_svr_serial.o \
           atm_com_svr.o \
           ${DB} \
           atm_svr_password.o

# Multi-Process Server - spawns a child process for each client request
MSOBJS   = atm_svr_processes.o \
           atm_com_svr.o \
           ${DB} \
           atm_svr_password.o

# Threaded Sr Objects
TSOBJS   = atm_svr.o \
	   atm_com_svr.o \
	   ${DB} \
	   atm_svr_password.o

# Threaded Sr Objects - single mutex
TSSMOBJS   = atm_svr_1mtx.o \
	   atm_com_svr.o \
	   ${DB} \
	   atm_svr_password.o

# Threaded Sr Objects - nosync
TSNSOBJS   = atm_svr_nosync.o \
	   atm_com_svr.o \
	   ${DB} \
	   atm_svr_password.o

# Broken Threaded Sr Objects
TSBOBJS   = atm_svr_broken.o \
	   atm_com_svr.o \
	   ${DB} \
	   atm_svr_password.o

# Threaded Svr Objects using thread pool
TSPOBJS   = atm_svr_tpool.o \
	   atm_com_svr.o \
	   ${DB} \
	   atm_svr_password.o\
           tpool.o

# Threaded Svr Objects with Cancellation Cleanup example
TSCOBJS   = atm_svr_cancel.o \
	   atm_com_svr.o \
	   ${DB} \
	   atm_svr_password.o

# Threaded Svr Objects with SIGUSR1 handling example
TSSIGOBJS   = atm_svr_signals.o \
	   atm_com_svr.o \
	   ${DB} \
	   atm_svr_password.o

# Threads Svr Objects that use pthread_once for init routine
TSPTOOBJS   = atm_svr.o \
           atm_com_svr_once.o \
           ${DB} \
           atm_svr_password.o

# Client Objects
COBJS    = atm_client.o \
	   atm_com_client.o

# Load Test Objects
LTOBJS   = aldtest.o \
	   atm_com_client.o \
	   atm_client_rpc.o

$(SSOBJS)  : atm.h atm_com.h makefile atm_db.h atm_svr_password.h
$(MSOBJS)  : atm.h atm_com.h makefile atm_db.h atm_svr_password.h
$(STSOBJS) : atm.h atm_com.h makefile atm_db.h atm_svr_password.h
$(TSOBJS)  : atm.h atm_com.h makefile atm_db.h atm_svr_password.h
$(TSBOBJS)  : atm.h atm_com.h makefile atm_db.h atm_svr_password.h
$(TSPOBJS)  : atm.h atm_com.h makefile atm_db.h atm_svr_password.h
$(TSCOBJS)  : atm.h atm_com.h makefile atm_db.h atm_svr_password.h
$(TSSIGOBJS)  : atm.h atm_com.h makefile atm_db.h atm_svr_password.h
$(TSPTOOBJS)  : atm.h atm_com.h makefile atm_db.h atm_svr_password.h
$(COBJS)   : atm.h atm_com.h makefile
$(TOBJS)   : atm.h atm_com.h makefile

all        : atm_svr_serial atm_svr_processes atm_svr atm_client aldtest atm_svr_broken atm_svr_cancel atm_svr_1mtx atm_svr_nosync atm_svr_tpool atm_svr_signals

atm_svr_serial : $(SSOBJS)
	${CC} ${CFLAGS} ${LDFLAGS} $(SSOBJS) -o atm_svr_serial

# SVID and XOPEN flags required with GCC on Linux
atm_svr_processes.o: atm_svr_processes.c
	${CC} -c ${CFLAGS} ${SFLAGS} -o atm_svr_processes.o atm_svr_processes.c

atm_svr_processes : $(MSOBJS)
	${CC} ${CFLAGS} ${LDFLAGS} $(MSOBJS) -o atm_svr_processes

atm_svr : $(TSOBJS)
	${CC} ${CFLAGS} ${LDFLAGS_T} $(TSOBJS) -o atm_svr 

atm_svr_1mtx.o : 
	${CC} ${CFLAGS} -c -DSINGLE_DATABASE_MUTEX_SYNC -o atm_svr_1mtx.o atm_svr.c

atm_svr_1mtx : $(TSSMOBJS)
	${CC} ${CFLAGS} ${LDFLAGS_T} $(TSSMOBJS) -o atm_svr_1mtx 

atm_svr_nosync.o : 
	${CC} ${CFLAGS} -c -DNO_DATABASE_SYNC -o atm_svr_nosync.o atm_svr.c

atm_svr_nosync : $(TSNSOBJS)
	${CC} ${CFLAGS} ${LDFLAGS_T} $(TSNSOBJS) -o atm_svr_nosync

atm_svr_broken : $(TSBOBJS)
	${CC} ${CFLAGS} ${LDFLAGS_T} $(TSBOBJS) -o atm_svr_broken 

tpool.o :
	${CC} -c ${CFLAGS} ${LDFLAGS_T} ../tpool/tpool.c -o ./tpool.o 

atm_svr_tpool : $(TSPOBJS)
	${CC} ${CFLAGS} ${LDFLAGS_T} $(TSPOBJS) -o atm_svr_tpool

atm_svr_cancel : $(TSCOBJS)
	${CC} ${CFLAGS} ${LDFLAGS_T} $(TSCOBJS) -o atm_svr_cancel

atm_svr_signals.o : atm_svr_signals.c
	${CC} -c ${NOANSI} -o atm_svr_signals.o atm_svr_signals.c

atm_svr_signals : $(TSSIGOBJS)
	${CC} ${CFLAGS} ${LDFLAGS_T} $(TSSIGOBJS) -o atm_svr_signals

atm_svr_once : $(TSPTOOBJS)
	${CC} ${CFLAGS} ${LDFLAGS_T} $(TSPTOOBJS) -o atm_svr_once

atm_client : $(COBJS)
	${CC} ${CFLAGS} $(COBJS) -o atm_client

aldtest.o : aldtest.c
	${CC} -c -g -Wstrict-prototypes -o aldtest.o aldtest.c

aldtest : $(LTOBJS)
	${CC} ${CFLAGS} $(LTOBJS) -o aldtest

clean :
	rm -f *.o *~ *# core \
	atm_svr_serial atm_svr_processes \
	atm_svr atm_client aldtest .atm_db .atm_pw .atm_conn \
	atm_svr_broken atm_svr_cancel atm_svr_signals atm_svr_tpool \
	atm_svr_once atm_svr_1mtx atm_svr_nosync \
	aldtest
