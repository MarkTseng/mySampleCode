dnl mpatrol
dnl A library for controlling and tracing dynamic memory allocations.
dnl Copyright (C) 1997-2008 Graeme S. Roy <graemeroy@users.sourceforge.net>
dnl
dnl This program is free software: you can redistribute it and/or modify it
dnl under the terms of the GNU Lesser General Public License as published by
dnl the Free Software Foundation, either version 3 of the License, or (at
dnl your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful, but
dnl WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
dnl General Public License for more details.
dnl
dnl You should have received a copy of the GNU Lesser General Public License
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>.


dnl Automake macro to build with the mpatrol library


dnl $Id$


# serial 1


# AM_WITH_MPATROL(DEFAULT)
# Check whether to build with the mpatrol library and also attempt to
# determine the support libraries that also need to be linked in when
# libmpatrol is used.  If `DEFAULT' is not specified then it is `no'.
# If `DEFAULT' is `threads' then the threadsafe version of the mpatrol
# library will be used.

AC_DEFUN(AM_WITH_MPATROL, [
  # Firstly, determine if the mpatrol library should be used.

  AC_MSG_CHECKING(if mpatrol should be used)
  AC_ARG_WITH(mpatrol,
   [  --with-mpatrol          build with the mpatrol library],
   [case "$withval" in
     threads)
      am_with_mpatrol=1
      am_with_mpatrol_threads=1;;
     yes)
      am_with_mpatrol=1
      am_with_mpatrol_threads=0;;
     no)
      am_with_mpatrol=0
      am_with_mpatrol_threads=0;;
     *)
      AC_MSG_RESULT(no)
      AC_MSG_ERROR(invalid value $withval for --with-mpatrol);;
    esac
   ],
   [if test "x[$1]" = x
    then
     am_with_mpatrol=0
     am_with_mpatrol_threads=0
    elif test "[$1]" = no
    then
     am_with_mpatrol=0
     am_with_mpatrol_threads=0
    elif test "[$1]" = yes
    then
     am_with_mpatrol=1
     am_with_mpatrol_threads=0
    elif test "[$1]" = threads
    then
     am_with_mpatrol=1
     am_with_mpatrol_threads=1
    else
     AC_MSG_RESULT(no)
     AC_MSG_ERROR(invalid argument [$1])
    fi
   ]
  )

  if test "$am_with_mpatrol" = 1
  then
   AC_MSG_RESULT(yes)

   # Next, determine which support libraries are available on this
   # system.  If we don't do this here then we can't link later with
   # the mpatrol library to perform any further tests.

   am_with_mpatrol_libs=""
   AC_CHECK_LIB(ld, ldopen,
                am_with_mpatrol_libs="$am_with_mpatrol_libs -lld")
   AC_CHECK_LIB(elf, elf_begin,
                am_with_mpatrol_libs="$am_with_mpatrol_libs -lelf")
   AC_CHECK_LIB(bfd, bfd_init,
                am_with_mpatrol_libs="$am_with_mpatrol_libs -lbfd -liberty -lintl", ,
                -liberty -lintl)
   AC_CHECK_LIB(imagehlp, SymInitialize,
                am_with_mpatrol_libs="$am_with_mpatrol_libs -limagehlp")
   AC_CHECK_LIB(unwind, unw_init_local,
                am_with_mpatrol_libs="$am_with_mpatrol_libs -lunwind")
   AC_CHECK_LIB(cl, U_get_previous_frame,
                am_with_mpatrol_libs="$am_with_mpatrol_libs -lcl")
   AC_CHECK_LIB(exc, unwind,
                am_with_mpatrol_libs="$am_with_mpatrol_libs -lexc")

   # Now determine which libraries really need to be linked in with
   # the version of libmpatrol that is on this system.  For example,
   # if the system has libelf and libbfd, we need to determine which
   # of these, if any, libmpatrol was built with support for.

   am_with_mpatrol_libs2=""
   AC_CHECK_LIB(mpatrol, __mp_libld,
                am_with_mpatrol_libs2="$am_with_mpatrol_libs2 -lld", ,
                $am_with_mpatrol_libs)
   AC_CHECK_LIB(mpatrol, __mp_libelf,
                am_with_mpatrol_libs2="$am_with_mpatrol_libs2 -lelf", ,
                $am_with_mpatrol_libs)
   AC_CHECK_LIB(mpatrol, __mp_libbfd,
                am_with_mpatrol_libs2="$am_with_mpatrol_libs2 -lbfd -liberty -lintl", ,
                $am_with_mpatrol_libs)
   AC_CHECK_LIB(mpatrol, __mp_libimagehlp,
                am_with_mpatrol_libs2="$am_with_mpatrol_libs2 -limagehlp", ,
                $am_with_mpatrol_libs)
   AC_CHECK_LIB(mpatrol, __mp_libunwind,
                am_with_mpatrol_libs2="$am_with_mpatrol_libs2 -lunwind", ,
                $am_with_mpatrol_libs)
   AC_CHECK_LIB(mpatrol, __mp_libcl,
                am_with_mpatrol_libs2="$am_with_mpatrol_libs2 -lcl", ,
                $am_with_mpatrol_libs)
   AC_CHECK_LIB(mpatrol, __mp_libexc,
                am_with_mpatrol_libs2="$am_with_mpatrol_libs2 -lexc", ,
                $am_with_mpatrol_libs)

   # If we are using the threadsafe mpatrol library then we may also need
   # to link in the threads library.  We check blindly for pthreads here
   # even if we don't need them (in which case it doesn't matter) since
   # the threads libraries are linked in by default on AmigaOS, Windows
   # and Netware and it is only UNIX systems that we need to worry about.

   if test "$am_with_mpatrol_threads" = 1
   then
    AC_CHECK_LIB(pthread, pthread_mutex_init,
                 am_with_mpatrol_libs2="$am_with_mpatrol_libs2 -lpthread", [
      AC_CHECK_LIB(pthreads, pthread_mutex_init,
                   am_with_mpatrol_libs2="$am_with_mpatrol_libs2 -lpthreads", [
        AC_CHECK_LIB(thread, pthread_mutex_init,
                     am_with_mpatrol_libs2="$am_with_mpatrol_libs2 -lthread")
       ]
      )
     ]
    )
   fi

   # We now know what libraries to use in order to link with libmpatrol.

   AC_DEFINE(HAVE_MPATROL, 1, [Define if using mpatrol])
   if test "$am_with_mpatrol_threads" = 1
   then
    LIBS="-lmpatrolmt $am_with_mpatrol_libs2 $LIBS"
   else
    LIBS="-lmpatrol $am_with_mpatrol_libs2 $LIBS"
   fi

   # Finally, verify that mpatrol is correctly installed and that we can
   # link a simple program with it.

   AC_CACHE_CHECK(for working mpatrol, am_cv_with_mpatrol, [
     AC_TRY_LINK([#include <mpatrol.h>],
      [void *p = malloc(4);],
      [am_cv_with_mpatrol=yes],
      [am_cv_with_mpatrol=no]
     )
    ]
   )

   if test "$am_cv_with_mpatrol" = no
   then
    AC_MSG_ERROR(mpatrol not installed correctly)
   fi
  else
   AC_MSG_RESULT(no)
  fi
 ]
)
