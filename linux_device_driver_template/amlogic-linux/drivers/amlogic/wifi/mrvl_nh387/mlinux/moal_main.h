/** @file moal_main.h
  *
  * @brief This file contains wlan driver specific defines etc.
  * 
  * Copyright (C) 2008-2011, Marvell International Ltd.  
  *
  * This software file (the "File") is distributed by Marvell International 
  * Ltd. under the terms of the GNU General Public License Version 2, June 1991 
  * (the "License").  You may use, redistribute and/or modify this File in 
  * accordance with the terms and conditions of the License, a copy of which 
  * is available by writing to the Free Software Foundation, Inc.,
  * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA or on the
  * worldwide web at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
  *
  * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE 
  * IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE 
  * ARE EXPRESSLY DISCLAIMED.  The License provides additional details about 
  * this warranty disclaimer.
  *
  */

/********************************************************
Change log:
    10/21/2008: initial version
********************************************************/

#ifndef _MOAL_MAIN_H
#define _MOAL_MAIN_H

/* warnfix for FS redefination if any? */
#ifdef FS
#undef FS
#endif

/* Linux header files */
#include        <linux/kernel.h>
#include        <linux/module.h>
#include        <linux/init.h>
#include        <linux/version.h>
#include        <linux/param.h>
#include        <linux/delay.h>
#include        <linux/slab.h>
#include        <linux/mm.h>
#include        <linux/types.h>
#include        <linux/sched.h>
#include        <linux/timer.h>
#include        <linux/ioport.h>
#include        <linux/pci.h>
#include        <linux/ctype.h>
#include        <linux/proc_fs.h>
#include        <linux/vmalloc.h>
#include	<linux/ptrace.h>
#include	<linux/string.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
#include       <linux/config.h>
#endif

/* ASM files */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
#include        <linux/semaphore.h>
#else
#include        <asm/semaphore.h>
#endif
#include        <asm/byteorder.h>
#include        <asm/irq.h>
#include        <asm/uaccess.h>
#include        <asm/io.h>
#include        <asm/system.h>

/* Net header files */
#include        <linux/wireless.h>
#include        <linux/netdevice.h>
#include        <linux/net.h>
#include        <linux/inet.h>
#include        <linux/ip.h>
#include        <linux/skbuff.h>
#include        <linux/if_arp.h>
#include        <linux/if_ether.h>
#include        <linux/etherdevice.h>
#include        <net/sock.h>
#include        <net/arp.h>
#include        <linux/rtnetlink.h>

#include	<linux/firmware.h>

/* Wireless header */
#include        <net/iw_handler.h>
#include        "mlan.h"
#include 	"moal_shim.h"
#ifdef STA_SUPPORT
#include	"moal_wext.h"
#include	"moal_priv.h"
#endif

/** Define BOOLEAN */
typedef t_u8 BOOLEAN;

/** Driver version */
extern const char driver_version[];

/** Private structure for MOAL */
typedef struct _moal_private moal_private;
/** Handle data structure for MOAL  */
typedef struct _moal_handle moal_handle;

/** Hardware status codes */
typedef enum _MOAL_HARDWARE_STATUS
{
    HardwareStatusReady,
    HardwareStatusInitializing,
    HardwareStatusFwReady,
    HardwareStatusReset,
    HardwareStatusClosing,
    HardwareStatusNotReady
} MOAL_HARDWARE_STATUS;

/** moal_wait_option */
enum
{
    MOAL_NO_WAIT,
    MOAL_IOCTL_WAIT,
    MOAL_CMD_WAIT,
    MOAL_PROC_WAIT,
    MOAL_WSTATS_WAIT
};

/** moal_main_state */
enum
{
    MOAL_STATE_IDLE,
    MOAL_RECV_INT,
    MOAL_ENTER_WORK_QUEUE,
    MOAL_START_MAIN_PROCESS,
    MOAL_END_MAIN_PROCESS
};

/** HostCmd_Header */
typedef struct _HostCmd_Header
{
    /** Command */
    t_u16 command;
    /** Size */
    t_u16 size;
} HostCmd_Header;

#ifndef MIN
/** Find minimum */
#define MIN(a,b)		((a) < (b) ? (a) : (b))
#endif

/*
 * OS timer specific
 */

/** Timer structure */
typedef struct _moal_drv_timer
{
        /** Timer list */
    struct timer_list tl;
        /** Timer function */
    void (*timer_function) (void *context);
        /** Timer function context */
    void *function_context;
        /** Time period */
    t_u32 time_period;
        /** Is timer periodic ? */
    t_u32 timer_is_periodic;
        /** Is timer cancelled ? */
    t_u32 timer_is_canceled;
} moal_drv_timer, *pmoal_drv_timer;

/** 
 *  @brief Timer handler
 *  
 *  @param fcontext	Timer context
 *
 *  @return		N/A
 */
static inline void
woal_timer_handler(unsigned long fcontext)
{
    pmoal_drv_timer timer = (pmoal_drv_timer) fcontext;

    timer->timer_function(timer->function_context);

    if (timer->timer_is_periodic == MTRUE) {
        mod_timer(&timer->tl, jiffies + ((timer->time_period * HZ) / 1000));
    } else {
        timer->timer_is_canceled = MTRUE;
        timer->time_period = 0;
    }
}

/** 
 *  @brief Initialize timer
 *  
 *  @param timer		Timer structure
 *  @param TimerFunction	Timer function
 *  @param FunctionContext	Timer function context
 *
 *  @return			N/A
 */
static inline void
woal_initialize_timer(pmoal_drv_timer timer,
                      void (*TimerFunction) (void *context),
                      void *FunctionContext)
{
    /* First, setup the timer to trigger the wlan_timer_handler proxy */
    init_timer(&timer->tl);
    timer->tl.function = woal_timer_handler;
    timer->tl.data = (t_u32) timer;

    /* Then tell the proxy which function to call and what to pass it */
    timer->timer_function = TimerFunction;
    timer->function_context = FunctionContext;
    timer->timer_is_canceled = MTRUE;
    timer->time_period = 0;
    timer->timer_is_periodic = MFALSE;
}

/** 
 *  @brief Modify timer
 *  
 *  @param timer		Timer structure
 *  @param MillisecondPeriod	Time period in millisecond
 *
 *  @return			N/A
 */
static inline void
woal_mod_timer(pmoal_drv_timer timer, t_u32 MillisecondPeriod)
{
    timer->time_period = MillisecondPeriod;
    mod_timer(&timer->tl, jiffies + (MillisecondPeriod * HZ) / 1000);
    timer->timer_is_canceled = MFALSE;
}

/** 
 *  @brief Cancel timer
 *  
 *  @param timer	Timer structure
 *
 *  @return		N/A
 */
static inline void
woal_cancel_timer(moal_drv_timer * timer)
{
    del_timer(&timer->tl);
    timer->timer_is_canceled = MTRUE;
    timer->time_period = 0;
}

#ifdef REASSOCIATION
/*
 * OS Thread Specific
 */

#include	<linux/kthread.h>

/** Kernel thread structure */
typedef struct _moal_thread
{
    /** Task control structrue */
    struct task_struct *task;
    /** Pointer to wait_queue_head */
    wait_queue_head_t wait_q;
    /** PID */
    pid_t pid;
    /** Pointer to moal_handle */
    void *handle;
} moal_thread;

/** 
 *  @brief Activate thread
 *  
 *  @param thr			Thread structure
 *  @return			N/A
 */
static inline void
woal_activate_thread(moal_thread * thr)
{
    /** Record the thread pid */
    thr->pid = current->pid;

    /** Initialize the wait queue */
    init_waitqueue_head(&thr->wait_q);
}

/** 
 *  @brief De-activate thread
 *  
 *  @param thr			Thread structure
 *  @return			N/A
 */
static inline void
woal_deactivate_thread(moal_thread * thr)
{
    /* Reset the pid */
    thr->pid = 0;
}

/** 
 *  @brief Create and run the thread
 *
 *  @param threadfunc		Thread function
 *  @param thr			Thread structure
 *  @param name			Thread name
 *  @return			N/A
 */
static inline void
woal_create_thread(int (*threadfunc) (void *), moal_thread * thr, char *name)
{
    /* Create and run the thread */
    thr->task = kthread_run(threadfunc, thr, "%s", name);
}
#endif /* REASSOCIATION */

/* The following macros are neccessary to retain compatibility
 * around the workqueue chenges happened in kernels >= 2.6.20:
 * - INIT_WORK changed to take 2 arguments and let the work function
 *   get its own data through the container_of macro
 * - delayed works have been split from normal works to save some
 *   memory usage in struct work_struct
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
/** Work_queue work initialization */
#define MLAN_INIT_WORK(_work, _fun)                 INIT_WORK(_work, ((void (*)(void *))_fun), _work)
/** Work_queue delayed work initialization */
#define MLAN_INIT_DELAYED_WORK(_work, _fun)         INIT_WORK(_work, ((void (*)(void *))_fun), _work)
/** Work_queue container parameter */
#define MLAN_DELAYED_CONTAINER_OF(_ptr, _type, _m)  container_of(_ptr, _type, _m)
#else /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20) */
/** Work_queue work initialization */
#define MLAN_INIT_WORK                              INIT_WORK
/** Work_queue delayed work initialization */
#define MLAN_INIT_DELAYED_WORK                      INIT_DELAYED_WORK
/** Work_queue container parameter */
#define MLAN_DELAYED_CONTAINER_OF(_ptr, _type, _m)  container_of(_ptr, _type, _m.work)
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20) */

/** 
 *  @brief Schedule timeout
 *  
 *  @param millisec	Timeout duration in milli second
 *
 *  @return		N/A
 */
static inline void
woal_sched_timeout(t_u32 millisec)
{
    set_current_state(TASK_INTERRUPTIBLE);

    schedule_timeout((millisec * HZ) / 1000);
}

#ifndef __ATTRIB_ALIGN__
#define __ATTRIB_ALIGN__ __attribute__((aligned(4)))
#endif

#ifndef __ATTRIB_PACK__
#define __ATTRIB_PACK__ __attribute__ ((packed))
#endif

/** Get module */
#define MODULE_GET	try_module_get(THIS_MODULE)
/** Put module */
#define MODULE_PUT	module_put(THIS_MODULE)

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37)
/** Initialize semaphore */
#define MOAL_INIT_SEMAPHORE(x)    	init_MUTEX(x)
/** Initialize semaphore */
#define MOAL_INIT_SEMAPHORE_LOCKED(x) 	init_MUTEX_LOCKED(x)
#else
/** Initialize semaphore */
#define MOAL_INIT_SEMAPHORE(x)    	sema_init(x, 1)
/** Initialize semaphore */
#define MOAL_INIT_SEMAPHORE_LOCKED(x) 	sema_init(x, 0)
#endif
/** Acquire semaphore and with blocking */
#define MOAL_ACQ_SEMAPHORE_BLOCK(x)	down_interruptible(x)
/** Acquire semaphore without blocking */
#define MOAL_ACQ_SEMAPHORE_NOBLOCK(x)	down_trylock(x)
/** Release semaphore */
#define MOAL_REL_SEMAPHORE(x) 		up(x)

/** Default watchdog timeout */
#define MRVDRV_DEFAULT_WATCHDOG_TIMEOUT (5 * HZ)

#ifdef UAP_SUPPORT
/** Default watchdog timeout 
    Increase the value to avoid kernel Tx timeout message in case 
    station in PS mode or left.
    The default value of PS station ageout timer is 40 seconds. 
    Hence, the watchdog timer is set to a value higher than it.
*/
#define MRVDRV_DEFAULT_UAP_WATCHDOG_TIMEOUT (41 * HZ)
#endif

/** 10 seconds */
#define MOAL_TIMER_10S                10000
/** 5 seconds */
#define MOAL_TIMER_5S                 5000
/** 1 second */
#define MOAL_TIMER_1S                 1000

#define REASSOC_TIMER_DEFAULT         500

/** BSS number bit mask */
#define BSS_NUM_MASK		7

/** Netlink protocol number */
#define NETLINK_MARVELL     (MAX_LINKS - 1)
/** Netlink maximum payload size */
#define NL_MAX_PAYLOAD      1024
/** Netlink multicast group number */
#define NL_MULTICAST_GROUP  1

/** MAX Tx Pending count */
#define MAX_TX_PENDING    	100

/** LOW Tx Pending count */
#define LOW_TX_PENDING      80

/** wait_queue structure */
typedef struct _wait_queue
{
        /** Pointer to wait_queue_head */
    wait_queue_head_t *wait;
        /** Pointer to wait condition */
    t_u16 *condition;
        /** Start time */
    t_u32 start_time;
        /** Status from MLAN */
    mlan_status status;
} wait_queue, *pwait_queue;

/** Driver mode STA */
#define DRV_MODE_STA       0x1
/** Driver mode UAP */
#define DRV_MODE_UAP       0x2
/** Driver mode STA+UAP */
#define DRV_MODE_UAP_STA   0x3

typedef struct _moal_drv_mode
{
    /** driver mode */
    t_u16 drv_mode;
    /** total number of interfaces */
    t_u16 intf_num;
    /** attribute of bss */
    mlan_bss_attr *bss_attr;
    /** name of firmware image */
    char *fw_name;
} moal_drv_mode;

#ifdef PROC_DEBUG
/** Debug data */
struct debug_data
{
    /** Name */
    char name[32];
    /** Size */
    t_u32 size;
    /** Address */
    t_u32 addr;
};

/** Private debug data */
struct debug_data_priv
{
    /** moal_private handle */
    moal_private *priv;
    /** Debug items */
    struct debug_data *items;
    /** numbre of item */
    int num_of_items;
};
#endif

/** Maximum IP address buffer length */
#define IPADDR_MAX_BUF          20
/** IP address operation: Remove */
#define IPADDR_OP_REMOVE        0

/** Private structure for MOAL */
struct _moal_private
{
        /** Handle structure */
    moal_handle *phandle;
        /** Tx timeout count */
    t_u32 num_tx_timeout;
        /** BSS index */
    t_u8 bss_index;
        /** BSS type */
    t_u8 bss_type;
        /** BSS role */
    t_u8 bss_role;
        /** MAC address information */
    t_u8 current_addr[ETH_ALEN];
        /** Media connection status */
    BOOLEAN media_connected;
#ifdef UAP_SUPPORT
        /** uAP started or not */
    BOOLEAN bss_started;
#endif
        /** Net device pointer */
    struct net_device *netdev;
        /** Net device statistics structure */
    struct net_device_stats stats;

        /** IOCTL wait queue token */
    t_u16 ioctl_wait_q_woken;
        /** ioctl wait queue */
    wait_queue_head_t ioctl_wait_q __ATTRIB_ALIGN__;
        /** Cmd wait queue token */
    t_u16 cmd_wait_q_woken;
        /** IOCTL wait queue */
    wait_queue_head_t cmd_wait_q __ATTRIB_ALIGN__;
#ifdef CONFIG_PROC_FS
        /** Proc entry */
    struct proc_dir_entry *proc_entry;
        /** Proc entry name */
    t_s8 proc_entry_name[IFNAMSIZ];
        /** Proc wait queue token */
    t_u16 proc_wait_q_woken;
        /** IOCTL wait queue */
    wait_queue_head_t proc_wait_q __ATTRIB_ALIGN__;
#endif                          /* CONFIG_PROC_FS */
#ifdef STA_SUPPORT
        /** Nickname */
    t_u8 nick_name[16];
        /** AdHoc link sensed flag */
    BOOLEAN is_adhoc_link_sensed;
        /** Block scan flag */
    t_u8 scan_block_flag;
        /** IW statistics */
    struct iw_statistics w_stats;
        /** w_stats wait queue token */
    t_u16 w_stats_wait_q_woken;
        /** w_stats wait queue */
    wait_queue_head_t w_stats_wait_q __ATTRIB_ALIGN__;
        /** Current WEP key index */
    t_u16 current_key_index;
#ifdef REASSOCIATION
    mlan_ssid_bssid prev_ssid_bssid;
        /** Re-association required */
    BOOLEAN reassoc_required;
#endif                          /* REASSOCIATION */
        /** Rate index */
    t_u16 rate_index;
        /** Async scan semaphore */
    struct semaphore async_sem;
        /** Scan pending on blocked flag */
    t_u8 scan_pending_on_block;
        /** Report scan result */
    t_u8 report_scan_result;
        /** wpa_version */
    t_u8 wpa_version;
        /** key mgmt */
    t_u8 key_mgmt;
        /** rx_filter */
    t_u8 rx_filter;
#endif                          /* STA_SUPPORT */
#ifdef PROC_DEBUG
    /** MLAN debug info */
    struct debug_data_priv items_priv;
#endif
};

/** Handle data structure for MOAL */
struct _moal_handle
{
        /** MLAN adapter structure */
    t_void *pmlan_adapter;
        /** Private pointer */
    moal_private *priv[MLAN_MAX_BSS_NUM];
        /** Priv number */
    t_u8 priv_num;
        /** Bss attr */
    moal_drv_mode *drv_mode;
        /**  MAC address */
    t_u8 set_mac_addr;
    t_u8 mac_addr[ETH_ALEN];
#ifdef CONFIG_PROC_FS
        /** Proc top level directory entry */
    struct proc_dir_entry *proc_mwlan;
#endif
        /** Firmware */
    const struct firmware *firmware;
        /** Init config file */
    const struct firmware *user_data;
        /** Hotplug device */
    struct device *hotplug_device;
        /** STATUS variables */
    MOAL_HARDWARE_STATUS hardware_status;
        /** POWER MANAGEMENT AND PnP SUPPORT */
    BOOLEAN surprise_removed;
        /** Firmware release number */
    t_u32 fw_release_number;
        /** Init wait queue token */
    t_u16 init_wait_q_woken;
        /** Init wait queue */
    wait_queue_head_t init_wait_q __ATTRIB_ALIGN__;
#if defined(SDIO_SUSPEND_RESUME)
        /** Device suspend flag */
    BOOLEAN is_suspended;
#ifdef SDIO_SUSPEND_RESUME
        /** suspend notify flag */
    BOOLEAN suspend_notify_req;
#endif
        /** Host Sleep activated flag */
    t_u8 hs_activated;
        /** Host Sleep activated event wait queue token */
    t_u16 hs_activate_wait_q_woken;
        /** Host Sleep activated event wait queue */
    wait_queue_head_t hs_activate_wait_q __ATTRIB_ALIGN__;
#endif
        /** Card pointer */
    t_void *card;
        /** Rx pending in MLAN */
    atomic_t rx_pending;
        /** Tx packet pending count in mlan */
    atomic_t tx_pending;
        /** Low tx pending */
    t_u32 low_tx_pending;
        /** Max tx pending */
    t_u32 max_tx_pending;
        /** IOCTL pending count in mlan */
    atomic_t ioctl_pending;
        /** Malloc count */
    t_u32 malloc_count;
        /** lock count */
    t_u32 lock_count;
        /** mlan buffer alloc count */
    t_u32 mbufalloc_count;
#if defined(SDIO_SUSPEND_RESUME)
        /** hs skip count */
    t_u32 hs_skip_count;
        /** hs force count */
    t_u32 hs_force_count;
        /** suspend_fail flag */
    BOOLEAN suspend_fail;
#endif
#ifdef REASSOCIATION
        /** Re-association thread */
    moal_thread reassoc_thread;
        /** Re-association timer set flag */
    BOOLEAN is_reassoc_timer_set;
        /** Re-association timer */
    moal_drv_timer reassoc_timer __ATTRIB_ALIGN__;
        /**  */
    struct semaphore reassoc_sem;
        /** Flag of re-association on/off */
    BOOLEAN reassoc_on;
#endif                          /* REASSOCIATION */
        /** Driver workqueue */
    struct workqueue_struct *workqueue;
        /** main work */
    struct work_struct main_work;
        /** Netlink kernel socket */
    struct sock *nl_sk;
        /** Netlink kernel socket number */
    t_u32 netlink_num;
    /** w_stats wait queue token */
    t_u16 meas_wait_q_woken;
    /** w_stats wait queue */
    wait_queue_head_t meas_wait_q __ATTRIB_ALIGN__;
    /** handle index - for multiple card supports */
    t_u8 handle_idx;
        /** main state */
    t_u8 main_state;
        /** cmd52 function */
    t_u8 cmd52_func;
        /** cmd52 register */
    t_u8 cmd52_reg;
        /** cmd52 value */
    t_u8 cmd52_val;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34)
        /** spinlock to stop_queue/wake_queue*/
    spinlock_t queue_lock;
#endif
};

/** 
 *  @brief set trans_start for each TX queue. 
 *  
 *  @param dev		A pointer to net_device structure
 *
 *  @return			N/A
 */
static inline void
woal_set_trans_start(struct net_device *dev)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34)
    unsigned int i;
    for (i = 0; i < dev->num_tx_queues; i++) {
        netdev_get_tx_queue(dev, i)->trans_start = jiffies;
    }
#endif
    dev->trans_start = jiffies;
}

/** 
 *  @brief Start queue
 *  
 *  @param dev		A pointer to net_device structure
 *
 *  @return			N/A
 */
static inline void
woal_start_queue(struct net_device *dev)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,34)
    netif_start_queue(dev);
#else
    netif_tx_start_all_queues(dev);
#endif
}

/** 
 *  @brief Stop queue
 *  
 *  @param dev		A pointer to net_device structure
 *
 *  @return			N/A
 */
static inline void
woal_stop_queue(struct net_device *dev)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34)
    unsigned long flags;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    spin_lock_irqsave(&priv->phandle->queue_lock, flags);
    woal_set_trans_start(dev);
    if (!netif_queue_stopped(dev))
        netif_tx_stop_all_queues(dev);
    spin_unlock_irqrestore(&priv->phandle->queue_lock, flags);
#else
    woal_set_trans_start(dev);
    if (!netif_queue_stopped(dev))
        netif_stop_queue(dev);
#endif
}

/** 
 *  @brief wake queue
 *  
 *  @param dev		A pointer to net_device structure
 *
 *  @return			N/A
 */
static inline void
woal_wake_queue(struct net_device *dev)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34)
    unsigned long flags;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    spin_lock_irqsave(&priv->phandle->queue_lock, flags);
    if (netif_queue_stopped(dev))
        netif_tx_wake_all_queues(dev);
    spin_unlock_irqrestore(&priv->phandle->queue_lock, flags);
#else
    if (netif_queue_stopped(dev))
        netif_wake_queue(dev);
#endif
}

/** Debug Macro definition*/
#ifdef	DEBUG_LEVEL1
extern t_u32 drvdbg;
extern t_u32 ifdbg;

/** Debug message control bit definition for ifdbg */
#define MIF_D	MBIT(0)

#ifdef	DEBUG_LEVEL2
#define	PRINTM_MINFO(msg...)  do {if (drvdbg & MINFO) printk(KERN_DEBUG msg);} while(0)
#define	PRINTM_MWARN(msg...)  do {if (drvdbg & MWARN) printk(KERN_DEBUG msg);} while(0)
#define	PRINTM_MENTRY(msg...) do {if (drvdbg & MENTRY) printk(KERN_DEBUG msg);} while(0)
#else
#define	PRINTM_MINFO(msg...)  do {} while (0)
#define	PRINTM_MWARN(msg...)  do {} while (0)
#define	PRINTM_MENTRY(msg...) do {} while (0)
#endif /* DEBUG_LEVEL2 */

#define	PRINTM_MFW_D(msg...)  do {if (drvdbg & MFW_D) printk(KERN_DEBUG msg);} while(0)
#define	PRINTM_MCMD_D(msg...) do {if (drvdbg & MCMD_D) printk(KERN_DEBUG msg);} while(0)
#define	PRINTM_MDAT_D(msg...) do {if (drvdbg & MDAT_D) printk(KERN_DEBUG msg);} while(0)

#define	PRINTM_MIOCTL(msg...) do {if (drvdbg & MIOCTL) printk(KERN_DEBUG msg);} while(0)
#define	PRINTM_MINTR(msg...)  do {if (drvdbg & MINTR) printk(KERN_DEBUG msg);} while(0)
#define	PRINTM_MEVENT(msg...) do {if (drvdbg & MEVENT) printk(msg);} while(0)
#define	PRINTM_MCMND(msg...)  do {if (drvdbg & MCMND) printk(KERN_DEBUG msg);} while(0)
#define	PRINTM_MDATA(msg...)  do {if (drvdbg & MDATA) printk(KERN_DEBUG msg);} while(0)
#define	PRINTM_MERROR(msg...) do {if (drvdbg & MERROR) printk(KERN_ERR msg);} while(0)
#define	PRINTM_MFATAL(msg...) do {if (drvdbg & MFATAL) printk(KERN_ERR msg);} while(0)
#define	PRINTM_MMSG(msg...)   do {if (drvdbg & MMSG) printk(KERN_ALERT msg);} while(0)

#define	PRINTM_MIF_D(msg...) do {if (ifdbg & MIF_D) printk(KERN_DEBUG msg);} while(0)

#define	PRINTM(level,msg...) PRINTM_##level(msg)

#else

#define	PRINTM(level,msg...) do {} while (0)

#endif /* DEBUG_LEVEL1 */

/** Wait until a condition becomes true */
#define MASSERT(cond)                   \
do {                                    \
    if (!(cond)) {                      \
        PRINTM(MFATAL, "ASSERT: %s: %i\n", __FUNCTION__, __LINE__); \
        panic("Assert failed: Panic!"); \
    }                                   \
} while(0)

/** Log entry point for debugging */
#define	ENTER()			PRINTM(MENTRY, "Enter: %s\n", \
                                    __FUNCTION__)
/** Log exit point for debugging */
#define	LEAVE()			PRINTM(MENTRY, "Leave: %s\n", \
                                    __FUNCTION__)

#ifdef DEBUG_LEVEL1
#define DBG_DUMP_BUF_LEN 	64
#define MAX_DUMP_PER_LINE	16

static inline void
hexdump(char *prompt, t_u8 * buf, int len)
{
    int i;
    char dbgdumpbuf[DBG_DUMP_BUF_LEN];
    char *ptr = dbgdumpbuf;

    printk(KERN_DEBUG "%s:\n", prompt);
    for (i = 1; i <= len; i++) {
        ptr += sprintf(ptr, "%02x ", *buf);
        buf++;
        if (i % MAX_DUMP_PER_LINE == 0) {
            *ptr = 0;
            printk(KERN_DEBUG "%s\n", dbgdumpbuf);
            ptr = dbgdumpbuf;
        }
    }
    if (len % MAX_DUMP_PER_LINE) {
        *ptr = 0;
        printk(KERN_DEBUG "%s\n", dbgdumpbuf);
    }
}

#define DBG_HEXDUMP_MCMD_D(x,y,z)    do {if (drvdbg & MCMD_D) hexdump(x,y,z);} while(0)
#define DBG_HEXDUMP_MDAT_D(x,y,z)    do {if (drvdbg & MDAT_D) hexdump(x,y,z);} while(0)
#define DBG_HEXDUMP_MIF_D(x,y,z)     do {if (ifdbg & MIF_D) hexdump(x,y,z);} while(0)
#define DBG_HEXDUMP_MFW_D(x,y,z)     do {if (drvdbg & MFW_D) hexdump(x,y,z);} while(0)
#define	DBG_HEXDUMP(level,x,y,z)    DBG_HEXDUMP_##level(x,y,z)

#else
/** Do nothing since debugging is not turned on */
#define DBG_HEXDUMP(level,x,y,z)    do {} while (0)
#endif

#ifdef DEBUG_LEVEL2
#define HEXDUMP(x,y,z)              do {if (drvdbg & MINFO) hexdump(x,y,z);} while(0)
#else
/** Do nothing since debugging is not turned on */
#define HEXDUMP(x,y,z)              do {} while (0)
#endif

#ifdef BIG_ENDIAN_SUPPORT
/** Convert from 16 bit little endian format to CPU format */
#define woal_le16_to_cpu(x) le16_to_cpu(x)
/** Convert from 32 bit little endian format to CPU format */
#define woal_le32_to_cpu(x) le32_to_cpu(x)
/** Convert from 64 bit little endian format to CPU format */
#define woal_le64_to_cpu(x) le64_to_cpu(x)
/** Convert to 16 bit little endian format from CPU format */
#define woal_cpu_to_le16(x) cpu_to_le16(x)
/** Convert to 32 bit little endian format from CPU format */
#define woal_cpu_to_le32(x) cpu_to_le32(x)
/** Convert to 64 bit little endian format from CPU format */
#define woal_cpu_to_le64(x) cpu_to_le64(x)
#else
/** Do nothing */
#define woal_le16_to_cpu(x) x
/** Do nothing */
#define woal_le32_to_cpu(x) x
/** Do nothing */
#define woal_le64_to_cpu(x) x
/** Do nothing */
#define woal_cpu_to_le16(x) x
/** Do nothing */
#define woal_cpu_to_le32(x) x
/** Do nothing */
#define woal_cpu_to_le64(x) x
#endif

/** 
 *  @brief This function returns first available priv
 *  based on the BSS role
 *  
 *  @param handle    A pointer to moal_handle
 *  @param bss_role  BSS role or MLAN_BSS_ROLE_ANY
 *
 *  @return          Pointer to moal_private
 */
static inline moal_private *
woal_get_priv(moal_handle * handle, mlan_bss_role bss_role)
{
    int i;

    for (i = 0; i < handle->priv_num; i++) {
        if (handle->priv[i]) {
            if (bss_role == MLAN_BSS_ROLE_ANY ||
                GET_BSS_ROLE(handle->priv[i]) == bss_role)
                break;
        }
    }
    return ((i < handle->priv_num) ? handle->priv[i] : NULL);
}

/** Max line length allowed in init config file */
#define MAX_LINE_LEN        256
/** Max MAC address string length allowed */
#define MAX_MAC_ADDR_LEN    18
/** Max register type/offset/value etc. parameter length allowed */
#define MAX_PARAM_LEN       12

/** HostCmd_CMD_CFG_DATA for CAL data */
#define HostCmd_CMD_CFG_DATA 0x008f
/** HostCmd action set */
#define HostCmd_ACT_GEN_SET 0x0001
/** HostCmd CAL data header length */
#define CFG_DATA_HEADER_LEN	6

typedef struct _HostCmd_DS_GEN
{
    t_u16 command;
    t_u16 size;
    t_u16 seq_num;
    t_u16 result;
} HostCmd_DS_GEN;

typedef struct _HostCmd_DS_802_11_CFG_DATA
{
    /** Action */
    t_u16 action;
    /** Type */
    t_u16 type;
    /** Data length */
    t_u16 data_len;
    /** Data */
    t_u8 data[1];
} __ATTRIB_PACK__ HostCmd_DS_802_11_CFG_DATA;

/**  Convert ASCII string to hex value */
int woal_ascii2hex(t_u8 * d, char *s, t_u32 dlen);
/**  Convert mac address from string to t_u8 buffer */
void woal_mac2u8(t_u8 * mac_addr, char *buf);
/**  Extract token from string */
char *woal_strsep(char **s, char delim, char esc);
/** Return int value of a given ASCII string */
mlan_status woal_atoi(int *data, char *a);
/** Return hex value of a given ASCII string */
int woal_atox(char *a);
/** Return hex value of a given character */
int woal_hexval(char chr);
/** Allocate buffer */
pmlan_buffer woal_alloc_mlan_buffer(moal_handle * handle, int size);
/** Allocate IOCTL request buffer */
pmlan_ioctl_req woal_alloc_mlan_ioctl_req(int size);
/** Free buffer */
void woal_free_mlan_buffer(moal_handle * handle, pmlan_buffer pmbuf);
/** Get private structure of a BSS by index */
moal_private *woal_bss_num_to_priv(moal_handle * handle, t_u8 bss_num);
/* Functions in interface module */
/** Add card */
moal_handle *woal_add_card(void *card);
/** Remove card */
mlan_status woal_remove_card(void *card);
/** broadcast event */
mlan_status woal_broadcast_event(moal_private * priv, t_u8 * payload,
                                 t_u32 len);
/** switch driver mode */
mlan_status woal_switch_drv_mode(moal_handle * handle, t_u32 mode);

/** Interrupt handler */
void woal_interrupt(moal_handle * handle);

/** Get driver version */
void woal_get_version(moal_handle * handle, char *version, int maxlen);
/** Request open */
mlan_status woal_request_open(moal_private * priv);
/** Request close */
mlan_status woal_request_close(moal_private * priv);
/** Request MAC address setting */
mlan_status woal_request_set_mac_address(moal_private * priv);
/** Request multicast list setting */
void woal_request_set_multicast_list(moal_private * priv,
                                     struct net_device *dev);
/** Request IOCTL action */
mlan_status woal_request_ioctl(moal_private * priv, mlan_ioctl_req * req,
                               t_u8 wait_option);
mlan_status woal_request_soft_reset(moal_handle * handle);
#ifdef PROC_DEBUG
/** Get debug information */
mlan_status woal_get_debug_info(moal_private * priv, t_u8 wait_option,
                                mlan_debug_info * debug_info);
/** Set debug information */
mlan_status woal_set_debug_info(moal_private * priv, t_u8 wait_option,
                                mlan_debug_info * debug_info);
#endif
/** Disconnect */
mlan_status woal_disconnect(moal_private * priv, t_u8 wait_option, t_u8 * mac);
/** associate */
mlan_status woal_bss_start(moal_private * priv, t_u8 wait_option,
                           mlan_ssid_bssid * ssid_bssid);
/** Request firmware information */
void woal_request_get_fw_info(moal_private * priv, t_u8 wait_option);
/** Set/get Host Sleep parameters */
mlan_status woal_set_get_hs_params(moal_private * priv, t_u16 action,
                                   t_u8 wait_option, mlan_ds_hs_cfg * hscfg);
/** Cancel Host Sleep configuration */
mlan_status woal_cancel_hs(moal_private * priv, t_u8 wait_option);
#if defined(SDIO_SUSPEND_RESUME)
/** Enable Host Sleep configuration */
int woal_enable_hs(moal_private * priv);
/** hs active timeout 2 second */
#define HS_ACTIVE_TIMEOUT  (2 * HZ)
#endif

/** set deep sleep */
int woal_set_deep_sleep(moal_private * priv, t_u8 wait_option,
                        BOOLEAN bdeep_sleep, t_u16 idletime);

/** Get BSS information */
mlan_status woal_get_bss_info(moal_private * priv, t_u8 wait_option,
                              mlan_bss_info * bss_info);
#ifdef STA_SUPPORT
void woal_send_iwevcustom_event(moal_private * priv, t_s8 * str);
void woal_send_mic_error_event(moal_private * priv, t_u32 event);
void woal_process_ioctl_resp(moal_private * priv, mlan_ioctl_req * req);
/** Get mode */
t_u32 woal_get_mode(moal_private * priv, t_u8 wait_option);
/** Get signal information */
mlan_status woal_get_signal_info(moal_private * priv, t_u8 wait_option,
                                 mlan_ds_get_signal * signal);
/** Get statistics information */
mlan_status woal_get_stats_info(moal_private * priv, t_u8 wait_option,
                                mlan_ds_get_stats * stats);
/** Get data rates */
mlan_status woal_get_data_rates(moal_private * priv, t_u8 wait_option,
                                moal_802_11_rates * m_rates);
/** Get channel list */
mlan_status woal_get_channel_list(moal_private * priv, t_u8 wait_option,
                                  mlan_chan_list * chanlist);
/** Request a network scan */
mlan_status woal_request_scan(moal_private * priv, t_u8 wait_option,
                              mlan_802_11_ssid * req_ssid);
/** Set radio on/off */
int woal_set_radio(moal_private * priv, t_u8 option);
/** Get scan table */
mlan_status woal_get_scan_table(moal_private * priv, t_u8 wait_option,
                                mlan_scan_resp * scanresp);
/** Set authentication mode */
mlan_status woal_set_auth_mode(moal_private * priv, t_u8 wait_option,
                               t_u32 auth_mode);
/** Get authentication mode */
mlan_status woal_get_auth_mode(moal_private * priv, t_u8 wait_option,
                               t_u32 * auth_mode);
/** Set encryption mode */
mlan_status woal_set_encrypt_mode(moal_private * priv, t_u8 wait_option,
                                  t_u32 encrypt_mode);
/** Get encryption mode */
mlan_status woal_get_encrypt_mode(moal_private * priv, t_u8 wait_option,
                                  t_u32 * encrypt_mode);
/** Enable wep key */
mlan_status woal_enable_wep_key(moal_private * priv, t_u8 wait_option);
/** Set WPA enable */
mlan_status woal_set_wpa_enable(moal_private * priv, t_u8 wait_option,
                                t_u32 enable);
/** Get WPA state */
mlan_status woal_get_wpa_enable(moal_private * priv, t_u8 wait_option,
                                t_u32 * enable);
/** Find best network to connect */
mlan_status woal_find_best_network(moal_private * priv, t_u8 wait_option,
                                   mlan_ssid_bssid * ssid_bssid);
/** Set E-Supplicant mode */
mlan_status woal_set_ewpa_mode(moal_private * priv, t_u8 wait_option,
                               mlan_ssid_bssid * ssid_bssid);
/** Set Ad-Hoc channel */
mlan_status woal_change_adhoc_chan(moal_private * priv, int channel);

#endif  /**STA_SUPPORT */

mlan_status woal_set_wapi_enable(moal_private * priv, t_u8 wait_option,
                                 t_u32 enable);

/** Initialize priv */
void woal_init_priv(moal_private * priv, t_u8 wait_option);
/** Reset interface(s) */
int woal_reset_intf(moal_private * priv, t_u8 wait_option, int all_intf);
/** common ioctl for uap, station */
int woal_custom_ie_ioctl(struct net_device *dev, struct ifreq *req);
int woal_get_bss_type(struct net_device *dev, struct ifreq *req);
int woal_host_command(moal_private * priv, struct iwreq *wrq);

#if defined(UAP_SUPPORT)
/** hostcmd ioctl for uap, wifidirect */
int woal_hostcmd_ioctl(struct net_device *dev, struct ifreq *req);
#endif

#ifdef CONFIG_PROC_FS
/** Initialize proc fs */
void woal_proc_init(moal_handle * handle);
/** Clean up proc fs */
void woal_proc_exit(moal_handle * handle);
/** Create proc entry */
void woal_create_proc_entry(moal_private * priv);
/** Remove proc entry */
void woal_proc_remove(moal_private * priv);
/** string to number */
int woal_string_to_number(char *s);
#endif

#ifdef PROC_DEBUG
/** Create debug proc fs */
void woal_debug_entry(moal_private * priv);
/** Remove debug proc fs */
void woal_debug_remove(moal_private * priv);
#endif /* PROC_DEBUG */

/** check pm info */
mlan_status woal_get_pm_info(moal_private * priv, mlan_ds_ps_info * pm_info);

#ifdef REASSOCIATION
int woal_reassociation_thread(void *data);
void woal_reassoc_timer_func(void *context);
#endif /* REASSOCIATION */

t_void woal_main_work_queue(struct work_struct *work);

int woal_hard_start_xmit(struct sk_buff *skb, struct net_device *dev);

#endif /* _MOAL_MAIN_H */
