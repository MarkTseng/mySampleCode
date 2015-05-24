/* **************** LPD:1.0 s_28/pollit.c **************** */
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
#include <sys/poll.h>

int main (void)
{
    struct pollfd ufds[1];
    int timeout = 8050, rc, fd;

    /* Watch stdin for input, stdout for output. */

    fd = open ("./afifo", O_RDONLY | O_NONBLOCK);

    ufds[0].fd = fd;
    ufds[0].events = POLLIN;

    if ((rc = poll (ufds, 1, timeout)) < 0) {
        printf ("error in poll()\n");
        exit (EXIT_FAILURE);
    }

    printf (" poll returned with rc=%d\n", rc);

    if (rc > 0 && (ufds[0].revents & POLLIN))
        printf ("Data ready for reading on fd=%d\n", fd);
    if (rc == 0)
        printf ("No data found\n");

    exit (EXIT_SUCCESS);

}
