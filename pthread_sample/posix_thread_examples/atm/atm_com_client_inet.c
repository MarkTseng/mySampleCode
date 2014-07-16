/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * atm_com_client_inet.c
 * 
 * Version that uses INET sockets instead of UNIX
 * domain sockets.
 * Start server then client with
 * 	atm_client hostname_where_server_is_running
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#include "acom.h"

static struct sockaddr_in sock_name;
static int sock_name_len;

static int    client_sock;

static int    cln_comm_inited=FALSE;

/************************************************************************** 
 * Initialize client communications with the server over a socket.
 **************************************************************************/
void client_comm_init(hostname)
char *hostname;
{
  struct hostent *host;
  struct servent *serv;

  /* Create the socket */
  if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("creating socket");
    exit(1);
  }

  /* Make connection to the named server socket */
  sock_name.sin_family = AF_INET;
  sock_name.sin_port = ATM_SERVER_PORT;

  if((host=gethostbyname(hostname))==NULL)
    {
      fprintf(stderr,"%s: no such host\n",hostname);
      exit(1);
    }
  bcopy((char*)host->h_addr, (char*)&sock_name.sin_addr, host->h_length);
  sock_name.sin_family = host->h_addrtype;

  if (connect(client_sock, (struct sockaddr *)&sock_name, sizeof(struct sockaddr_in)) < 0) {
    /* allow one retry */
    if (errno ==  ECONNREFUSED) {
      sleep(1);
      if (connect(client_sock, (struct sockaddr *)&sock_name, sizeof(struct sockaddr_in)) < 0) {
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

  if ((close(client_sock)) < 0) {
    perror ("socket close");
    exit (1);
  }
}
