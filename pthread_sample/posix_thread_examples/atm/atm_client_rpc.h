/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * atm_client_rpc.h
 * 
 * Include file for rpc style client call interface to 
 * atm server. 
 */

int accnt_open_rpc(int *, int *);
int accnt_deposit_rpc(int, int, int);
int accnt_withdraw_rpc(int, int, int);
int accnt_balance_rpc(int, int, int *);
int server_shutdown_rpc(int);

