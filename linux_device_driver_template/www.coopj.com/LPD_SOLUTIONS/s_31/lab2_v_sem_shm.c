/* **************** LPD:1.0 s_31/lab2_v_sem_shm.c **************** */
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
 * Semaphores and Shared Memory (System V Solution)
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
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <string.h>

#define DEATH(mess) { perror(mess); exit(errno); }

#define KEY (key_t)26
#define SIZE 8196

union semun
{
    int val;                    /* value for SETVAL */
    struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
    unsigned short int *array;  /* array for GETALL, SETALL */
    struct seminfo *__buf;      /* buffer for IPC_INFO */
};

void set_sem (int semid, int val)
{
    struct sembuf buffer;

    buffer.sem_num = 0;         /* first (and only) semaphore in the list */
    buffer.sem_op = val;        /* either wait (-1) or release (1) */
    buffer.sem_flg = SEM_UNDO;  /* let the system clean up after us */

    if ((semop (semid, &buffer, 1)) == -1)
        DEATH ("setop");
}

void create_it (void)
{
    int shmid, semid;
    void *shm_area;
    union semun sem_union;

    if ((shmid = shmget (KEY, SIZE, IPC_CREAT | 0666)) == -1)
        DEATH ("shmget");

    if ((shm_area = shmat (shmid, (void *)0, 0)) == (void *)-1)
        DEATH ("shmat");

    printf ("CREATE: Memory attached at %lX\n", (unsigned long)shm_area);

    if (shmdt (shm_area) == -1)
        DEATH ("shmdt");

    if ((semid = semget (KEY, 1, 0666 | IPC_CREAT)) == -1)
        DEATH ("semget");

    sem_union.val = 1;
    if ((semctl (semid, 0, SETVAL, sem_union)) == -1)
        DEATH ("semctl");

    printf ("Semaphore successfully created and initialized\n");

    exit (EXIT_SUCCESS);
}

void remove_it (void)
{
    int shmid, semid;
    union semun sem_union;

    if ((shmid = shmget (KEY, SIZE, 0)) == -1)
        DEATH ("shmget");

    if (shmctl (shmid, IPC_RMID, 0))
        DEATH ("shmctl");

    printf ("Marked shared memory segment for deletion\n");

    if ((semid = semget (KEY, 1, 0)) == -1)
        DEATH ("semget");

    if ((semctl (semid, 0, IPC_RMID, sem_union)) == -1)
        DEATH ("semctl");

    printf ("Semaphore successfully deleted\n");

    exit (EXIT_SUCCESS);
}

void send_it (void)
{
    int shmid, semid, i;
    void *shm_area;

    if ((shmid = shmget (KEY, SIZE, 0)) == -1)
        DEATH ("shmget");

    if ((shm_area = shmat (shmid, (void *)0, 0)) == (void *)-1)
        DEATH ("shmat");

    printf ("SEND: Memory attached at %lX\n", (unsigned long)shm_area);

    if ((semid = semget (KEY, 1, 0)) == -1)
        DEATH ("semget");

    for (i = 0; i < 4; i++) {
        set_sem (semid, -1);    /* wait */
        printf ("Please type a message:\n");
        fgets (shm_area, SIZE, stdin);
        set_sem (semid, 1);     /* release */
        usleep (1000);          /* pause a millisecond to give other guy a chance */
    }

    if (shmdt (shm_area) == -1)
        DEATH ("shmdt");

    printf ("SENDND has successfully completed\n");

    exit (EXIT_SUCCESS);
}

void receive_it (void)
{
    int shmid, semid, i;
    void *shm_area;

    if ((shmid = shmget (KEY, SIZE, 0)) == -1)
        DEATH ("shmget");

    if ((shm_area = shmat (shmid, (void *)0, 0)) == (void *)-1)
        DEATH ("shmat");

    printf ("RCV: Memory attached at %lX\n", (unsigned long)shm_area);

    if ((semid = semget (KEY, 1, 0)) == -1)
        DEATH ("semget");

    for (i = 0; i < 4; i++) {
        set_sem (semid, -1);    /* wait */
        fprintf (stdout, "%s\n", (char *)shm_area);
        set_sem (semid, 1);     /* release */
        usleep (1000);          /* pause a millisecond to give other guy a chance */
    }

    printf ("RCV has successfully completed\n");

    if (shmdt (shm_area) == -1)
        DEATH ("shmdt");

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
