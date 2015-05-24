/* **************** LPD:1.0 s_32/lab1_v_mq.c **************** */
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
 * Message Queues (System V SOLUTION)
 *
 * Construct one or more programs that communicate with each other by
 * using message queues. Run them in separate windows.
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

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define BUFSIZE 4096
size_t size = BUFSIZE;

#define DEATH(mess) { perror(mess); exit(errno); }

struct my_msg_st
{
    long my_msg_type;
    char some_text[BUFSIZE];
};

void create_it (void)
{
    int msgid;

    if ((msgid = msgget ((key_t) 1234, 0666 | IPC_CREAT | IPC_EXCL)) == -1)
        DEATH ("msgget");

    printf ("Message Queue successfully created\n");

    exit (EXIT_SUCCESS);
}

void remove_it (void)
{
    int msgid;

    if ((msgid = msgget ((key_t) 1234, 0666)) == -1)
        DEATH ("msgget");

    if (msgctl (msgid, IPC_RMID, 0) == -1)
        DEATH ("msgctl");

    printf ("Message Queue successfully destroyed\n");

    exit (EXIT_SUCCESS);
}

void send_it (void)
{
    int running = 1;
    struct my_msg_st some_data;
    int msgid;
    char buffer[BUFSIZE];

    if ((msgid = msgget ((key_t) 1234, 0666)) == -1)
        DEATH ("msgget");

    while (running) {
        printf ("Enter some text: ");
        fgets (buffer, BUFSIZE, stdin);
        some_data.my_msg_type = 1;
        strcpy (some_data.some_text, buffer);

        if (msgsnd (msgid, (void *)&some_data, BUFSIZE, 0) == -1)
            DEATH ("msgsnd");

        if (strncasecmp (buffer, "end", 3) == 0) {
            running = 0;
        }
    }

    printf ("SEND has successfully completed\n");

    exit (EXIT_SUCCESS);
}

void receive_it (void)
{
    int running = 1;
    int msgid;
    struct my_msg_st some_data;
    long msg_to_receive = 0;

    if ((msgid = msgget ((key_t) 1234, 0666)) == -1)
        DEATH ("msgget");

    while (running) {
        if (msgrcv (msgid, (void *)&some_data, BUFSIZE,
                    msg_to_receive, 0) == -1)
            DEATH ("msgrcv");

        printf ("You wrote: %s", some_data.some_text);
        if (strncasecmp (some_data.some_text, "end", 3) == 0) {
            running = 0;
        }
    }

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
