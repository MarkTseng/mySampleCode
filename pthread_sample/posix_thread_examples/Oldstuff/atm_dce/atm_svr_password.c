/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * atm_svr_password.c
 * 
 * Server password module for atm example program.
 * 
 * Simple implementation that stores and retrieves an integer password
 * from a file. Interface operations are to initialize the service,
 * Compare a password to the password stored and to reset the password.
 */

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

#define ATM_PASSWD_FILE      ".atm_pw"
/*
#define ATM_DEFAULT_PW       1
*/
static const int ATM_DEFAULT_PW;

/************************************************************************** 
 * Initialize server password.
 *
 * If a password file does not already exist one is created and a default
 * password is placed in it. If the create flag is set then an pre-existing
 * password file is always ignored.
 **************************************************************************/
void server_password_init(int create)
{
  int pw_fd;
  size_t amt;
  int rtn;
  char spw[10] = "         ";
 
  /* Force a reset of password to default if "create" */
  if (create) unlink(ATM_PASSWD_FILE);

  if ((pw_fd = open(ATM_PASSWD_FILE, O_RDWR)) < 0) {
    /* Create the file. */
    if ((pw_fd = creat(ATM_PASSWD_FILE, S_IRUSR | S_IWUSR)) < 0)  {
      perror("Creat of server password file");
      exit(1);
    }

    /* Insert default password */
    sprintf(spw, "%d", ATM_DEFAULT_PW);
    if ((amt = write(pw_fd, (char *)spw, 10*sizeof(char))) < 0) {
      perror("Writing to server password file");
      close(pw_fd);
      exit(1);
    }
  } else {
    if ((amt = read(pw_fd, (char *)spw, sizeof(char)*10)) != 10) {
      perror("Reading server password file");
      close(pw_fd);
      exit (1);
    }
  }

  if ((rtn = close(pw_fd)) < 0) {
    perror("Closing server password file");
    exit (1);
  }


}

/************************************************************************** 
 * Check a password against the stored password. 
 *
 * If the password does not match or the password file cannot be accessed
 * 0 is returned, else if a match a 1.
 **************************************************************************/
int  check_server_password(int cpassword)
{
  int pw_fd, rtn;
  size_t amt;
  int rpassword;
  char spw[10];

  if ((pw_fd = open(ATM_PASSWD_FILE, O_RDWR)) < 0) {
    perror("Opening server password file");
    return 0;
  }

  if ((amt = read(pw_fd, (char *)spw, sizeof(char)*10)) < 0) {
    perror("Reading server password file");
    close(pw_fd);
    return 0;
  }
  sscanf(spw, "%d", &rpassword);

  close(pw_fd);

  if (rpassword != cpassword) {
    return 0;
  } else {
    return 1;
  }
}

/************************************************************************** 
 * Reset the store password.
 *
 * Requires that the caller provides the current password as well.
 **************************************************************************/
int  set_server_password(int old_password, int new_password)
{
  int pw_fd, rtn;
  size_t amt;
  int rpassword;
  char spw[10];

  if ((pw_fd = open(ATM_PASSWD_FILE, O_RDWR)) < 0) {
    perror("Opening server password file");
    return 0;
  }

  if ((amt = read(pw_fd, (char *)spw, sizeof(char)*10)) < 0) {
    perror("Reading server password file");
    close(pw_fd);
    return 0;
  }
  sscanf(spw, "%d", &rpassword);

  if (rpassword != old_password) {
    close(pw_fd);
    return 0;
  } else {
    sprintf(spw, "%d", new_password);
    if ((amt = write(pw_fd, (char *)spw, 10*sizeof(char))) < 0) {
      perror("Writing to server password file");
      close(pw_fd);
      return 0;
    } else {
      close(pw_fd);
      return 1;
    }
  }
}


