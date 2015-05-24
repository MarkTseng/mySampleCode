/* **************** LPD:1.0 s_13/lab2_zombie.c **************** */
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
 * Zombie Lifecycle
 *
 * Modify the previous forking program so that the child terminates
 * quickly, while the parent hibernates for a period of time with
 * sleep().
 *    
 * Run the program in one terminal window (or in background with &)
 * while monitoring your active processes with ps ux, noting the state
 * of the child.  The child should disappear when the parent exits.
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
    pid_t pid;

    printf ("\nThis is the parent, about to fork. pid=%d\n", getpid ());
    fflush (stdout);
    pid = fork ();

    if (pid > 0) {
        printf ("\nThis is the parent, after the fork. Child pid=%d\n", pid);
        sleep(10);
        printf ("\nThis is the parent waking up and exiting\n");
        exit (EXIT_SUCCESS);
    }
    if (pid == 0) {
        printf ("\nThis is the child, after the fork. pid=%d\n", getpid ());
        exit (EXIT_SUCCESS);
    }
    DEATH ("fork");
}

