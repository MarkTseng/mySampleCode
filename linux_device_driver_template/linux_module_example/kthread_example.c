#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/kthread.h> /* kthread_run, kthread_stop */
#include <linux/delay.h>
#include <linux/completion.h>

DECLARE_COMPLETION(comp_lock);
struct task_struct *kthread_task; /* task struct for kthread. */

int MainRoutine( void *pParam )
{
   	while (1)
	{
		pr_info("start to wait complete : %lu \n", jiffies);
		wait_for_completion(&comp_lock);
		pr_info("complete : %lu \n", jiffies);

		// exit kthead
		if (kthread_should_stop())
			break;

	}

	return 0;
}


static int __init dsp_init(void)
{
    int ret = 0;

	INIT_COMPLETION(comp_lock);
	kthread_task = kthread_run(MainRoutine, NULL, "myKthread");
	if (IS_ERR(kthread_task )) {
		pr_info("start kthread.\n");
	}
	msleep(10000);

	complete (&comp_lock);

	kthread_stop(kthread_task);

    return ret;
	
}

static void __exit dsp_cleanup(void)
{
	
	return;
}
	
module_init(dsp_init);
module_exit(dsp_cleanup);
MODULE_AUTHOR("Mark Tseng mark.tseng@sunplus.com");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("SPV9200 DSP / SSD driver with TVFE API v1.0.0.0");


