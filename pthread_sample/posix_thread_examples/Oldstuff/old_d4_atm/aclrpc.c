/* arpccl.c
 *
 * Created by Brad Nichols, 10/18/93
 *
 * RPC style call interface to access atm server.
 * ONC, DCE RPC etc. where invented so we wouldn't have to do this
 * anymore.
 * But seeing how this is for a book on pthreads and not RPC
 * generators and is ment to be as portable as possible, ....
 */

#include <stdio.h> 

#include "a.h"
#include "acom.h"

/*******************************************************************
 * Open an account with the bank.
 *
 *******************************************************************/
int accnt_open_rpc(int *idp, int *passwordp)
{
  int  success;
  char err_msg[ERR_MSG_SIZE];
  char req_buf[COMM_BUF_SIZE], resp_buf[COMM_BUF_SIZE];

  sprintf(req_buf, "%d", OPEN_ACCT_TRANS);
      
  client_comm_send_request(req_buf);
  client_comm_get_response(resp_buf);
      
  sscanf(resp_buf, "%d", &success);
  if (success) {
    sscanf(resp_buf, "%d %d %d", &success, idp, passwordp);
    return 1;
  } else {
    sscanf(resp_buf, "%d %s", &success, err_msg);
    fprintf(stderr, "Open transaction failed, %s\n", err_msg);
    return -1;
  }
}



/*********************************************************************
 * Deposit money in an account
 *
 ********************************************************************/
int accnt_deposit_rpc(int id, int password, int amount)
{
  int  success;
  char err_msg[ERR_MSG_SIZE];
  char req_buf[COMM_BUF_SIZE], resp_buf[COMM_BUF_SIZE];
  
  sprintf(req_buf, "%d %d %d %d",
	  DEPOSIT_TRANS, id, password, amount);
  
  client_comm_send_request(req_buf);
  client_comm_get_response(resp_buf);
  
  sscanf(resp_buf, "%d", &success);
  if (success) {
    return 1;
  } else {
    sscanf(resp_buf, "%d %s", &success, err_msg);
    fprintf(stderr, "Deposit transaction failed, %s\n", err_msg);
    return -1;
  }
}


/********************************************************************
 * Withdraw money form an account.
 *
 ********************************************************************/
int accnt_withdraw_rpc(int id, int password, int amount)
{
  int  success;
  char err_msg[ERR_MSG_SIZE];
  char req_buf[COMM_BUF_SIZE], resp_buf[COMM_BUF_SIZE];

  sprintf(req_buf, "%d %d %d %d",
	  WITHDRAW_TRANS, id, password, amount);

  client_comm_send_request(req_buf);
  client_comm_get_response(resp_buf);

  sscanf(resp_buf, "%d", &success);
  if (success) {
    return 1;
  } else {
    sscanf(resp_buf, "%d %s", &success, err_msg);
    fprintf(stderr, "Withdraw transaction failed, %s\n", err_msg);
    return -1;
  }
}

/*********************************************************************
 * Get the current balance of an account.
 *
 ******************************************************************/
int accnt_balance_rpc(int id, int password, int *balancep)
{
  int  success;
  char err_msg[ERR_MSG_SIZE];
  char req_buf[COMM_BUF_SIZE], resp_buf[COMM_BUF_SIZE];
 
  sprintf(req_buf, "%d %d %d",
	  BALANCE_TRANS, id, password);		
  
  client_comm_send_request(req_buf);
  client_comm_get_response(resp_buf);
  
  sscanf(resp_buf, "%d", &success);
  if (success) {
    sscanf(resp_buf, "%d %d", &success, balancep);
    return 1;
  } else {
    sscanf(resp_buf, "%d %s", &success, err_msg);
    fprintf(stderr, "Balance transaction failed %s\n", err_msg);
    return -1;
  }
}


/*********************************************************************
 * Send a request to shutdown the server.
 *
 * Requires a special server password be used.
 ********************************************************************/
int server_shutdown_rpc(int server_password)
{
  int  success;
  char err_msg[ERR_MSG_SIZE];
  char req_buf[COMM_BUF_SIZE], resp_buf[COMM_BUF_SIZE];

  sprintf(req_buf, "%d %d",
	  SHUTDOWN, server_password);		
  
  client_comm_send_request(req_buf);
  client_comm_get_response(resp_buf);
  
  sscanf(resp_buf, "%d", &success);
  if (success) {
    return 1;
  } else {
    sscanf(resp_buf, "%d %s", &success, err_msg);
    fprintf(stderr, "Shutdown server transaction failed %s\n",
	    err_msg);
    return -1;
  }
}

