/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2010.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


/* us_xfr_v2.h

   Header file for us_xfr_sv.c and us_xfr_cl.c.
*/
#include "unix_sockets.h"       /* Declares our socket functions */
#include "tlpi_hdr.h"

#define SV_SOCK_PATH "us_xfr_v2"

#define BUF_SIZE 100
