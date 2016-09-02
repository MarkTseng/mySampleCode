SUBDIRS = po

bin_PROGRAMS = memprof
noinst_PROGRAMS = testmemprof
pkgdata_DATA = memprof.glade leak.xpm leak.png memprof.png

pkglib_LTLIBRARIES = libmemintercept.la libspeedintercept.la

testmemprof_SOURCES =		\
	tests/testmemprof.c

memprof_SOURCES =	        \
	src/gui.h		\
	src/inode.c		\
	src/leakdetect.c	\
	src/leakdetect.h	\
	src/main.c		\
	src/memprof.h		\
	src/process.c		\
	src/process.h		\
	src/profile.c		\
	src/profile.h		\
	src/server.c		\
	src/server.h		\
	src/stackstash.c	\
	src/stackstash.h	\
	src/treeviewutils.c	\
	src/treeviewutils.h	\
	src/tree.c		\
	src/binfile.h		\
	src/binfile.c		\
	src/elfparser.h		\
	src/elfparser.c		\
	src/binparser.h		\
	src/binparser.c		\
	src/demangle.c		\
	src/detailwin.c

memprof_LDADD = $(MEMPROF_LIBS)
memprof_LDFLAGS = -export-dynamic

intercept_sources =		\
	lib/intercept.c		\
	lib/intercept.h		\
	lib/memintercept-utils.c\
	lib/memintercept-utils.h\
	lib/stack-frame.c	\
	lib/stack-frame.h


libmemintercept_la_SOURCES =    \
	$(intercept_sources)	\
	lib/memintercept.c	\
	lib/memintercept.h

libmemintercept_la_LDFLAGS = -module -avoid-version
libmemintercept_la_LIBADD = -ldl

libspeedintercept_la_SOURCES =  \
	$(intercept_sources)	\
	lib/mi-perfctr.c	\
	lib/mi-perfctr.h	\
	lib/perfctr.h		\
	lib/speedintercept.c

libspeedintercept_la_LDFLAGS = -module -avoid-version
libspeedintercept_la_LIBADD = -ldl

INCLUDES =					   \
	$(MEMPROF_CFLAGS)			   \
	-I$(top_srcdir)/lib			   \
	-DDATADIR=\"$(pkgdatadir)\"		   \
	-DPKGLIBDIR=\"$(pkglibdir)\"		   \
	-DLOCALEDIR=\""$(datadir)/locale"\"

EXTRA_DIST =			\
	memprof.desktop		\
	leak.png		\
	leak.xpm		\
	memprof.glade		\
	memprof.spec.in		\
	memprof.png	        \
	intltool-extract.in	\
	intltool-merge.in	\
	intltool-update.in	\
	$(schema_DATA)

Developmentdir = $(datadir)/applications
Development_DATA = memprof.desktop

pixmapsdir = $(datadir)/pixmaps
pixmaps_DATA = memprof.png

DISTCLEANFILES =		\
	intltool-extract	\
	intltool-merge		\
	intltool-update

install-exec-hook:
	cd $(DESTDIR)$(bindir) && ln -sf memprof speedprof

.PHONY: copyrights

copyrights:
	FILES="$(memprof_SOURCES) $(libmemintercept_la_SOURCES)";	  \
	for i in $$FILES ; do						  \
           mv $$i $$i.bak;						  \
           cat copyright-header > $$i;					  \
	   if grep "/\*====\*/" $$i.bak ; then                            \
	     perl -ne 'print if m!/\*====\*/!..0' $$i.bak >> $$i;	  \
	   else								  \
	     echo "/*====*/" >> $$	i;				  \
             cat $$i.bak >> $$i;					  \
	   fi;								  \
	done

dist-hook: memprof.spec
	if grep '^- Version @VERSION@' $(srcdir)/memprof.spec.in ; then : ; else \
	   perl -ni.bak -e \
	      'print; if (/^%changelog/) { chomp ($$date = `date +"%a %b %d %Y"`); print "* $$date Holger Hans Peter Freyther <zecke+memprof\@secretlabs.de>\n- Version @VERSION@\n\n"; }' \
	      $(srcdir)/memprof.spec.in ; \
	   ( CONFIG_FILES=memprof.spec CONFIG_HEADERS= $(SHELL) ./config.status ) ; \
	   cp -p $(srcdir)/memprof.spec.in $(distdir) ; \
	fi
	cp memprof.spec $(distdir)
