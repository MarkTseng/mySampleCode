/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2010.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#include "tlpi_hdr.h"

#ifdef NOSYSCALL
static int myfunc() { return 1; }
#endif

int
main(int argc, char *argv[])
{
    int numCalls, j, s;

    numCalls = (argc > 1) ? getInt(argv[1], GN_GT_0, "num-calls") : 10000000;

#ifdef NOSYSCALL
        printf("Calling normal function\n");
#else
        printf("Calling getppid()\n");
#endif

    for (j = 0; j < numCalls; j++)
#ifdef NOSYSCALL
        s = myfunc();
#else
        s = getppid();
#endif

    exit(EXIT_SUCCESS);
}
