/** @file mlan_wmm.c
 *
 *  @brief This file contains functions for WMM.
 *
 *  Copyright (C) 2008-2010, Marvell International Ltd. 
 *  All Rights Reserved
 */

/********************************************************
Change log:
    10/24/2008: initial version
********************************************************/

#include "mlan.h"
#ifdef STA_SUPPORT
#include "mlan_join.h"
#endif
#include "mlan_util.h"
#include "mlan_fw.h"
#include "mlan_main.h"
#include "mlan_wmm.h"
#include "mlan_11n.h"
#include "mlan_sdio.h"

/********************************************************
    Local Variables
********************************************************/

/** Maximum value FW can accept for driver delay in packet transmission */
#define DRV_PKT_DELAY_TO_FW_MAX   512

/*
 * Upper and Lower threshold for packet queuing in the driver
 
 *    - When the number of packets queued reaches the upper limit,
 *      the driver will stop the net queue in the app/kernel space.
 
 *    - When the number of packets drops beneath the lower limit after
 *      having reached the upper limit, the driver will restart the net
 *      queue.
 */

/** Lower threshold for packet queuing in the driver.
  * When the number of packets drops beneath the lower limit after having
  * reached the upper limit, the driver will restart the net queue.
  */
#define WMM_QUEUED_PACKET_LOWER_LIMIT   180

/** Upper threshold for packet queuing in the driver.
  * When the number of packets queued reaches the upper limit, the driver
  * will stop the net queue in the app/kernel space.
  */
#define WMM_QUEUED_PACKET_UPPER_LIMIT   200

/** Offset for TOS field in the IP header */
#define IPTOS_OFFSET 5

/** WMM information IE */
static const t_u8 wmm_info_ie[] = { WMM_IE, 0x07,
    0x00, 0x50, 0xf2, 0x02,
    0x00, 0x01, 0x00
};

/**
 * AC Priorities go from AC_BK to AC_VO.  The ACI enumeration for AC_BK (1)
 *   is higher than the enumeration for AC_BE (0); hence the needed
 *   mapping conversion for wmm AC to priority Queue Index
 */
static const t_u8 wmm_aci_to_qidx_map[] = { WMM_AC_BE,
    WMM_AC_BK,
    WMM_AC_VI,
    WMM_AC_VO
};

/** 
 * This table will be used to store the tid values based on ACs.
 * It is initialized to default values per TID.
 */
t_u8 tos_to_tid[] = {
    /* TID DSCP_P2 DSCP_P1 DSCP_P0 WMM_AC */
    0x01,                       /* 0 1 0 AC_BK */
    0x02,                       /* 0 0 0 AC_BK */
    0x00,                       /* 0 0 1 AC_BE */
    0x03,                       /* 0 1 1 AC_BE */
    0x04,                       /* 1 0 0 AC_VI */
    0x05,                       /* 1 0 1 AC_VI */
    0x06,                       /* 1 1 0 AC_VO */
    0x07                        /* 1 1 1 AC_VO */
};

/** 
 * This table inverses the tos_to_tid operation to get a priority
 * which is in sequential order, and can be compared.
 * Use this to compare the priority of two different TIDs.
 */
t_u8 tos_to_tid_inv[] = { 0x02, /* from tos_to_tid[2] = 0 */
    0x00,                       /* from tos_to_tid[0] = 1 */
    0x01,                       /* from tos_to_tid[1] = 2 */
    0x03,
    0x04,
    0x05,
    0x06,
    0x07
};

/** 
 * This table will provide the tid value for given ac. This table does not
 * change and will be used to copy back the default values to tos_to_tid in
 * case of disconnect.
 */
static const t_u8 ac_to_tid[4][2] = { {1, 2}, {0, 3}, {4, 5}, {6, 7} };

/* Map of TOS UP values to WMM AC */
static const mlan_wmm_ac_e tos_to_ac[] = { WMM_AC_BE,
    WMM_AC_BK,
    WMM_AC_BK,
    WMM_AC_BE,
    WMM_AC_VI,
    WMM_AC_VI,
    WMM_AC_VO,
    WMM_AC_VO
};

/********************************************************
    Local Functions
********************************************************/
#ifdef DEBUG_LEVEL2
/**
 *  @brief Debug print function to display the priority parameters for a WMM AC
 *
 *  @param pac_param	Pointer to the AC parameters to display
 *
 *  @return		N/A
 */
static void
wlan_wmm_ac_debug_print(const IEEEtypes_WmmAcParameters_t * pac_param)
{
    const char *ac_str[] = { "BK", "BE", "VI", "VO" };

    ENTER();

    PRINTM(MINFO, "WMM AC_%s: ACI=%d, ACM=%d, Aifsn=%d, "
           "EcwMin=%d, EcwMax=%d, TxopLimit=%d\n",
           ac_str[wmm_aci_to_qidx_map[pac_param->aci_aifsn.aci]],
           pac_param->aci_aifsn.aci, pac_param->aci_aifsn.acm,
           pac_param->aci_aifsn.aifsn, pac_param->ecw.ecw_min,
           pac_param->ecw.ecw_max, wlan_le16_to_cpu(pac_param->tx_op_limit));

    LEAVE();
}

/** Print the WMM AC for debug purpose */
#define PRINTM_AC(pac_param) wlan_wmm_ac_debug_print(pac_param)
#else
/** Print the WMM AC for debug purpose */
#define PRINTM_AC(pac_param)
#endif

/**
 *  @brief Get the threshold value for BA setup using system time.
 *
 *  @param pmadapter       Pointer to the mlan_adapter structure
 *
 *  @return         threshold value.
 */
t_u8
wlan_get_random_ba_threshold(pmlan_adapter pmadapter)
{
    t_u32 sec, usec;
    t_u8 ba_threshold = 0;

    ENTER();

    /* setup ba_packet_threshold here random number between [1 -
       BA_SETUP_MAX_PACKET_THRESHOLD] */

#define BA_SETUP_MAX_PACKET_THRESHOLD   16

    pmadapter->callbacks.moal_get_system_time(pmadapter->pmoal_handle, &sec,
                                              &usec);
    sec = (sec & 0xFFFF) + (sec >> 16);
    usec = (usec & 0xFFFF) + (usec >> 16);

    ba_threshold = (((sec << 16) + usec) % BA_SETUP_MAX_PACKET_THRESHOLD);
    PRINTM(MINFO, "setup BA after %d packets\n", ba_threshold);

    LEAVE();
    return ba_threshold;
}

/**
 *  @brief Allocate route address
 *
 *  @param pmadapter       Pointer to the mlan_adapter structure
 *  @param ra              Pointer to the route address
 *
 *  @return         ra_list
 */
static raListTbl *
wlan_wmm_allocate_ralist_node(pmlan_adapter pmadapter, t_u8 * ra)
{
    raListTbl *ra_list = MNULL;

    ENTER();

    if (pmadapter->callbacks.
        moal_malloc(pmadapter->pmoal_handle, sizeof(raListTbl), MLAN_MEM_DEF,
                    (t_u8 **) & ra_list)) {
        PRINTM(MERROR, "Fail to allocate ra_list\n");
        goto done;
    }
    util_init_list((pmlan_linked_list) ra_list);
    util_init_list_head((t_void *) pmadapter->pmoal_handle,
                        &ra_list->buf_head, MFALSE,
                        pmadapter->callbacks.moal_init_lock);

    memcpy(pmadapter, ra_list->ra, ra, MLAN_MAC_ADDR_LENGTH);

    ra_list->total_pkts_size = 0;

    PRINTM(MINFO, "RAList: Allocating buffers for TID %p\n", ra_list);
  done:
    LEAVE();
    return ra_list;
}

/**
 *  @brief  This function cleans Tx/Rx queues
 *
 *  @param priv		A pointer to mlan_private
 *
 *  @return		N/A
 */
t_void
wlan_clean_txrx(pmlan_private priv)
{
    mlan_adapter *pmadapter = priv->adapter;
    t_u8 i = 0;

    ENTER();

    if (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA) {
        wlan_cleanup_bypass_txq(pmadapter);
    }
    wlan_11n_cleanup_reorder_tbl(priv);

    pmadapter->callbacks.moal_spin_lock(pmadapter->pmoal_handle,
                                        priv->wmm.ra_list_spinlock);

    wlan_wmm_cleanup_queues(priv);
    wlan_11n_deleteall_txbastream_tbl(priv);
#ifdef SDIO_MULTI_PORT_TX_AGGR
    MP_TX_AGGR_BUF_RESET(priv->adapter);
#endif
#ifdef SDIO_MULTI_PORT_RX_AGGR
    MP_RX_AGGR_BUF_RESET(priv->adapter);
#endif
    wlan_wmm_delete_all_ralist(priv);
    memcpy(pmadapter, tos_to_tid, ac_to_tid, sizeof(tos_to_tid));
    for (i = 0; i < MAX_NUM_TID; i++) {
        tos_to_tid_inv[tos_to_tid[i]] = (t_u8) i;
    }
    pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                          priv->wmm.ra_list_spinlock);

    LEAVE();
}

/**
 *  @brief  Allocate and add a RA list for all TIDs with the given RA
 *
 *  @param priv  Pointer to the mlan_private driver data struct
 *  @param ra	 Address of the receiver STA (AP in case of infra)
 *
 *  @return      N/A
 */
void
wlan_ralist_add(mlan_private * priv, t_u8 * ra)
{
    int i;
    raListTbl *ra_list;
    pmlan_adapter pmadapter = priv->adapter;

    ENTER();

    for (i = 0; i < MAX_NUM_TID; ++i) {
        ra_list = wlan_wmm_allocate_ralist_node(pmadapter, ra);
        PRINTM(MINFO, "Creating RA List %p for tid %d\n", ra_list, i);
        if (!ra_list)
            break;
        if (queuing_ra_based(priv))
            ra_list->is_11n_enabled = wlan_is_11n_enabled(priv, ra);
        else
            ra_list->is_11n_enabled = IS_11N_ENABLED(priv);
        PRINTM(MDATA, "ralist %p: is_11n_enabled=%d\n",
               ra_list, ra_list->is_11n_enabled);

        if (ra_list->is_11n_enabled) {
            ra_list->packet_count = 0;
            ra_list->ba_packet_threshold =
                wlan_get_random_ba_threshold(pmadapter);
        }
        util_enqueue_list_tail(pmadapter->pmoal_handle,
                               &priv->wmm.tid_tbl_ptr[i].ra_list,
                               (pmlan_linked_list) ra_list, MNULL, MNULL);

        if (!priv->wmm.tid_tbl_ptr[i].ra_list_curr)
            priv->wmm.tid_tbl_ptr[i].ra_list_curr = ra_list;
    }

    LEAVE();
}

/**
 *  @brief Set the WMM queue priorities to their default values
 *
 *  @param priv     Pointer to the mlan_private driver data struct
 *
 *  @return         N/A
 */
void
wlan_wmm_default_queue_priorities(pmlan_private priv)
{
    ENTER();

    /* Default queue priorities: VO->VI->BE->BK */
    priv->wmm.queue_priority[0] = WMM_AC_VO;
    priv->wmm.queue_priority[1] = WMM_AC_VI;
    priv->wmm.queue_priority[2] = WMM_AC_BE;
    priv->wmm.queue_priority[3] = WMM_AC_BK;

    LEAVE();
}

/**
 * @brief Map ACs to TID 
 *
 * @param priv             Pointer to the mlan_private driver data struct
 * @param queue_priority   Queue_priority structure
 *
 * @return 	   N/A
 */
static void
wlan_wmm_queue_priorities_tid(pmlan_private priv, t_u8 queue_priority[])
{
    int i;

    ENTER();

    for (i = 0; i < 4; ++i) {
        tos_to_tid[7 - (i * 2)] = ac_to_tid[queue_priority[i]][1];
        tos_to_tid[6 - (i * 2)] = ac_to_tid[queue_priority[i]][0];
    }

    for (i = 0; i < MAX_NUM_TID; i++) {
        tos_to_tid_inv[tos_to_tid[i]] = (t_u8) i;
    }

    /* in case priorities have changed, force highest priority so next packet
       will check from top to re-establish the highest */
    util_scalar_write(priv->adapter->pmoal_handle,
                      &priv->wmm.highest_queued_prio,
                      HIGH_PRIO_TID,
                      priv->adapter->callbacks.moal_spin_lock,
                      priv->adapter->callbacks.moal_spin_unlock);

    LEAVE();
}

/**
 *  @brief Initialize WMM priority queues
 *
 *  @param priv     Pointer to the mlan_private driver data struct
 *  @param pwmm_ie  Pointer to the IEEEtypes_WmmParameter_t data struct
 *
 *  @return         N/A
 */
void
wlan_wmm_setup_queue_priorities(pmlan_private priv,
                                IEEEtypes_WmmParameter_t * pwmm_ie)
{
    t_u16 cw_min, avg_back_off, tmp[4];
    t_u32 i, j, num_ac;
    t_u8 ac_idx;

    ENTER();

    if (!pwmm_ie || priv->wmm_enabled == MFALSE) {
        /* WMM is not enabled, just set the defaults and return */
        wlan_wmm_default_queue_priorities(priv);
        LEAVE();
        return;
    }

    HEXDUMP("WMM: setup_queue_priorities: param IE",
            (t_u8 *) pwmm_ie, sizeof(IEEEtypes_WmmParameter_t));

    PRINTM(MINFO, "WMM Parameter IE: version=%d, "
           "qos_info Parameter Set Count=%d, Reserved=%#x\n",
           pwmm_ie->vend_hdr.version, pwmm_ie->qos_info.para_set_count,
           pwmm_ie->reserved);

    for (num_ac = 0; num_ac < NELEMENTS(pwmm_ie->ac_params); num_ac++) {
        cw_min = (1 << pwmm_ie->ac_params[num_ac].ecw.ecw_min) - 1;
        avg_back_off
            = (cw_min >> 1) + pwmm_ie->ac_params[num_ac].aci_aifsn.aifsn;

        ac_idx = wmm_aci_to_qidx_map[pwmm_ie->ac_params[num_ac].aci_aifsn.aci];
        priv->wmm.queue_priority[ac_idx] = ac_idx;
        tmp[ac_idx] = avg_back_off;

        PRINTM(MCMND, "WMM: CWmax=%d CWmin=%d Avg Back-off=%d\n",
               (1 << pwmm_ie->ac_params[num_ac].ecw.ecw_max) - 1,
               cw_min, avg_back_off);
        PRINTM_AC(&pwmm_ie->ac_params[num_ac]);
    }

    HEXDUMP("WMM: avg_back_off", (t_u8 *) tmp, sizeof(tmp));
    HEXDUMP("WMM: queue_priority", priv->wmm.queue_priority,
            sizeof(priv->wmm.queue_priority));

    /* Bubble sort */
    for (i = 0; i < num_ac; i++) {
        for (j = 1; j < num_ac - i; j++) {
            if (tmp[j - 1] > tmp[j]) {
                SWAP_U16(tmp[j - 1], tmp[j]);
                SWAP_U8(priv->wmm.queue_priority[j - 1],
                        priv->wmm.queue_priority[j]);
            } else if (tmp[j - 1] == tmp[j]) {
                if (priv->wmm.queue_priority[j - 1]
                    < priv->wmm.queue_priority[j]) {
                    SWAP_U8(priv->wmm.queue_priority[j - 1],
                            priv->wmm.queue_priority[j]);
                }
            }
        }
    }

    wlan_wmm_queue_priorities_tid(priv, priv->wmm.queue_priority);

    HEXDUMP("WMM: avg_back_off, sort", (t_u8 *) tmp, sizeof(tmp));
    DBG_HEXDUMP(MCMD_D, "WMM: queue_priority, sort", priv->wmm.queue_priority,
                sizeof(priv->wmm.queue_priority));
    LEAVE();
}

/**
 *  @brief Evaluate whether or not an AC is to be downgraded
 *
 *  @param priv     Pointer to the mlan_private driver data struct
 *  @param eval_ac  AC to evaluate for downgrading
 *
 *  @return WMM AC  The eval_ac traffic is to be sent on.
 */
static mlan_wmm_ac_e
wlan_wmm_eval_downgrade_ac(pmlan_private priv, mlan_wmm_ac_e eval_ac)
{
    int down_ac;
    mlan_wmm_ac_e ret_ac;
    WmmAcStatus_t *pac_status;

    ENTER();

    pac_status = &priv->wmm.ac_status[eval_ac];

    if (pac_status->disabled == MFALSE) {
        LEAVE();
        /* Okay to use this AC, its enabled */
        return eval_ac;
    }

    /* Setup a default return value of the lowest priority */
    ret_ac = WMM_AC_BK;

    /* 
     *  Find the highest AC that is enabled and does not require admission
     *    control.  The spec disallows downgrading to an AC, which is enabled
     *    due to a completed admission control.  Unadmitted traffic is not
     *    to be sent on an AC with admitted traffic.
     */
    for (down_ac = WMM_AC_BK; down_ac < eval_ac; down_ac++) {
        pac_status = &priv->wmm.ac_status[down_ac];

        if ((pac_status->disabled == MFALSE)
            && (pac_status->flow_required == MFALSE))
            /* AC is enabled and does not require admission control */
            ret_ac = (mlan_wmm_ac_e) down_ac;
    }

    LEAVE();
    return ret_ac;
}

/**
 *  @brief Downgrade WMM priority queue
 *
 *  @param priv     Pointer to the mlan_private driver data struct
 *
 *  @return         N/A
 */
void
wlan_wmm_setup_ac_downgrade(pmlan_private priv)
{
    int ac_val;

    ENTER();

    PRINTM(MINFO, "WMM: AC Priorities: BK(0), BE(1), VI(2), VO(3)\n");

    if (priv->wmm_enabled == MFALSE) {
        /* WMM is not enabled, default priorities */
        for (ac_val = WMM_AC_BK; ac_val <= WMM_AC_VO; ac_val++) {
            priv->wmm.ac_down_graded_vals[ac_val] = (mlan_wmm_ac_e) ac_val;
        }
    } else {
        for (ac_val = WMM_AC_BK; ac_val <= WMM_AC_VO; ac_val++) {
            priv->wmm.ac_down_graded_vals[ac_val]
                = wlan_wmm_eval_downgrade_ac(priv, (mlan_wmm_ac_e) ac_val);
            PRINTM(MINFO, "WMM: AC PRIO %d maps to %d\n",
                   ac_val, priv->wmm.ac_down_graded_vals[ac_val]);
        }
    }

    LEAVE();
}

/**
 *  @brief Convert the IP TOS field to an WMM AC Queue assignment
 *
 *  @param pmadapter A pointer to mlan_adapter structure
 *  @param tos       IP TOS field
 *
 *  @return     WMM AC Queue mapping of the IP TOS field
 */
static mlan_wmm_ac_e INLINE
wlan_wmm_convert_tos_to_ac(pmlan_adapter pmadapter, t_u32 tos)
{
    ENTER();

    if (tos >= NELEMENTS(tos_to_ac)) {
        LEAVE();
        return WMM_AC_BE;
    }

    LEAVE();
    return tos_to_ac[tos];
}

/**
 *  @brief  Evaluate a given TID and downgrade it to a lower TID if the
 *          WMM Parameter IE received from the AP indicates that the AP
 *          is disabled (due to call admission control (ACM bit). Mapping
 * 			of TID to AC is taken care internally
 *
 *  @param priv		Pointer to the mlan_private data struct
 *  @param tid  	tid to evaluate for downgrading
 *
 *  @return       Same tid as input if downgrading not required or
 *                the tid the traffic for the given tid should be downgraded to
 */
static t_u8 INLINE
wlan_wmm_downgrade_tid(pmlan_private priv, t_u32 tid)
{
    mlan_wmm_ac_e ac_down;
    pmlan_adapter pmadapter = priv->adapter;

    ENTER();

    ac_down =
        priv->wmm.ac_down_graded_vals[wlan_wmm_convert_tos_to_ac(pmadapter,
                                                                 tid)];
    LEAVE();
    /* 
     * Send the index to tid array, picking from the array will be
     * taken care by dequeuing function
     */
    if (tid == 1 || tid == 2)
        return ac_to_tid[ac_down][(tid + 1) % 2];
    else if (tid >= MAX_NUM_TID)
        return ac_to_tid[ac_down][0];
    else
        return ac_to_tid[ac_down][tid % 2];
}

/********************************************************
    Global Functions
********************************************************/
/**
 *  @brief Initialize the WMM state information and the WMM data path queues.
 *
 *  @param pmadapter  Pointer to the mlan_adapter data structure
 *
 *  @return         N/A
 */
t_void
wlan_wmm_init(pmlan_adapter pmadapter)
{
    int i, j;
    pmlan_private priv;

    ENTER();

    for (j = 0; j < MLAN_MAX_BSS_NUM; ++j) {
        if ((priv = pmadapter->priv[j])) {
            for (i = 0; i < MAX_NUM_TID; ++i) {
                priv->aggr_prio_tbl[i].amsdu = BA_STREAM_NOT_ALLOWED;
                priv->aggr_prio_tbl[i].ampdu_ap =
                    priv->aggr_prio_tbl[i].ampdu_user = tos_to_tid_inv[i];
                priv->wmm.pkts_queued[i] = 0;
                priv->wmm.packets_out[i] = 0;
                priv->wmm.packets_in[i] = 0;
                priv->wmm.tid_tbl_ptr[i].ra_list_curr = MNULL;
            }

            priv->aggr_prio_tbl[6].ampdu_ap
                = priv->aggr_prio_tbl[6].ampdu_user = BA_STREAM_NOT_ALLOWED;

            priv->aggr_prio_tbl[7].ampdu_ap
                = priv->aggr_prio_tbl[7].ampdu_user = BA_STREAM_NOT_ALLOWED;

            priv->add_ba_param.timeout = MLAN_DEFAULT_BLOCK_ACK_TIMEOUT;
            priv->add_ba_param.tx_win_size = MLAN_AMPDU_DEF_TXWINSIZE;
            priv->add_ba_param.rx_win_size = MLAN_AMPDU_DEF_RXWINSIZE;
            memset(priv->adapter, priv->rx_seq, 0xff, sizeof(priv->rx_seq));
            wlan_wmm_default_queue_priorities(priv);
        }
    }

    LEAVE();
}

/**
 *  @brief Setup the queue priorities and downgrade any queues as required
 *         by the WMM info.  Setups default values if WMM is not active
 *         for this association.
 *
 *  @param priv     Pointer to the mlan_private driver data struct
 *
 *  @return         N/A
 */
void
wlan_wmm_setup_queues(pmlan_private priv)
{
    ENTER();
    wlan_wmm_setup_queue_priorities(priv, MNULL);
    wlan_wmm_setup_ac_downgrade(priv);
    LEAVE();
}

#ifdef STA_SUPPORT
/**
 *  @brief  Send a command to firmware to retrieve the current WMM status
 *
 *  @param priv     Pointer to the mlan_private driver data struct
 *
 *  @return         MLAN_STATUS_SUCCESS; MLAN_STATUS_FAILURE
 */
mlan_status
wlan_cmd_wmm_status_change(pmlan_private priv)
{
    return wlan_prepare_cmd(priv, HostCmd_CMD_WMM_GET_STATUS, 0, 0, 0, MNULL);
}
#endif

/**
 *  @brief Check if wmm TX queue is empty
 *
 *  @param pmadapter  Pointer to the mlan_adapter driver data struct
 *
 *  @return         MFALSE if not empty; MTRUE if empty
 */
int
wlan_wmm_lists_empty(pmlan_adapter pmadapter)
{
    int j;
    pmlan_private priv;

    ENTER();

    for (j = 0; j < MLAN_MAX_BSS_NUM; ++j) {
        if ((priv = pmadapter->priv[j])) {
            if ((priv->port_ctrl_mode == MTRUE) && (priv->port_open == MFALSE)) {
                PRINTM(MINFO,
                       "wmm_lists_empty: PORT_CLOSED Ignore pkts from BSS%d\n",
                       j);
                continue;
            }

            if (util_scalar_read(pmadapter->pmoal_handle,
                                 &priv->wmm.tx_pkts_queued,
                                 pmadapter->callbacks.moal_spin_lock,
                                 pmadapter->callbacks.moal_spin_unlock)) {
                LEAVE();
                return MFALSE;
            }
        }
    }

    LEAVE();
    return MTRUE;
}

/**
 *  @brief Delete packets in RA node
 *
 *  @param priv			Pointer to the mlan_private driver data struct
 *  @param ra_list	    	Pointer to raListTbl
 *
 *  @return		N/A
 */
static INLINE void
wlan_wmm_del_pkts_in_ralist_node(pmlan_private priv, raListTbl * ra_list)
{
    pmlan_buffer pmbuf;
    pmlan_adapter pmadapter = priv->adapter;

    ENTER();
    while ((pmbuf =
            (pmlan_buffer) util_peek_list(pmadapter->pmoal_handle,
                                          &ra_list->buf_head, MNULL, MNULL))) {
        util_unlink_list(pmadapter->pmoal_handle, &ra_list->buf_head,
                         (pmlan_linked_list) pmbuf, MNULL, MNULL);
        wlan_write_data_complete(pmadapter, pmbuf, MLAN_STATUS_FAILURE);
    }
    util_free_list_head((t_void *) pmadapter->pmoal_handle, &ra_list->buf_head,
                        pmadapter->callbacks.moal_free_lock);

    LEAVE();
}

/**
 *  @brief Delete packets in RA list
 *
 *  @param priv			Pointer to the mlan_private driver data struct
 *  @param ra_list_head	ra list header
 *
 *  @return		N/A
 */
static INLINE void
wlan_wmm_del_pkts_in_ralist(pmlan_private priv, mlan_list_head * ra_list_head)
{
    raListTbl *ra_list;

    ENTER();

    ra_list =
        (raListTbl *) util_peek_list(priv->adapter->pmoal_handle, ra_list_head,
                                     MNULL, MNULL);

    while (ra_list && ra_list != (raListTbl *) ra_list_head) {
        wlan_wmm_del_pkts_in_ralist_node(priv, ra_list);

        ra_list = ra_list->pnext;
    }

    LEAVE();
}

/**
 *  @brief Clean up the wmm queue
 *
 *  @param priv  Pointer to the mlan_private driver data struct
 *
 *  @return      N/A
 */
void
wlan_wmm_cleanup_queues(pmlan_private priv)
{
    int i;

    ENTER();

    for (i = 0; i < MAX_NUM_TID; i++) {
        wlan_wmm_del_pkts_in_ralist(priv, &priv->wmm.tid_tbl_ptr[i].ra_list);
        priv->wmm.pkts_queued[i] = 0;
    }
    util_scalar_write(priv->adapter->pmoal_handle, &priv->wmm.tx_pkts_queued, 0,
                      MNULL, MNULL);
    util_scalar_write(priv->adapter->pmoal_handle,
                      &priv->wmm.highest_queued_prio, HIGH_PRIO_TID, MNULL,
                      MNULL);

    LEAVE();
}

/**
 *  @brief Delete all route address from RA list
 *
 *  @param priv     Pointer to the mlan_private driver data struct
 *
 *  @return         N/A
 */
void
wlan_wmm_delete_all_ralist(pmlan_private priv)
{
    raListTbl *ra_list;
    int i;
    pmlan_adapter pmadapter = priv->adapter;

    ENTER();

    for (i = 0; i < MAX_NUM_TID; ++i) {
        PRINTM(MINFO, "RAList: Freeing buffers for TID %d\n", i);
        while ((ra_list = (raListTbl *) util_peek_list(pmadapter->pmoal_handle,
                                                       &priv->wmm.
                                                       tid_tbl_ptr[i].ra_list,
                                                       MNULL, MNULL))) {
            util_unlink_list(pmadapter->pmoal_handle,
                             &priv->wmm.tid_tbl_ptr[i].ra_list,
                             (pmlan_linked_list) ra_list, MNULL, MNULL);

            pmadapter->callbacks.moal_mfree(pmadapter->pmoal_handle,
                                            (t_u8 *) ra_list);
        }

        util_init_list((pmlan_linked_list)
                       & priv->wmm.tid_tbl_ptr[i].ra_list);
        priv->wmm.tid_tbl_ptr[i].ra_list_curr = MNULL;
    }

    LEAVE();
}

/**
 *   @brief Get ralist node
 *  
 *   @param priv     Pointer to the mlan_private driver data struct
 *   @param tid      TID
 *   @param ra_addr  Pointer to the route address
 * 
 *   @return         ra_list or MNULL
 */
raListTbl *
wlan_wmm_get_ralist_node(pmlan_private priv, t_u8 tid, t_u8 * ra_addr)
{
    raListTbl *ra_list;
    ENTER();
    ra_list =
        (raListTbl *) util_peek_list(priv->adapter->pmoal_handle,
                                     &priv->wmm.tid_tbl_ptr[tid].ra_list, MNULL,
                                     MNULL);
    while (ra_list && (ra_list != (raListTbl *)
                       & priv->wmm.tid_tbl_ptr[tid].ra_list)) {
        if (!memcmp(priv->adapter, ra_list->ra, ra_addr, MLAN_MAC_ADDR_LENGTH)) {
            LEAVE();
            return ra_list;
        }
        ra_list = ra_list->pnext;
    }
    LEAVE();
    return MNULL;
}

/**
 *   @brief Get queue RA pointer
 *  
 *   @param priv     Pointer to the mlan_private driver data struct
 *   @param tid      TID
 *   @param ra_addr  Pointer to the route address
 * 
 *   @return         ra_list
 */
static raListTbl *
wlan_wmm_get_queue_raptr(pmlan_private priv, t_u8 tid, t_u8 * ra_addr)
{
    raListTbl *ra_list;

    ENTER();

    ra_list = wlan_wmm_get_ralist_node(priv, tid, ra_addr);
    if (ra_list) {
        LEAVE();
        return ra_list;
    }
    wlan_ralist_add(priv, ra_addr);

    ra_list = wlan_wmm_get_ralist_node(priv, tid, ra_addr);
    LEAVE();
    return ra_list;
}

/**
 *   @brief Check if RA list is valid or not
 *  
 *   @param priv     Pointer to the mlan_private driver data struct
 *   @param ra_list	 Pointer to raListTbl
 *   @param ptrindex TID pointer index
 * 
 *   @return         TRUE- valid. FALSE- invalid. 
 */
int
wlan_is_ralist_valid(mlan_private * priv, raListTbl * ra_list, int ptrindex)
{
    raListTbl *rlist;

    rlist =
        (raListTbl *) util_peek_list(priv->adapter->pmoal_handle,
                                     &priv->wmm.tid_tbl_ptr[ptrindex].ra_list,
                                     MNULL, MNULL);

    while (rlist && (rlist != (raListTbl *)
                     & priv->wmm.tid_tbl_ptr[ptrindex].ra_list)) {
        if (rlist == ra_list)
            return MTRUE;

        rlist = rlist->pnext;
    }

    return MFALSE;
}

/**
 *  @brief Add packet to WMM queue
 *
 *  @param pmadapter  Pointer to the mlan_adapter driver data struct
 *  @param pmbuf      Pointer to the mlan_buffer data struct
 *
 *  @return         N/A
 */
t_void
wlan_wmm_add_buf_txqueue(pmlan_adapter pmadapter, pmlan_buffer pmbuf)
{
    pmlan_private priv = pmadapter->priv[pmbuf->bss_num];
    t_u32 tid;
    raListTbl *ra_list;
    t_u8 ra[MLAN_MAC_ADDR_LENGTH], tid_down;

    ENTER();
    pmbuf->buf_type = MLAN_BUF_TYPE_DATA;
    if (!priv->media_connected) {
        PRINTM(MWARN, "Drop packet in disconnect state\n");
        wlan_write_data_complete(pmadapter, pmbuf, MLAN_STATUS_FAILURE);
        LEAVE();
        return;
    }
    tid = pmbuf->priority;
    pmadapter->callbacks.moal_spin_lock(pmadapter->pmoal_handle,
                                        priv->wmm.ra_list_spinlock);
    tid_down = wlan_wmm_downgrade_tid(priv, tid);

    /* In case of infra as we have already created the list during association
       we just don't have to call get_queue_raptr, we will have only 1 raptr
       for a tid in case of infra */
    if (!queuing_ra_based(priv)) {
        ra_list = (raListTbl *) util_peek_list(pmadapter->pmoal_handle,
                                               &priv->wmm.tid_tbl_ptr[tid_down].
                                               ra_list, MNULL, MNULL);
    } else {
        memcpy(pmadapter, ra, pmbuf->pbuf + pmbuf->data_offset,
               MLAN_MAC_ADDR_LENGTH);
        ra_list = wlan_wmm_get_queue_raptr(priv, tid_down, ra);
    }

    if (!ra_list) {
        PRINTM(MWARN, "Drop packet, ra_list=%p, "
               "media_connected=%d\n", ra_list, priv->media_connected);
        pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                              priv->wmm.ra_list_spinlock);
        wlan_write_data_complete(pmadapter, pmbuf, MLAN_STATUS_FAILURE);
        LEAVE();
        return;
    }

    PRINTM(MDAT_D, "Adding pkt to ra_list %p %p\n", ra_list, pmbuf);
    util_enqueue_list_tail(pmadapter->pmoal_handle, &ra_list->buf_head,
                           (pmlan_linked_list) pmbuf, MNULL, MNULL);

    ra_list->total_pkts_size += pmbuf->data_len;
    ra_list->packet_count++;
    priv->wmm.pkts_queued[tid_down]++;
    priv->wmm.packets_in[tid_down]++;
    // PRINTM(MERROR, "[+%d->%u] ", pmbuf->data_len, ra_list->total_pkts_size);

    util_scalar_increment(pmadapter->pmoal_handle, &priv->wmm.tx_pkts_queued,
                          MNULL, MNULL);
    /* if highest_queued_prio < prio(tid_down), set it to prio(tid_down) */
    util_scalar_conditional_write(pmadapter->pmoal_handle,
                                  &priv->wmm.highest_queued_prio,
                                  MLAN_SCALAR_COND_LESS_THAN,
                                  tos_to_tid_inv[tid_down],
                                  tos_to_tid_inv[tid_down], MNULL, MNULL);

    /* Record the current time the packet was queued; used to determine the
       amount of time the packet was queued in the driver before it was sent to 
       the firmware.  The delay is then sent along with the packet to the
       firmware for aggregate delay calculation for stats and MSDU lifetime
       expiry. */
    pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                          priv->wmm.ra_list_spinlock);
    pmadapter->callbacks.moal_get_system_time(pmadapter->pmoal_handle,
                                              &pmbuf->in_ts_sec,
                                              &pmbuf->in_ts_usec);

    LEAVE();
}

#ifdef STA_SUPPORT
static void
wlan_send_wmmac_host_event(pmlan_private priv,
                           char *typeStr,
                           t_u8 * srcAddr, t_u8 tid, t_u8 up, t_u8 status)
{
    t_u8 event_buf[100];
    mlan_event *pevent;
    t_u8 *pOutBuf;

    /* Format one of the following two output strings: ** -
       TSPEC:ADDTS_RSP:[<status code>]:TID=X:UP=Y ** - TSPEC:DELTS_RX:[<reason
       code>]:TID=X:UP=Y */
    pevent = (mlan_event *) event_buf;
    pOutBuf = pevent->event_buf;

    memcpy(priv->adapter, pOutBuf, (t_u8 *) "TSPEC:", 6);
    pOutBuf += 6;

    memcpy(priv->adapter, pOutBuf, (t_u8 *) typeStr, wlan_strlen(typeStr));
    pOutBuf += wlan_strlen(typeStr);

    *pOutBuf++ = ':';
    *pOutBuf++ = '[';

    if (status >= 100) {
        *pOutBuf++ = (status / 100) + '0';
        status = (status % 100);
    }

    if (status >= 10) {
        *pOutBuf++ = (status / 10) + '0';
        status = (status % 10);
    }

    *pOutBuf++ = status + '0';

    memcpy(priv->adapter, pOutBuf, (t_u8 *) "]:TID", 5);
    pOutBuf += 5;
    *pOutBuf++ = tid + '0';

    memcpy(priv->adapter, pOutBuf, (t_u8 *) ":UP", 3);
    pOutBuf += 3;
    *pOutBuf++ = up + '0';

    *pOutBuf = '\0';

    pevent->bss_num = priv->bss_index;
    pevent->event_id = MLAN_EVENT_ID_DRV_REPORT_STRING;
    pevent->event_len = wlan_strlen((const t_s8 *) (pevent->event_buf));

    wlan_recv_event(priv, MLAN_EVENT_ID_DRV_REPORT_STRING, pevent);
}

/**
 *  @brief Process the GET_WMM_STATUS command response from firmware
 *
 *  The GET_WMM_STATUS response may contain multiple TLVs for:
 *      - AC Queue status TLVs
 *      - Current WMM Parameter IE TLV
 *      - Admission Control action frame TLVs
 *
 *  This function parses the TLVs and then calls further functions
 *   to process any changes in the queue prioritize or state.
 *
 *  @param priv      Pointer to the mlan_private driver data struct
 *  @param ptlv      Pointer to the tlv block returned in the response.
 *  @param resp_len  Length of TLV block
 *
 *  @return MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_ret_wmm_get_status(pmlan_private priv, t_u8 * ptlv, int resp_len)
{
    t_u8 *pcurrent = ptlv;
    t_u32 tlv_len;
    t_u8 sendWmmEvent;
    MrvlIEtypes_Data_t *pTlvHdr;
    MrvlIEtypes_WmmQueueStatus_t *pTlvWmmQStatus;
    IEEEtypes_WmmParameter_t *pWmmParamIe = MNULL;
    WmmAcStatus_t *pac_status;

    MrvlIETypes_ActionFrame_t *pTlvAction;
    IEEEtypes_Action_WMM_AddTsRsp_t *pAddTsRsp;
    IEEEtypes_Action_WMM_DelTs_t *pDelTs;

    ENTER();

    sendWmmEvent = MFALSE;

    PRINTM(MINFO, "WMM: WMM_GET_STATUS cmdresp received: %d\n", resp_len);
    HEXDUMP("CMD_RESP: WMM_GET_STATUS", pcurrent, resp_len);

    while (resp_len >= sizeof(pTlvHdr->header)) {
        pTlvHdr = (MrvlIEtypes_Data_t *) pcurrent;
        tlv_len = wlan_le16_to_cpu(pTlvHdr->header.len);

        switch (wlan_le16_to_cpu(pTlvHdr->header.type)) {
        case TLV_TYPE_WMMQSTATUS:
            pTlvWmmQStatus = (MrvlIEtypes_WmmQueueStatus_t *) pTlvHdr;
            PRINTM(MEVENT, "WMM_STATUS: QSTATUS TLV: %d\n",
                   pTlvWmmQStatus->queue_index);

            PRINTM(MINFO,
                   "CMD_RESP: WMM_GET_STATUS: QSTATUS TLV: %d, %d, %d\n",
                   pTlvWmmQStatus->queue_index,
                   pTlvWmmQStatus->flow_required, pTlvWmmQStatus->disabled);

            pac_status = &priv->wmm.ac_status[pTlvWmmQStatus->queue_index];
            pac_status->disabled = pTlvWmmQStatus->disabled;
            pac_status->flow_required = pTlvWmmQStatus->flow_required;
            pac_status->flow_created = pTlvWmmQStatus->flow_created;
            break;

        case WMM_IE:
            /* 
             * Point the regular IEEE IE 2 bytes into the Marvell IE
             *   and setup the IEEE IE type and length byte fields
             */

            PRINTM(MEVENT, "WMM STATUS: WMM IE\n");

            HEXDUMP("WMM: WMM TLV:", (t_u8 *) pTlvHdr, tlv_len + 4);

            pWmmParamIe = (IEEEtypes_WmmParameter_t *) (pcurrent + 2);
            pWmmParamIe->vend_hdr.len = (t_u8) tlv_len;
            pWmmParamIe->vend_hdr.element_id = WMM_IE;

            PRINTM(MINFO, "CMD_RESP: WMM_GET_STATUS: WMM Parameter Set: %d\n",
                   pWmmParamIe->qos_info.para_set_count);

            memcpy(priv->adapter,
                   (t_u8 *) & priv->curr_bss_params.bss_descriptor.wmm_ie,
                   pWmmParamIe, pWmmParamIe->vend_hdr.len + 2);
            sendWmmEvent = MTRUE;
            break;

        case TLV_TYPE_IEEE_ACTION_FRAME:
            PRINTM(MEVENT, "WMM_STATUS: IEEE Action Frame\n");
            pTlvAction = (MrvlIETypes_ActionFrame_t *) pcurrent;

            if (pTlvAction->actionFrame.wmmAc.tspecAct.category
                == IEEE_MGMT_ACTION_CATEGORY_WMM_TSPEC) {

                switch (pTlvAction->actionFrame.wmmAc.tspecAct.action) {
                case TSPEC_ACTION_CODE_ADDTS_RSP:
                    pAddTsRsp = &pTlvAction->actionFrame.wmmAc.addTsRsp;
                    wlan_send_wmmac_host_event(priv, "ADDTS_RSP",
                                               pTlvAction->srcAddr,
                                               pAddTsRsp->tspecIE.TspecBody.
                                               TSInfo.TID,
                                               pAddTsRsp->tspecIE.TspecBody.
                                               TSInfo.UserPri,
                                               pAddTsRsp->statusCode);
                    break;

                case TSPEC_ACTION_CODE_DELTS:
                    pDelTs = &pTlvAction->actionFrame.wmmAc.delTs;
                    wlan_send_wmmac_host_event(priv, "DELTS_RX",
                                               pTlvAction->srcAddr,
                                               pDelTs->tspecIE.TspecBody.TSInfo.
                                               TID,
                                               pDelTs->tspecIE.TspecBody.TSInfo.
                                               UserPri, pDelTs->reasonCode);
                    break;

                case TSPEC_ACTION_CODE_ADDTS_REQ:
                default:
                    break;
                }
            }
            break;

        default:
            break;
        }

        pcurrent += (tlv_len + sizeof(pTlvHdr->header));
        resp_len -= (tlv_len + sizeof(pTlvHdr->header));
    }

    wlan_wmm_setup_queue_priorities(priv, pWmmParamIe);
    wlan_wmm_setup_ac_downgrade(priv);

    if (sendWmmEvent) {
        wlan_recv_event(priv, MLAN_EVENT_ID_FW_WMM_CONFIG_CHANGE, MNULL);
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Call back from the command module to allow insertion of a WMM TLV
 *
 *  If the BSS we are associating to supports WMM, add the required WMM
 *    Information IE to the association request command buffer in the form
 *    of a Marvell extended IEEE IE.
 *
 *  @param priv        Pointer to the mlan_private driver data struct
 *  @param ppAssocBuf  Output parameter: Pointer to the TLV output buffer,
 *                     modified on return to point after the appended WMM TLV
 *  @param pWmmIE      Pointer to the WMM IE for the BSS we are joining
 *  @param pHTCap      Pointer to the HT IE for the BSS we are joining
 *
 *  @return Length of data appended to the association tlv buffer
 */
t_u32
wlan_wmm_process_association_req(pmlan_private priv,
                                 t_u8 ** ppAssocBuf,
                                 IEEEtypes_WmmParameter_t * pWmmIE,
                                 IEEEtypes_HTCap_t * pHTCap)
{
    MrvlIEtypes_WmmParamSet_t *pwmm_tlv;
    t_u32 ret_len = 0;

    ENTER();

    /* Null checks */
    if (!ppAssocBuf) {
        LEAVE();
        return 0;
    }
    if (!(*ppAssocBuf)) {
        LEAVE();
        return 0;
    }

    if (!pWmmIE) {
        LEAVE();
        return 0;
    }

    PRINTM(MINFO, "WMM: process assoc req: bss->wmmIe=0x%x\n",
           pWmmIE->vend_hdr.element_id);

    if ((priv->wmm_required
         || (pHTCap && (pHTCap->ieee_hdr.element_id == HT_CAPABILITY)
             && (priv->adapter->config_bands & BAND_GN
                 || priv->adapter->config_bands & BAND_AN))
        )
        && pWmmIE->vend_hdr.element_id == WMM_IE) {
        pwmm_tlv = (MrvlIEtypes_WmmParamSet_t *) * ppAssocBuf;
        pwmm_tlv->header.type = (t_u16) wmm_info_ie[0];
        pwmm_tlv->header.type = wlan_cpu_to_le16(pwmm_tlv->header.type);
        pwmm_tlv->header.len = (t_u16) wmm_info_ie[1];
        memcpy(priv->adapter, pwmm_tlv->wmm_ie, &wmm_info_ie[2],
               pwmm_tlv->header.len);
        if (pWmmIE->qos_info.qos_uapsd)
            memcpy(priv->adapter,
                   (t_u8 *) (pwmm_tlv->wmm_ie + pwmm_tlv->header.len -
                             sizeof(priv->wmm_qosinfo)), &priv->wmm_qosinfo,
                   sizeof(priv->wmm_qosinfo));

        ret_len = sizeof(pwmm_tlv->header) + pwmm_tlv->header.len;
        pwmm_tlv->header.len = wlan_cpu_to_le16(pwmm_tlv->header.len);

        HEXDUMP("ASSOC_CMD: WMM IE", (t_u8 *) pwmm_tlv, ret_len);
        *ppAssocBuf += ret_len;
    }

    LEAVE();
    return ret_len;
}
#endif /* STA_SUPPORT */

/**
 *   @brief Compute the time delay in the driver queues for a given packet.
 *
 *   When the packet is received at the OS/Driver interface, the current
 *     time is set in the packet structure.  The difference between the present
 *     time and that received time is computed in this function and limited
 *     based on pre-compiled limits in the driver.
 *
 *   @param priv   Ptr to the mlan_private driver data struct
 *   @param pmbuf  Ptr to the mlan_buffer which has been previously timestamped
 *
 *   @return  Time delay of the packet in 2ms units after having limit applied
 */
t_u8
wlan_wmm_compute_driver_packet_delay(pmlan_private priv,
                                     const pmlan_buffer pmbuf)
{
    t_u8 ret_val = 0;
    t_u32 out_ts_sec, out_ts_usec, queue_delay;

    ENTER();

    priv->adapter->callbacks.moal_get_system_time(priv->adapter->pmoal_handle,
                                                  &out_ts_sec, &out_ts_usec);

    queue_delay = (out_ts_sec - pmbuf->in_ts_sec) * 1000;
    queue_delay += (out_ts_usec - pmbuf->in_ts_usec) / 1000;

    /* 
     * Queue delay is passed as a uint8 in units of 2ms (ms shifted
     *  by 1). Min value (other than 0) is therefore 2ms, max is 510ms.
     *
     * Pass max value if queue_delay is beyond the uint8 range
     */
    ret_val = (t_u8) (MIN(queue_delay, priv->wmm.drv_pkt_delay_max) >> 1);

    PRINTM(MINFO, "WMM: Pkt Delay: %d ms, %d ms sent to FW\n",
           queue_delay, ret_val);

    LEAVE();
    return ret_val;
}

/**
 *  @brief This function gets the highest priority list pointer
 *  
 *  @param pmadapter      A pointer to mlan_adapter
 *  @param priv           A pointer to mlan_private
 *  @param tid            A pointer to return tid
 *
 *  @return             raListTbl
 */
static raListTbl *
wlan_wmm_get_highest_priolist_ptr(pmlan_adapter pmadapter,
                                  pmlan_private * priv, int *tid)
{
    pmlan_private priv_tmp;
    raListTbl *ptr, *head;
    mlan_bssprio_node *bssprio_node, *bssprio_head;
    tid_tbl_t *tid_ptr;
    int i, j;

    ENTER();

    PRINTM(MDAT_D, "POP\n");
    for (j = MLAN_MAX_BSS_NUM - 1; j >= 0; --j) {
        if (!(util_peek_list(pmadapter->pmoal_handle,
                             &pmadapter->bssprio_tbl[j].bssprio_head,
                             pmadapter->callbacks.moal_spin_lock,
                             pmadapter->callbacks.moal_spin_unlock)))
            continue;

        if (pmadapter->bssprio_tbl[j].bssprio_cur == (mlan_bssprio_node *)
            & pmadapter->bssprio_tbl[j].bssprio_head) {
            pmadapter->bssprio_tbl[j].bssprio_cur =
                pmadapter->bssprio_tbl[j].bssprio_cur->pnext;
        }

        bssprio_head = bssprio_node = pmadapter->bssprio_tbl[j].bssprio_cur;

        do {
            priv_tmp = bssprio_node->priv;

            if ((priv_tmp->port_ctrl_mode == MTRUE)
                && (priv_tmp->port_open == MFALSE)) {
                PRINTM(MINFO, "get_highest_prio_ptr(): "
                       "PORT_CLOSED Ignore pkts from BSS%d\n",
                       priv_tmp->bss_index);
                /* Ignore data pkts from a BSS if port is closed */
                goto next_intf;
            }

            pmadapter->callbacks.moal_spin_lock(pmadapter->pmoal_handle,
                                                priv_tmp->wmm.ra_list_spinlock);

            for (i = util_scalar_read(pmadapter->pmoal_handle,
                                      &priv_tmp->wmm.highest_queued_prio,
                                      MNULL, MNULL); i >= LOW_PRIO_TID; --i) {

                tid_ptr = &(priv_tmp)->wmm.tid_tbl_ptr[tos_to_tid[i]];
                if (!util_peek_list
                    (pmadapter->pmoal_handle, &tid_ptr->ra_list, MNULL, MNULL))
                    continue;

                /* 
                 * Always choose the next ra we transmitted 
                 * last time, this way we pick the ra's in 
                 * round robin fashion.
                 */
                head = ptr = tid_ptr->ra_list_curr->pnext;
                if (ptr == (raListTbl *) & tid_ptr->ra_list)
                    head = ptr = ptr->pnext;

                do {
                    if (util_peek_list
                        (pmadapter->pmoal_handle, &ptr->buf_head, MNULL,
                         MNULL)) {
                        /* if highest_queued_prio > i, set it to i */
                        util_scalar_conditional_write(pmadapter->pmoal_handle,
                                                      &priv_tmp->wmm.
                                                      highest_queued_prio,
                                                      MLAN_SCALAR_COND_GREATER_THAN,
                                                      i, i, MNULL, MNULL);
                        *priv = priv_tmp;
                        *tid = tos_to_tid[i];
                        /* hold priv->ra_list_spinlock to maintain ptr */
                        PRINTM(MINFO, "highest prio ptr %p, tid %d\n",
                               ptr, *tid);
                        LEAVE();
                        return ptr;
                    }

                    if ((ptr = ptr->pnext) == (raListTbl *) & tid_ptr->ra_list)
                        ptr = ptr->pnext;
                } while (ptr != head);
            }

            pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                                  priv_tmp->wmm.
                                                  ra_list_spinlock);

          next_intf:
            if ((bssprio_node = bssprio_node->pnext) == (mlan_bssprio_node *)
                & pmadapter->bssprio_tbl[j].bssprio_head)
                bssprio_node = bssprio_node->pnext;
        } while (bssprio_node != bssprio_head);
    }

    LEAVE();
    return MNULL;
}

/**
 *  @brief This function gets the number of packets in the Tx queue
 *  
 *  @param priv         A pointer to mlan_private
 *  @param ptr          A pointer to RA list table
 *  @param maxBufSize   Maximum buffer size
 *
 *  @return             Packet count
 */
static int
wlan_num_pkts_in_txq(mlan_private * priv, raListTbl * ptr, int maxBufSize)
{
    int count = 0, total_size = 0;
    pmlan_buffer pmbuf;

    ENTER();

    for (pmbuf = (pmlan_buffer) ptr->buf_head.pnext;
         pmbuf != (pmlan_buffer) (&ptr->buf_head); pmbuf = pmbuf->pnext) {

        total_size += pmbuf->data_len;
        if (total_size < maxBufSize)
            ++count;
        else
            break;
    }

    LEAVE();
    return count;
}

/**
 *  @brief This function sends a single packet
 *  
 *  @param priv         A pointer to mlan_private
 *  @param ptr          A pointer to RA list table
 *  @param ptrindex     ptr's TID index
 *
 *  @return             N/A
 */
static void INLINE
wlan_send_single_packet(pmlan_private priv, raListTbl * ptr, int ptrindex)
{
    pmlan_buffer pmbuf;
    pmlan_buffer pmbuf_next;
    mlan_tx_param tx_param;
    pmlan_adapter pmadapter = priv->adapter;
    mlan_status status = MLAN_STATUS_SUCCESS;

    ENTER();

    if ((pmbuf = (pmlan_buffer) util_dequeue_list(pmadapter->pmoal_handle,
                                                  &ptr->buf_head,
                                                  MNULL, MNULL))) {
        PRINTM(MINFO, "Dequeuing the packet %p %p\n", ptr, pmbuf);
        priv->wmm.pkts_queued[ptrindex]--;
        util_scalar_decrement(pmadapter->pmoal_handle,
                              &priv->wmm.tx_pkts_queued, MNULL, MNULL);
        ptr->total_pkts_size -= pmbuf->data_len;
        pmbuf_next = (pmlan_buffer) util_peek_list(pmadapter->pmoal_handle,
                                                   &ptr->buf_head,
                                                   MNULL, MNULL);
        pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                              priv->wmm.ra_list_spinlock);

        tx_param.next_pkt_len = ((pmbuf_next)
                                 ? pmbuf_next->data_len + sizeof(TxPD) : 0);
        status = wlan_process_tx(priv, pmbuf, &tx_param);

        if (status == MLAN_STATUS_RESOURCE) {
            /** Queue the packet back at the head */
            PRINTM(MDAT_D, "Queuing pkt back to raList %p %p\n", ptr, pmbuf);
            pmadapter->callbacks.moal_spin_lock(pmadapter->pmoal_handle,
                                                priv->wmm.ra_list_spinlock);

            if (!wlan_is_ralist_valid(priv, ptr, ptrindex)) {
                pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                                      priv->wmm.
                                                      ra_list_spinlock);
                wlan_write_data_complete(pmadapter, pmbuf, MLAN_STATUS_FAILURE);
                LEAVE();
                return;
            }
            priv->wmm.pkts_queued[ptrindex]++;
            util_scalar_increment(pmadapter->pmoal_handle,
                                  &priv->wmm.tx_pkts_queued, MNULL, MNULL);
            util_enqueue_list_head(pmadapter->pmoal_handle, &ptr->buf_head,
                                   (pmlan_linked_list) pmbuf, MNULL, MNULL);

            ptr->total_pkts_size += pmbuf->data_len;
            pmbuf->flags |= MLAN_BUF_FLAG_REQUEUED_PKT;
            pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                                  priv->wmm.ra_list_spinlock);
        } else {
            pmadapter->callbacks.moal_spin_lock(pmadapter->pmoal_handle,
                                                priv->wmm.ra_list_spinlock);
            if (wlan_is_ralist_valid(priv, ptr, ptrindex)) {
                priv->wmm.packets_out[ptrindex]++;
                priv->wmm.tid_tbl_ptr[ptrindex].ra_list_curr = ptr;
            }
            pmadapter->bssprio_tbl[priv->bss_priority].bssprio_cur =
                pmadapter->bssprio_tbl[priv->bss_priority].bssprio_cur->pnext;
            pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                                  priv->wmm.ra_list_spinlock);
        }
    } else {
        pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                              priv->wmm.ra_list_spinlock);
        PRINTM(MINFO, "Nothing to send\n");
    }

    LEAVE();
}

/**
 *  @brief This function checks if this mlan_buffer is already processed.
 *  
 *  @param priv     A pointer to mlan_private
 *  @param ptr      A pointer to RA list table
 *
 *  @return 	    MTRUE or MFALSE
 */
static int INLINE
wlan_is_ptr_processed(mlan_private * priv, raListTbl * ptr)
{
    pmlan_buffer pmbuf;

    if ((pmbuf = (pmlan_buffer) util_peek_list(priv->adapter->pmoal_handle,
                                               &ptr->buf_head, MNULL, MNULL))
        && (pmbuf->flags & MLAN_BUF_FLAG_REQUEUED_PKT))
        return MTRUE;

    return MFALSE;
}

/**
 *  @brief This function sends a single packet that has been processed
 *  
 *  @param priv         A pointer to mlan_private
 *  @param ptr          A pointer to RA list table
 *  @param ptrindex     ptr's TID index
 *
 *  @return             N/A
 */
static void INLINE
wlan_send_processed_packet(pmlan_private priv, raListTbl * ptr, int ptrindex)
{
    pmlan_buffer pmbuf_next;
    mlan_tx_param tx_param;
    pmlan_buffer pmbuf;
    pmlan_adapter pmadapter = priv->adapter;
    mlan_status ret = MLAN_STATUS_FAILURE;

    if ((pmbuf = (pmlan_buffer) util_dequeue_list(pmadapter->pmoal_handle,
                                                  &ptr->buf_head,
                                                  MNULL, MNULL))) {
        pmbuf_next =
            (pmlan_buffer) util_peek_list(pmadapter->pmoal_handle,
                                          &ptr->buf_head, MNULL, MNULL);
        pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                              priv->wmm.ra_list_spinlock);
        tx_param.next_pkt_len =
            ((pmbuf_next) ? pmbuf_next->data_len + sizeof(TxPD) : 0);
        ret =
            wlan_sdio_host_to_card(pmadapter, MLAN_TYPE_DATA, pmbuf, &tx_param);
        switch (ret) {
        case MLAN_STATUS_RESOURCE:
            PRINTM(MINFO, "MLAN_STATUS_RESOURCE is returned\n");
            pmadapter->callbacks.moal_spin_lock(pmadapter->pmoal_handle,
                                                priv->wmm.ra_list_spinlock);

            if (!wlan_is_ralist_valid(priv, ptr, ptrindex)) {
                pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                                      priv->wmm.
                                                      ra_list_spinlock);
                wlan_write_data_complete(pmadapter, pmbuf, MLAN_STATUS_FAILURE);
                LEAVE();
                return;
            }
            util_enqueue_list_head(pmadapter->pmoal_handle,
                                   &ptr->buf_head,
                                   (pmlan_linked_list) pmbuf, MNULL, MNULL);

            pmbuf->flags |= MLAN_BUF_FLAG_REQUEUED_PKT;
            pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                                  priv->wmm.ra_list_spinlock);
            break;
        case MLAN_STATUS_FAILURE:
            pmadapter->data_sent = MFALSE;
            PRINTM(MERROR, "Error: Failed to write data\n");
            pmadapter->dbg.num_tx_host_to_card_failure++;
            wlan_write_data_complete(pmadapter, pmbuf, ret);
            break;
        case MLAN_STATUS_PENDING:
            pmadapter->data_sent = MFALSE;
        default:
            break;
        }
        if (ret != MLAN_STATUS_RESOURCE) {
            pmadapter->callbacks.moal_spin_lock(pmadapter->pmoal_handle,
                                                priv->wmm.ra_list_spinlock);
            if (wlan_is_ralist_valid(priv, ptr, ptrindex)) {
                priv->wmm.packets_out[ptrindex]++;
                priv->wmm.tid_tbl_ptr[ptrindex].ra_list_curr = ptr;
            }
            pmadapter->bssprio_tbl[priv->bss_priority].bssprio_cur =
                pmadapter->bssprio_tbl[priv->bss_priority].bssprio_cur->pnext;
            priv->wmm.pkts_queued[ptrindex]--;
            util_scalar_decrement(pmadapter->pmoal_handle,
                                  &priv->wmm.tx_pkts_queued, MNULL, MNULL);
            pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                                  priv->wmm.ra_list_spinlock);
        }
    } else {
        pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                              priv->wmm.ra_list_spinlock);
    }
}

/**
 *  @brief This function dequeues a packet
 *  
 *  @param pmadapter  A pointer to mlan_adapter
 *
 *  @return 	    MLAN_STATUS_SUCCESS;
 */
static int
wlan_dequeue_tx_packet(pmlan_adapter pmadapter)
{
    raListTbl *ptr;
    pmlan_private priv = MNULL;
    int ptrindex = 0;
    t_u8 ra[MLAN_MAC_ADDR_LENGTH];
    int tid_del = 0;
    int tid = 0;
    t_u8 avail_ports = 0;
    int i;

    ENTER();

    if (!(ptr = wlan_wmm_get_highest_priolist_ptr(pmadapter, &priv, &ptrindex))) {
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }

    /* Note:- Spinlock is locked in wlan_wmm_get_highest_priolist_ptr when it
       returns a pointer (for the priv it returns), and is unlocked in
       wlan_send_processed_packet, wlan_send_single_packet or
       wlan_11n_aggregate_pkt.  The spinlock would be required for some parts
       of both of function.  But, the the bulk of these function will execute
       w/o spinlock.  Unlocking the spinlock inside these function will help
       us avoid taking the spinlock again, check to see if the ptr is still
       valid and then proceed. This is done purely to increase execution time. 
     */

    /* Note:- Also, anybody adding code which does not get into
       wlan_send_processed_packet, wlan_send_single_packet, or
       wlan_11n_aggregate_pkt should make sure ra_list_spinlock is freed.
       Otherwise there would be a lock up. */

    tid = wlan_get_tid(priv->adapter, ptr);
    if (tid >= MAX_NUM_TID)
        tid = wlan_wmm_downgrade_tid(priv, tid);

    for (i = 1; i < pmadapter->mp_end_port; i++) {
        if ((pmadapter->mp_wr_bitmap >> i) & 1)
            avail_ports++;
    }

    PRINTM(MINFO,
           "mp_wr_bitmap=0x%04x avail_ports=%d tid=%d tx_eligibility[%d]=%d\n",
           pmadapter->mp_wr_bitmap, avail_ports,
           tid, tid, pmadapter->tx_eligibility[tid]);

    if (avail_ports < pmadapter->tx_eligibility[tid]) {
        pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                              priv->wmm.ra_list_spinlock);
        pmadapter->data_sent = MTRUE;
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }

    if (wlan_is_ptr_processed(priv, ptr)) {
        wlan_send_processed_packet(priv, ptr, ptrindex);
        LEAVE();
        return MLAN_STATUS_SUCCESS;
    }

    if (!ptr->is_11n_enabled || wlan_is_bastream_setup(priv, ptr, tid)
#ifdef STA_SUPPORT
        || ((priv->sec_info.ewpa_enabled == MFALSE) &&
            ((priv->sec_info.wpa_enabled
              || priv->sec_info.wpa2_enabled) &&
             priv->wpa_is_gtk_set == MFALSE))
#endif /* STA_SUPPORT */
        ) {
        wlan_send_single_packet(priv, ptr, ptrindex);
    } else {
        if (wlan_is_ampdu_allowed(priv, ptr, tid) &&
            (ptr->packet_count > ptr->ba_packet_threshold)) {
            if (wlan_is_bastream_avail(priv)) {
                PRINTM(MDATA, "BA setup threshold %d reached.\n",
                       ptr->packet_count);
                wlan_11n_create_txbastream_tbl(priv,
                                               ptr->ra, tid,
                                               BA_STREAM_SETUP_INPROGRESS);
                wlan_send_addba(priv, tid, ptr->ra);
            } else if (wlan_find_stream_to_delete(priv, ptr, tid, &tid_del, ra)) {
                PRINTM(MDATA, "tid_del=%d tid=%d\n", tid_del, tid);
                wlan_11n_create_txbastream_tbl(priv,
                                               ptr->ra, tid,
                                               BA_STREAM_SETUP_INPROGRESS);
                wlan_send_delba(priv, tid_del, ra, 1);
            }
        }
/** Minimum number of AMSDU */
#define MIN_NUM_AMSDU 2
        if (wlan_is_amsdu_allowed(priv, ptr, tid) &&
            (wlan_num_pkts_in_txq(priv, ptr,
                                  pmadapter->tx_buf_size) >= MIN_NUM_AMSDU)) {
            wlan_11n_aggregate_pkt(priv, ptr, INTF_HEADER_LEN, ptrindex);
        } else {
            wlan_send_single_packet(priv, ptr, ptrindex);
        }
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Transmit the highest priority packet awaiting in the WMM Queues
 *
 *  @param pmadapter Pointer to the mlan_adapter driver data struct
 *
 *  @return        N/A
 */
void
wlan_wmm_process_tx(pmlan_adapter pmadapter)
{
    ENTER();

    do {
        if (wlan_dequeue_tx_packet(pmadapter))
            break;
        /* Check if busy */
    } while (!pmadapter->data_sent && !pmadapter->tx_lock_flag
             && !wlan_wmm_lists_empty(pmadapter));

    LEAVE();
    return;
}

/** 
 *  @brief select wmm queue
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param tid          TID 0-7
 *
 *  @return             wmm_queue priority (0-3)
 */
t_u8
wlan_wmm_select_queue(mlan_private * pmpriv, t_u8 tid)
{
    pmlan_adapter pmadapter = pmpriv->adapter;
    t_u8 i;
    mlan_wmm_ac_e ac_down =
        pmpriv->wmm.
        ac_down_graded_vals[wlan_wmm_convert_tos_to_ac(pmadapter, tid)];

    for (i = 0; i < 4; i++) {
        if (pmpriv->wmm.queue_priority[i] == ac_down)
            return i;
    }
    return 0;
}

#ifdef STA_SUPPORT
/**
 *  @brief This function prepares the command of ADDTS
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param pdata_buf    A pointer to data buffer
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_cmd_wmm_addts_req(IN pmlan_private pmpriv,
                       OUT HostCmd_DS_COMMAND * cmd, IN t_void * pdata_buf)
{
    mlan_ds_wmm_addts *paddts = (mlan_ds_wmm_addts *) pdata_buf;
    HostCmd_DS_WMM_ADDTS_REQ *pcmd_addts = &cmd->params.add_ts;

    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_WMM_ADDTS_REQ);
    cmd->size = wlan_cpu_to_le16(sizeof(pcmd_addts->dialog_token)
                                 + sizeof(pcmd_addts->timeout_ms)
                                 + sizeof(pcmd_addts->command_result)
                                 + sizeof(pcmd_addts->ieee_status_code)
                                 + paddts->ie_data_len + S_DS_GEN);
    cmd->result = 0;

    pcmd_addts->timeout_ms = wlan_cpu_to_le32(paddts->timeout);
    pcmd_addts->dialog_token = paddts->dialog_tok;
    memcpy(pmpriv->adapter,
           pcmd_addts->tspec_data, paddts->ie_data, paddts->ie_data_len);

    LEAVE();

    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of ADDTS
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_ret_wmm_addts_req(IN pmlan_private pmpriv,
                       const IN HostCmd_DS_COMMAND * resp,
                       OUT mlan_ioctl_req * pioctl_buf)
{
    mlan_ds_wmm_cfg *pwmm = MNULL;
    mlan_ds_wmm_addts *paddts = MNULL;
    const HostCmd_DS_WMM_ADDTS_REQ *presp_addts = &resp->params.add_ts;

    ENTER();

    if (pioctl_buf) {
        pwmm = (mlan_ds_wmm_cfg *) pioctl_buf->pbuf;
        paddts = (mlan_ds_wmm_addts *) & pwmm->param.addts;
        paddts->result = presp_addts->command_result;
        paddts->dialog_tok = presp_addts->dialog_token;
        paddts->status_code = (t_u32) presp_addts->ieee_status_code;

        if (presp_addts->command_result == MLAN_CMD_RESULT_SUCCESS) {
            /* The tspecData field is potentially variable in size due to extra 
               IEs that may have been in the ADDTS response action frame.
               Calculate the data length from the firmware command response. */
            paddts->ie_data_len
                = (t_u8) (resp->size - sizeof(presp_addts->command_result)
                          - sizeof(presp_addts->timeout_ms)
                          - sizeof(presp_addts->dialog_token)
                          - sizeof(presp_addts->ieee_status_code)
                          - S_DS_GEN);

            /* Copy the TSPEC data include any extra IEs after the TSPEC */
            memcpy(pmpriv->adapter,
                   paddts->ie_data,
                   presp_addts->tspec_data, paddts->ie_data_len);
        } else {
            paddts->ie_data_len = 0;
        }
        PRINTM(MINFO, "TSPEC: ADDTS ret = %d,%d sz=%d\n",
               paddts->result, paddts->status_code, paddts->ie_data_len);

        HEXDUMP("TSPEC: ADDTS data", paddts->ie_data, paddts->ie_data_len);
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function prepares the command of DELTS
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param pdata_buf    A pointer to data buffer
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_cmd_wmm_delts_req(IN pmlan_private pmpriv,
                       OUT HostCmd_DS_COMMAND * cmd, IN t_void * pdata_buf)
{
    mlan_ds_wmm_delts *pdelts = (mlan_ds_wmm_delts *) pdata_buf;
    HostCmd_DS_WMM_DELTS_REQ *pcmd_delts = &cmd->params.del_ts;

    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_WMM_DELTS_REQ);
    cmd->size = wlan_cpu_to_le16(sizeof(pcmd_delts->dialog_token)
                                 + sizeof(pcmd_delts->command_result)
                                 + sizeof(pcmd_delts->ieee_reason_code)
                                 + pdelts->ie_data_len + S_DS_GEN);
    cmd->result = 0;
    pcmd_delts->ieee_reason_code = (t_u8) pdelts->status_code;
    memcpy(pmpriv->adapter,
           pcmd_delts->tspec_data, pdelts->ie_data, pdelts->ie_data_len);

    LEAVE();

    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of DELTS
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_ret_wmm_delts_req(IN pmlan_private pmpriv,
                       const IN HostCmd_DS_COMMAND * resp,
                       OUT mlan_ioctl_req * pioctl_buf)
{
    mlan_ds_wmm_cfg *pwmm;
    IEEEtypes_WMM_TSPEC_t *pTspecIE;
    const HostCmd_DS_WMM_DELTS_REQ *presp_delts = &resp->params.del_ts;

    ENTER();

    if (pioctl_buf) {
        pwmm = (mlan_ds_wmm_cfg *) pioctl_buf->pbuf;
        pwmm->param.delts.result = presp_delts->command_result;

        PRINTM(MINFO, "TSPEC: DELTS result = %d\n",
               presp_delts->command_result);

        if (presp_delts->command_result == 0) {
            pTspecIE = (IEEEtypes_WMM_TSPEC_t *) presp_delts->tspec_data;
            wlan_send_wmmac_host_event(pmpriv,
                                       "DELTS_TX",
                                       MNULL,
                                       pTspecIE->TspecBody.TSInfo.TID,
                                       pTspecIE->TspecBody.TSInfo.UserPri,
                                       presp_delts->ieee_reason_code);

        }
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function prepares the command of WMM_QUEUE_CONFIG
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param pdata_buf    A pointer to data buffer
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_cmd_wmm_queue_config(IN pmlan_private pmpriv,
                          OUT HostCmd_DS_COMMAND * cmd, IN t_void * pdata_buf)
{
    mlan_ds_wmm_queue_config *pqcfg = (mlan_ds_wmm_queue_config *) pdata_buf;
    HostCmd_DS_WMM_QUEUE_CONFIG *pcmd_qcfg = &cmd->params.queue_config;

    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_WMM_QUEUE_CONFIG);
    cmd->size = wlan_cpu_to_le16(sizeof(pcmd_qcfg->action)
                                 + sizeof(pcmd_qcfg->access_category)
                                 + sizeof(pcmd_qcfg->msdu_lifetime_expiry)
                                 + S_DS_GEN);
    cmd->result = 0;

    pcmd_qcfg->action = pqcfg->action;
    pcmd_qcfg->access_category = pqcfg->access_category;
    pcmd_qcfg->msdu_lifetime_expiry =
        wlan_cpu_to_le16(pqcfg->msdu_lifetime_expiry);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of WMM_QUEUE_CONFIG
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_ret_wmm_queue_config(IN pmlan_private pmpriv,
                          const IN HostCmd_DS_COMMAND * resp,
                          OUT mlan_ioctl_req * pioctl_buf)
{
    mlan_ds_wmm_cfg *pwmm = MNULL;
    const HostCmd_DS_WMM_QUEUE_CONFIG *presp_qcfg = &resp->params.queue_config;

    ENTER();

    if (pioctl_buf) {
        pwmm = (mlan_ds_wmm_cfg *) pioctl_buf->pbuf;
        pwmm->param.q_cfg.action = presp_qcfg->action;
        pwmm->param.q_cfg.access_category = presp_qcfg->access_category;
        pwmm->param.q_cfg.msdu_lifetime_expiry =
            wlan_le16_to_cpu(presp_qcfg->msdu_lifetime_expiry);
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function prepares the command of WMM_QUEUE_STATS
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param pdata_buf    A pointer to data buffer
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_cmd_wmm_queue_stats(IN pmlan_private pmpriv,
                         OUT HostCmd_DS_COMMAND * cmd, IN t_void * pdata_buf)
{
    mlan_ds_wmm_queue_stats *pqstats = (mlan_ds_wmm_queue_stats *) pdata_buf;
    HostCmd_DS_WMM_QUEUE_STATS *pcmd_qstats = &cmd->params.queue_stats;
    t_u8 id;

    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_WMM_QUEUE_STATS);
    cmd->size = wlan_cpu_to_le16(sizeof(HostCmd_DS_WMM_QUEUE_STATS)
                                 + S_DS_GEN);
    cmd->result = 0;

    pcmd_qstats->action = pqstats->action;
    pcmd_qstats->access_category = pqstats->access_category;
    pcmd_qstats->pkt_count = wlan_cpu_to_le16(pqstats->pkt_count);
    pcmd_qstats->pkt_loss = wlan_cpu_to_le16(pqstats->pkt_loss);
    pcmd_qstats->avg_queue_delay = wlan_cpu_to_le32(pqstats->avg_queue_delay);
    pcmd_qstats->avg_tx_delay = wlan_cpu_to_le32(pqstats->avg_tx_delay);
    pcmd_qstats->used_time = wlan_cpu_to_le16(pqstats->used_time);
    pcmd_qstats->policed_time = wlan_cpu_to_le16(pqstats->policed_time);
    for (id = 0; id < MLAN_WMM_STATS_PKTS_HIST_BINS; id++) {
        pcmd_qstats->delay_histogram[id] =
            wlan_cpu_to_le16(pqstats->delay_histogram[id]);
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of WMM_QUEUE_STATS
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_ret_wmm_queue_stats(IN pmlan_private pmpriv,
                         const IN HostCmd_DS_COMMAND * resp,
                         OUT mlan_ioctl_req * pioctl_buf)
{
    mlan_ds_wmm_cfg *pwmm = MNULL;
    mlan_ds_wmm_queue_stats *pqstats = MNULL;
    const HostCmd_DS_WMM_QUEUE_STATS *presp_qstats = &resp->params.queue_stats;
    t_u8 id;

    ENTER();

    if (pioctl_buf) {
        pwmm = (mlan_ds_wmm_cfg *) pioctl_buf->pbuf;
        pqstats = (mlan_ds_wmm_queue_stats *) & pwmm->param.q_stats;

        pqstats->action = presp_qstats->action;
        pqstats->access_category = presp_qstats->access_category;
        pqstats->pkt_count = wlan_le16_to_cpu(presp_qstats->pkt_count);
        pqstats->pkt_loss = wlan_le16_to_cpu(presp_qstats->pkt_loss);
        pqstats->avg_queue_delay
            = wlan_le32_to_cpu(presp_qstats->avg_queue_delay);
        pqstats->avg_tx_delay = wlan_le32_to_cpu(presp_qstats->avg_tx_delay);
        pqstats->used_time = wlan_le16_to_cpu(presp_qstats->used_time);
        pqstats->policed_time = wlan_le16_to_cpu(presp_qstats->policed_time);
        for (id = 0; id < MLAN_WMM_STATS_PKTS_HIST_BINS; id++) {
            pqstats->delay_histogram[id]
                = wlan_le16_to_cpu(presp_qstats->delay_histogram[id]);
        }
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function prepares the command of WMM_TS_STATUS
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param pdata_buf    A pointer to data buffer
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_cmd_wmm_ts_status(IN pmlan_private pmpriv,
                       OUT HostCmd_DS_COMMAND * cmd, IN t_void * pdata_buf)
{
    mlan_ds_wmm_ts_status *pts_status = (mlan_ds_wmm_ts_status *) pdata_buf;
    HostCmd_DS_WMM_TS_STATUS *pcmd_ts_status = &cmd->params.ts_status;

    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_WMM_TS_STATUS);
    cmd->size = wlan_cpu_to_le16(sizeof(HostCmd_DS_WMM_TS_STATUS)
                                 + S_DS_GEN);
    cmd->result = 0;

    memcpy(pmpriv->adapter, (t_void *) pcmd_ts_status, (t_void *) pts_status,
           sizeof(HostCmd_DS_WMM_TS_STATUS));

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of WMM_TS_STATUS
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_ret_wmm_ts_status(IN pmlan_private pmpriv,
                       IN HostCmd_DS_COMMAND * resp,
                       OUT mlan_ioctl_req * pioctl_buf)
{
    mlan_ds_wmm_cfg *pwmm = MNULL;
    HostCmd_DS_WMM_TS_STATUS *presp_ts_status = &resp->params.ts_status;

    ENTER();

    if (pioctl_buf) {
        pwmm = (mlan_ds_wmm_cfg *) pioctl_buf->pbuf;
        presp_ts_status->medium_time
            = wlan_le16_to_cpu(presp_ts_status->medium_time);
        memcpy(pmpriv->adapter,
               (t_void *) & pwmm->param.ts_status,
               (t_void *) presp_ts_status, sizeof(mlan_ds_wmm_ts_status));
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}
#endif /* STA_SUPPORT */
