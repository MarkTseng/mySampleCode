# mpatrol
# A library for controlling and tracing dynamic memory allocations.
# Copyright (C) 1997-2002 Graeme S. Roy <graeme.roy@analog.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public
# License along with this library; if not, write to the Free
# Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307, USA.


# GNU Makefile to compile on Windows with Microsoft Visual C++


# $Id: Makefile,v 1.70 2002/01/08 20:41:56 graeme Exp $


# Definitions

SHELL = sh
CC    = cl
AR    = lib
LD    = link

SOURCE = ../../src
TOOLS  = ../../tools
VPATH  = $(SOURCE):$(TOOLS)

CFLAGS = -nologo -I$(SOURCE) -I$(TOOLS)
OFLAGS = -Ogt
TFLAGS = -DMP_THREADS_SUPPORT

MPATROL	= mpatrol
MPALLOC	= mpalloc
MPTOOLS	= mptools

ARCHIVE_MPTOBJS	= list.obj tree.obj slots.obj utils.obj memory.obj heap.obj \
		  alloc.obj info.obj stack.obj addr.obj strtab.obj symbol.obj \
		  signals.obj diag.obj option.obj leaktab.obj profile.obj \
		  trace.obj inter.obj malloc.obj cplus.obj sbrk.obj version.obj
SHARED_MPTOBJS	= list.obj tree.obj slots.obj utils.obj memory.obj heap.obj \
		  alloc.obj info.obj stack.obj addr.obj strtab.obj symbol.obj \
		  signals.obj diag.obj option.obj leaktab.obj profile.obj \
		  trace.obj inter.obj malloc.obj cplus.obj sbrk.obj main.obj \
		  version.obj
ARCHTS_MPTOBJS	= list.ts.obj tree.ts.obj slots.ts.obj utils.ts.obj \
		  memory.ts.obj heap.ts.obj alloc.ts.obj info.ts.obj \
		  stack.ts.obj addr.ts.obj strtab.ts.obj symbol.ts.obj \
		  signals.ts.obj mutex.ts.obj diag.ts.obj option.ts.obj \
		  leaktab.ts.obj profile.ts.obj trace.ts.obj inter.ts.obj \
		  malloc.ts.obj cplus.ts.obj sbrk.ts.obj version.ts.obj
SHARTS_MPTOBJS	= list.ts.obj tree.ts.obj slots.ts.obj utils.ts.obj \
		  memory.ts.obj heap.ts.obj alloc.ts.obj info.ts.obj \
		  stack.ts.obj addr.ts.obj strtab.ts.obj symbol.ts.obj \
		  signals.ts.obj mutex.ts.obj diag.ts.obj option.ts.obj \
		  leaktab.ts.obj profile.ts.obj trace.ts.obj inter.ts.obj \
		  malloc.ts.obj cplus.ts.obj sbrk.ts.obj main.ts.obj \
		  version.ts.obj
ARCHIVE_MPAOBJS	= mpalloc.obj mpfailure.obj mpsetfail.obj mpmalloc.obj \
		  mpcalloc.obj mpstrdup.obj mprealloc.obj
SHARED_MPAOBJS	= mpalloc.obj mpfailure.obj mpsetfail.obj mpmalloc.obj \
		  mpcalloc.obj mpstrdup.obj mprealloc.obj main2.obj
ARCHIVE_MPLOBJS	= dbmalloc.obj dmalloc.obj heapdiff.obj mgauge.obj mtrace.obj

ARCHIVE_MPATROL	= lib$(MPATROL).lib
SHARED_MPATROL	= $(MPATROL).dll
OBJECT_MPATROL	= lib$(MPATROL).obj
ARCHTS_MPATROL	= lib$(MPATROL)mt.lib
SHARTS_MPATROL	= $(MPATROL)mt.dll
OBJTS_MPATROL	= lib$(MPATROL)mt.obj

ARCHIVE_MPALLOC	= lib$(MPALLOC).lib
SHARED_MPALLOC	= $(MPALLOC).dll
OBJECT_MPALLOC	= lib$(MPALLOC).obj

ARCHIVE_MPTOOLS	= lib$(MPTOOLS).lib


# Rules

.SUFFIXES: .c .obj .ts.obj

.c.obj:
	$(CC) $(CFLAGS) $(OFLAGS) -TP -c $<

.c.ts.obj:
	$(CC) $(CFLAGS) $(OFLAGS) $(TFLAGS) -TP -c -Fo$@ $<

$(ARCHIVE_MPATROL): $(ARCHIVE_MPTOBJS)
	$(AR) -nologo -out:$@ $(ARCHIVE_MPTOBJS)

$(SHARED_MPATROL): $(SHARED_MPTOBJS) $(MPATROL).def
	$(LD) -dll -nologo -def:$(MPATROL).def -out:$@ $(SHARED_MPTOBJS) -defaultlib:imagehlp

$(OBJECT_MPATROL): $(SOURCE)/library.c
	$(CC) $(CFLAGS) $(OFLAGS) -TP -c -Fo$@ $<

$(ARCHTS_MPATROL): $(ARCHTS_MPTOBJS)
	$(AR) -nologo -out:$@ $(ARCHTS_MPTOBJS)

$(SHARTS_MPATROL): $(SHARTS_MPTOBJS) $(MPATROL)mt.def
	$(LD) -dll -nologo -def:$(MPATROL)mt.def -out:$@ $(SHARTS_MPTOBJS) -defaultlib:imagehlp

$(OBJTS_MPATROL): $(SOURCE)/library.c
	$(CC) $(CFLAGS) $(OFLAGS) $(TFLAGS) -TP -c -Fo$@ $<

$(ARCHIVE_MPALLOC): $(ARCHIVE_MPAOBJS)
	$(AR) -nologo -out:$@ $(ARCHIVE_MPAOBJS)

$(SHARED_MPALLOC): $(SHARED_MPAOBJS) $(MPALLOC).def
	$(LD) -dll -nologo -def:$(MPALLOC).def -out:$@ $(SHARED_MPAOBJS)

$(OBJECT_MPALLOC): $(SOURCE)/library2.c
	$(CC) $(CFLAGS) $(OFLAGS) -TP -c -Fo$@ $<

$(ARCHIVE_MPTOOLS): $(ARCHIVE_MPLOBJS)
	$(AR) -nologo -out:$@ $(ARCHIVE_MPLOBJS)

mpatrol.exe: mpatrol.obj getopt.obj version.obj
	$(CC) $(CFLAGS) $(OFLAGS) mpatrol.obj getopt.obj version.obj

mprof.exe: mprof.obj graph.obj getopt.obj list.obj tree.obj version.obj
	$(CC) $(CFLAGS) $(OFLAGS) mprof.obj graph.obj getopt.obj list.obj tree.obj version.obj

mptrace.exe: mptrace.obj getopt.obj tree.obj slots.obj utils.obj version.obj
	$(CC) $(CFLAGS) $(OFLAGS) mptrace.obj getopt.obj tree.obj slots.obj utils.obj version.obj

mleak.exe: mleak.obj getopt.obj tree.obj version.obj
	$(CC) $(CFLAGS) $(OFLAGS) mleak.obj getopt.obj tree.obj version.obj

all: $(ARCHIVE_MPATROL) $(SHARED_MPATROL) $(ARCHTS_MPATROL) $(SHARTS_MPATROL) \
	$(ARCHIVE_MPALLOC) $(SHARED_MPALLOC) $(ARCHIVE_MPTOOLS) \
	mpatrol.exe mprof.exe mptrace.exe mleak.exe

clean:
	-@ rm -f $(ARCHIVE_MPTOBJS) $(ARCHTS_MPTOBJS) 2>&1 >/dev/null
	-@ rm -f $(SHARED_MPTOBJS) $(SHARTS_MPTOBJS) 2>&1 >/dev/null
	-@ rm -f $(ARCHIVE_MPAOBJS) $(SHARED_MPAOBJS) 2>&1 >/dev/null
	-@ rm -f $(ARCHIVE_MPLOBJS) 2>&1 >/dev/null
	-@ rm -f mpatrol.obj mprof.obj mptrace.obj mleak.obj graph.obj getopt.obj 2>&1 >/dev/null

clobber: clean
	-@ rm -f $(ARCHIVE_MPATROL) $(ARCHTS_MPATROL) 2>&1 >/dev/null
	-@ rm -f $(SHARED_MPATROL) $(SHARTS_MPATROL) 2>&1 >/dev/null
	-@ rm -f $(MPATROL).lib $(MPATROL).exp 2>&1 >/dev/null
	-@ rm -f $(MPATROL)mt.lib $(MPATROL)mt.exp 2>&1 >/dev/null
	-@ rm -f $(OBJECT_MPATROL) $(OBJTS_MPATROL) 2>&1 >/dev/null
	-@ rm -f $(ARCHIVE_MPALLOC) $(SHARED_MPALLOC) 2>&1 >/dev/null
	-@ rm -f $(MPALLOC).lib $(MPALLOC).exp 2>&1 >/dev/null
	-@ rm -f $(OBJECT_MPALLOC) 2>&1 >/dev/null
	-@ rm -f $(ARCHIVE_MPTOOLS) 2>&1 >/dev/null
	-@ rm -f mpatrol.exe mprof.exe mptrace.exe mleak.exe 2>&1 >/dev/null


# Dependencies

list.obj list.ts.obj: $(SOURCE)/list.c $(SOURCE)/list.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
tree.obj tree.ts.obj: $(SOURCE)/tree.c $(SOURCE)/tree.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
slots.obj slots.ts.obj: $(SOURCE)/slots.c $(SOURCE)/slots.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/utils.h
utils.obj utils.ts.obj: $(SOURCE)/utils.c $(SOURCE)/utils.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
memory.obj memory.ts.obj: $(SOURCE)/memory.c $(SOURCE)/memory.h \
	$(SOURCE)/config.h $(SOURCE)/target.h $(SOURCE)/stack.h \
	$(SOURCE)/utils.h
heap.obj heap.ts.obj: $(SOURCE)/heap.c $(SOURCE)/heap.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/memory.h $(SOURCE)/slots.h \
	$(SOURCE)/tree.h $(SOURCE)/trace.h $(SOURCE)/utils.h
alloc.obj alloc.ts.obj: $(SOURCE)/alloc.c $(SOURCE)/alloc.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/heap.h $(SOURCE)/memory.h \
	$(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h $(SOURCE)/utils.h
info.obj: $(SOURCE)/info.c $(SOURCE)/info.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/addr.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/stack.h $(SOURCE)/alloc.h $(SOURCE)/signals.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/diag.h \
	$(SOURCE)/utils.h $(SOURCE)/profile.h $(SOURCE)/trace.h \
	$(SOURCE)/leaktab.h
info.ts.obj: $(SOURCE)/info.c $(SOURCE)/info.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/addr.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/stack.h $(SOURCE)/alloc.h $(SOURCE)/signals.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/diag.h \
	$(SOURCE)/mutex.h $(SOURCE)/utils.h $(SOURCE)/profile.h \
	$(SOURCE)/trace.h $(SOURCE)/leaktab.h
stack.obj stack.ts.obj: $(SOURCE)/stack.c $(SOURCE)/stack.h $(SOURCE)/memory.h \
	$(SOURCE)/machine.h $(SOURCE)/config.h $(SOURCE)/target.h
addr.obj addr.ts.obj: $(SOURCE)/addr.c $(SOURCE)/addr.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/heap.h $(SOURCE)/memory.h \
	$(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h $(SOURCE)/stack.h \
	$(SOURCE)/utils.h
strtab.obj strtab.ts.obj: $(SOURCE)/strtab.c $(SOURCE)/strtab.h \
	$(SOURCE)/config.h $(SOURCE)/target.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/utils.h
symbol.obj symbol.ts.obj: $(SOURCE)/symbol.c $(SOURCE)/symbol.h \
	$(SOURCE)/config.h $(SOURCE)/target.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h \
	$(SOURCE)/strtab.h $(SOURCE)/diag.h $(SOURCE)/info.h $(SOURCE)/addr.h \
	$(SOURCE)/list.h $(SOURCE)/stack.h $(SOURCE)/alloc.h \
	$(SOURCE)/signals.h $(SOURCE)/utils.h $(SOURCE)/profile.h \
	$(SOURCE)/trace.h $(SOURCE)/leaktab.h
signals.obj: $(SOURCE)/signals.c $(SOURCE)/signals.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/diag.h $(SOURCE)/info.h $(SOURCE)/addr.h \
	$(SOURCE)/heap.h $(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h \
	$(SOURCE)/list.h $(SOURCE)/stack.h $(SOURCE)/alloc.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/inter.h \
	$(SOURCE)/profile.h $(SOURCE)/trace.h $(SOURCE)/leaktab.h
signals.ts.obj: $(SOURCE)/signals.c $(SOURCE)/signals.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/diag.h $(SOURCE)/info.h $(SOURCE)/addr.h \
	$(SOURCE)/heap.h $(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h \
	$(SOURCE)/list.h $(SOURCE)/stack.h $(SOURCE)/alloc.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/inter.h \
	$(SOURCE)/profile.h $(SOURCE)/trace.h $(SOURCE)/leaktab.h \
	$(SOURCE)/mutex.h
mutex.ts.obj: $(SOURCE)/mutex.c $(SOURCE)/mutex.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
diag.obj: $(SOURCE)/diag.c $(SOURCE)/diag.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/stack.h $(SOURCE)/alloc.h $(SOURCE)/signals.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/utils.h \
	$(SOURCE)/profile.h $(SOURCE)/trace.h $(SOURCE)/leaktab.h \
	$(SOURCE)/version.h
diag.ts.obj: $(SOURCE)/diag.c $(SOURCE)/diag.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/stack.h $(SOURCE)/alloc.h $(SOURCE)/signals.h \
	$(SOURCE)/mutex.h $(SOURCE)/symbol.h $(SOURCE)/strtab.h \
	$(SOURCE)/utils.h $(SOURCE)/profile.h $(SOURCE)/trace.h \
	$(SOURCE)/leaktab.h $(SOURCE)/version.h
option.obj option.ts.obj: $(SOURCE)/option.c $(SOURCE)/option.h \
	$(SOURCE)/config.h $(SOURCE)/target.h $(SOURCE)/info.h \
	$(SOURCE)/addr.h $(SOURCE)/heap.h $(SOURCE)/memory.h $(SOURCE)/slots.h \
	$(SOURCE)/tree.h $(SOURCE)/list.h $(SOURCE)/stack.h $(SOURCE)/alloc.h \
	$(SOURCE)/signals.h $(SOURCE)/symbol.h $(SOURCE)/strtab.h \
	$(SOURCE)/diag.h $(SOURCE)/utils.h $(SOURCE)/profile.h \
	$(SOURCE)/trace.h $(SOURCE)/leaktab.h
leaktab.obj leaktab.ts.obj: $(SOURCE)/leaktab.c $(SOURCE)/leaktab.h \
	$(SOURCE)/config.h $(SOURCE)/target.h $(SOURCE)/list.h \
	$(SOURCE)/heap.h $(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h \
	$(SOURCE)/utils.h
profile.obj profile.ts.obj: $(SOURCE)/profile.c $(SOURCE)/profile.h \
	$(SOURCE)/config.h $(SOURCE)/target.h $(SOURCE)/alloc.h \
	$(SOURCE)/heap.h $(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h \
	$(SOURCE)/list.h $(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/stack.h \
	$(SOURCE)/signals.h $(SOURCE)/symbol.h $(SOURCE)/strtab.h \
	$(SOURCE)/diag.h $(SOURCE)/utils.h $(SOURCE)/trace.h \
	$(SOURCE)/leaktab.h $(SOURCE)/version.h
trace.obj trace.ts.obj: $(SOURCE)/trace.c $(SOURCE)/trace.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/memory.h $(SOURCE)/diag.h \
	$(SOURCE)/info.h $(SOURCE)/alloc.h $(SOURCE)/heap.h $(SOURCE)/slots.h \
	$(SOURCE)/tree.h $(SOURCE)/list.h $(SOURCE)/addr.h $(SOURCE)/stack.h \
	$(SOURCE)/profile.h $(SOURCE)/symbol.h $(SOURCE)/strtab.h \
	$(SOURCE)/signals.h $(SOURCE)/leaktab.h $(SOURCE)/utils.h \
	$(SOURCE)/version.h
inter.obj: $(SOURCE)/inter.c $(SOURCE)/inter.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/stack.h $(SOURCE)/alloc.h $(SOURCE)/signals.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/diag.h \
	$(SOURCE)/option.h $(SOURCE)/sbrk.h $(SOURCE)/profile.h \
	$(SOURCE)/trace.h $(SOURCE)/leaktab.h $(SOURCE)/machine.h \
	$(SOURCE)/version.h
inter.ts.obj: $(SOURCE)/inter.c $(SOURCE)/inter.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/stack.h $(SOURCE)/alloc.h $(SOURCE)/signals.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/diag.h \
	$(SOURCE)/mutex.h $(SOURCE)/option.h $(SOURCE)/sbrk.h \
	$(SOURCE)/profile.h $(SOURCE)/trace.h $(SOURCE)/leaktab.h \
	$(SOURCE)/machine.h $(SOURCE)/version.h
malloc.obj malloc.ts.obj: $(SOURCE)/malloc.c $(SOURCE)/inter.h \
	$(SOURCE)/config.h $(SOURCE)/target.h $(SOURCE)/info.h \
	$(SOURCE)/addr.h $(SOURCE)/heap.h $(SOURCE)/memory.h $(SOURCE)/slots.h \
	$(SOURCE)/tree.h $(SOURCE)/list.h $(SOURCE)/stack.h $(SOURCE)/alloc.h \
	$(SOURCE)/signals.h $(SOURCE)/symbol.h $(SOURCE)/strtab.h \
	$(SOURCE)/profile.h $(SOURCE)/trace.h $(SOURCE)/leaktab.h \
	$(SOURCE)/diag.h
cplus.obj: $(SOURCE)/cplus.c $(SOURCE)/inter.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/stack.h $(SOURCE)/alloc.h $(SOURCE)/signals.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/profile.h \
	$(SOURCE)/trace.h $(SOURCE)/diag.h $(SOURCE)/leaktab.h
cplus.ts.obj: $(SOURCE)/cplus.c $(SOURCE)/inter.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/stack.h $(SOURCE)/alloc.h $(SOURCE)/signals.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/profile.h \
	$(SOURCE)/trace.h $(SOURCE)/diag.h $(SOURCE)/leaktab.h $(SOURCE)/mutex.h
sbrk.obj sbrk.ts.obj: $(SOURCE)/sbrk.c $(SOURCE)/sbrk.h $(SOURCE)/config.h \
        $(SOURCE)/target.h $(SOURCE)/memory.h $(SOURCE)/utils.h
main.obj main.ts.obj: main.c $(SOURCE)/inter.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/stack.h $(SOURCE)/alloc.h $(SOURCE)/signals.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/profile.h \
	$(SOURCE)/trace.h $(SOURCE)/leaktab.h $(SOURCE)/diag.h
version.obj version.ts.obj: $(SOURCE)/version.c $(SOURCE)/version.h \
	$(SOURCE)/config.h $(SOURCE)/target.h
mpalloc.obj: $(SOURCE)/mpalloc.c $(SOURCE)/inter.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/info.h $(SOURCE)/alloc.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/addr.h $(SOURCE)/stack.h $(SOURCE)/profile.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/trace.h \
	$(SOURCE)/leaktab.h $(SOURCE)/signals.h $(SOURCE)/diag.h \
	$(SOURCE)/mpalloc.h $(SOURCE)/version.h
mpfailure.obj: $(SOURCE)/mpfailure.c $(SOURCE)/mpalloc.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
mpsetfail.obj: $(SOURCE)/mpsetfail.c $(SOURCE)/mpalloc.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
mpmalloc.obj: $(SOURCE)/mpmalloc.c $(SOURCE)/mpalloc.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
mpcalloc.obj: $(SOURCE)/mpcalloc.c $(SOURCE)/mpalloc.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
mpstrdup.obj: $(SOURCE)/mpstrdup.c $(SOURCE)/mpalloc.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
mprealloc.obj: $(SOURCE)/mprealloc.c $(SOURCE)/mpalloc.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
main2.obj: main2.c $(SOURCE)/config.h $(SOURCE)/target.h
$(OBJECT_MPATROL): $(SOURCE)/library.c $(SOURCE)/config.h $(SOURCE)/target.h \
	$(SOURCE)/list.c $(SOURCE)/list.h $(SOURCE)/tree.c $(SOURCE)/tree.h \
	$(SOURCE)/slots.c $(SOURCE)/slots.h $(SOURCE)/utils.h \
	$(SOURCE)/utils.c $(SOURCE)/memory.c $(SOURCE)/memory.h \
	$(SOURCE)/stack.h $(SOURCE)/heap.c $(SOURCE)/heap.h $(SOURCE)/alloc.c \
	$(SOURCE)/alloc.h $(SOURCE)/info.c $(SOURCE)/info.h $(SOURCE)/addr.h \
	$(SOURCE)/signals.h $(SOURCE)/symbol.h $(SOURCE)/strtab.h \
	$(SOURCE)/diag.h $(SOURCE)/stack.c $(SOURCE)/addr.c $(SOURCE)/strtab.c \
	$(SOURCE)/symbol.c $(SOURCE)/signals.c $(SOURCE)/inter.h \
	$(SOURCE)/diag.c $(SOURCE)/version.h $(SOURCE)/option.c \
	$(SOURCE)/option.h $(SOURCE)/inter.c $(SOURCE)/malloc.c \
	$(SOURCE)/cplus.c $(SOURCE)/sbrk.h $(SOURCE)/sbrk.c \
	$(SOURCE)/leaktab.c $(SOURCE)/leaktab.h $(SOURCE)/profile.c \
	$(SOURCE)/profile.h $(SOURCE)/trace.c $(SOURCE)/trace.h \
	$(SOURCE)/machine.h $(SOURCE)/version.c
$(OBJTS_MPATROL): $(SOURCE)/library.c $(SOURCE)/config.h $(SOURCE)/target.h \
	$(SOURCE)/list.c $(SOURCE)/list.h $(SOURCE)/tree.c $(SOURCE)/tree.h \
	$(SOURCE)/slots.c $(SOURCE)/slots.h $(SOURCE)/utils.h \
	$(SOURCE)/utils.c $(SOURCE)/memory.c $(SOURCE)/memory.h \
	$(SOURCE)/stack.h $(SOURCE)/heap.c $(SOURCE)/heap.h $(SOURCE)/alloc.c \
	$(SOURCE)/alloc.h $(SOURCE)/info.c $(SOURCE)/info.h $(SOURCE)/addr.h \
	$(SOURCE)/signals.h $(SOURCE)/symbol.h $(SOURCE)/strtab.h \
	$(SOURCE)/diag.h $(SOURCE)/mutex.h $(SOURCE)/stack.c $(SOURCE)/addr.c \
	$(SOURCE)/strtab.c $(SOURCE)/symbol.c $(SOURCE)/signals.c \
	$(SOURCE)/inter.h $(SOURCE)/mutex.c $(SOURCE)/diag.c \
	$(SOURCE)/version.h $(SOURCE)/option.c $(SOURCE)/option.h \
	$(SOURCE)/inter.c $(SOURCE)/malloc.c $(SOURCE)/cplus.c \
	$(SOURCE)/sbrk.h $(SOURCE)/sbrk.c $(SOURCE)/leaktab.c \
	$(SOURCE)/leaktab.h $(SOURCE)/profile.c $(SOURCE)/profile.h \
	$(SOURCE)/trace.c $(SOURCE)/trace.h $(SOURCE)/machine.h \
	$(SOURCE)/version.c
$(OBJECT_MPALLOC): $(SOURCE)/library2.c $(SOURCE)/config.h $(SOURCE)/target.h \
	$(SOURCE)/mpalloc.c $(SOURCE)/inter.h $(SOURCE)/info.h \
	$(SOURCE)/alloc.h $(SOURCE)/heap.h $(SOURCE)/memory.h \
	$(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h $(SOURCE)/addr.h \
	$(SOURCE)/stack.h $(SOURCE)/profile.h $(SOURCE)/symbol.h \
	$(SOURCE)/strtab.h $(SOURCE)/trace.h $(SOURCE)/signals.h \
	$(SOURCE)/diag.h $(SOURCE)/leaktab.h $(SOURCE)/mpalloc.h \
	$(SOURCE)/mpfailure.c $(SOURCE)/mpsetfail.c $(SOURCE)/mpmalloc.c \
	$(SOURCE)/mpcalloc.c $(SOURCE)/mpstrdup.c $(SOURCE)/mprealloc.c
mpatrol.obj: $(SOURCE)/mpatrol.c $(SOURCE)/getopt.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/version.h
mprof.obj: $(SOURCE)/mprof.c $(SOURCE)/getopt.h $(SOURCE)/tree.h \
	$(SOURCE)/graph.h $(SOURCE)/list.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/version.h
mptrace.obj: $(SOURCE)/mptrace.c $(SOURCE)/getopt.h $(SOURCE)/tree.h \
	$(SOURCE)/slots.h $(SOURCE)/config.h $(SOURCE)/target.h \
	$(SOURCE)/utils.h $(SOURCE)/version.h
mleak.obj: $(SOURCE)/mleak.c $(SOURCE)/getopt.h $(SOURCE)/tree.h \
	$(SOURCE)/config.h $(SOURCE)/target.h $(SOURCE)/version.h
graph.obj: $(SOURCE)/graph.c $(SOURCE)/graph.h $(SOURCE)/list.h \
	$(SOURCE)/config.h $(SOURCE)/target.h
getopt.obj: $(SOURCE)/getopt.c $(SOURCE)/getopt.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
dbmalloc.obj: $(TOOLS)/dbmalloc.c $(TOOLS)/dbmalloc.h $(SOURCE)/mpatrol.h \
	$(SOURCE)/config.h $(SOURCE)/target.h
dmalloc.obj: $(TOOLS)/dmalloc.c $(TOOLS)/dmalloc.h $(SOURCE)/mpatrol.h \
	$(SOURCE)/config.h $(SOURCE)/target.h
heapdiff.obj: $(TOOLS)/heapdiff.c $(TOOLS)/heapdiff.h $(SOURCE)/mpatrol.h \
	$(SOURCE)/config.h $(SOURCE)/target.h
mgauge.obj: $(TOOLS)/mgauge.c $(TOOLS)/mgauge.h $(SOURCE)/mpatrol.h \
	$(SOURCE)/config.h $(SOURCE)/target.h
mtrace.obj: $(TOOLS)/mtrace.c $(TOOLS)/mtrace.h $(SOURCE)/mpatrol.h \
	$(SOURCE)/config.h $(SOURCE)/target.h
