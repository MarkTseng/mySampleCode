/* **************** LPD:1.0 s_28/selectit.c **************** */
/*
 * The code herein is: Copyright Jerry Cooperstein, 2009
 *
 * This Copyright is retained for the purpose of protecting free
 * redistribution of source.
 *
 *     URL:    http://www.coopj.com
 *     email:  coop@coopj.com
 *
 * The primary maintainer for this code is Jerry Cooperstein
 * The CONTRIBUTORS file (distributed with this
 * file) lists those known to have contributed to the source.
 *
 * This code is distributed under Version 2 of the GNU General Public
 * License, which you should have received with the source.
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>

int main (void)
{
    fd_set fd_rset;
    struct timeval timeout = { 8, 5000 };
    int rc, fd;

    fd = open ("./afifo", O_RDONLY | O_NONBLOCK);

    FD_ZERO (&fd_rset);
    FD_SET (fd, &fd_rset);

    if ((rc = select (fd + 1, &fd_rset, NULL, NULL, &timeout)) < 0) {
        printf ("error in select()\n");
        exit (EXIT_FAILURE);
    }
    printf (" select returned with rc=%d\n", rc);

    if (rc > 0 && FD_ISSET (fd, &fd_rset))
        printf ("Data ready for reading on fd=%d\n", fd);
    if (rc == 0)
        printf ("No data found\n");
    exit (EXIT_SUCCESS);
}
