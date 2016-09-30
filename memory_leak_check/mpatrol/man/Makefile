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


# GNU Makefile to build the UNIX manual pages


# $Id$


# Definitions

SHELL     = /bin/sh
MAN2ASCII = groff -man -t -Tascii
MAN2HTML  = man2html -r
MAN2DVI   = groff -ma4 -man -t -Tdvi
MAN2PS    = groff -ma4 -man -t -Tps
MAN2PDF   = pstill -Fa4 -c -c -p

CATPAGES  = cat1/mpatrol.1 cat1/mprof.1 cat1/mptrace.1 cat1/mleak.1 \
	    cat1/mpsym.1 cat1/mpedit.1 cat1/hexwords.1 cat3/libmpatrol.3 \
	    cat3/libmpalloc.3
HTMLPAGES = html/mpatrol.html html/mprof.html html/mptrace.html \
	    html/mleak.html html/mpsym.html html/mpedit.html \
	    html/hexwords.html html/libmpatrol.html html/libmpalloc.html
DVIPAGES  = dvi/mpatrol.dvi dvi/mprof.dvi dvi/mptrace.dvi dvi/mleak.dvi \
	    dvi/mpsym.dvi dvi/mpedit.dvi dvi/hexwords.dvi dvi/libmpatrol.dvi \
	    dvi/libmpalloc.dvi
PSPAGES	  = ps/mpatrol.ps ps/mprof.ps ps/mptrace.ps ps/mleak.ps \
	    ps/mpsym.ps ps/mpedit.ps ps/hexwords.ps ps/libmpatrol.ps \
	    ps/libmpalloc.ps
PDFPAGES  = pdf/mpatrol.pdf pdf/mprof.pdf pdf/mptrace.pdf pdf/mleak.pdf \
	    pdf/mpsym.pdf pdf/mpedit.pdf pdf/hexwords.pdf pdf/libmpatrol.pdf \
	    pdf/libmpalloc.pdf


# Rules

.SUFFIXES: .1 .3 .html .dvi .ps .pdf

default: $(CATPAGES)

cat1/mpatrol.1: man1/mpatrol.1
	$(MAN2ASCII) $< >$@

cat1/mprof.1: man1/mprof.1
	$(MAN2ASCII) $< >$@

cat1/mptrace.1: man1/mptrace.1
	$(MAN2ASCII) $< >$@

cat1/mleak.1: man1/mleak.1
	$(MAN2ASCII) $< >$@

cat1/mpsym.1: man1/mpsym.1
	$(MAN2ASCII) $< >$@

cat1/mpedit.1: man1/mpedit.1
	$(MAN2ASCII) $< >$@

cat1/hexwords.1: man1/hexwords.1
	$(MAN2ASCII) $< >$@

cat3/libmpatrol.3: man3/libmpatrol.3
	$(MAN2ASCII) $< >$@

cat3/libmpalloc.3: man3/libmpalloc.3
	$(MAN2ASCII) $< >$@

html/mpatrol.html: man1/mpatrol.1
	$(MAN2HTML) $< >$@

html/mprof.html: man1/mprof.1
	$(MAN2HTML) $< >$@

html/mptrace.html: man1/mptrace.1
	$(MAN2HTML) $< >$@

html/mleak.html: man1/mleak.1
	$(MAN2HTML) $< >$@

html/mpsym.html: man1/mpsym.1
	$(MAN2HTML) $< >$@

html/mpedit.html: man1/mpedit.1
	$(MAN2HTML) $< >$@

html/hexwords.html: man1/hexwords.1
	$(MAN2HTML) $< >$@

html/libmpatrol.html: man3/libmpatrol.3
	$(MAN2HTML) $< >$@

html/libmpalloc.html: man3/libmpalloc.3
	$(MAN2HTML) $< >$@

dvi/mpatrol.dvi: man1/mpatrol.1
	$(MAN2DVI) $< >$@

dvi/mprof.dvi: man1/mprof.1
	$(MAN2DVI) $< >$@

dvi/mptrace.dvi: man1/mptrace.1
	$(MAN2DVI) $< >$@

dvi/mleak.dvi: man1/mleak.1
	$(MAN2DVI) $< >$@

dvi/mpsym.dvi: man1/mpsym.1
	$(MAN2DVI) $< >$@

dvi/mpedit.dvi: man1/mpedit.1
	$(MAN2DVI) $< >$@

dvi/hexwords.dvi: man1/hexwords.1
	$(MAN2DVI) $< >$@

dvi/libmpatrol.dvi: man3/libmpatrol.3
	$(MAN2DVI) $< >$@

dvi/libmpalloc.dvi: man3/libmpalloc.3
	$(MAN2DVI) $< >$@

ps/mpatrol.ps: man1/mpatrol.1
	$(MAN2PS) $< >$@

ps/mprof.ps: man1/mprof.1
	$(MAN2PS) $< >$@

ps/mptrace.ps: man1/mptrace.1
	$(MAN2PS) $< >$@

ps/mleak.ps: man1/mleak.1
	$(MAN2PS) $< >$@

ps/mpsym.ps: man1/mpsym.1
	$(MAN2PS) $< >$@

ps/mpedit.ps: man1/mpedit.1
	$(MAN2PS) $< >$@

ps/hexwords.ps: man1/hexwords.1
	$(MAN2PS) $< >$@

ps/libmpatrol.ps: man3/libmpatrol.3
	$(MAN2PS) $< >$@

ps/libmpalloc.ps: man3/libmpalloc.3
	$(MAN2PS) $< >$@

pdf/mpatrol.pdf: ps/mpatrol.ps
	$(MAN2PDF) -o $@ $<

pdf/mprof.pdf: ps/mprof.ps
	$(MAN2PDF) -o $@ $<

pdf/mptrace.pdf: ps/mptrace.ps
	$(MAN2PDF) -o $@ $<

pdf/mleak.pdf: ps/mleak.ps
	$(MAN2PDF) -o $@ $<

pdf/mpsym.pdf: ps/mpsym.ps
	$(MAN2PDF) -o $@ $<

pdf/mpedit.pdf: ps/mpedit.ps
	$(MAN2PDF) -o $@ $<

pdf/hexwords.pdf: ps/hexwords.ps
	$(MAN2PDF) -o $@ $<

pdf/libmpatrol.pdf: ps/libmpatrol.ps
	$(MAN2PDF) -o $@ $<

pdf/libmpalloc.pdf: ps/libmpalloc.ps
	$(MAN2PDF) -o $@ $<

all: $(CATPAGES) $(HTMLPAGES) $(DVIPAGES) $(PSPAGES) $(PDFPAGES)

clean:
	-@ rm -f $(CATPAGES) $(HTMLPAGES) >/dev/null 2>&1
	-@ rm -f $(DVIPAGES) $(PSPAGES) $(PDFPAGES) >/dev/null 2>&1

clobber: clean
