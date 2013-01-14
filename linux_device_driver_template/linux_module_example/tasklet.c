#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");

char my_tasklet_data[]="my_tasklet_function was called";
int count = 0;

void my_tasklet_function( unsigned long data );

DECLARE_TASKLET( my_tasklet, my_tasklet_function, 
		(unsigned long) &my_tasklet_data );

/* Bottom Half Function */
void my_tasklet_function( unsigned long data )
{
	printk( "count %d (%ld): %s\n",count, jiffies,(char *)data );

	count++;
	if(count < 10)
		tasklet_schedule( &my_tasklet );

}

int init_module( void )
{
	/* Schedule the Bottom Half */
	tasklet_schedule( &my_tasklet );

	return 0;
}

void cleanup_module( void )
{
	/* Stop the tasklet before we exit */
	tasklet_kill( &my_tasklet );
	return;
}

