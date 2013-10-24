#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include "unix_socket_utils.h"          /* some utility functions */
#define KBD_SOCKET_NAME "/tmp/kdb_socket"

int main(void) {
    struct sockaddr_un address;
    int sock;
    size_t addrLength;

	signal(SIGINT, SIG_IGN);

    if ((sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)
        die("socket");

    address.sun_family = AF_UNIX;    /* Unix domain socket */
    strcpy(address.sun_path, KBD_SOCKET_NAME);

    /* The total length of the address includes the sun_family
       element */
    addrLength = sizeof(address.sun_family) +
                 strlen(address.sun_path);

    if (connect(sock, (struct sockaddr *) &address, addrLength))
        die("connect");

    cliCopyData(0, sock);
	

    close(sock);
   
    return 0;
}

