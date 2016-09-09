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


# GNU Makefile to build all formats of documentation


# $Id: Makefile,v 1.38 2002/01/08 20:45:08 graeme Exp $


# Definitions

SHELL = /bin/sh

DOCUMENT = mpatrol
REFCARD  = refcard
SOURCE	 = source

SPLIT      = false
THUMBNAILS = true
VERBOSE    = false


# Rules

.SUFFIXES: .texi .tex .txt .info .guide .html .dvi .ps .pdf .jpg .eps

default: $(DOCUMENT).txt

# Build the text file from the Texinfo file using makeinfo

$(DOCUMENT).txt: $(DOCUMENT).texi images/$(DOCUMENT).txt
	@ echo creating $@ from $<...
ifeq ($(VERBOSE),true)
	@ makeinfo --no-headers --number-sections --verbose $< >$@
else
	@ makeinfo --no-headers --number-sections $< >$@
endif

# Build the info file from the Texinfo file using makeinfo

$(DOCUMENT).info: $(DOCUMENT).texi images/$(DOCUMENT).txt
	@ echo creating $@ from $<...
ifeq ($(SPLIT),true)
ifeq ($(VERBOSE),true)
	@ makeinfo --verbose $<
else
	@ makeinfo $<
endif
else
ifeq ($(VERBOSE),true)
	@ makeinfo --no-split --verbose $<
else
	@ makeinfo --no-split $<
endif
endif

# Build the AmigaGuide file from the Texinfo file using makeguide

$(DOCUMENT).guide: $(DOCUMENT).texi images/$(DOCUMENT).txt
	@ echo creating $@ from $<...
ifeq ($(VERBOSE),true)
	@ makeguide --amiga --verbose $<
else
	@ makeguide --amiga $<
endif

ifeq ($(SPLIT),true)
# Build the HTML files from the Texinfo file using texi2html

$(DOCUMENT).html: $(DOCUMENT).texi images/$(DOCUMENT).jpg images/test.jpg \
	images/gcc.jpg images/cpp.jpg images/cc1.jpg images/collect2.jpg \
	images/trace.jpg
	@ echo creating $@ from $<...
ifeq ($(VERBOSE),true)
	@ texi2html -top_file $@ -expand none -split section -menu -number -verbose $<
else
	@ texi2html -top_file $@ -expand none -split section -menu -number $<
endif
else
# Build the HTML file from the Texinfo file using makeinfo

$(DOCUMENT).html: $(DOCUMENT).texi images/$(DOCUMENT).jpg images/test.jpg \
	images/gcc.jpg images/cpp.jpg images/cc1.jpg images/collect2.jpg \
	images/trace.jpg
	@ echo creating $@ from $<...
ifeq ($(VERBOSE),true)
	@ makeinfo --html --no-split --number-sections --verbose $<
else
	@ makeinfo --html --no-split --number-sections $<
endif
	@ sed "s/<[ ]*<[ ]*[Pp][ ]*>[ ]*[Ii][Mm][Gg][ ]*/<img /g" $@ >$@.new
	@ mv $@.new $@
endif

# Build the DVI file from the Texinfo file using texi2dvi

$(DOCUMENT).dvi: $(DOCUMENT).texi texinfo.tex images/$(DOCUMENT).eps \
	images/test.eps images/gcc.eps images/cpp.eps images/cc1.eps \
	images/collect2.eps images/trace.eps
	@ echo creating $@ from $<...
ifeq ($(VERBOSE),true)
	@ TEXPSHEADERS=`pwd`: texi2dvi --batch --clean --verbose $<
else
	@ TEXPSHEADERS=`pwd`: texi2dvi --batch --clean --quiet $<
endif

# Build the postscript file from the DVI file using dvips

$(DOCUMENT).ps: $(DOCUMENT).dvi
	@ echo creating $@ from $<...
ifeq ($(VERBOSE),true)
	@ dvips -ta4 -Pcmz -o $@ $<
else
	@ dvips -ta4 -Pcmz -q -o $@ $<
endif

# Build the PDF file from the Texinfo file using texi2dvi

$(DOCUMENT).pdf: $(DOCUMENT).texi texinfo.tex images/$(DOCUMENT).pdf \
	images/test.pdf images/gcc.pdf images/cpp.pdf images/cc1.pdf \
	images/collect2.pdf images/trace.pdf
	@ echo creating $@ from $<...
ifeq ($(VERBOSE),true)
	@ TEXPSHEADERS=`pwd`: texi2dvi --pdf --batch --clean --verbose $<
else
	@ TEXPSHEADERS=`pwd`: texi2dvi --pdf --batch --clean --quiet $<
endif
ifeq ($(THUMBNAILS),true)
ifeq ($(VERBOSE),true)
	@ thumbpdf --verbose $@
	-@ rm -f $@ thumb*.png thumbpdf.pdf thumbpdf.log >/dev/null 2>&1
	@ TEXPSHEADERS=`pwd`: texi2dvi --pdf --batch --clean --verbose $<
	-@ rm -f thumbdta.tex >/dev/null 2>&1
else
	@ thumbpdf --quiet $@
	-@ rm -f $@ thumb*.png thumbpdf.pdf thumbpdf.log >/dev/null 2>&1
	@ TEXPSHEADERS=`pwd`: texi2dvi --pdf --batch --clean --quiet $<
	-@ rm -f thumbdta.tex >/dev/null 2>&1
endif
endif

# Build the DVI reference card from the LaTeX file using texi2dvi

$(REFCARD).dvi: $(REFCARD).tex
	@ echo creating $@ from $<...
ifeq ($(VERBOSE),true)
	@ texi2dvi --language=latex --batch --clean --verbose $<
else
	@ texi2dvi --language=latex --batch --clean --quiet $<
endif

# Build the postscript reference card from the DVI file using dvips

$(REFCARD).ps: $(REFCARD).dvi
	@ echo creating $@ from $<...
ifeq ($(VERBOSE),true)
	@ dvips -tlandscape -Pcmz -o $@ $<
else
	@ dvips -tlandscape -Pcmz -q -o $@ $<
endif

# Build the PDF reference card from the postscript file using pstill

$(REFCARD).pdf: $(REFCARD).ps
	@ echo creating $@ from $<...
ifeq ($(VERBOSE),true)
	@ pstill -Fa4 -R270 -ctv -o $@ $<
else
	@ pstill -Fa4 -R270 -ct -o $@ $<
endif

# Build the DVI source code from the LaTeX file using texi2dvi

$(SOURCE).dvi: $(SOURCE).tex
	@ echo creating $@ from $<...
	-@ rm -rf bin src tools >/dev/null 2>&1
	@ mkdir bin src tools
	@ for file in ../bin/[a-z]*; do lgrind -i -c -lsh -o bin/`basename $$file` $$file; done
	@ for file in ../src/*.[ch]; do lgrind -i -c -lCC -o src/`basename $$file` $$file; done
	@ for file in ../tools/*.[ch]; do lgrind -i -c -lCC -o tools/`basename $$file` $$file; done
ifeq ($(VERBOSE),true)
	@ texi2dvi --language=latex --batch --clean --verbose $<
else
	@ texi2dvi --language=latex --batch --clean --quiet $<
endif
	-@ rm -rf bin src tools >/dev/null 2>&1

# Build the postscript source code from the DVI file using dvips

$(SOURCE).ps: $(SOURCE).dvi
	@ echo creating $@ from $<...
ifeq ($(VERBOSE),true)
	@ dvips -ta4 -Pcmz -o $@ $<
else
	@ dvips -ta4 -Pcmz -q -o $@ $<
endif

# Build the PDF source code from the LaTeX file using texi2dvi

$(SOURCE).pdf: $(SOURCE).tex
	@ echo creating $@ from $<...
	-@ rm -rf bin src tools >/dev/null 2>&1
	@ mkdir bin src tools
	@ for file in ../bin/[a-z]*; do lgrind -i -c -lsh -o bin/`basename $$file` $$file; done
	@ for file in ../src/*.[ch]; do lgrind -i -c -lCC -o src/`basename $$file` $$file; done
	@ for file in ../tools/*.[ch]; do lgrind -i -c -lCC -o tools/`basename $$file` $$file; done
ifeq ($(VERBOSE),true)
	@ texi2dvi --language=latex --pdf --batch --clean --verbose $<
else
	@ texi2dvi --language=latex --pdf --batch --clean --quiet $<
endif
ifeq ($(THUMBNAILS),true)
ifeq ($(VERBOSE),true)
	@ thumbpdf --verbose $@
	-@ rm -f $@ thumb*.png thumbpdf.pdf thumbpdf.log >/dev/null 2>&1
	@ TEXPSHEADERS=`pwd`: texi2dvi --language=latex --pdf --batch --clean --verbose $<
	-@ rm -f thumbdta.tex >/dev/null 2>&1
else
	@ thumbpdf --quiet $@
	-@ rm -f $@ thumb*.png thumbpdf.pdf thumbpdf.log >/dev/null 2>&1
	@ TEXPSHEADERS=`pwd`: texi2dvi --language=latex --pdf --batch --clean --quiet $<
	-@ rm -f thumbdta.tex >/dev/null 2>&1
endif
endif
	-@ rm -rf bin src tools >/dev/null 2>&1

all: $(DOCUMENT).txt $(DOCUMENT).info $(DOCUMENT).guide $(DOCUMENT).html \
	$(DOCUMENT).dvi $(DOCUMENT).ps $(DOCUMENT).pdf $(REFCARD).dvi \
	$(REFCARD).ps $(REFCARD).pdf

source: $(SOURCE).dvi $(SOURCE).ps $(SOURCE).pdf

clean:
	-@ rm -f $(DOCUMENT).txt $(DOCUMENT).info >/dev/null 2>&1
	-@ rm -f $(DOCUMENT).info-* >/dev/null 2>&1
	-@ rm -f $(DOCUMENT).guide $(DOCUMENT).html >/dev/null 2>&1
	-@ rm -f $(DOCUMENT)_*.html >/dev/null 2>&1
	-@ rm -f $(DOCUMENT).dvi $(DOCUMENT).ps $(DOCUMENT).pdf >/dev/null 2>&1
	-@ rm -f $(REFCARD).dvi $(REFCARD).ps $(REFCARD).pdf >/dev/null 2>&1
	-@ rm -f $(SOURCE).dvi $(SOURCE).ps $(SOURCE).pdf >/dev/null 2>&1

clobber: clean
