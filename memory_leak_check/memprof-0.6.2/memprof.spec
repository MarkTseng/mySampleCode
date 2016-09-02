Summary: Tool for memory profiling and leak detection
Name: memprof
Version: 0.6.2
Release: 1
# Serial is one for historical Red Hat reasons
Serial: 1
License: GPL
Group: Development/Debuggers
Source: memprof-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-root
ExclusiveArch: i386

BuildRequires: libgnomeui-devel >= 2.0.0
BuildRequires: libglade2-devel >= 2.0.0

%description
Memprof is a tool for profiling memory usage and
detecting memory leaks. It can be used with existing
binaries without need for recompilation.
%prep
%setup -q

%build
%configure
make

%install
rm -rf $RPM_BUILD_ROOT
export GCONF_DISABLE_MAKEFILE_SCHEMA_INSTALL=1
make DESTDIR=$RPM_BUILD_ROOT install
unset GCONF_DISABLE_MAKEFILE_SCHEMA_INSTALL

%find_lang %name

%clean
rm -rf $RPM_BUILD_ROOT

%post
export GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source`
SCHEMAS="memprof.schemas"
for S in $SCHEMAS; do
  gconftool-2 --makefile-install-rule %{_sysconfdir}/gconf/schemas/$S > /dev/null
done

%files -f %{name}.lang
%defattr(-, root, root)

%doc README AUTHORS NEWS
%{_bindir}/*
%{_libdir}/libmemintercept.so
%{_datadir}/pixmaps/memprof.png
%{_datadir}/applications/memprof.desktop
%{_datadir}/memprof
%{_sysconfdir}/gconf/schemas/*.schemas

%changelog
* Mo Okt 26 2009 Holger Hans Peter Freyther <zecke+memprof@secretlabs.de>
- Version 0.6.2

* Mo Jun 08 2009 Holger Hans Peter Freyther <zecke+memprof@secretlabs.de>
- Version 0.6.1

* Thu Jun 19 2003 Owen Taylor <otaylor@redhat.com>
- Version 0.5.1-ssp

* Sat Aug 31 2002 Owen Taylor <otaylor@redhat.com>
- Version 0.5.1

* Sat Aug 10 2002 Owen Taylor <otaylor@redhat.com>
- Cleanups from Red Hat spec file, minor fixes.

* Sat Aug 10 2002 Kristian Rietveld <kris@gtk.org>
- Version 0.5.0
- Updates deps, files and handle installation of gconf schemas.

* Fri Feb 09 2001 Owen Taylor <otaylor@redhat.com>
- Version 0.4.1

* Sat Jul 08 2000 Owen Taylor <otaylor@redhat.com>
- Some cleanups, reenable local files

* Wed Nov 10 1999 Owen Taylor <otaylor@redhat.com>
  (From Nils Philippsen <nils@wombat.dialup.fht-esslingen.de>)
- Added %defattr() so that files are owned by root
- Only require libglade >= 0.7, not >= 0.7-1
  (From mkj@redhat.com)
- ExclusiveArch: i386
- Changed group to Development/Debuggers

* Wed Oct 27 1999 Owen Taylor <otaylor@redhat.com>
- Initial package
