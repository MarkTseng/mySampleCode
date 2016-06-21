/* 
* Dist/Platform Kernel Version:
*     Ubuntu 14.04(Linux Mint 17): 3.13
*     CentOS 7: 3.10
*     Cavium AC: 3.10
*     Ubuntu 12.04(Linux Mint 13): 3.2
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/netlink.h>
#include <net/sock.h>
#include <net/net_namespace.h>
#include <linux/version.h>
 
#define NETLINK_TEST 18
//#define MAX_PAYLOAD 1024
//#define MAX_PAYLOAD 2048
//#define MAX_PAYLOAD 4096
#define MAX_PAYLOAD 8192
 
MODULE_LICENSE("GPL");
 
static struct sock *nl_sk = NULL;
 
int netlinkSayHello(int pid)
{
    struct sk_buff *skb;
    struct nlmsghdr *nlh;
 
    skb = nlmsg_new(MAX_PAYLOAD, GFP_ATOMIC);
    if (skb == NULL)
    {
        printk(KERN_ERR "Failed to alloc skb\n");
        return 0;
    }
 
    // put into skb
    nlh = nlmsg_put(skb, 0, 0, 0, MAX_PAYLOAD, 0);
 
    // below line is meaningless
    memcpy(NLMSG_DATA(nlh), "Hello Client", sizeof("Hello Client"));
 
    if (netlink_unicast(nl_sk, skb, pid, 0) < 0)
    {
        printk(KERN_ERR"Failed to unicast skb\n");
        return 0;
    }
 
    return 1;
}
 
static void nl_data_ready(struct sk_buff *skb)
{
    struct nlmsghdr *nlh = NULL;
 
    if (skb == NULL)
    {
        printk(KERN_INFO"skb is NULL\n");
        return;
    }
 
    nlh = (struct nlmsghdr *)skb->data;
 
    netlinkSayHello(nlh->nlmsg_pid);
}
 
static void netlink_test(void)
{
/* for Cavium AC/CentOS 7/Ubuntu 14.04/Linux Mint 17 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 7, 0)
    struct netlink_kernel_cfg cfg = {
        .input = nl_data_ready,
    };
    nl_sk = netlink_kernel_create(&init_net, NETLINK_TEST, &cfg);
 
/* for Ubuntu 12.04/Linux Mint 13 */
//#elif LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
#else
    nl_sk = netlink_kernel_create(&init_net,
                                  NETLINK_TEST,
                                  0,
                                  nl_data_ready,
                                  NULL,
                                  THIS_MODULE);
#endif
 
    if (nl_sk == NULL)
    {
        printk(KERN_ERR "Failed to create netlink socket\n");
    }
}
 
static int __init my_module_init(void)
{
    printk(KERN_INFO "Initializing Netlink Socket\n");
    netlink_test();
 
    return 0;
}
 
static void __exit my_module_exit(void)
{
    netlink_kernel_release(nl_sk);
    printk(KERN_INFO "Goodbye\n");
}
 
module_init(my_module_init);
module_exit(my_module_exit);

