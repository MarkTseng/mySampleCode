# mpatrol
# A library for controlling and tracing dynamic memory allocations.
# Copyright (C) 1997-2008 Graeme S. Roy <graemeroy@users.sourceforge.net>
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or (at
# your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
# General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


# GNU Makefile to compile on AmigaOS with GNU C++


# $Id$


# Definitions

CC = g++
AR = ar

SOURCE = ../../src
TOOLS  = ../../tools
VPATH  = $(SOURCE):$(TOOLS)

CFLAGS = -I$(SOURCE) -I$(TOOLS)
OFLAGS = -O3 -fno-inline-functions
TFLAGS = -DMP_THREADS_SUPPORT

MPATROL	= mpatrol
MPALLOC	= mpalloc
MPTOOLS	= mptools

ARCHIVE_MPTOBJS	= list.o tree.o slots.o utils.o memory.o heap.o alloc.o info.o \
		  stack.o addr.o strtab.o symbol.o signals.o diag.o option.o \
		  leaktab.o profile.o trace.o inter.o malloc.o cplus.o sbrk.o \
		  version.o
ARCHTS_MPTOBJS	= list.t.o tree.t.o slots.t.o utils.t.o memory.t.o heap.t.o \
		  alloc.t.o info.t.o stack.t.o addr.t.o strtab.t.o symbol.t.o \
		  signals.t.o mutex.t.o diag.t.o option.t.o leaktab.t.o \
		  profile.t.o trace.t.o inter.t.o malloc.t.o cplus.t.o \
		  sbrk.t.o version.t.o
ARCHIVE_MPAOBJS	= mpalloc.o mpfailure.o mpsetfail.o mpmalloc.o mpcalloc.o \
		  mpstrdup.o mprealloc.o
ARCHIVE_MPLOBJS	= dbmalloc.o dmalloc.o heapdiff.o mgauge.o mtrace.o

ARCHIVE_MPATROL	= lib$(MPATROL).a
OBJECT_MPATROL	= lib$(MPATROL).o
ARCHTS_MPATROL	= lib$(MPATROL)mt.a
OBJTS_MPATROL	= lib$(MPATROL)mt.o

ARCHIVE_MPALLOC	= lib$(MPALLOC).a
OBJECT_MPALLOC	= lib$(MPALLOC).o

ARCHIVE_MPTOOLS	= lib$(MPTOOLS).a


# Rules

.SUFFIXES: .c .o .t.o

.c.o:
	$(CC) $(CFLAGS) $(OFLAGS) -c $<

.c.t.o:
	$(CC) $(CFLAGS) $(OFLAGS) $(TFLAGS) -c -o $@ $<

$(ARCHIVE_MPATROL): $(ARCHIVE_MPTOBJS)
	$(AR) cr $@ $(ARCHIVE_MPTOBJS)

$(OBJECT_MPATROL): $(SOURCE)/library.c
	$(CC) $(CFLAGS) $(OFLAGS) -c -o $@ $<

$(ARCHTS_MPATROL): $(ARCHTS_MPTOBJS)
	$(AR) cr $@ $(ARCHTS_MPTOBJS)

$(OBJTS_MPATROL): $(SOURCE)/library.c
	$(CC) $(CFLAGS) $(OFLAGS) $(TFLAGS) -c -o $@ $<

$(ARCHIVE_MPALLOC): $(ARCHIVE_MPAOBJS)
	$(AR) cr $@ $(ARCHIVE_MPAOBJS)

$(OBJECT_MPALLOC): $(SOURCE)/library2.c
	$(CC) $(CFLAGS) $(OFLAGS) -c -o $@ $<

$(ARCHIVE_MPTOOLS): $(ARCHIVE_MPLOBJS)
	$(AR) cr $@ $(ARCHIVE_MPLOBJS)

mpatrol: mpatrol.o getopt.o version.o
	$(CC) $(CFLAGS) $(OFLAGS) -o $@ mpatrol.o getopt.o version.o

mprof: mprof.o graph.o getopt.o list.o tree.o version.o
	$(CC) $(CFLAGS) $(OFLAGS) -o $@ mprof.o graph.o getopt.o list.o tree.o version.o

mptrace: mptrace.o getopt.o tree.o slots.o utils.o version.o
	$(CC) $(CFLAGS) $(OFLAGS) -o $@ mptrace.o getopt.o tree.o slots.o utils.o version.o

mleak: mleak.o getopt.o tree.o version.o
	$(CC) $(CFLAGS) $(OFLAGS) -o $@ mleak.o getopt.o tree.o version.o

all: $(ARCHIVE_MPATROL) $(ARCHTS_MPATROL) $(ARCHIVE_MPALLOC) \
	$(ARCHIVE_MPTOOLS) mpatrol mprof mptrace mleak

clean:
	-@ rm -f $(ARCHIVE_MPTOBJS) $(ARCHTS_MPTOBJS) 2>&1 >/dev/null
	-@ rm -f $(ARCHIVE_MPAOBJS) $(ARCHIVE_MPLOBJS) 2>&1 >/dev/null
	-@ rm -f mpatrol.o mprof.o mptrace.o mleak.o graph.o getopt.o 2>&1 >/dev/null

clobber: clean
	-@ rm -f $(ARCHIVE_MPATROL) $(ARCHTS_MPATROL) 2>&1 >/dev/null
	-@ rm -f $(OBJECT_MPATROL) $(OBJTS_MPATROL) 2>&1 >/dev/null
	-@ rm -f $(ARCHIVE_MPALLOC) $(OBJECT_MPALLOC) 2>&1 >/dev/null
	-@ rm -f $(ARCHIVE_MPTOOLS) 2>&1 >/dev/null
	-@ rm -f mpatrol mprof mptrace mleak 2>&1 >/dev/null


# Dependencies

list.o list.t.o: $(SOURCE)/list.c $(SOURCE)/list.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
tree.o tree.t.o: $(SOURCE)/tree.c $(SOURCE)/tree.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
slots.o slots.t.o: $(SOURCE)/slots.c $(SOURCE)/slots.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/utils.h
utils.o utils.t.o: $(SOURCE)/utils.c $(SOURCE)/utils.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
memory.o memory.t.o: $(SOURCE)/memory.c $(SOURCE)/memory.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/stack.h $(SOURCE)/utils.h
heap.o heap.t.o: $(SOURCE)/heap.c $(SOURCE)/heap.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/memory.h $(SOURCE)/slots.h \
	$(SOURCE)/tree.h $(SOURCE)/trace.h $(SOURCE)/utils.h
alloc.o alloc.t.o: $(SOURCE)/alloc.c $(SOURCE)/alloc.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/heap.h $(SOURCE)/memory.h \
	$(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h $(SOURCE)/utils.h
info.o: $(SOURCE)/info.c $(SOURCE)/info.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/addr.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/stack.h $(SOURCE)/alloc.h $(SOURCE)/signals.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/diag.h \
	$(SOURCE)/utils.h $(SOURCE)/profile.h $(SOURCE)/trace.h \
	$(SOURCE)/leaktab.h
info.t.o: $(SOURCE)/info.c $(SOURCE)/info.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/addr.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/stack.h $(SOURCE)/alloc.h $(SOURCE)/signals.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/diag.h \
	$(SOURCE)/mutex.h $(SOURCE)/utils.h $(SOURCE)/profile.h \
	$(SOURCE)/trace.h $(SOURCE)/leaktab.h
stack.o stack.t.o: $(SOURCE)/stack.c $(SOURCE)/stack.h $(SOURCE)/memory.h \
	$(SOURCE)/machine.h $(SOURCE)/config.h $(SOURCE)/target.h
addr.o addr.t.o: $(SOURCE)/addr.c $(SOURCE)/addr.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/heap.h $(SOURCE)/memory.h \
	$(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h $(SOURCE)/stack.h \
	$(SOURCE)/utils.h
strtab.o strtab.t.o: $(SOURCE)/strtab.c $(SOURCE)/strtab.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/heap.h $(SOURCE)/memory.h \
	$(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h $(SOURCE)/utils.h
symbol.o symbol.t.o: $(SOURCE)/symbol.c $(SOURCE)/symbol.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/heap.h $(SOURCE)/memory.h \
	$(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/strtab.h $(SOURCE)/diag.h \
	$(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/list.h $(SOURCE)/stack.h \
	$(SOURCE)/alloc.h $(SOURCE)/signals.h $(SOURCE)/utils.h \
	$(SOURCE)/profile.h $(SOURCE)/trace.h $(SOURCE)/leaktab.h
signals.o: $(SOURCE)/signals.c $(SOURCE)/signals.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/diag.h $(SOURCE)/info.h $(SOURCE)/addr.h \
	$(SOURCE)/heap.h $(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h \
	$(SOURCE)/list.h $(SOURCE)/stack.h $(SOURCE)/alloc.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/inter.h \
	$(SOURCE)/profile.h $(SOURCE)/trace.h $(SOURCE)/leaktab.h
signals.t.o: $(SOURCE)/signals.c $(SOURCE)/signals.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/diag.h $(SOURCE)/info.h $(SOURCE)/addr.h \
	$(SOURCE)/heap.h $(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h \
	$(SOURCE)/list.h $(SOURCE)/stack.h $(SOURCE)/alloc.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/inter.h \
	$(SOURCE)/profile.h $(SOURCE)/trace.h $(SOURCE)/leaktab.h \
	$(SOURCE)/mutex.h
mutex.t.o: $(SOURCE)/mutex.c $(SOURCE)/mutex.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
diag.o: $(SOURCE)/diag.c $(SOURCE)/diag.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/stack.h $(SOURCE)/alloc.h $(SOURCE)/signals.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/utils.h \
	$(SOURCE)/profile.h $(SOURCE)/trace.h $(SOURCE)/leaktab.h \
	$(SOURCE)/version.h
diag.t.o: $(SOURCE)/diag.c $(SOURCE)/diag.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/stack.h $(SOURCE)/alloc.h $(SOURCE)/signals.h \
	$(SOURCE)/mutex.h $(SOURCE)/symbol.h $(SOURCE)/strtab.h \
	$(SOURCE)/utils.h $(SOURCE)/profile.h $(SOURCE)/trace.h \
	$(SOURCE)/leaktab.h $(SOURCE)/version.h
option.o option.t.o: $(SOURCE)/option.c $(SOURCE)/option.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/stack.h $(SOURCE)/alloc.h $(SOURCE)/signals.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/diag.h \
	$(SOURCE)/utils.h $(SOURCE)/profile.h $(SOURCE)/trace.h \
	$(SOURCE)/leaktab.h
leaktab.o leaktab.t.o: $(SOURCE)/leaktab.c $(SOURCE)/leaktab.h \
	$(SOURCE)/config.h $(SOURCE)/target.h $(SOURCE)/list.h \
	$(SOURCE)/heap.h $(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h \
	$(SOURCE)/utils.h
profile.o profile.t.o: $(SOURCE)/profile.c $(SOURCE)/profile.h \
	$(SOURCE)/config.h $(SOURCE)/target.h $(SOURCE)/alloc.h \
	$(SOURCE)/heap.h $(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h \
	$(SOURCE)/list.h $(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/stack.h \
	$(SOURCE)/signals.h $(SOURCE)/symbol.h $(SOURCE)/strtab.h \
	$(SOURCE)/diag.h $(SOURCE)/utils.h $(SOURCE)/trace.h \
	$(SOURCE)/leaktab.h $(SOURCE)/version.h
trace.o trace.t.o: $(SOURCE)/trace.c $(SOURCE)/trace.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/memory.h $(SOURCE)/diag.h \
	$(SOURCE)/info.h $(SOURCE)/alloc.h $(SOURCE)/heap.h $(SOURCE)/slots.h \
	$(SOURCE)/tree.h $(SOURCE)/list.h $(SOURCE)/addr.h $(SOURCE)/stack.h \
	$(SOURCE)/profile.h $(SOURCE)/symbol.h $(SOURCE)/strtab.h \
	$(SOURCE)/signals.h $(SOURCE)/leaktab.h $(SOURCE)/utils.h \
	$(SOURCE)/version.h
inter.o: $(SOURCE)/inter.c $(SOURCE)/inter.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/stack.h $(SOURCE)/alloc.h $(SOURCE)/signals.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/diag.h \
	$(SOURCE)/option.h $(SOURCE)/sbrk.h $(SOURCE)/profile.h \
	$(SOURCE)/trace.h $(SOURCE)/leaktab.h $(SOURCE)/machine.h \
	$(SOURCE)/version.h
inter.t.o: $(SOURCE)/inter.c $(SOURCE)/inter.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/stack.h $(SOURCE)/alloc.h $(SOURCE)/signals.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/diag.h \
	$(SOURCE)/mutex.h $(SOURCE)/option.h $(SOURCE)/sbrk.h \
	$(SOURCE)/profile.h $(SOURCE)/trace.h $(SOURCE)/leaktab.h \
	$(SOURCE)/machine.h $(SOURCE)/version.h
malloc.o malloc.t.o: $(SOURCE)/malloc.c $(SOURCE)/inter.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/stack.h $(SOURCE)/alloc.h $(SOURCE)/signals.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/profile.h \
	$(SOURCE)/trace.h $(SOURCE)/leaktab.h $(SOURCE)/diag.h
cplus.o: $(SOURCE)/cplus.c $(SOURCE)/inter.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/stack.h $(SOURCE)/alloc.h $(SOURCE)/signals.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/profile.h \
	$(SOURCE)/trace.h $(SOURCE)/diag.h $(SOURCE)/leaktab.h
cplus.t.o: $(SOURCE)/cplus.c $(SOURCE)/inter.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/stack.h $(SOURCE)/alloc.h $(SOURCE)/signals.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/profile.h \
	$(SOURCE)/trace.h $(SOURCE)/diag.h $(SOURCE)/leaktab.h $(SOURCE)/mutex.h
sbrk.o sbrk.t.o: $(SOURCE)/sbrk.c $(SOURCE)/sbrk.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/memory.h $(SOURCE)/utils.h
version.o version.t.o: $(SOURCE)/version.c $(SOURCE)/version.h \
	$(SOURCE)/config.h $(SOURCE)/target.h
mpalloc.o: $(SOURCE)/mpalloc.c $(SOURCE)/inter.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/info.h $(SOURCE)/alloc.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/addr.h $(SOURCE)/stack.h $(SOURCE)/profile.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/trace.h \
	$(SOURCE)/leaktab.h $(SOURCE)/signals.h $(SOURCE)/diag.h \
	$(SOURCE)/mpalloc.h $(SOURCE)/version.h
mpfailure.o: $(SOURCE)/mpfailure.c $(SOURCE)/mpalloc.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
mpsetfail.o: $(SOURCE)/mpsetfail.c $(SOURCE)/mpalloc.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
mpmalloc.o: $(SOURCE)/mpmalloc.c $(SOURCE)/mpalloc.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
mpcalloc.o: $(SOURCE)/mpcalloc.c $(SOURCE)/mpalloc.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
mpstrdup.o: $(SOURCE)/mpstrdup.c $(SOURCE)/mpalloc.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
mprealloc.o: $(SOURCE)/mprealloc.c $(SOURCE)/mpalloc.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
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
mpatrol.o: $(SOURCE)/mpatrol.c $(SOURCE)/getopt.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/version.h
mprof.o: $(SOURCE)/mprof.c $(SOURCE)/getopt.h $(SOURCE)/tree.h \
	$(SOURCE)/graph.h $(SOURCE)/list.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/version.h
mptrace.o: $(SOURCE)/mptrace.c $(SOURCE)/getopt.h $(SOURCE)/tree.h \
	$(SOURCE)/slots.h $(SOURCE)/config.h $(SOURCE)/target.h \
	$(SOURCE)/utils.h $(SOURCE)/version.h
mleak.o: $(SOURCE)/mleak.c $(SOURCE)/getopt.h $(SOURCE)/tree.h \
	$(SOURCE)/config.h $(SOURCE)/target.h $(SOURCE)/version.h
graph.o: $(SOURCE)/graph.c $(SOURCE)/graph.h $(SOURCE)/list.h \
	$(SOURCE)/config.h $(SOURCE)/target.h
getopt.o: $(SOURCE)/getopt.c $(SOURCE)/getopt.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
dbmalloc.o: $(TOOLS)/dbmalloc.c $(TOOLS)/dbmalloc.h $(SOURCE)/mpatrol.h \
	$(SOURCE)/config.h $(SOURCE)/target.h
dmalloc.o: $(TOOLS)/dmalloc.c $(TOOLS)/dmalloc.h $(SOURCE)/mpatrol.h \
	$(SOURCE)/config.h $(SOURCE)/target.h
heapdiff.o: $(TOOLS)/heapdiff.c $(TOOLS)/heapdiff.h $(SOURCE)/mpatrol.h \
	$(SOURCE)/config.h $(SOURCE)/target.h
mgauge.o: $(TOOLS)/mgauge.c $(TOOLS)/mgauge.h $(SOURCE)/mpatrol.h \
	$(SOURCE)/config.h $(SOURCE)/target.h
mtrace.o: $(TOOLS)/mtrace.c $(TOOLS)/mtrace.h $(SOURCE)/mpatrol.h \
	$(SOURCE)/config.h $(SOURCE)/target.h
