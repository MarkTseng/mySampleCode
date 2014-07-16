/* adb.c
 *
 * Created by Brad Nichols, 10/10/93
 *
 * Database Module for the ATM example program.
 *
 * in-memory version
 *
 *
 */

/*
 * Accounts are long lived objects that can be accessed in the following
 * ways;
 *
 *   new_account()      Provides a pointer to a account_t representing 
 *                      a new account with a unique id.
 *   retrieve_account() Provides a pointer to a account_t representing
 *                      an active account.
 *   store_account()    Takes an account_t pointer and writes it out
 *                      to perment storage then frees the account pointer.
 *   return_account()   Takes an account pointer that the caller does not
 *                      to use anymore and frees it. (Does not change
 *                      the account image on perment store.)
 *   delete_account()   Takes a pointer to an account and
 *                      resets the account image in perment store, frees 
 *                      the acocunt pointer and reclaims the id for 
 *                      reuse.
 *
 * A new_ or a retrieve_ is expected to be matched by a return_, store_,
 * or delete_
 */

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#include "a.h"
#include "adb.h"

#define ATM_DB_FILE     "./.atm_db"      /* File that contains database. */

#define PAD             8192             

					 /* Multiple of where to put
					    each account in the file.
					    An artificial way to simulate
					    a slower backing store by
					    causing more page faults
					    on access. */

static account_t blank_account = {  0,   /* id */
				    0,   /* password */
				    0,   /* balance */
				    0};  /* inuse */

                                         /* Instance of an empty account
					    used to initially fill 
					    database file and overwrite
					    freed accounts. */

key_t  shared_mem_key = 0x666;
int    shared_mem_id;
static account_t *accounts;
static int increased_cpu = 0;

/************************************************************************** 
 * Initialize the database,
 ***************************************************************************/
void init_account_db(int force_create, int io, int cpu) 
{
  int i;
  extern int increased_cpu;

  if (io != 0) 
    printf("in-memory database, increased io arg ignored\n");
  increased_cpu = cpu;


  /* initialize shared memory segment */
  if ((shared_mem_id = shmget(shared_mem_key, 
			 MAX_NUM_ACCOUNTS * sizeof(account_t), 
			      0600 | IPC_CREAT | IPC_EXCL)) < 0) {
    if (errno == EEXIST)
      shared_mem_id = shmget(shared_mem_key,
			     MAX_NUM_ACCOUNTS * sizeof(account_t), 0);
  }

  accounts = (account_t *)shmat(shared_mem_id, (void *)0, 0);

  /* Fill in with blank accounts */
  for (i = 0; i < MAX_NUM_ACCOUNTS; i++) {
    accounts[i] = blank_account;
  }
  
}
/************************************************************************** 
 * Return a new account for use.
 * id and inuse fields should be set.
 * If no more accounts return ERR_NUM_NO_ACCOUNTS, if an error accessing
 * the database return ERR_NUM_SRV_DATA_UNAVAIL.
 ***************************************************************************/
int new_account(int *idp, account_t **raccountpp) {
  int db_fd, i, rtn;
  off_t pos;
  size_t amt;

  for (i = 0; i < MAX_NUM_ACCOUNTS; i++) {
    if (accounts[i].inuse == 0)
      break;
  }
 
  if (i == MAX_NUM_ACCOUNTS) {
    return (ERR_NUM_NO_ACCOUNTS);
  }

  /* set values in the database and returned structure */
  accounts[i].id = i;
  accounts[i].inuse = 1;

  /* set return values */
  *raccountpp = &(accounts[i]);
  *idp = i;

  return (1);
}

/************************************************************************** 
 * Retrieve an account from backing store.
 *
 * Watch for bad account id or problems accessing db file.
 ***************************************************************************/
int retrieve_account(int id, account_t **raccountpp) {
  int db_fd, rtn;
  off_t pos;
  size_t amt;
  account_t *accountp;

  if ((id < 0) || (id >= MAX_NUM_ACCOUNTS))
    return (ERR_NUM_BAD_ACCOUNT);

  if ((accounts[id].inuse != 1) || (accounts[id].id != id)) {
    return (ERR_NUM_BAD_ACCOUNT);
  }

  /* set return values */
  *raccountpp = &accounts[id];

  return (1);
}

/************************************************************************** 
 * Store an account back to perment storage.
 *
 * Also free the passed account pointer. Watch for a bad account id
 * and problems with db file.
 ***************************************************************************/
int store_account(account_t *accountp) 
{
  int db_fd, rtn, i, j;
  off_t pos;
  size_t amt;
  long count=0;
  extern int increased_io;

  if ((accountp->id < 0) || (accountp->id >= MAX_NUM_ACCOUNTS))
    return (ERR_NUM_BAD_ACCOUNT);

  for (i = 0; i < increased_cpu; i++) 
    for (j = 0; j < CPU_LOOP_SIZE; j++)
      count+=j;
  
  return (1);

}

/************************************************************************** 
 * Clean-up an account pointer no longer wanted.
 *
 * See store_account() to write account data out to perment store.
 ***************************************************************************/
int return_account(account_t *accountp)
{
  return (1);
}
/************************************************************************** 
 * Delete an active account.
 *
 * Watch for bad account id and problems with db file.
 ***************************************************************************/
int delete_account(account_t *accountp)
{
  int db_fd, rtn;
  off_t pos;
  size_t amt;
  
  if ((accountp->id < 0) || (accountp->id >= MAX_NUM_ACCOUNTS))
    return (ERR_NUM_BAD_ACCOUNT);
  
  return (1);
}

