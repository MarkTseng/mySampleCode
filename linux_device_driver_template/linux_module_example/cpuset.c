#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/kthread.h> /* kthread_run, kthread_stop */
#include <linux/delay.h>
#include <linux/completion.h>

struct task_struct *kthread_task; /* task struct for kthread. */

int MainRoutine( void *pParam )
{
	struct cpumask vsc_cpumask;
	cpumask_clear(&vsc_cpumask);
	cpumask_set_cpu(3, &vsc_cpumask); // run task in core 3
	sched_setaffinity(0, &vsc_cpumask);

   	while (1)
	{
		pr_err("thread run ...\n");

		msleep(1000);
		// exit kthead
		if (kthread_should_stop())
			break;

	}

	return 0;
}


static int __init dsp_init(void)
{
    int ret = 0;

	kthread_task = kthread_run(MainRoutine, NULL, "myKthread");
	if (IS_ERR(kthread_task )) {
		pr_info("start kthread.\n");
	}

    return ret;
	
}

static void __exit dsp_cleanup(void)
{
	kthread_stop(kthread_task);
	return;
}
	
module_init(dsp_init);
module_exit(dsp_cleanup);
MODULE_AUTHOR("Mark Tseng mark.tseng@sunplus.com");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("SPV9200 DSP / SSD driver with TVFE API v1.0.0.0");


