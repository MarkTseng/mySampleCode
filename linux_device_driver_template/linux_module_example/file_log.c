#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kthread.h> /* kthread_run, kthread_stop */
#include <linux/delay.h>
#include <linux/completion.h>
#include <linux/version.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <asm/uaccess.h>


#define DEFAULT_LOGTO  "/tmp/test.log"
char trace_file[128]       = DEFAULT_LOGTO;
static struct file *log_file;
static int log_file_error = 0;
unsigned long cycle_mb = 25;

static void log_write( char *fmt, int len)
{
  if ( len <= 0 || 0 == fmt[0] )
    return;

  if ( 0 != trace_file[0] ) {
    log_file = filp_open( trace_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUGO | S_IWUGO );
    if ( IS_ERR(log_file) ) {
      long error = PTR_ERR(log_file);
      log_file = NULL;
      pr_err(": failed to start log to '%s' (errno=%ld), using system log\n", trace_file, -error);
    }
  }

  if ( NULL != log_file && NULL != log_file->f_op && NULL != log_file->f_op->write && !log_file_error ) {

    mm_segment_t old_limit = get_fs();
    long error = 0;

    set_fs( KERNEL_DS );

    if ( 0 != cycle_mb ) {
      size_t bytes = cycle_mb << 20;
      int to_write = log_file->f_pos + len > bytes? (bytes - log_file->f_pos) : len;
      if ( to_write <= 0 )
        to_write = 0;
      else {
        error = log_file->f_op->write(log_file, fmt, to_write, &log_file->f_pos);
        if ( error < 0 )
          log_file_error = error;
        fmt += to_write;
        len -= to_write;
      }

      if ( 0 != len )
        log_file->f_pos = 0;
    }

    if ( 0 != len ) {
      error = log_file->f_op->write(log_file, fmt, len, &log_file->f_pos );
      if ( error < 0 )
        log_file_error = error;
    }

    set_fs( old_limit );

    if ( error < 0 )
      printk("log write failed: %ld\n", -error);
  }
  // Comment out this 'else' to duplicate the output to klog.
  else {
    pr_err(":%s", fmt );
  }
}

void log_close( void )
{
  if ( NULL != log_file ){
    filp_close( log_file, NULL );
    log_file = NULL;
	log_file_error = 0;
  }
}




static int __init log_init(void)
{
    int ret = 0;

	// test
	log_write( "*** trace_indent < 0\n", sizeof("*** trace_indent < 0\n")-1 );

    return ret;
	
}

static void __exit log_cleanup(void)
{
	log_close();
	return;
}
	
module_init(log_init);
module_exit(log_cleanup);
MODULE_AUTHOR("Mark Tseng, mark.tseng@realtek.com");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("VFS debug msg to file");


