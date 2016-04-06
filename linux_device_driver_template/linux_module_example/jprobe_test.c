/*jprobe-example.c */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uio.h>
#include <linux/kprobes.h>
#include <linux/kallsyms.h>

/*
 * Jumper probe for do_fork.
 * Mirror principle enables access to arguments of the probed routine
 * from the probe handler.
 */

/* Proxy routine having the same arguments as actual do_fork() routine */
long jdo_fork(unsigned long clone_flags, unsigned long stack_start,
		struct pt_regs *regs, unsigned long stack_size,
		int __user * parent_tidptr, int __user * child_tidptr)
{
	printk("jprobe: clone_flags=0x%lx, stack_size=0x%lx, regs=0x%p\n",
			clone_flags, stack_size, regs);
	/* Always end with a call to jprobe_return(). */
	jprobe_return();
	/*NOTREACHED*/
	return 0;
}

static struct jprobe my_jprobe = {
	.entry = (kprobe_opcode_t *) jdo_fork
};

int init_module(void)
{
	int ret;
	my_jprobe.kp.addr = (kprobe_opcode_t *) kallsyms_lookup_name("do_fork");
	if (!my_jprobe.kp.addr) {
		printk("Couldn't find %s to plant jprobe\n", "do_fork");
		return -1;
	}

	if ((ret = register_jprobe(&my_jprobe)) <0) {
		printk("register_jprobe failed, returned %d\n", ret);
		return -1;
	}
	printk("Planted jprobe at %p, handler addr %p\n",
			my_jprobe.kp.addr, my_jprobe.entry);
	return 0;
}

void cleanup_module(void)
{
	unregister_jprobe(&my_jprobe);
	printk("jprobe unregistered\n");
}

MODULE_LICENSE("GPL");

