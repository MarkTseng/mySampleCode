/*******************************************************************
 * 
 *  Copyright C 2012 by Amlogic, Inc. All Rights Reserved.
 *
 *  Description: 
 *
 *  Author: jian.xu 
 *  Created: 04/18 2012
 *
 * amlogic s/pdif output driver 
 *******************************************************************/
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>	
#include <linux/device.h>	
#include <linux/mm.h>
#include <mach/am_regs.h>

#include "spdif_module.h"


#define DEVICE_NAME    "audio_spdif"

static int device_opened = 0;
static int major_spdif;
static struct class *class_spdif;
static struct device *dev_spdif;
static struct mutex mutex_spdif;  
static  _iec958_data_info   iec958_info = {0};
extern void	aml_alsa_hw_reprepare();
extern unsigned int IEC958_mode_raw;
extern unsigned int IEC958_mode_codec;
static struct file_operations fops_spdif = {
    .read = audio_spdif_read,
    .ioctl = audio_spdif_ioctl,
    .write = audio_spdif_write,
    .open = audio_spdif_open,
    .mmap = audio_spdif_mmap,
    .release = audio_spdif_release
};

static inline int if_audio_output_iec958_enable(void)
{
	return READ_MPEG_REG_BITS(AIU_MEM_IEC958_CONTROL, 1, 2);	
}
static inline int if_audio_output_i2s_enable(void)
{
	return READ_MPEG_REG_BITS(AIU_MEM_I2S_CONTROL, 1, 2);
}
static inline void audio_output_iec958_enable(unsigned flag)
{
		if(flag){
	              WRITE_MPEG_REG(AIU_958_FORCE_LEFT, 0);
	              WRITE_MPEG_REG_BITS(AIU_958_DCU_FF_CTRL, 1, 0, 1);
	              WRITE_MPEG_REG_BITS(AIU_MEM_IEC958_CONTROL, 3, 1, 2);
		}
		else{
            		WRITE_MPEG_REG(AIU_958_DCU_FF_CTRL, 0);
            		WRITE_MPEG_REG_BITS(AIU_MEM_IEC958_CONTROL, 0, 1, 2);			
		}
}
static int audio_spdif_open(struct inode *inode, struct file *file)
{
	if (device_opened){
		printk("error,audio_spdif device busy\n");
		return -EBUSY;
	}	
	device_opened++;
	try_module_get(THIS_MODULE);
	return 0;
}
static int audio_spdif_release(struct inode *inode, struct file *file)
{
    device_opened--;		
    module_put(THIS_MODULE);
    return 0;
}
static int audio_spdif_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long args)
{
	int err = 0;
	unsigned long  *val = (unsigned long*)args;
	mutex_lock(&mutex_spdif);	
	switch(cmd){
		case AUDIO_SPDIF_GET_958_BUF_RD_OFFSET:
			*val = READ_MPEG_REG(AIU_MEM_IEC958_RD_PTR) -READ_MPEG_REG(AIU_MEM_IEC958_START_PTR);
			break;
		case AUDIO_SPDIF_GET_958_BUF_SIZE:
			*val = READ_MPEG_REG(AIU_MEM_IEC958_END_PTR) -READ_MPEG_REG(AIU_MEM_IEC958_START_PTR)+64;//iec958_info.iec958_buffer_size;
			break;
		case AUDIO_SPDIF_GET_958_ENABLE_STATUS:
			*val = if_audio_output_iec958_enable();
			break;	
		case AUDIO_SPDIF_GET_I2S_ENABLE_STATUS:
			*val = if_audio_output_i2s_enable();
			break;	
		case AUDIO_SPDIF_SET_958_ENABLE:
		//	IEC958_mode_raw = 1;
			IEC958_mode_codec = 2;
			audio_output_iec958_enable(args);
			break;
		case AUDIO_SPDIF_SET_958_INIT_PREPARE:
			aml_alsa_hw_reprepare();
			break;
		default:
			printk("audio spdif: cmd not implemented\n");
			break;
	}
	mutex_unlock(&mutex_spdif);
	return err;
}
static ssize_t audio_spdif_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
    printk(KERN_ALERT " audio spdif: write operation isn't supported.\n");
    return -EINVAL;
}
static ssize_t audio_spdif_read(struct file *filp,	
        char __user *buffer,	
        size_t length,	
        loff_t * offset)
{
    printk(KERN_ALERT "audio spdif: read operation isn't supported.\n");
    return -EINVAL;
}

static int audio_spdif_mmap(struct file *file, struct vm_area_struct *vma)
{
    unsigned long off = vma->vm_pgoff << PAGE_SHIFT;
    unsigned vm_size = vma->vm_end - vma->vm_start;
    if (vm_size == 0) {
        return -EAGAIN;
    }
    off += READ_MPEG_REG(AIU_MEM_IEC958_START_PTR);//mapping the 958 dma buffer to user space to write

    vma->vm_flags |= VM_RESERVED | VM_IO;

    if (remap_pfn_range(vma, vma->vm_start, off >> PAGE_SHIFT,
                        vma->vm_end - vma->vm_start, vma->vm_page_prot)) {
        printk("	audio spdif : failed remap_pfn_range\n");
        return -EAGAIN;
    }
    printk("audio spdif: mmap finished\n");	
    return 0;

	
}
static int __init audio_spdif_init_module(void)
{
    void *ptr_err;
    int ret = 0;
    major_spdif = register_chrdev(0, DEVICE_NAME, &fops_spdif);
    if (major_spdif < 0) {
        printk(KERN_ALERT "Registering spdif char device %s failed with %d\n", DEVICE_NAME, major_spdif);
        return major_spdif;
    }
    class_spdif = class_create(THIS_MODULE, DEVICE_NAME);
    if(IS_ERR(ptr_err = class_spdif)){
        goto err0;
    }
 //   create_spdif_attrs(class_spdif);
    dev_spdif = device_create(class_spdif, NULL, MKDEV(major_spdif,0),NULL, DEVICE_NAME);
    if(IS_ERR(ptr_err = dev_spdif)){
        goto err1;
    }
    mutex_init(&mutex_spdif);	
    printk(KERN_INFO "amlogic audio spdif interface device init!\n");
    return 0;
err2:
    device_destroy(class_spdif, MKDEV(major_spdif, 0));
err1:
    class_destroy(class_spdif);
err0:
    unregister_chrdev(major_spdif, DEVICE_NAME);
    return PTR_ERR(ptr_err);
}
static int __exit  audio_spdif_exit_module(void)
{
    device_destroy(class_spdif, MKDEV(major_spdif, 0));
    class_destroy(class_spdif);
    unregister_chrdev(major_spdif, DEVICE_NAME);	
    return 0;	
}
module_init(audio_spdif_init_module);
module_exit(audio_spdif_exit_module);
MODULE_DESCRIPTION("AMLOGIC IEC958 output interface driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("jian.xu <jian.xu@amlogic.com>");
