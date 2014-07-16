/* acomcln.c
 * 
 * Created by Brad Nichols, 9/1/93
 *
 * The communications package used by Automated Teller Machine (ATM)
 * example program.
 *
 * Client Side
 *
 * A simple implementation using UNIX doamin (local IPC) sockets
 * to allow multiple clients to connect to a server over multiple 
 * channels and transfer character buffers in a request/response 
 * format.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stdio.h>

#include "acom.h"

static struct sockaddr_un sock_name;
static int sock_name_len;

static int    client_sock;

static int    cln_comm_inited=FALSE;

/************************************************************************** 
 * Initialize client communications with the server over a socket.
 **************************************************************************/
void client_comm_init(void)
{

  /* Create the socket */
  if ((client_sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
    perror("creating socket");
    exit(1);
  }

  /* Make connection to the named server socket */
  sock_name.sun_family = AF_UNIX;
  strcpy(sock_name.sun_path, SOCKET_NAME);
  if (connect(client_sock, (struct sockaddr *)&sock_name, sizeof(struct sockaddr_un)) < 0) {
    /* allow one retry */
    if (errno ==  ECONNREFUSED) {
      sleep(1);
      if (connect(client_sock, (struct sockaddr *)&sock_name, sizeof(struct sockaddr_un)) < 0) {
	perror("connecting socket second try");
	exit(1);
      }
    } else {
      perror("connecting socket");
      exit(1);
    }
  }
  
  sock_name_len = sizeof(sock_name);

  cln_comm_inited = TRUE;
}

/************************************************************************** 
 * Send a request buffer to the server.
 **************************************************************************/
void client_comm_send_request(char *req_buf)
{
  int bw = 0;

  if (!cln_comm_inited)
    client_comm_init();

  if ((bw = write(client_sock, req_buf, COMM_BUF_SIZE)) != COMM_BUF_SIZE) {
    perror("write incomplete on send request"); 
    exit (1);
  }
}

/************************************************************************** 
 * Get a response buffer from the connection to the server.
 **************************************************************************/
void client_comm_get_response(char *resp_buf)
{
  int br = 0;

  if (!cln_comm_inited)
    client_comm_init();

  if ((br = read(client_sock, resp_buf, COMM_BUF_SIZE)) != COMM_BUF_SIZE) {
    perror("read incomplete on get response\n");
    exit (1);
  }
}
/************************************************************************** 
 * Shutdown client communications
 **************************************************************************/
void client_comm_shutdown(void)
{
  if (!cln_comm_inited)
    client_comm_init();

  if (close(client_sock)) {
    perror ("socket close");
    exit (1);
  }
}
