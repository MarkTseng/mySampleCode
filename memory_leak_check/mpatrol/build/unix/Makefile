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


# GNU Makefile to compile on UNIX with GNU C++


# $Id$


# Definitions

SHELL = /bin/sh
CC    = gcc
CXX   = g++
AR    = ar
LD    = $(CC)
LINT  = lint

SOURCE = ../../src
TOOLS  = ../../tools
VPATH  = $(SOURCE):$(TOOLS)

GUISUP	= false

ifeq ($(GUISUP),true)
CFLAGS	= -I$(SOURCE) -I$(TOOLS) -I/usr/X11R6/include -I/usr/X11R5/include -I/usr/X11/include -I/usr/X/include $(UNWIND_CFLAGS) -DMP_GUI_SUPPORT=1
else
CFLAGS	= -I$(SOURCE) -I$(TOOLS) $(UNWIND_CFLAGS)
endif
OFLAGS	= -O3 -fno-inline-functions
SFLAGS	= -fPIC
TFLAGS	= -DMP_THREADS_SUPPORT
LDFLAGS	= -shared

MPATROL	 = mpatrol
MPALLOC	 = mpalloc
MPTOOLS	 = mptools
VERSION	 = 1
REVISION = 5

MPTOBJS	 = list.o tree.o slots.o utils.o memory.o heap.o alloc.o info.o \
	   stack.o addr.o strtab.o symbol.o signals.o diag.o option.o \
	   leaktab.o profile.o trace.o inter.o malloc.o cplus.o version.o
MPTTOBJS = list.o tree.o slots.o utils.o memory.o heap.o alloc.o info.o \
	   stack.o addr.o strtab.o symbol.o signals.o mutex.o diag.o option.o \
	   leaktab.o profile.o trace.o inter.o malloc.o cplus.o version.o
MPAOBJS	 = mpalloc.o mpfailure.o mpsetfail.o mpmalloc.o mpcalloc.o mpstrdup.o \
	   mprealloc.o
MPLOBJS	 = dbmalloc.o dmalloc.o heapdiff.o mgauge.o mtrace.o

ARCHIVE_MPTOBJS	= $(MPTOBJS) machine.o
SHARED_MPTOBJS	= $(MPTOBJS:.o=.s.o) machine.s.o
LINT_MPTOBJS	= $(MPTOBJS:.o=.n.ln)
ARCHTS_MPTOBJS	= $(MPTTOBJS:.o=.t.o) machine.t.o
SHARTS_MPTOBJS	= $(MPTTOBJS:.o=.s.t.o) machine.s.t.o
LINTTS_MPTOBJS	= $(MPTTOBJS:.o=.t.ln)

ARCHIVE_MPAOBJS	= $(MPAOBJS)
SHARED_MPAOBJS	= $(MPAOBJS:.o=.s.o)
LINT_MPAOBJS	= $(MPAOBJS:.o=.n.ln)

ARCHIVE_MPLOBJS	= $(MPLOBJS)
LINT_MPLOBJS	= $(MPLOBJS:.o=.n.ln)

ARCHIVE_MPATROL	= lib$(MPATROL).a
SHARED_MPATROL	= lib$(MPATROL).so
OBJECT_MPATROL	= lib$(MPATROL).o
LINT_MPATROL	= llib-l$(MPATROL).ln
ARCHTS_MPATROL	= lib$(MPATROL)mt.a
SHARTS_MPATROL	= lib$(MPATROL)mt.so
OBJTS_MPATROL	= lib$(MPATROL)mt.o
LINTTS_MPATROL	= llib-l$(MPATROL)mt.ln

ARCHIVE_MPALLOC	= lib$(MPALLOC).a
SHARED_MPALLOC	= lib$(MPALLOC).so
OBJECT_MPALLOC	= lib$(MPALLOC).o
LINT_MPALLOC	= llib-l$(MPALLOC).ln

ARCHIVE_MPTOOLS	= lib$(MPTOOLS).a
LINT_MPTOOLS	= llib-l$(MPTOOLS).ln


# Rules

.SUFFIXES: .c .s .o .s.o .n.ln .t.o .s.t.o .t.ln

.c.o:
	$(CC) $(CFLAGS) $(OFLAGS) -c $<

.c.s.o:
	$(CC) $(CFLAGS) $(OFLAGS) $(SFLAGS) -c -o $@ $<

.c.n.ln:
	$(LINT) $(CFLAGS) -y -c $<
	-@ mv -f `basename $@ .n.ln`.ln $@

.c.t.o:
	$(CC) $(CFLAGS) $(OFLAGS) $(TFLAGS) -c -o $@ $<

.c.s.t.o:
	$(CC) $(CFLAGS) $(OFLAGS) $(SFLAGS) $(TFLAGS) -c -o $@ $<

.c.t.ln:
	$(LINT) $(CFLAGS) $(TFLAGS) -y -c $<
	-@ mv -f `basename $@ .t.ln`.ln $@

$(ARCHIVE_MPATROL): $(ARCHIVE_MPTOBJS)
	$(AR) cr $@ $(ARCHIVE_MPTOBJS)

$(SHARED_MPATROL).$(VERSION).$(REVISION): $(SHARED_MPTOBJS)
	$(LD) $(LDFLAGS) -o $@ $(SHARED_MPTOBJS)

$(SHARED_MPATROL): $(SHARED_MPATROL).$(VERSION).$(REVISION)
	@ ln -f -s $< $@

$(OBJECT_MPATROL): $(SOURCE)/library.c
	$(CC) $(CFLAGS) $(OFLAGS) -c -o $@ $<

$(LINT_MPATROL): $(LINT_MPTOBJS)
	$(LINT) -o $(MPATROL) $(LINT_MPTOBJS)

$(ARCHTS_MPATROL): $(ARCHTS_MPTOBJS)
	$(AR) cr $@ $(ARCHTS_MPTOBJS)

$(SHARTS_MPATROL).$(VERSION).$(REVISION): $(SHARTS_MPTOBJS)
	$(LD) $(LDFLAGS) -o $@ $(SHARTS_MPTOBJS)

$(SHARTS_MPATROL): $(SHARTS_MPATROL).$(VERSION).$(REVISION)
	@ ln -f -s $< $@

$(OBJTS_MPATROL): $(SOURCE)/library.c
	$(CC) $(CFLAGS) $(OFLAGS) $(TFLAGS) -c -o $@ $<

$(LINTTS_MPATROL): $(LINTTS_MPTOBJS)
	$(LINT) -o $(MPATROL)mt $(LINTTS_MPTOBJS)

$(ARCHIVE_MPALLOC): $(ARCHIVE_MPAOBJS)
	$(AR) cr $@ $(ARCHIVE_MPAOBJS)

$(SHARED_MPALLOC).$(VERSION).$(REVISION): $(SHARED_MPAOBJS)
	$(LD) $(LDFLAGS) -o $@ $(SHARED_MPAOBJS)

$(SHARED_MPALLOC): $(SHARED_MPALLOC).$(VERSION).$(REVISION)
	@ ln -f -s $< $@

$(OBJECT_MPALLOC): $(SOURCE)/library2.c
	$(CC) $(CFLAGS) $(OFLAGS) -c -o $@ $<

$(LINT_MPALLOC): $(LINT_MPAOBJS)
	$(LINT) -o $(MPALLOC) $(LINT_MPAOBJS)

$(ARCHIVE_MPTOOLS): $(ARCHIVE_MPLOBJS)
	$(AR) cr $@ $(ARCHIVE_MPLOBJS)

$(LINT_MPTOOLS): $(LINT_MPLOBJS)
	$(LINT) -o $(MPTOOLS) $(LINT_MPLOBJS)

machine.o: $(SOURCE)/machine.c
	$(CC) $(CFLAGS) -E -P $< >machine.s
	$(CC) $(CFLAGS) $(OFLAGS) -c machine.s
	-@ rm -f machine.s 2>&1 >/dev/null

machine.s.o: $(SOURCE)/machine.c
	$(CC) $(CFLAGS) $(SFLAGS) -E -P $< >machine.s
	$(CC) $(CFLAGS) $(OFLAGS) $(SFLAGS) -c -o $@ machine.s
	-@ rm -f machine.s 2>&1 >/dev/null

machine.t.o: $(SOURCE)/machine.c
	$(CC) $(CFLAGS) $(TFLAGS) -E -P $< >machine.s
	$(CC) $(CFLAGS) $(OFLAGS) $(TFLAGS) -c -o $@ machine.s
	-@ rm -f machine.s 2>&1 >/dev/null

machine.s.t.o: $(SOURCE)/machine.c
	$(CC) $(CFLAGS) $(SFLAGS) $(TFLAGS) -E -P $< >machine.s
	$(CC) $(CFLAGS) $(OFLAGS) $(SFLAGS) $(TFLAGS) -c -o $@ machine.s
	-@ rm -f machine.s 2>&1 >/dev/null

cplus.o:
	$(CXX) $(CFLAGS) $(OFLAGS) -c $<

cplus.s.o:
	$(CXX) $(CFLAGS) $(OFLAGS) $(SFLAGS) -c -o $@ $<

cplus.t.o:
	$(CXX) $(CFLAGS) $(OFLAGS) $(TFLAGS) -c -o $@ $<

cplus.s.t.o:
	$(CXX) $(CFLAGS) $(OFLAGS) $(SFLAGS) $(TFLAGS) -c -o $@ $<

mpatrol: mpatrol.o getopt.o version.o
	$(CC) $(CFLAGS) $(OFLAGS) -o $@ mpatrol.o getopt.o version.o

mprof: mprof.o graph.o getopt.o list.o tree.o version.o
	$(CC) $(CFLAGS) $(OFLAGS) -o $@ mprof.o graph.o getopt.o list.o tree.o version.o

mptrace: mptrace.o getopt.o tree.o slots.o utils.o version.o
ifeq ($(GUISUP),true)
	$(CC) $(CFLAGS) $(OFLAGS) -o $@ mptrace.o getopt.o tree.o slots.o utils.o version.o -L/usr/X11R6/lib -L/usr/X11R5/lib -L/usr/X11/lib -L/usr/X/lib -lXm -lXt -lXp -lXext -lX11
else
	$(CC) $(CFLAGS) $(OFLAGS) -o $@ mptrace.o getopt.o tree.o slots.o utils.o version.o
endif

mleak: mleak.o getopt.o tree.o version.o
	$(CC) $(CFLAGS) $(OFLAGS) -o $@ mleak.o getopt.o tree.o version.o

all: $(ARCHIVE_MPATROL) $(SHARED_MPATROL) $(ARCHTS_MPATROL) $(SHARTS_MPATROL) \
	$(ARCHIVE_MPALLOC) $(SHARED_MPALLOC) $(ARCHIVE_MPTOOLS) \
	mpatrol mprof mptrace mleak

lint: $(LINT_MPATROL) $(LINTTS_MPATROL) $(LINT_MPALLOC) $(LINT_MPTOOLS)

clean:
	-@ rm -f $(ARCHIVE_MPTOBJS) $(ARCHTS_MPTOBJS) 2>&1 >/dev/null
	-@ rm -f $(SHARED_MPTOBJS) $(SHARTS_MPTOBJS) 2>&1 >/dev/null
	-@ rm -f $(LINT_MPTOBJS) $(LINTTS_MPTOBJS) 2>&1 >/dev/null
	-@ rm -f $(ARCHIVE_MPAOBJS) $(SHARED_MPAOBJS) 2>&1 >/dev/null
	-@ rm -f $(LINT_MPAOBJS) 2>&1 >/dev/null
	-@ rm -f $(ARCHIVE_MPLOBJS) $(LINT_MPLOBJS) 2>&1 >/dev/null
	-@ rm -f mpatrol.o mprof.o mptrace.o mleak.o graph.o getopt.o 2>&1 >/dev/null

clobber: clean
	-@ rm -f $(ARCHIVE_MPATROL) $(ARCHTS_MPATROL) 2>&1 >/dev/null
	-@ rm -f $(SHARED_MPATROL).$(VERSION).$(REVISION) 2>&1 >/dev/null
	-@ rm -f $(SHARTS_MPATROL).$(VERSION).$(REVISION) 2>&1 >/dev/null
	-@ rm -f $(SHARED_MPATROL) $(SHARTS_MPATROL) 2>&1 >/dev/null
	-@ rm -f $(OBJECT_MPATROL) $(OBJTS_MPATROL) 2>&1 >/dev/null
	-@ rm -f $(LINT_MPATROL) $(LINTTS_MPATROL) 2>&1 >/dev/null
	-@ rm -f $(ARCHIVE_MPALLOC) 2>&1 >/dev/null
	-@ rm -f $(SHARED_MPALLOC).$(VERSION).$(REVISION) 2>&1 >/dev/null
	-@ rm -f $(SHARED_MPALLOC) $(OBJECT_MPALLOC) 2>&1 >/dev/null
	-@ rm -f $(LINT_MPALLOC) 2>&1 >/dev/null
	-@ rm -f $(ARCHIVE_MPTOOLS) $(LINT_MPTOOLS) 2>&1 >/dev/null
	-@ rm -f mpatrol mprof mptrace mleak 2>&1 >/dev/null


# Dependencies

list.o list.s.o list.n.ln list.t.o list.s.t.o list.t.ln: $(SOURCE)/list.c \
	$(SOURCE)/list.h $(SOURCE)/config.h $(SOURCE)/target.h
tree.o tree.s.o tree.n.ln tree.t.o tree.s.t.o tree.t.ln: $(SOURCE)/tree.c \
	$(SOURCE)/tree.h $(SOURCE)/config.h $(SOURCE)/target.h
slots.o slots.s.o slots.n.ln slots.t.o slots.s.t.o slots.t.ln: \
	$(SOURCE)/slots.c $(SOURCE)/slots.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/utils.h
utils.o utils.s.o utils.n.ln utils.t.o utils.s.t.o utils.t.ln: \
	$(SOURCE)/utils.c $(SOURCE)/utils.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
memory.o memory.s.o memory.n.ln memory.t.o memory.s.t.o memory.t.ln: \
	$(SOURCE)/memory.c $(SOURCE)/memory.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/stack.h $(SOURCE)/utils.h
heap.o heap.s.o heap.n.ln heap.t.o heap.s.t.o heap.t.ln: $(SOURCE)/heap.c \
	$(SOURCE)/heap.h $(SOURCE)/config.h $(SOURCE)/target.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h \
	$(SOURCE)/trace.h $(SOURCE)/utils.h
alloc.o alloc.s.o alloc.n.ln alloc.t.o alloc.s.t.o alloc.t.ln: \
	$(SOURCE)/alloc.c $(SOURCE)/alloc.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/heap.h $(SOURCE)/memory.h \
	$(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h $(SOURCE)/utils.h
info.o info.s.o info.n.ln: $(SOURCE)/info.c $(SOURCE)/info.h \
	$(SOURCE)/config.h $(SOURCE)/target.h $(SOURCE)/addr.h \
	$(SOURCE)/heap.h $(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h \
	$(SOURCE)/list.h $(SOURCE)/stack.h $(SOURCE)/alloc.h \
	$(SOURCE)/signals.h $(SOURCE)/symbol.h $(SOURCE)/strtab.h \
	$(SOURCE)/diag.h $(SOURCE)/utils.h $(SOURCE)/profile.h \
	$(SOURCE)/trace.h $(SOURCE)/leaktab.h
info.t.o info.s.t.o info.t.ln: $(SOURCE)/info.c $(SOURCE)/info.h \
	$(SOURCE)/config.h $(SOURCE)/target.h $(SOURCE)/addr.h \
	$(SOURCE)/heap.h $(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h \
	$(SOURCE)/list.h $(SOURCE)/stack.h $(SOURCE)/alloc.h \
	$(SOURCE)/signals.h $(SOURCE)/symbol.h $(SOURCE)/strtab.h \
	$(SOURCE)/diag.h $(SOURCE)/mutex.h $(SOURCE)/utils.h \
	$(SOURCE)/profile.h $(SOURCE)/trace.h $(SOURCE)/leaktab.h
stack.o stack.s.o stack.n.ln stack.t.o stack.s.t.o stack.t.ln: \
	$(SOURCE)/stack.c $(SOURCE)/stack.h $(SOURCE)/memory.h \
	$(SOURCE)/machine.h $(SOURCE)/config.h $(SOURCE)/target.h
addr.o addr.s.o addr.n.ln addr.t.o addr.s.t.o addr.t.ln: $(SOURCE)/addr.c \
	$(SOURCE)/addr.h $(SOURCE)/config.h $(SOURCE)/target.h \
	$(SOURCE)/heap.h $(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h \
	$(SOURCE)/list.h $(SOURCE)/stack.h $(SOURCE)/utils.h
strtab.o strtab.s.o strtab.n.ln strtab.t.o strtab.s.t.o strtab.t.ln: \
	$(SOURCE)/strtab.c $(SOURCE)/strtab.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/heap.h $(SOURCE)/memory.h \
	$(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h $(SOURCE)/utils.h
symbol.o symbol.s.o symbol.n.ln symbol.t.o symbol.s.t.o symbol.t.ln: \
	$(SOURCE)/symbol.c $(SOURCE)/symbol.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/heap.h $(SOURCE)/memory.h \
	$(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/strtab.h $(SOURCE)/diag.h \
	$(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/list.h $(SOURCE)/stack.h \
	$(SOURCE)/alloc.h $(SOURCE)/signals.h $(SOURCE)/utils.h \
	$(SOURCE)/profile.h $(SOURCE)/trace.h $(SOURCE)/leaktab.h
signals.o signals.s.o signals.n.ln: $(SOURCE)/signals.c $(SOURCE)/signals.h \
	$(SOURCE)/config.h $(SOURCE)/target.h $(SOURCE)/diag.h \
	$(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/heap.h $(SOURCE)/memory.h \
	$(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h $(SOURCE)/stack.h \
	$(SOURCE)/alloc.h $(SOURCE)/symbol.h $(SOURCE)/strtab.h \
	$(SOURCE)/inter.h $(SOURCE)/profile.h $(SOURCE)/trace.h \
	$(SOURCE)/leaktab.h
signals.t.o signals.s.t.o signals.t.ln: $(SOURCE)/signals.c \
	$(SOURCE)/signals.h $(SOURCE)/config.h $(SOURCE)/target.h \
	$(SOURCE)/diag.h $(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/stack.h $(SOURCE)/alloc.h $(SOURCE)/symbol.h \
	$(SOURCE)/strtab.h $(SOURCE)/inter.h $(SOURCE)/profile.h \
	$(SOURCE)/trace.h $(SOURCE)/mutex.h $(SOURCE)/leaktab.h
mutex.t.o mutex.s.t.o mutex.t.ln: $(SOURCE)/mutex.c $(SOURCE)/mutex.h \
	$(SOURCE)/config.h $(SOURCE)/target.h
diag.o diag.s.o diag.n.ln: $(SOURCE)/diag.c $(SOURCE)/diag.h \
	$(SOURCE)/config.h $(SOURCE)/target.h $(SOURCE)/info.h \
	$(SOURCE)/addr.h $(SOURCE)/heap.h $(SOURCE)/memory.h $(SOURCE)/slots.h \
	$(SOURCE)/tree.h $(SOURCE)/list.h $(SOURCE)/stack.h $(SOURCE)/alloc.h \
	$(SOURCE)/signals.h $(SOURCE)/symbol.h $(SOURCE)/strtab.h \
	$(SOURCE)/utils.h $(SOURCE)/profile.h $(SOURCE)/trace.h \
	$(SOURCE)/leaktab.h $(SOURCE)/version.h
diag.t.o diag.s.t.o diag.t.ln: $(SOURCE)/diag.c $(SOURCE)/diag.h \
	$(SOURCE)/config.h $(SOURCE)/target.h $(SOURCE)/info.h \
	$(SOURCE)/addr.h $(SOURCE)/heap.h $(SOURCE)/memory.h \
	$(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h $(SOURCE)/stack.h \
	$(SOURCE)/alloc.h $(SOURCE)/signals.h $(SOURCE)/mutex.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/utils.h \
	$(SOURCE)/profile.h $(SOURCE)/trace.h $(SOURCE)/leaktab.h \
	$(SOURCE)/version.h
option.o option.s.o option.n.ln option.t.o option.s.t.o option.t.ln: \
	$(SOURCE)/option.c $(SOURCE)/option.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/stack.h $(SOURCE)/alloc.h $(SOURCE)/signals.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/diag.h \
	$(SOURCE)/utils.h $(SOURCE)/profile.h $(SOURCE)/trace.h \
	$(SOURCE)/leaktab.h
leaktab.o leaktab.s.o leaktab.n.ln leaktab.t.o leaktab.s.t.o leaktab.t.ln: \
	$(SOURCE)/leaktab.c $(SOURCE)/leaktab.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/list.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/utils.h
profile.o profile.s.o profile.n.ln profile.t.o profile.s.t.o profile.t.ln: \
	$(SOURCE)/profile.c $(SOURCE)/profile.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/alloc.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/stack.h \
	$(SOURCE)/signals.h $(SOURCE)/symbol.h $(SOURCE)/strtab.h \
	$(SOURCE)/diag.h $(SOURCE)/utils.h $(SOURCE)/trace.h \
	$(SOURCE)/leaktab.h $(SOURCE)/version.h
trace.o trace.s.o trace.n.ln trace.t.o trace.s.t.o trace.t.ln: \
	$(SOURCE)/trace.c $(SOURCE)/trace.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/memory.h $(SOURCE)/diag.h \
	$(SOURCE)/info.h $(SOURCE)/alloc.h $(SOURCE)/heap.h $(SOURCE)/slots.h \
	$(SOURCE)/tree.h $(SOURCE)/list.h $(SOURCE)/addr.h $(SOURCE)/stack.h \
	$(SOURCE)/profile.h $(SOURCE)/symbol.h $(SOURCE)/strtab.h \
	$(SOURCE)/signals.h $(SOURCE)/utils.h $(SOURCE)/leaktab.h \
	$(SOURCE)/version.h
inter.o inter.s.o inter.n.ln: $(SOURCE)/inter.c $(SOURCE)/inter.h \
	$(SOURCE)/config.h $(SOURCE)/target.h $(SOURCE)/info.h \
	$(SOURCE)/addr.h $(SOURCE)/heap.h $(SOURCE)/memory.h $(SOURCE)/slots.h \
	$(SOURCE)/tree.h $(SOURCE)/list.h $(SOURCE)/stack.h $(SOURCE)/alloc.h \
	$(SOURCE)/signals.h $(SOURCE)/symbol.h $(SOURCE)/strtab.h \
	$(SOURCE)/diag.h $(SOURCE)/option.h $(SOURCE)/profile.h \
	$(SOURCE)/trace.h $(SOURCE)/leaktab.h $(SOURCE)/machine.h \
	$(SOURCE)/version.h
inter.t.o inter.s.t.o inter.t.ln: $(SOURCE)/inter.c $(SOURCE)/inter.h \
	$(SOURCE)/config.h $(SOURCE)/target.h $(SOURCE)/info.h \
	$(SOURCE)/addr.h $(SOURCE)/heap.h $(SOURCE)/memory.h $(SOURCE)/slots.h \
	$(SOURCE)/tree.h $(SOURCE)/list.h $(SOURCE)/stack.h $(SOURCE)/alloc.h \
	$(SOURCE)/signals.h $(SOURCE)/symbol.h $(SOURCE)/strtab.h \
	$(SOURCE)/diag.h $(SOURCE)/mutex.h $(SOURCE)/option.h \
	$(SOURCE)/profile.h $(SOURCE)/trace.h $(SOURCE)/leaktab.h \
	$(SOURCE)/machine.h $(SOURCE)/version.h
malloc.o malloc.s.o malloc.n.ln malloc.t.o malloc.s.t.o malloc.t.ln: \
	$(SOURCE)/malloc.c $(SOURCE)/inter.h $(SOURCE)/config.h \
	$(SOURCE)/target.h $(SOURCE)/info.h $(SOURCE)/addr.h $(SOURCE)/heap.h \
	$(SOURCE)/memory.h $(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h \
	$(SOURCE)/stack.h $(SOURCE)/alloc.h $(SOURCE)/signals.h \
	$(SOURCE)/symbol.h $(SOURCE)/strtab.h $(SOURCE)/profile.h \
	$(SOURCE)/trace.h $(SOURCE)/leaktab.h $(SOURCE)/diag.h
cplus.o cplus.s.o cplus.n.ln: $(SOURCE)/cplus.c $(SOURCE)/inter.h \
	$(SOURCE)/config.h $(SOURCE)/target.h $(SOURCE)/info.h \
	$(SOURCE)/addr.h $(SOURCE)/heap.h $(SOURCE)/memory.h $(SOURCE)/slots.h \
	$(SOURCE)/tree.h $(SOURCE)/list.h $(SOURCE)/stack.h $(SOURCE)/alloc.h \
	$(SOURCE)/signals.h $(SOURCE)/symbol.h $(SOURCE)/strtab.h \
	$(SOURCE)/profile.h $(SOURCE)/trace.h $(SOURCE)/diag.h \
	$(SOURCE)/leaktab.h
cplus.t.o cplus.s.t.o cplus.t.ln: $(SOURCE)/cplus.c $(SOURCE)/inter.h \
	$(SOURCE)/config.h $(SOURCE)/target.h $(SOURCE)/info.h \
	$(SOURCE)/addr.h $(SOURCE)/heap.h $(SOURCE)/memory.h $(SOURCE)/slots.h \
	$(SOURCE)/tree.h $(SOURCE)/list.h $(SOURCE)/stack.h $(SOURCE)/alloc.h \
	$(SOURCE)/signals.h $(SOURCE)/symbol.h $(SOURCE)/strtab.h \
	$(SOURCE)/profile.h $(SOURCE)/trace.h $(SOURCE)/diag.h \
	$(SOURCE)/leaktab.h $(SOURCE)/mutex.h
version.o version.s.o version.n.ln version.t.o version.s.t.o version.t.ln: \
	$(SOURCE)/version.c $(SOURCE)/version.h $(SOURCE)/config.h \
	$(SOURCE)/target.h
machine.o machine.s.o machine.t.o machine.s.t.o: $(SOURCE)/machine.c \
	$(SOURCE)/config.h $(SOURCE)/target.h
mpalloc.o mpalloc.s.o mpalloc.n.ln: $(SOURCE)/mpalloc.c $(SOURCE)/inter.h \
	$(SOURCE)/config.h $(SOURCE)/target.h $(SOURCE)/info.h \
	$(SOURCE)/alloc.h $(SOURCE)/heap.h $(SOURCE)/memory.h \
	$(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h $(SOURCE)/addr.h \
	$(SOURCE)/stack.h $(SOURCE)/profile.h $(SOURCE)/symbol.h \
	$(SOURCE)/strtab.h $(SOURCE)/trace.h $(SOURCE)/leaktab.h \
	$(SOURCE)/signals.h $(SOURCE)/diag.h $(SOURCE)/mpalloc.h \
	$(SOURCE)/version.h
mpfailure.o mpfailure.s.o mpfailure.n.ln: $(SOURCE)/mpfailure.c \
	$(SOURCE)/mpalloc.h $(SOURCE)/config.h $(SOURCE)/target.h
mpsetfail.o mpsetfail.s.o mpsetfail.n.ln: $(SOURCE)/mpsetfail.c \
	$(SOURCE)/mpalloc.h $(SOURCE)/config.h $(SOURCE)/target.h
mpmalloc.o mpmalloc.s.o mpmalloc.n.ln: $(SOURCE)/mpmalloc.c \
	$(SOURCE)/mpalloc.h $(SOURCE)/config.h $(SOURCE)/target.h
mpcalloc.o mpcalloc.s.o mpcalloc.n.ln: $(SOURCE)/mpcalloc.c \
	$(SOURCE)/mpalloc.h $(SOURCE)/config.h $(SOURCE)/target.h
mpstrdup.o mpstrdup.s.o mpstrdup.n.ln: $(SOURCE)/mpstrdup.c \
	$(SOURCE)/mpalloc.h $(SOURCE)/config.h $(SOURCE)/target.h
mprealloc.o mprealloc.s.o mprealloc.n.ln: $(SOURCE)/mprealloc.c \
	$(SOURCE)/mpalloc.h $(SOURCE)/config.h $(SOURCE)/target.h
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
	$(SOURCE)/cplus.c $(SOURCE)/leaktab.c $(SOURCE)/leaktab.h \
	$(SOURCE)/profile.c $(SOURCE)/profile.h $(SOURCE)/trace.c \
	$(SOURCE)/trace.h $(SOURCE)/machine.h $(SOURCE)/version.c
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
	$(SOURCE)/leaktab.c $(SOURCE)/leaktab.h $(SOURCE)/profile.c \
	$(SOURCE)/profile.h $(SOURCE)/trace.c $(SOURCE)/trace.h \
	$(SOURCE)/machine.h $(SOURCE)/version.c
$(OBJECT_MPALLOC): $(SOURCE)/library2.c $(SOURCE)/config.h $(SOURCE)/target.h \
	$(SOURCE)/mpalloc.c $(SOURCE)/inter.h $(SOURCE)/info.h \
	$(SOURCE)/alloc.h $(SOURCE)/heap.h $(SOURCE)/memory.h \
	$(SOURCE)/slots.h $(SOURCE)/tree.h $(SOURCE)/list.h $(SOURCE)/addr.h \
	$(SOURCE)/stack.h $(SOURCE)/profile.h $(SOURCE)/symbol.h \
	$(SOURCE)/strtab.h $(SOURCE)/trace.h $(SOURCE)/leaktab.h \
	$(SOURCE)/signals.h $(SOURCE)/diag.h $(SOURCE)/mpalloc.h \
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
dbmalloc.o dbmalloc.n.ln: $(TOOLS)/dbmalloc.c $(TOOLS)/dbmalloc.h \
	$(SOURCE)/mpatrol.h $(SOURCE)/config.h $(SOURCE)/target.h
dmalloc.o dmalloc.n.ln: $(TOOLS)/dmalloc.c $(TOOLS)/dmalloc.h \
	$(SOURCE)/mpatrol.h $(SOURCE)/config.h $(SOURCE)/target.h
heapdiff.o heapdiff.n.ln: $(TOOLS)/heapdiff.c $(TOOLS)/heapdiff.h \
	$(SOURCE)/mpatrol.h $(SOURCE)/config.h $(SOURCE)/target.h
mgauge.o mgauge.n.ln: $(TOOLS)/mgauge.c $(TOOLS)/mgauge.h $(SOURCE)/mpatrol.h \
	$(SOURCE)/config.h $(SOURCE)/target.h
mtrace.o mtrace.n.ln: $(TOOLS)/mtrace.c $(TOOLS)/mtrace.h $(SOURCE)/mpatrol.h \
	$(SOURCE)/config.h $(SOURCE)/target.h
