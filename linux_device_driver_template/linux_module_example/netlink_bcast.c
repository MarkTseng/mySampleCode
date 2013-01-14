#include <linux/module.h>  
#include <linux/kernel.h> 
#include <linux/init.h> 
#include <net/sock.h>
#include <net/netlink.h>
#include <linux/skbuff.h>

#define BROADCAST_INTERVAL 3
static struct sock *netlink_sock;
static struct timer_list	broadcast_timer;
static void udp_broadcast(int gid,void *payload)
{
	struct sk_buff	*skb;
	struct nlmsghdr	*nlh;
	int size=strlen(payload)+1;
	int		len = NLMSG_SPACE(size);
	void		*data;
	int ret;
	
	skb = alloc_skb(len, GFP_KERNEL);
	if (!skb)
		return;
	nlh= NLMSG_PUT(skb, 0, 0, 0, size);
	nlh->nlmsg_flags = 0;
	data=NLMSG_DATA(nlh);
	memcpy(data, payload, size);
	NETLINK_CB(skb).pid = 0;         /* from kernel */
	NETLINK_CB(skb).dst_group = gid;  /* unicast */
	//ret=netlink_unicast(netlink_sock, skb, pid, MSG_DONTWAIT);
	ret=netlink_broadcast(netlink_sock, skb, 0, gid, GFP_KERNEL);

	if (ret <0)
	{
		printk("send failed\n");
		return;
	}
	return;
	
nlmsg_failure:			/* Used by NLMSG_PUT */
	if (skb)
		kfree_skb(skb);
}
void MyTimerFunction(unsigned long data)
{
	
	udp_broadcast(1,"Hello World\n");
	init_timer(&broadcast_timer);
	broadcast_timer.function = MyTimerFunction;
	broadcast_timer.expires = jiffies + BROADCAST_INTERVAL*HZ;
	add_timer(&broadcast_timer);
}
static void udp_receive(struct sk_buff  *skb)
{

}
static int __init kudp_init(void)
{
	netlink_sock = netlink_kernel_create(&init_net, NETLINK_USERSOCK, 0,udp_receive, NULL, THIS_MODULE);
	init_timer(&broadcast_timer);
	broadcast_timer.function = MyTimerFunction;
	broadcast_timer.expires = jiffies + BROADCAST_INTERVAL;
	add_timer(&broadcast_timer);
	return 0;
}

static void __exit kudp_exit(void)
{

	netlink_kernel_release(netlink_sock);
	del_timer(&broadcast_timer);
	printk("netlink driver remove successfully\n");
}
module_init(kudp_init);
module_exit(kudp_exit);

MODULE_DESCRIPTION("kudp broadcast server module");
MODULE_AUTHOR("Joey Cheng<jemicheng@gmail.com>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("QT2410:kudp broadcast server module");

