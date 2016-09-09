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


# GNU Makefile to build the UNIX manual pages


# $Id: Makefile,v 1.16 2002/01/08 20:28:40 graeme Exp $


# Definitions

SHELL  = /bin/sh
ROFF   = groff
HTML   = man2html
PSTILL = pstill

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
	$(ROFF) -Tascii -man $< >$@

cat1/mprof.1: man1/mprof.1
	$(ROFF) -Tascii -man $< >$@

cat1/mptrace.1: man1/mptrace.1
	$(ROFF) -Tascii -man $< >$@

cat1/mleak.1: man1/mleak.1
	$(ROFF) -Tascii -man $< >$@

cat1/mpsym.1: man1/mpsym.1
	$(ROFF) -Tascii -man $< >$@

cat1/mpedit.1: man1/mpedit.1
	$(ROFF) -Tascii -man $< >$@

cat1/hexwords.1: man1/hexwords.1
	$(ROFF) -Tascii -man -t $< >$@

cat3/libmpatrol.3: man3/libmpatrol.3
	$(ROFF) -Tascii -man -t $< >$@

cat3/libmpalloc.3: man3/libmpalloc.3
	$(ROFF) -Tascii -man $< >$@

html/mpatrol.html: man1/mpatrol.1
	$(HTML) $< >$@

html/mprof.html: man1/mprof.1
	$(HTML) $< >$@

html/mptrace.html: man1/mptrace.1
	$(HTML) $< >$@

html/mleak.html: man1/mleak.1
	$(HTML) $< >$@

html/mpsym.html: man1/mpsym.1
	$(HTML) $< >$@

html/mpedit.html: man1/mpedit.1
	$(HTML) $< >$@

html/hexwords.html: man1/hexwords.1
	$(HTML) $< >$@

html/libmpatrol.html: man3/libmpatrol.3
	$(HTML) $< >$@

html/libmpalloc.html: man3/libmpalloc.3
	$(HTML) $< >$@

dvi/mpatrol.dvi: man1/mpatrol.1
	$(ROFF) -Tdvi -man $< >$@

dvi/mprof.dvi: man1/mprof.1
	$(ROFF) -Tdvi -man $< >$@

dvi/mptrace.dvi: man1/mptrace.1
	$(ROFF) -Tdvi -man $< >$@

dvi/mleak.dvi: man1/mleak.1
	$(ROFF) -Tdvi -man $< >$@

dvi/mpsym.dvi: man1/mpsym.1
	$(ROFF) -Tdvi -man $< >$@

dvi/mpedit.dvi: man1/mpedit.1
	$(ROFF) -Tdvi -man $< >$@

dvi/hexwords.dvi: man1/hexwords.1
	$(ROFF) -Tdvi -man -t $< >$@

dvi/libmpatrol.dvi: man3/libmpatrol.3
	$(ROFF) -Tdvi -man -t $< >$@

dvi/libmpalloc.dvi: man3/libmpalloc.3
	$(ROFF) -Tdvi -man $< >$@

ps/mpatrol.ps: man1/mpatrol.1
	$(ROFF) -Tps -man $< >$@

ps/mprof.ps: man1/mprof.1
	$(ROFF) -Tps -man $< >$@

ps/mptrace.ps: man1/mptrace.1
	$(ROFF) -Tps -man $< >$@

ps/mleak.ps: man1/mleak.1
	$(ROFF) -Tps -man $< >$@

ps/mpsym.ps: man1/mpsym.1
	$(ROFF) -Tps -man $< >$@

ps/mpedit.ps: man1/mpedit.1
	$(ROFF) -Tps -man $< >$@

ps/hexwords.ps: man1/hexwords.1
	$(ROFF) -Tps -man -t $< >$@

ps/libmpatrol.ps: man3/libmpatrol.3
	$(ROFF) -Tps -man -t $< >$@

ps/libmpalloc.ps: man3/libmpalloc.3
	$(ROFF) -Tps -man $< >$@

pdf/mpatrol.pdf: ps/mpatrol.ps
	$(PSTILL) -c -o $@ $<

pdf/mprof.pdf: ps/mprof.ps
	$(PSTILL) -c -o $@ $<

pdf/mptrace.pdf: ps/mptrace.ps
	$(PSTILL) -c -o $@ $<

pdf/mleak.pdf: ps/mleak.ps
	$(PSTILL) -c -o $@ $<

pdf/mpsym.pdf: ps/mpsym.ps
	$(PSTILL) -c -o $@ $<

pdf/mpedit.pdf: ps/mpedit.ps
	$(PSTILL) -c -o $@ $<

pdf/hexwords.pdf: ps/hexwords.ps
	$(PSTILL) -c -o $@ $<

pdf/libmpatrol.pdf: ps/libmpatrol.ps
	$(PSTILL) -c -o $@ $<

pdf/libmpalloc.pdf: ps/libmpalloc.ps
	$(PSTILL) -c -o $@ $<

all: $(CATPAGES) $(HTMLPAGES) $(DVIPAGES) $(PSPAGES) $(PDFPAGES)

clean:
	-@ rm -f $(CATPAGES) $(HTMLPAGES) >/dev/null 2>&1
	-@ rm -f $(DVIPAGES) $(PSPAGES) $(PDFPAGES) >/dev/null 2>&1

clobber: clean
