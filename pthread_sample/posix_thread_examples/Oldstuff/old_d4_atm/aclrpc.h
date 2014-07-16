/* aclrpc.h
 * 
 * Created by Brad Nichols, 10/18/93
 *
 * Include file for rpc style client call interface to atm server. 
 */

int accnt_open_rpc(int *, int *);
int accnt_deposit_rpc(int, int, int);
int accnt_withdraw_rpc(int, int, int);
int accnt_balance_rpc(int, int, int *);
int server_shutdown_rpc(int);

