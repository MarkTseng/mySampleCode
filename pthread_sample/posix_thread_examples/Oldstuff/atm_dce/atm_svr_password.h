/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * atm_svr_password.h
 * 
 * Include file for server password module for atm example program.
 *
 */

void server_password_init(int);
int  check_server_password(int);
int  set_server_password(int, int);

