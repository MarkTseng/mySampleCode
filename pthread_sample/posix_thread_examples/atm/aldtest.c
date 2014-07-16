/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 * aldtest.c
 *
 * Performance load test program for atm example program.
 *
 * Parent program sets up a number of accounts with the 
 * atm_server then records the time and spawns a number 
 * of child processes that make their own connections to 
 * the server and access the accounts. When the children
 * have exited the parent records the time.
 *
 * Arguments;
 * 
 * aldtest [-n|-c]  num_clients accounts_per_client 
 *         cycles_per_accounts [hot_spot_rate]
 *
 * contention flag       - 'n' no contention clients share
 *                          no accounts.
 *                         'c' contention, once every cycle
 *                         through their accounts the clients
 *                         access a "hot spot" account.
 * num_clients           - Number of test client processes 
 * accounts_per_client   - accounts accessed per Client 
 * cycles_per_accounts   - number of access cycles by a client 
 *                         on its accounts.
 * hot_spot_rate         - if contention test, how often
 *                         to reference the hot spot account.
 *                         4 means that four every fouth account
 *                         the client will substitute the hot spot
 *                         account for one of its own in the test.
 *                           
 * For each account accessed the client performs a deposit, 
 * withdraw and balance.
 *
 * When comparing results from tests it is important to maintain 
 * a equivelent amount of "work" being done in eatm.h case. The 
 * metric that most closely represents this is total access cycles
 * delivered by the server;
 *
 * access_cycles 
 *      = num_clients * account_per_client * cycles_per_accounts
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

#include "atm.h"
#include "atm_com.h"
#include "atm_client_rpc.h"

#define NUM_CLIENTS               10
#define ACCNTS_PER_CLIENT          5
#define CYCLES_PER_ACCNTS          5
#define CONTENTION_FLAG          'n'

struct accnt {
  int id;
  int password;
} *accnts;

/* Test Parameters */
char cont = CONTENTION_FLAG;
int num_clnts = NUM_CLIENTS;
int accnts_per_clnt = ACCNTS_PER_CLIENT;
int cycs_per_accnts  = CYCLES_PER_ACCNTS; 
int hot_rate = 1;
int contention = 0;
int num_accnts = 0;

/* Accomplished "work" Metric */
int access_cycs = 0;
int accnts_accessed = 0;


/* Low level per accnt work parameters */ 
int deposit_amnt      = 10000; 
int withdraw_amnt     =  5000; 

void catchit(int);
int atm_load_routine(int);
int access_account_wo(int, int);


/******************************************************************
 * main()
 * 
 * Reads command line arguments contacts server and initializes
 * test accnts.
 * Forks child processes who wait for a signal to start are go 
 * into their main routine atm_load_routine.
 * Parent gets the time, sends signal to children to start then
 * waits for them to exit. When they have exited the parent
 * checks the time and prints the result.
 ****************************************************************/
extern int 
main(int argc, char **argv)
{
  int rtn;
  int i, status;
  struct sigaction action;
  pid_t parent_pg;
  pid_t *child_pids;
  struct timeval stime, etime;
  long    diff_secs, diff_usecs;
  double  elapsed_time_sec;

  DPRINTF(("LOADTEST, Parent: pid %d, ppid %d, pgrp %d\n", getpid(), getppid(), getpgrp())); 

  /* move to own new process group so that signals exchanged with
     children do not end up hitting anyone else! */
  if ((rtn = setpgid(0,getpid())) < 0) {
	perror("changing pgrp");
	exit(1);
	}

  DPRINTF(("LOADTEST, Parent: changed pgrp to %d\n", getpgrp()));

  if (argc >= 6)  sscanf(argv[5], "%d", &hot_rate);
  if (argc >= 5)  sscanf(argv[4], "%d", &cycs_per_accnts); 
  if (argc >= 4)  sscanf(argv[3], "%d", &accnts_per_clnt); 
  if (argc >= 3)  sscanf(argv[2], "%d", &num_clnts); 
  if (argc >= 2)  {
    if ((rtn = strcmp(argv[1], "-c")) == 0) contention = 1;
    if (((rtn = strcmp(argv[1], "-h")) == 0) || 
	((rtn = strcmp(argv[1], "h")) == 0)  ||	
	((rtn = strcmp(argv[1], "help")) == 0)) {
      printf("  %s [-c] [num_clients accnts_per_clnt cycs_per_accnts hot_rate]\n",
	     argv[0] );

      printf("\t-c\t\tAdd contention for hot spot account.\n");
      printf("\tnum_clients\tThe number of child client processes.\n");
      printf("\taccnts_per_clnt\tThe number of accounts per child client.\n");
      printf("\tcycs_per_accnts\tThe number of times child client will test\n");
      printf("\t\t\taccess to its accounts. Eatm.h access to an account\n");
      printf("\t\t\tis 4 deposit requests.\n");
      printf("\thot_rate\tRate that child client will access hot spot\n");
      printf("\t\t\taccount in place of one of its own.\n");
      printf("\t\t\t(1 is everytime,.... 4 is 1 out of 4 etc.)\n");
      exit(1);
    }
  }


  num_accnts = num_clnts * accnts_per_clnt;
  access_cycs = num_clnts * accnts_per_clnt * cycs_per_accnts;

  /* Check Boundries on Test Parameters */
  if ((num_clnts <= 0) || (accnts_per_clnt <= 0) || 
      (cycs_per_accnts <= 0) || (hot_rate <= 0)) {
    fprintf(stderr, "Input parameters must be greater than zero\n");
    exit (1);
  }

  accnts = (struct accnt *)malloc(sizeof(struct accnt)*(num_accnts + 
							contention));
  child_pids = (pid_t *)malloc(sizeof(pid_t)*num_clnts);

  printf("Atm load test,\n");
  if (contention)
    printf("\tContention for one account (flag %c)\n", cont);
  else
    printf("\tNo contention for no accounts (flag %c)\n", cont);
  printf("\tNumber of clients\t\t\t\t%5d\n", num_clnts); 
  printf("\tNumber of accounts per client\t\t\t%5d\n",
	 accnts_per_clnt); 
  printf("\tAccess cycles per client accounts\t\t%5d\n",
	 cycs_per_accnts);
  if (contention) printf("\tHot spot hit rate\t\t\t\t1 in %d\n", hot_rate);
  printf("\tNumber of accounts\t\t\t\t%5d\n", num_accnts);
  if (contention) printf("\t(Plus one hot spot account)\n"); 
  printf("\tNumber of account cycles\t\t\t%5d\n", access_cycs);

  /* Initialize communications with server */
  client_comm_init();
  
  /* Open test accnts */
  for (i = 0; i < num_accnts + contention; i++) {
    if ((rtn = accnt_open_rpc(&(accnts[i].id), &(accnts[i].password))) 
	< 0) {
      fprintf(stderr, "Unable to open test account %d\n", i);
      exit(1);
    }
  }

  /* Shutdown communications with server */
  client_comm_shutdown();

  DPRINTF(("LOAD_TEST, Parent: Starting load test children\n"));

  /* Start children */ 
  for (i = 0; i < num_clnts; i++) {

    DPRINTF(("LOAD_TEST, Parent: Creating child %d\n",i));

    if ((child_pids[i] = fork()) == 0) {

      DPRINTF(("LOAD_TEST, Child %d: I'm alive pid %d, ppid %d, pgrp %d\n", i,getpid(),getppid(),getpgrp()));

      /* Initialize communications */
      client_comm_init();
      
      /* Setup handler and suspend until start signal */
      action.sa_handler = catchit;
      if (sigaction(SIGUSR1, &action, NULL) < 0)
	perror("load test child sigaction"), exit(1);
      pause();	

      /* Do work */
      if (atm_load_routine(i) < 0) {
	fprintf(stderr,
		"aldtest.c, atm_stress_routine returned failure\n");
	exit(1); 
      }

      /* All done */
      DPRINTF(("LOAD_TEST, Child %d: complete and exiting\n", i));
      exit(0);

    }
    /* Parent continues */
  }

  DPRINTF(("LOAD_TEST, Parent: Children Created\n"));
  DPRINTF(("LOAD_TEST, Parent: Masking SIGUSR1 from self\n"));

  /* setup handler to ignore the signal to be sent to children */
  action.sa_handler	= SIG_IGN;
  if (sigaction(SIGUSR1, &action, NULL) < 0)
    perror("load test parent sigaction"), exit(1);
  parent_pg = getpgrp();

  DPRINTF(("LOAD_TEST, Parent: Giving children %d secs to get setup\n",1 + (int)(.1*num_clnts)));
  
  sleep(1 + (int)(.1*num_clnts));

  /* Record start time */
  rtn = gettimeofday(&stime,(void *)0);

  /* send start signal */
  rtn = kill(-1*parent_pg, SIGUSR1);

  DPRINTF(("LOAD_TEST, Parent: Sent start signal SIGUSR1\n"));

  for (i = 0; i < num_clnts; i++) {
    rtn = waitpid(child_pids[i], &status, 0);
    if (rtn != child_pids[i]) {
      perror("waitpid");
      fprintf(stderr,
	      "wait returned something other than the pid\n");
    }
    if (status != 0) {
      fprintf(stderr,
	      "LOAD_TEST, Parent, client %d exited with error\n", i);
      exit(1);
    }
    DPRINTF(("LOAD_TEST, Parent: Child %d has finished\n", i)); 
  }
  DPRINTF(("LOAD_TEST, Parent: All children have completed\n"));

  /* Record finish time */
  rtn = gettimeofday(&etime,(void *)0);

  /* output results */
  diff_secs = (signed long) (etime.tv_sec - stime.tv_sec);
  diff_usecs = (signed long) (etime.tv_usec - stime.tv_usec);

  DPRINTF(("\nLOAD_TEST, diff_secs %ld, diff_usecs %ld\n",
	   diff_secs, diff_usecs)); 

  elapsed_time_sec = (double)diff_secs + (double)diff_usecs*1E-6;

  printf("\n\tTest Complete:\n");
  printf("\t\tElapsed time\t\t\t\t%5.3f sec\n", elapsed_time_sec);

  return 0; 
}

/********************************************************************
 * Signal Handler for child processes.
 *
 * Does nothing.
 *******************************************************************/
void catchit(int signo)
{
  DPRINTF(("LOAD_TEST, Child in sighandler, %d\n", signo));
}


/*******************************************************************
 * Main child process server stress routine
 *
 * For the number of accnts the child is responsible for the routine
 * deposits, withdraws and gets the balanace (dwb) a set number of
 * times for that accnt. 
 ******************************************************************/
int atm_load_routine(int child_id)
{
  int i = 0,j = 0;
  int start_accnt = 0, cur_accnt = 0;

  
  DPRINTF(("LOAD_TEST, Child %d: atm_load_routine\n", child_id));
  
  start_accnt = child_id*accnts_per_clnt;  

  for (j = 0; j < cycs_per_accnts; j++) { 

    cur_accnt = start_accnt;

    for (i = 0; i < accnts_per_clnt; i++) {
    
      if ((accnts_accessed%hot_rate == 0) && (contention)) {

	DPRINTF(("Child %i: hitting hot spot, (skipping %d) accnts_accessed %d\n", 
		 child_id, cur_accnt, accnts_accessed));
	if (access_account_wo(num_accnts, child_id) < 0)

	  return -1;
	
      } else {

	DPRINTF(("Child %i: cur_accnt %d, accnts_accessed %d\n", 
		 child_id, cur_accnt, accnts_accessed));
	if (access_account_wo(cur_accnt, child_id) < 0)
	  return -1;

      }
   
      accnts_accessed++;
      cur_accnt++;

    }
  }
  return 0;
}


/*************************************************************
 * access account - write only - deposit requests
 * 
 *************************************************************/
int access_account_wo(int accnt, int child_id)
{
  struct accnt *ap;
  int i;
  ap = &accnts[accnt];

  for (i = 0; i < 4; i++) {
    if (accnt_deposit_rpc(ap->id, ap->password, deposit_amnt) < 0) {
      fprintf(stderr, "Child %d: accnt_deposit_rpc failed\n",
	      child_id);
      return -1;
    }
  }


}

/*************************************************************
 * access_account  - read and write - balance, deposit, withdraw, 
 *                                    balance
 * 
 *************************************************************/
int access_account_rw(int accnt, int child_id)
{
  int b_before, b_after;
  struct accnt *ap;
  
  ap = &accnts[accnt];
  
  if (accnt_balance_rpc(ap->id, ap->password, &b_before) < 0 ) {
    fprintf(stderr, "Child %d: accnt_balance_rpc failed\n",
	    child_id);
    return -1;
  }
  if (accnt_deposit_rpc(ap->id, ap->password, deposit_amnt) < 0) {
    fprintf(stderr, "Child %d: accnt_deposit_rpc failed\n",
	    child_id);
    return -1;
  }
  if (accnt_withdraw_rpc(ap->id, ap->password, withdraw_amnt) < 0) {
    fprintf(stderr, "Child %d: accnt_withdraw_rpc failed\n", 
	    child_id);
    return -1;
  }
  if (accnt_balance_rpc(ap->id, ap->password, &b_after) < 0 ) {
    fprintf(stderr, "Child %d: accnt_balance_rpc failed\n",
	    child_id);
    return -1;
  }
  if (((b_after - b_before) != (deposit_amnt - withdraw_amnt)) && 
      (!contention)) {
    fprintf(stderr, "Child %d: balance missmatch %d %d, %d %d\n", 
	    child_id, b_before, b_after, deposit_amnt, 
	    withdraw_amnt); 
    return -1;
  }
}

/*************************************************************
 * access account - read only - balance requests
 * 
 *************************************************************/
int access_account_ro(int accnt, int child_id)
{
  int b_before, b_after;
  struct accnt *ap;
  int i;
  ap = &accnts[accnt];

  for (i = 0; i < 4; i++) {
    if (accnt_balance_rpc(ap->id, ap->password, &b_before) < 0 ) {
      fprintf(stderr, "Child %d: accnt_balance_rpc failed\n",
	      child_id);
      return -1;
    }
  }
}













