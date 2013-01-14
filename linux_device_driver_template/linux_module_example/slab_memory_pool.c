#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/workqueue.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");

static struct kmem_cache *my_cachep;

static void init_my_cache(void)
{

	my_cachep = kmem_cache_create( 
			"my_cache",            /* Name */
			409600,                    /* Object Size */
			4096,                     /* Alignment */
			SLAB_HWCACHE_ALIGN,    /* Flags */
			NULL);          /* Constructor*/

	return;
}

int slab_test( void )
{
	void *object;

	printk("Cache name is %s\n", kmem_cache_name(my_cachep));
	printk("Cache object size is %d\n", kmem_cache_size(my_cachep));

	object = kmem_cache_alloc(my_cachep, GFP_KERNEL);

	if (object) {
		kmem_cache_free(my_cachep, object);
	}

	return 0;
}

static void remove_my_cache(void)
{

	if(my_cachep) 
		kmem_cache_destroy(my_cachep);

	return;
}

int init_module(void)
{
	init_my_cache();
	slab_test();

	return 0;
}

void cleanup_module(void)
{
	remove_my_cache();

	return;
}

