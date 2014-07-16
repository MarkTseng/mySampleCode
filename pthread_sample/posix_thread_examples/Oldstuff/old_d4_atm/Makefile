#
# Makefile
#
# makefile for automated teller machine (ATM) sr example program
#
# Initially setup for a DEC OSF/1 environment. Use on other platforms
# may require changes to LOADLIBS, CLAGS etc.
#

CC = cc
CFLAGS = -std1
THREAD_LOADLIBS = -threads
DB = adb.o


# Default (Sequential) Sr Objects
DSOBJS   = asr.o \
	   acomsr.o \
	   ${DB} \
	   asrpw.o

# Threaded Sr Objects
TSOBJS   = asrt.o \
	   acomsr.o \
	   ${DB} \
	   asrpw.o

# Multi-Process Sr Objects
MSOBJS   = asrp.o \
	   acomsr.o \
	   ${DB} \
	   asrpw.o

# Client Objects
COBJS    = acl.o \
	   acomcl.o

# Load Test Objects
LTOBJS   = aldtest.o \
	   acomcl.o \
	   aclrpc.o

$(DSOBJS)  : a.h acom.h Makefile adb.h asrpw.h
$(STSOBJS) : a.h acom.h Makefile adb.h asrpw.h
$(TSOBJS)  : a.h acom.h Makefile adb.h asrpw.h
$(MSOBJS)  : a.h acom.h Makefile adb.h asrpw.h
$(COBJS)   : a.h acom.h Makefile
$(TOBJS)   : a.h acom.h Makefile

all        : asr asrt asrp acl aldtest

asr : $(DSOBJS)
	${CC} ${CFLAGS} $(DSOBJS) -o asr

asrt : $(TSOBJS)
	${CC} ${CFLAGS} $(TSOBJS) -o asrt ${THREAD_LOADLIBS}

asrp : $(MSOBJS)
	${CC} ${CFLAGS} $(MSOBJS) -o asrp 

acl : $(COBJS)
	${CC} ${CFLAGS} $(COBJS) -o acl

aldtest : $(LTOBJS)
	${CC} ${CFLAGS} $(LTOBJS) -o aldtest

clean :
	rm -f *.o *~ *# core  \
	asr asrt asrp acl aldtest \
	.atm_db .atm_pw .atm_conn

