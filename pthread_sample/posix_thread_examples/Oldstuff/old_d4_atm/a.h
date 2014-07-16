/* a.h
 *
 * Created by Brad Nichols, 9/1/93
 *
 * The include file for the Automated Teller Machine (ATM) example
 * program.
 * Includes definitions used by both client and server programs.
 *
 */

/* #define DEBUG */ 
#ifdef DEBUG
#define DPRINTF(x)    printf x  
#else
#define DPRINTF(x)
#endif


/* Clients and server communicate by passing request and response 
 * character strings of fixed formats. Request strings start with a 
 * digit character representing the transaction id which determine
 * the format of the remaining string. The first charactor in a
 * reposonse string is a digit representing success or failure for
 * the request. If success is true, the remaining response string
 * format is determined by the transaction type that they are in
 * response to.
 *
 * Request string formats
 *
 *                   trans_id  account  passwd  amount
 *                   interger  integer  integer integer
 * OPEN_ACCT_TRANS   X
 * DEPOSIT_TRANS     X         X        X       X 
 * WITHDRAW_TRANS    X         X        X       X
 * BALANCE_TRANS     X         X        X 
 * SHUTDOWN          X                  X
 *
 *
 * Response string formats
 *
 *                   success   account  passwd  amount  err_msg
 *                   interger  integer  integer integer char [ERR_MSG_SIZE]
 * OPEN_ACCT_TRANS   X         X        X               X
 * DEPOSIT_TRANS     X                                  X
 * WITHDRAW_TRANS    X                                  X
 * BALANCE_TRANS     X                          X       X 
 * SHUTDOWN          X                                  X
 *
 */


#define OPEN_ACCT_TRANS             1
#define DEPOSIT_TRANS               2
#define WITHDRAW_TRANS              3
#define BALANCE_TRANS               4
#define SHUTDOWN                    5

#define TRANS_SUCCESS               1
#define TRANS_FAILURE               0

#define ERR_MSG_SIZE               20
#define NUM_ERR_MSG                 6

#define ERR_MSG_NO_ACCOUNTS        "No_More_Accounts"
#define ERR_MSG_DATA_UNAVAIL       "Database_Unavail"
#define ERR_MSG_BAD_ACCOUNT        "Bad_Account_ID"
#define ERR_MSG_BAD_PASSWORD       "Bad_Password"
#define ERR_MSG_INS_FUNDS          "Insufficient_Funds"
#define ERR_MSG_BAD_TRANSID        "Bad_Transaction_ID"

#define ERR_NUM_SIZE               -1
#define ERR_NUM_NO_ACCOUNTS        -2
#define ERR_NUM_DATA_UNAVAIL       -3
#define ERR_NUM_BAD_ACCOUNT        -4
#define ERR_NUM_BAD_PASSWORD       -5
#define ERR_NUM_INS_FUNDS          -6
#define ERR_NUM_BAD_TRANSID        -7









