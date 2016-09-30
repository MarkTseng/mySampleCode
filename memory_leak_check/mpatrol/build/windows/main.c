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
 * DLL entry point.  Provides a way to initialise and finalise the mpatrol
 * library when it is built as a DLL.
 */


#include "inter.h"
#include <windows.h>
#include <winbase.h>


#if MP_IDENT_SUPPORT
#ident "$Id$"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *main_id = "$Id$";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* DLL entry point to initialise and finalise the library.
 */

BOOL
WINAPI
DllMain(HANDLE h, DWORD r, LPVOID d)
{
    switch (r)
    {
      case DLL_PROCESS_ATTACH:
        __mp_init();
        break;
      case DLL_PROCESS_DETACH:
        __mp_fini();
        break;
      default:
        break;
    }
    return TRUE;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
