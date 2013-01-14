
# This Makefile has been simplified as much as possible, by putting all
# generic material, independent of this specific directory, into
# ../Rules.make. Read that file for details

TOPDIR  := $(shell cd ..; pwd)
include $(TOPDIR)/Rules.make

CFLAGS += -I.. -O

OBJS = skull.o

all: $(OBJS)

skull.o: skull_init.o skull_clean.o
	$(LD) -r $^ -o $@

install:
	install -d $(INSTALLDIR)
	install -c $(OBJS) $(INSTALLDIR)

clean:
	rm -f *.o *~ core
