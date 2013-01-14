# Comment/uncomment the following line to enable/disable debugging
#DEBUG = y

# This Makefile has been simplified as much as possible, by putting all
# generic material, independent of this specific directory, into
# ../Rules.make. Read that file for details

TOPDIR  := $(shell cd ..; pwd)
include $(TOPDIR)/Rules.make

# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DJIT_DEBUG -DJIQ_DEBUG -DALL_DEBUG
else
  DEBFLAGS = -O2
endif

CFLAGS += $(DEBFLAGS) -I..

GENKSYMS = /sbin/genksyms
ifdef CONFIG_SMP
	GENKSYMS += -p smp_
endif



OBJS = faulty.o sleepy.o silly.o slave.o slaveD.o slaveH.o import.o \
	master.o export.o hello.o \
	jit.o jiq.o inter.o \
	kdatasize.o kdataalign.o

SRC = $(OBJS:.o:.c)

# if compiling a partial list, use a different list of objects
ifdef LDD_PARTIAL        
OBJS = hello.o faulty.o sleepy.o jit.o jiq.o kdatasize.o kdataalign.o
endif


all: $(OBJS)

clean:
	rm -f *.o *.ver *~ 

# Don't use -Wall here: the code is silly by design
hello.o: hello.c
	$(CC) -D__KERNEL__ -I$(INCLUDEDIR) -c $^ -o $@


#FIXME: the 2.4.0 below is plain wrong :)
ifdef CONFIG_MODVERSIONS
export.o import.o: export.ver
endif

export.ver: export.c
	$(CC) -I$(INCLUDEDIR) $(CFLAGS) -E -D__GENKSYMS__ $^ | \
		$(GENKSYMS) -k 2.4.0 > $@


