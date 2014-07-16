/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * atm_client_dce.c -- 
 *
 * Client ATM code, DCE version
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dce/utc.h>
#include <dce/dce_error.h>
#include "atm_dce_interface.h"
#include "atm_dce.h"

#define CINTERFACE_SPEC     atm_v1_0_c_ifspec

main()
{
  char answer[100];
  int done = 0;
  char temp;
  int  success, account, password, amount;
  char err_msg[ERR_MSG_SIZE];

   rpc_ns_handle_t         import_context;
   unsigned32              status;
   error_status_t          rpc_comm_status;
   rpc_binding_handle_t    binding_handle;
   unsigned char server_name[]=SERVER_NAME;
   
   /******  import binding info from name space ******/
   rpc_ns_binding_import_begin (
				rpc_c_ns_syntax_default,
				server_name,
				CINTERFACE_SPEC,
				NULL,
				&import_context,
				&status);
   
   if (status != rpc_s_ok)  {
     REPORT_DCE_ERROR (status);
     exit(1);
   }
   
   rpc_ns_binding_import_next (import_context, &binding_handle, &status);
   if (status != rpc_s_ok)  {
     REPORT_DCE_ERROR (status);
   }

   rpc_ns_binding_import_done (&import_context, &status);
   if (status != rpc_s_ok) {
     REPORT_DCE_ERROR (status);
     exit(1);
   }

   if (binding_handle == NULL) {
     printf("No Binding Handle found\n");
     exit(1);
   } else {
     /* DPRINT_BINDING(binding_handle);
*/
 }


   /**** get on with life ****/

  for(;;) {

    printf("\nWelcome to Your Bank ATM:\n");
    printf("Open New Account, Type : O \n");
    printf("Deposit,          Type : D account password amount\n");
    printf("Withdrawal,       Type : W account password amount\n");
    printf("Balance,          Type : B account password \n"); 
    printf("Kill Server,      Type : K server_password \n"); 
    printf("Quit,             Type : Q \n");   	
    
    printf(" What transaction would you like to perform\n? ");
    
    gets(answer);    
    
    switch((int) answer[0]) {
      
      case 'O':
      case 'o':
                open_account(binding_handle,
                        (idl_long_int *)&account,
                        (idl_long_int *)&password,
			(idl_long_int *)&success,
			(idl_char *)err_msg);

                if (success) {
	          printf("Your transaction has been completed\n");
		  printf("Your new account id is %d with passwd %d\n",
			 account, password);
	        } else {
		  printf("Your transaction failed\n");
		  printf("%s\n", err_msg);
		}

		break;
      case 'D':
      case 'd':
	        sscanf(answer, "%c %d %d %d",
		       &temp, &account, &password, &amount);

		deposit(binding_handle,
                        (idl_long_int *)&account,
                        (idl_long_int *)&password,
                        (idl_long_int *)&amount,
			(idl_long_int *)&success,
			(idl_char *)err_msg);

		if (success) {
		  printf("Your transaction has been completed\n");
                } else {
		  printf("Your transaction failed\n");
		  printf("%s\n", err_msg);
		}

	        break;
      case 'W':
      case 'w':
	        sscanf(answer, "%c %d %d %d",
		      &temp, &account, &password, &amount);

		withdraw(binding_handle,
                        (idl_long_int *)&account,
                        (idl_long_int *)&password,
                        (idl_long_int *)&amount,
			(idl_long_int *)&success,
			(idl_char *)err_msg);

		if (success) {
		  printf("Your transaction has been completed\n");
                } else {
		  printf("Your transaction failed\n");
		  printf("%s\n", err_msg);
		}

	        break;
      case 'B':
      case 'b': 
	        sscanf(answer, "%c %d %d", &temp, &account, &password);

		balance(binding_handle,
                        (idl_long_int *)&account,
                        (idl_long_int *)&password,
			(idl_long_int *)&success,
                        (idl_long_int *)&amount,
			(idl_char *)err_msg);

		if (success) {
		  printf("Your transaction has been completed\n");
		  printf("Your account balance is %d", amount);
                } else {
		  printf("Your transaction failed\n");
		  printf("%s\n", err_msg);
		}

	        break;
      case 'K':
      case 'k':
	        sscanf(answer, "%c %d", &temp, &password);

		shutdown_server(binding_handle,
                        (idl_long_int *)&password,
			(idl_long_int *)&success,
			(idl_char *)err_msg);

		if (success) {
		  printf("Your transaction has been completed\n");
                } else {
		  printf("Your transaction failed\n");
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

}
	




