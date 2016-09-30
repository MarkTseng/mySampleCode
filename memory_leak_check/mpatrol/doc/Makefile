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


# GNU Makefile to build all formats of documentation


# $Id$


# Definitions

SHELL = /bin/sh

DOCUMENT = mpatrol
REFCARD  = refcard
SOURCE	 = source

SPLIT      = false
THUMBNAILS = true
VERBOSE    = false


# Rules

.SUFFIXES: .texi .tex .txt .info .guide .xml .html .dvi .ps .pdf .jpg .eps

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

# Build the DocBook file from the Texinfo file using makeinfo

$(DOCUMENT).xml: $(DOCUMENT).texi images/$(DOCUMENT).txt
	@ echo creating $@ from $<...
ifeq ($(VERBOSE),true)
	@ makeinfo --docbook --ifinfo --verbose $<
else
	@ makeinfo --docbook --ifinfo $<
endif

# Build the HTML file from the Texinfo file using makeinfo

$(DOCUMENT).html: $(DOCUMENT).texi images/$(DOCUMENT).jpg images/test.jpg \
	images/gcc.jpg images/cpp.jpg images/cc1.jpg images/collect2.jpg \
	images/trace.jpg
	@ echo creating $@ from $<...
ifeq ($(SPLIT),true)
ifeq ($(VERBOSE),true)
	@ makeinfo --html --number-sections --output pages --verbose $<
else
	@ makeinfo --html --number-sections --output pages $<
endif
	@ echo "<html><head>mpatrol</head><body>" >$@
	@ echo "<a href="pages/index.html">documentation</a>" >>$@
	@ echo "</body></html>" >>$@
else
ifeq ($(VERBOSE),true)
	@ makeinfo --html --no-split --number-sections --verbose $<
else
	@ makeinfo --html --no-split --number-sections $<
endif
endif

# Build the DVI file from the Texinfo file using texi2dvi

$(DOCUMENT).dvi: $(DOCUMENT).texi images/$(DOCUMENT).eps images/test.eps \
	images/gcc.eps images/cpp.eps images/cc1.eps images/collect2.eps \
	images/trace.eps
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
	@ dvips -ta4 -Ppdf -o $@ $<
else
	@ dvips -ta4 -Ppdf -q -o $@ $<
endif

# Build the PDF file from the Texinfo file using texi2dvi

$(DOCUMENT).pdf: $(DOCUMENT).texi images/$(DOCUMENT).pdf images/test.pdf \
	images/gcc.pdf images/cpp.pdf images/cc1.pdf images/collect2.pdf \
	images/trace.pdf
	@ echo creating $@ from $<...
ifeq ($(VERBOSE),true)
	@ TEXPSHEADERS=`pwd`: texi2dvi --pdf --batch --clean --verbose $<
else
	@ TEXPSHEADERS=`pwd`: texi2dvi --pdf --batch --clean --quiet $<
endif
ifeq ($(THUMBNAILS),true)
ifeq ($(VERBOSE),true)
	@ thumbpdf --verbose $@
	@ TEXPSHEADERS=`pwd`: texi2dvi --pdf --batch --clean --verbose $<
	-@ rm -f $(DOCUMENT).tpt >/dev/null 2>&1
else
	@ thumbpdf --quiet $@
	@ TEXPSHEADERS=`pwd`: texi2dvi --pdf --batch --clean --quiet $<
	-@ rm -f $(DOCUMENT).tpt >/dev/null 2>&1
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
	@ dvips -tlandscape -Ppdf -o $@ $<
else
	@ dvips -tlandscape -Ppdf -q -o $@ $<
endif

# Build the PDF reference card from the postscript file using pstill

$(REFCARD).pdf: $(REFCARD).ps
	@ echo creating $@ from $<...
ifeq ($(VERBOSE),true)
	@ pstill -Fa4 -R270 -c -c -p -t -v -o $@ $<
else
	@ pstill -Fa4 -R270 -c -c -p -t -o $@ $<
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
	@ dvips -ta4 -Ppdf -o $@ $<
else
	@ dvips -ta4 -Ppdf -q -o $@ $<
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
	@ TEXPSHEADERS=`pwd`: texi2dvi --language=latex --pdf --batch --clean --verbose $<
	-@ rm -f $(SOURCE).tpt >/dev/null 2>&1
else
	@ thumbpdf --quiet $@
	@ TEXPSHEADERS=`pwd`: texi2dvi --language=latex --pdf --batch --clean --quiet $<
	-@ rm -f $(SOURCE).tpt >/dev/null 2>&1
endif
endif
	-@ rm -rf bin src tools >/dev/null 2>&1

all: $(DOCUMENT).txt $(DOCUMENT).info $(DOCUMENT).guide $(DOCUMENT).xml \
	$(DOCUMENT).html $(DOCUMENT).dvi $(DOCUMENT).ps $(DOCUMENT).pdf \
	$(REFCARD).dvi $(REFCARD).ps $(REFCARD).pdf

source: $(SOURCE).dvi $(SOURCE).ps $(SOURCE).pdf

clean:
	-@ rm -f $(DOCUMENT).txt $(DOCUMENT).info >/dev/null 2>&1
	-@ rm -f $(DOCUMENT).info-* $(DOCUMENT).guide >/dev/null 2>&1
	-@ rm -f $(DOCUMENT).xml $(DOCUMENT).html >/dev/null 2>&1
	-@ rm -rf pages >/dev/null 2>&1
	-@ rm -f $(DOCUMENT).dvi $(DOCUMENT).ps $(DOCUMENT).pdf >/dev/null 2>&1
	-@ rm -f $(REFCARD).dvi $(REFCARD).ps $(REFCARD).pdf >/dev/null 2>&1
	-@ rm -f $(SOURCE).dvi $(SOURCE).ps $(SOURCE).pdf >/dev/null 2>&1

clobber: clean
