/* **************** LPD:1.0 s_30/v_shm_examp.c **************** */
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
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

int main (int argc, char *argv[])
{
    int shmid;
    void *shared_mem;

    shmid = shmget ((key_t) 89, getpagesize (), 0666 | IPC_CREAT);
    shared_mem = shmat (shmid, (void *)0, 0);
    strcpy (shared_mem, "a string");

    /* do something */ sleep (5);

    shmdt (shared_mem);
    shmctl (shmid, IPC_RMID, 0);
    exit (0);
}
