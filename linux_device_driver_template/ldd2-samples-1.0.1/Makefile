CFLAGS = -O
SUBDIRS =  misc-progs misc-modules \
           skull scull scullc scullp scullv sbull spull snull\
	   short shortprint pci simple usb allocator
FTPDIR = ../ftp
all: subdirs

subdirs:
	for n in $(SUBDIRS); do $(MAKE) -C $$n || exit 1; done

clean:
	rm -f *.o *~
	for n in $(SUBDIRS); do $(MAKE) -C $$n clean; done

checkthem:
	for n in $(SUBDIRS); do $(MAKE) -C $$n checkthem; done

check:
	for n in $(SUBDIRS); do $(MAKE) -C $$n check; done

objs:
	for n in $(SUBDIRS); do $(MAKE) -C $$n objs; done

