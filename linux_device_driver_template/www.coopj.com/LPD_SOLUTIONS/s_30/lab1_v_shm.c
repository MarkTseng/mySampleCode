/* **************** LPD:1.0 s_30/lab1_v_shm.c **************** */
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
 * Shared Memory  (SYSTEM V SOLUTION)
 *
 * Construct one or more programs to pass messages to each other
 * in shared memory.
 *
 * The receiving program may terminate when it receives the message
 * from the sending program.
 *
 * Make sure the shared memory is released when no longer needed.
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
#include <errno.h>
#include <string.h>

#define DEATH(mess) { perror(mess); exit(errno); }

#define KEY (key_t)26
#define SIZE 8196

void create_it (void)
{
    int shmid;
    void *shm_area;

    if ((shmid = shmget (KEY, SIZE, IPC_CREAT | 0666)) == -1)
        DEATH ("shmget");

    if ((shm_area = shmat (shmid, (void *)0, 0)) == (void *)-1)
        DEATH ("shmat");

    printf ("CREATE: Memory attached at %lX\n", (unsigned long)shm_area);

    if (shmdt (shm_area) == -1)
        DEATH ("shmdt");

    printf ("Shared Memory Region successfully created\n");

    exit (EXIT_SUCCESS);
}

void remove_it (void)
{
    int shmid;

    if ((shmid = shmget (KEY, SIZE, 0)) == -1)
        DEATH ("shmget");

    if (shmctl (shmid, IPC_RMID, 0))
        DEATH ("shmctl");

    printf ("Marked shared memory segment for deletion\n");

    exit (EXIT_SUCCESS);
}

void send_it (void)
{
    int shmid, iflag = 1;
    void *shm_area;

    if ((shmid = shmget (KEY, SIZE, 0)) == -1)
        DEATH ("shmget");

    if ((shm_area = shmat (shmid, (void *)0, 0)) == (void *)-1)
        DEATH ("shmat");

    printf ("SEND: Memory attached at %lX\n", (unsigned long)shm_area);
    memcpy (shm_area, &iflag, 4);

    if (shmdt (shm_area) == -1)
        DEATH ("shmdt");

    printf ("SENDND has successfully completed\n");

    exit (EXIT_SUCCESS);
}

void receive_it (void)
{
    int shmid, iflag = 8;
    void *shm_area;

    if ((shmid = shmget (KEY, SIZE, 0)) == -1)
        DEATH ("shmget");

    if ((shm_area = shmat (shmid, (void *)0, 0)) == (void *)-1)
        DEATH ("shmat");

    printf ("RCV: Memory attached at %lX\n", (unsigned long)shm_area);
    memcpy (shm_area, &iflag, 4);

    printf ("iflag is now = %d\n", iflag);
    while (iflag == 8) {
        memcpy (&iflag, shm_area, 4);
        sleep (1);
    }
    printf ("RCV has successfully completed\n");
    printf ("iflag is now = %d\n", iflag);

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
