/**  @file moal_proc.c
  *
  * @brief This file contains functions for proc file.
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

#include	"moal_main.h"
#ifdef UAP_SUPPORT
#include    "moal_uap.h"
#endif
#include 	"moal_sdio.h"

/********************************************************
		Local Variables
********************************************************/
#ifdef CONFIG_PROC_FS
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
#define PROC_DIR	NULL
#define MWLAN_PROC_DIR  "mwlan/"
/** Proc top level directory entry */
struct proc_dir_entry *proc_mwlan = NULL;
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)
#define PROC_DIR	&proc_root
#else
#define PROC_DIR	proc_net
#endif

#ifdef STA_SUPPORT
static char *szModes[] = {
    "Unknown",
    "Managed",
    "Ad-hoc",
    "Auto",
};
#endif

extern int drv_mode;

/********************************************************
		Global Variables
********************************************************/

/********************************************************
		Local Functions
********************************************************/
/** 
 *  @brief Proc read function for info
 *
 *  @param page	   Pointer to buffer
 *  @param start   Read data starting position
 *  @param offset  Offset
 *  @param count   Counter 
 *  @param eof     End of file flag
 *  @param data    Data to output
 *
 *  @return 	   Number of output data
 */
static int
woal_info_proc_read(char *page, char **start, off_t offset,
                    int count, int *eof, void *data)
{
    char *p = page;
    struct net_device *netdev = data;
    char fmt[64];
    moal_private *priv = (moal_private *) netdev_priv(netdev);
#ifdef STA_SUPPORT
    int i = 0;
    moal_handle *handle = priv->phandle;
    mlan_bss_info info;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35)
    struct dev_mc_list *mcptr = netdev->mc_list;
    int mc_count = netdev->mc_count;
#else
    struct netdev_hw_addr *mcptr = NULL;
    int mc_count = netdev_mc_count(netdev);
#endif /* < 2.6.35 */
#else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34)
    int i = 0;
#endif /* >= 2.6.34 */
#endif
#ifdef UAP_SUPPORT
    mlan_ds_uap_stats ustats;
#endif

    if (offset) {
        *eof = 1;
        goto exit;
    }
#ifdef UAP_SUPPORT
    if (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_UAP) {
        p += sprintf(p, "driver_name = " "\"uap\"\n");
        woal_uap_get_version(priv, fmt, sizeof(fmt) - 1);
        if (MLAN_STATUS_SUCCESS !=
            woal_uap_get_stats(priv, MOAL_PROC_WAIT, &ustats)) {
            *eof = 1;
            goto exit;
        }
    }
#endif /* UAP_SUPPORT */
#ifdef STA_SUPPORT
    if (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA) {
        woal_get_version(handle, fmt, sizeof(fmt) - 1);
        memset(&info, 0, sizeof(info));
        if (MLAN_STATUS_SUCCESS !=
            woal_get_bss_info(priv, MOAL_PROC_WAIT, &info)) {
            *eof = 1;
            goto exit;
        }
        p += sprintf(p, "driver_name = " "\"wlan\"\n");
    }
#endif
    p += sprintf(p, "driver_version = %s", fmt);
    p += sprintf(p, "\ninterface_name=\"%s\"\n", netdev->name);
#ifdef STA_SUPPORT
    if (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA)
        p += sprintf(p, "bss_mode=\"%s\"\n", szModes[info.bss_mode]);
#endif
    p += sprintf(p, "media_state=\"%s\"\n",
                 ((priv->media_connected ==
                   MFALSE) ? "Disconnected" : "Connected"));
    p += sprintf(p, "mac_address=\"%02x:%02x:%02x:%02x:%02x:%02x\"\n",
                 netdev->dev_addr[0], netdev->dev_addr[1], netdev->dev_addr[2],
                 netdev->dev_addr[3], netdev->dev_addr[4], netdev->dev_addr[5]);
#ifdef STA_SUPPORT
    if (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA) {
        p += sprintf(p, "multicast_count=\"%d\"\n", mc_count);
        p += sprintf(p, "essid=\"%s\"\n", info.ssid.ssid);
        p += sprintf(p, "bssid=\"%02x:%02x:%02x:%02x:%02x:%02x\"\n",
                     info.bssid[0], info.bssid[1],
                     info.bssid[2], info.bssid[3],
                     info.bssid[4], info.bssid[5]);
        p += sprintf(p, "channel=\"%d\"\n", (int) info.bss_chan);
        p += sprintf(p, "region_code = \"%02x\"\n", (t_u8) info.region_code);

        /* 
         * Put out the multicast list 
         */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35)
        for (i = 0; i < netdev->mc_count; i++) {
            p += sprintf(p,
                         "multicast_address[%d]=\"%02x:%02x:%02x:%02x:%02x:%02x\"\n",
                         i,
                         mcptr->dmi_addr[0], mcptr->dmi_addr[1],
                         mcptr->dmi_addr[2], mcptr->dmi_addr[3],
                         mcptr->dmi_addr[4], mcptr->dmi_addr[5]);

            mcptr = mcptr->next;
        }
#else
        netdev_for_each_mc_addr(mcptr, netdev)
            p += sprintf(p,
                         "multicast_address[%d]=\"%02x:%02x:%02x:%02x:%02x:%02x\"\n",
                         i++,
                         mcptr->addr[0], mcptr->addr[1],
                         mcptr->addr[2], mcptr->addr[3],
                         mcptr->addr[4], mcptr->addr[5]);
#endif /* < 2.6.35 */
    }
#endif
    p += sprintf(p, "num_tx_bytes = %lu\n", priv->stats.tx_bytes);
    p += sprintf(p, "num_rx_bytes = %lu\n", priv->stats.rx_bytes);
    p += sprintf(p, "num_tx_pkts = %lu\n", priv->stats.tx_packets);
    p += sprintf(p, "num_rx_pkts = %lu\n", priv->stats.rx_packets);
    p += sprintf(p, "num_tx_pkts_dropped = %lu\n", priv->stats.tx_dropped);
    p += sprintf(p, "num_rx_pkts_dropped = %lu\n", priv->stats.rx_dropped);
    p += sprintf(p, "num_tx_pkts_err = %lu\n", priv->stats.tx_errors);
    p += sprintf(p, "num_rx_pkts_err = %lu\n", priv->stats.rx_errors);
    p += sprintf(p, "carrier %s\n",
                 ((netif_carrier_ok(priv->netdev)) ? "on" : "off"));
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34)
    for (i = 0; i < netdev->num_tx_queues; i++) {
        p += sprintf(p, "tx queue %d:  %s\n", i,
                     ((netif_tx_queue_stopped(netdev_get_tx_queue(netdev, 0))) ?
                      "stopped" : "started"));
    }
#else
    p += sprintf(p, "tx queue %s\n",
                 ((netif_queue_stopped(priv->netdev)) ? "stopped" : "started"));
#endif
#ifdef UAP_SUPPORT
    if (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_UAP) {
        p += sprintf(p, "tkip_mic_failures = %lu\n", ustats.tkip_mic_failures);
        p += sprintf(p, "ccmp_decrypt_errors = %lu\n",
                     ustats.ccmp_decrypt_errors);
        p += sprintf(p, "wep_undecryptable_count = %lu\n",
                     ustats.wep_undecryptable_count);
        p += sprintf(p, "wep_icv_error_count = %lu\n",
                     ustats.wep_icv_error_count);
        p += sprintf(p, "decrypt_failure_count = %lu\n",
                     ustats.decrypt_failure_count);
        p += sprintf(p, "mcast_tx_count = %lu\n", ustats.mcast_tx_count);
        p += sprintf(p, "failed_count = %lu\n", ustats.failed_count);
        p += sprintf(p, "retry_count = %lu\n", ustats.retry_count);
        p += sprintf(p, "multiple_retry_count = %lu\n",
                     ustats.multi_retry_count);
        p += sprintf(p, "frame_duplicate_count = %lu\n",
                     ustats.frame_dup_count);
        p += sprintf(p, "rts_success_count = %lu\n", ustats.rts_success_count);
        p += sprintf(p, "rts_failure_count = %lu\n", ustats.rts_failure_count);
        p += sprintf(p, "ack_failure_count = %lu\n", ustats.ack_failure_count);
        p += sprintf(p, "rx_fragment_count = %lu\n", ustats.rx_fragment_count);
        p += sprintf(p, "mcast_rx_frame_count = %lu\n",
                     ustats.mcast_rx_frame_count);
        p += sprintf(p, "fcs_error_count = %lu\n", ustats.fcs_error_count);
        p += sprintf(p, "tx_frame_count = %lu\n", ustats.tx_frame_count);
        p += sprintf(p, "rsna_tkip_cm_invoked = %lu\n",
                     ustats.rsna_tkip_cm_invoked);
        p += sprintf(p, "rsna_4way_hshk_failures = %lu\n",
                     ustats.rsna_4way_hshk_failures);
    }
#endif /* UAP_SUPPORT */
  exit:
    return (p - page);
}

#define     CMD52_STR_LEN   50
/*
 *  @brief Parse cmd52 string
 *
 *  @param buffer  A pointer user buffer
 *  @param len     Length user buffer
 *  @param func    Parsed func number
 *  @param reg     Parsed reg value
 *  @param val     Parsed value to set
 *  @return 	   BT_STATUS_SUCCESS
 */
static int
parse_cmd52_string(const char __user * buffer, size_t len, int *func, int *reg,
                   int *val)
{
    int ret = MLAN_STATUS_SUCCESS;
    char *string = NULL;
    char *pos = NULL;

    ENTER();

    string = (char *) kmalloc(CMD52_STR_LEN, GFP_KERNEL);
    memset(string, 0, CMD52_STR_LEN);
    memcpy(string, buffer + strlen("sdcmd52rw="), len - strlen("sdcmd52rw="));
    string = strstrip(string);

    *func = -1;
    *reg = -1;
    *val = -1;

    /* Get func */
    pos = strsep(&string, " \t");
    if (pos) {
        *func = woal_string_to_number(pos);
    }

    /* Get reg */
    pos = strsep(&string, " \t");
    if (pos) {
        *reg = woal_string_to_number(pos);
    }

    /* Get val (optional) */
    pos = strsep(&string, " \t");
    if (pos) {
        *val = woal_string_to_number(pos);
    }
    if (string)
        kfree(string);
    LEAVE();
    return ret;
}

/** 
 *  @brief config proc write function
 *
 *  @param f	   file pointer
 *  @param buf     pointer to data buffer
 *  @param cnt     data number to write
 *  @param data    data to write
 *  @return 	   number of data
 */
static int
woal_config_write(struct file *f, const char *buf, unsigned long cnt,
                  void *data)
{
    char databuf[100];
    char *line;
    t_u32 config_data = 0;
    moal_handle *handle = (moal_handle *) data;
    int func, reg, val;

    MODULE_GET;
    if (cnt > sizeof(databuf)) {
        MODULE_PUT;
        return cnt;
    }
    memset(databuf, 0, sizeof(databuf));
    if (copy_from_user(databuf, buf, cnt)) {
        MODULE_PUT;
        return 0;
    }
    line = databuf;
    if (!strncmp(databuf, "soft_reset", strlen("soft_reset"))) {
        line += strlen("soft_reset") + 1;
        config_data = (t_u32) woal_string_to_number(line);
        PRINTM(MINFO, "soft_reset: %d\n", (int) config_data);
        woal_request_soft_reset(handle);
        handle->hardware_status = HardwareStatusReset;
    }
    if (!strncmp(databuf, "drv_mode", strlen("drv_mode"))) {
        line += strlen("drv_mode") + 1;
        config_data = (t_u32) woal_string_to_number(line);
        PRINTM(MINFO, "drv_mode: %d\n", (int) config_data);
        if (config_data != (t_u32) drv_mode)
            woal_switch_drv_mode(handle, config_data);
    }
    if (!strncmp(databuf, "sdcmd52rw=", strlen("sdcmd52rw="))) {
        parse_cmd52_string(databuf, cnt, &func, &reg, &val);
        woal_sdio_read_write_cmd52(handle, func, reg, val);
    }
    MODULE_PUT;
    return cnt;
}

/** 
 *  @brief config proc read function
 *
 *  @param page	   pointer to buffer
 *  @param s       read data starting position
 *  @param off     offset
 *  @param cnt     counter 
 *  @param eof     end of file flag
 *  @param data    data to output
 *  @return 	   number of output data
 */
static int
woal_config_read(char *page, char **s, off_t off, int cnt, int *eof, void *data)
{
    char *p = page;
    moal_handle *handle = (moal_handle *) data;
    MODULE_GET;
    p += sprintf(p, "hardware_status=%d\n", (int) handle->hardware_status);
    p += sprintf(p, "netlink_num=%d\n", (int) handle->netlink_num);
    p += sprintf(p, "drv_mode=%d\n", (int) drv_mode);
    p += sprintf(p, "sdcmd52rw=%d 0x%0x 0x%02X\n", handle->cmd52_func,
                 handle->cmd52_reg, handle->cmd52_val);
    MODULE_PUT;
    return p - page;
}

#ifdef UAP_SUPPORT
/** 
 *  @brief uap setting proc write function
 *
 *  @param f	   file pointer
 *  @param buf     pointer to data buffer
 *  @param cnt     data number to write
 *  @param data    data to write
 *  @return 	   number of data
 */
static int
woal_setting_write(struct file *f, const char *buf, unsigned long cnt,
                   void *data)
{
    char databuf[100];
    char *line;
    struct net_device *netdev = data;
    moal_private *priv = (moal_private *) netdev_priv(netdev);
    t_u32 config_data = 0;
    MODULE_GET;
    if (cnt > sizeof(databuf)) {
        MODULE_PUT;
        return cnt;
    }
    memset(databuf, 0, sizeof(databuf));
    if (copy_from_user(databuf, buf, cnt)) {
        MODULE_PUT;
        return 0;
    }
    line = databuf;
    if (!strncmp(databuf, "bss_start", strlen("bss_start"))) {
        line += strlen("bss_start") + 1;
        config_data = (t_u32) woal_string_to_number(line);
        if (config_data == 1) {
            PRINTM(MINFO, "bss_start: %d\n", (int) config_data);
            woal_bss_start(priv, MOAL_PROC_WAIT, NULL);
        } else if (config_data == 0) {
            PRINTM(MINFO, "bss_stop: %d\n", (int) config_data);
            woal_disconnect(priv, MOAL_PROC_WAIT, NULL);
        }
    }
    MODULE_PUT;
    return cnt;
}
#endif /* UAP_SUPPORT */

/********************************************************
		Global Functions
********************************************************/
/** 
 *  @brief Convert string to number
 *
 *  @param s   	   Pointer to numbered string
 *
 *  @return 	   Converted number from string s
 */
int
woal_string_to_number(char *s)
{
    int r = 0;
    int base = 0;
    int pn = 1;

    if (!strncmp(s, "-", 1)) {
        pn = -1;
        s++;
    }
    if (!strncmp(s, "0x", 2) || !strncmp(s, "0X", 2)) {
        base = 16;
        s += 2;
    } else
        base = 10;

    for (s = s; *s; s++) {
        if ((*s >= '0') && (*s <= '9'))
            r = (r * base) + (*s - '0');
        else if ((*s >= 'A') && (*s <= 'F'))
            r = (r * base) + (*s - 'A' + 10);
        else if ((*s >= 'a') && (*s <= 'f'))
            r = (r * base) + (*s - 'a' + 10);
        else
            break;
    }

    return (r * pn);
}

/** 
 *  @brief Create the top level proc directory
 *
 *  @param handle   Pointer to woal_handle
 *
 *  @return 	    N/A
 */
void
woal_proc_init(moal_handle * handle)
{
    struct proc_dir_entry *r;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
    struct proc_dir_entry *pde = PROC_DIR;
#endif
    char config_proc_dir[20];

    ENTER();

    PRINTM(MINFO, "Create Proc Interface\n");
    if (!handle->proc_mwlan) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
        /* Check if directory already exists */
        for (pde = pde->subdir; pde; pde = pde->next) {
            if (pde->namelen && !strcmp("mwlan", pde->name)) {
                /* Directory exists */
                PRINTM(MWARN, "proc interface already exists!\n");
                handle->proc_mwlan = pde;
                break;
            }
        }
        if (pde == NULL) {
            handle->proc_mwlan = proc_mkdir("mwlan", PROC_DIR);
            if (!handle->proc_mwlan)
                PRINTM(MERROR, "Cannot create proc interface!\n");
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
            else
                atomic_set(&handle->proc_mwlan->count, 1);
#endif
        }
#else
        if (!proc_mwlan) {
            handle->proc_mwlan = proc_mkdir("mwlan", PROC_DIR);
            if (!handle->proc_mwlan) {
                PRINTM(MERROR, "Cannot create proc interface!\n");
            }
        } else {
            handle->proc_mwlan = proc_mwlan;
        }
#endif
        if (handle->proc_mwlan) {
            if (handle->handle_idx)
                sprintf(config_proc_dir, "config%d", handle->handle_idx);
            else
                strcpy(config_proc_dir, "config");

            r = create_proc_entry(config_proc_dir, 0644, handle->proc_mwlan);

            if (r) {
                r->data = handle;
                r->read_proc = woal_config_read;
                r->write_proc = woal_config_write;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
                r->owner = THIS_MODULE;
#endif
            } else
                PRINTM(MMSG, "Fail to create proc config\n");
        }
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,26)
        proc_mwlan = handle->proc_mwlan;
#endif
    }

    LEAVE();
}

/** 
 *  @brief Remove the top level proc directory
 *
 *  @param handle  pointer moal_handle
 *
 *  @return 	   N/A
 */
void
woal_proc_exit(moal_handle * handle)
{
    ENTER();

    PRINTM(MINFO, "Remove Proc Interface\n");
    if (handle->proc_mwlan) {
        char config_proc_dir[20];
        if (handle->handle_idx)
            sprintf(config_proc_dir, "config%d", handle->handle_idx);
        else
            strcpy(config_proc_dir, "config");
        remove_proc_entry(config_proc_dir, handle->proc_mwlan);

        /* Remove only if we are the only instance using this */
        if (atomic_read(&(handle->proc_mwlan->count)) > 1) {
            PRINTM(MWARN, "More than one interface using proc!\n");
        } else {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
            atomic_dec(&(handle->proc_mwlan->count));
#endif
            remove_proc_entry("mwlan", PROC_DIR);
            handle->proc_mwlan = NULL;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,26)
            proc_mwlan = NULL;
#endif
        }
    }

    LEAVE();
}

/** 
 *  @brief Create proc file for interface
 *
 *  @param priv	   pointer moal_private
 *
 *  @return 	   N/A
 */
void
woal_create_proc_entry(moal_private * priv)
{
    struct net_device *dev = priv->netdev;
    struct proc_dir_entry *r;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
    char proc_dir_name[20];
#endif

    ENTER();

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
    if (!priv->proc_entry) {
        memset(proc_dir_name, 0, sizeof(proc_dir_name));
        strcpy(proc_dir_name, MWLAN_PROC_DIR);
        strcat(proc_dir_name, dev->name);
        /* Try to create mwlan/mlanX first */
        priv->proc_entry = proc_mkdir(proc_dir_name, PROC_DIR);
        if (priv->proc_entry) {
            /* Success. Continue normally */
            if (!priv->phandle->proc_mwlan) {
                priv->phandle->proc_mwlan = priv->proc_entry->parent;
            }
            atomic_inc(&(priv->phandle->proc_mwlan->count));
        } else {
            /* Failure. mwlan may not exist. Try to create that first */
            priv->phandle->proc_mwlan = proc_mkdir("mwlan", PROC_DIR);
            if (!priv->phandle->proc_mwlan) {
                /* Failure. Something broken */
                LEAVE();
                return;
            } else {
                /* Success. Now retry creating mlanX */
                priv->proc_entry = proc_mkdir(proc_dir_name, PROC_DIR);
                atomic_inc(&(priv->phandle->proc_mwlan->count));
            }
        }
#else
    if (priv->phandle->proc_mwlan && !priv->proc_entry) {
        priv->proc_entry = proc_mkdir(dev->name, priv->phandle->proc_mwlan);
        atomic_inc(&(priv->phandle->proc_mwlan->count));
#endif
        strcpy(priv->proc_entry_name, dev->name);
        if (priv->proc_entry) {
            r = create_proc_read_entry("info", 0, priv->proc_entry,
                                       woal_info_proc_read, dev);
#ifdef UAP_SUPPORT
            if (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_UAP) {
                r = create_proc_entry("setting", 0644, priv->proc_entry);
                if (r) {
                    r->data = dev;
                    r->write_proc = woal_setting_write;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
                    r->owner = THIS_MODULE;
#endif
                }
            }
#endif /* UAP_SUPPORT */
        }
    }

    LEAVE();
}

/** 
 *  @brief Remove proc file
 *
 *  @param priv	   Pointer moal_private
 *
 *  @return 	   N/A
 */
void
woal_proc_remove(moal_private * priv)
{
    ENTER();
    if (priv->phandle->proc_mwlan && priv->proc_entry) {
#ifdef UAP_SUPPORT
        if (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_UAP)
            remove_proc_entry("setting", priv->proc_entry);
#endif /* UAP_SUPPORT */
        remove_proc_entry("info", priv->proc_entry);
        remove_proc_entry(priv->proc_entry_name, priv->phandle->proc_mwlan);
        atomic_dec(&(priv->phandle->proc_mwlan->count));
        priv->proc_entry = NULL;
    }
    LEAVE();
}
#endif
