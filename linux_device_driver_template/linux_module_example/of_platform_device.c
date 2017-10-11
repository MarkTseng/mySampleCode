#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/workqueue.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/version.h>
#include <linux/err.h>
#include <linux/types.h>
#include <linux/i2c.h>
//#include <linux/clk.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/of.h>
#include <asm/io.h>
//#include <linux/clk-provider.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/dma-mapping.h>
#include <linux/dmapool.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <asm/string.h>


static int __init of_device_init_driver(void)
{
	struct resource *res;
	struct device_node *dev_node;
	struct platform_device *pdev;

	dev_node = of_find_compatible_node(NULL, NULL, "mstar,i2c");
	if (!dev_node) {
		if (!dev_node)
			return -ENODEV;
	}

	pdev = of_find_device_by_node(dev_node);
	if (!pdev) {
		of_node_put(dev_node);
		return -ENODEV;
	}

    pr_info("of_device_init_driver:%s\n", pdev->name);
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    pr_info("res name: %s\n", res->name);
    pr_info("res start: %#x\n", res->start);
    pr_info("res end: %#x\n", res->end);
	
    return 0;
}

static void __exit of_device_exit_driver(void)
{
    pr_info("of_device_exit_driver\n");
}

subsys_initcall(of_device_init_driver);
module_exit(of_device_exit_driver);

MODULE_DESCRIPTION("OF platform_device driver");
MODULE_AUTHOR("Mark Tseng");
MODULE_LICENSE("GPL");
