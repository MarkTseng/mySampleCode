/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2010.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Lesser General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the files COPYING.lgpl-v3 and COPYING.gpl-v3 for details.           *
\*************************************************************************/

#include "unix_sockets.h"       /* Declares functions defined here */
#include "tlpi_hdr.h"

int
unixBuildAddress(const char *path, struct sockaddr_un *addr)
{
    if (addr == NULL || path == NULL ||
            strlen(path) >= sizeof(addr->sun_path) - 1) {
        errno = EINVAL;
        return -1;
    }

    memset(addr, 0, sizeof(struct sockaddr_un));
    addr->sun_family = AF_UNIX;
    if (strlen(path) < sizeof(addr->sun_path)) {
        strncpy(addr->sun_path, path, sizeof(addr->sun_path) - 1);
        return 0;
    } else {
        errno = ENAMETOOLONG;
        return -1;
    }
}

int
unixConnect(const char *path, int type)
{
    int sd, savedErrno;
    struct sockaddr_un addr;

    if (unixBuildAddress(path, &addr) == -1)
        return -1;

    sd = socket(AF_UNIX, type, 0);
    if (sd == -1)
        return -1;

    if (connect(sd, (struct sockaddr *) &addr,
                sizeof(struct sockaddr_un)) == -1) {
        savedErrno = errno;
        close(sd);                      /* Might change 'errno' */
        errno = savedErrno;
        return -1;
    }

    return sd;
}

/* Create a UNIX domain socket and bind it to 'path'. If 'doListen'
   is true, then call listen() with specified 'backlog'.
   Return the socket descriptor on success, or -1 on error. */

static int              /* Public interfaces: unixBind() and unixListen() */
unixPassiveSocket(const char *path, int type, Boolean doListen, int backlog)
{
    int sd, savedErrno;
    struct sockaddr_un addr;

    if (unixBuildAddress(path, &addr) == -1)
        return -1;

    sd = socket(AF_UNIX, type, 0);
    if (sd == -1)
        return -1;

    if (bind(sd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
        savedErrno = errno;
        close(sd);                      /* Might change 'errno' */
        errno = savedErrno;
        return -1;
    }

    if (doListen) {
        if (listen(sd, backlog) == -1) {
            savedErrno = errno;
            close(sd);                  /* Might change 'errno' */
            errno = savedErrno;
            return -1;
        }
    }

    return sd;
}

int
unixListen(const char *path, int backlog)
{
    return unixPassiveSocket(path, SOCK_STREAM, TRUE, backlog);
}

int
unixBind(const char *path, int type)
{
    return unixPassiveSocket(path, type, FALSE, 0);
}
