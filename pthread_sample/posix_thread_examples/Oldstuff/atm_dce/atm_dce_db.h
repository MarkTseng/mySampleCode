/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * atm_dce_db.h
 *
 * Include file for database module of example atm program.
 */

#define MAX_NUM_ACCOUNTS  240 
#define CPU_LOOP_SIZE     325000


typedef struct {
  int id;
  int password;
  int balance;
  int inuse;
} account_t;

void init_account_db(int, int, int);
int new_account(int *, account_t **);
int retrieve_account(int, account_t **);
int store_account(account_t *);
int return_account(account_t *);
int delete_account(account_t *);












