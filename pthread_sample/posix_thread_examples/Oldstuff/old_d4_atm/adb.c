/* adb.c
 *
 * Created by Brad Nichols, 10/10/93
 *
 * Database Module for the ATM example program.
 *
 * Implementation of perment storge back end for example atm program.
 * Accounts are stored in a single file which is opened and close
 * with each access - Brain-dead I know, but the point (besides simplicity)
 * is two simulate the large io bound tasks "real" servers are bound to
 * see when accessing data on permenent storage or perhaps accross another
 * network link. 
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

/************************************************************************** 
 * Initialize the database,
 * If create, create new file and populate with blank accounts.
 * Otherwise look for an existing database file. If initialization
 * fails return a ERR_NUM_DATA_UNAVAIL.
 ***************************************************************************/
void init_account_db(int force_create, int io, int cpu) 
{
  int db_fd, i, rtn;
  size_t amt;
  off_t pos;
  account_t account = { -1, -1, -1, -1};

  extern int increased_io, increased_cpu;

  increased_io = io;
  increased_cpu = cpu;


  /* Force new database if requested. */
  if (force_create) unlink(ATM_DB_FILE);

  if ((db_fd = open(ATM_DB_FILE, O_RDWR)) < 0) {
    /* Force create the file. */
    if ((db_fd = creat(ATM_DB_FILE, S_IRUSR | S_IWUSR)) < 0)  {
      perror("Creat of db file");
      exit(1);
    }

    /* Fill in with blank accounts */
    for (i = 0; i < MAX_NUM_ACCOUNTS; i++) {

      if ((pos = lseek(db_fd, (off_t)PAD*i, SEEK_SET)) < -1) {
	perror("Lseeking in db file");
	close(db_fd);
	exit (1);
      }
      if ((amt = write(db_fd, (char *)&blank_account, sizeof(account_t))) < 0){
	perror("Writing to db file");
	close(db_fd);
	exit(1);
      }
    }
  } else {
    /* check for consistancy */ 
    for (i = 0; i < MAX_NUM_ACCOUNTS; i++) {

      if ((pos = lseek(db_fd, (off_t)PAD*i, SEEK_SET)) < -1) {
	perror("Lseeking in db file");
	close(db_fd);
	exit (1);
      }

      if ((amt = read(db_fd, (char *)&account, sizeof(account_t))) < 0) {
	perror("Reading to db file");
	close(db_fd);
	exit (1);
      }

      if (!((account.inuse == 0) || (account.inuse == 1)))  {
	fprintf(stderr, "Bad data in db file\n");
	close(db_fd);
	exit (1);
      }
    }
  }

  if ((rtn = close(db_fd)) < 0) {
    perror("Closing db file");
    exit (1);
  }

}

/************************************************************************** 
 * Return a new account for use.
 * id and inuse fields should be set.
 * If no more accounts return ERR_NUM_NO_ACCOUNTS, if an error accessing
 * the database return ERR_NUM_SRV_DATA_UNAVAIL.
 ***************************************************************************/
int new_account(int *idp, account_t **raccountpp) {
  int db_fd, i, rtn;
  off_t pos;
  size_t amt;
  account_t *accountp;

  accountp = (account_t *)malloc(sizeof(account_t));

  if ((db_fd = open(ATM_DB_FILE, O_RDWR)) < 0) {
    free(accountp);
    return (ERR_NUM_DATA_UNAVAIL);
  }
  for (i = 0; i < MAX_NUM_ACCOUNTS; i++) {
      if ((pos = lseek(db_fd, (off_t)PAD*i, SEEK_SET)) < -1) {
	perror("Lseeking db file"), free(accountp);
	close(db_fd);
	return (ERR_NUM_DATA_UNAVAIL);
      }
      if ((amt = read(db_fd, (char *)accountp, sizeof(account_t))) < 0) {
	perror("Reading db file");
	free(accountp);
	close(db_fd);
	return (ERR_NUM_DATA_UNAVAIL);
      }
      if (accountp->inuse == 0)
	break;
    }
  if (i == MAX_NUM_ACCOUNTS) {
    free(accountp);
    close(db_fd);
    return (ERR_NUM_NO_ACCOUNTS);
  }
 
  /* set values in structure */
  accountp->id = i;
  accountp->inuse = 1;
 
  if ((amt = write(db_fd, (char *)accountp, sizeof(account_t))) < 0) {
    perror("Writing db file");
    free(accountp);
    close(db_fd);
    return (ERR_NUM_DATA_UNAVAIL);
  }

  if ((rtn = fsync(db_fd)) < 0) {
    perror("Sync'ing db file");
    free(accountp);
    close(db_fd);
    return (ERR_NUM_DATA_UNAVAIL);
  }

  /* set return values */
  *raccountpp = accountp;
  *idp = i;

  if ((rtn = close(db_fd)) < 0) {
    perror("Closing db file");
    free(accountp);
    close(db_fd);
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

  if ((pos = lseek(db_fd, (off_t)PAD*id, SEEK_SET)) < -1) {
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
  
  if ((pos = lseek(db_fd, (off_t)PAD*(accountp->id), SEEK_SET)) < -1) {
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
  
  if ((pos = lseek(db_fd, (off_t)PAD*(accountp->id), SEEK_SET)) < -1) {
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

