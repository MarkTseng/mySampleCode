/** @file mlan_uap_ioctl.c
 *
 *  @brief This file contains the handling of AP mode ioctls 
 * 
 *  Copyright (C) 2009-2010, Marvell International Ltd. 
 *  All Rights Reserved
 */

/********************************************************
Change log:
    02/05/2009: initial version
********************************************************/

#include "mlan.h"
#include "mlan_util.h"
#include "mlan_fw.h"
#ifdef STA_SUPPORT
#include "mlan_join.h"
#endif
#include "mlan_main.h"
#include "mlan_uap.h"
#include "mlan_sdio.h"
#include "mlan_11n.h"
#include "mlan_fw.h"

/********************************************************
    Local Functions
********************************************************/
/** 
 *  @brief Stop BSS
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
static mlan_status
wlan_uap_bss_ioctl_stop(IN pmlan_adapter pmadapter,
                        IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    pmlan_private pmpriv = pmadapter->priv[pioctl_req->bss_num];

    ENTER();

    ret = wlan_prepare_cmd(pmpriv,
                           HOST_CMD_APCMD_BSS_STOP,
                           HostCmd_ACT_GEN_SET,
                           0, (t_void *) pioctl_req, MNULL);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/** 
 *  @brief Start BSS
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
static mlan_status
wlan_uap_bss_ioctl_start(IN pmlan_adapter pmadapter,
                         IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    pmlan_private pmpriv = pmadapter->priv[pioctl_req->bss_num];

    ENTER();

    ret = wlan_prepare_cmd(pmpriv,
                           HOST_CMD_APCMD_BSS_START,
                           HostCmd_ACT_GEN_SET,
                           0, (t_void *) pioctl_req, MNULL);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/** 
 *  @brief reset BSS
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
static mlan_status
wlan_uap_bss_ioctl_reset(IN pmlan_adapter pmadapter,
                         IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    pmlan_private pmpriv = pmadapter->priv[pioctl_req->bss_num];

    ENTER();

    ret = wlan_prepare_cmd(pmpriv,
                           HOST_CMD_APCMD_SYS_RESET,
                           HostCmd_ACT_GEN_SET,
                           0, (t_void *) pioctl_req, MNULL);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/** 
 *  @brief Set/Get MAC address
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
mlan_status
wlan_uap_bss_ioctl_mac_address(IN pmlan_adapter pmadapter,
                               IN pmlan_ioctl_req pioctl_req)
{
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_num];
    mlan_ds_bss *bss = MNULL;
    mlan_status ret = MLAN_STATUS_SUCCESS;
    t_u16 cmd_action = 0;

    ENTER();

    bss = (mlan_ds_bss *) pioctl_req->pbuf;
    if (pioctl_req->action == MLAN_ACT_SET) {
        memcpy(pmadapter, pmpriv->curr_addr, &bss->param.mac_addr,
               MLAN_MAC_ADDR_LENGTH);
        cmd_action = HostCmd_ACT_GEN_SET;
    } else
        cmd_action = HostCmd_ACT_GEN_GET;
    /* Send request to firmware */
    ret = wlan_prepare_cmd(pmpriv, HOST_CMD_APCMD_SYS_CONFIGURE,
                           cmd_action, 0, (t_void *) pioctl_req, MNULL);

    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/** 
 *  @brief Get Uap statistics
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
static mlan_status
wlan_uap_get_stats(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    pmlan_private pmpriv = pmadapter->priv[pioctl_req->bss_num];

    ENTER();

    ret = wlan_prepare_cmd(pmpriv,
                           HostCmd_CMD_802_11_SNMP_MIB,
                           HostCmd_ACT_GEN_GET,
                           0, (t_void *) pioctl_req, MNULL);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/** 
 *  @brief Set/Get AP config
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
mlan_status
wlan_uap_bss_ioctl_config(IN pmlan_adapter pmadapter,
                          IN pmlan_ioctl_req pioctl_req)
{
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_num];
    mlan_status ret = MLAN_STATUS_SUCCESS;
    t_u16 cmd_action = 0;

    ENTER();

    if (pioctl_req->action == MLAN_ACT_SET)
        cmd_action = HostCmd_ACT_GEN_SET;
    else
        cmd_action = HostCmd_ACT_GEN_GET;

    /* Send request to firmware */
    ret = wlan_prepare_cmd(pmpriv, HOST_CMD_APCMD_SYS_CONFIGURE,
                           cmd_action, 0, (t_void *) pioctl_req, MNULL);

    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/** 
 *  @brief deauth sta
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
static mlan_status
wlan_uap_bss_ioctl_deauth_sta(IN pmlan_adapter pmadapter,
                              IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    pmlan_private pmpriv = pmadapter->priv[pioctl_req->bss_num];
    mlan_ds_bss *bss = MNULL;

    ENTER();

    bss = (mlan_ds_bss *) pioctl_req->pbuf;
    ret = wlan_prepare_cmd(pmpriv,
                           HOST_CMD_APCMD_STA_DEAUTH,
                           HostCmd_ACT_GEN_SET,
                           0,
                           (t_void *) pioctl_req,
                           (t_void *) & bss->param.deauth_param);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/** 
 *  @brief Get extended version information
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
static mlan_status
wlan_uap_get_info_ver_ext(IN pmlan_adapter pmadapter,
                          IN pmlan_ioctl_req pioctl_req)
{
    pmlan_private pmpriv = pmadapter->priv[pioctl_req->bss_num];
    mlan_status ret = MLAN_STATUS_SUCCESS;

    ENTER();

    /* Send request to firmware */
    ret = wlan_prepare_cmd(pmpriv,
                           HOST_CMD_APCMD_SYS_INFO,
                           HostCmd_ACT_GEN_GET,
                           0, (t_void *) pioctl_req, MNULL);

    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/** 
 *  @brief Get station list 
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
static mlan_status
wlan_uap_get_sta_list(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    pmlan_private pmpriv = pmadapter->priv[pioctl_req->bss_num];
    mlan_status ret = MLAN_STATUS_SUCCESS;

    ENTER();

    /* Send request to firmware */
    ret = wlan_prepare_cmd(pmpriv,
                           HOST_CMD_APCMD_STA_LIST,
                           HostCmd_ACT_GEN_GET,
                           0, (t_void *) pioctl_req, MNULL);

    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/** 
 *  @brief soft_reset
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
mlan_status
wlan_uap_misc_ioctl_soft_reset(IN pmlan_adapter pmadapter,
                               IN pmlan_ioctl_req pioctl_req)
{
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_num];
    mlan_status ret = MLAN_STATUS_SUCCESS;

    ENTER();

    ret = wlan_prepare_cmd(pmpriv,
                           HostCmd_CMD_SOFT_RESET,
                           HostCmd_ACT_GEN_SET,
                           0, (t_void *) pioctl_req, MNULL);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/** 
 *  @brief Set/Get Power mode
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
mlan_status
wlan_uap_pm_ioctl_mode(IN pmlan_adapter pmadapter,
                       IN pmlan_ioctl_req pioctl_req)
{
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_num];
    mlan_ds_pm_cfg *pm = MNULL;
    mlan_status ret = MLAN_STATUS_SUCCESS;
    t_u16 cmd_action = 0;
    t_u32 cmd_oid = 0;

    ENTER();

    pm = (mlan_ds_pm_cfg *) pioctl_req->pbuf;
    if (pioctl_req->action == MLAN_ACT_SET) {
        if (pm->param.ps_mgmt.ps_mode == PS_MODE_INACTIVITY) {
            cmd_action = EN_AUTO_PS;
            cmd_oid = BITMAP_UAP_INACT_PS;
        } else if (pm->param.ps_mgmt.ps_mode == PS_MODE_PERIODIC_DTIM) {
            cmd_action = EN_AUTO_PS;
            cmd_oid = BITMAP_UAP_DTIM_PS;
        } else {
            cmd_action = DIS_AUTO_PS;
            cmd_oid = BITMAP_UAP_INACT_PS | BITMAP_UAP_DTIM_PS;
        }
    } else {
        cmd_action = GET_PS;
        cmd_oid = BITMAP_UAP_INACT_PS | BITMAP_UAP_DTIM_PS;
    }
    /* Send request to firmware */
    ret = wlan_prepare_cmd(pmpriv,
                           HostCmd_CMD_802_11_PS_MODE_ENH,
                           cmd_action, cmd_oid, (t_void *) pioctl_req,
                           (t_void *) & pm->param.ps_mgmt);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/** 
 *  @brief Set WAPI IE   
 *
 *  @param priv         A pointer to mlan_private structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return             MLAN_STATUS_SUCCESS --success, otherwise fail
 */
mlan_status
wlan_uap_set_wapi_ie(mlan_private * priv, pmlan_ioctl_req pioctl_req)
{
    mlan_ds_misc_cfg *misc = MNULL;
    mlan_status ret = MLAN_STATUS_SUCCESS;

    ENTER();
    misc = (mlan_ds_misc_cfg *) pioctl_req->pbuf;
    if (misc->param.gen_ie.len) {
        if (misc->param.gen_ie.len > sizeof(priv->wapi_ie)) {
            PRINTM(MWARN, "failed to copy WAPI IE, too big \n");
            LEAVE();
            return MLAN_STATUS_FAILURE;
        }
        memcpy(priv->adapter, priv->wapi_ie, misc->param.gen_ie.ie_data,
               misc->param.gen_ie.len);
        priv->wapi_ie_len = misc->param.gen_ie.len;
        PRINTM(MIOCTL, "Set wapi_ie_len=%d IE=%#x\n", priv->wapi_ie_len,
               priv->wapi_ie[0]);
        DBG_HEXDUMP(MCMD_D, "wapi_ie", priv->wapi_ie, priv->wapi_ie_len);
        if (priv->wapi_ie[0] == WAPI_IE)
            priv->sec_info.wapi_enabled = MTRUE;
    } else {
        memset(priv->adapter, priv->wapi_ie, 0, sizeof(priv->wapi_ie));
        priv->wapi_ie_len = misc->param.gen_ie.len;
        PRINTM(MINFO, "Reset wapi_ie_len=%d IE=%#x\n", priv->wapi_ie_len,
               priv->wapi_ie[0]);
        priv->sec_info.wapi_enabled = MFALSE;
    }

    /* Send request to firmware */
    ret = wlan_prepare_cmd(priv, HOST_CMD_APCMD_SYS_CONFIGURE,
                           HostCmd_ACT_GEN_SET, 0, (t_void *) pioctl_req,
                           MNULL);

    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/** 
 *  @brief Set generic IE
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
static mlan_status
wlan_uap_misc_ioctl_gen_ie(IN pmlan_adapter pmadapter,
                           IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_num];
    mlan_ds_misc_cfg *misc = MNULL;
    IEEEtypes_VendorHeader_t *pvendor_ie = MNULL;

    misc = (mlan_ds_misc_cfg *) pioctl_req->pbuf;

    if ((misc->param.gen_ie.type == MLAN_IE_TYPE_GEN_IE) &&
        (pioctl_req->action == MLAN_ACT_SET)) {
        if (misc->param.gen_ie.len) {
            pvendor_ie =
                (IEEEtypes_VendorHeader_t *) misc->param.gen_ie.ie_data;
            if (pvendor_ie->element_id == WAPI_IE) {
                /* IE is a WAPI IE so call set_wapi function */
                ret = wlan_uap_set_wapi_ie(pmpriv, pioctl_req);
            }
        } else {
            /* clear WAPI IE */
            ret = wlan_uap_set_wapi_ie(pmpriv, pioctl_req);
        }
    }
    return ret;
}

/** 
 *  @brief Set/Get WAPI status
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
static mlan_status
wlan_uap_sec_ioctl_wapi_enable(IN pmlan_adapter pmadapter,
                               IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_num];
    mlan_ds_sec_cfg *sec = MNULL;
    ENTER();
    sec = (mlan_ds_sec_cfg *) pioctl_req->pbuf;
    if (pioctl_req->action == MLAN_ACT_GET) {
        if (pmpriv->wapi_ie_len)
            sec->param.wapi_enabled = MTRUE;
        else
            sec->param.wapi_enabled = MFALSE;
    } else {
        if (sec->param.wapi_enabled == MFALSE) {
            memset(pmpriv->adapter, pmpriv->wapi_ie, 0,
                   sizeof(pmpriv->wapi_ie));
            pmpriv->wapi_ie_len = 0;
            PRINTM(MINFO, "Reset wapi_ie_len=%d IE=%#x\n", pmpriv->wapi_ie_len,
                   pmpriv->wapi_ie[0]);
            pmpriv->sec_info.wapi_enabled = MFALSE;
        }
    }
    pioctl_req->data_read_written = sizeof(t_u32) + MLAN_SUB_COMMAND_SIZE;
    LEAVE();
    return ret;
}

/** 
 *  @brief Set encrypt key
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
static mlan_status
wlan_uap_sec_ioctl_set_encrypt_key(IN pmlan_adapter pmadapter,
                                   IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_num];
    mlan_ds_sec_cfg *sec = MNULL;

    ENTER();
    sec = (mlan_ds_sec_cfg *) pioctl_req->pbuf;
    if (pioctl_req->action != MLAN_ACT_SET) {
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }
    ret = wlan_prepare_cmd(pmpriv,
                           HostCmd_CMD_802_11_KEY_MATERIAL,
                           HostCmd_ACT_GEN_SET,
                           KEY_INFO_ENABLED,
                           (t_void *) pioctl_req, &sec->param.encrypt_key);

    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;
    LEAVE();
    return ret;
}

/** 
 *  @brief Get BSS information
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
static mlan_status
wlan_uap_get_bss_info(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    pmlan_private pmpriv = pmadapter->priv[pioctl_req->bss_num];
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_ds_get_info *info;

    ENTER();

    info = (mlan_ds_get_info *) pioctl_req->pbuf;
    /* Connection status */
    info->param.bss_info.media_connected = pmpriv->media_connected;
    /* BSSID */
    memcpy(pmadapter, &info->param.bss_info.bssid, pmpriv->curr_addr,
           MLAN_MAC_ADDR_LENGTH);
    info->param.bss_info.is_hs_configured = pmadapter->is_hs_configured;
    pioctl_req->data_read_written =
        sizeof(mlan_bss_info) + MLAN_SUB_COMMAND_SIZE;

    LEAVE();
    return ret;
}

/** 
 *  @brief Set Host Sleep configurations
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
static mlan_status
wlan_uap_pm_ioctl_deepsleep(IN pmlan_adapter pmadapter,
                            IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_num];
    mlan_ds_pm_cfg *pm = MNULL;
    mlan_ds_auto_ds auto_ds;
    t_u32 mode;

    ENTER();

    if (pioctl_req->buf_len < sizeof(mlan_ds_pm_cfg)) {
        PRINTM(MWARN, "MLAN bss IOCTL length is too short.\n");
        pioctl_req->data_read_written = 0;
        pioctl_req->buf_len_needed = sizeof(mlan_ds_pm_cfg);
        LEAVE();
        return MLAN_STATUS_RESOURCE;
    }
    pm = (mlan_ds_pm_cfg *) pioctl_req->pbuf;

    if (pioctl_req->action == MLAN_ACT_GET) {
        if (pmadapter->is_deep_sleep)
            pm->param.auto_deep_sleep.auto_ds = DEEP_SLEEP_ON;
        else
            pm->param.auto_deep_sleep.auto_ds = DEEP_SLEEP_OFF;
    } else {
        if (((mlan_ds_pm_cfg *) pioctl_req->pbuf)->param.auto_deep_sleep.
            auto_ds == DEEP_SLEEP_ON) {
            auto_ds.auto_ds = DEEP_SLEEP_ON;
            mode = EN_AUTO_PS;
            PRINTM(MINFO, "Auto Deep Sleep: on\n");
        } else {
            mode = DIS_AUTO_PS;
            auto_ds.auto_ds = DEEP_SLEEP_OFF;
            PRINTM(MINFO, "Auto Deep Sleep: off\n");
        }
        auto_ds.idletime =
            ((mlan_ds_pm_cfg *) pioctl_req->pbuf)->param.auto_deep_sleep.
            idletime;
        ret =
            wlan_prepare_cmd(pmpriv, HostCmd_CMD_802_11_PS_MODE_ENH, mode,
                             BITMAP_AUTO_DS, (t_void *) pioctl_req, &auto_ds);
        if (ret == MLAN_STATUS_SUCCESS)
            ret = MLAN_STATUS_PENDING;
    }
    LEAVE();
    return ret;
}

/********************************************************
    Global Functions
********************************************************/
/** 
 *  @brief MLAN uap ioctl handler
 *
 *  @param adapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
mlan_status
mlan_uap_ioctl(t_void * adapter, pmlan_ioctl_req pioctl_req)
{
    pmlan_adapter pmadapter = (pmlan_adapter) adapter;
    mlan_status status = MLAN_STATUS_SUCCESS;
    mlan_ds_bss *bss = MNULL;
    mlan_ds_get_info *pget_info = MNULL;
    mlan_ds_misc_cfg *misc = MNULL;
    mlan_ds_sec_cfg *sec = MNULL;
    mlan_ds_pm_cfg *pm = MNULL;
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_num];

    ENTER();

    switch (pioctl_req->req_id) {
    case MLAN_IOCTL_BSS:
        bss = (mlan_ds_bss *) pioctl_req->pbuf;
        if (bss->sub_command == MLAN_OID_BSS_MAC_ADDR)
            status = wlan_uap_bss_ioctl_mac_address(pmadapter, pioctl_req);
        else if (bss->sub_command == MLAN_OID_BSS_STOP)
            status = wlan_uap_bss_ioctl_stop(pmadapter, pioctl_req);
        else if (bss->sub_command == MLAN_OID_BSS_START)
            status = wlan_uap_bss_ioctl_start(pmadapter, pioctl_req);
        else if (bss->sub_command == MLAN_OID_UAP_BSS_CONFIG)
            status = wlan_uap_bss_ioctl_config(pmadapter, pioctl_req);
        else if (bss->sub_command == MLAN_OID_UAP_DEAUTH_STA)
            status = wlan_uap_bss_ioctl_deauth_sta(pmadapter, pioctl_req);
        else if (bss->sub_command == MLAN_OID_UAP_BSS_RESET)
            status = wlan_uap_bss_ioctl_reset(pmadapter, pioctl_req);
        break;
    case MLAN_IOCTL_GET_INFO:
        pget_info = (mlan_ds_get_info *) pioctl_req->pbuf;
        if (pget_info->sub_command == MLAN_OID_GET_VER_EXT)
            status = wlan_uap_get_info_ver_ext(pmadapter, pioctl_req);
        else if (pget_info->sub_command == MLAN_OID_GET_DEBUG_INFO)
            status = wlan_get_info_debug_info(pmadapter, pioctl_req);
        else if (pget_info->sub_command == MLAN_OID_GET_STATS)
            status = wlan_uap_get_stats(pmadapter, pioctl_req);
        else if (pget_info->sub_command == MLAN_OID_UAP_STA_LIST)
            status = wlan_uap_get_sta_list(pmadapter, pioctl_req);
        else if (pget_info->sub_command == MLAN_OID_GET_BSS_INFO)
            status = wlan_uap_get_bss_info(pmadapter, pioctl_req);
        else if (pget_info->sub_command == MLAN_OID_GET_FW_INFO) {
            pioctl_req->data_read_written =
                sizeof(mlan_fw_info) + MLAN_SUB_COMMAND_SIZE;
            memcpy(pmadapter, &pget_info->param.fw_info.mac_addr,
                   pmpriv->curr_addr, MLAN_MAC_ADDR_LENGTH);
            pget_info->param.fw_info.fw_ver = pmadapter->fw_release_number;
        }
        break;
    case MLAN_IOCTL_MISC_CFG:
        misc = (mlan_ds_misc_cfg *) pioctl_req->pbuf;
        if (misc->sub_command == MLAN_OID_MISC_INIT_SHUTDOWN)
            status = wlan_misc_ioctl_init_shutdown(pmadapter, pioctl_req);
        if (misc->sub_command == MLAN_OID_MISC_SOFT_RESET)
            status = wlan_uap_misc_ioctl_soft_reset(pmadapter, pioctl_req);
        if (misc->sub_command == MLAN_OID_MISC_HOST_CMD)
            status = wlan_misc_ioctl_host_cmd(pmadapter, pioctl_req);
        if (misc->sub_command == MLAN_OID_MISC_GEN_IE)
            status = wlan_uap_misc_ioctl_gen_ie(pmadapter, pioctl_req);
        if (misc->sub_command == MLAN_OID_MISC_CUSTOM_IE)
            status = wlan_misc_ioctl_custom_ie_list(pmadapter, pioctl_req);
        break;
    case MLAN_IOCTL_PM_CFG:
        pm = (mlan_ds_pm_cfg *) pioctl_req->pbuf;
        if (pm->sub_command == MLAN_OID_PM_CFG_PS_MODE)
            status = wlan_uap_pm_ioctl_mode(pmadapter, pioctl_req);
        if (pm->sub_command == MLAN_OID_PM_CFG_DEEP_SLEEP)
            status = wlan_uap_pm_ioctl_deepsleep(pmadapter, pioctl_req);
        if (pm->sub_command == MLAN_OID_PM_CFG_HS_CFG) {
            status = wlan_pm_ioctl_hscfg(pmadapter, pioctl_req);
        }
        if (pm->sub_command == MLAN_OID_PM_INFO) {
            status = wlan_get_pm_info(pmadapter, pioctl_req);
        }
        break;

    case MLAN_IOCTL_SEC_CFG:
        sec = (mlan_ds_sec_cfg *) pioctl_req->pbuf;
        if (sec->sub_command == MLAN_OID_SEC_CFG_ENCRYPT_KEY)
            status = wlan_uap_sec_ioctl_set_encrypt_key(pmadapter, pioctl_req);
        if (sec->sub_command == MLAN_OID_SEC_CFG_WAPI_ENABLED)
            status = wlan_uap_sec_ioctl_wapi_enable(pmadapter, pioctl_req);
        break;
    case MLAN_IOCTL_11N_CFG:
        status = wlan_11n_cfg_ioctl(pmadapter, pioctl_req);
        break;

    default:
        break;
    }
    LEAVE();
    return status;
}
