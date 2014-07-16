/* asrp.c
 *
 * Created by Brad Nichols, 10/20/93
 *
 * Multi-Process Server program for the Automated Teller Machine (ATM) 
 * example program.
 *
 * Maintains a database of bank accounts accepting requests from
 * client programs to perform transactions on them.
 *
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/table.h>

#include "a.h"
#include "acom.h"
#include "adb.h"
#include "asrpw.h"

static char    atm_err_tbl[NUM_ERR_MSG][ERR_MSG_SIZE];

#define        MAX_NUM_CHILDREN 30
key_t          num_active_sem_key = 0x111;
int            num_active_sem_id;
struct sembuf  num_active_sem_buf;
pid_t          child_pid;


/* Database Account Access Synchronization Options, uncomment on of the 
   following; */

/* #define NO_DATABASE_SYNC */
/* #define SINGLE_DATABASE_SEM_SYNC */
#define ACCOUNT_SEM_SYNC 

#ifdef NO_DATABASE_SYNC
                                                 /* No synchronization, 
						    account data may be 
						    corrupted by race
						    conditions */
#elif defined(SINGLE_DATABASE_SEM_SYNC)
key_t          single_database_sem_key = 0x222;
int            single_database_sem_id;
struct sembuf  single_database_sem_buf;

#elif defined( ACCOUNT_SEM_SYNC)
key_t          open_account_sem_key = 0x333;
int            open_account_sem_id;
struct sembuf  open_account_sem_buf;

/* Semaphores per set and total semaphores per host are system limited \
   in DEC OSF/1 to rather low (in my opinion) default values of 25 and 10.
   These can be configured larger by rebuilding a new kernel, but who wants
   to do that to run a simple little demo program?. 
   Here I'll map some minium number of semophore sets (to obain the required
   one semaphore per account) into a linear array of semaphore reference 
   information.
*/
#define        ACCOUNT_SEM_ARRAY_KEY_BASE 0x444
typedef struct { 
  int            set_id;
  int            sem_num;
} sem_array_t;
sem_array_t    account_sem_array[MAX_NUM_ACCOUNTS];

struct sembuf  account_sem_buf;
#endif

/* Forward declarations of routines */
void process_request(char *, int);
void atm_server_init(int, char **);
void open_account(char *);
void deposit(char *, char *);
void withdraw(char *, char *);
void balance(char *, char *);
int  shutdown_req(char *, char *);
void handle_bad_trans_id(char *, char *);

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
  int  status;
  pid_t rtn_pid;

  atm_server_init(argc, argv);

  for(;;) {

    /*** Wait for a request ***/

    DPRINTF(("SERVER: main(), going to get a request\n"));
    server_comm_get_request(&conn, req_buf);
    DPRINTF(("SERVER: main(), got a request\n"));


    /*** Is it a shutdown request? ***/

    sscanf(req_buf, "%d", &trans_id);
    if (trans_id == SHUTDOWN) {
      DPRINTF(("SERVER: main(), processing a shutdown request\n"));
      
      /* process it here with the parent */
      if (shutdown_req(req_buf, resp_buf)) {
	server_comm_send_response(conn, resp_buf);
	break;
      }
    }

    /*** Have we exceeded our limit of active child processes ? ***/

    /* "p()" num active semaphore */
    /* Initial value is set a MAX_NUM_CHILDREN so that this operation
       will only wait if that number of "p"s have already been done. */
    num_active_sem_buf.sem_num = 0;
    num_active_sem_buf.sem_op  = -1; /* "p()" */
    num_active_sem_buf.sem_flg = SEM_UNDO;
    if (semop(num_active_sem_id, &num_active_sem_buf, 1) == -1) {
      perror("SERVER main(), num_active p semop() failed %d\n");
      exit (1);
    }

    /*** fork a child process for request ***/

    DPRINTF(("SERVER: main(), spawing child process \n"));

    if ((child_pid = fork()) < 0) {
      /* error */
      perror("SERVER main(), fork failed\n");
      exit(1);
    } else if (child_pid == 0) {
      /* child */
      child_pid = getpid();
      process_request(req_buf, conn);
    }

    /* "v()" num active semaphore */

    num_active_sem_buf.sem_num = 0;
    num_active_sem_buf.sem_op  = 1; /* "v()" */
    num_active_sem_buf.sem_flg = SEM_UNDO;
    if (semop(num_active_sem_id, &num_active_sem_buf, 1) == -1) {
      perror("SERVER main(), num_active v semop() failed %d\n");
      exit (1);   
    }

    /* clean-up any zoombie child processes by doing a non-blocking wait */
    rtn_pid = waitpid(-1, &status, WNOHANG);

  }
  DPRINTF(("SERVER: main(), shuting down\n"));

  server_comm_shutdown();

}

/************************************************************************* 
 * Base worker routine
 *
 * Child process looks at req_buf to determine requested operation.
 * Executes assoicated account routine, sends response back to client.
 *************************************************************************/
void process_request(char *req_buf, int conn)
{
  char resp_buf[COMM_BUF_SIZE];
  int  trans_id;                /* The transaction type of the request */

  sscanf(req_buf, "%d", &trans_id);
  
  DPRINTF(("SERVER: child process %d, processing request \n", child_pid));

  /* "p()" num active semaphore */
  num_active_sem_buf.sem_num = 0;
  num_active_sem_buf.sem_op  = -1; /* "p()" */
  num_active_sem_buf.sem_flg = SEM_UNDO;
  if (semop(num_active_sem_id, &num_active_sem_buf, 1) == -1) {
    perror("SERVER: child process, num_active p semop() failed %d\n");
    exit (1);
  }
  

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

      default: 
	   handle_bad_trans_id(req_buf, resp_buf);
	   break;

      }		

  server_comm_send_response(conn, resp_buf);

  DPRINTF(("SERVER: child process %d, response sent\n", child_pid));

  /* "v()" num active semaphore */
  num_active_sem_buf.sem_num = 0;
  num_active_sem_buf.sem_op  = 1; /* "v()" */
  num_active_sem_buf.sem_flg = SEM_UNDO;
  if (semop(num_active_sem_id, &num_active_sem_buf, 1) == -1) {
    perror("SERVER child process, num_active v semop() failed %d\n");
    exit (1);
  }

  exit(0);
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

#ifdef ACCOUNT_SEM_SYNC
  key_t sem_set_key;
  int   sem_set_id;
  int   sem_set_sem_num;
  int   sems_per_set;
#endif 


  /* process input arguments */
  if (argc >= 4) sscanf(argv[3], "%d", &increased_cpu);
  if (argc >= 3) sscanf(argv[2], "%d", &increased_io); 
  if (argc >= 2) {
    if ((rtn = strcmp(argv[1], "-c")) == 0)
      force_create = 1;
    if (((rtn = strcmp(argv[1], "-h")) == 0) || 
	((rtn = strcmp(argv[1], "h")) == 0)  ||	
	((rtn = strcmp(argv[1], "help")) == 0)) {
      printf("%s [-c] [increased_io increased_cpu]\n", argv[0]);
      printf("\t-c\t\tcreate a new database\n");
      printf("\tincreased_io\tadd extra io work to storing a account\n");
      printf("\tincreased_cpu\tadd extra cpu work to storing an account\n");
      exit(1);
    }
  }

#ifdef NO_DATABASE_SYNC 
  printf("\t%s: USING NO SYNCHRONIZATION TO ACCOUNTS\n", argv[0]);
#endif
#ifdef SINGLE_DATABASE_SEM_SYNC 
  printf("\t%s: USING ONE LOCK FOR ALL ACCOUNTS\n", argv[0]);
#endif

  /* Initialize error translation table. */ 
  sprintf(atm_err_tbl[-1*ERR_NUM_NO_ACCOUNTS],  "%s", ERR_MSG_NO_ACCOUNTS);
  sprintf(atm_err_tbl[-1*ERR_NUM_DATA_UNAVAIL], "%s", ERR_MSG_DATA_UNAVAIL);
  sprintf(atm_err_tbl[-1*ERR_NUM_BAD_ACCOUNT],  "%s", ERR_MSG_BAD_ACCOUNT);
  sprintf(atm_err_tbl[-1*ERR_NUM_BAD_PASSWORD], "%s", ERR_MSG_BAD_PASSWORD);
  sprintf(atm_err_tbl[-1*ERR_NUM_INS_FUNDS],    "%s", ERR_MSG_INS_FUNDS);
  sprintf(atm_err_tbl[-1*ERR_NUM_BAD_TRANSID],  "%s", ERR_MSG_BAD_TRANSID);

  /* Initialize num_active semaphore */
  if ((num_active_sem_id = semget(num_active_sem_key,
			     1,
			     0600 | IPC_CREAT | IPC_EXCL)) < 0 ) {
    if (errno == EEXIST)
      num_active_sem_id = semget(num_active_sem_key, 1, 0);
  }
  if ((rtn = semctl(num_active_sem_id, 0, SETVAL, MAX_NUM_CHILDREN)) < 0) {
    perror("SERVER main(): semctl num_active_sem failed\n");
    exit (1);
  }


#ifdef NO_DATABASE_SYNC

#elif defined(SINGLE_DATABASE_SEM_SYNC)
  /* Initialize single database semaphore */
  if ((single_database_sem_id = semget(single_database_sem_key,
			     1,
			     0600 | IPC_CREAT | IPC_EXCL)) < 0 ) {
    if (errno == EEXIST)
      single_database_sem_id = semget(single_database_sem_key, 1, 0);
  }
  if ((rtn = semctl(single_database_sem_id, 0, SETVAL, 1)) < 0) {
    perror("SERVER main(): semctl single_database_sem failed\n");
    exit (1);
  }

#elif defined(ACCOUNT_SEM_SYNC)
  /* Initialize open account semaphore */
  if ((open_account_sem_id = semget(open_account_sem_key,
			     1,
			     0600 | IPC_CREAT | IPC_EXCL)) < 0 ) {
    if (errno == EEXIST)
      open_account_sem_id = semget(open_account_sem_key, 1, 0);
  }
  if ((rtn = semctl(open_account_sem_id, 0, SETVAL, 1)) < 0) {
    perror("SERVER main(): semctl open_account_sem failed\n");
    exit (1);
  }

  /* Initialize per account semaphore array */
  rtn = table(TBL_SEMINFO, SEMINFO_MSL, &sems_per_set, 1, sizeof(int));

  for (i = 0; i < MAX_NUM_ACCOUNTS; i++) {
    /* jump to next set? */
    if ((i == 0) || ((i >= sems_per_set) && ((i % sems_per_set) == 0))) {
      sem_set_key = ACCOUNT_SEM_ARRAY_KEY_BASE + i;
      if ((sem_set_id = semget(sem_set_key,
				   sems_per_set,
				   0600 | IPC_CREAT | IPC_EXCL)) < 0 ) {
	if (errno == EEXIST) {
	   if ((sem_set_id = semget(sem_set_key, sems_per_set, 0)) < 0) {
	     perror("SERVER main(): semget account sem failed\n");
	     exit(1);
	   } 
	} else {
	  perror("SERVER main(): semget account sem failed\n");
	  exit(1);
	}
      }
    }
    
    sem_set_sem_num = ((i < sems_per_set) ? i : (i % sems_per_set));
    
    if ((rtn = semctl(sem_set_id, sem_set_sem_num, SETVAL, 1)) < 0) {
      perror("SERVER main(): semctl account sem array failed\n");
      exit (1);
    }
    account_sem_array[i].set_id  = sem_set_id;
    account_sem_array[i].sem_num = sem_set_sem_num;

  }
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

#elif defined(SINGLE_DATABASE_SEM_SYNC)
  /* "p()" single database semaphore */
  single_database_sem_buf.sem_num = 0;
  single_database_sem_buf.sem_op  = -1; /* "p()" */
  single_database_sem_buf.sem_flg = SEM_UNDO;
  if (semop(single_database_sem_id, &single_database_sem_buf, 1) == -1) {
    perror("SERVER: child process, single database p semop() failed %d\n");
    exit (1);
  }
#elif defined(ACCOUNT_SEM_SYNC)
  /* "p()" open account semaphore */
  open_account_sem_buf.sem_num = 0;
  open_account_sem_buf.sem_op  = -1; /* "p()" */
  open_account_sem_buf.sem_flg = SEM_UNDO;
  if (semop(open_account_sem_id, &open_account_sem_buf, 1) == -1) {
    perror("SERVER: child process, open account p semop() failed %d\n");
    exit (1);
  }
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

#elif defined(SINGLE_DATABASE_SEM_SYNC)
  /* "v()" single database semaphore */
  single_database_sem_buf.sem_num = 0;
  single_database_sem_buf.sem_op  = 1; /* "v()" */
  single_database_sem_buf.sem_flg = SEM_UNDO;
  if (semop(single_database_sem_id, &single_database_sem_buf, 1) == -1) {
    perror("SERVER child process, single_database v semop() failed %d\n");
    exit (1);
  }
#elif defined(ACCOUNT_SEM_SYNC)
  /* "v()" open account semaphore */
  open_account_sem_buf.sem_num = 0;
  open_account_sem_buf.sem_op  = 1; /* "v()" */
  open_account_sem_buf.sem_flg = SEM_UNDO;
  if (semop(open_account_sem_id, &open_account_sem_buf, 1) == -1) {
    perror("SERVER child process, open account v semop() failed %d\n");
    exit (1);
  }
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

#elif defined(SINGLE_DATABASE_SEM_SYNC)
  /* "p()" single database semaphore */
  single_database_sem_buf.sem_num = 0;
  single_database_sem_buf.sem_op  = -1; /* "p()" */
  single_database_sem_buf.sem_flg = SEM_UNDO;
  if (semop(single_database_sem_id, &single_database_sem_buf, 1) == -1) {
    perror("SERVER: child process, single database p semop() failed %d\n");
    exit (1);
  }
#elif defined(ACCOUNT_SEM_SYNC)
  /* "p()" account semaphore */
  account_sem_buf.sem_num = account_sem_array[id].sem_num;
  account_sem_buf.sem_op  = -1; /* "p()" */
  account_sem_buf.sem_flg = SEM_UNDO;
  if (semop(account_sem_array[id].set_id, &account_sem_buf, 1) == -1) {
    perror("SERVER: child process, account sem p semop() failed %d\n");
    exit (1);
  }
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

#ifdef NO_DATABASE_SYNC

#elif defined(SINGLE_DATABASE_SEM_SYNC)
  /* "v()" single database semaphore */
  single_database_sem_buf.sem_num = 0;
  single_database_sem_buf.sem_op  = 1; /* "v()" */
  single_database_sem_buf.sem_flg = SEM_UNDO;
  if (semop(single_database_sem_id, &single_database_sem_buf, 1) == -1) {
    perror("SERVER child process, single_database v semop() failed %d\n");
    exit (1);
  }
#elif defined(ACCOUNT_SEM_SYNC)
  /* "v()" account semaphore */
  account_sem_buf.sem_num = account_sem_array[id].sem_num;
  account_sem_buf.sem_op  = 1; /* "v()" */
  account_sem_buf.sem_flg = SEM_UNDO;
  if (semop(account_sem_array[id].set_id, &account_sem_buf, 1) == -1) {
    perror("SERVER child process, account sem v semop() failed %d\n");
    exit (1);
  }
#endif

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

#elif defined(SINGLE_DATABASE_SEM_SYNC)
  /* "p()" single database semaphore */
  single_database_sem_buf.sem_num = 0;
  single_database_sem_buf.sem_op  = -1; /* "p()" */
  single_database_sem_buf.sem_flg = SEM_UNDO;
  if (semop(single_database_sem_id, &single_database_sem_buf, 1) == -1) {
    perror("SERVER: child process, single database p semop() failed %d\n");
    exit (1);
  }
#elif defined(ACCOUNT_SEM_SYNC)
  /* "p()" account semaphore */
  account_sem_buf.sem_num = account_sem_array[id].sem_num;
  account_sem_buf.sem_op  = -1; /* "p()" */
  account_sem_buf.sem_flg = SEM_UNDO;
  if (semop(account_sem_array[id].set_id, &account_sem_buf, 1) == -1) {
    perror("SERVER: child process, account sem p semop() failed %d\n");
    exit (1);
  }
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

#elif defined(SINGLE_DATABASE_SEM_SYNC)
  /* "v()" single database semaphore */
  single_database_sem_buf.sem_num = 0;
  single_database_sem_buf.sem_op  = 1; /* "v()" */
  single_database_sem_buf.sem_flg = SEM_UNDO;
  if (semop(single_database_sem_id, &single_database_sem_buf, 1) == -1) {
    perror("SERVER child process, single_database v semop() failed %d\n");
    exit (1);
  }
#elif defined(ACCOUNT_SEM_SYNC)
  /* "v()" account semaphore */
  account_sem_buf.sem_num = account_sem_array[id].sem_num;
  account_sem_buf.sem_op  = 1; /* "v()" */
  account_sem_buf.sem_flg = SEM_UNDO;
  if (semop(account_sem_array[id].set_id, &account_sem_buf, 1) == -1) {
    perror("SERVER child process, account sem v semop() failed %d\n");
    exit (1);
  }
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

#ifdef NO_DATABASE_SYNC

#elif defined(SINGLE_DATABASE_SEM_SYNC)
  /* "p()" single database semaphore */
  single_database_sem_buf.sem_num = 0;
  single_database_sem_buf.sem_op  = -1; /* "p()" */
  single_database_sem_buf.sem_flg = SEM_UNDO;
  if (semop(single_database_sem_id, &single_database_sem_buf, 1) == -1) {
    perror("SERVER: child process, single database p semop() failed %d\n");
    exit (1);
  }
#elif defined(ACCOUNT_SEM_SYNC)
  /* "p()" account semaphore */
  account_sem_buf.sem_num = account_sem_array[id].sem_num;
  account_sem_buf.sem_op  = -1; /* "p()" */
  account_sem_buf.sem_flg = SEM_UNDO;
  if (semop(account_sem_array[id].set_id, &account_sem_buf, 1) == -1) {
    perror("SERVER: child process, account sem p semop() failed %d\n");
    exit (1);
  }
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

#elif defined(SINGLE_DATABASE_SEM_SYNC)
  /* "v()" single database semaphore */
  single_database_sem_buf.sem_num = 0;
  single_database_sem_buf.sem_op  = 1; /* "v()" */
  single_database_sem_buf.sem_flg = SEM_UNDO;
  if (semop(single_database_sem_id, &single_database_sem_buf, 1) == -1) {
    perror("SERVER child process, single_database v semop() failed %d\n");
    exit (1);
  }
#elif defined(ACCOUNT_SEM_SYNC)
  /* "v()" account semaphore */
  account_sem_buf.sem_num = account_sem_array[id].sem_num;
  account_sem_buf.sem_op  = 1; /* "v()" */
  account_sem_buf.sem_flg = SEM_UNDO;
  if (semop(account_sem_array[id].set_id, &account_sem_buf, 1) == -1) {
    perror("SERVER child process, account sem v semop() failed %d\n");
    exit (1);
  }
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


