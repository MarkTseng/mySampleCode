/*
 *	Sunplus IOMMAP driver on 2011/11/014
 *	Author: Mark Tseng
 */

#include <linux/module.h>

#if defined (CONFIG_SMP)
#define __SMP__
#endif

#if defined(CONFIG_MODVERSIONS)
#define MODVERSIONS
#include <linux/modversions.h>
#endif

#include <linux/kernel.h>
#include <linux/sched.h>	/* tasklet workqueue */
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>   /* printk() */
#include <linux/slab.h>     /* kmalloc() */
#include <linux/fs.h>       /* everything... */
#include <linux/errno.h>    /* error codes */
#include <linux/types.h>    /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>    /* O_ACCMODE */
#include <linux/aio.h>
#include <linux/ioctl.h>	/* ioctl */
#include <linux/cdev.h>		/* char device API */
#include <linux/device.h>	/* dev_name(),dev_get_drvdata,dev_set_drvdata  */
#include <linux/list.h>		/* link list*/
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/seq_file.h>
#include <linux/vmalloc.h>
#include <linux/platform_device.h> /* platform device structure*/
#include <linux/ioport.h> /* resource structure */


#include <asm/mach-spv9200/regmap_9200.h> /* SPV9200 CPU Register */
#include <asm/mach-spv9200/spv9200_memlayout.h> /* SPV9200 memory layout */
#include <asm/mach-spv9200/sizes.h> /* SZ_XX for resource structure */
#include <asm/cacheflush.h>
#include <asm/uaccess.h>
#include <asm/page.h>
#include <asm/io.h>	/* ioremap,ioremap_nocache, ioread32/iowrite32, request_mem_region, release_mem_region */

#include "spv9200_iommap.h"


/*
 * The phys start addr : base address in dram 
 * The phys end addr   : end addree in dram 
 */
static unsigned long phys_start_addr = 0x13000000;
static unsigned long phys_end_addr	 = 0x13ffffff;
module_param(phys_start_addr, ulong, S_IRUGO);
module_param(phys_end_addr, ulong, S_IRUGO);

IOMMAP_DEV *pIOMMAPDev;

static int iommap_mmap(struct file *file, struct vm_area_struct *vma)
{
	unsigned long size;

	printk("mmap: vma->vm_start     = 0x%lx\n", vma->vm_start);
    printk("mmap: vma->vm_pgoff     = 0x%lx\n", vma->vm_pgoff);
    printk("mmap: vma->vm_end       = 0x%lx\n", vma->vm_end);
    printk("mmap: size              = 0x%lx\n", vma->vm_end - vma->vm_start);
	printk("mmap: SDRAMB virtual    = 0x%lx\n", (unsigned long)phys_start_addr >> PAGE_SHIFT);
    printk("mmap: vm_page_prot      = 0x%lx\n", vma->vm_page_prot);
    printk("mmap: vm_page_prot nocache = 0x%lx\n", pgprot_noncached(vma->vm_page_prot));
    printk("mmap: vm_page_prot cache   = 0x%lx\n", __pgprot(pgprot_val(vma->vm_page_prot)));

	/* no such device */
	if (!pIOMMAPDev->virtual_mem_base)
		return -ENXIO;
	
	/* size must be a multiple of PAGE_SIZE */
	size = vma->vm_end - vma->vm_start;
	// NONCACHE
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	//vma->vm_page_prot = __pgprot(pgprot_val(vma->vm_page_prot));

    vma->vm_flags |= VM_RESERVED | VM_IO;

	// mmap : convert phys addr to virt addr in 3rd argument
    if (io_remap_pfn_range(vma, vma->vm_start, phys_start_addr >> PAGE_SHIFT,
                        vma->vm_end - vma->vm_start, vma->vm_page_prot)) {
        IOMMAP_DEBUGMSG("set_noncached: failed remap_pfn_range\n");
        return -EAGAIN;
    }

	return 0;
}

static long iommap_ioctl(struct file *filp,
		       unsigned int cmd, unsigned long arg)
{
	IOMMAP_DEV *idev = pIOMMAPDev;

	switch (cmd) {
		/* create the wanted device */
		case IOMMAP_SET: {
			/* if virtual_mem_base is set, device is in use */
			if (idev->virtual_mem_base)
				return -EBUSY;
			if (copy_from_user(idev, (IOMMAP_DEV *)arg, sizeof(IOMMAP_DEV)))
				return -EFAULT;
			/* virtual_mem_base and size must be page aligned */
			
			IOMMAP_DEBUGMSG("setting up minor %d\n", MINOR(filp->f_dentry->d_inode->i_rdev));
			return 0;
		}

		case IOMMAP_GET: {
			/* maybe device is not set up */
			if (!idev->virtual_mem_base)
				return -ENXIO;
			if (copy_to_user((IOMMAP_DEV *)arg, idev, sizeof(IOMMAP_DEV)))
				return -EFAULT;
			return 0;
		}
	}

	return -ENOTTY;
}

static ssize_t iommap_read(struct file *file, char *buf, size_t count,
			  loff_t *offset)
{
	return 0;
}

static ssize_t iommap_write(struct file *file, const char *buf, size_t count,
			   loff_t *offset)
{
	return 0;
}

static int iommap_open(struct inode *inode, struct file *file)
{

	IOMMAP_DEBUGMSG("virtual_mem_base : 0x%lx extending 0x%lx bytes\n", (unsigned long)pIOMMAPDev->virtual_mem_base, (unsigned long)pIOMMAPDev->virtual_size);
	return 0;
}

static int iommap_release(struct inode *inode, struct file *file)
{
	return 0;
}

static const struct file_operations iommap_fops = {
	.owner			= THIS_MODULE,
	.read			= iommap_read,
	.write			= iommap_write,
	.unlocked_ioctl = iommap_ioctl,
	.mmap			= iommap_mmap,
	.open			= iommap_open,
	.release		= iommap_release,
};


static int __init iommap_init(void)
{
	int ret = 0,i;
	struct resource *ioarea;

	IOMMAP_DEBUGMSG("IOMMAP initial up\n");

	/* print module param*/
	IOMMAP_DEBUGMSG("phys_start_addr: 0x%lx\n",(unsigned long)phys_start_addr);
	IOMMAP_DEBUGMSG("phys_end_addr  : 0x%lx\n",(unsigned long)phys_end_addr);

	/* register device with kernel */
	ret = register_chrdev(IOMMAP_MAJOR, IOMMAP_DEVICE_NAME, &iommap_fops);
	if (ret) {
		IOMMAP_DEBUGMSG("can't register device with kernel\n");
		return ret;
	}
	
	pIOMMAPDev = (IOMMAP_DEV *) kmalloc(sizeof(IOMMAP_DEV), GFP_KERNEL);
	

	pIOMMAPDev->virtual_size = (phys_end_addr - phys_start_addr) + 1;
	pIOMMAPDev->phys_addr = phys_start_addr;

	IOMMAP_DEBUGMSG("iommap start: 0x%llx, end:0x%llx\n",(unsigned long long)pIOMMAPDev->phys_addr,(unsigned long long)phys_end_addr);

    ioarea = request_mem_region(pIOMMAPDev->phys_addr,
            pIOMMAPDev->virtual_size, "DRAM_IOMMAP");
    if (!ioarea) {
        IOMMAP_DEBUGMSG("memory region already claimed\n");
        ret = -EBUSY;
        goto err_release_data;
    }

	pIOMMAPDev->virtual_mem_base = ioremap(pIOMMAPDev->phys_addr, pIOMMAPDev->virtual_size);
	if(pIOMMAPDev->virtual_mem_base == NULL)
	{
		goto err_release_region;
	}

	IOMMAP_DEBUGMSG("mapping success virtual_mem_base : 0x%lx extending 0x%lx bytes\n", (unsigned long)pIOMMAPDev->virtual_mem_base, (unsigned long)pIOMMAPDev->virtual_size);

	memset_io(pIOMMAPDev->virtual_mem_base, 0, pIOMMAPDev->virtual_size);

#if 1 /* test memory map */
	/* write */
	
	for(i=0;i<100;i++)
	{
		writeb(i,pIOMMAPDev->virtual_mem_base+i);
		IOMMAP_DEBUGMSG("virtual mem write : 0x%lx, value: 0x%02x\n", (unsigned long)pIOMMAPDev->virtual_mem_base+i, i);		
	}
	
	/* read */
	for(i=0;i<100;i++)
	{
		IOMMAP_DEBUGMSG("virtual mem read : 0x%lx, value: 0x%02x\n", (unsigned long)pIOMMAPDev->virtual_mem_base+i, readb(pIOMMAPDev->virtual_mem_base+i));		
	}
#endif

	return ret;


err_release_region:
    release_mem_region(pIOMMAPDev->phys_addr, pIOMMAPDev->virtual_size);
err_release_data:
    kfree(pIOMMAPDev);

	return ret;
}

static void __exit iommap_cleanup(void)
{
	
	IOMMAP_DEBUGMSG("unloaded\n");

	/* delete the devices */
	IOMMAP_DEBUGMSG("buffer at 0x%lx will be removed\n", (unsigned long)pIOMMAPDev->virtual_mem_base);
	iounmap(pIOMMAPDev->virtual_mem_base);

	/* release memory region*/
	release_mem_region(pIOMMAPDev->phys_addr, pIOMMAPDev->virtual_size);
	IOMMAP_DEBUGMSG("release_mem_region\n");

	unregister_chrdev(IOMMAP_MAJOR, IOMMAP_DEVICE_NAME);
	
	kfree(pIOMMAPDev);
	return;
}
	
module_init(iommap_init);
module_exit(iommap_cleanup);
MODULE_AUTHOR("Mark Tseng");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION(" IOMMAP driver");


