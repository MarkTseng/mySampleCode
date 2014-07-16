/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * atm_com.h
 *
 * The include file for the communications package used by the 
 * Automated Teller Machine (ATM) example program.
 *
 */


#define COMM_BUF_SIZE              100
#define MAX_NUM_CONNECTIONS        100
#define SOCKET_NAME                 "./.atm_conn"

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

/* server side */
void server_comm_init(void);
void server_comm_get_request(int *, char *);
void server_comm_send_response(int, char *);
void server_comm_close_conn(int);
void server_comm_shutdown(void);

/* client side */
void client_comm_init(void);
void client_comm_send_request(char *);
void client_comm_get_response(char *);
void client_comm_shutdown(void);









