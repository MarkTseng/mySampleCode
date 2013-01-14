#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>
#include <linux/kernel.h> /* printk() */
#include <linux/fs.h>	  /* everything... */
#include <linux/errno.h>  /* error codes */
#include <linux/delay.h>  /* udelay */
#include <linux/slab.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/timer.h>
#include <linux/poll.h>
#include <asm/io.h>
#include <asm/mach-sphe8600/sphe8600_irq.h>

MODULE_LICENSE("Dual BSD/GPL");

static int irq_nm = CYGNUM_HAL_INTERRUPT_TGEN_FLD_START;
static int condition = 1;

static void irqtest_do_work(struct work_struct *work);
static DECLARE_WORK(irqtest_work, irqtest_do_work);
static struct workqueue_struct *irqtest_workqueue;

static void irqtest_do_work(struct work_struct *work)
{
	//printk("CYGNUM_HAL_INTERRUPT_DEC_END interrupt\n");
}

/*
 * The top-half interrupt handler.
 */
static irqreturn_t irqtest_interrupt(int irq, void *dev_id)
{
	if (! condition) 
		return IRQ_NONE;

	queue_work(irqtest_workqueue, &irqtest_work);
	return IRQ_HANDLED;
}

int init_module(void)
{
	int ret;
 
	/* Set up our workqueue. */
	irqtest_workqueue = create_singlethread_workqueue("irqtest_workqueue");

	/* Request the IRQ */
	ret = request_irq(irq_nm, irqtest_interrupt, 0, "irqtest", NULL);	
	if (ret) {
		printk(KERN_INFO "irqtest: can't get assigned irq %i\n", irq_nm);
		cleanup_module();
		return ret;
	}

	return ret;
}

void cleanup_module(void)
{
	free_irq(irq_nm, NULL);
	flush_workqueue(irqtest_workqueue);
	destroy_workqueue(irqtest_workqueue);
}

