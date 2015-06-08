/* **************** LPD:1.0 s_14/lab2_popen.c **************** */
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
 * popen/pclose
 *
 * Modify the previous lab to have the child use popen/pclose to
 * execute the command.
 @*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define DEATH(mess) { perror(mess); exit(errno); }

int main (int argc, char *argv[])
{
    pid_t pid;
    char *dirname = "/usr/bin";
    int rc, filedes[2], BSIZE=getpagesize();
    char *buffer = malloc(BSIZE);

    if (-1 == pipe (filedes))
        DEATH ("pipe");
    if (argc > 1)
        dirname = argv[1];

    printf ("\nThis is the parent, about to fork\n");
    fflush (stdout);
    pid = fork ();

    if (pid > 0) {
        printf ("\nThis is the parent, after the fork\n");
        close (filedes[1]);

        /* Read (blocking) from the pipe until it goes away. */
        printf ("\nThe parent just read the following from the pipe:\n");
        while ((rc = read (filedes[0], buffer, BSIZE)) > 0)
            write (STDOUT_FILENO, buffer, rc);

        exit (EXIT_SUCCESS);
    }

    if (pid == 0) {
        FILE *cmd_fd;
        char cmd[1024] = "ls -l ";

        printf ("\nThis is the child, after the fork\n");
        close (filedes[0]);
        cmd_fd = popen (strcat (cmd, dirname), "r");

        while ((rc = fread (buffer, 1, BSIZE, cmd_fd)) > 0)
            write (filedes[1], buffer, rc);

        pclose (cmd_fd);
        exit (EXIT_SUCCESS);
    }
    DEATH ("fork");
}
