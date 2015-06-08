/* **************** LPD:1.0 s_28/epollit.c **************** */
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
#include <string.h>
#include <sys/epoll.h>

int main (void)
{
    int timeout = 8050, rc, fd, epfd, maxevents = 1;
    struct epoll_event ep_sd, ep_event;

    /* Watch stdin for input, stdout for output. */

    fd = open ("./afifo", O_RDONLY | O_NONBLOCK);

    ep_sd.data.fd = fd;
    ep_sd.events = EPOLLIN;

    epfd = epoll_create (1);
    epoll_ctl (epfd, EPOLL_CTL_ADD, fd, &ep_sd);

    if ((rc = epoll_wait (epfd, &ep_event, maxevents, timeout)) < 0) {
        printf ("error in epoll()\n");
        exit (EXIT_FAILURE);
    }

    printf (" epoll returned with rc=%d\n", rc);

    if (rc > 0 && (ep_event.events & EPOLLIN))
        printf ("Data ready for reading on fd=%d\n", ep_event.data.fd);

    if (rc == 0)
        printf ("No data found\n");

    exit (EXIT_SUCCESS);

}
