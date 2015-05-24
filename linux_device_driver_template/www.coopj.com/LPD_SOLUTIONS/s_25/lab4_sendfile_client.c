/* **************** LPD:1.0 s_25/lab4_sendfile_client.c **************** */
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
 * Using sendfile() (CLIENT)
 *
 * Generate an input file by whatever method you choose.  We will then
 * send this data across the network using and not using sendfile and
 * compare results.
 *
 * You are provided with a server program to receive the data as
 * supplied by clients with and without sendfile().
 *
 * The client programs take as arguments the input file name, the
 * server machine name argument, the number of chunks the I/O should
 * be done in, and the number of times the whole operation should
 * be repeated in order to produce better averaged timing information.
 *
 * Time the results with sendfile() on and off and compare.
 *
 * You'll probably want to do this several times, making sure your
 * system is lightly loaded. The time command will report on real (or
 * wall clock), user, and system time. The system time may not be
 * reliable, so to be rough, just concentrate on the wall clock.
 *
 * If you want to get fancier you can put timing instrumentation in
 * the code using gettimeofday() or a similar function.
 *
 * Note: It is quite possible your timings will be limited by disk I/O
 * and not the network transmission. In order to make sure this is not
 * the case, work with a ram disk. You can do this by:
 *
 *      mkdir ramdisk
 *      mount -t tmpfs none ./ramdisk
 * and then make sure your data file is on the ramdisk.  (Even easier,
 * you can just place your files in /dev/shm which sould
 * already be mounted.)
 @*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <sys/sendfile.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <sys/time.h>

#define PORT_NUMBER 7177

#define GET_ELAPSED_TIME(tv1,tv2) ( \
  (double)( (tv2.tv_sec - tv1.tv_sec) \
            + .000001 * (tv2.tv_usec - tv1.tv_usec)))

int main (int argc, char *argv[])
{
    char *buffer;
    char hname[128] = "localhost", infile[128] = "./infile";
    int rc, fd1, sd, j, k, size, nbuf, nchunk = 1, nrep = 10, which = 0;
    struct sockaddr_in addr;
    struct hostent *hostent;
    struct timeval tv1, tv2;
    double io_time = 0;

    /* 1st arg = on or off for sendfile or read/write
       2nd arg = filename (./infile by default)
       3rd arg = hostname ("localhost" by default)
       4th arg = number of chunks (1 by default)
       5th arg = number of reps (10 by default) */

    if (argc < 2) {
        fprintf (stderr, "USAGE: %s on | off "
                 "[infile] [hostname] [nchunks] [nreps]", argv[0]);
        exit (-1);
    }
    if (!strncmp (argv[1], "on", 2))
        which = 1;

    if (argc > 2)
        strcpy (infile, argv[2]);
    if (argc > 3)
        strcpy (hname, argv[3]);
    if (argc > 4)
        nchunk = atoi (argv[4]);
    if (argc > 5)
        nrep = atoi (argv[5]);

    /* get the file size and then determine the size of the buffers, chunks */

    fd1 = open (infile, O_RDONLY);
    size = (int)lseek (fd1, 0, SEEK_END);
    close (fd1);
    nbuf = (size - 1) / nchunk + 1;

    /* malloc the input buffer */
    buffer = malloc (nbuf);

    printf ("Input file: %s,  Size: %d, Chunks: %d, Bytes: %d \n", infile,
            size, nchunk, nbuf);

    hostent = gethostbyname (hname);
    addr.sin_family = AF_INET;
    addr.sin_port = htons (PORT_NUMBER);
    memcpy (&addr.sin_addr, hostent->h_addr, hostent->h_length);

    printf ("Doing iteration:");
    for (k = 0; k < nrep; k++) {
        printf ("%4d", k + 1);
        fflush (stdout);
        fd1 = open (infile, O_RDONLY);

        sd = socket (PF_INET, SOCK_STREAM, 0);

        connect (sd, (struct sockaddr *)&addr, sizeof (addr));

        gettimeofday (&tv1, NULL);
        for (j = 0; j < nchunk; j++) {
            if (which == 0) {
                rc = read (fd1, buffer, nbuf);
                write (sd, buffer, rc);
            } else {
                off_t offset = j * nbuf;
                rc = sendfile (sd, fd1, &offset, nbuf);
            }
        }
        gettimeofday (&tv2, NULL);
        io_time += GET_ELAPSED_TIME (tv1, tv2);
        close (fd1);
        close (sd);
    }
    if (which == 0)
        fprintf (stderr, "\nDONE, Did Not Use sendfile()");
    else
        fprintf (stderr, "\nDONE, Did     Use sendfile()");
    printf ("\n Total time spent in I/O = %f seconds\n", io_time);
    exit (EXIT_SUCCESS);
}
