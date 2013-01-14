/*
   Traceroute to init
   traceinit.c
   Robert Love Chapter 3
*/

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/module.h>

int init_module(void)
{
	struct task_struct *task;

	for(task=current;task!=&init_task;task=task->parent)
		//current is a macro which points to the current task / process
	{
		printk("%s [%d]\n",task->comm , task->pid);
	}

	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Cleaning up 1.\n");
}
