@echo off


rem mpatrol
rem A library for controlling and tracing dynamic memory allocations.
rem Copyright (C) 1997-2008 Graeme S. Roy <graemeroy@users.sourceforge.net>
rem
rem This program is free software: you can redistribute it and/or modify it
rem under the terms of the GNU Lesser General Public License as published by
rem the Free Software Foundation, either version 3 of the License, or (at
rem your option) any later version.
rem
rem This program is distributed in the hope that it will be useful, but
rem WITHOUT ANY WARRANTY; without even the implied warranty of
rem MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
rem General Public License for more details.
rem
rem You should have received a copy of the GNU Lesser General Public License
rem along with this program.  If not, see <http://www.gnu.org/licenses/>.


rem DOS batch file to build a zip distribution file


rem $Id$


set version=1.5.1


rem Build the mpatrol library.

cd ..\..\build\windows
nmake -f NMakefile clobber
nmake -f NMakefile libmpatrol.lib mpatrol.dll
nmake -f NMakefile libmpatrolmt.lib mpatrolmt.dll
nmake -f NMakefile libmpalloc.lib mpalloc.dll
nmake -f NMakefile libmptools.lib
nmake -f NMakefile mpatrol.exe mprof.exe mptrace.exe mleak.exe
cd ..\..\pkg\zip


rem Create the distribution directory structure and copy the necessary
rem files into it.  This assumes that the Texinfo manual, reference card
rem and manual pages have already been processed into the different
rem documentation formats.

rmdir /s /q mpatrol
mkdir mpatrol
copy ..\..\README mpatrol\README.txt
copy ..\..\AUTHORS mpatrol\AUTHORS.txt
copy ..\..\THANKS mpatrol\THANKS.txt
copy ..\..\COPYING mpatrol\COPYING.txt
copy ..\..\COPYING.LESSER mpatrol\COPYING_LESSER.txt
copy ..\..\NEWS mpatrol\NEWS.txt
copy ..\..\ChangeLog mpatrol\ChangeLog.txt
mkdir mpatrol\bin
copy ..\..\build\windows\mpatrol.exe mpatrol\bin
copy ..\..\build\windows\mprof.exe mpatrol\bin
copy ..\..\build\windows\mptrace.exe mpatrol\bin
copy ..\..\build\windows\mleak.exe mpatrol\bin
copy ..\..\bin\mpsym mpatrol\bin
copy ..\..\bin\mpedit mpatrol\bin
copy ..\..\bin\hexwords mpatrol\bin
copy ..\..\build\windows\mpatrol.dll mpatrol\bin
copy ..\..\build\windows\mpatrolmt.dll mpatrol\bin
copy ..\..\build\windows\mpalloc.dll mpatrol\bin
mkdir mpatrol\doc
copy ..\..\doc\README mpatrol\doc\README.txt
copy ..\..\doc\mpatrol.txt mpatrol\doc
copy ..\..\doc\mpatrol.info mpatrol\doc
copy ..\..\doc\mpatrol.guide mpatrol\doc
copy ..\..\doc\mpatrol.html mpatrol\doc
copy ..\..\doc\mpatrol.dvi mpatrol\doc
copy ..\..\doc\mpatrol.ps mpatrol\doc
copy ..\..\doc\mpatrol.pdf mpatrol\doc
copy ..\..\doc\refcard.dvi mpatrol\doc
copy ..\..\doc\refcard.ps mpatrol\doc
copy ..\..\doc\refcard.pdf mpatrol\doc
mkdir mpatrol\doc\images
copy ..\..\doc\images\*.txt mpatrol\doc\images
copy ..\..\doc\images\*.jpg mpatrol\doc\images
copy ..\..\doc\images\*.eps mpatrol\doc\images
copy ..\..\doc\images\*.pdf mpatrol\doc\images
mkdir mpatrol\doc\man
copy ..\..\man\README mpatrol\doc\man\README.txt
mkdir mpatrol\doc\man\html
copy ..\..\man\html\*.html mpatrol\doc\man\html
mkdir mpatrol\doc\man\dvi
copy ..\..\man\dvi\*.dvi mpatrol\doc\man\dvi
mkdir mpatrol\doc\man\ps
copy ..\..\man\ps\*.ps mpatrol\doc\man\ps
mkdir mpatrol\doc\man\pdf
copy ..\..\man\pdf\*.pdf mpatrol\doc\man\pdf
mkdir mpatrol\include
copy ..\..\src\mpatrol.h mpatrol\include
copy ..\..\src\mpalloc.h mpatrol\include
copy ..\..\src\mpdebug.h mpatrol\include
mkdir mpatrol\include\mpatrol
copy ..\..\tools\*.h mpatrol\include\mpatrol
mkdir mpatrol\lib
copy ..\..\build\windows\libmpatrol.lib mpatrol\lib
copy ..\..\build\windows\libmpatrolmt.lib mpatrol\lib
copy ..\..\build\windows\libmpalloc.lib mpatrol\lib
copy ..\..\build\windows\libmptools.lib mpatrol\lib
copy ..\..\build\windows\mpatrol.lib mpatrol\lib
copy ..\..\build\windows\mpatrol.exp mpatrol\lib
copy ..\..\build\windows\mpatrolmt.lib mpatrol\lib
copy ..\..\build\windows\mpatrolmt.exp mpatrol\lib
copy ..\..\build\windows\mpalloc.lib mpatrol\lib
copy ..\..\build\windows\mpalloc.exp mpatrol\lib
mkdir mpatrol\man
mkdir mpatrol\man\cat1
copy ..\..\man\cat1\*.1 mpatrol\man\cat1
mkdir mpatrol\man\man1
copy ..\..\man\man1\*.1 mpatrol\man\man1
mkdir mpatrol\man\cat3
copy ..\..\man\cat3\*.3 mpatrol\man\cat3
mkdir mpatrol\man\man3
copy ..\..\man\man3\*.3 mpatrol\man\man3
mkdir mpatrol\share
mkdir mpatrol\share\aclocal
copy ..\..\extra\mpatrol.m4 mpatrol\share\aclocal
mkdir mpatrol\tests
mkdir mpatrol\tests\pass
copy ..\..\tests\pass\test*.c mpatrol\tests\pass
mkdir mpatrol\tests\fail
copy ..\..\tests\fail\test*.c mpatrol\tests\fail
mkdir mpatrol\tests\profile
copy ..\..\tests\profile\test*.c mpatrol\tests\profile
mkdir mpatrol\tests\tutorial
copy ..\..\tests\tutorial\test*.c mpatrol\tests\tutorial


rem Create the ZIP distribution archive.

del /f /q mpatrol_%version%.zip
wzzip -P -r mpatrol_%version%.zip mpatrol
rmdir /s /q mpatrol


rem Clean up the build directory.

cd ..\..\build\windows
nmake -f NMakefile clobber
cd ..\..\pkg\zip
