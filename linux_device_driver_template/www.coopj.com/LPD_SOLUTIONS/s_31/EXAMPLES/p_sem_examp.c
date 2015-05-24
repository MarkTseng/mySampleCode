/* **************** LPD:1.0 s_31/p_sem_examp.c **************** */
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
/* compile with -lrt  */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

int main (int argc, char *argv[])
{
    sem_t *mysem;
    int i;

    mysem = sem_open ("/my_sem", O_CREAT | O_EXCL, 0666, 1);

    for (i = 0; i < 4; i++) {
        sem_wait (mysem);
        printf ("Entering critical section, %d, pid=%d\n", i, getpid ());
        sleep (3);
        printf (" Leaving critical section, %d, pid=%d\n", i, getpid ());
        sem_post (mysem);
        sleep (3);
    }
    printf ("PROTECT region exited for pid=%d\n", getpid ());

    sem_close (mysem);
    sem_unlink ("/my_sem");

    exit (0);
}
