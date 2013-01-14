/* ProcessList.c 
   Robert Love Chapter 3
   */
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/mm.h>
#include <linux/nmi.h>
#include <linux/quicklist.h>


#define K(x) ((x) << (PAGE_SHIFT-10))
int init_module(void)
{
	struct task_struct *task;
	struct mm_struct *mm;
	struct vm_area_struct *vma;
	int count = 0;
	int rem = 0;

#if 1
	for_each_process(task)
	{
		printk("[name]=%s, [pid]=%d, [state]=%ld",task->comm , task->pid, task->state);
		printk("[nvcsw]=%lu, [nivcsw]=%lu\n",task->nvcsw, task->nivcsw);

		if(task->mm != NULL)
		{
			mm = task->mm;
			for (vma = mm->mmap ; vma ; vma = vma->vm_next) {
				printk ("Vma number %d: ", ++count);
				printk("  Starts at 0x%lx, Ends at 0x%lx\n",
						vma->vm_start, vma->vm_end);
			}
			printk("Code  Segment start = 0x%lx, end = 0x%lx \n"
					"Data  Segment start = 0x%lx, end = 0x%lx\n"
					"Stack Segment start = 0x%lx\n",
					mm->start_code, mm->end_code,
					mm->start_data, mm->end_data,
					mm->start_stack);

			rem = global_page_state(NR_ACTIVE_ANON) +
				global_page_state(NR_ACTIVE_FILE) +
				global_page_state(NR_INACTIVE_ANON) +
				global_page_state(NR_INACTIVE_FILE);
			printk("[rem]=%dkB, [mm_rss]=%lukB,[total_vm]=%lukB\n",K(rem), K(get_mm_rss(task->mm)),K(task->mm->total_vm));

		}
		count = 0;
		
#if 0
		// kill avplayer task
		if(!strcmp(task->comm,"avplayer"))
			force_sig(SIGKILL, task);
#endif

		printk("\n");
	}
#endif

#if 0
	list_for_each(listptr,&init_task.tasks)
	{
		task = list_entry(listptr,struct task_struct ,tasks);
		printk("%s [%d]\n",task->comm , task->pid);
	}
#endif

	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Cleaning Up.\n");
}
