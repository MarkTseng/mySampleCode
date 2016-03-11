/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/delay.h>
#include <linux/printk.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/platform_device.h>
#include <asm/uaccess.h>
#include "platform_driver_template.h"

/* char device variable */
int crt_major 		= CRT_MAJOR;
int crt_minor 		= 0;
int crt_nr_devs 	= CRT_NR_DEVS;
dev_t crt_devnum;
static struct class *crt_class;
static struct kobject *crt_kobj;
struct cdev crt_cdev;
spinlock_t crt_lock;
static unsigned int register_addr, register_val = 0;
static unsigned int register_password = 0;
static struct device *crt_device;
static struct platform_device *crt_platform_devs;


static ssize_t crt_register_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	if (register_password == 0) {
		return sprintf(buf, "enter register address access password\n");
	}else if ((register_addr < 0xb8000000)) {
		return sprintf(buf, "wrong register address 0x%08X\n", register_addr);
	}

	return sprintf(buf, "reg addr = 0x%08X , val = 0x%08X\n", register_addr, rtd_inl(register_addr));
}

static ssize_t crt_register_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{

	sscanf(buf, "0x%08X 0x%08X", &register_addr,&register_val);

	if(register_addr == 0x1234abcd)
		register_password = 1;

	if(register_addr == 0xabcd1234)
		register_password = 0;

	if((register_password == 1) && (register_val !=0))
	{
		rtd_outl(register_addr, register_val);
		register_val = 0;
	}


	return count;
}

static struct kobj_attribute crt_register =
__ATTR(register, 0666, crt_register_show, crt_register_store);

static struct attribute *attrs[] = {
	&crt_register.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

/* CRT char device file operation session */
long crt_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int retval = 0;
	unsigned long flags;
	crt_clk_onoff_cmd CLK_OnOff_CMD;

	spin_lock_irqsave(&crt_lock, flags);
	switch(cmd)
	{
		case CRT_IOC_CLK_ONOFF:
			retval = copy_from_user((void *)&CLK_OnOff_CMD, (const void __user *)arg, sizeof(crt_clk_onoff_cmd)) ?  -EFAULT : 0;
			break;
		default:
			pr_err("CRT : wrong ioctl cmd\n");
	}
	spin_unlock_irqrestore(&crt_lock, flags);

	return retval;
}


int crt_open(struct inode *inode, struct file *filp)
{
	return 0;
}

int crt_release(struct inode *inode, struct file *filp)
{
	return 0;
}

/* CRT file operation */
struct file_operations crt_fops = {
	read:		NULL,
				write:		NULL,
				unlocked_ioctl: crt_ioctl,
				open:		crt_open,
				release:	crt_release,
};

/* set /dev/crt mode to 0666  */
static char *crt_devnode(struct device *dev, mode_t *mode)
{
	*mode = 0666;
	return NULL;
}


#ifdef CONFIG_PM
static int crt_suspend(struct platform_device *dev, pm_message_t state)
{
	pr_debug("crt_suspend\n");
	return 0;
}
static int crt_resume(struct platform_device *dev)
{
	pr_debug("crt_resume\n");
	return 0;
}
#endif

static struct platform_driver crt_platform_driver = {
#ifdef CONFIG_PM
	.suspend    = crt_suspend,
	.resume     = crt_resume,
#endif
	. driver = {
		.name       = "crt",
		.bus        = &platform_bus_type,
	} ,
} ;


static int __init crt_module_init(void)
{
	int retval;

	crt_kobj = kobject_create_and_add("crt", NULL);
	if (!crt_kobj)
		return -ENOMEM;

	retval = sysfs_create_group(crt_kobj, &attr_group);
	if (retval)
		kobject_put(crt_kobj);


	/* register device to get major and minor number */
	if (crt_major) {
		crt_devnum = MKDEV(crt_major, crt_minor);
		retval = register_chrdev_region(crt_devnum, crt_nr_devs, "crt");
	} else {
		retval = alloc_chrdev_region(&crt_devnum, crt_minor, crt_nr_devs, "crt");
		crt_major = MAJOR(crt_devnum);
	}
	if (retval < 0) {
		pr_warn("CRT : can't get major %d\n", crt_major);
		return retval;
	}

	/* create device node by udev API */
	crt_class = class_create(THIS_MODULE, "crt");
	if (IS_ERR(crt_class)) {
		return PTR_ERR(crt_class);
	}
	crt_class->devnode = crt_devnode;
	crt_device = device_create(crt_class, NULL, crt_devnum, NULL, "crt");
	pr_info("CRT module init, major number = %d, device name = %s \n", crt_major, dev_name(crt_device));

	/* cdev API to register file operation */
	cdev_init (&crt_cdev, &crt_fops);
	crt_cdev.owner = THIS_MODULE;
	crt_cdev.ops = &crt_fops;
	retval = cdev_add (&crt_cdev, crt_devnum , 1);
	if (retval) {
		pr_err("Error %d adding char_reg_setup_cdev", retval);
	}

	crt_platform_devs = platform_device_register_simple("crt", -1, NULL, 0);
	/* pr_info("CRT platform device name %s\n", dev_name(&crt_platform_devs->dev)); */

	if(platform_driver_register(&crt_platform_driver) != 0) {
		platform_device_unregister(crt_platform_devs);
		crt_platform_devs = NULL;
		pr_warn("crt platform driver register fail\n");
		return retval;
	}

	spin_lock_init(&crt_lock);

	return retval;
}

static void __exit crt_module_exit(void)
{
	if(crt_platform_devs)
		platform_device_unregister(crt_platform_devs);

	platform_driver_unregister(&crt_platform_driver);

	kobject_put(crt_kobj);
	device_destroy(crt_class, crt_devnum);
	class_destroy(crt_class);
	unregister_chrdev_region(crt_devnum, crt_nr_devs);

}

module_init(crt_module_init);
module_exit(crt_module_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Tseng <mark.tseng@realtek.com>");
