#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/workqueue.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");

#define SIZE_1M		(1*1024*1024)
#define SIZE_2M		(2*1024*1024)
#define SIZE_3M		(3*1024*1024)
#define SIZE_4M		(4*1024*1024)
#define SIZE_8M		(8*1024*1024)
#define SIZE_16M	(16*1024*1024)
#define SIZE_32M	(32*1024*1024)
#define SIZE_64M	(64*1024*1024)

unsigned char *momory_ptr = NULL;
unsigned char *momory_ptr2 = NULL;
unsigned char *momory_ptr3 = NULL;
unsigned char *momory_ptr4 = NULL;
unsigned char *momory_ptr5 = NULL;
unsigned char *momory_ptr6 = NULL;

int init_module( void )
{

	momory_ptr = (unsigned char *)kmalloc(SIZE_4M, GFP_KERNEL);
	memset(momory_ptr,0,SIZE_4M);
	momory_ptr2 = (unsigned char *)kmalloc(SIZE_4M, GFP_KERNEL);
	memset(momory_ptr2,0,SIZE_4M);
	momory_ptr3 = (unsigned char *)kmalloc(SIZE_4M, GFP_KERNEL);
	memset(momory_ptr3,0,SIZE_4M);
	momory_ptr4 = (unsigned char *)kmalloc(SIZE_4M, GFP_KERNEL);
	memset(momory_ptr4,0,SIZE_4M);
	momory_ptr5 = (unsigned char *)kmalloc(SIZE_4M, GFP_KERNEL);
	memset(momory_ptr5,0,SIZE_4M);
	momory_ptr6 = (unsigned char *)vmalloc(SIZE_16M);
	memset(momory_ptr6,0,SIZE_16M);

	printk("momory_ptr(0x%p): physical addr(0x%lx)\n",momory_ptr,virt_to_phys(momory_ptr));
	printk("momory_ptr2(0x%p): physical addr(0x%lx)\n",momory_ptr2,virt_to_phys(momory_ptr2));
	printk("momory_ptr3(0x%p): physical addr(0x%lx)\n",momory_ptr3,virt_to_phys(momory_ptr3));
	printk("momory_ptr4(0x%p): physical addr(0x%lx)\n",momory_ptr4,virt_to_phys(momory_ptr4));
	printk("momory_ptr5(0x%p): physical addr(0x%lx)\n",momory_ptr5,virt_to_phys(momory_ptr5));
	printk("momory_ptr6(0x%p): physical addr(0x%lx)\n",momory_ptr6,virt_to_phys(momory_ptr6));
#if 0
	/* test kmalloc 1M size*/
	momory_ptr = (char *)kmalloc(SIZE_1M, GFP_KERNEL);
	if(momory_ptr != NULL)
	{
		printk("Allocate 1M success\n");
		kfree(momory_ptr);
		momory_ptr = NULL;
	}

	/* test kmalloc 2M size*/
	momory_ptr = (char *)kmalloc(SIZE_2M, GFP_KERNEL);
	if(momory_ptr != NULL)
	{
		printk("Allocate 2M success\n");
		kfree(momory_ptr);
		momory_ptr = NULL;
	}

	/* test kmalloc 3M size*/
	momory_ptr = (char *)kmalloc(SIZE_3M, GFP_KERNEL);
	if(momory_ptr != NULL)
	{
		printk("Allocate 3M success\n");
		kfree(momory_ptr);
		momory_ptr = NULL;
	}

	/* test kmalloc 4M size*/
	momory_ptr = (char *)kmalloc(SIZE_4M, GFP_KERNEL);
	if(momory_ptr != NULL)
	{
		printk("Allocate 4M success\n");
		kfree(momory_ptr);
		momory_ptr = NULL;
	}

	/* test kmalloc 8M size*/
	momory_ptr = (char *)kmalloc(SIZE_8M, GFP_KERNEL);
	if(momory_ptr != NULL)
	{
		printk("Allocate 8M success\n");
		kfree(momory_ptr);
		momory_ptr = NULL;
	}

	/* test kmalloc 16M size*/
	momory_ptr = (char *)kmalloc(SIZE_16M, GFP_KERNEL);
	if(momory_ptr != NULL)
	{
		printk("Allocate 16M success\n");
		kfree(momory_ptr);
		momory_ptr = NULL;
	}
#endif


	return 0;
}

void cleanup_module( void )
{

	kfree(momory_ptr);
	kfree(momory_ptr2);
	kfree(momory_ptr3);
	kfree(momory_ptr4);
	kfree(momory_ptr5);
	vfree(momory_ptr6);
	return;
}

