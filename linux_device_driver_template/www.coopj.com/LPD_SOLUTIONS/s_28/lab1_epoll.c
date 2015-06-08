/* **************** LPD:1.0 s_28/lab1_epoll.c **************** */
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
 * I/O Multiplexing and select() (select() solution)
 *
 * Write a program that takes some files names as arguments.  It
 * should open each file, and then scan each one for incoming data.
 *
 * When any file has data ready to read it should print out the
 * filename as well as the data.  Loop indefinitely, printing the data
 * from each file as it comes.
 *
 * Test the program by creating two named pipes with the mkfifo
 * command:
 *
 *   mkfifo file1
 *   mkfifo file2
 *
 *  (we'll discuss this when we discuss pipes) and opening up three
 *  shell windows (or using three virtual terminals if you don't have
 *  X running).
 *
 * In two windows, type cat > file1 and cat > file2; in the third
 * window, run your program. You should be able to type "Hello" in the
 * first window, and see "file1 says: Hello" come from your test
 * program, then type "Goodbye" in the second window and see "file2
 * says: Goodbye", etc.
 *
 * Some hints:
 *
 *    Print out the name of each file as you open it.  Remember that
 *    if you try to open a fifo for reading, and nobody has it open
 *    for writing, you will stop dead right there! This is an easy
 *    mistake to make and printing each filename you open will make it
 *    obvious when this happens.
 *
 *    You don't have to worry about files being closed if you don't
 *    want to. For extra credit, have your program "forget" about
 *    closed files when you hear that they are closed, but this can be
 *    tricky so don't worry about it if you don't want to.
 *
 *    Don't forget that the first argument to select() should be the
 *    highest file descriptor that is set in your file descriptor set
 *    plus one, not the number of files set!  This is very important!
 *
 * Implement the program using select().
 *
 * Implement the program using epoll.
 @*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/epoll.h>

#define MAX_CONNS 10

/* private data structure describing an event */

struct my_data
{
    int fd;
    char *filename;
};

int main (int argc, char *argv[])
{
    int fd, epfd, nfd, nev, rfd, maxbytes, maxevents, rc, i, timeout = -1;
    char *buffer, *filename;
    struct epoll_event ep_event[MAX_CONNS], *ep;
    ssize_t nbytes;
    struct my_data *tptr;

    if (argc < 2 || argc >= MAX_CONNS) {
        fprintf (stderr,
                 "give at least one filename and not more than %d names\n",
                 MAX_CONNS);
        exit (-1);
    }

    nfd = argc - 1;
    maxevents = 4 * nfd;
    maxbytes = getpagesize () - 1;
    buffer = malloc (maxbytes + 1);

    epfd = epoll_create (nfd);
    printf ("epfd from epoll_create = %d\n", epfd);

    /* open all the files and associate data structures with them */

    for (i = 0; i < nfd; i++) {

        filename = argv[i + 1];
        if ((fd = open (filename, O_RDONLY)) < 0) {
            fprintf (stderr, "Opening %s failed: %s\n", filename,
                     strerror (errno));
            exit (1);
        }
        printf ("Opened %s successfully\n", filename);

        tptr = malloc (sizeof (struct my_data));
        tptr->fd = fd;
        tptr->filename = filename;
        ep_event[i].data.ptr = tptr;
        ep_event[i].events = EPOLLIN;

        rc = epoll_ctl (epfd, EPOLL_CTL_ADD, fd, &ep_event[i]);
        printf ("rc from epoll_ctl = %d, for i=%d\n", rc, i);
        printf ("fd = %d for filename = %s\n\n", fd, filename);
    }

    /* go into a loop and wait on events */

    for (;;) {

        nev = epoll_wait (epfd, ep_event, maxevents, timeout);
        printf ("nev from epoll_wait = %d\n", nev);

        /* found at least one, loop on each and read the data */

        for (i = 0; i < nev; i++) {

            ep = &ep_event[i];

            if (ep->events && EPOLLIN) {
                tptr = ep->data.ptr;
                rfd = tptr->fd;
                filename = tptr->filename;

                nbytes = read (rfd, buffer, maxbytes);
                if (nbytes <= 0) {
                    printf (" Reached end of data, terminating\n");
                    exit (-1);
                }

                /* Make sure the buffer ends with a null byte for printf's sake. */
                buffer[nbytes] = '\0';
                printf ("\nGot data:%2d:%20s: %s\n\n", rfd, filename, buffer);
            }
        }
    }
}
