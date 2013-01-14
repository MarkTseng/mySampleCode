/** @file mlan_sta_tx.c
 *
 *  @brief This file contains the handling of data packet
 *  transmission in MLAN module.
 * 
 *  Copyright (C) 2008-2011, Marvell International Ltd. 
 *  All Rights Reserved
 */

/********************************************************
Change log:
    10/21/2008: initial version
********************************************************/

#include "mlan.h"
#include "mlan_join.h"
#include "mlan_util.h"
#include "mlan_fw.h"
#include "mlan_main.h"
#include "mlan_wmm.h"
#include "mlan_sdio.h"

/********************************************************
		Local Variables
********************************************************/

/********************************************************
		Global Variables
********************************************************/

/********************************************************
		Local Functions
********************************************************/

/********************************************************
		Global functions
********************************************************/
/** 
 *  @brief This function fill the txpd for tx packet  
 *  
 *  @param priv	   A pointer to mlan_private structure
 *  @param pmbuf   A pointer to the mlan_buffer for process
 *
 *  @return 	   headptr or MNULL
 */
t_void *
mlan_process_sta_txpd(IN t_void * priv, IN pmlan_buffer pmbuf)
{
    mlan_private *pmpriv = (mlan_private *) priv;
    pmlan_adapter pmadapter = pmpriv->adapter;
    TxPD *plocal_tx_pd;
    t_u8 *head_ptr = MNULL;

    if (!pmbuf->data_len) {
        PRINTM(MERROR, "STA Tx Error: Invalid packet length: %d\n",
               pmbuf->data_len);
        pmbuf->status_code = MLAN_ERROR_PKT_SIZE_INVALID;
        goto done;
    }

    if (pmbuf->data_offset < (sizeof(TxPD) + INTF_HEADER_LEN + DMA_ALIGNMENT)) {
        PRINTM(MERROR, "not enough space for TxPD: %d\n", pmbuf->data_len);
        pmbuf->status_code = MLAN_ERROR_PKT_SIZE_INVALID;
        goto done;
    }

    /* head_ptr should be aligned */
    head_ptr =
        pmbuf->pbuf + pmbuf->data_offset - sizeof(TxPD) - INTF_HEADER_LEN;
    head_ptr = (t_u8 *) ((t_ptr) head_ptr & ~((t_ptr) (DMA_ALIGNMENT - 1)));

    plocal_tx_pd = (TxPD *) (head_ptr + INTF_HEADER_LEN);
    memset(pmadapter, plocal_tx_pd, 0, sizeof(TxPD));
    /* Set the BSS number to TxPD */
    plocal_tx_pd->bss_num = GET_BSS_NUM(pmpriv);

    plocal_tx_pd->tx_pkt_length = (t_u16) pmbuf->data_len;

    plocal_tx_pd->priority = (t_u8) pmbuf->priority;
    plocal_tx_pd->pkt_delay_2ms =
        wlan_wmm_compute_driver_packet_delay(pmpriv, pmbuf);

    if (plocal_tx_pd->priority < NELEMENTS(pmpriv->wmm.user_pri_pkt_tx_ctrl))
        /* 
         * Set the priority specific tx_control field, setting of 0 will
         *   cause the default value to be used later in this function
         */
        plocal_tx_pd->tx_control
            = pmpriv->wmm.user_pri_pkt_tx_ctrl[plocal_tx_pd->priority];
    if (pmadapter->pps_uapsd_mode) {
        if (MTRUE == wlan_check_last_packet_indication(pmpriv)) {
            pmadapter->tx_lock_flag = MTRUE;
            plocal_tx_pd->flags = MRVDRV_TxPD_POWER_MGMT_LAST_PACKET;
        }
    }

    /* Offset of actual data */
    plocal_tx_pd->tx_pkt_offset =
        (t_u16) ((t_ptr) pmbuf->pbuf + pmbuf->data_offset -
                 (t_ptr) plocal_tx_pd);

    if (!plocal_tx_pd->tx_control) {
        /* TxCtrl set by user or default */
        plocal_tx_pd->tx_control = pmpriv->pkt_tx_ctrl;
    }

    endian_convert_TxPD(plocal_tx_pd);

    /* Adjust the data offset and length to include TxPD in pmbuf */
    pmbuf->data_len += pmbuf->data_offset;
    pmbuf->data_offset = (t_u32) (head_ptr - pmbuf->pbuf);
    pmbuf->data_len -= pmbuf->data_offset;

  done:
    return head_ptr;
}

/** 
 *  @brief This function tells firmware to send a NULL data packet.
 *  
 *  @param priv     A pointer to mlan_private structure
 *  @param flags    Transmit Pkt Flags
 *
 *  @return 	    N/A
 */
mlan_status
wlan_send_null_packet(pmlan_private priv, t_u8 flags)
{
    pmlan_adapter pmadapter = priv->adapter;
    TxPD *ptx_pd;
/* sizeof(TxPD) + Interface specific header */
#define NULL_PACKET_HDR 64
    t_u32 data_len = NULL_PACKET_HDR;
    pmlan_buffer pmbuf = MNULL;
    t_u8 *ptr;
    mlan_status ret = MLAN_STATUS_SUCCESS;
    t_u32 sec, usec;

    ENTER();

    if (pmadapter->surprise_removed == MTRUE) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    if (priv->media_connected == MFALSE) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    if (pmadapter->data_sent == MTRUE) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    pmbuf = wlan_alloc_mlan_buffer(pmadapter, data_len, 0, MTRUE);
    if (!pmbuf) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
    memset(pmadapter, pmbuf->pbuf, 0, data_len);
    pmbuf->bss_num = priv->bss_index;
    pmbuf->buf_type = MLAN_BUF_TYPE_DATA;
    ptr = pmbuf->pbuf + pmbuf->data_offset;
    pmbuf->data_len = sizeof(TxPD) + INTF_HEADER_LEN;
    ptx_pd = (TxPD *) (ptr + INTF_HEADER_LEN);
    ptx_pd->tx_control = priv->pkt_tx_ctrl;
    ptx_pd->flags = flags;
    ptx_pd->priority = WMM_HIGHEST_PRIORITY;
    ptx_pd->tx_pkt_offset = sizeof(TxPD);
    /* Set the BSS number to TxPD */
    ptx_pd->bss_num = GET_BSS_NUM(priv);

    endian_convert_TxPD(ptx_pd);

    ret = wlan_sdio_host_to_card(pmadapter, MLAN_TYPE_DATA, pmbuf, MNULL);

    switch (ret) {
    case MLAN_STATUS_RESOURCE:
        pmadapter->data_sent = MTRUE;
        /* Fall through FAILURE handling */
    case MLAN_STATUS_FAILURE:
        wlan_free_mlan_buffer(pmadapter, pmbuf);
        PRINTM(MERROR, "STA Tx Error: Failed to send NULL packet!\n");
        pmadapter->dbg.num_tx_host_to_card_failure++;
        goto done;
    case MLAN_STATUS_SUCCESS:
        wlan_free_mlan_buffer(pmadapter, pmbuf);
        PRINTM(MDATA, "STA Tx: Successfully send the NULL packet\n");
        pmadapter->tx_lock_flag = MTRUE;
        break;
    case MLAN_STATUS_PENDING:
        break;
    default:
        break;
    }

    pmadapter->callbacks.moal_get_system_time(pmadapter->pmoal_handle, &sec,
                                              &usec);
    PRINTM(MDATA, "%lu.%06lu : Null data => FW\n", sec, usec);
    DBG_HEXDUMP(MDAT_D, "Null data", ptr, sizeof(TxPD) + INTF_HEADER_LEN);
  done:
    LEAVE();
    return ret;
}

/** 
 *  @brief This function checks if we need to send last packet indication.
 *  
 *  @param priv    A pointer to mlan_private structure
 *
 *  @return 	   MTRUE or MFALSE
 */
t_u8
wlan_check_last_packet_indication(pmlan_private priv)
{
    pmlan_adapter pmadapter = priv->adapter;
    t_u8 ret = MFALSE;
    t_u8 prop_ps = MTRUE;

    ENTER();

    if (!pmadapter->sleep_period.period) {
        LEAVE();
        return ret;
    }
    if (wlan_bypass_tx_list_empty(pmadapter) && wlan_wmm_lists_empty(pmadapter)) {
        if ((
                /* TODO */
                (priv->curr_bss_params.wmm_uapsd_enabled == MTRUE) &&
                priv->wmm_qosinfo) || prop_ps)

            ret = MTRUE;
    }
    if (ret && !pmadapter->cmd_sent && !pmadapter->curr_cmd
        && !IS_COMMAND_PENDING(pmadapter)) {
        pmadapter->delay_null_pkt = MFALSE;
        ret = MTRUE;
    } else {
        ret = MFALSE;
        pmadapter->delay_null_pkt = MTRUE;
    }
    LEAVE();
    return ret;
}
