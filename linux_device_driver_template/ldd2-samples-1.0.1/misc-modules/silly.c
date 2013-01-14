/*
 * silly.c -- Simple Tool for Unloading and Printing ISA Data
 *
 * Copyright (C) 2001 Alessandro Rubini and Jonathan Corbet
 * Copyright (C) 2001 O'Reilly & Associates
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.  The citation
 * should list that the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates.   No warranty is attached;
 * we cannot take responsibility for errors or fitness for use.
 *
 * $Id: silly.c,v 1.17 2001/07/18 22:28:16 rubini Exp $
 */

/* =========================> BIG FAT WARNING:
 * This will only work on architectures with an ISA memory range.
 * It won't work on other computers.
 */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/module.h>
#include <linux/version.h>

#include <linux/sched.h>
#include <linux/kernel.h> /* printk() */
#include <linux/fs.h>     /* everything... */
#include <linux/errno.h>  /* error codes */
#include <linux/tqueue.h>
#include <linux/malloc.h>
#include <linux/mm.h>
#include <linux/ioport.h>

#include <asm/io.h>

#include "sysdep.h"  /* linux 2.0, 2.2, 2.4 compatibility macros */

int silly_major = 0;
MODULE_PARM(silly_major, "i");

MODULE_AUTHOR("Alessandro Rubini");

/*
 * The devices access the 640k-1M memory.
 * minor 0 uses readb/writeb
 * minor 1 uses readw/writew
 * minor 2 uses readl/writel
 * minor 3 uses memcpy_fromio()/memcpy_toio()
 */

/*
 * Here's our address range, and a place to store the ioremap'd base.
 */
#define ISA_BASE    0xA0000
#define ISA_MAX    0x100000  /* for general memory access */

#define VIDEO_MAX   0xC0000  /* for vga access */
#define VGA_BASE    0xb8000
static void *io_base;



int silly_open(struct inode *inode, struct file *filp)
{
    MOD_INC_USE_COUNT;
    return 0;
}

#ifdef LINUX_20
void silly_release(struct inode *inode, struct file *filp)
{
    MOD_DEC_USE_COUNT;
}
#else
int silly_release(struct inode *inode, struct file *filp)
{
    MOD_DEC_USE_COUNT;
    return 0;
}
#endif /* LINUX_20 */

enum silly_modes {M_8=0, M_16, M_32, M_memcpy};

ssize_t silly_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    int retval;
    int mode = MINOR(INODE_FROM_F(filp)->i_rdev);
    void *add;
    unsigned long isa_addr = ISA_BASE + *f_pos;
    unsigned char *kbuf, *ptr;


    if (isa_addr + count > ISA_MAX) /* range: 0xA0000-0x100000 */
	count = ISA_MAX - isa_addr;

    /*
     * too big an f_pos (caused by a malicious lseek())
     * would result in a negative count
     */
    if (count < 0) return 0;

    kbuf = kmalloc(count, GFP_KERNEL);
    if (!kbuf) return -ENOMEM;
    ptr=kbuf;
    retval=count;
   /*
    * Convert our address into our remapped area.
    */
    add = io_base + (isa_addr - ISA_BASE);
    /*
     * kbuf is aligned, but the reads might not. In order not to
     * drive me mad with unaligned leading and trailing bytes,
     * I downgrade the `mode' if unaligned xfers are requested.
     */

    if (mode==M_32 && ((isa_addr | count) & 3))
        mode = M_16;
    if (mode==M_16 && ((isa_addr | count) & 1))
        mode = M_8;

    switch(mode) {
      case M_32: 
        while (count >= 4) {
            *(u32 *)ptr = readl(add);
            add+=4; count-=4; ptr+=4;
        }
        break;
            
      case M_16: 
        while (count >= 2) {
            *(u16 *)ptr = readw(add);
            add+=2; count-=2; ptr+=2;
        }
        break;
            
      case M_8: 
        while (count) {
            *ptr = readb(add);
            add++; count--; ptr++;
        }
        break;

      case M_memcpy:
        memcpy_fromio(ptr, add, count);
        break;

      default:
        return -EINVAL;
    }
    if ( (retval > 0) && copy_to_user(buf, kbuf, retval))
	retval = -EFAULT;
    kfree(kbuf);
    *f_pos += retval;
    return retval;
}


ssize_t silly_write(struct file *filp, const char *buf, size_t count,
		    loff_t *f_pos)
{
    int retval;
    int mode = MINOR(INODE_FROM_F(filp)->i_rdev);
    unsigned long isa_addr = ISA_BASE + *f_pos;
    unsigned char *kbuf, *ptr;
    void *add;

    /*
     * Writing is dangerous.
     * Allow root-only, independently of device permissions
     */
    if (!capable (CAP_SYS_RAWIO)) return -EPERM;

    if (isa_addr + count > ISA_MAX) /* range: 0xA0000-0x100000 */
	count = ISA_MAX - isa_addr;

    /*
     * too big an f_pos (caused by a malicious lseek())
     * results in a negative count
     */
    if (count < 0) return 0;

    kbuf = kmalloc(count, GFP_KERNEL);
    if (!kbuf) return -ENOMEM;
    ptr=kbuf;
    retval=count;

    /*
     * kbuf is aligned, but the writes might not. In order not to
     * drive me mad with unaligned leading and trailing bytes,
     * I downgrade the `mode' if unaligned xfers are requested.
     */

    if (mode==M_32 && ((isa_addr | count) & 3))
        mode = M_16;
    if (mode==M_16 && ((isa_addr | count) & 1))
        mode = M_8;

    if (copy_from_user(kbuf, buf, count)) {
	kfree(kbuf); return -EFAULT;
    }
    ptr=kbuf;

    /*
     * Switch over to our remapped address space.
     */
    add = io_base + (isa_addr - ISA_BASE);

    switch(mode) {
      case M_32: 
        while (count >= 4) {
            writel(*(u32 *)ptr, add);
            add+=4; count-=4; ptr+=4;
        }
        break;
            
      case M_16: 
        while (count >= 2) {
            writew(*(u16 *)ptr, add);
            add+=2; count-=2; ptr+=2;
        }
        break;
            
      case M_8: 
        while (count) {
            writeb(*ptr, add);
            add++; count--; ptr++;
        }
        break;

      case M_memcpy:
        memcpy_toio(add, ptr, count);
        break;

      default:
        return -EINVAL;
    }
    *f_pos += retval;
    kfree(kbuf);
    return retval;
}


#ifdef __USE_OLD_SELECT__
int silly_poll(struct inode *inode, struct file *filp,
                  int mode, select_table *table)
{
    return mode==SEL_EX ? 0 : 1; /* readable, writable, not-exceptionable */
}
#define poll select /* to use as the method field in the fops structure */

#else

unsigned int silly_poll(struct file *filp, poll_table *wait)
{
    return POLLIN | POLLRDNORM | POLLOUT | POLLWRNORM;
}
#endif /* __USE_OLD_SELECT__ */

/*
 * Done, now wrappers for 2.0 and the fops structure
 */

#ifdef LINUX_20

int silly_read_20(struct inode *ino, struct file *f, char *buf, int count)
{
    return (int)silly_read(f, buf, count, &f->f_pos);
}

int silly_write_20(struct inode *ino, struct file *f, const char *b, int c)
{
    return (int)silly_write(f, b, c, &f->f_pos);
}

void silly_release_20(struct inode *ino, struct file *f)
{
    silly_release(ino, f);
}

#define silly_read silly_read_20
#define silly_write silly_write_20
#define silly_release silly_release_20
#define poll select

#endif

struct file_operations silly_fops = {
    read:     silly_read,
    write:    silly_write,
    poll:     silly_poll,
    open:     silly_open,
    release:  silly_release,
};

int silly_init(void)
{
    int result = register_chrdev(silly_major, "silly", &silly_fops);
    if (result < 0) {
        printk(KERN_INFO "silly: can't get major number\n");
        return result;
    }
    if (silly_major == 0)
            silly_major = result; /* dynamic */
    SET_MODULE_OWNER(&silly_fops);
/*
 * Set up our I/O range.
 */

    /* this line appears in silly_init */
    io_base = ioremap(ISA_BASE, ISA_MAX - ISA_BASE);
    return 0;
}

void silly_cleanup(void)
{
    iounmap(io_base);
    unregister_chrdev(silly_major, "silly");
}


module_init(silly_init);
module_exit(silly_cleanup);
