/* **************** LPD:1.0 s_32/p_mq_examp.c **************** */
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
/* compile with -lrt */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <mqueue.h>

int main (int argc, char *argv[])
{
    struct mq_attr attr = {
        .mq_maxmsg = 10,
        .mq_msgsize = 4096,
    };

    mqd_t msg_fd;
    char buffer[4096];

    msg_fd = mq_open ("/my_mq", O_RDWR | O_CREAT | O_EXCL, 0666, &attr);

    while (1) {
        if ((mq_receive (msg_fd, buffer, attr.mq_msgsize, NULL)) == -1)
            exit (-1);
        printf ("Message Received %s", buffer);
    }
    mq_close (msg_fd);
    exit (0);
}
