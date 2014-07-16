/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * atm_client.c
 * 
 * Client program for Automated Teller Machine (ATM) example program.
 * Accepts user requests to perform transactions on bank accounts
 * maintained by the server.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "atm_com.h"
#include "atm.h"

#define MAX_INPUT_SIZE 256

/************************************************************************** 
 * main()
 * Basic processing loop of the bank database client.
 * The program initializies communications with the server then loops,
 * promping and receiving requests via standard io, passing them on to
 * the server then printing the response.
 ***************************************************************************/
extern int
main(void)
{
  char answer[MAX_INPUT_SIZE];
  int done = 0;
  char temp;
  int  success, account, password, amount;
  char err_msg[ERR_MSG_SIZE];
  char req_buf[COMM_BUF_SIZE], resp_buf[COMM_BUF_SIZE];
  
  for(;;) {

    printf("\nWelcome to Your Bank ATM:\n");
    printf("Open New Account, Type : O \n");
    printf("Deposit,          Type : D account password amount\n");
    printf("Withdrawal,       Type : W account password amount\n");
    printf("Balance,          Type : B account password \n"); 
    printf("Kill Server,      Type : K server_password \n"); 
    printf("Quit,             Type : Q \n");   	
    
    printf(" What transaction would you like to perform\n? ");

    bzero(answer, MAX_INPUT_SIZE), bzero(req_buf,COMM_BUF_SIZE), bzero(resp_buf, COMM_BUF_SIZE); 
    success = account = password = amount = -1;
 
    fgets(answer,MAX_INPUT_SIZE,stdin);    
    
    switch((int) answer[0]) {
      
      case 'O':
      case 'o':
               sprintf(req_buf, "%d", OPEN_ACCT_TRANS);
      
               client_comm_send_request(req_buf);
               client_comm_get_response(resp_buf);
      
               sscanf(resp_buf, "%d", &success);
               if (success) {
	        printf("Your transaction has been completed\n");
		  sscanf(resp_buf, "%d %d %d", &success, &account, &password);
		  printf("Your new account id is %d with passwd %d\n",
			 account, password);
                } else {
		  printf("Your transaction failed\n");
		  sscanf(resp_buf, "%d %s", &success, err_msg);
		  printf("%s\n", err_msg);
		}

		break;
      case 'D':
      case 'd':
	        sscanf(answer, "%c %d %d %d",
		       &temp, &account, &password, &amount);
		sprintf(req_buf, "%d %d %d %d",
			DEPOSIT_TRANS, account, password, amount);

		client_comm_send_request(req_buf);
		client_comm_get_response(resp_buf);

		sscanf(resp_buf, "%d", &success);
		if (success) {
		  printf("Your transaction has been completed\n");
                } else {
		  printf("Your transaction failed\n");
		  sscanf(resp_buf, "%d %s", &success, err_msg);
		  printf("%s\n", err_msg);
		}

	        break;
      case 'W':
      case 'w':
	       sscanf(answer, "%c %d %d %d",
		      &temp, &account, &password, &amount);
		sprintf(req_buf, "%d %d %d %d",
			WITHDRAW_TRANS, account, password, amount);

		client_comm_send_request(req_buf);
		client_comm_get_response(resp_buf);

		sscanf(resp_buf, "%d", &success);
		if (success) {
		  printf("Your transaction has been completed\n");
                } else {
		  printf("Your transaction failed\n");
		  sscanf(resp_buf, "%d %s", &success, err_msg);
		  printf("%s\n", err_msg);
		}

	        break;
      case 'B':
      case 'b': 
	        sscanf(answer, "%c %d %d", &temp, &account, &password);
		sprintf(req_buf, "%d %d %d",
			BALANCE_TRANS, account, password);		

		client_comm_send_request(req_buf);
		client_comm_get_response(resp_buf);

		sscanf(resp_buf, "%d", &success);
		if (success) {
		  printf("Your transaction has been completed\n");
		  sscanf(resp_buf, "%d %d", &success, &amount);
		  printf("Your account balance is %d", amount);
                } else {
		  printf("Your transaction failed\n");
		  sscanf(resp_buf, "%d %s", &success, err_msg);
		  printf("%s\n", err_msg);
		}

	        break;
      case 'K':
      case 'k':
	        sscanf(answer, "%c %d", &temp, &password);
		sprintf(req_buf, "%d %d",
			SHUTDOWN, password);		

		client_comm_send_request(req_buf);
		client_comm_get_response(resp_buf);

		sscanf(resp_buf, "%d", &success);
		if (success) {
		  printf("Your transaction has been completed\n");
                } else {
		  printf("Your transaction failed\n");
		  sscanf(resp_buf, "%d %s", &success, err_msg);
		  printf("%s\n", err_msg);
		}

	        break;
      case 'Q':
      case 'q':
	        done = 1;
	        break;

      default: 
	        break;
	     }
    
    if(done) break;
    
  }

  client_comm_shutdown();

  return 0;
}
