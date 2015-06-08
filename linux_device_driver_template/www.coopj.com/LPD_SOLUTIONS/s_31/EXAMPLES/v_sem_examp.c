/* **************** LPD:1.0 s_31/v_sem_examp.c **************** */
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
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>

/* data types */
union semun
{
    int val;                    /* value for SETVAL */
    struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
    unsigned short int *array;  /* array for GETALL, SETALL */
    struct seminfo *__buf;      /* buffer for IPC_INFO */
};

int main (int argc, char *argv[])
{
    int semid;
    union semun sem_union;
    struct sembuf buffer;

    semid = semget ((key_t) 261, 1, 0666 | IPC_CREAT);

    /* initialize the semaphore */

    sem_union.val = 1;
    semctl (semid, 0, SETVAL, sem_union);

    buffer.sem_num = 0;
    buffer.sem_flg = SEM_UNDO;

    printf ("Entering critical section\n");
    buffer.sem_op = -1;
    semop (semid, &buffer, 1);
    /* do something */ sleep (5);

    printf (" Leaving critical section\n");
    buffer.sem_op = 1;
    semop (semid, &buffer, 1);

    /* only the last process to use the semaphore should do this */

    semctl (semid, 0, IPC_RMID, sem_union);

    exit (0);
}
