#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/workqueue.h>

static void mykmod_work_handler(struct work_struct *w);

static struct workqueue_struct *wq = 0;
static DECLARE_DELAYED_WORK(mykmod_work, mykmod_work_handler);
static unsigned long onesec;

	static void
mykmod_work_handler(struct work_struct *w)
{
	pr_info("mykmod work %u jiffies\n", (unsigned)onesec);
}


static int __devinit mykmod_init(void)
{
	onesec = msecs_to_jiffies(1000);
	pr_info("mykmod loaded %u jiffies\n", (unsigned)onesec);

	if (!wq)
		wq = create_singlethread_workqueue("mykmod");
	if (wq)
		queue_delayed_work(wq, &mykmod_work, onesec);

	return 0;
}

static void __devexit mykmod_exit(void)
{
	if (wq)
		destroy_workqueue(wq);
	pr_info("mykmod exit\n");
}

module_init(mykmod_init);
module_exit(mykmod_exit);

MODULE_DESCRIPTION("mykmod");
MODULE_LICENSE("GPL");

