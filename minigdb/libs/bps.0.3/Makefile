# bps - Breakpoint Shenanigans - K Sheldrake
# Makefile
#
# Copyright (C) 2015  Kevin Sheldrake
#
# This file is part of bps.
#
# Bps is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#

VERSION=0.3
ARCH=-DARCHARM
WARN=-Wall
CFLAGS=$(WARN) $(ARCH) -I/home/mark/myResearch/elfutils-0.165/ARM_LIBS/include
LIBS=-L/home/mark/myResearch/elfutils-0.165/ARM_LIBS/lib -L/home/mark/nfs/ARM_LIBS/lib -lelf -lz
#CFLAGS =  -DMLX_LINUX -DMLX_WITH_LIBDW -I/home/mark/myResearch/elfutils-0.165/ARM_LIBS/include/elfutils -I/home/mark/myResearch/elfutils-0.165/ARM_LIBS/include -g -O2 -Wall -I/home/mark/myResearch/libunwind/LIBUNWIND_ARM/include 
#LDLIBS =  -L/home/mark/myResearch/libunwind/LIBUNWIND_ARM/lib -L/home/mark/myResearch/elfutils-0.165/ARM_LIBS/lib -L/home/mark/myResearch/elfutils-0.165/ARM_LIBS/lib/elfutils -L/home/mark/nfs/ARM_LIBS/lib -lunwind-ptrace -lunwind -lunwind-arm -lelf -ldw -lz -lreadline
CC=arm-linux-gnueabihf-gcc

all: bps.$(VERSION).c bps.h bps_cmdline.o bps_data.o bps_elf.o bps_ptrace.o bps_arm.o bps_util.o
	$(CC) $(WARN) $(ARCH) $(CFLAGS) $(LIBS) -o bps.$(VERSION) bps.$(VERSION).c bps_cmdline.o bps_data.o bps_elf.o bps_ptrace.o bps_arm.o bps_util.o

bps_cmdline.o: bps_cmdline.c bps_cmdline.h
	$(CC) -c $(CFLAGS) bps_cmdline.c

bps_data.o: bps_data.c bps_data.h
	$(CC) -c $(CFLAGS) bps_data.c

bps_elf.o: bps_elf.c bps_elf.h
	$(CC) -c $(CFLAGS) bps_elf.c

bps_ptrace.o: bps_ptrace.c bps_ptrace.h
	$(CC) -c $(CFLAGS) bps_ptrace.c

bps_arm.o: bps_arm.c bps_ptrace.h
	$(CC) -c $(CFLAGS) bps_arm.c

bps_util.o: bps_util.c bps_util.h
	$(CC) -c $(CFLAGS) bps_util.c

clean:
	rm -rf *.o bps.$(VERSION)
