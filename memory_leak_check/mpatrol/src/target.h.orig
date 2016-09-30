#ifndef MP_TARGET_H
#define MP_TARGET_H


/*
 * mpatrol
 * A library for controlling and tracing dynamic memory allocations.
 * Copyright (C) 1997-2008 Graeme S. Roy <graemeroy@users.sourceforge.net>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


/*
 * Target definitions.  These are automatically deduced from the
 * preprocessor macros defined by the compiler on the host system,
 * but they can also be overridden.
 */


/*
 * $Id$
 */


#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif /* HAVE_CONFIG_H */


/* The target operating system.  This determines how the mpatrol library
 * should interact with the underlying system, and how it should work around
 * deficiencies in some operating systems.
 */

#define TARGET_ANY     0 /* no specific operating system */
#define TARGET_UNIX    1 /* UNIX or UNIX-like */
#define TARGET_AMIGA   2 /* Commodore AmigaOS */
#define TARGET_WINDOWS 3 /* Microsoft Windows 95/98/ME/NT/2000 */
#define TARGET_NETWARE 4 /* Novell Netware */


#ifndef TARGET
#if defined(unix) || defined(_unix) || defined(__unix) || defined(__unix__) || \
    defined(AIX) || defined(_AIX) || defined(__AIX) || defined(__AIX__) || \
    defined(__Lynx) || defined(__Lynx__) || defined(__INTERIX) || \
    defined(__INTERIX__)
#define TARGET TARGET_UNIX
#elif defined(AMIGA) || defined(_AMIGA) || defined(__AMIGA) || \
      defined(__AMIGA__)
#define TARGET TARGET_AMIGA
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32) || \
      defined(__WIN32__)
#define TARGET TARGET_WINDOWS
#elif defined(NETWARE) || defined(_NETWARE) || defined(__NETWARE) || \
      defined(__NETWARE__)
#define TARGET TARGET_NETWARE
#else /* TARGET */
#define TARGET TARGET_ANY
#endif /* TARGET */
#endif /* TARGET */


/* The string representation of the target operating system.
 */

#if TARGET == TARGET_UNIX
#define TARGET_STR "UNIX"
#elif TARGET == TARGET_AMIGA
#define TARGET_STR "Commodore AmigaOS"
#elif TARGET == TARGET_WINDOWS
#define TARGET_STR "Microsoft Windows NT/2000/XP/Vista"
#elif TARGET == TARGET_NETWARE
#define TARGET_STR "Novell Netware"
#else /* TARGET */
#define TARGET_STR "Unknown"
#endif /* TARGET */


/* The UNIX system variant.  This is only needed on UNIX systems due to the
 * wide variety of vendors and their UNIX systems that support different
 * features.
 */

#define SYSTEM_ANY      0  /* no specific system */
#define SYSTEM_AIX      1  /* AIX */
#define SYSTEM_CYGWIN   2  /* Cygwin */
#define SYSTEM_DGUX     3  /* DG/UX */
#define SYSTEM_DRSNX    4  /* DRS/NX */
#define SYSTEM_DYNIX    5  /* DYNIX/ptx */
#define SYSTEM_FREEBSD  6  /* FreeBSD */
#define SYSTEM_HPUX     7  /* HP/UX */
#define SYSTEM_INTERIX  8  /* Interix / OpenNT / SFU / SUA */
#define SYSTEM_IRIX     9  /* IRIX */
#define SYSTEM_LINUX    10 /* Linux */
#define SYSTEM_LYNXOS   11 /* LynxOS */
#define SYSTEM_NETBSD   12 /* NetBSD */
#define SYSTEM_OPENBSD  13 /* OpenBSD */
#define SYSTEM_SINIX    14 /* SINIX */
#define SYSTEM_SOLARIS  15 /* Solaris */
#define SYSTEM_SUNOS    16 /* SunOS */
#define SYSTEM_TRU64    17 /* Compaq Tru64 / Digital UNIX / OSF/1 */
#define SYSTEM_UNIXWARE 18 /* UnixWare */


#ifndef SYSTEM
#if TARGET == TARGET_UNIX
#if defined(AIX) || defined(_AIX) || defined(__AIX) || defined(__AIX__)
#define SYSTEM SYSTEM_AIX
#elif defined(CYGWIN) || defined(_CYGWIN) || defined(__CYGWIN) || \
      defined(__CYGWIN__)
#define SYSTEM SYSTEM_CYGWIN
#elif defined(DGUX) || defined(_DGUX) || defined(__DGUX) || defined(__DGUX__)
#define SYSTEM SYSTEM_DGUX
#elif defined(sequent) || defined(_sequent) || defined(__sequent) || \
      defined(__sequent__) || defined(SEQUENT) || defined(_SEQUENT_)
#define SYSTEM SYSTEM_DYNIX
#elif defined(__FreeBSD) || defined(__FreeBSD__)
#define SYSTEM SYSTEM_FREEBSD
#elif defined(hpux) || defined(_hpux) || defined(__hpux) || defined(__hpux__)
#define SYSTEM SYSTEM_HPUX
#elif defined(__INTERIX) || defined(__INTERIX__)
#define SYSTEM SYSTEM_INTERIX
#elif defined(sgi) || defined(_sgi) || defined(__sgi) || defined(__sgi__)
#define SYSTEM SYSTEM_IRIX
#elif defined(linux) || defined(_linux) || defined(__linux) || \
      defined(__linux__)
#define SYSTEM SYSTEM_LINUX
#elif defined(__Lynx) || defined(__Lynx__)
#define SYSTEM SYSTEM_LYNXOS
#elif defined(__NetBSD) || defined(__NetBSD__)
#define SYSTEM SYSTEM_NETBSD
#elif defined(__OpenBSD) || defined(__OpenBSD__)
#define SYSTEM SYSTEM_OPENBSD
#elif defined(sinix) || defined(_sinix) || defined(__sinix) || \
      defined(__sinix__) || defined(SNI) || defined(_SNI) || defined(__SNI) || \
      defined(__SNI__)
#define SYSTEM SYSTEM_SINIX
#elif defined(sun) || defined(_sun) || defined(__sun) || defined(__sun__)
#if defined(svr4) || defined(_svr4) || defined(__svr4) || defined(__svr4__) || \
    defined(SVR4) || defined(_SVR4) || defined(__SVR4) || defined(__SVR4__)
#define SYSTEM SYSTEM_SOLARIS
#else /* svr4 */
#define SYSTEM SYSTEM_SUNOS
#endif /* svr4 */
#elif defined(digital) || defined(_digital) || defined(__digital) || \
      defined(__digital__) || defined(osf) || defined(_osf) || \
      defined(__osf) || defined(__osf__)
#define SYSTEM SYSTEM_TRU64
#else /* SYSTEM */
#define SYSTEM SYSTEM_ANY
#endif /* SYSTEM */
#else /* TARGET */
#define SYSTEM SYSTEM_ANY
#endif /* TARGET */
#endif /* SYSTEM */


/* The string representation of the UNIX system variant.
 */

#if TARGET == TARGET_UNIX
#if SYSTEM == SYSTEM_AIX
#define SYSTEM_STR "AIX"
#elif SYSTEM == SYSTEM_CYGWIN
#define SYSTEM_STR "Cygwin"
#elif SYSTEM == SYSTEM_DGUX
#define SYSTEM_STR "DG/UX"
#elif SYSTEM == SYSTEM_DRSNX
#define SYSTEM_STR "DRS/NX"
#elif SYSTEM == SYSTEM_DYNIX
#define SYSTEM_STR "DYNIX/ptx"
#elif SYSTEM == SYSTEM_FREEBSD
#define SYSTEM_STR "FreeBSD"
#elif SYSTEM == SYSTEM_HPUX
#define SYSTEM_STR "HP/UX"
#elif SYSTEM == SYSTEM_INTERIX
#define SYSTEM_STR "INTERIX"
#elif SYSTEM == SYSTEM_IRIX
#define SYSTEM_STR "IRIX"
#elif SYSTEM == SYSTEM_LINUX
#define SYSTEM_STR "Linux"
#elif SYSTEM == SYSTEM_LYNXOS
#define SYSTEM_STR "LynxOS"
#elif SYSTEM == SYSTEM_NETBSD
#define SYSTEM_STR "NetBSD"
#elif SYSTEM == SYSTEM_OPENBSD
#define SYSTEM_STR "OpenBSD"
#elif SYSTEM == SYSTEM_SINIX
#define SYSTEM_STR "SINIX"
#elif SYSTEM == SYSTEM_SOLARIS
#define SYSTEM_STR "Solaris"
#elif SYSTEM == SYSTEM_SUNOS
#define SYSTEM_STR "SunOS"
#elif SYSTEM == SYSTEM_TRU64
#define SYSTEM_STR "Tru64"
#elif SYSTEM == SYSTEM_UNIXWARE
#define SYSTEM_STR "UnixWare"
#else /* SYSTEM */
#define SYSTEM_STR "Unknown"
#endif /* SYSTEM */
#else /* TARGET */
#define SYSTEM_STR "N/A"
#endif /* TARGET */


/* The processor architecture.  This defines families of compatible processors
 * from chip manufacturers rather than specific processor models.
 */

#define ARCH_ANY     0  /* no specific architecture */
#define ARCH_ALPHA   1  /* DEC Alpha */
#define ARCH_ARM     2  /* Acorn/Advanced RISC Machine */
#define ARCH_IA64    3  /* Intel Itanium */
#define ARCH_IX86    4  /* Intel 80x86 */
#define ARCH_M68K    5  /* Motorola 680x0 */
#define ARCH_M88K    6  /* Motorola 88xx0 */
#define ARCH_MIPS    7  /* MIPS */
#define ARCH_PARISC  8  /* HP PA/RISC */
#define ARCH_POWER   9  /* IBM RS/6000 */
#define ARCH_POWERPC 10 /* PowerPC */
#define ARCH_SPARC   11 /* SPARC */


#ifndef ARCH
#if defined(ALPHA) || defined(_ALPHA) || defined(__ALPHA) || \
    defined(__ALPHA__) || defined(alpha) || defined(_alpha) || \
    defined(__alpha) || defined(__alpha__) || defined(_M_ALPHA)
#define ARCH ARCH_ALPHA
#define ENVIRON ENVIRON_64
#elif defined(arm) || defined(_arm) || defined(__arm) || defined(__arm__) || \
      defined(ARM) || defined(_ARM) || defined(__ARM) || defined(__ARM__)
#define ARCH ARCH_ARM
#define ENVIRON ENVIRON_32
#elif defined(ia64) || defined(_ia64) || defined(__ia64) || \
      defined(__a64__) || defined(IA64) || defined(_IA64) || \
      defined(__IA64) || defined(__IA64__) || defined(_M_IA64)
#define ARCH ARCH_IA64
#define ENVIRON ENVIRON_64
#elif defined(amd64) || defined(_amd64) || defined(__amd64) || \
      defined(__amd64__) || defined(AMD64) || defined(_AMD64) || \
      defined(__AMD64) || defined(__AMD64__) || defined(x86_64) || \
      defined(_x86_64) || defined(__x86_64) || defined(__x86_64__)
#define ARCH ARCH_IX86
#define ENVIRON ENVIRON_64
#elif defined(i386) || defined(_i386) || defined(__i386) || \
      defined(__i386__) || defined(I386) || defined(_I386) || \
      defined(__I386) || defined(__I386__) || defined(ix86) || \
      defined(_ix86) || defined(__ix86) || defined(__ix86__) || \
      defined(x86) || defined(_x86) || defined(__x86) || defined(__x86__) || \
      defined(_M_IX86)
#define ARCH ARCH_IX86
#define ENVIRON ENVIRON_32
#elif defined(m68k) || defined(_m68k) || defined(__m68k) || \
      defined(__m68k__) || defined(mc68000) || defined(_mc68000) || \
      defined(__mc68000) || defined(__mc68000__) || defined(M68000) || \
      defined(_M68000) || defined(__M68000) || defined(__M68000__)
#define ARCH ARCH_M68K
#define ENVIRON ENVIRON_32
#elif defined(m88k) || defined(_m88k) || defined(__m88k) || \
      defined(__m88k__) || defined(m88000) || defined(_m88000) || \
      defined(__m88000) || defined(__m88000__)
#define ARCH ARCH_M88K
#define ENVIRON ENVIRON_32
#elif defined(mips) || defined(_mips) || defined(__mips) || \
      defined(__mips__) || defined(_M_MRX000)
#define ARCH ARCH_MIPS
#elif defined(hppa) || defined(_hppa) || defined(__hppa) || defined(__hppa__)
#define ARCH ARCH_PARISC
#elif defined(POWER) || defined(_POWER) || defined(__POWER) || \
      defined(__POWER__)
#define ARCH ARCH_POWER
#define ENVIRON ENVIRON_32
#elif defined(ppc) || defined(_ppc) || defined(__ppc) || defined(__ppc__) || \
      defined(powerpc) || defined(_powerpc) || defined(__powerpc) || \
      defined(__powerpc__) || defined(POWERPC) || defined(_POWERPC) || \
      defined(__POWERPC) || defined(__POWERPC__) || defined(_M_PPC)
#define ARCH ARCH_POWERPC
#elif defined(sparcv9) || defined(_sparcv9) || defined(__sparcv9) || \
      defined(__sparcv9__)
#define ARCH ARCH_SPARC
#define ENVIRON ENVIRON_64
#elif defined(sparc) || defined(_sparc) || defined(__sparc) || \
      defined(__sparc__)
#define ARCH ARCH_SPARC
#define ENVIRON ENVIRON_32
#else /* ARCH */
#define ARCH ARCH_ANY
#endif /* ARCH */
#endif /* ARCH */


/* The string representation of the processor architecture.
 */

#if ARCH == ARCH_ALPHA
#define ARCH_STR "DEC Alpha"
#elif ARCH == ARCH_ARM
#define ARCH_STR "ARM"
#elif ARCH == ARCH_IA64
#define ARCH_STR "Intel Itanium"
#elif ARCH == ARCH_IX86
#define ARCH_STR "Intel 80x86"
#elif ARCH == ARCH_M68K
#define ARCH_STR "Motorola 680x0"
#elif ARCH == ARCH_M88K
#define ARCH_STR "Motorola 88xx0"
#elif ARCH == ARCH_MIPS
#define ARCH_STR "MIPS"
#elif ARCH == ARCH_PARISC
#define ARCH_STR "HP PA/RISC"
#elif ARCH == ARCH_POWER
#define ARCH_STR "IBM RS/6000"
#elif ARCH == ARCH_POWERPC
#define ARCH_STR "PowerPC"
#elif ARCH == ARCH_SPARC
#define ARCH_STR "SPARC"
#else /* ARCH */
#define ARCH_STR "Unknown"
#endif /* ARCH */


/* The processor word size.  This is used to determine the size of pointers
 * and long integers on the target processor.
 */

#define ENVIRON_ANY 0 /* no specific word size */
#define ENVIRON_32  1 /* 32-bit */
#define ENVIRON_64  2 /* 64-bit */


#ifndef ENVIRON
#if defined(HAVE_CONFIG_H) && defined(SIZEOF_VOID_P)
#if SIZEOF_VOID_P == 4
#define ENVIRON ENVIRON_32
#elif SIZEOF_VOID_P == 8
#define ENVIRON ENVIRON_64
#else /* SIZEOF_VOID_P */
#define ENVIRON ENVIRON_ANY
#endif /* SIZEOF_VOID_P */
#else /* HAVE_CONFIG_H && SIZEOF_VOID_P */
#if defined(_ILP32)
#define ENVIRON ENVIRON_32
#elif defined(_LP64)
#define ENVIRON ENVIRON_64
#else /* _ILP32 && _LP64 */
#if SYSTEM == SYSTEM_IRIX
#if defined(ABI64) || defined(_ABI64)
#define ENVIRON ENVIRON_64
#else /* ABI64 */
#define ENVIRON ENVIRON_32
#endif /* ABI64 */
#elif SYSTEM == SYSTEM_TRU64
#if defined(arch64) || defined(_arch64) || defined(__arch64) || \
    defined(__arch64__)
#define ENVIRON ENVIRON_64
#else /* arch64 */
#define ENVIRON ENVIRON_32
#endif /* arch64 */
#else /* SYSTEM */
#define ENVIRON ENVIRON_32
#endif /* SYSTEM */
#endif /* _ILP32 && _LP64 */
#endif /* HAVE_CONFIG_H && SIZEOF_VOID_P */
#endif /* ENVIRON */


/* The string representation of the processor word size.
 */

#if ENVIRON == ENVIRON_32
#define ENVIRON_STR "32-bit"
#elif ENVIRON == ENVIRON_64
#define ENVIRON_STR "64-bit"
#else /* ENVIRON */
#define ENVIRON_STR "Unknown"
#endif /* ENVIRON */


/* The object file format.  This can either specify an explicit file format
 * for executable files and shared libraries, or specify an object file access
 * library.
 */

#define FORMAT_NONE   0 /* no symbol support */
#define FORMAT_AOUT   1 /* a.out */
#define FORMAT_COFF   2 /* COFF */
#define FORMAT_XCOFF  3 /* XCOFF */
#define FORMAT_PECOFF 4 /* PE-COFF */
#define FORMAT_ELF32  5 /* ELF32 */
#define FORMAT_ELF64  6 /* ELF64 */
#define FORMAT_BFD    7 /* GNU BFD */
#define FORMAT_IMGHLP 8 /* Microsoft IMAGEHLP */


#ifndef FORMAT
#if TARGET == TARGET_UNIX
#if SYSTEM == SYSTEM_AIX
#define FORMAT FORMAT_XCOFF
#elif SYSTEM == SYSTEM_INTERIX
#define FORMAT FORMAT_PECOFF
#elif SYSTEM == SYSTEM_DGUX || SYSTEM == SYSTEM_DRSNX || \
      SYSTEM == SYSTEM_DYNIX || SYSTEM == SYSTEM_IRIX || \
      SYSTEM == SYSTEM_SINIX || SYSTEM == SYSTEM_SOLARIS || \
      SYSTEM == SYSTEM_UNIXWARE
#if ENVIRON == ENVIRON_64
#define FORMAT FORMAT_ELF64
#else /* ENVIRON */
#define FORMAT FORMAT_ELF32
#endif /* ENVIRON */
#elif SYSTEM == SYSTEM_FREEBSD || SYSTEM == SYSTEM_NETBSD || \
      SYSTEM == SYSTEM_OPENBSD || SYSTEM == SYSTEM_SUNOS
#ifdef __ELF__
#if ENVIRON == ENVIRON_64
#define FORMAT FORMAT_ELF64
#else /* ENVIRON */
#define FORMAT FORMAT_ELF32
#endif /* ENVIRON */
#else /* __ELF__ */
#define FORMAT FORMAT_AOUT
#endif /* __ELF__ */
#elif SYSTEM == SYSTEM_CYGWIN || SYSTEM == SYSTEM_HPUX || \
      SYSTEM == SYSTEM_LINUX || SYSTEM == SYSTEM_LYNXOS || \
      SYSTEM == SYSTEM_TRU64
#define FORMAT FORMAT_BFD
#else /* SYSTEM */
#define FORMAT FORMAT_NONE
#endif /* SYSTEM */
#elif TARGET == TARGET_WINDOWS
#ifdef __MINGW32__
#define FORMAT FORMAT_BFD
#else /* __MINGW32__ */
#define FORMAT FORMAT_IMGHLP
#endif /* __MINGW32__ */
#else /* TARGET */
#if TARGET == TARGET_AMIGA && defined(__GNUC__)
#define FORMAT FORMAT_BFD
#else /* TARGET && __GNUC__ */
#define FORMAT FORMAT_NONE
#endif /* TARGET && __GNUC__ */
#endif /* TARGET */
#endif /* FORMAT */


/* The string representation of the object file format.
 */

#if FORMAT == FORMAT_AOUT
#define FORMAT_STR "a.out"
#elif FORMAT == FORMAT_COFF
#define FORMAT_STR "COFF"
#elif FORMAT == FORMAT_XCOFF
#define FORMAT_STR "XCOFF"
#elif FORMAT == FORMAT_PECOFF
#define FORMAT_STR "PE-COFF"
#elif FORMAT == FORMAT_ELF32
#define FORMAT_STR "ELF32"
#elif FORMAT == FORMAT_ELF64
#define FORMAT_STR "ELF64"
#elif FORMAT == FORMAT_BFD
#define FORMAT_STR "BFD"
#elif FORMAT == FORMAT_IMGHLP
#define FORMAT_STR "IMAGEHLP"
#else /* FORMAT */
#define FORMAT_STR "Unknown"
#endif /* FORMAT */


/* The dynamic linker type.  This is used to specify the method used to obtain
 * information about the shared libraries that a program requires when it is
 * running.
 */

#define DYNLINK_NONE    0 /* no dynamic linker support */
#define DYNLINK_AIX     1 /* AIX dynamic linker */
#define DYNLINK_BSD     2 /* BSD dynamic linker */
#define DYNLINK_HPUX    3 /* HP/UX dynamic linker */
#define DYNLINK_INTERIX 4 /* Interix dynamic linker */
#define DYNLINK_IRIX    5 /* IRIX dynamic linker */
#define DYNLINK_OSF     6 /* OSF dynamic linker */
#define DYNLINK_SVR4    7 /* SVR4 dynamic linker */
#define DYNLINK_WINDOWS 8 /* Windows dynamic linker */


#ifndef DYNLINK
#if TARGET == TARGET_UNIX
#if SYSTEM == SYSTEM_AIX
#define DYNLINK DYNLINK_AIX
#elif SYSTEM == SYSTEM_CYGWIN
#define DYNLINK DYNLINK_WINDOWS
#elif SYSTEM == SYSTEM_DGUX || SYSTEM == SYSTEM_DRSNX || \
      SYSTEM == SYSTEM_DYNIX || SYSTEM == SYSTEM_LINUX || \
      SYSTEM == SYSTEM_SINIX || SYSTEM == SYSTEM_SOLARIS || \
      SYSTEM == SYSTEM_UNIXWARE
#define DYNLINK DYNLINK_SVR4
#elif SYSTEM == SYSTEM_FREEBSD || SYSTEM == SYSTEM_NETBSD || \
      SYSTEM == SYSTEM_OPENBSD || SYSTEM == SYSTEM_SUNOS
#ifdef __ELF__
#define DYNLINK DYNLINK_SVR4
#else /* __ELF__ */
#define DYNLINK DYNLINK_BSD
#endif /* __ELF__ */
#elif SYSTEM == SYSTEM_HPUX
#define DYNLINK DYNLINK_HPUX
#elif SYSTEM == SYSTEM_INTERIX
#define DYNLINK DYNLINK_INTERIX
#elif SYSTEM == SYSTEM_IRIX
#define DYNLINK DYNLINK_IRIX
#elif SYSTEM == SYSTEM_TRU64
#define DYNLINK DYNLINK_OSF
#else /* SYSTEM */
#define DYNLINK DYNLINK_NONE
#endif /* SYSTEM */
#elif TARGET == TARGET_WINDOWS
#define DYNLINK DYNLINK_WINDOWS
#else /* TARGET */
#define DYNLINK DYNLINK_NONE
#endif /* TARGET */
#endif /* DYNLINK */


/* The string representation of the dynamic linker type.
 */

#if DYNLINK == DYNLINK_AIX
#define DYNLINK_STR "AIX"
#elif DYNLINK == DYNLINK_BSD
#define DYNLINK_STR "BSD"
#elif DYNLINK == DYNLINK_HPUX
#define DYNLINK_STR "HP/UX"
#elif DYNLINK == DYNLINK_INTERIX
#define DYNLINK_STR "INTERIX"
#elif DYNLINK == DYNLINK_IRIX
#define DYNLINK_STR "IRIX"
#elif DYNLINK == DYNLINK_OSF
#define DYNLINK_STR "OSF"
#elif DYNLINK == DYNLINK_SVR4
#define DYNLINK_STR "SVR4"
#elif DYNLINK == DYNLINK_WINDOWS
#define DYNLINK_STR "Windows"
#else /* DYNLINK */
#define DYNLINK_STR "Unknown"
#endif /* DYNLINK */


/* Target feature macros.  Some systems require certain preprocessor macros
 * to be defined before non-standard definitions in system header files are
 * included.
 */

#if TARGET == TARGET_UNIX
#if SYSTEM != SYSTEM_FREEBSD && SYSTEM != SYSTEM_NETBSD && \
    SYSTEM != SYSTEM_OPENBSD
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif /* _POSIX_SOURCE */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199506L
#endif /* _POSIX_C_SOURCE */
#endif /* SYSTEM */
#if SYSTEM == SYSTEM_AIX || SYSTEM == SYSTEM_INTERIX
#ifndef _ALL_SOURCE
#define _ALL_SOURCE 1
#endif /* _ALL_SOURCE */
#elif SYSTEM == SYSTEM_DGUX
#ifndef _DGUX_SOURCE
#define _DGUX_SOURCE 1
#endif /* _DGUX_SOURCE */
#elif SYSTEM == SYSTEM_HPUX
#ifndef _HPUX_SOURCE
#define _HPUX_SOURCE 1
#endif /* _HPUX_SOURCE */
#elif SYSTEM == SYSTEM_IRIX
#ifndef _SGI_SOURCE
#define _SGI_SOURCE 1
#endif /* _SGI_SOURCE */
#elif SYSTEM == SYSTEM_LINUX
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif /* _GNU_SOURCE */
#elif SYSTEM == SYSTEM_SOLARIS
#ifndef __EXTENSIONS__
#define __EXTENSIONS__ 1
#endif /* __EXTENSIONS__ */
#elif SYSTEM == SYSTEM_TRU64
#ifndef _OSF_SOURCE
#define _OSF_SOURCE 1
#endif /* _OSF_SOURCE */
#ifndef _XOPEN_SOURCE_EXTENDED
#define _XOPEN_SOURCE_EXTENDED 1
#endif /* _XOPEN_SOURCE_EXTENDED */
#endif /* SYSTEM */
#endif /* TARGET */


#endif /* MP_TARGET_H */
