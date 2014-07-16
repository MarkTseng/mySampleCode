/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * atm_com_svr_once.c --
 * 
 * A variation using pthread_once() of the communications 
 * package used by Automated Teller Machine (ATM)
 * example program.
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>

#include <stdio.h>
#include <errno.h>
#include <pthread.h>

#include "atm_com.h"

static struct sockaddr_un sock_name;
static int sock_name_len;

static int    srv_socks_active;
static int    srv_sock[MAX_NUM_CONNECTIONS];
static fd_set srv_read_set;

static pthread_once_t server_comm_init_once=PTHREAD_ONCE_INIT;


/************************************************************************** 
 * Initialize communications package, server side
 * The server maintains an array of sockets to use for connections.
 * Socket 0 in the array is named and exported into the file namespace
 * as a connection point for clients. 
 **************************************************************************/
void server_comm_init(void)
{
  int i;

  /* Initialize the connections db */
  srv_socks_active = 0;
  FD_ZERO(&srv_read_set);

  /* Delete an old incarseration of the named socket if it exists. */
  unlink(SOCKET_NAME);


  /* Create the socket to receive initial connection requests on */
  if ((srv_sock[0] = socket (AF_UNIX, SOCK_STREAM, 0)) < 0) {
    perror("creating socket");
    exit(1);
  }
  sock_name_len = sizeof(sock_name);

  /* Bind the socket to the UNIX domain and a name */
  sock_name.sun_family = AF_UNIX;
  strcpy(sock_name.sun_path, SOCKET_NAME);
  if ((bind(srv_sock[0], (struct sockaddr *)&sock_name, sizeof(sock_name))) < 0) {
    perror("binding socket to a name");
    exit(1);
  }
  
  /* Indicate to system to start listening on the socket */
  if ((listen(srv_sock[0], 5)) < 0) {
    perror(" socket listen");
    exit(1);
  }

  /* Add the socket to the server's set of active sockets */
  FD_SET(srv_sock[0], &srv_read_set);
  srv_socks_active++;

}


/************************************************************************** 
 *
 * Local rountine, assigns a socket to a new connection request.
 **************************************************************************/
void handle_new_connection(void)
{
  int i;

  if (srv_socks_active < MAX_NUM_CONNECTIONS) {
    /* Find a free socket entry to use */ 
    for (i = 1; i < MAX_NUM_CONNECTIONS; i++) {
      if (!(FD_ISSET(srv_sock[i], &srv_read_set))) {
	if ((srv_sock[i] = accept (srv_sock[0], 
				   (struct sockaddr *) &sock_name,
				   &sock_name_len)) < 0) {
	  perror("socket accept");
	  exit(1);
	}
	srv_socks_active++;
	FD_SET(srv_sock[i], &srv_read_set);
	break;
      }
    }
  } else {
    int black_widow_sock;
    fprintf(stderr, 
	    "Supported number of simultainious connections exceeded\n");
    fprintf(stderr, "Ignoring client connect request\n");
    /* There has to be a better way to ignore a connection request,..
       when I get my hands on a sockets wiz I'll modify this */
    black_widow_sock  = accept (srv_sock[0], 
				(struct sockaddr *) &sock_name,
				&sock_name_len);
    close(black_widow_sock);
  }
}

/************************************************************************** 
 *
 * Local routine, reads a request off a socket indicated by a select set.
 **************************************************************************/
int handle_new_request(fd_set read_selects, int **connp, char **req_bufp)
{
  int i;
  int br;

  /* Find the descriptor */
  for (i = 1; i < MAX_NUM_CONNECTIONS; i++) {
    if (FD_ISSET(srv_sock[i], &read_selects)) break;
  } 
  
  /* Read from it */
  if ((br = read(srv_sock[i], 
		 *req_bufp, COMM_BUF_SIZE)) != COMM_BUF_SIZE) {
    /* Handle non-data read cases */
    if (br == 0) {
      /* Close socket down */
      FD_CLR(srv_sock[i], &srv_read_set);
      close(srv_sock[i]);
      srv_socks_active--;
    } else if (br < 0) {
      perror("Read() error on get request");
      exit (1);
    } else {
      fprintf(stderr, 
	      "Read, data < request buf size, ignoring data\n");
    }
    return -1;
  } else {
    /* Ok, have a live one, A real data req buf has been obtained */
    **connp =  i;
    return 0;
  }
}

/************************************************************************** 
 * Get an incomming request buffer from a client,
 * Provide a handle on the connection on which it was received. 
 *
 * The server takes this oportunity to process new connection
 * requests on the name sockets first. These requests are accept()ed 
 * and an unused socket is then used for the dialoge with a that particular
 * client.
 **************************************************************************/
void server_comm_get_request(int *conn, char *req_buf)
{
  int i, nr, not_done = 1;
  fd_set read_selects;
  int rtn;

  pthread_once(&server_comm_init_once,server_comm_init);

  /* loop, processing new connection requests until a client buffer
     is read in on an existing connection. */

  while (not_done) {
    
    /* Set up the socket descriptor mask for the select.
       copy srv_read_set, into the local copy */

    FD_ZERO(&read_selects);
    for (i = 0; i < MAX_NUM_CONNECTIONS; i++) {
      if (FD_ISSET(srv_sock[i], &srv_read_set))
	FD_SET(srv_sock[i], &read_selects);
    }
    
    /* Poll active connections using select() */
    if ((nr = select(FD_SETSIZE,
	   &read_selects, 
	   (fd_set *)NULL,
	   (fd_set *)NULL,
	   (struct timeval *)NULL)) <= 0) {
      perror("Select new reads");
      exit(1);
    }

    if (FD_ISSET(srv_sock[0], &read_selects)) {

       /* Handle the case of a new connection request on the named socket */
      handle_new_connection();

    } else {

      /* Read data from client specific descriptor */
      if (handle_new_request(read_selects, &conn, &req_buf) == 0)
	not_done = 0;
    }

  } /* While not_done */

}


/************************************************************************** 
 *  Send out a response buffer on the appropriote client connection.
 **************************************************************************/
void server_comm_send_response(int conn, char *resp_buf)
{
  int bw = 0;
  int rtn;

  pthread_once(&server_comm_init_once,server_comm_init);

  if (!FD_ISSET(srv_sock[conn], &srv_read_set)) {
    /* not in threaded and multi-process versions this may be a
       condition that is not an error. 
       the client may have gone away while the worker was processing
       the request and the boss shutdown the connection in get request,
       actually a race condition on srv_read_set that must be fixed. */
    fprintf(stderr, "bad connection id on send response\n");
    exit (1);
  }
  if ((bw = write(srv_sock[conn], resp_buf, COMM_BUF_SIZE)) != COMM_BUF_SIZE) {
    perror("write incomplete on send response"); 
    exit (1);
  }
}

/************************************************************************** 
 * Close a particular connection from a client.
 **************************************************************************/
void server_comm_close_conn(int conn)
{

  int rtn;

  pthread_once(&server_comm_init_once,server_comm_init);

  if (!FD_ISSET(srv_sock[conn], &srv_read_set)) {
    fprintf(stderr, "bad connection id on server close connection \n");
    exit (1);
  }

  FD_CLR(srv_sock[conn], &srv_read_set);
  srv_socks_active--;
  if ((close(srv_sock[conn])) < 0) {
    perror ("socket close");
    exit (1);
  }
}


/************************************************************************** 
 * Shutdown communications for the server.
 **************************************************************************/
void server_comm_shutdown(void)
{
  int i;
  int rtn;

  pthread_once(&server_comm_init_once,server_comm_init);

  for (i = 0; i < MAX_NUM_CONNECTIONS; i++) {
    if (FD_ISSET(srv_sock[i], &srv_read_set)) {
      if ((close(srv_sock[i])) < 0) {
	perror ("socket close");
	exit (1);
      }
      srv_socks_active--;
    }
  }
  unlink(SOCKET_NAME);
}
