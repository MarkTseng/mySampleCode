/* **************** LPD:1.0 s_14/lab1_pchild_exec.c **************** */
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
 * pipe - parent/child with exec
 *
 * Change the parent/child lab example have the child exec() a command
 * (say an "ls -l /usr/bin") and stuff it into the pipe, and have
 * the parent read the output from the pipe and display it.
 *
 * Do this using only the pipe(), call, and not the popen() call.
 @*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>

#define DEATH(mess) { perror(mess); exit(errno); }

int main (int argc, char *argv[])
{
    pid_t pid;
    int rc, filedes[2], BSIZE=getpagesize();
    char *buffer = malloc (BSIZE);
    char *dirname = "/usr/bin";

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
        printf ("\nThis is the child, after the fork\n");
        dup2 (STDOUT_FILENO, filedes[1]);
        /* could also do:
           close (STDOUT_FILENO);
           dup (filedes[1]);
         */
        close (filedes[0]);
        execlp ("ls", "ls", "-l", dirname, NULL);
        exit (EXIT_FAILURE);
    }
    DEATH ("fork");
}
