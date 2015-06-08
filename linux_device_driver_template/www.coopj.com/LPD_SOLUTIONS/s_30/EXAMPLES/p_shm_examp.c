/* **************** LPD:1.0 s_30/p_shm_examp.c **************** */
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
/* compile with -lrt */

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>

int main (int argc, char *argv[])
{
    int shm_fd;
    void *shm_area;
    size_t size = 8192;

    shm_fd = shm_open ("/my_shm", O_CREAT | O_RDWR | O_EXCL, 0666);

    ftruncate (shm_fd, size);

    shm_area = mmap (NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    /* Do something with it now */

    munmap (shm_area, size);

    /* shm_unlink ("/my_shm"); */

    exit (0);
}
