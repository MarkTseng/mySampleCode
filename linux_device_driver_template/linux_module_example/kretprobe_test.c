/*kretprobe-example.c*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/kallsyms.h>

static const char *probed_func = "sys_open";

/* Return-probe handler: If the probed function fails, log the return value. */
static int ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
	// Substitute the appropriate register name for your architecture --
	// e.g., regs->rax for x86_64, regs->gpr[3] for ppc64.
	int retval = (int) regs->uregs[0];
	if (retval < 0) {
		printk("%s returns %d\n", probed_func, retval);
	}
	return 0;
}

static struct kretprobe my_kretprobe = {
	.handler = ret_handler,
	/* Probe up to 20 instances concurrently. */
	.maxactive = 20
};

int init_module(void)
{
	int ret;
	my_kretprobe.kp.addr =
		(kprobe_opcode_t *) kallsyms_lookup_name(probed_func);
	if (!my_kretprobe.kp.addr) {
		printk("Couldn't find %s to plant return probe\n", probed_func);
		return -1;
	}
	if ((ret = register_kretprobe(&my_kretprobe)) < 0) {
		printk("register_kretprobe failed, returned %d\n", ret);
		return -1;
	}
	printk("Planted return probe at %p\n", my_kretprobe.kp.addr);
	return 0;
}

void cleanup_module(void)
{
	unregister_kretprobe(&my_kretprobe);
	printk("kretprobe unregistered\n");
	/* nmissed > 0 suggests that maxactive was set too low. */
	printk("Missed probing %d instances of %s\n",
			my_kretprobe.nmissed, probed_func);
}

MODULE_LICENSE("GPL");

