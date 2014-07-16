/* asr.c
 *
 * Created by Brad Nichols, 9/1/93
 *
 * Server program for the Automated Teller Machine (ATM) example program.
 * Maintains a database of bank accounts accepting requests from
 * client programs to perform transactions on them.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "a.h"
#include "acom.h"
#include "adb.h"
#include "asrpw.h"

/* Forward declarations of routines */
void atm_server_init(int, char **);
void open_account(char *);
void deposit(char *, char *);
void withdraw(char *, char *);
void balance(char *, char *);
int  shutdown_req(char *, char *);
void handle_bad_trans_id(char *, char *);
void say_hello(int *);

static char atm_err_tbl[NUM_ERR_MSG][ERR_MSG_SIZE];

pthread_t *threads;
int *args;



/************************************************************************** 
 * main()
 * Basic processing loop of the bank database server.
 * The program initializies the database then loops, accepting
 * transaction requests from clients.
 ***************************************************************************/

main(argc, argv)
int argc;
char **argv;
{
  char req_buf[COMM_BUF_SIZE], resp_buf[COMM_BUF_SIZE];
                                /* The character string buffers passed from
				   and to client programs. The strings follow
				   a format defined in atm.h and the parameters
				   are read in and out using sptring processing
				   calls sprintf() and sscanf(). */
  int  conn;                    /* A handle on which connection (client)
                                   the request came from. Not used initially
                                   but included so that in the future different
				   requests could be processed cuncurrently. */
  int  trans_id;                /* The transaction type of the current 
				   request */
  int  done=0;

  void * status; 
  int i=0;

  atm_server_init(argc, argv);

  for(;;) {

    server_comm_get_request(&conn, req_buf);
    sscanf(req_buf, "%d", &trans_id);

    DPRINTF(("SERVER: main(), request received\n"));

    args = (int *)malloc(sizeof(int));
    *args = i;
    threads = (pthread_t *)malloc(sizeof(pthread_t));
    
    pthread_create(threads, 
		   pthread_attr_default, 
		   (void *) say_hello,
		   (void *) args); 
    i++ ;

    pthread_join(*threads, &status);

    DPRINTF(("SERVER: main(), thread join complete\n"));

    switch(trans_id) {

      case OPEN_ACCT_TRANS:
           open_account(resp_buf);
           break;

      case DEPOSIT_TRANS:
	   deposit(req_buf, resp_buf);
	   break;

      case WITHDRAW_TRANS:
	   withdraw(req_buf, resp_buf);
	   break;

      case BALANCE_TRANS:
	   balance(req_buf, resp_buf);
	   break;

      case SHUTDOWN:
	   if (shutdown_req(req_buf, resp_buf)) done = 1;
	   break;
      
      default: 
	   handle_bad_trans_id(req_buf, resp_buf);
	   break;

      }		

    server_comm_send_response(conn, resp_buf);
    
    DPRINTF(("SERVER: main(), response sent\n"));
    
    if(done) break;
    
  }

  DPRINTF(("SERVER: main(), shuting down\n"));

  server_comm_shutdown();

  
}

void say_hello(int *ptr)
{
  printf("say_hello: I'm alive\n");

  printf("say_hello: hello from %d\n", *ptr);   

}


/************************************************************************* 
 * Initializes server
 *************************************************************************/
void atm_server_init(int argc, char **argv)
{
  int rtn;
  int force_create = 0;

  /* process input arguments */
  if (argc == 2)
    if ((rtn = strcmp(argv[1], "-c")) == 0)
      force_create = 1;
    else { 
      printf("%s %s\n",argv[0], argv[1]);
      fprintf(stderr,"%s [-c]\n", argv[0]); exit (1);
    }

  if (argc > 2) {
    fprintf(stderr,"%s [-c]\n", argv[0]);
    exit (1);
  }

  /* Initialize error translation table. */ 
  sprintf(atm_err_tbl[-1*ERR_NUM_NO_ACCOUNTS],  "%s", ERR_MSG_NO_ACCOUNTS);
  sprintf(atm_err_tbl[-1*ERR_NUM_DATA_UNAVAIL], "%s", ERR_MSG_DATA_UNAVAIL);
  sprintf(atm_err_tbl[-1*ERR_NUM_BAD_ACCOUNT],  "%s", ERR_MSG_BAD_ACCOUNT);
  sprintf(atm_err_tbl[-1*ERR_NUM_BAD_PASSWORD], "%s", ERR_MSG_BAD_PASSWORD);
  sprintf(atm_err_tbl[-1*ERR_NUM_INS_FUNDS],    "%s", ERR_MSG_INS_FUNDS);
  sprintf(atm_err_tbl[-1*ERR_NUM_BAD_TRANSID],  "%s", ERR_MSG_BAD_TRANSID);

  init_account_db(force_create);  
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
  sscanf(req_buf, "%c %d %d", &temp, &id, &password);
  DPRINTF(("SERVER: balance, id %d, password %d\n", id, password));

  /* Check inputs */
  if ((id < 0) || (id >= MAX_NUM_ACCOUNTS)) {
    DPRINTF(("SERVER: balance, %s", ERR_MSG_BAD_ACCOUNT));
    sprintf(resp_buf, "%d %s",
	    TRANS_FAILURE,
	    ERR_MSG_BAD_ACCOUNT);
    return;
  }

  /* Retrieve account from database */
  if ((rtn = retrieve_account( id, &accountp)) < 0) {
    DPRINTF(("SERVER: balance, retrieve_account failed %s\n", 
	     atm_err_tbl[-rtn]));
    sprintf(resp_buf, "%d %s",
	    TRANS_FAILURE, 
	    atm_err_tbl[-rtn]);

  /* Check password */
  } else if (password != accountp->password)  {
    DPRINTF(("SERVER: balance, %s\n", ERR_MSG_BAD_PASSWORD));
    sprintf(resp_buf, "%d %s",
	    TRANS_FAILURE, 
	    ERR_MSG_BAD_PASSWORD);

  /* Get balance */
  } else {
    cur_bal = accountp->balance;
    sprintf(resp_buf, "%d %d",
	    TRANS_SUCCESS,
	    cur_bal);
    return_account(accountp);
  }
}

/************************************************************************** 
 * Provide a means to remotely shutdown server.
 * Any valid account can do it under this simple scheme.
 **************************************************************************/
int shutdown_req(char *req_buf, char *resp_buf)
{
  int temp, password;
  
  /* Parse input string */
  sscanf(req_buf, "%d %d %d", &temp, &password);
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


