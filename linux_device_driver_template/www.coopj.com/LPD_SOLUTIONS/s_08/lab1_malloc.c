/* **************** LPD:1.0 s_08/lab1_malloc.c **************** */
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
 * Exmaining malloc() Behaviour
 *
 * Write a program that uses mallinfo() and/or malloc_stats() to
 * examine the system's allocation behaviour.
 *
 * Try allocating a small, medium, and large amount of memory and see
 * how much is allocated by resizing the data segment as compared to
 * memory mapping.
 *
 * Try using mallopt() to either turn off memory mapping altogether or
 * to change the threshold between the two techniques.
@*/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>

#define PRINTIT(a) fprintf(stderr,"      " #a    "\t= %d", mi->a);

void dump_mi (struct mallinfo *mi)
{
    PRINTIT (arena);
    PRINTIT (ordblks);
    PRINTIT (smblks);
    fprintf (stderr, "\n");
    PRINTIT (hblks);
    PRINTIT (hblkhd);
    PRINTIT (usmblks);
    fprintf (stderr, "\n");
    PRINTIT (fsmblks);
    PRINTIT (uordblks);
    PRINTIT (keepcost);
    fprintf (stderr, "\n");
}

void doit (int bytes)
{
    char *buf;
    struct mallinfo mi;
    fprintf (stderr, "\n Allocating %10d bytes ------- \n", bytes);
    buf = malloc (bytes);
    malloc_stats ();
    mi = mallinfo ();
    dump_mi (&mi);
    free (buf);
}

int main (int argc, char *argv[])
{
    int thresh, rc;
    if (argc > 1) {
        thresh = atoi (argv[1]);
        rc = mallopt (M_MMAP_THRESHOLD, thresh);
        fprintf (stderr, "rc=%d from requesting mmap"
                 " for more than thresh=%d bytes\n", rc, thresh);
    }
    doit (13);
    doit (13 * 1024);
    doit (13 * 1024 * 1024);
    exit (0);
}
