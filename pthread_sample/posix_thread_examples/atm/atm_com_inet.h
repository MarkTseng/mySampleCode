/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * atm_com_inet.h
 *
 * Version that uses INET sockets instead of UNIX domain
 * sockets.
 *
 */


#define COMM_BUF_SIZE              100
#define MAX_NUM_CONNECTIONS        100
#define ATM_SERVER_PORT 	  5621

/* server side */
void server_comm_get_request(int *, char *);
void server_comm_send_response(int, char *);
void server_comm_close_conn(int);
void server_comm_shutdown(void);

/* client side */
void client_comm_send_request(char *);
void client_comm_get_response(char *);
void client_comm_shutdown(void);









