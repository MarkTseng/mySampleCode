
# This Makefile has been simplified as much as possible, by putting all
# generic material, independent of this specific directory, into
# ../Rules.make. Read that file for details

TOPDIR  := $(shell cd ..; pwd)
include $(TOPDIR)/Rules.make

CFLAGS += -O2 -I..

# To please automatic compilation, no TARGET is there, only OBJS
OBJS = pcidata.o pciregions.o
SRC = $(OBJS:.o=.c)
EXE    = pcidump
EXESRC = pcidump.c

all: .depend $(EXE) $(OBJS)

clean:
	rm -f *.o *~ $(EXE) .depend

# Compile pcidump without kernel-related headers (it will break)
# (for user space we want use `cc')
pcidump: pcidump.c
	cc $^ -o $@

# in make depend make the same split
# (for user space we want use `cc')
depend .depend dep:
	$(CC) $(CFLAGS) -M $(SRC) > $@
	cc -M $(EXESRC) >> $@


ifeq (.depend,$(wildcard .depend))
include .depend
endif


