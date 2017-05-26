
#include <stdio.h>              /* for vsprintf */
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "mguard.h"
#include <time.h>
#include "string.h"

static  int outfile_fd = -1;        /* output file descriptor */
static char  log_file_path[1024]={0};
/* the following are here to reduce stack overhead */
static  char    message_str[1024];      /* message string buffer */
extern char *mguard_env_logpath;

void dmalloc_message(const char *format, ...);


static  void    build_logfile_path(int sid, char *timestamp_str,char *buf, const int buf_len);

/*
 * Local ascii to unsigned long function
 */
unsigned long   loc_atoul(const char *str)
{
  const char    *str_p;
  unsigned long result = 0;

  /* skip opening white space */
  for (str_p = str; *str_p == ' ' || *str_p == '\t'; str_p++) {
  }

  /* now add up all digits */
  for (; *str_p >= '0' && *str_p <= '9'; str_p++) {
    result = result * (unsigned long)10 + (unsigned long)(*str_p - '0');
  }

  return result;
}

/*
 * Hexadecimal STR to int translation
 */
long    hex_to_long(const char *str)
{
  long      ret;

  /* strip off spaces */
  for (; *str == ' ' || *str == '\t'; str++) {
  }

  /* skip a leading 0[xX] */
  if (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X')) {
    str += 2;
  }

  for (ret = 0;; str++) {
    if (*str >= '0' && *str <= '9') {
      ret = ret * 16 + (*str - '0');
    }
    else if (*str >= 'a' && *str <= 'f') {
      ret = ret * 16 + (*str - 'a' + 10);
    }
    else if (*str >= 'A' && *str <= 'F') {
      ret = ret * 16 + (*str - 'A' + 10);
    }
    else {
      break;
    }
  }

  return ret;
}


/*
 * Local vsnprintf which handles the buffer-size or not.  Returns the
 * number of characters copied into BUF.
 */
static int loc_vsnprintf(char *buf, const int buf_size, const char *format, va_list args)
{
  char  *buf_p;
  (void)vsnprintf(buf, buf_size, format, args);
  /* now find the end of the buffer */
  for (buf_p = buf; *buf_p != '\0'; buf_p++) {
  }

  return buf_p - buf;
}

/*
 * Local snprintf which handles the buf-size not.  Returns the number
 * of characters copied into BUF.
 */
static int loc_snprintf(char *buf, const int buf_size, const char *format, ...)
{
  va_list   args;
  int       len;

  va_start(args, format);
  len = loc_vsnprintf(buf, buf_size, format, args);
  va_end(args);

  return len;
}


void mguard_open_log(int sid,char *timestamp_str)
{

//  int   len;

  /* if it's already open or if we don't have a log file configured */
  if (outfile_fd >= 0 || mguard_env_logpath == NULL) {
    return;
  }

  build_logfile_path(sid,timestamp_str,log_file_path, sizeof(log_file_path));
  /* open our logfile */
  outfile_fd = open(log_file_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if (outfile_fd < 0) {
//    /* NOTE: we can't use dmalloc_message of course so do it the hardway */
//    len = loc_snprintf(error_str, sizeof(error_str),
//               "debug-malloc library: could not open '%s'\r\n",
//               log_file_path);
//    (void)write(STDERR, error_str, len);
//    /* disable log_file_path */
//    mguard_env_logpath = NULL;
      printf("[MGUARD]\"%s\" log file opend failed!!\n",log_file_path);
    return;
  }
  else
  {
      printf("[MGUARD] log file opened!! %s\n",log_file_path);
  }

  /*
   * NOTE: this makes it go recursive here, but it will never enter
   * this section of code.
   */

  dmalloc_message("MGUARD version '%s'\n\n",MGUARD_VER);
//  dmalloc_message("flags = %#x, logfile '%s'", _dmalloc_flags, log_file_path);
//  dmalloc_message("interval = %lu, addr = %#lx, seen # = %ld, limit = %ld",
//          _dmalloc_check_interval, (unsigned long)_dmalloc_address,
//          _dmalloc_address_seen_n, _dmalloc_memory_limit);
#if LOCK_THREADS
  dmalloc_message("threads enabled, lock-on = %d, lock-init = %d",
          _dmalloc_lock_on, THREAD_INIT_LOCK);
#endif

#if LOG_PNT_TIMEVAL
  {
    char    time_buf[64];
    dmalloc_message("starting time = %s",
             _dmalloc_ptimeval(&_dmalloc_start, time_buf,
                       sizeof(time_buf), 0));
  }
#else
#if HAVE_TIME /* NOT LOG_PNT_TIME */
  {
    char    time_buf[64];
    dmalloc_message("starting time = %s",
             _dmalloc_ptime(&_dmalloc_start, time_buf,
                    sizeof(time_buf), 0));
  }
#endif
#endif

#if HAVE_GETPID
  {
    /* we make it long in case it's big and we hope it will promote if not */
    long    our_pid = getpid();

    dmalloc_message("process pid = %ld", our_pid);
  }
#endif
}


void mguard_close_log(void)
{
    if(outfile_fd>0)
    {
        close(outfile_fd);
        outfile_fd=-1;
        printf("[MGUARD] log file closed!! %s\n",log_file_path);
        memset(log_file_path,0,sizeof(log_file_path));
    }

}

/*
 * void _dmalloc_vmessage
 *
 * DESCRIPTION:
 *
 * Message writer with vprintf like arguments which adds a line to the
 * dmalloc logfile.
 *
 * RETURNS:
 *
 * None.
 *
 * ARGUMENTS:
 *
 * format -> Printf-style format statement.
 *
 * args -> Already converted pointer to a stdarg list.
 */
static void    _dmalloc_vmessage(const char *format, int single_line ,va_list args)
{
  char  *str_p, *bounds_p;
  int   len;

  str_p = message_str;
  bounds_p = str_p + sizeof(message_str);

  /* no logpath and no print then no workie */
  if (mguard_env_logpath == NULL) {
    return;
  }

//#if HAVE_GETPID && LOG_REOPEN
//  if (dmalloc_logpath != NULL) {
//    char    *log_p;
//
//    /*
//     * This static pid will be checked to make sure it doesn't change.
//     * We make it long in case it's big and we hope it will promote if
//     * not.
//     */
//    static long     current_pid = -1;
//    long        new_pid;
//
//    new_pid = getpid();
//    if (new_pid != current_pid) {
//      /* NOTE: we need to do this _before_ the reopen otherwise we recurse */
//      current_pid = new_pid;
//
//      /* if the new pid doesn't match the old one then reopen it */
//      if (current_pid >= 0) {
//
//    /* this only works if there is a %p in the logpath */
//    for (log_p = dmalloc_logpath; *log_p != '\0'; log_p++) {
//      if (*log_p == '%' && *(log_p + 1) == 'p') {
//        _dmalloc_reopen_log();
//        break;
//      }
//    }
//      }
//    }
//  }
//#endif

//  /* do we need to open the logfile? */
//  if (dmalloc_logpath != NULL && outfile_fd < 0) {
//    _dmalloc_open_log();
//  }

#if HAVE_TIME
#if LOG_TIME_NUMBER
  {
    long    now;
    now = time(NULL);
    str_p += loc_snprintf(str_p, bounds_p - str_p, "%ld: ", now);
  }
#endif /* LOG_TIME_NUMBER */
#if HAVE_CTIME
#if LOG_CTIME_STRING
  {
    TIME_TYPE   now;
    now = time(NULL);
    str_p += loc_snprintf(str_p, bounds_p - str_p, "%.24s: ", ctime(&now));
  }
#endif /* LOG_CTIME_STRING */
#endif /* HAVE_CTIME */
#endif /* HAVE_TIME */

#if LOG_ITERATION
  /* add the iteration number */
  str_p += loc_snprintf(str_p, bounds_p - str_p, "%lu: ", _dmalloc_iter_c);
#endif
#if LOG_PID && HAVE_GETPID
  {
    /* we make it long in case it's big and we hope it will promote if not */
    long    our_pid = getpid();

    /* add the pid to the log file */
    str_p += loc_snprintf(str_p, bounds_p - str_p, "p%ld: ", our_pid);
  }
#endif

  /*
   * NOTE: the following code, as well as the function definition
   * above, would need to be altered to conform to non-ANSI-C
   * specifications if necessary.
   */

  /* write the format + info into str */
  len = loc_vsnprintf(str_p, bounds_p - str_p, format, args);

  /* was it an empty format? */
  if (len == 0) {
    return;
  }
  str_p += len;

  if(single_line)
  {
      /* tack on a '\n' if necessary */
      if (*(str_p - 1) != '\n') {
        *str_p++ = '\n';
        *str_p = '\0';
      }
  }
  len = str_p - message_str;

  /* do we need to write the message to the logfile */
  if (outfile_fd>0)
  {
    (void)write(outfile_fd, message_str, len);
  }

}

extern pid_t ourgetpid();
static  void    build_logfile_path(int sid,char *timestamp_str,char *buf, const int buf_len)
{
  char  *bounds_p;
  char  *path_p, *buf_p;//,*start_p;
  //int   len;

  if (mguard_env_logpath == NULL) {
    buf[0] = '\0';
    return;
  }

  buf_p = buf;
  bounds_p = buf + buf_len;

//  start_p = dmalloc_logpath;
  for (path_p = mguard_env_logpath; *path_p != '\0'; path_p++) {

    /* if we don't have to do anything special then just continue */
    if (*path_p != '%' || *(path_p + 1) == '\0') {
      if (buf_p < bounds_p) {
    *buf_p++ = *path_p;
      }
      continue;
    }

    /* skip over the % */
    path_p++;

    /* dump the hostname */
    if (*path_p == 'h') {
#if HAVE_GETHOSTNAME
      char  our_host[128];
      gethostname(our_host, sizeof(our_host));
      buf_p += loc_snprintf(buf_p, bounds_p - buf_p, "%s", our_host);
#else
      buf_p += loc_snprintf(buf_p, bounds_p - buf_p, "not-gethostname");
#endif
    }
    /* dump the thread-id */
    if (*path_p == 'i') {
#if LOG_PNT_THREAD_ID
      char      id_str[256];
      THREAD_TYPE   id;

      id = THREAD_GET_ID();
      THREAD_ID_TO_STRING(id_str, sizeof(id_str), id);
      buf_p += loc_snprintf(buf_p, bounds_p - buf_p, "%s", id_str);
#else
      buf_p += loc_snprintf(buf_p, bounds_p - buf_p, "no-thread-id");
#endif
    }
    /* dump the pid -- also support backwards compatibility with %d */
    if (*path_p == 'p') {
      buf_p += loc_snprintf(buf_p, bounds_p - buf_p, "%d",ourgetpid());
    }
    /* dump the time value */
    if (*path_p == 't') {
      buf_p += loc_snprintf(buf_p, bounds_p - buf_p, "%s",timestamp_str);
    }
    /* dump the user-id */
    if (*path_p == 'u') {
#if HAVE_GETUID
      /* we make it long in case it's big and we hope it will promote if not */
      long  our_uid = getuid();
      buf_p += loc_snprintf(buf_p, bounds_p - buf_p, "%ld", our_uid);
#else
      buf_p += loc_snprintf(buf_p, bounds_p - buf_p, "no-uid");
#endif
    }

    /*sequence ID*/
    if (*path_p == 's') {
          buf_p += loc_snprintf(buf_p, bounds_p - buf_p, "%ld", sid);
    }
  }

  if (buf_p >= bounds_p - 1) {
      printf("ERROR!! [%s]%d\n",__FILE__,__LINE__);
//    /* NOTE: we can't use dmalloc_message of course so do it the hard way */
//    len = loc_snprintf(error_str, sizeof(error_str),
//               "debug-malloc library: logfile path too large '%s'\r\n",
//               dmalloc_logpath);
//    (void)write(STDERR, error_str, len);
  }

  *buf_p = '\0';
}

void dmalloc_message(const char *format, ...)
  /* __attribute__ ((format (printf, 1, 2))) */
{
  va_list   args;

  va_start(args, format);
  _dmalloc_vmessage(format,1,args);
  va_end(args);
}

void dmalloc_message_concat(const char *format, ...)
  /* __attribute__ ((format (printf, 1, 2))) */
{
  va_list   args;

  va_start(args, format);
  _dmalloc_vmessage(format,0,args);
  va_end(args);
}

