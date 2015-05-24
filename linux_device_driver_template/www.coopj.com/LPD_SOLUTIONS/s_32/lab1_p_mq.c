/* **************** LPD:1.0 s_32/lab1_p_mq.c **************** */
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
 * Message Queues (POSIX SOLUTION)
 *
 * Construct one or more programs that communicate with each other by
 * using message queues.  Run them in separate windows.
 *
 * The sending program should take lines of input until being given
 * the string "end".
 *
 * The receiving program should print out the messages sent.
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
 *
 @*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <mqueue.h>

#define BUFSIZE 4096
#define NAME "/my_mq"

#define DEATH(mess) { perror(mess); exit(errno); }

size_t size = BUFSIZE;
struct mq_attr attr = {
    .mq_maxmsg = 10,
    .mq_msgsize = BUFSIZE,
};

void create_it (void)
{
    mqd_t msg_fd;

    if ((msg_fd = mq_open (NAME, O_RDWR | O_CREAT | O_EXCL, 0666, &attr)) == -1)
        DEATH ("mq_open");

    mq_getattr (msg_fd, &attr);
    printf ("size = %d\n", (int)size);

    if (mq_close (msg_fd))
        DEATH ("mq_close");

    printf ("Message Queue successfully created\n");

    exit (EXIT_SUCCESS);
}

void remove_it (void)
{
    mqd_t msg_fd;

    if ((msg_fd = mq_open (NAME, O_RDWR, 0, NULL)) == -1)
        DEATH ("mq_open");

    if (mq_close (msg_fd))
        DEATH ("mq_close");

    if (mq_unlink (NAME))
        DEATH ("mq_unlink");

    printf ("Message Queue successfully destroyed\n");

    exit (EXIT_SUCCESS);
}

void send_it (void)
{
    mqd_t msg_fd;
    int running = 1;
    char some_text[BUFSIZE];

    if ((msg_fd = mq_open (NAME, O_RDWR, 0, NULL)) == -1)
        DEATH ("mq_open");

    while (running) {
        printf ("Enter some text: ");
        fgets (some_text, BUFSIZE, stdin);

        if (mq_send (msg_fd, some_text, size, 0) == -1)
            DEATH ("mq_send");

        if (strncasecmp (some_text, "end", 3) == 0)
            running = 0;
    }

    if (mq_close (msg_fd))
        DEATH ("mq_close");

    printf ("SEND has successfully completed\n");

    exit (EXIT_SUCCESS);
}

void receive_it (void)
{
    mqd_t msg_fd;
    int running = 1;
    char some_text[BUFSIZE];

    if ((msg_fd = mq_open (NAME, O_RDWR, 0, NULL)) == -1)
        DEATH ("mq_open");

    printf ("size = %d\n", (int)size);

    while (running) {

        if (mq_receive (msg_fd, some_text, size, NULL) == -1)
            DEATH ("mq_receive");

        printf ("You wrote: %s", some_text);

        if (strncasecmp (some_text, "end", 3) == 0)
            running = 0;
    }

    if (mq_close (msg_fd))
        DEATH ("mq_close");

    printf ("RECEIVE has successfully completed\n");

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
