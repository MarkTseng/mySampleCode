/* **************** LPD:1.0 s_31/lab1_p_sem.c **************** */
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
 * Semaphores (POSIX SOLUTION)
 *
 * Write one or more programs that protect a critical section with a
 * semaphore.
 *
 * The critical section can be as simple as a sleep().
 *
 * You may write more than one program, or write one program that can
 * do multiple actions based on the arguments.
 *
 * Two solutions are given; one for System V IPC and one for POSIX
 * IPC.
 *
 * The solutions are written so that they take an argument, which can
 * be either create, remove, or protect, which controls their action.
 *
 * Don't forget to compile with -lrt for the POSIX IPC solution.
 @*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include <string.h>
#include <fcntl.h>

#define DEATH(mess) { perror(mess); exit(errno); }
#define NAME "/my_sem"

void create_it (void)
{
    sem_t *mysem;

    if ((mysem = sem_open (NAME, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        DEATH ("sem_open");

    if ((sem_close (mysem)) == -1)
        DEATH ("sem_close");

    printf ("Semaphore successfully created and initialized\n");
    exit (EXIT_SUCCESS);
}

void remove_it (void)
{
    if ((sem_unlink (NAME)) == -1)
        DEATH ("sem_unlink");

    printf ("Semaphore successfully deleted\n");
    exit (EXIT_SUCCESS);
}

void protect_it (void)
{
    int i;
    sem_t *mysem;

    printf ("Attaching to the semaphore\n");

    if ((mysem = sem_open (NAME, 0)) == SEM_FAILED)
        DEATH ("sem_open");

    for (i = 0; i < 4; i++) {
        if (sem_wait (mysem))
            DEATH ("sem_wait");
        printf ("Entering critical section, %d, pid=%d\n", i, getpid ());
        sleep (3);
        printf (" Leaving critical section, %d, pid=%d\n", i, getpid ());
        if (sem_post (mysem))
            DEATH ("sem_post");
        sleep (3);
    }
    printf ("PROTECT region exited for pid=%d\n", getpid ());

    if ((sem_close (mysem)) == -1)
        DEATH ("sem_close");

    exit (EXIT_SUCCESS);
}

int main (int argc, char *argv[])
{
    if (argc > 1) {
        if (!strcasecmp ("create", argv[1]))
            create_it ();
        if (!strcasecmp ("remove", argv[1]))
            remove_it ();
        if (!strcasecmp ("protect", argv[1]))
            protect_it ();
    }
    printf ("Usage: %s  create | remove | protect\n", argv[0]);
    exit (-1);
}
