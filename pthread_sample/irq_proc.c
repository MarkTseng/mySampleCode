#include <linux/proc_fs.h>
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <asm/mach-sphe8600/sphe8600_irq.h>
#include "gl_common.h"
#include "irq_proc.h"

#ifdef GL_DEBUG_IRQ_PROC
#define glirq_printf(fmt, arg...)	gsl_printf(fmt, ##arg)
#else
#define glirq_printf(fmt, arg...)
#endif

#define LAMBO86_NR_IRQS NR_IRQS
#define LAMBO86_IRQ_PROC_DIR	"lambo86-irq"
#define AUTO_IRQ_RESPONSE 

struct irq_proc {
	unsigned int irq;
	wait_queue_head_t q;
	atomic_t count;
	char devname[TASK_COMM_LEN];
};

static struct proc_dir_entry *lambo86_irq_dir;
static struct irq_proc *lambo86_irq_proc[LAMBO86_NR_IRQS];

static irqreturn_t lambo86_irq_proc_handler(int irq, void *vip)
{
	struct irq_proc *ip = lambo86_irq_proc[irq];

	glirq_printf("");

	if (ip) {
		disable_irq_nosync(ip->irq);
		atomic_inc(&ip->count);
		glirq_printf("%s : ip->count = %d, irq = %d\n", __FUNCTION__, atomic_read(&ip->count), irq);
		wake_up_interruptible(&ip->q);
	}
	return IRQ_HANDLED;
}

static unsigned int irq_proc_poll(struct file *filp, struct poll_table_struct *wait)
{
	struct proc_dir_entry *ent = PDE(filp->f_path.dentry->d_inode);
	unsigned int irq = (unsigned int)ent->data;
	struct irq_proc *ip = lambo86_irq_proc[irq];

	glirq_printf("");

	if (ip) {
		poll_wait(filp, &ip->q, wait);
#ifdef AUTO_IRQ_RESPONSE
		enable_irq(ip->irq);
		if (atomic_read(&ip->count) > 0)
		{
			atomic_dec(&ip->count);
			return POLLIN | POLLRDNORM; /* readable */
		}
#else
		if (atomic_read(&ip->count) > 0)
			return POLLIN | POLLRDNORM; /* readable */
#endif
	}

	return 0;
}

static int irq_proc_ioctl(struct inode * inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	glirq_printf("");

	switch(cmd) {
	case LAMBO86_IOC_IRQ_REQUEST:
	{
		struct proc_dir_entry *ent = PDE(inode);
		unsigned int irq = (unsigned int)ent->data;
		struct irq_proc *ip = lambo86_irq_proc[irq];
		int error;
		int proto = IRQF_SHARED;

		glirq_printf("request irq num is [%d]\n", irq);

		error = request_irq(irq,
							lambo86_irq_proc_handler,
							proto,
							ip->devname,
							lambo86_irq_proc_handler);
		if (error < 0) {
			printk(KERN_ERR"request irq%d failed\n", (unsigned int)ip->irq);
			return error;
		}
		break;
	}
	case LAMBO86_IOC_IRQ_FREE:
	{
		struct proc_dir_entry *ent = PDE(inode);
		unsigned int irq = (unsigned int)ent->data;

		free_irq(irq, lambo86_irq_proc_handler);
		break;
	}
	default:
		return -1;
	}

	return 0;
}

static ssize_t irq_proc_read(struct file *filp, char  __user *bufp, size_t len, loff_t *ppos)
{
#ifndef AUTO_IRQ_RESPONSE 
	struct proc_dir_entry *ent = PDE(filp->f_path.dentry->d_inode);
	unsigned int irq = (unsigned int)ent->data;
	struct irq_proc *ip = lambo86_irq_proc[irq];
	int pending;

	glirq_printf("");

	if (len < sizeof(int)) {
		return -EINVAL;
	}

	if (ip) {
		pending = atomic_read(&ip->count);
		if (pending == 0) {
			if (filp->f_flags & O_NONBLOCK)
				return -EWOULDBLOCK;
		}
		glirq_printf("pending = %d\n", pending);

		if (copy_to_user(bufp, &pending, sizeof(pending))) {
			return -EFAULT;
		}

		*ppos += sizeof(pending);
	}

	return sizeof(pending);
#else
	return 0;
#endif
}

static ssize_t irq_proc_write(struct file *filp, const char  __user *bufp, size_t len, loff_t *ppos)
{
#ifndef AUTO_IRQ_RESPONSE
	struct proc_dir_entry *ent = PDE(filp->f_path.dentry->d_inode);
	unsigned int irq = (unsigned int)ent->data;
	struct irq_proc *ip = lambo86_irq_proc[irq];
	int pending;

	glirq_printf("");

	if (len < sizeof(int)) {
		return -EINVAL;
	}

	if (ip) {
		pending = (int)bufp;
		pending = atomic_sub_return(pending, &ip->count);
		if (pending == 0) {
			enable_irq(ip->irq);
			if (filp->f_flags & O_NONBLOCK) {
				return -EWOULDBLOCK;
			}
		}
	}

	return sizeof(pending);
#else
	return 0;
#endif
}

static int irq_proc_open(struct inode *inop, struct file *filp)
{
	struct irq_proc *ip;
	struct proc_dir_entry *ent = PDE(inop);

	glirq_printf();

	ip = kmalloc(sizeof(*ip), GFP_KERNEL);
	if (ip == NULL) {
		return -ENOMEM;
	}

	memset(ip, 0, sizeof(*ip));
	strcpy(ip->devname, current->comm);
	init_waitqueue_head(&ip->q);
	atomic_set(&ip->count, 0);
	ip->irq = (unsigned int)ent->data;
	glirq_printf("register irq num is [%d]\n", (unsigned int)ip->irq);
	lambo86_irq_proc[ip->irq] = ip;

	return 0;
}

static int irq_proc_release(struct inode *inop, struct file *filp)
{
	struct proc_dir_entry *ent = PDE(inop);
	unsigned int irq = (unsigned int)ent->data;
	struct irq_proc *ip = lambo86_irq_proc[irq];

	glirq_printf("");
	kfree(ip);

	return 0;
}

static struct file_operations lambo86_irq_proc_file_operations = {
 	.read		= irq_proc_read,
 	.write		= irq_proc_write,
 	.open		= irq_proc_open,
 	.release	= irq_proc_release,
 	.poll		= irq_proc_poll,
	.ioctl		= irq_proc_ioctl,
};

static int gl_irq_proc_register(void)
{
	int i;
	char name[8] = "irq00";

	glirq_printf("");

	if (!lambo86_irq_dir) {
		return -1;
	}

	glirq_printf("LAMBO86_NR_IRQS = %d\n", LAMBO86_NR_IRQS);
	for (i = CYGNUM_HAL_INTERRUPT_RISC_TIMER; i <= CYGNUM_HAL_INTERRUPT_CBSW; i++) {

		/* The irq used by kernel should be protected */
		switch(i)
		{
		case CYGNUM_HAL_INTERRUPT_RISC_TIMER:
		//case CYGNUM_HAL_INTERRUPT_MINI_USB:
			continue;
		default:
			;
		}

		name[3] = '0' + i / 10;
		name[4] = '0' + i % 10;
		proc_create_data(name, 0600, lambo86_irq_dir,
			 &lambo86_irq_proc_file_operations, (void *)(long)i);
	}

	return -1;
}

static void gl_irq_proc_unregister(void)
{
	int i;
	char name[32] = LAMBO86_IRQ_PROC_DIR"/irq00";
	int len = strlen(name);

	glirq_printf("unregister irq proc\n");

	if (!lambo86_irq_dir) {
		return;
	}

	for (i = CYGNUM_HAL_INTERRUPT_RISC_TIMER; i <= CYGNUM_HAL_INTERRUPT_CBSW; i++) {

		/* The irq used by kernel should be protected */
		switch(i)
		{
		case CYGNUM_HAL_INTERRUPT_RISC_TIMER:
		//case CYGNUM_HAL_INTERRUPT_MINI_USB:
			continue;
		default:
			;
		}

		name[len - 2] = '0' + i / 10;
		name[len - 1] = '0' + i % 10;
		remove_proc_entry(name, NULL);
	}
}

int gl_irq_proc_init(void)
{
    glirq_printf("");

    /* Create /proc/LAMBO86_IRQ_PROC_DIR */
    lambo86_irq_dir = proc_mkdir(LAMBO86_IRQ_PROC_DIR, NULL);
    if (!lambo86_irq_dir) {
        return -1;
	}

    /* Create entries for all IRQs */
    gl_irq_proc_register();

	return 0;
}

void gl_irq_proc_exit(void)
{
	glirq_printf("irq proc clean up!\n");

	if (!lambo86_irq_dir) {
		return;
	}

    /* Delete entries for all IRQs */
    gl_irq_proc_unregister();

    /* Delete /proc/LAMBO86_IRQ_PROC_DIR */
	remove_proc_entry(LAMBO86_IRQ_PROC_DIR, NULL);
}


