/* **************** LPD:1.0 s_14/pipe2.c **************** */
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
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#define SIZE 1024
int main ()
{
    int rc, status, filedes[2];
    pid_t pid;
    char buffer[SIZE];
    if (pipe (filedes) != 0)
        exit (EXIT_FAILURE);

    memset (buffer, 0, SIZE);
    pid = fork ();

    if (pid > 0) {              /* parent */
        memset (buffer, 'X', SIZE);
        close (filedes[0]);
        rc = write (filedes[1], buffer, SIZE);
        printf ("\nParent wrote %d bytes\n", rc);
        (void)wait (&status);
        printf ("\nParent now exiting\n");
        exit (EXIT_SUCCESS);
    }
    if (pid == 0) {             /* child */
        close (filedes[1]);
        rc = read (filedes[0], buffer, SIZE);
        printf ("\n      Child read %d bytes  \n", rc);
        write (STDOUT_FILENO, buffer, SIZE);
        exit (EXIT_SUCCESS);
    }
    printf ("fork failed");
    exit (EXIT_FAILURE);
}
