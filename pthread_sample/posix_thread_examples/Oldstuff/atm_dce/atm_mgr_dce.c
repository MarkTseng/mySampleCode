/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ******************************************************** 
 * atm_mgr_dce.c --
 *
 * Transaction routines for ATM DCE example
 */

#include <stdio.h>
#include <dce/utc.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "atm_dce.h"
#include "atm_dce_db.h"
#include "../atm_basic/atm_svr_password.h"
#include "atm_dce_interface.h"

static char atm_err_tbl[NUM_ERR_MSG][ERR_MSG_SIZE];

/* #define NO_DATABASE_SYNC */
/* #define SINGLE_DATABASE_MUTEX_SYNC */
#define ACCOUNT_MUTEX_SYNC

#ifdef NO_DATABASE_SYNC
                                                 /* No synchronization,
                                                    account data may be
                                                    corrupted by race
                                                    conditions */
#elif defined(SINGLE_DATABASE_MUTEX_SYNC)
static pthread_mutex_t global_data_mutex;        /* Single mutex that controls
                                                    access to all database
                                                    accounts. */
#elif defined(ACCOUNT_MUTEX_SYNC)
static pthread_mutex_t account_mutex[MAX_NUM_ACCOUNTS];
                                                 /* Array of mutex, one per
                                                    database account. */
static pthread_mutex_t open_account_mutex;       /* Special case mutex,
                                                    controls access to creating
                                                    new account in database. */
#endif

/**************************************************************************
 * Open an account. The first unused account is selected. A password
 * equal to the account number is assigned by the server.
 **************************************************************************/
void open_account(
	     rpc_binding_handle_t           handle,       /* in */
	     idl_long_int                   *id,          /* out */
	     idl_long_int		    *password,    /* out */
	     idl_long_int                   *success,     /* out */
	     idl_char                       estring[20]) /* out */

{
  int rtn;
  account_t *accountp;
  

  DPRINTF(("SERVER: open \n"));

#ifdef NO_DATABASE_SYNC

#elif defined(SINGLE_DATABASE_MUTEX_SYNC)
  pthread_mutex_lock(&global_data_mutex);

#elif defined(ACCOUNT_MUTEX_SYNC)
  pthread_mutex_lock(&open_account_mutex);

#endif

  /* Get a new account */
  if ((rtn = new_account(id, &accountp)) < 0) {
    DPRINTF(("SERVER: open new_account failed, %s\n", atm_err_tbl[-rtn]));
    *success = TRANS_FAILURE;
    strncpy((char *)estring, atm_err_tbl[-rtn], ERR_MSG_SIZE);

  } else {
    accountp->password = *id;
    accountp->balance  = 0;
    
    /* Store it back to the database */
    if ((rtn = store_account(accountp)) < 0) {
      DPRINTF(("SERVER: open, store_account failed %s\n", atm_err_tbl[-rtn]));
      *success = TRANS_FAILURE;
      strncpy((char *)estring, atm_err_tbl[-rtn], ERR_MSG_SIZE);
    } else {
      *success = TRANS_SUCCESS;
      *password = *id;      
    }
  }

#ifdef NO_DATABASE_SYNC

#elif defined(SINGLE_DATABASE_MUTEX_SYNC)
  pthread_mutex_unlock(&global_data_mutex);

#elif defined(ACCOUNT_MUTEX_SYNC)
  pthread_mutex_unlock(&open_account_mutex);

#endif

}

/**************************************************************************
 * Deposit money in an account. The account id and access password are
 * verified before the transaction is done.
 **************************************************************************/
void deposit( 
	     rpc_binding_handle_t           handle,       /* in */
	     idl_long_int                   *id,          /* in */
	     idl_long_int		    *password,    /* in */
	     idl_long_int 	            *amount,      /* in */
	     idl_long_int                   *success,     /* out */
	     idl_char                       estring[20]) /* out */
{
  int rtn;
  int temp;
  account_t *accountp;

  /* Parse input string */
  DPRINTF(("SERVER: deposit, id %d, password %d, amount %d\n",
         *id, *password, *amount));

  /* Check inputs */
  if ((*id < 0) || (*id >= MAX_NUM_ACCOUNTS)) {
    DPRINTF(("SERVER: deposit, %s\n", ERR_MSG_BAD_ACCOUNT));
    *success = TRANS_FAILURE;
    strncpy((char *)estring,ERR_MSG_BAD_ACCOUNT, ERR_MSG_SIZE);
    return;
  }

#ifdef NO_DATABASE_SYNC

#elif defined(SINGLE_DATABASE_MUTEX_SYNC)
  pthread_mutex_lock(&global_data_mutex);

#elif defined(ACCOUNT_MUTEX_SYNC)
  pthread_mutex_lock(&account_mutex[*id]);

#endif

  /* Retrieve account from database */
  if ((rtn = retrieve_account( *id, &accountp)) < 0) {
    DPRINTF(("SERVER: deposit, retrieve_account failed %s\n", atm_err_tbl[-rtn]\
));
    *success = TRANS_FAILURE;
    strncpy((char *)estring,atm_err_tbl[-rtn], ERR_MSG_SIZE);

  /* Check password */
  } else if (*password != accountp->password)  {
    DPRINTF(("SERVER: deposit, %s\n", ERR_MSG_BAD_PASSWORD));
    *success = TRANS_FAILURE;
/*  ERR_MSG_BAD_PASSWORD; */

  /* Add new balance */
  } else {
    (accountp->balance) += *amount;

    /* Store back to database */
    if ((rtn = store_account(accountp)) < 0) {
      DPRINTF(("SERVER: deposit, store_account failed %s\n", atm_err_tbl[-rtn])\
);
      *success = TRANS_FAILURE;
      strncpy((char *)estring,atm_err_tbl[-rtn], ERR_MSG_SIZE);
      
    /* Everything OK */
    } else {
      *success = TRANS_SUCCESS;
    }
  }

#ifdef NO_DATABASE_SYNC

#elif defined(SINGLE_DATABASE_MUTEX_SYNC)
  pthread_mutex_unlock(&global_data_mutex);

#elif defined(ACCOUNT_MUTEX_SYNC)
  pthread_mutex_unlock(&account_mutex[*id]);

#endif

}

/**************************************************************************
 * Remove money from an account. The account id and access password are
 * verified, and sufficient funds must be available for the transaction
 * to proceed.
 **************************************************************************/
void withdraw(
	      rpc_binding_handle_t	    handle,       /* in */
 	      idl_long_int                  *id,          /* in */
	      idl_long_int		    *password,    /* in */
	      idl_long_int 	            *amount,      /* in */
	      idl_long_int	            *success,     /* out */
	      idl_char                      estring[20]) /* out */
{
  int rtn;
  int temp;
  account_t *accountp;

  /* Parse input string */
  DPRINTF(("SERVER: withdraw,  id %d, password %d, amount %d\n",
          *id, *password, *amount));

  /* Check inputs */
  if ((*id < 0) || (*id >= MAX_NUM_ACCOUNTS)) {
    DPRINTF(("SERVER: withdraw, %s", ERR_MSG_BAD_ACCOUNT));
    *success = TRANS_FAILURE;
    strncpy((char *)estring,ERR_MSG_BAD_ACCOUNT, ERR_MSG_SIZE);
    return;
  }

#ifdef NO_DATABASE_SYNC

#elif defined(SINGLE_DATABASE_MUTEX_SYNC)
  pthread_mutex_lock(&global_data_mutex);

#elif defined(ACCOUNT_MUTEX_SYNC)
  pthread_mutex_lock(&account_mutex[*id]);

#endif

  /* Retrieve account from database */
  if ((rtn = retrieve_account( *id, &accountp)) < 0) {
    DPRINTF(("SERVER: withdraw, retrieve_account failed %s\n",
             atm_err_tbl[-rtn]));
    *success = TRANS_FAILURE;
    strncpy((char *)estring,atm_err_tbl[-rtn], ERR_MSG_SIZE);

  /* Check password */
  } else if (*password != accountp->password)  {
    DPRINTF(("SERVER: withdraw, %s\n", ERR_MSG_BAD_PASSWORD));
    *success = TRANS_FAILURE;
    strncpy((char *)estring,ERR_MSG_BAD_PASSWORD,ERR_MSG_SIZE);

  /* Check for funds */
} else if (*amount > accountp->balance) {
    DPRINTF(("SERVER: withdraw, %s\n", ERR_MSG_INS_FUNDS));
    return_account(accountp);
    *success = TRANS_FAILURE;
    strncpy((char *)estring,ERR_MSG_INS_FUNDS,ERR_MSG_SIZE);

  /* Compute new balance */
  } else {
    accountp->balance -= *amount;
    if ((rtn = store_account(accountp)) < 0) {
      DPRINTF(("SERVER: withdraw, store_account failed %s\n", atm_err_tbl[-rtn]\
));
      *success = TRANS_FAILURE;
      strncpy((char *)estring,atm_err_tbl[-rtn],ERR_MSG_SIZE);

  } else {
      *success = TRANS_SUCCESS;
    }
}

#ifdef NO_DATABASE_SYNC

#elif defined(SINGLE_DATABASE_MUTEX_SYNC)
  pthread_mutex_unlock(&global_data_mutex);

#elif defined(ACCOUNT_MUTEX_SYNC)
  pthread_mutex_unlock(&account_mutex[*id]);

#endif
       
}

/**************************************************************************
 * Provide the current balance in an account. Account id and access password
 * are verified.
 **************************************************************************/
void balance(
	     rpc_binding_handle_t	    handle,       /* in */
 	     idl_long_int                   *id,          /* in */
	     idl_long_int		    *password,    /* in */
	     idl_long_int                   *success,     /* out */
	     idl_long_int	            *cur_bal,     /* out */
	     idl_char                       estring[20]) /* out */
{
  int rtn;
  int temp;
  account_t *accountp;

  /* Parse input string */
  DPRINTF(("SERVER: balance, id %d, password %d\n", *id, *password));

  /* Check inputs */
  if ((*id < 0) || (*id >= MAX_NUM_ACCOUNTS)) {
    DPRINTF(("SERVER: balance, %s", ERR_MSG_BAD_ACCOUNT));
    *success = TRANS_FAILURE;
    strncpy((char *)estring,ERR_MSG_BAD_ACCOUNT,ERR_MSG_SIZE);
    return;
  }

#ifdef NO_DATABASE_SYNC

#elif defined(SINGLE_DATABASE_MUTEX_SYNC)
  pthread_mutex_lock(&global_data_mutex);

#elif defined(ACCOUNT_MUTEX_SYNC)
  pthread_mutex_lock(&account_mutex[*id]);

#endif

  /* Retrieve account from database */
  if ((rtn = retrieve_account( *id, &accountp)) < 0) {
    DPRINTF(("SERVER: balance, retrieve_account failed %s\n",
             atm_err_tbl[-rtn]));
    *success = TRANS_FAILURE;
    strncpy((char *)estring,atm_err_tbl[-rtn],ERR_MSG_SIZE);

  } else {

    /* Check password */
    if (*password != accountp->password)  {
      DPRINTF(("SERVER: balance, %s\n", ERR_MSG_BAD_PASSWORD));
      *success = TRANS_FAILURE;
      strncpy((char *)estring,ERR_MSG_BAD_PASSWORD,ERR_MSG_SIZE);

    } else {

      /* Get balance */
      *cur_bal = accountp->balance;
      *success = TRANS_SUCCESS;
    }

    return_account(accountp);
  }

#ifdef NO_DATABASE_SYNC

#elif defined(SINGLE_DATABASE_MUTEX_SYNC)
  pthread_mutex_unlock(&global_data_mutex);

#elif defined(ACCOUNT_MUTEX_SYNC)
  pthread_mutex_unlock(&account_mutex[*id]);

#endif
}

/************************************************************************** 
 * Provide a means to remotely shutdown server.
 * Any valid account can do it under this simple scheme.
 **************************************************************************/
void shutdown_server(
	     rpc_binding_handle_t	    handle,       /* in */
	     idl_long_int		    *password,    /* in */
	     idl_long_int                   *success,     /* out */
	     idl_char                       estring[20]) /* out */
{
  int temp;
  
  /* Parse input string */
  DPRINTF(("SERVER: shutdown, password %d\n", *password));

  /* Check password */
  if (check_server_password(*password) != 1) {
    DPRINTF(("SERVER: shutdown_req, %s\n", ERR_MSG_BAD_PASSWORD));
    success = TRANS_FAILURE;
    strncpy((char *)estring, ERR_MSG_BAD_PASSWORD, ERR_MSG_SIZE);
  } else {
    /* ok, go ahead */
    *success = TRANS_SUCCESS;
  }
}

/*************************************************************************
 * Initializes server
 *************************************************************************/
void atm_server_init(int argc, char **argv)
{
  int i, rtn;
  int force_create = 0;
  int increased_io = 0;
  int increased_cpu = 0;

  /* process input arguments */
  if (argc >= 4) sscanf(argv[3], "%d", &increased_cpu);
  if (argc >= 3) sscanf(argv[2], "%d", &increased_io);
  if (argc >= 2) {
    if ((rtn = strcmp(argv[1], "-c")) == 0)
      force_create = 1;
    if (((rtn = strcmp(argv[1], "-h")) == 0) ||
        ((rtn = strcmp(argv[1], "h")) == 0)  ||
        ((rtn = strcmp(argv[1], "help")) == 0)) {
      printf("  %s [-c] [increased_io increased_cpu]\n", argv[0]);
      printf("\t-c\t\tcreate a new database\n");
      printf("\tincreased_io\tadd extra io work to storing a account\n");
      printf("\tincreased_cpu\tadd extra cpu work to storing an account\n");
      exit(1);
    }
  }

#ifdef NO_DATABASE_SYNC
  printf("\t%s: USING NO SYNCHRONIZATION TO ACCOUNTS\n", argv[0]);
#endif
#ifdef SINGLE_DATABASE_MUTEX_SYNC
  printf("\t%s: USING ONE LOCK FOR ALL ACCOUNTS\n", argv[0]);
#endif

  /* Initialize error translation table. */
  sprintf(atm_err_tbl[-1*ERR_NUM_NO_ACCOUNTS],  "%s", ERR_MSG_NO_ACCOUNTS);
  sprintf(atm_err_tbl[-1+ERR_NUM_DATA_UNAVAIL], "%s", ERR_MSG_DATA_UNAVAIL);
  sprintf(atm_err_tbl[-1*ERR_NUM_BAD_ACCOUNT],  "%s", ERR_MSG_BAD_ACCOUNT);
  sprintf(atm_err_tbl[-1*ERR_NUM_BAD_PASSWORD], "%s", ERR_MSG_BAD_PASSWORD);
  sprintf(atm_err_tbl[-1*ERR_NUM_INS_FUNDS],    "%s", ERR_MSG_INS_FUNDS);
  sprintf(atm_err_tbl[-1*ERR_NUM_BAD_TRANSID],  "%s", ERR_MSG_BAD_TRANSID);

#ifdef NO_DATABASE_SYNC
  DPRINTF(("atm server using no database synchronization\n"));

#elif defined(SINGLE_DATABASE_MUTEX_SYNC)
  pthread_mutex_init(&global_data_mutex, pthread_mutexattr_default);
  DPRINTF(("atm server using single mutex database synchronization\n"));

#elif defined(ACCOUNT_MUTEX_SYNC)
  pthread_mutex_init(&open_account_mutex, pthread_mutexattr_default);

  for (i = 0; i < MAX_NUM_ACCOUNTS; i++)
    pthread_mutex_init(&account_mutex[i], pthread_mutexattr_default);
  DPRINTF(("atm server using per account mutex database synchronization\n"));
#endif

  init_account_db(force_create, increased_io, increased_cpu);
  server_password_init(force_create);

}









