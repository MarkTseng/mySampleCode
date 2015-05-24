/* **************** LPD:1.0 s_32/v_mq_examp.c **************** */
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define BUFSIZE 132

int main (int argc, char *argv[])
{
    int msgid;
    struct
    {
        long my_msg_type;
        char some_text[BUFSIZE];
    } some_data;
    some_data.my_msg_type = 1;

    msgid = msgget ((key_t) 1234, 0666 | IPC_CREAT);

    strcpy (some_data.some_text, "A MESSAGE");
    printf ("You are sending: %s\n", some_data.some_text);

    msgsnd (msgid, (void *)&some_data, BUFSIZE, 0);

    memset (some_data.some_text, 0, BUFSIZE);
    printf ("Message is cleared, is now: %s\n", some_data.some_text);
    sleep (5);

    msgrcv (msgid, (void *)&some_data, BUFSIZE, 0, 0);
    printf ("You received: %s\n", some_data.some_text);

    msgctl (msgid, IPC_RMID, 0);
    exit (0);
}
