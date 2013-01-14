#include <linux/module.h>       /* Specifically, a module */
#include <linux/kernel.h>       /* We're doing kernel work */
#include <linux/slab.h>
#include <linux/proc_fs.h>      /* Necessary because we use the proc fs */
#include <asm/uaccess.h>        /* for copy_from_user */
#define PROCFS_MAX_SIZE         2048
#define PROCFS_NAME             "buffer2k"

MODULE_LICENSE("GPL");

static char procfs_buffer[PROCFS_MAX_SIZE];
static unsigned long procfs_buffer_size = 0;

static int buffer_read(char *buf, char **start, off_t offset, int len, int *eof, void *data)
{
	int ret;
	printk(KERN_INFO "procfile_read (/proc/%s) called\n", PROCFS_NAME);
	if (offset > 0) {
		/* we have finished to read, return 0 */
		ret  = 0;
	} else {
		/* fill the buffer, return the buffer size */
		memcpy(buf, procfs_buffer, procfs_buffer_size);
		ret = procfs_buffer_size;
	}
	return ret;

}

int init_module()
{

	create_proc_read_entry(PROCFS_NAME, 0, NULL, buffer_read, NULL);
    return 0;       /* everything is ok */
}

void cleanup_module()
{
	remove_proc_entry(PROCFS_NAME, NULL);	
}
