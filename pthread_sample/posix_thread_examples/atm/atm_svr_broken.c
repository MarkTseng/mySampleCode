/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * atm_svr_broken.c
 *
 * **This file has been broken for demonstration of debug
 * **facilities. 
 *
 * Multi-Threaded Server program for the Automated Teller 
 * Machine (ATM) example program.
 * Maintains a database of bank accounts accepting requests from
 * client programs to perform transactions on them.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <pthread.h>

#include "atm.h"
#include "atm_com.h"
#include "atm_db.h"
#include "atm_svr_password.h"

static char atm_err_tbl[ERR_MSG_TBL_SIZE][ERR_MSG_SIZE];

#define MAX_NUM_THREADS 10

typedef struct workorder {
  int  conn;                   
  char req_buf[COMM_BUF_SIZE]; 
} workorder_t;

          /* Structure used to pass infomation to a worker thread.
	     Contains what a thread needs to know to execute the 
	     process_request() routine. Note that a struct is used 
	     because the syntax of a pthread_create() only allows a single
	     argument to be passed to the routine being executed 
	     by the starting thread. */

typedef struct {
  int             num_active;
  pthread_cond_t  thread_exit_cv;
  pthread_mutex_t mutex;
} thread_info_t;

thread_info_t pthread_info;

         /* Structure used by main() thread to keep track of the number of
	    active threads. Used to limit how many threads it will create 
	    and under a shutdown condition to wait till all in-progress
	    requests have finished before "turning off the lights". */


/* Database Account Access Synchronization Options, uncomment on of the 
   following; */

/* #define NO_DATABASE_SYNC */
/* #define SINGLE_DATABASE_MUTEX_SYNC */
#define ACCOUNT_MUTEX_SYNC 

#ifdef NO_DATABASE_SYNC
                                                 /* No synchronization, 
						    account data may be 
						    corrupted by race
						    conditions */
#elif defined(SINGLE_DATABASE_MUTEX_SYNC)
pthread_mutex_t global_data_mutex;               /* Single mutex that controls
                                                    access to all database 
						    accounts. */

         /* Mutex used in the Single Database Mutex Option
	    To control (synchronize) concurrent access by multiple
	    threads to the database this options defines a single
	    mutex that any thread must hold before entering the database 
	    module. Advantages: Simple, non-change to underlying package 
	    (esp. nice if all we have is a library that we can't modify).
	    Disadvantages: No concurrency in database. all threads block. */

#elif defined(ACCOUNT_MUTEX_SYNC)
pthread_mutex_t account_mutex[MAX_NUM_ACCOUNTS]; /* Array of mutex, one per
						    database account. */
pthread_mutex_t open_account_mutex;              /* Special case mutex,
						    controls access to creating
						    new account in database. */
#endif

workorder_t *workorderp;      /* a structure containing the
                                 information a worker thread
                                 needs to go off and process
                                 a request. */

/* Forward declarations of routines */
void *process_request(void *);
void atm_server_init(int, char **);
void open_account(char *);
void deposit(char *, char *);
void withdraw(char *, char *);
void balance(char *, char *);
int  shutdown_req(char *, char *);
void handle_bad_trans_id(char *, char *);

pthread_t   *worker_threadp;   /* a pointer to a newly created thread 
				   object. In this example, the main()
				   thread and none of the worker threads
				   need to have atm.handle to anyother thread.
				   So here never store a pre-thread handle and
				   reuse this pointer no matter how many
				   threads are currently active.
				   An example where we might need to maintain
				   a set of handles for all active threads
				   would be if we used the cancellation
				   mechanism or join. */

/************************************************************************** 
 * main()
 * Basic processing loop of the bank database server.
 * The program initializies the database then loops, accepting
 * transaction requests from clients.
 ***************************************************************************/

extern int
main(int argc, char **argv)
{
  int  trans_id;                /* The transaction type of the current 
				   request */
  void *status;


  atm_server_init(argc, argv);

  for(;;) {

    /*** Wait for a request ***/

    DPRINTF(("SERVER: main(), going to get a request\n"));
    workorderp = (workorder_t *)malloc(sizeof(workorder_t));
    server_comm_get_request(&workorderp->conn, workorderp->req_buf);
    DPRINTF(("SERVER: main(), got a request\n"));


    /*** Is it a shutdown request? ***/

    sscanf(workorderp->req_buf, "%d", &trans_id);
    if (trans_id == SHUTDOWN) {

      char resp_buf[COMM_BUF_SIZE];

      DPRINTF(("SERVER: main(), processing a shutdown request\n"));
      pthread_mutex_lock(&pthread_info.mutex);

      /* Wait for in-progress requests threads to finish */
      while (pthread_info.num_active > 0) {
	pthread_cond_wait(&pthread_info.thread_exit_cv, &pthread_info.mutex);
      }
      pthread_mutex_unlock(&pthread_info.mutex);

      /* process it here with main() thread */
      if (shutdown_req(workorderp->req_buf, resp_buf)) {
	server_comm_send_response(workorderp->conn, resp_buf);
	free(workorderp);
	break;
      }
    }

    
    /*** Have we exceeded our limit of active threads ? ***/

    pthread_mutex_lock(&pthread_info.mutex);

    DPRINTF(("SERVER: main(), num_active %d\n", pthread_info.num_active));

    while (pthread_info.num_active == MAX_NUM_THREADS) {
      DPRINTF(("SERVER: main(), num_active %d, waiting for a thread to exit\n",
	     pthread_info.num_active));
      pthread_cond_wait(&pthread_info.thread_exit_cv, &pthread_info.mutex);
    }
    pthread_info.num_active++;

    pthread_mutex_unlock(&pthread_info.mutex);


    /*** Spawn a thread to process this request ***/

    DPRINTF(("SERVER: main(), spawing worker thread\n"));

    worker_threadp = (pthread_t *)malloc(sizeof(pthread_t));

    pthread_create(worker_threadp,  
		   NULL,                    /* Use default attributes */
		   process_request,  
	 	   (void *) workorderp );

    pthread_detach(*worker_threadp);

  }

  DPRINTF(("SERVER: main(), shuting down\n"));

  server_comm_shutdown();

  return 0; 
}

/************************************************************************* 
 * Base worker routine
 *
 * Thread looks at workorder req_buf field to determine requested operation.
 * Executes assoicated account routine, sends response back to client and
 * then will do an implicit exit when this routine finishes.
 *************************************************************************/
void *process_request(void *input_orderp)
{
  workorder_t *workorderp = (workorder_t *)input_orderp;
  char resp_buf[COMM_BUF_SIZE];
  int  trans_id;                /* The transaction type of the request */

  DPRINTF(("SERVER worker thread: processing request \n"));
 
  sscanf(workorderp->req_buf, "%d", &trans_id);
  
  DPRINTF(("SERVER worker thread: processing request \n"));

  switch(trans_id) {
    
      case OPEN_ACCT_TRANS:
           open_account(resp_buf);
           break;

      case DEPOSIT_TRANS:
	   deposit(workorderp->req_buf, resp_buf);
	   break;

      case WITHDRAW_TRANS:
	   withdraw(workorderp->req_buf, resp_buf);
	   break;

      case BALANCE_TRANS:
	   balance(workorderp->req_buf, resp_buf);
	   break;

      default: 
	   handle_bad_trans_id(workorderp->req_buf, resp_buf);
	   break;

      }		

  server_comm_send_response(workorderp->conn, resp_buf);

  free(workorderp);
    
  DPRINTF(("SERVER worker thread: response sent\n"));
  pthread_mutex_lock(&pthread_info.mutex);
  pthread_info.num_active--;
  pthread_cond_signal(&pthread_info.thread_exit_cv);
  pthread_mutex_unlock(&pthread_info.mutex);

  return(NULL);
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
  sprintf(atm_err_tbl[-1*ERR_NUM_DATA_UNAVAIL], "%s", ERR_MSG_DATA_UNAVAIL);
  sprintf(atm_err_tbl[-1*ERR_NUM_BAD_ACCOUNT],  "%s", ERR_MSG_BAD_ACCOUNT);
  sprintf(atm_err_tbl[-1*ERR_NUM_BAD_PASSWORD], "%s", ERR_MSG_BAD_PASSWORD);
  sprintf(atm_err_tbl[-1*ERR_NUM_INS_FUNDS],    "%s", ERR_MSG_INS_FUNDS);
  sprintf(atm_err_tbl[-1*ERR_NUM_BAD_TRANSID],  "%s", ERR_MSG_BAD_TRANSID);

  pthread_info.num_active = 0;
  pthread_cond_init(&pthread_info.thread_exit_cv, NULL); /* Use default attr */
  pthread_mutex_init(&pthread_info.mutex, NULL); /* Use default attr */

#ifdef NO_DATABASE_SYNC
  DPRINTF(("atm server using no database synchronization\n"));

#elif defined(SINGLE_DATABASE_MUTEX_SYNC)
  pthread_mutex_init(&global_data_mutex, NULL);
  DPRINTF(("atm server using single mutex database synchronization\n")); 

#elif defined(ACCOUNT_MUTEX_SYNC)
  pthread_mutex_init(&open_account_mutex, NULL);
  for (i = 0; i < MAX_NUM_ACCOUNTS; i++)
    pthread_mutex_init(&account_mutex[i], NULL);
  DPRINTF(("atm server using per account mutex database synchronization\n")); 
#endif

  init_account_db(force_create, increased_io, increased_cpu);  
  server_comm_init();
  server_password_init(force_create);

}
/**************************************************************************
 * Open an account. The first unused account is selected. A password 
 * equal to the account number is assigned by the server.
 **************************************************************************/
void open_account(char *resp_buf)
{
  int id;
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
  if ((rtn = new_account(&id, &accountp)) < 0) {
    DPRINTF(("SERVER: open new_account failed, %s\n", atm_err_tbl[-rtn]));
    sprintf(resp_buf, "%d %d %d %s", 
	    TRANS_FAILURE, 
	    -1, 
	    -1,
	    atm_err_tbl[-rtn]);
  } else {
    accountp->password = id;
    accountp->balance  = 0;
    
    /* Store it back to the database */
    if ((rtn = store_account(accountp)) < 0) {
      DPRINTF(("SERVER: open, store_account failed %s\n", atm_err_tbl[-rtn]));
      sprintf(resp_buf, "%d %d %d %s", 
	    TRANS_FAILURE, 
	    -1, 
	    -1,
	    atm_err_tbl[-rtn]);
    } else {
      sprintf(resp_buf, "%d %d %d", 
	    TRANS_SUCCESS,  
	    id, 
	    id);
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
void deposit(char *req_buf, char *resp_buf)
{
  int rtn;
  int temp, id, password, amount;
  account_t *accountp;

  /* Parse input string */
  sscanf(req_buf, "%d %d %d %d ", &temp, &id, &password, &amount);
  DPRINTF(("SERVER: deposit, id %d, password %d, amount %d\n", 
	 id, password, amount));
  
  /* Check inputs */
  if ((id < 0) || (id >= MAX_NUM_ACCOUNTS)) {
    DPRINTF(("SERVER: deposit, %s\n", ERR_MSG_BAD_ACCOUNT));
    sprintf(resp_buf, "%d %s",
	    TRANS_FAILURE, 
	    ERR_MSG_BAD_ACCOUNT);
    return;
  }

#ifdef NO_DATABASE_SYNC

#elif defined(SINGLE_DATABASE_MUTEX_SYNC)
  pthread_mutex_lock(&global_data_mutex);

#elif defined(ACCOUNT_MUTEX_SYNC)
  pthread_mutex_lock(&account_mutex[id]);

#endif

  /* Retrieve account from database */
  if ((rtn = retrieve_account( id, &accountp)) < 0) {
    DPRINTF(("SERVER: deposit, retrieve_account failed %s\n", atm_err_tbl[-rtn]));
    sprintf(resp_buf, "%d %s",
	    TRANS_FAILURE, 
	    atm_err_tbl[-rtn]);

  /* Check password */
  } else if (password != accountp->password)  {
    DPRINTF(("SERVER: deposit, %s\n", ERR_MSG_BAD_PASSWORD));
    sprintf(resp_buf, "%d %s",
	    TRANS_FAILURE, 
	    ERR_MSG_BAD_PASSWORD);

  /* Add new balance */
  } else {
    (accountp->balance) += amount;

    /* Store back to database */
    if ((rtn = store_account(accountp)) < 0) {
      DPRINTF(("SERVER: deposit, store_account failed %s\n", atm_err_tbl[-rtn]));
      sprintf(resp_buf, "%d %d %d %s", 
	     TRANS_FAILURE, 
	    -1, 
	    -1,
	    atm_err_tbl[-rtn]);

    /* Everything OK */
    } else { 
      sprintf(resp_buf, "%d",
	      TRANS_SUCCESS);
    }
  }

/*
#ifdef NO_DATABASE_SYNC

#elif defined(SINGLE_DATABASE_MUTEX_SYNC)
  pthread_mutex_unlock(&global_data_mutex);

#elif defined(ACCOUNT_MUTEX_SYNC)
  pthread_mutex_unlock(&account_mutex[id]);

#endif
*/

}

/************************************************************************** 
 * Remove money from an account. The account id and access password are 
 * verified, and sufficient funds must be available for the transaction
 * to proceed.
 **************************************************************************/
void withdraw(char *req_buf, char *resp_buf)
{
  int rtn;
  int temp, id, password, amount;
  account_t *accountp;

  /* Parse input string */
  sscanf(req_buf, "%d %d %d %d", &temp, &id, &password, &amount);
  DPRINTF(("SERVER: withdraw,  id %d, password %d, amount %d\n", 
	  id, password, amount));


  /* Check inputs */
  if ((id < 0) || (id >= MAX_NUM_ACCOUNTS)) {
    DPRINTF(("SERVER: withdraw, %s", ERR_MSG_BAD_ACCOUNT));
    sprintf(resp_buf, "%d %s",
	    TRANS_FAILURE,
	    ERR_MSG_BAD_ACCOUNT);
    return;
  }

#ifdef NO_DATABASE_SYNC

#elif defined(SINGLE_DATABASE_MUTEX_SYNC)
  pthread_mutex_lock(&global_data_mutex);

#elif defined(ACCOUNT_MUTEX_SYNC)
  pthread_mutex_lock(&account_mutex[id]);

#endif

  /* Retrieve account from database */
  if ((rtn = retrieve_account( id, &accountp)) < 0) {
    DPRINTF(("SERVER: withdraw, retrieve_account failed %s\n", 
	     atm_err_tbl[-rtn]));
    sprintf(resp_buf, "%d %s",
	    TRANS_FAILURE, 
	    atm_err_tbl[-rtn]);

  /* Check password */
  } else if (password != accountp->password)  {
    DPRINTF(("SERVER: withdraw, %s\n", ERR_MSG_BAD_PASSWORD));
    sprintf(resp_buf, "%d %s",
	    TRANS_FAILURE, 
	    ERR_MSG_BAD_PASSWORD);

  /* Check for funds */
  } else if (amount > accountp->balance) {
    DPRINTF(("SERVER: withdraw, %s\n", ERR_MSG_INS_FUNDS));
    return_account(accountp);
    sprintf(resp_buf, "%d %s",
	    TRANS_FAILURE, 
	    ERR_MSG_INS_FUNDS);

  /* Compute new balance */
  } else {
    accountp->balance -= amount;
    if ((rtn = store_account(accountp)) < 0) {
      DPRINTF(("SERVER: withdraw, store_account failed %s\n", atm_err_tbl[-rtn]));
      sprintf(resp_buf, "%d %d %d %s", 
	      TRANS_FAILURE, 
	      -1, 
	      -1,
	      atm_err_tbl[-rtn]);
    } else { 
      sprintf(resp_buf, "%d",
	      TRANS_SUCCESS);
    }
  }

#ifdef NO_DATABASE_SYNC

#elif defined(SINGLE_DATABASE_MUTEX_SYNC)
  pthread_mutex_unlock(&global_data_mutex);

#elif defined(ACCOUNT_MUTEX_SYNC)
  pthread_mutex_unlock(&account_mutex[id]);

#endif

}

/************************************************************************** 
 * Provide the current balance in an account. Account id and access password
 * are verified.
 **************************************************************************/
void balance(char *req_buf, char *resp_buf)
{
  int rtn;
  int temp, id, password;
  int cur_bal;
  account_t *accountp;

  /* Parse input string */
  sscanf(req_buf, "%d %d %d", &temp, &id, &password);
  DPRINTF(("SERVER: balance, id %d, password %d\n", id, password));

  /* Check inputs */
  if ((id < 0) || (id >= MAX_NUM_ACCOUNTS)) {
    DPRINTF(("SERVER: balance, %s", ERR_MSG_BAD_ACCOUNT));
    sprintf(resp_buf, "%d %s",
	    TRANS_FAILURE,
	    ERR_MSG_BAD_ACCOUNT);
    return;
  }

#ifdef NO_DATABASE_SYNC

#elif defined(SINGLE_DATABASE_MUTEX_SYNC)
  pthread_mutex_lock(&global_data_mutex);

#elif defined(ACCOUNT_MUTEX_SYNC)
  pthread_mutex_lock(&account_mutex[id]);

#endif

  /* Retrieve account from database */
  if ((rtn = retrieve_account( id, &accountp)) < 0) {
    DPRINTF(("SERVER: balance, retrieve_account failed %s\n", 
	     atm_err_tbl[-rtn]));
    sprintf(resp_buf, "%d %s",
	    TRANS_FAILURE, 
	    atm_err_tbl[-rtn]); 
    
  } else {
    
    /* Check password */
    if (password != accountp->password)  {
      DPRINTF(("SERVER: balance, %s\n", ERR_MSG_BAD_PASSWORD));
      sprintf(resp_buf, "%d %s",
	      TRANS_FAILURE, 
	      ERR_MSG_BAD_PASSWORD);
    } else {
      
      /* Get balance */
      cur_bal = accountp->balance;
      sprintf(resp_buf, "%d %d",
	      TRANS_SUCCESS,
	      cur_bal);
      
    }
    
    return_account(accountp);

  }

#ifdef NO_DATABASE_SYNC

#elif defined(SINGLE_DATABASE_MUTEX_SYNC)
  pthread_mutex_unlock(&global_data_mutex);

#elif defined(ACCOUNT_MUTEX_SYNC)
  pthread_mutex_unlock(&account_mutex[id]);

#endif

}

/************************************************************************** 
 * Provide a means to remotely shutdown server.
 * Any valid account can do it under this simple scheme.
 **************************************************************************/
int shutdown_req(char *req_buf, char *resp_buf)
{
  int temp, password;
  
  /* Parse input string */
  sscanf(req_buf, "%d %d", &temp, &password);
  DPRINTF(("SERVER: shutdown, password %d\n", password));

  /* Check password */
  if (check_server_password(password) != 1) {
    DPRINTF(("SERVER: shutdown_req, %s\n", ERR_MSG_BAD_PASSWORD));
    sprintf(resp_buf, "%d %s",
	    TRANS_FAILURE, 
	    ERR_MSG_BAD_PASSWORD);
    return 0;
  } else {
    /* ok, go ahead */
    sprintf(resp_buf, "%d",
	    TRANS_SUCCESS);
    return 1;
  }
}


/************************************************************************** 
 * Respond to client when transaction is unrecognized.
 **************************************************************************/
void handle_bad_trans_id(char *req_buf, char *resp_buf)
{
  int trans_id;

  /* Parse input string */
  sscanf(req_buf, "%d ", &trans_id);
  DPRINTF(("SERVER: bad transaction id, trans id %d\n", trans_id));
  sprintf(resp_buf, "%d %s",
	  TRANS_FAILURE,
	  ERR_MSG_BAD_TRANSID);
}


