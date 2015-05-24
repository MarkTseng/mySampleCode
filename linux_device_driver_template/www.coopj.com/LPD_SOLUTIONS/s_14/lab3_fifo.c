/* **************** LPD:1.0 s_14/lab3_fifo.c **************** */
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
/*
 * FIFO's
 *
 * Have your program Create a node using mkfifo or mknod function
 * call.  Fork and then have the child stuff a message in the FIFO and
 * have the parent read it and print it out.
 @*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define DEATH(mess) { perror(mess); exit(errno); }
#define FIFO_NAME "/tmp/myfifo"

int main (int argc, char *argv[])
{
    char message[] = "Hello World on a FIFO";
    int fd;
    pid_t pid;

    if (mkfifo (FIFO_NAME, 0666) < 0)
        DEATH (FIFO_NAME);

    printf ("FIFO created: %s\n", FIFO_NAME);

    fflush (stdout);
    pid = fork ();

    if (pid == 0) {
        /* child */
        fd = open (FIFO_NAME, O_WRONLY);
        printf ("The child is sending: %s ***\n", message);

        if (write (fd, message, sizeof (message)) < 0)
            DEATH ("write error from child to fifo");

        close (fd);
        exit (EXIT_SUCCESS);
    }

    if (pid > 0) {
        /* parent */
        fd = open (FIFO_NAME, O_RDONLY);
        if (read (fd, message, sizeof (message)) < 0)
            DEATH ("read error in parent from fifo");

        printf ("The parent received: %s ***\n", message);
        close (fd);

        if (unlink (FIFO_NAME))
            DEATH ("Had trouble removing fifo");

        exit (EXIT_SUCCESS);
    }

    DEATH ("fork");
}
