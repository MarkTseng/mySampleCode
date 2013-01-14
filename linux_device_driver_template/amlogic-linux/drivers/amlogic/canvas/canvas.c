/*
 * AMLOGIC Canvas management driver.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the named License,
 * or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Author:  Tim Yao <timyao@amlogic.com>
 *
 */

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/major.h>
#include <linux/amports/canvas.h>

#include <mach/am_regs.h>

#define DRIVER_NAME "amcanvas"
#define MODULE_NAME "amcanvas"
#define DEVICE_NAME "amcanvas"
#define CLASS_NAME  "amcanvas-class"

#define pr_dbg(fmt, args...) printk(KERN_DEBUG "Canvas: " fmt, ## args)
#define pr_error(fmt, args...) printk(KERN_ERR "Canvas: " fmt, ## args)

#define CANVAS_NUM	192

static struct platform_device *canvas_dev;
static spinlock_t lock = SPIN_LOCK_UNLOCKED;
static canvas_t canvasPool[CANVAS_NUM];

void canvas_config(u32 index, ulong addr, u32 width,
				  u32 height, u32 wrap, u32 blkmode)
{
    ulong flags;
    canvas_t *canvasP = &canvasPool[index];

	if (index >= CANVAS_NUM)
		return;

    spin_lock_irqsave(&lock, flags);

    WRITE_APB_REG(DC_CAV_LUT_DATAL,
					(((addr + 7) >> 3) & CANVAS_ADDR_LMASK) |
					((((width + 7) >> 3) & CANVAS_WIDTH_LMASK) << CANVAS_WIDTH_LBIT));

    WRITE_APB_REG(DC_CAV_LUT_DATAH,
					((((width + 7) >> 3) >> CANVAS_WIDTH_LWID) << CANVAS_WIDTH_HBIT) |
					((height & CANVAS_HEIGHT_MASK) << CANVAS_HEIGHT_BIT)	|
					((wrap & CANVAS_XWRAP) ? CANVAS_XWRAP : 0)              |
					((wrap & CANVAS_YWRAP) ? CANVAS_YWRAP : 0)              |
					((blkmode & CANVAS_BLKMODE_MASK) << CANVAS_BLKMODE_BIT));

    WRITE_APB_REG(DC_CAV_LUT_ADDR, CANVAS_LUT_WR_EN | index);

	READ_APB_REG(DC_CAV_LUT_DATAH);

	canvasP->addr = addr;
	canvasP->width = width;
	canvasP->height = height;
	canvasP->wrap = wrap;
	canvasP->blkmode = blkmode;

    spin_unlock_irqrestore(&lock, flags);
}
EXPORT_SYMBOL(canvas_config);

void canvas_read(u32 index, canvas_t *p)
{
	if (index < CANVAS_NUM)
		*p = canvasPool[index];
}
EXPORT_SYMBOL(canvas_read);

void canvas_copy(u32 src, u32 dst)
{
    unsigned long addr;
    unsigned width, height, wrap, blkmode;

    if ((src >= CANVAS_NUM) || (dst >= CANVAS_NUM))
        return;

    addr = canvasPool[src].addr;
    width = canvasPool[src].width;
    height = canvasPool[src].height;
    wrap = canvasPool[src].wrap;
    blkmode = canvasPool[src].blkmode;
    
    WRITE_APB_REG(DC_CAV_LUT_DATAL,
        (((addr + 7) >> 3) & CANVAS_ADDR_LMASK) |
        ((((width + 7) >> 3) & CANVAS_WIDTH_LMASK) << CANVAS_WIDTH_LBIT));

    WRITE_APB_REG(DC_CAV_LUT_DATAH,
        ((((width + 7) >> 3) >> CANVAS_WIDTH_LWID) << CANVAS_WIDTH_HBIT) |
        ((height & CANVAS_HEIGHT_MASK) << CANVAS_HEIGHT_BIT)    |
        ((wrap & CANVAS_XWRAP) ? CANVAS_XWRAP : 0)              | 
        ((wrap & CANVAS_YWRAP) ? CANVAS_YWRAP : 0)              | 
        ((blkmode & CANVAS_BLKMODE_MASK) << CANVAS_BLKMODE_BIT));

    WRITE_APB_REG(DC_CAV_LUT_ADDR, CANVAS_LUT_WR_EN | dst);

    // read a cbus to make sure last write finish.
    READ_APB_REG(DC_CAV_LUT_DATAH);
    
    canvasPool[dst].addr = addr;
    canvasPool[dst].width = width;
    canvasPool[dst].height = height;
    canvasPool[dst].wrap = wrap;
    canvasPool[dst].blkmode = blkmode;

    return;
}
EXPORT_SYMBOL(canvas_copy);

void canvas_update_addr(u32 index, u32 addr)
{
    ulong flags;
    
    if (index >= CANVAS_NUM)
        return;

    spin_lock_irqsave(&lock, flags);

    canvasPool[index].addr = addr;

    WRITE_APB_REG(DC_CAV_LUT_DATAL,
        (((canvasPool[index].addr + 7) >> 3) & CANVAS_ADDR_LMASK) |
        ((((canvasPool[index].width + 7) >> 3) & CANVAS_WIDTH_LMASK) << CANVAS_WIDTH_LBIT));

    WRITE_APB_REG(DC_CAV_LUT_DATAH,
        ((((canvasPool[index].width + 7) >> 3) >> CANVAS_WIDTH_LWID) << CANVAS_WIDTH_HBIT) |
        ((canvasPool[index].height & CANVAS_HEIGHT_MASK) << CANVAS_HEIGHT_BIT)   |
        ((canvasPool[index].wrap & CANVAS_XWRAP) ? CANVAS_XWRAP : 0)             | 
        ((canvasPool[index].wrap & CANVAS_YWRAP) ? CANVAS_YWRAP : 0)             | 
        ((canvasPool[index].blkmode & CANVAS_BLKMODE_MASK) << CANVAS_BLKMODE_BIT));

    WRITE_APB_REG(DC_CAV_LUT_ADDR, CANVAS_LUT_WR_EN | index);

    // read a cbus to make sure last write finish.
    READ_APB_REG(DC_CAV_LUT_DATAH);

    spin_unlock_irqrestore(&lock, flags);

    return;
}
EXPORT_SYMBOL(canvas_update_addr);

unsigned int canvas_get_addr(u32 index)
{
    return canvasPool[index].addr;
}
EXPORT_SYMBOL(canvas_get_addr);

/*********************************************************/
#define to_canvas(kobj) container_of(kobj, canvas_t, kobj)
static ssize_t addr_show(canvas_t *canvas, char *buf)
{
	return sprintf(buf, "0x%lx\n", canvas->addr);
}

static ssize_t width_show(canvas_t *canvas, char *buf)
{
	return sprintf(buf, "%d\n", canvas->width);
}

static ssize_t height_show(canvas_t *canvas, char *buf)
{
	return sprintf(buf, "%d\n", canvas->height);
}

struct canvas_sysfs_entry {
	struct attribute attr;
	ssize_t (*show)(canvas_t *, char *);
};
static struct canvas_sysfs_entry addr_attribute = __ATTR_RO(addr);
static struct canvas_sysfs_entry width_attribute = __ATTR_RO(width);
static struct canvas_sysfs_entry height_attribute = __ATTR_RO(height);

static void canvas_release(struct kobject *kobj)
{
}

static ssize_t canvas_type_show(struct kobject *kobj, struct attribute *attr,
			     char *buf)
{
	canvas_t *canvas = to_canvas(kobj);
	struct canvas_sysfs_entry *entry;

	entry = container_of(attr, struct canvas_sysfs_entry, attr);

	if (!entry->show)
		return -EIO;

	return entry->show(canvas, buf);
}

static struct sysfs_ops canvas_sysfs_ops = {
	.show = canvas_type_show,
};

static struct attribute *canvas_attrs[] =
{
	&addr_attribute.attr,
	&width_attribute.attr,
	&height_attribute.attr,
	NULL,
};

static struct kobj_type canvas_attr_type = {
	.release	= canvas_release,
	.sysfs_ops	= &canvas_sysfs_ops,
	.default_attrs	= canvas_attrs,
};

static int __devinit canvas_probe(struct platform_device *pdev)
{
    int i, r;
    
	for (i = 0; i < CANVAS_NUM; i++) {
		r = kobject_init_and_add(&canvasPool[i].kobj, &canvas_attr_type,
				&pdev->dev.kobj, "%d", i);
		if (r) {
			pr_error("Unable to create canvas objects %d\n", i);
			goto err;
		}
	}

    canvas_dev = pdev;

    return 0;

err:
	for (i--; i>=0; i--)
		kobject_put(&canvasPool[i].kobj);

	pr_error("Canvas driver probe failed\n");

	return r;	
}

static int __devexit canvas_remove(struct platform_device *pdev)
{
    int i;

	for (i=0; i<CANVAS_NUM; i++)
		kobject_put(&canvasPool[i].kobj);

    return 0;
}

static struct platform_driver canvas_driver = {
    .probe  = canvas_probe,
    .remove = canvas_remove,
    .driver = {
        .name = DEVICE_NAME,
    },
};

static int __init amcanvas_init(void)
{
    int r;
	
    r = platform_driver_register(&canvas_driver);
    if (r) {
        pr_error("Unable to register canvas driver\n");
        return r;
    }

    canvas_dev = platform_device_register_simple(DEVICE_NAME, -1,
                                                NULL, 0);
    if (IS_ERR(canvas_dev)) {
        pr_error("Unable to register canvas device\n");
        platform_driver_unregister(&canvas_driver);
        return PTR_ERR(canvas_dev);
    }

    return 0;
}

static void __exit amcanvas_exit(void)
{
    platform_device_unregister(canvas_dev);
    platform_driver_unregister(&canvas_driver);
}

subsys_initcall(amcanvas_init);
module_exit(amcanvas_exit);

MODULE_DESCRIPTION("AMLOGIC Canvas management driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tim Yao <timyao@amlogic.com>");
