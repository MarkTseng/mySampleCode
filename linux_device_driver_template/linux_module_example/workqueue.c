#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/workqueue.h>

MODULE_LICENSE("GPL");

static struct workqueue_struct *my_wq;

typedef struct {
	struct work_struct my_work;
	int    x;
} my_work_t;

my_work_t *work, *work2;

static void my_wq_function( struct work_struct *work)
{
	my_work_t *my_work = (my_work_t *)work;
	printk( "my_work.x %d\n", my_work->x );
	kfree( (void *)work );
	return;
}

int init_module( void )
{
	int ret;

	my_wq = create_workqueue("my_queue");
	if (my_wq) {

		/* Queue some work (item 1) */
		work = (my_work_t *)kmalloc(sizeof(my_work_t), GFP_KERNEL);
		if (work) {
			INIT_WORK( (struct work_struct *)work, my_wq_function );
			work->x = 1;
			ret = queue_work( my_wq, (struct work_struct *)work );
		}

		/* Queue some additional work (item 2) */
		work2 = (my_work_t *)kmalloc(sizeof(my_work_t), GFP_KERNEL);
		if (work2) {
			INIT_WORK( (struct work_struct *)work2, my_wq_function );
			work2->x = 2;
			ret = queue_work( my_wq, (struct work_struct *)work2 );
		}

	}

	return 0;
}

void cleanup_module( void )
{
	flush_workqueue( my_wq );
	destroy_workqueue( my_wq );
	return;
}

