##########################################################
# An example source module to accompany...
#
# "Using POSIX Threads: Programming with Pthreads"
#     by Brad nichols, Dick Buttlar, Jackie Farrell
#     O'Reilly & Associates, Inc.
#
##########################################################
#
# Makefile
#
# makefile for automated teller machine (ATM) DCE example
#
# Initially setup for a Digital UNIX environment. Use on 
# other platforms may require changes to LOADLIBS, CLAGS etc.

CC = cc
CFLAGS = -std1 -g
LOADLIBS = -ldce 
THREADLIBS = -threads

# Threaded Sr Objects
TSSRCS   = atm_svr_dce.o \
	   atm_mgr_dce.o \
	   atm_dce_db.o \
	   atm_svr_password.o \
	   atm_dce_interface_sstub.o

# Client Objects
CSRCS    = atm_client_dce.o \
	   atm_dce_interface_cstub.o

IDEF     = atm_dce_interface.idl

$(TSSRCS)  : atm_dce.h makefile atm_dce_db.h atm_svr_password.h interface
$(CSRCS)   : atm_dce.h makefile interface

all        : atm_svr_dce atm_client_dce

interface : $(IDEF)
	idl $(IDEF)

atm_svr_dce : $(TSSRCS)
	${CC} ${CFLAGS} $(TSSRCS) -o atm_svr_dce ${LOADLIBS} ${THREADLIBS}

atm_client_dce : $(CSRCS)
	${CC} ${CFLAGS} $(CSRCS) -o atm_client_dce ${LOADLIBS} ${THREADLIBS}

clean :
	rm -f *.o *~ *# core  \
	atm_client_dce atm_svr_dce atm_dce_interface.h \
	.atm_db .atm_pw 

