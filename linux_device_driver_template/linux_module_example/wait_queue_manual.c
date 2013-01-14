#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");

wait_queue_head_t my_wait;
static int condition = 0;

int init_module( void )
{
	init_waitqueue_head(&my_wait);
	DEFINE_WAIT(wait);
	
	while(condition != 10) {
		prepare_to_wait(&my_wait, &wait, TASK_INTERRUPTIBLE);
		if (condition == 10) 
			break;
		condition++;	
		printk(KERN_ALERT "condition:%d\n",condition);
		schedule_timeout(100);
	}
	finish_wait(&my_wait, &wait);

	if (signal_pending(current))
		return -ERESTARTSYS;

	return 0;
}

void cleanup_module( void )
{
	printk(KERN_ALERT "wait queue exit module\n");
}

