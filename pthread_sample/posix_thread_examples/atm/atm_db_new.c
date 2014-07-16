/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * atm_db_new.c
 *
 * Database Module for the ATM example program.
 *
 * This Version adds a proper free-list for the account database file
 *
 */

/*
 * Accounts are long lived objects that can be accessed in the following
 * ways;
 *
 *   new_account()      Provides a pointer to a account_t representing 
 *                      a new account with a unique id.
 *   retrieve_account() Provides a pointer to a account_t representing
 *                      an active account.
 *   store_account()    Takes an account_t pointer and writes it out
 *                      to perment storage then frees the account pointer.
 *   return_account()   Takes an account pointer that the caller does not
 *                      to use anymore and frees it. (Does not change
 *                      the account image on perment store.)
 *   delete_account()   Takes a pointer to an account and
 *                      resets the account image in perment store, frees 
 *                      the acocunt pointer and reclaims the id for 
 *                      reuse.
 *
 * A new_ or a retrieve_ is expected to be matched by a return_, store_,
 * or delete_
 */

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

#include "a.h"
#include "adb.h"

#define ATM_DB_FILE     "./.atm_db"      /* File that contains database. */
#define PAD             8192             /* Multiple of where to put
					    each account in the file.
					    An artificial way to simulate
					    a slower backing store by
					    causing more page faults
					    on access. */
static int increased_io = 0;
static int increased_cpu = 0;

static account_t blank_account = {  0,   /* id */
				    0,   /* password */
				    0,   /* balance */
				    0};  /* inuse */

                                         /* Instance of an empty account
					    used to initially fill 
					    database file and overwrite
					    freed accounts. */

static char inuse_mark = 1;
static char free_mark = 0;

static int accounts_start = 0; 

void format_account_db(void);
void check_account_db(void);

/************************************************************************** 
 * Initialize the database,
 * If create, create new file and populate with blank accounts.
 * Otherwise look for an existing database file. If initialization
 * fails return a ERR_NUM_DATA_UNAVAIL.
 ***************************************************************************/
void init_account_db(int force_create, int io, int cpu) 
{
  extern int increased_io, increased_cpu;

  /* loading factors for performance testing */
  increased_io = io;
  increased_cpu = cpu;
 
  /* where is first account given free list size and account padding */
  if ((MAX_NUM_ACCOUNTS % PAD) == 0) 
     accounts_start = (MAX_NUM_ACCOUNTS / PAD) * PAD;
  else
     accounts_start = ((MAX_NUM_ACCOUNTS / PAD) + 1) * PAD; 

  /* Force new database if requested. */
  if (force_create) {
	format_account_db();
  } else {
	check_account_db();
  }
}

/************************************************************************** 
 * Return a new account for use.
 * id and inuse fields should be set.
 * If no more accounts return ERR_NUM_NO_ACCOUNTS, if an error accessing
 * the database return ERR_NUM_SRV_DATA_UNAVAIL.
 ***************************************************************************/
int new_account(int *idp, account_t **raccountpp) {
  int db_fd, i, j, rtn, account_num = -1;
  off_t pos;
  size_t amt;
  account_t *accountp;
  char inuse = free_mark;
  char rbuf[256];

  bzero(rbuf, 256);

  if ((db_fd = open(ATM_DB_FILE, O_RDWR)) < 0)  {
    free(accountp);
    return (ERR_NUM_DATA_UNAVAIL);
  }
 
  /* scan free list for free account */
 
  for (i = 0; i < MAX_NUM_ACCOUNTS; i++) {
      if ((i%256) == 0) {
         if ((pos = lseek(db_fd, (off_t)i, SEEK_SET)) < 0) { 
	   perror("Seeking db file"), close(db_fd);
           return (ERR_NUM_DATA_UNAVAIL);
         }
      
         if ((amt = read(db_fd, (char *)rbuf, 256)) < 0 ) { 
           perror("Reading db file"), close(db_fd);
           return (ERR_NUM_DATA_UNAVAIL);
         }
         for (j = 0; j < 256; j++) {
           DPRINTF(("new_account: position %d, inuse %d\n",i+j,rbuf[j]));
	   if (rbuf[j] == free_mark) { 
	     DPRINTF(("new_account: account %d not in use\n",i));
	     break;
           }
         }
         if (rbuf[j] == free_mark) break; 
       }
  }

  account_num = i+j;

  if (account_num == MAX_NUM_ACCOUNTS) {
    close(db_fd);
    return (ERR_NUM_NO_ACCOUNTS);
  }

  /* set values in structure */
  accountp = (account_t *)malloc(256);
  bzero((char *)accountp, 256);
  accountp->id = account_num;
  accountp->inuse = 1;

  /* write initialized account */
  DPRINTF(("new_account: new account values %d, %d\n", accountp->id, accountp->inuse));
  DPRINTF(("new_account: storing new account at %d\n",(accounts_start + PAD*i)));

  if ((pos = lseek(db_fd, (off_t)(accounts_start + PAD*account_num), SEEK_SET)) < 0) {
    perror("Lseeking db file"), free(accountp), close(db_fd);
    return (ERR_NUM_DATA_UNAVAIL);
  }

  if ((amt = write(db_fd, (char *)accountp, 256)) < 0) { 
    perror("Writing db file"), free(accountp), close(db_fd);
    return (ERR_NUM_DATA_UNAVAIL);
  } 

  /* set free list value */
  DPRINTF(("new_account: setting free list position %d\n",i));

  if ((pos = lseek(db_fd, (off_t)i, SEEK_SET)) < 0) {  
    perror("Lseeking db file"), free(accountp), close(db_fd);
    return (ERR_NUM_DATA_UNAVAIL);
  }

  rbuf[j] = inuse_mark; 

  if ((amt = write(db_fd, (char *)rbuf, 256)) < 0) { 
    perror("Writing db file"), free(accountp), close(db_fd);
    return (ERR_NUM_DATA_UNAVAIL);
  }
 
  if ((rtn = fsync(db_fd)) < 0 ) {  
    perror("Sync'ing db file"), free(accountp), close(db_fd);
    return (ERR_NUM_DATA_UNAVAIL);
  }  

  /* set return values */
  *raccountpp = accountp;
  *idp = account_num;

  if ((rtn = close(db_fd))  < 0) {
    perror("Closing db file"), free(accountp), close(db_fd);
    return (ERR_NUM_DATA_UNAVAIL);
  }

  return (1);
}

/************************************************************************** 
 * Retrieve an account from backing store.
 *
 * Watch for bad account id or problems accessing db file.
 ***************************************************************************/
int retrieve_account(int id, account_t **raccountpp) {
  int db_fd, rtn;
  off_t pos;
  size_t amt;
  account_t *accountp;

  if ((id < 0) || (id >= MAX_NUM_ACCOUNTS))
    return (ERR_NUM_BAD_ACCOUNT);

  accountp = (account_t *)malloc(sizeof(account_t));

  if ((db_fd = open(ATM_DB_FILE, O_RDWR)) < 0) {
    free(accountp);
    return (ERR_NUM_DATA_UNAVAIL);
  }

  if ((pos = lseek(db_fd, (off_t)(accounts_start + PAD*id), SEEK_SET)) < 0) {
    perror("Lseeking db file");
    free(accountp);
    close(db_fd);
    return (ERR_NUM_DATA_UNAVAIL);
  }

  if ((amt = read(db_fd, (char *)accountp, sizeof(account_t))) < 0) {
    perror("Reading db file");
    free(accountp);
    close(db_fd);
    return (ERR_NUM_DATA_UNAVAIL);
  }

  if ((accountp->inuse != 1) || (accountp->id != id)) {
    free(accountp);
    close(db_fd);
    return (ERR_NUM_BAD_ACCOUNT);
  }

  /* set return values */
  *raccountpp = accountp;

  if ((rtn = close(db_fd)) < 0) {
    perror("Closing db file");
    free(accountp);
    close(db_fd);
    return (ERR_NUM_DATA_UNAVAIL);
  }

  return (1);
}

/************************************************************************** 
 * Store an account back to perment storage.
 *
 * Also free the passed account pointer. Watch for a bad account id
 * and problems with db file.
 ***************************************************************************/
int store_account(account_t *accountp) 
{
  int db_fd, rtn, i, j;
  off_t pos;
  size_t amt;
  long count=0;
  extern int increased_io;

  
  if ((accountp->id < 0) || (accountp->id >= MAX_NUM_ACCOUNTS))
    return (ERR_NUM_BAD_ACCOUNT);
  
  if ((db_fd = open(ATM_DB_FILE, O_RDWR)) < 0)
    return (ERR_NUM_DATA_UNAVAIL);
  
  if ((pos = lseek(db_fd, (off_t)(accounts_start + PAD*(accountp->id)), SEEK_SET)) < 0) {
    perror("Lseeking db file");
    close(db_fd);
    return (ERR_NUM_DATA_UNAVAIL);
  }

  for (i=0; i < 1 + increased_io; i++) {

    if ((amt = write(db_fd, (char *)accountp, sizeof(account_t))) < 0) {
      perror("Writing db file");
      close(db_fd);
      return (ERR_NUM_DATA_UNAVAIL);
    }
    
    if ((rtn = fsync(db_fd)) < 0) {
      perror("Fsyncing db file");
      close(db_fd);
      return (ERR_NUM_DATA_UNAVAIL);
    }

  }

  for (i = 0; i < increased_cpu; i++) 
    for (j = 0; j < CPU_LOOP_SIZE; j++)
      count+=j;

  if ((rtn = close(db_fd)) < 0) {
    perror("Closing db file");
    close(db_fd);
    return (ERR_NUM_DATA_UNAVAIL);
  }

  free(accountp);
  return (1);

}

/************************************************************************** 
 * Clean-up an account pointer no longer wanted.
 *
 * See store_account() to write account data out to perment store.
 ***************************************************************************/
int return_account(account_t *accountp)
{
  free(accountp);

  return (1);
}
/************************************************************************** 
 * Delete an active account.
 *
 * Reset the account image in perment store and free the account pointer.
 * Watch for bad account id and problems with db file.
 ***************************************************************************/
int delete_account(account_t *accountp)
{
  int db_fd, rtn;
  off_t pos;
  size_t amt;
  
  if ((accountp->id < 0) || (accountp->id >= MAX_NUM_ACCOUNTS))
    return (ERR_NUM_BAD_ACCOUNT);
  
  if ((db_fd = open(ATM_DB_FILE, O_RDWR)) < 0)
    return (ERR_NUM_DATA_UNAVAIL);
  
  if ((pos = lseek(db_fd, (off_t)(accounts_start + PAD*(accountp->id)), SEEK_SET)) < 0) {
    perror("Lseeking db file");
    close(db_fd);
    return (ERR_NUM_DATA_UNAVAIL);
  }
  if ((amt = write(db_fd, (char *)&blank_account, sizeof(account_t))) < 0) {
    perror("Writing db file");
    close(db_fd);
    return (ERR_NUM_DATA_UNAVAIL);
  }
  
  if ((rtn = fsync(db_fd)) < 0) {
    perror("Fsyncing db file");
    close(db_fd);
    return (ERR_NUM_DATA_UNAVAIL);
  }
  
  if ((rtn = close(db_fd)) < 0) {
    perror("Closing db file");
    return (ERR_NUM_DATA_UNAVAIL);
  }

  free(accountp);
  return (1);
}
void format_account_db(void)
{
  int i, db_fd, rtn;
  off_t pos;
  size_t amt;

  DPRINTF(("init_account_db: creating new data base file,"));
  DPRINTF(("init_account_db: num_accounts %d, accounts_start %d\n",MAX_NUM_ACCOUNTS, accounts_start));

  if ((db_fd = open(ATM_DB_FILE, O_RDWR)) < 0) {
   if ((db_fd = open(ATM_DB_FILE, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) < 0)
        perror("Creating db file"), exit(1);
  }

  /* initialize free list */
  for (i = 0; i < MAX_NUM_ACCOUNTS; i++) {
     if ((pos = lseek(db_fd, (off_t)i, SEEK_SET)) < 0)
        perror("Seeking db file"), close(db_fd), exit(1);
     if ((amt = write(db_fd, (char *)&free_mark, sizeof(char))) < 0)
        perror("Writing to db file"), close(db_fd), exit(1);
  }

  /* Fill in with blank accounts */
  for (i = 0; i < MAX_NUM_ACCOUNTS; i++) {
     if ((pos = lseek(db_fd, (off_t)(accounts_start + PAD*i), SEEK_SET)) < 0)
        perror("Lseeking in db file"), close(db_fd), exit (1);
     if ((amt = write(db_fd, (char *)&blank_account, sizeof(account_t))) < 0)
        perror("Writing to db file"), close(db_fd), exit(1);
  }

  if ((rtn = close(db_fd)) < 0 )
    perror("Closing db file"), exit (1);

}
void check_account_db(void)
{
  int i, db_fd, rtn;
  off_t pos;
  size_t amt;
  account_t account = { -1, -1, -1, -1};
  int account_inuse_count = 0;
  int free_list_inuse_count = 0;
  char inuse = free_mark;

  if ((db_fd = open(ATM_DB_FILE, O_RDWR)) < 0)
      perror("Opening db file"), exit(1);

  /* count inuse accounts in free list */
  for (i = 0; i < MAX_NUM_ACCOUNTS; i++) {
      inuse = free_mark;
      if ((pos = lseek(db_fd, (off_t)i, SEEK_SET)) < 0)
          perror("seeking in db file"), exit(1);

      if ((amt = read(db_fd, (char *)&inuse, sizeof(char))) < 0)
          perror("reading db file"), exit(1);

      if (inuse != free_mark) {
         free_list_inuse_count++;
         DPRINTF(("init_account_db: account %d inuse\n",i));
      }
  }
  DPRINTF(("init_account_db: using existing database file\n"));
  DPRINTF(("init_account_db: free list indicates %d inuse accounts\n",free_list_inuse_count));

  /* count accounts records that show inuse */
  for (i = 0; i < MAX_NUM_ACCOUNTS; i++) {
     account.inuse = -1;

     if ((pos = lseek(db_fd, (off_t)(accounts_start + PAD*i), SEEK_SET)) < 0)
        perror("Lseeking in db file"), close(db_fd), exit (1);

      if ((amt = read(db_fd, (char *)&account, sizeof(account_t))) < 0)
        perror("Reading to db file"), close(db_fd), exit (1);

      if (!((account.inuse == 0) || (account.inuse == 1)))
        fprintf(stderr, "Bad data in db file\n"), close(db_fd), exit (1);

      if (account.inuse == 1)
        account_inuse_count++;
  }

  DPRINTF(("init_account_db: accounts indicate %d are inuse\n", account_inuse_count));

  if (free_list_inuse_count != account_inuse_count)
        fprintf(stderr, "Inconsistancy in inuse count\n"), exit(1);

  if ((rtn = close(db_fd)) < 0 )
    perror("Closing db file"), exit (1);

}

