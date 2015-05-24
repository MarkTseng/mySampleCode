/* **************** LPD:1.0 s_13/lab1_fork.c **************** */
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
 * Forking
 *
 * Write a simple forking example.
 *
 * Parent and child must be able to identify themselves as to which is
 * which.
 *
 * The parent should wait for the child to terminate.
 *
 @*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>

#define DEATH(mess) { perror(mess); exit(errno); }

int main (int argc, char *argv[])
{
    int wait_stat;
    pid_t pid, ch_pid;

    printf ("\nThis is the parent, about to fork. pid=%d\n", getpid ());
    fflush (stdout);
    pid = fork ();

    if (pid > 0) {
        printf ("\nThis is the parent, after the fork. Child pid=%d\n", pid);
        ch_pid = wait (&wait_stat);
        if (WIFEXITED (wait_stat))
            printf ("\nChild exited with code %d\n", WEXITSTATUS (wait_stat));
        else
            printf ("\nChild terminated abnormally\n");
        exit (EXIT_SUCCESS);
    }
    if (pid == 0) {
        printf ("\nThis is the child, after the fork. pid=%d\n", getpid ());
        exit (EXIT_SUCCESS);
    }
    DEATH ("fork");
}
