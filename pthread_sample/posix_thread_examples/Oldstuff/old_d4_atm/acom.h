/* acom.h
 *
 * Created by Brad Nichols, 9/1/93
 *
 * The include file for the communications package used by the 
 * Automated Teller Machine (ATM) example program.
 *
 */


#define COMM_BUF_SIZE              100
#define MAX_NUM_CONNECTIONS        100
#define SOCKET_NAME                 "./.atm_conn"

/* server side */
void server_comm_get_request(int *, char *);
void server_comm_send_response(int, char *);
void server_comm_close_conn(int);
void server_comm_shutdown(void);

/* client side */
void client_comm_send_request(char *);
void client_comm_get_response(char *);
void client_comm_shutdown(void);









