CFLAGS =  -DMLX_LINUX -DMLX_WITH_LIBDW -I/home/mark/myResearch/elfutils-0.165/ARM_LIBS/include/elfutils -I/home/mark/myResearch/elfutils-0.165/ARM_LIBS/include -g -O2 -Wall -I/home/mark/myResearch/libunwind/LIBUNWIND_ARM/include 
LDLIBS =  -L/home/mark/myResearch/libunwind/LIBUNWIND_ARM/lib -L/home/mark/myResearch/elfutils-0.165/ARM_LIBS/lib -L/home/mark/myResearch/elfutils-0.165/ARM_LIBS/lib/elfutils -L/home/mark/nfs/ARM_LIBS/lib -lunwind-ptrace -lunwind -lunwind-arm -lelf -ldw -lz 
LDFLAGS = 
PREFIX = /usr
DESTDIR =
CC = arm-linux-gnueabihf-gcc
TARGET = memleax

SOURCES = debug_file.c debug_line.c memleax.c ptr_backtrace.c proc_info.c symtab.c addr_maps.c
OBJS = debug_file.o debug_line.o memleax.o ptr_backtrace.o   proc_info.o symtab.o addr_maps.o

$(TARGET) : $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(LDLIBS)

clean :
	rm -f $(OBJS) $(TARGET)

install :
	mkdir -p $(DESTDIR)$(PREFIX)/bin/
	install $(TARGET) $(DESTDIR)$(PREFIX)/bin/
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man1/
	install -m 644 memleax.1 $(DESTDIR)$(PREFIX)/share/man/man1/
uninstall :
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	rm -f $(DESTDIR)$(PREFIX)/share/man/man1/memleax.1

debug_file.o: debug_file.c debug_file.h memleax.h
debug_line.o: debug_line.c debug_line.h array.h memleax.h proc_info.h \
 debug_file.h /usr/include/elfutils/libdw.h
memleax.o: memleax.c \
 hash.h list.h ptr_backtrace.h symtab.h debug_line.h proc_info.h \
 addr_maps.h memleax.h
ptr_backtrace.o: ptr_backtrace.c ptr_backtrace.h proc_info.h list.h \
 hash.h memleax.h ptrace_utils.h
proc_info.o: proc_info.c proc_info.h
symtab.o: symtab.c array.h memleax.h proc_info.h debug_file.h
addr_maps.o: addr_maps.c addr_maps.h proc_info.h array.h
