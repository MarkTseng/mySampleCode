#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/syscalls.h>
#include <asm/unistd.h>
#include <asm/uaccess.h>

#define DEFAULT_LOGTO "/tmp/test.log"

char buf[128];
struct file *file = NULL;

static int __init init(void)
{
        mm_segment_t old_fs;

        if(file == NULL)
                file = filp_open(DEFAULT_LOGTO, O_RDWR | O_APPEND | O_CREAT, 0644);
        if (IS_ERR(file)) {
                printk("error occured while opening file %s, exiting...\n", DEFAULT_LOGTO);
                return 0;
        }

        sprintf(buf,"%s\n", "Write log from kernel by vfs_write");

        old_fs = get_fs();
        set_fs(KERNEL_DS);
        //file->f_op->write(file, (char *)buf, sizeof(buf), &file->f_pos);
		vfs_write(file, (char *)buf, sizeof(buf), &file->f_pos);
        vfs_fsync(file, 0);
		set_fs(old_fs);
        filp_close(file, NULL);  
        file = NULL;
        return 0;
}

static void __exit fini(void)
{
        if(file != NULL)
                filp_close(file, NULL);
}

module_init(init);
module_exit(fini);
MODULE_AUTHOR("Mark Tseng, mark.tseng@realtek.com");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("VFS debug msg to file");



