/* **************** LPD:1.0 s_31/lab2_p_sem_shm.c **************** */
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
 * Semaphores and Shared Memory (POSIX SOLUTION)
 *
 * Modify the shared memory lab to use semaphores to have the
 * programs pass messages to each other in shared memory.
 *
 * The first program should get some input from the user, stuff it
 * into shared memory, and release the semaphore to signal the second
 * program.
 *
 * The second program should read the message and display it, then
 * release the semaphore to let the first one know that he's ready for
 * more.
 *
 * You may write more than one program, or write one program that can
 * do multiple actions based on the arguments.
 *
 * Two solutions are given; one for System V IPC and one for POSIX
 * IPC.
 *
 * The solutions are written so that they take an argument, which
 * can be either create, remove, send, or receive, which controls
 * their action.
 *
 * Don't forget to compile with -lrt for the POSIX IPC solution.
 @*/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/types.h>

#define DEATH(mess) { perror(mess); exit(errno); }
#define SIZE 8196
#define NAME_SHM "/my_shm"
#define NAME_SEM "/my_sem"

void create_it (void)
{
    int shm_fd;
    sem_t *mysem;

    if ((shm_fd = shm_open (NAME_SHM, O_RDWR | O_CREAT | O_EXCL, 0666)) == -1)
        DEATH ("shm_open");

    ftruncate (shm_fd, SIZE);

    printf ("Shared Memory Region successfully created\n");

    if ((mysem = sem_open (NAME_SEM, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        DEATH ("sem_open");

    if ((sem_close (mysem)) == -1)
        DEATH ("sem_close");

    printf ("Semaphore successfully created and initialized\n");
    exit (EXIT_SUCCESS);
}

void remove_it (void)
{
    int shm_fd;
    sem_t *mysem;

    if ((shm_fd = shm_open (NAME_SHM, O_RDWR, 0) == -1))
        DEATH ("shm_open");

    if (shm_unlink (NAME_SHM))
        DEATH ("shm_unlink");

    printf ("Shared Memory Region successfully destroyed\n");

    if ((mysem = sem_open (NAME_SEM, 0)) == SEM_FAILED)
        DEATH ("sem_open");

    if ((sem_unlink (NAME_SEM)) == -1)
        DEATH ("sem_unlink");

    printf ("Semaphore successfully deleted\n");

    exit (EXIT_SUCCESS);
}

void send_it (void)
{
    int shm_fd, i;
    void *shm_area;
    sem_t *mysem;

    if ((shm_fd = shm_open (NAME_SHM, O_RDWR, 0)) == -1)
        DEATH ("shm_open");

    shm_area = mmap (NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (shm_area == MAP_FAILED)
        DEATH ("mmap");

    printf ("SEND: Memory attached at %lX\n", (unsigned long)shm_area);

    printf ("Attaching to the semaphore\n");

    if ((mysem = sem_open (NAME_SEM, 0)) == SEM_FAILED)
        DEATH ("sem_open");

    for (i = 0; i < 4; i++) {
        if (sem_wait (mysem))
            DEATH ("sem_wait");
        printf ("Please type a message:\n");
        fgets (shm_area, SIZE, stdin);
        if (sem_post (mysem))
            DEATH ("sem_post");
        usleep (1000);          /* pause a millisecond to give other guy a chance */
    }

    if (munmap (shm_area, SIZE))
        DEATH ("munmap");

    if ((sem_close (mysem)) == -1)
        DEATH ("sem_close");

    printf ("SEND has successfully completed\n");

    exit (EXIT_SUCCESS);
}

void receive_it (void)
{
    int shm_fd, i;
    void *shm_area;
    sem_t *mysem;

    if ((shm_fd = shm_open (NAME_SHM, O_RDWR, 0)) == -1)
        DEATH ("shm_open");

    shm_area = mmap (NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (shm_area == MAP_FAILED)
        DEATH ("mmap");

    printf ("RCV: Memory attached at %lX\n", (unsigned long)shm_area);

    printf ("Attaching to the semaphore\n");

    if ((mysem = sem_open (NAME_SEM, 0)) == SEM_FAILED)
        DEATH ("sem_open");

    for (i = 0; i < 4; i++) {
        if (sem_wait (mysem))
            DEATH ("sem_wait");
        fprintf (stdout, "%s\n", (char *)shm_area);
        if (sem_post (mysem))
            DEATH ("sem_post");
        usleep (1000);          /* pause a millisecond to give other guy a chance */
    }

    printf ("RCV has successfully completed\n");

    if (munmap (shm_area, SIZE))
        DEATH ("munmap");

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
        if (!strcasecmp ("receive", argv[1]))
            receive_it ();
        if (!strcasecmp ("send", argv[1]))
            send_it ();
    }
    printf ("Usage: %s  create | remove | receive | send \n", argv[0]);
    exit (-1);
}
