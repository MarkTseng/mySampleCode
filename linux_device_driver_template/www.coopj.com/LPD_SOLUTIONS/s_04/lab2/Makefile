#/* **************** LPD:1.0 s_04/Makefile **************** */
#/*
# * The code herein is: Copyright Jerry Cooperstein, 2009
# *
# * This Copyright is retained for the purpose of protecting free
# * redistribution of source.
# *
# *     URL:    http://www.coopj.com
# *     email:  coop@coopj.com
# *
# * The primary maintainer for this code is Jerry Cooperstein
# * The CONTRIBUTORS file (distributed with this
# * file) lists those known to have contributed to the source.
# *
# * This code is distributed under Version 2 of the GNU General Public
# * License, which you should have received with the source.
# *
# */
CFLAGS = -O2 -pedantic -Wall
TARGET = lab_program
LIBDIR = ./lib
LIBS   = -L $(LIBDIR) -llabsub
MYLIB  = $(LIBDIR)/liblabsub.a
SRCS   = lab_progmain.c
HFILES = lab_progmain.h

ALL:	$(TARGET)

$(TARGET):	$(SRCS) $(HFILES)  $(MYLIB)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

$(MYLIB):
	$(MAKE) -C $(LIBDIR)

clean:
	rm -f $(TARGET)
	$(MAKE) -C $(LIBDIR) clean
