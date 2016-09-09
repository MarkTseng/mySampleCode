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


# RPM package specification file


# $Id: mpatrol.spec,v 1.73 2002/01/08 20:22:08 graeme Exp $


%define libversion 1.4


Summary: A library for controlling and tracing dynamic memory allocations.
Name: mpatrol
Version: 1.4.8
Release: 1
License: LGPL
Group: Development/Debuggers
Source0: http://www.cbmamiga.demon.co.uk/mpatrol/files/%{name}_%{version}.tar.gz
URL: http://www.cbmamiga.demon.co.uk/mpatrol/
Buildroot: %{_tmppath}/%{name}-root
Prereq: /sbin/install-info /sbin/ldconfig
Packager: Graeme S. Roy <graeme.roy@analog.com>


%description
A link library that attempts to diagnose run-time errors that are caused
by the wrong use of dynamically allocated memory.  Along with providing a
comprehensive and configurable log of all dynamic memory operations that
occurred during the lifetime of a program, the mpatrol library performs
extensive checking to detect any misuse of dynamically allocated memory.
All of this functionality can be integrated into existing code through
the inclusion of a single header file at compile-time.  All logging and
tracing output from the mpatrol library is sent to a separate log file in
order to keep its diagnostics separate from any that the program being
tested might generate.  A wide variety of library settings can also be
changed at run-time via an environment variable, thus removing the need
to recompile or relink in order to change the library's behaviour.


%prep
%setup -q -n %{name}


%build
cd build/unix
make libmpatrol.a libmpatrol.so.%{libversion}
make libmpatrolmt.a libmpatrolmt.so.%{libversion}
make libmpalloc.a libmpalloc.so.%{libversion}
make libmptools.a
make mpatrol mprof mptrace mleak


%install
rm -rf $RPM_BUILD_ROOT
mv doc/README README.DOC
mv man/README README.MAN
mkdir -p $RPM_BUILD_ROOT/%{_bindir}
install -m755 -s build/unix/mpatrol $RPM_BUILD_ROOT/%{_bindir}
install -m755 -s build/unix/mprof $RPM_BUILD_ROOT/%{_bindir}
install -m755 -s build/unix/mptrace $RPM_BUILD_ROOT/%{_bindir}
install -m755 -s build/unix/mleak $RPM_BUILD_ROOT/%{_bindir}
install -m755 bin/mpsym $RPM_BUILD_ROOT/%{_bindir}
install -m755 bin/mpedit $RPM_BUILD_ROOT/%{_bindir}
install -m755 bin/hexwords $RPM_BUILD_ROOT/%{_bindir}
mv man/html .
mv man/dvi .
mv man/ps .
mv man/pdf .
mkdir -p $RPM_BUILD_ROOT/%{_includedir}
install -m644 src/mpatrol.h $RPM_BUILD_ROOT/%{_includedir}
install -m644 src/mpalloc.h $RPM_BUILD_ROOT/%{_includedir}
install -m644 src/mpdebug.h $RPM_BUILD_ROOT/%{_includedir}
mkdir -p $RPM_BUILD_ROOT/%{_includedir}/mpatrol
install -m644 tools/*.h $RPM_BUILD_ROOT/%{_includedir}/mpatrol
mkdir -p $RPM_BUILD_ROOT/%{_infodir}
install -m644 doc/mpatrol.info* $RPM_BUILD_ROOT/%{_infodir}
mkdir -p $RPM_BUILD_ROOT/%{_libdir}
install -m644 build/unix/libmpatrol.a $RPM_BUILD_ROOT/%{_libdir}
install -m755 build/unix/libmpatrol.so.%{libversion} $RPM_BUILD_ROOT/%{_libdir}
install -m644 build/unix/libmpatrolmt.a $RPM_BUILD_ROOT/%{_libdir}
install -m755 build/unix/libmpatrolmt.so.%{libversion} $RPM_BUILD_ROOT/%{_libdir}
install -m644 build/unix/libmpalloc.a $RPM_BUILD_ROOT/%{_libdir}
install -m755 build/unix/libmpalloc.so.%{libversion} $RPM_BUILD_ROOT/%{_libdir}
install -m644 build/unix/libmptools.a $RPM_BUILD_ROOT/%{_libdir}
mkdir -p $RPM_BUILD_ROOT/%{_mandir}/man1
install -m644 man/man1/*.1 $RPM_BUILD_ROOT/%{_mandir}/man1
mkdir -p $RPM_BUILD_ROOT/%{_mandir}/man3
install -m644 man/man3/*.3 $RPM_BUILD_ROOT/%{_mandir}/man3
mkdir -p $RPM_BUILD_ROOT/usr/share/aclocal
install -m644 extra/mpatrol.m4 $RPM_BUILD_ROOT/usr/share/aclocal


%files
%defattr(-,root,root)
%doc README README.DOC README.MAN AUTHORS THANKS
%doc COPYING COPYING.LIB NEWS ChangeLog
%doc doc/mpatrol.txt doc/mpatrol.guide doc/mpatrol.html
%doc doc/mpatrol.dvi doc/mpatrol.ps doc/mpatrol.pdf
%doc doc/refcard.dvi doc/refcard.ps doc/refcard.pdf
%doc doc/images html dvi ps pdf
%doc tests
%{_bindir}
%{_includedir}
%{_infodir}
%{_libdir}
%{_mandir}
/usr/share/aclocal


%post
/sbin/ldconfig
/sbin/install-info %{_infodir}/mpatrol.info* %{_infodir}/dir


%preun
if [ $1 = 0 ]
then
    /sbin/install-info --delete %{_infodir}/mpatrol.info* %{_infodir}/dir
fi


%postun
if [ $1 = 0 ]
then
    /sbin/ldconfig
fi


%clean
rm -rf $RPM_BUILD_ROOT
