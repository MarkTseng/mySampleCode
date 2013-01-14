/*
 * snull.c --  the Simple Network Utility
 *
 * Copyright (C) 2001 Alessandro Rubini and Jonathan Corbet
 * Copyright (C) 2001 O'Reilly & Associates
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.  The citation
 * should list that the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates.   No warranty is attached;
 * we cannot take responsibility for errors or fitness for use.
 *
 * $Id: snull.c,v 1.17 2001/07/18 22:28:18 rubini Exp $
 */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/config.h>
#include <linux/module.h>

#include <linux/sched.h>
#include <linux/kernel.h> /* printk() */
#include <linux/malloc.h> /* kmalloc() */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/interrupt.h> /* mark_bh */

#include <linux/in.h>
#include <linux/netdevice.h>   /* struct device, and other headers */
#include <linux/etherdevice.h> /* eth_type_trans */
#include <linux/ip.h>          /* struct iphdr */
#include <linux/tcp.h>         /* struct tcphdr */
#include <linux/skbuff.h>

#include "snull.h"

#ifdef LINUX_20
#  include <linux/if_ether.h>
#  define net_device_stats enet_statistics
#else
#  include <linux/in6.h>
#endif
#include <asm/checksum.h>

MODULE_AUTHOR("Alessandro Rubini");

/* This is a load-time options */
static int eth = 0; /* Call yourself "ethX". Default is "sn0"/"sn1" */
MODULE_PARM(eth, "i");

/*
 * Transmitter lockup simulation, normally disabled.
 */
static int lockup = 0;
MODULE_PARM(lockup, "i");

#ifdef HAVE_TX_TIMEOUT
static int timeout = SNULL_TIMEOUT;
MODULE_PARM(timeout, "i");
#endif

int snull_eth;


/*
 * This structure is private to each device. It is used to pass
 * packets in and out, so there is place for a packet
 */

struct snull_priv {
    struct net_device_stats stats;
    int status;
    int rx_packetlen;
    u8 *rx_packetdata;
    int tx_packetlen;
    u8 *tx_packetdata;
    struct sk_buff *skb;
    spinlock_t lock;
};

extern struct net_device snull_devs[];
void snull_tx_timeout (struct net_device *dev);
        

/*
 * Open and close
 */

int snull_open(struct net_device *dev)
{
    MOD_INC_USE_COUNT;
    
    /* request_region(), request_irq(), ....  (like fops->open) */
#if 0 && defined(LINUX_20)
    /*
     * We have no irq line, otherwise this assignment can be used to
     * grab a non-shared interrupt. To share interrupt lines use
     * the dev_id argument of request_irq. See snull_interrupt below.
     */
    irq2dev_map[dev->irq] = dev;
#endif

    /* 
     * Assign the hardware address of the board: use "\0SNULx", where
     * x is 0 or 1. The first byte is '\0' to avoid being a multicast
     * address (the first byte of multicast addrs is odd).
     */
    memcpy(dev->dev_addr, "\0SNUL0", ETH_ALEN);
    dev->dev_addr[ETH_ALEN-1] += (dev - snull_devs); /* the number */

#ifndef LINUX_24                
    dev->start = 1;             
#endif                          
    netif_start_queue(dev);
    return 0;
}

int snull_release(struct net_device *dev)
{
    /* release ports, irq and such -- like fops->close */

    netif_stop_queue(dev); /* can't transmit any more */
#ifndef LINUX_24                
    dev->start = 0;             
#endif                          
    MOD_DEC_USE_COUNT;
    /* if irq2dev_map was used (2.0 kernel), zero the entry here */
    return 0;
}

/*
 * Configuration changes (passed on by ifconfig)
 */
int snull_config(struct net_device *dev, struct ifmap *map)
{
    if (dev->flags & IFF_UP) /* can't act on a running interface */
        return -EBUSY;

    /* Don't allow changing the I/O address */
    if (map->base_addr != dev->base_addr) {
        printk(KERN_WARNING "snull: Can't change I/O address\n");
        return -EOPNOTSUPP;
    }

    /* Allow changing the IRQ */
    if (map->irq != dev->irq) {
        dev->irq = map->irq;
        /* request_irq() is delayed to open-time */
    }

    /* ignore other fields */
    return 0;
}

/*
 * Receive a packet: retrieve, encapsulate and pass over to upper levels
 */
void snull_rx(struct net_device *dev, int len, unsigned char *buf)
{
    struct sk_buff *skb;
    struct snull_priv *priv = (struct snull_priv *) dev->priv;

    /*
     * The packet has been retrieved from the transmission
     * medium. Build an skb around it, so upper layers can handle it
     */
    skb = dev_alloc_skb(len+2);
    if (!skb) {
        printk("snull rx: low on mem - packet dropped\n");
        priv->stats.rx_dropped++;
        return;
    }
    skb_reserve(skb, 2); /* align IP on 16B boundary */  
    memcpy(skb_put(skb, len), buf, len);

    /* Write metadata, and then pass to the receive level */
    skb->dev = dev;
    skb->protocol = eth_type_trans(skb, dev);
    skb->ip_summed = CHECKSUM_UNNECESSARY; /* don't check it */
    priv->stats.rx_packets++;
#ifndef LINUX_20                        
    priv->stats.rx_bytes += len;
#endif                                  
    netif_rx(skb);
    return;
}
    
        
/*
 * The typical interrupt entry point
 */
#ifdef LINUX_24
void snull_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
    int statusword;
    struct snull_priv *priv;
    /*
     * As usual, check the "device" pointer for shared handlers.
     * Then assign "struct device *dev"
     */
    struct net_device *dev = (struct net_device *)dev_id;
    /* ... and check with hw if it's really ours */

    if (!dev /*paranoid*/ ) return;

    /* Lock the device */
    priv = (struct snull_priv *) dev->priv;
    spin_lock(&priv->lock);

    /* retrieve statusword: real netdevices use I/O instructions */
    statusword = priv->status;
    if (statusword & SNULL_RX_INTR) {
        /* send it to snull_rx for handling */
        snull_rx(dev, priv->rx_packetlen, priv->rx_packetdata);
    }
    if (statusword & SNULL_TX_INTR) {
        /* a transmission is over: free the skb */
        priv->stats.tx_packets++;
        priv->stats.tx_bytes += priv->tx_packetlen;
        dev_kfree_skb(priv->skb);
    }

    /* Unlock the device and we are done */
    spin_unlock(&priv->lock);
    return;
}
#else /* LINUX_22 or earlier */

void snull_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
    int statusword;
    struct snull_priv *priv;
    /*
     * As usual, check the "device" pointer for shared handlers.
     * Then assign "struct device *dev"
     */
#if 0 && defined(LINUX_20)
    /* This is the way to do things for non-shared handlers */
    struct device *dev = (struct device *)(irq2dev_map[irq]);
#else
    /* Otherwise use this SA_SHIRQ-safe approach */
    struct net_device *dev = (struct net_device *)dev_id;
    /* ... and check with hw if it's really ours */
#endif

    if (!dev /*paranoid*/ ) return;

    /* Lock the device */
    priv = (struct snull_priv *) dev->priv;
    spin_lock(&priv->lock);
    dev->interrupt = 1;

    /* retrieve statusword: real netdevices use inb() or inw() */
    statusword = priv->status;
    if (statusword & SNULL_RX_INTR) {
        /* send it to snull_rx for handling */
        snull_rx(dev, priv->rx_packetlen, priv->rx_packetdata);
    }
    if (statusword & SNULL_TX_INTR) {
        /* a transmission is over: tell we are no more busy */
        priv->stats.tx_packets++;
#ifndef LINUX_20
        priv->stats.tx_bytes += priv->tx_packetlen;
#endif
        dev_kfree_skb(priv->skb);
    }
    spin_unlock(&priv->lock);
    dev->interrupt = 0;
    return;
}

#endif /* LINUX_22 or earlier */


/*
 * Transmit a packet (low level interface)
 */
void snull_hw_tx(char *buf, int len, struct net_device *dev)
{
    /*
     * This function deals with hw details. This interface loops
     * back the packet to the other snull interface (if any).
     * In other words, this function implements the snull behaviour,
     * while all other procedures are rather device-independent
     */
    struct iphdr *ih;
    struct net_device *dest;
    struct snull_priv *priv;
    u32 *saddr, *daddr;

    /* I am paranoid. Ain't I? */
    if (len < sizeof(struct ethhdr) + sizeof(struct iphdr)) {
        printk("snull: Hmm... packet too short (%i octets)\n",
               len);
        return;
    }

    if (0) { /* enable this conditional to look at the data */
        int i;
        PDEBUG("len is %i\n" KERN_DEBUG "data:",len);
        for (i=14 ; i<len; i++)
            printk(" %02x",buf[i]&0xff);
        printk("\n");
    }
    /*
     * Ethhdr is 14 bytes, but the kernel arranges for iphdr
     * to be aligned (i.e., ethhdr is unaligned)
     */
    ih = (struct iphdr *)(buf+sizeof(struct ethhdr));
    saddr = &ih->saddr;
    daddr = &ih->daddr;

    ((u8 *)saddr)[2] ^= 1; /* change the third octet (class C) */
    ((u8 *)daddr)[2] ^= 1;

    ih->check = 0;         /* and rebuild the checksum (ip needs it) */
    ih->check = ip_fast_csum((unsigned char *)ih,ih->ihl);

    if (dev == snull_devs)
        PDEBUGG("%08x:%05i --> %08x:%05i\n",
               ntohl(ih->saddr),ntohs(((struct tcphdr *)(ih+1))->source),
               ntohl(ih->daddr),ntohs(((struct tcphdr *)(ih+1))->dest));
    else
        PDEBUGG("%08x:%05i <-- %08x:%05i\n",
               ntohl(ih->daddr),ntohs(((struct tcphdr *)(ih+1))->dest),
               ntohl(ih->saddr),ntohs(((struct tcphdr *)(ih+1))->source));

    /*
     * Ok, now the packet is ready for transmission: first simulate a
     * receive interrupt on the twin device, then  a
     * transmission-done on the transmitting device
     */
    dest = snull_devs + (dev==snull_devs ? 1 : 0);
    priv = (struct snull_priv *) dest->priv;
    priv->status = SNULL_RX_INTR;
    priv->rx_packetlen = len;
    priv->rx_packetdata = buf;
    snull_interrupt(0, dest, NULL);

    priv = (struct snull_priv *) dev->priv;
    priv->status = SNULL_TX_INTR;
    priv->tx_packetlen = len;
    priv->tx_packetdata = buf;
    if (lockup && ((priv->stats.tx_packets + 1) % lockup) == 0) {
        /* Simulate a dropped transmit interrupt */
        netif_stop_queue(dev);
        PDEBUG("Simulate lockup at %ld, txp %ld\n", jiffies,
                        (unsigned long) priv->stats.tx_packets);
    }
    else
        snull_interrupt(0, dev, NULL);
}

/*
 * Transmit a packet (called by the kernel)
 */
int snull_tx(struct sk_buff *skb, struct net_device *dev)
{
    int len;
    char *data;
    struct snull_priv *priv = (struct snull_priv *) dev->priv;

#ifndef LINUX_24
    if (dev->tbusy || skb == NULL) {
        PDEBUG("tint for %p, tbusy %ld, skb %p\n", dev, dev->tbusy, skb);
        snull_tx_timeout (dev);
        if (skb == NULL)
            return 0;
    }
#endif

    len = skb->len < ETH_ZLEN ? ETH_ZLEN : skb->len;
    data = skb->data;
    dev->trans_start = jiffies; /* save the timestamp */

    /* Remember the skb, so we can free it at interrupt time */
    priv->skb = skb;

    /* actual deliver of data is device-specific, and not shown here */
    snull_hw_tx(data, len, dev);

    return 0; /* Our simple device can not fail */
}

/*
 * Deal with a transmit timeout.
 */
void snull_tx_timeout (struct net_device *dev)
{
    struct snull_priv *priv = (struct snull_priv *) dev->priv;

    PDEBUG("Transmit timeout at %ld, latency %ld\n", jiffies,
                    jiffies - dev->trans_start);
    priv->status = SNULL_TX_INTR;
    snull_interrupt(0, dev, NULL);
    priv->stats.tx_errors++;
    netif_wake_queue(dev);
    return;
}



/*
 * Ioctl commands 
 */
int snull_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
 
    PDEBUG("ioctl\n");
    return 0;
}

/*
 * Return statistics to the caller
 */
struct net_device_stats *snull_stats(struct net_device *dev)
{
    struct snull_priv *priv = (struct snull_priv *) dev->priv;
    return &priv->stats;
}

/*
 * This function is called to fill up an eth header, since arp is not
 * available on the interface
 */
#ifndef LINUX_20
int snull_rebuild_header(struct sk_buff *skb)
{
    struct ethhdr *eth = (struct ethhdr *) skb->data;
    struct net_device *dev = skb->dev;
    
    memcpy(eth->h_source, dev->dev_addr, dev->addr_len);
    memcpy(eth->h_dest, dev->dev_addr, dev->addr_len);
    eth->h_dest[ETH_ALEN-1]   ^= 0x01;   /* dest is us xor 1 */
    return 0;
}
#else /* LINUX_20 */
int snull_rebuild_header(void *buff, struct net_device *dev, unsigned long dst,
                    struct sk_buff *skb)
{
    struct ethhdr *eth = (struct ethhdr *)buff;

    memcpy(eth->h_source, dev->dev_addr, dev->addr_len);
    memcpy(eth->h_dest, dev->dev_addr, dev->addr_len);
    eth->h_dest[ETH_ALEN-1]   ^= 0x01;   /* dest is us xor 1 */
    return 0;
}
#endif /* LINUX_20 */


int snull_header(struct sk_buff *skb, struct net_device *dev,
                unsigned short type, void *daddr, void *saddr,
                unsigned int len)
{
    struct ethhdr *eth = (struct ethhdr *)skb_push(skb,ETH_HLEN);

    eth->h_proto = htons(type);
    memcpy(eth->h_source, saddr ? saddr : dev->dev_addr, dev->addr_len);
    memcpy(eth->h_dest,   daddr ? daddr : dev->dev_addr, dev->addr_len);
    eth->h_dest[ETH_ALEN-1]   ^= 0x01;   /* dest is us xor 1 */
    return (dev->hard_header_len);
}





/*
 * The "change_mtu" method is usually not needed.
 * If you need it, it must be like this.
 */
int snull_change_mtu(struct net_device *dev, int new_mtu)
{
    unsigned long flags;
    spinlock_t *lock = &((struct snull_priv *) dev->priv)->lock;
    
    /* check ranges */
    if ((new_mtu < 68) || (new_mtu > 1500))
        return -EINVAL;
    /*
     * Do anything you need, and the accept the value
     */
    spin_lock_irqsave(lock, flags);
    dev->mtu = new_mtu;
    spin_unlock_irqrestore(lock, flags);
    return 0; /* success */
}

/*
 * The init function (sometimes called probe).
 * It is invoked by register_netdev()
 */
int snull_init(struct net_device *dev)
{
#if 0
    /*
     * Make the usual checks: check_region(), probe irq, ...  -ENODEV
     * should be returned if no device found.  No resource should be
     * grabbed: this is done on open(). 
     */
#endif

    /* 
     * Then, assign other fields in dev, using ether_setup() and some
     * hand assignments
     */
    ether_setup(dev); /* assign some of the fields */

    dev->open            = snull_open;
    dev->stop            = snull_release;
    dev->set_config      = snull_config;
    dev->hard_start_xmit = snull_tx;
    dev->do_ioctl        = snull_ioctl;
    dev->get_stats       = snull_stats;
    dev->change_mtu      = snull_change_mtu;  
    dev->rebuild_header  = snull_rebuild_header;
    dev->hard_header     = snull_header;
#ifdef HAVE_TX_TIMEOUT
    dev->tx_timeout     = snull_tx_timeout;
    dev->watchdog_timeo = timeout;
#endif
    /* keep the default flags, just add NOARP */
    dev->flags           |= IFF_NOARP;
#ifndef LINUX_20                        
    dev->hard_header_cache = NULL;      /* Disable caching */
#endif                                  
    SET_MODULE_OWNER(dev);

    /*
     * Then, allocate the priv field. This encloses the statistics
     * and a few private fields.
     */
    dev->priv = kmalloc(sizeof(struct snull_priv), GFP_KERNEL);
    if (dev->priv == NULL)
        return -ENOMEM;
    memset(dev->priv, 0, sizeof(struct snull_priv));
    spin_lock_init(& ((struct snull_priv *) dev->priv)->lock);
    return 0;
}

/*
 * The devices
 */

#ifdef LINUX_24
struct net_device snull_devs[2] = {
    { init: snull_init, },  /* init, nothing more */
    { init: snull_init, }
};
#else /* pre-2.4 */
char snull_names[16];

struct net_device snull_devs[2] = {
    {
        name: snull_names,
        init: snull_init,  /* init function */
    },
    {
        name: snull_names+8,
        init: snull_init,            /* init function */
    }
};
#endif /* LINUX_24 */



/*
 * Finally, the module stuff
 */

int snull_init_module(void)
{

    int result, i, device_present = 0;

    snull_eth = eth; /* copy the cfg datum in the non-static place */

    if (!snull_eth) { /* call them "sn0" and "sn1" */
        strcpy(snull_devs[0].name, "sn0");
        strcpy(snull_devs[1].name, "sn1");
    } else { /* use automatic assignment */
#ifdef LINUX_24                                                 
        strcpy(snull_devs[0].name, "eth%d");
        strcpy(snull_devs[1].name, "eth%d");
#else                                                           
        snull_devs[0].name[0] = snull_devs[1].name[0] = ' ';    
#endif                                                          
    }

    for (i=0; i<2;  i++)
        if ( (result = register_netdev(snull_devs + i)) )
            printk("snull: error %i registering device \"%s\"\n",
                   result, snull_devs[i].name);
        else device_present++;
#ifndef SNULL_DEBUG
    EXPORT_NO_SYMBOLS;
#endif

    return device_present ? 0 : -ENODEV;
}

void snull_cleanup(void)
{
    int i;
   
    for (i=0; i<2;  i++) {
        kfree(snull_devs[i].priv);
        unregister_netdev(snull_devs + i);
    }
    return;
}


module_init(snull_init_module);
module_exit(snull_cleanup);
