#CFLAGS =  -DMLX_LINUX -DMLX_WITH_LIBDW -I/home/mark/myResearch/elfutils-0.165/ARM_LIBS/include/elfutils -I/home/mark/myResearch/elfutils-0.165/ARM_LIBS/include -g -O2 -Wall -I/home/mark/myResearch/libunwind/LIBUNWIND_ARM/include 
#LDLIBS =  -L/home/mark/myResearch/libunwind/LIBUNWIND_ARM/lib -L/home/mark/myResearch/elfutils-0.165/ARM_LIBS/lib -L/home/mark/myResearch/elfutils-0.165/ARM_LIBS/lib/elfutils -L/home/mark/nfs/ARM_LIBS/lib -lunwind-ptrace -lunwind -lunwind-arm -lelf -ldw -lz -lreadline
CFLAGS =  -DMLX_LINUX -DMLX_WITH_LIBDW -I/usr/include/elfutils/ -g -O2 -Wall -DX86_64
LDLIBS =  -lunwind-ptrace -lunwind -lunwind-x86_64 -lelf -ldw -lreadline
LDFLAGS = 
PREFIX = /usr
DESTDIR =
#CC = arm-linux-gnueabihf-gcc
CC = gcc
TARGET = minigdb

SOURCES = breakpoint.c debug_file.c debug_line.c minigdb.c ptr_backtrace.c callstack.c memblock.c proc_info.c symtab.c addr_maps.c
OBJS = breakpoint.o debug_file.o debug_line.o minigdb.o ptr_backtrace.o callstack.o memblock.o proc_info.o symtab.o addr_maps.o

$(TARGET) : $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(LDLIBS)
	$(CC) -o test test.c

clean :
	rm -f $(OBJS) $(TARGET)

install :
	mkdir -p $(DESTDIR)$(PREFIX)/bin/
	install $(TARGET) $(DESTDIR)$(PREFIX)/bin/
uninstall :
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)

breakpoint.o: breakpoint.c hash.h list.h ptr_backtrace.h \
	 memblock.h breakpoint.h ptrace_utils.h symtab.h minigdb.h
debug_file.o: debug_file.c debug_file.h minigdb.h
debug_line.o: debug_line.c debug_line.h array.h minigdb.h proc_info.h \
 debug_file.h /usr/include/elfutils/libdw.h
minigdb.o: minigdb.c \
 hash.h list.h ptr_backtrace.h symtab.h debug_line.h proc_info.h \
 addr_maps.h minigdb.h
ptr_backtrace.o: ptr_backtrace.c ptr_backtrace.h proc_info.h list.h \
 hash.h minigdb.h ptrace_utils.h
callstack.o: callstack.c hash.h list.h array.h symtab.h callstack.h \
	 ptr_backtrace.h addr_maps.h debug_line.h minigdb.h memblock.h
memblock.o: memblock.c hash.h list.h memblock.h callstack.h \
		 ptr_backtrace.h
proc_info.o: proc_info.c proc_info.h
symtab.o: symtab.c array.h minigdb.h proc_info.h debug_file.h
addr_maps.o: addr_maps.c addr_maps.h proc_info.h array.h
