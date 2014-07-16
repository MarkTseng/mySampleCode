/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * iotest.c
 */
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

#include "atm.h"
#include "atm_db.h"

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


void main(int argc, char **argv)
{
  int db_fd, i, rtn, write_count=0;
  size_t amt;
  off_t pos;
  account_t account = { -1, -1, -1, -1};

  if (argc >= 1) sscanf(argv[1], "%d", &write_count);


  if ((db_fd = open(ATM_DB_FILE, O_RDWR)) < 0) {
    /* Force create the file. */
    if ((db_fd = creat(ATM_DB_FILE, S_IRUSR | S_IWUSR)) < 0)  {
      perror("Creat of db file");
      exit(1);
    }
    
  }

  for (i = 0; i < write_count; i++) {
    
    if ((pos = lseek(db_fd, 0, SEEK_SET)) < 0) {
      perror("Lseeking in db file");
      close(db_fd);
      exit (1);
    }
    if ((amt = write(db_fd, (char *)&blank_account, sizeof(account_t))) < 0){
      perror("Writing to db file");
      close(db_fd);
      exit(1);
    }
    if ((rtn = fsync(db_fd)) < 0) {
      perror("Fsyncing db file");
      close(db_fd);
      exit(1);
    }
  }


  if ((rtn = close(db_fd)) < 0) {
    perror("Closing db file");
    exit (1);
  }

}
