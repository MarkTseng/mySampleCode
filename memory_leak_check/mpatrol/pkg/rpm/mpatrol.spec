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


# RPM package specification file


# $Id$


%define libversion 1.5


Summary: A library for controlling and tracing dynamic memory allocations.
Name: mpatrol
Version: 1.5.1
Release: 1
License: LGPL
Group: Development/Debuggers
Source0: http://heanet.dl.sourceforge.net/sourceforge/mpatrol/%{name}_%{version}.tar.gz
URL: http://sourceforge.net/projects/mpatrol/
Buildroot: %{_tmppath}/%{name}-root
Prereq: /sbin/install-info /sbin/ldconfig
Packager: Graeme S. Roy <graemeroy@users.sourceforge.net>


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
ln -s libmpatrol.so.%{libversion} libmpatrol.so.1
ln -s libmpatrol.so.1 libmpatrol.so
make libmpatrolmt.a libmpatrolmt.so.%{libversion}
ln -s libmpatrolmt.so.%{libversion} libmpatrolmt.so.1
ln -s libmpatrolmt.so.1 libmpatrolmt.so
make libmpalloc.a libmpalloc.so.%{libversion}
ln -s libmpalloc.so.%{libversion} libmpalloc.so.1
ln -s libmpalloc.so.1 libmpalloc.so
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
mv build/unix/libmpatrol.so.1 $RPM_BUILD_ROOT/%{_libdir}
mv build/unix/libmpatrol.so $RPM_BUILD_ROOT/%{_libdir}
install -m644 build/unix/libmpatrolmt.a $RPM_BUILD_ROOT/%{_libdir}
install -m755 build/unix/libmpatrolmt.so.%{libversion} $RPM_BUILD_ROOT/%{_libdir}
mv build/unix/libmpatrolmt.so.1 $RPM_BUILD_ROOT/%{_libdir}
mv build/unix/libmpatrolmt.so $RPM_BUILD_ROOT/%{_libdir}
install -m644 build/unix/libmpalloc.a $RPM_BUILD_ROOT/%{_libdir}
install -m755 build/unix/libmpalloc.so.%{libversion} $RPM_BUILD_ROOT/%{_libdir}
mv build/unix/libmpalloc.so.1 $RPM_BUILD_ROOT/%{_libdir}
mv build/unix/libmpalloc.so $RPM_BUILD_ROOT/%{_libdir}
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
%doc COPYING COPYING.LESSER NEWS ChangeLog
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
