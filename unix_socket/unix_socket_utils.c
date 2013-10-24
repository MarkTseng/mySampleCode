#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "unix_socket_utils.h"

/* issue an error message via perror() and terminate the program */
void die(char * message) {
    perror(message);
    exit(1);
}

/* Copies data from file descriptor 'from' to file descriptor
   'to' until nothing is left to be copied. Exits if an error
   occurs. This assumes both from and to are set for blocking
   reads and writes. */
void SvrCopyData(int from, int to) {
    char buf[1024];
    int amount;
   
    while ((amount = read(from, buf, sizeof(buf))) > 0) {
        if (write(to, buf, amount) != amount) {
            die("write");
            return;
        }
		if(strncmp("exit",buf,4) == 0)
		{
			printf("exit socket\n");
			break;
		}
    }
    if (amount < 0)
        die("read");
}

/* Copies data from file descriptor 'from' to file descriptor
   'to' until nothing is left to be copied. Exits if an error
   occurs. This assumes both from and to are set for blocking
   reads and writes. */
void cliCopyData(int from, int to) {
    char buf[1024];
    int amount;
   
	printf("<<< RTK kbd debug >>>\n");
    while (1) {
		printf("cmd > ");
		fflush(stdout);
		amount = read(from, buf, sizeof(buf));
        if (write(to, buf, amount) != amount) {
            die("write");
            return;
        }
		if(strncmp("exit",buf,4) == 0)
		{
			printf("exit socket\n");
			break;
		}
    }
    if (amount < 0)
        die("read");
}
