#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/workqueue.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

/** dedicated thread for the Workqueue **/
/** Make the structure to be passed to the workqueue handler**/
typedef struct
{
	struct work_struct my_work;
	int x;
}my_work_t;

/** my_work_t instance **/

my_work_t *work;

/** This is the workqueue handler function - a deferred function **/
static void ework_handler(struct work_struct *pwork);

static void ework_handler2(struct work_struct *pwork);

/** Declare the workqueue struct **/
static struct workqueue_struct *eWq = 0;

/** statically declare the work associated with the workqueue handler */
/**  DECLARE_WORK  (struct work_struct name,  void (*func)(void *));  **/
/** WORKQUEUENAME - the name of the workqueue, ework_handler - the workqueue handler function **/
static DECLARE_WORK(eWorkqueue, ework_handler2);

/** struct container *my_container; */
/** my_container = container_of(my_ptr, struct container, this_data); **/

/** container_of macro -gets the parent structure pointer from the member pointer of the structure **/

static void ework_handler2(struct work_struct *pwork)
{
	printk("The ework_handler.2..called\r\n");

}

static void ework_handler(struct work_struct *pwork)
{
	my_work_t *temp;
	printk("The ework_handler...called\r\n");

	/** pwork is the pointer of my_work **/
	temp = container_of(pwork,my_work_t,my_work);

	printk("The value of x is %d\r\n",temp->x);

}

static int eworkqueue_init(void)
{
	printk("Hello eWorkqueue !!! Welcome!!\r\n");

	printk("Create work struct object!!\r\n");

	work = (my_work_t *) kmalloc(sizeof(my_work_t), GFP_KERNEL);

	work->x = 1010;

	/** Init the work struct with the work handler **/
	INIT_WORK( &(work->my_work), ework_handler );

	if (!eWq)
	{
		printk("ewq..Single Thread created\r\n");
		eWq = create_singlethread_workqueue("eWorkqueue");

	}
	if (eWq)
	{
		printk("Push!! my work into the eWq--Queue Work..\r\n");
		queue_work(eWq, &(work->my_work) );
	}
	/** This, if you want to use the default event thread -- no need to create eWq here*/
	schedule_work(&eWorkqueue);

	return 0;
}

static void eworkqueue_exit(void)
{
	if (eWq)
		destroy_workqueue(eWq);

	kfree(work);
	printk("GoodBye..WorkQueue");
}

module_init(eworkqueue_init);
module_exit(eworkqueue_exit);


