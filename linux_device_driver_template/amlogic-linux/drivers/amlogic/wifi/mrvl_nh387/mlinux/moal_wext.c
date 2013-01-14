/** @file  moal_wext.c
  *
  * @brief This file contains wireless extension standard ioctl functions
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

/************************************************************************
Change log:
    10/21/2008: initial version
************************************************************************/

#include        "moal_main.h"

/** Approximate amount of data needed to pass a scan result back to iwlist */
#define MAX_SCAN_CELL_SIZE  (IW_EV_ADDR_LEN             \
                             + MLAN_MAX_SSID_LENGTH   \
                             + IW_EV_UINT_LEN           \
                             + IW_EV_FREQ_LEN           \
                             + IW_EV_QUAL_LEN           \
                             + MLAN_MAX_SSID_LENGTH    \
                             + IW_EV_PARAM_LEN          \
                             + 40)      /* 40 for WPAIE */
/** Macro for minimum size of scan buffer */
#define MIN_ACCEPTED_GET_SCAN_BUF 8000
/** Macro for maximum size of scan response buffer */
#define MAX_SCAN_RSP_BUF (16 * 1024)

/** Region code mapping */
typedef struct _region_code_mapping
{
    /** Region */
    t_u8 region[COUNTRY_CODE_LEN];
    /** Code */
    t_u8 code;
} region_code_mapping_t;

/** Region code mapping table */
static region_code_mapping_t region_code_mapping[] = {
    {"US ", 0x10},              /* US FCC */
    {"CA ", 0x20},              /* IC Canada */
    {"SG ", 0x10},              /* Singapore */
    {"EU ", 0x30},              /* ETSI */
    {"AU ", 0x30},              /* Australia */
    {"KR ", 0x30},              /* Republic Of Korea */
    {"FR ", 0x32},              /* France */
    {"CN ", 0x50},              /* China */
    {"JP ", 0xFF},              /* Japan special */
};

/********************************************************
                Global Variables
********************************************************/

/********************************************************
                Local Functions
********************************************************/
/**
 *  @brief This function validates a SSID as being able to be printed
 *
 *  @param pssid   SSID structure to validate
 *
 *  @return        MTRUE or MFALSE
 */
static BOOLEAN
woal_ssid_valid(mlan_802_11_ssid * pssid)
{
#if 0
    unsigned int ssid_idx;

    ENTER();

    for (ssid_idx = 0; ssid_idx < pssid->ssid_len; ssid_idx++) {
        if ((pssid->ssid[ssid_idx] < 0x20) || (pssid->ssid[ssid_idx] > 0x7e)) {
            LEAVE();
            return MFALSE;
        }
    }
    LEAVE();
#endif
    return MTRUE;
}

/** 
 *  @brief This function converts region string to region code
 *
 *  @param region_string         Region string
 *
 *  @return     Region code
 */
static t_u8
region_string_2_region_code(char *region_string)
{
    t_u8 i;
    t_u8 size = sizeof(region_code_mapping) / sizeof(region_code_mapping_t);

    for (i = 0; i < size; i++) {
        if (!memcmp(region_string,
                    region_code_mapping[i].region, strlen(region_string))) {
            return (region_code_mapping[i].code);
        }
    }
    /* default is US */
    return (region_code_mapping[0].code);
}

/**
 * @brief Set region code
 * 
 * @param priv     A pointer to moal_private structure
 * @param region   A pointer to region string
 * 
 * @return           0 --success, otherwise fail
 */
static mlan_status
woal_set_regioncode(moal_private * priv, char *region)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_ds_misc_cfg *cfg = NULL;
    mlan_ioctl_req *req = NULL;

    ENTER();
    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_misc_cfg));
    if (req == NULL) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
    cfg = (mlan_ds_misc_cfg *) req->pbuf;
    cfg->sub_command = MLAN_OID_MISC_REGION;
    req->req_id = MLAN_IOCTL_MISC_CFG;
    req->action = MLAN_ACT_SET;
    cfg->param.region_code = region_string_2_region_code(region);
    ret = woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT);
  done:
    if (req)
        kfree(req);
    LEAVE();
    return ret;
}

/**
 *  @brief Compare two SSIDs
 *
 *  @param ssid1    A pointer to ssid to compare
 *  @param ssid2    A pointer to ssid to compare
 *
 *  @return         0--ssid is same, otherwise is different
 */
t_s32
woal_ssid_cmp(mlan_802_11_ssid * ssid1, mlan_802_11_ssid * ssid2)
{
    ENTER();

    if (!ssid1 || !ssid2) {
        LEAVE();
        return -1;
    }
    if (ssid1->ssid_len != ssid2->ssid_len) {
        LEAVE();
        return -1;
    }

    LEAVE();
    return memcmp(ssid1->ssid, ssid2->ssid, ssid1->ssid_len);
}

/**
 *  @brief Sort Channels
 *
 *  @param freq                 A pointer to iw_freq structure
 *  @param num                  Number of Channels
 *
 *  @return                     N/A
 */
static inline void
woal_sort_channels(struct iw_freq *freq, int num)
{
    int i, j;
    struct iw_freq temp;

    for (i = 0; i < num; i++)
        for (j = i + 1; j < num; j++)
            if (freq[i].i > freq[j].i) {
                temp.i = freq[i].i;
                temp.m = freq[i].m;

                freq[i].i = freq[j].i;
                freq[i].m = freq[j].m;

                freq[j].i = temp.i;
                freq[j].m = temp.m;
            }
}

/**
 *  @brief Convert RSSI to quality
 *
 *  @param rssi     RSSI in dBm
 *
 *  @return         Quality of the link (0-5)
 */
static t_u8
woal_rssi_to_quality(t_s16 rssi)
{
/** Macro for RSSI range */
#define MOAL_RSSI_NO_SIGNAL -90
#define MOAL_RSSI_VERY_LOW  -80
#define MOAL_RSSI_LOW       -70
#define MOAL_RSSI_GOOD      -60
#define MOAL_RSSI_VERY_GOOD -50
#define MOAL_RSSI_INVALID   0
    if (rssi <= MOAL_RSSI_NO_SIGNAL || rssi == MOAL_RSSI_INVALID)
        return 0;
    else if (rssi <= MOAL_RSSI_VERY_LOW)
        return 1;
    else if (rssi <= MOAL_RSSI_LOW)
        return 2;
    else if (rssi <= MOAL_RSSI_GOOD)
        return 3;
    else if (rssi <= MOAL_RSSI_VERY_GOOD)
        return 4;
    else
        return 5;
}

/**
 *  @brief Set Radio On/OFF
 *
 *  @param priv                 A pointer to moal_private structure
 *  @param option               Radio Option
 *
 *  @return                     0 --success, otherwise fail
 */
int
woal_set_radio(moal_private * priv, t_u8 option)
{
    int ret = 0;
    mlan_ds_radio_cfg *radio = NULL;
    mlan_ioctl_req *req = NULL;
    ENTER();
    if ((option != 0) && (option != 1)) {
        ret = -EINVAL;
        goto done;
    }
    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_radio_cfg));
    if (req == NULL) {
        ret = -ENOMEM;
        goto done;
    }
    radio = (mlan_ds_radio_cfg *) req->pbuf;
    radio->sub_command = MLAN_OID_RADIO_CTRL;
    req->req_id = MLAN_IOCTL_RADIO_CFG;
    req->action = MLAN_ACT_SET;
    radio->param.radio_on_off = option;
    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = -EFAULT;
        goto done;
    }
  done:
    if (req)
        kfree(req);
    LEAVE();
    return ret;
}

/**
 *  @brief Set Adapter Node Name
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param dwrq                 A pointer to iw_point structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_set_nick(struct net_device *dev, struct iw_request_info *info,
              struct iw_point *dwrq, char *extra)
{
    moal_private *priv = (moal_private *) netdev_priv(dev);
    ENTER();
    /* 
     * Check the size of the string
     */
    if (dwrq->length > 16) {
        LEAVE();
        return -E2BIG;
    }
    memset(priv->nick_name, 0, sizeof(priv->nick_name));
    memcpy(priv->nick_name, extra, dwrq->length);
    LEAVE();
    return 0;
}

/**
 *  @brief Get Adapter Node Name
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param dwrq                 A pointer to iw_point structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_get_nick(struct net_device *dev, struct iw_request_info *info,
              struct iw_point *dwrq, char *extra)
{
    moal_private *priv = (moal_private *) netdev_priv(dev);
    ENTER();
    /* 
     * Get the Nick Name saved
     */
    strncpy(extra, (char *) priv->nick_name, 16);
    extra[16] = '\0';
    /* 
     * If none, we may want to get the one that was set
     */

    /* 
     * Push it out !
     */
    dwrq->length = strlen(extra) + 1;
    LEAVE();
    return 0;
}

/**
 *  @brief Commit handler: called after a bunch of SET operations
 *
 *  @param dev          A pointer to net_device structure
 *  @param info         A pointer to iw_request_info structure
 *  @param cwrq         A pointer to char buffer
 *  @param extra        A pointer to extra data buf
 *
 *  @return             0 --success, otherwise fail
 */
static int
woal_config_commit(struct net_device *dev,
                   struct iw_request_info *info, char *cwrq, char *extra)
{
    ENTER();

    LEAVE();
    return 0;
}

/**
 *  @brief Get name
 *
 *  @param dev          A pointer to net_device structure
 *  @param info         A pointer to iw_request_info structure
 *  @param cwrq         A pointer to char buffer
 *  @param extra        A pointer to extra data buf
 *
 *  @return             0 --success, otherwise fail
 */
static int
woal_get_name(struct net_device *dev, struct iw_request_info *info,
              char *cwrq, char *extra)
{
    ENTER();
    strcpy(cwrq, "IEEE 802.11-DS");
    LEAVE();
    return 0;
}

/**
 *  @brief Set frequency
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param fwrq                 A pointer to iw_freq structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_set_freq(struct net_device *dev, struct iw_request_info *info,
              struct iw_freq *fwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_ds_bss *bss = NULL;
    mlan_ioctl_req *req = NULL;

    ENTER();
    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_bss));
    if (req == NULL) {
        ret = -ENOMEM;
        goto done;
    }
    bss = (mlan_ds_bss *) req->pbuf;
    /* 
     * If setting by frequency, convert to a channel 
     */
    if (fwrq->e == 1) {
        long f = fwrq->m / 100000;
        bss->param.bss_chan.freq = f;
    } else
        bss->param.bss_chan.channel = fwrq->m;

    bss->sub_command = MLAN_OID_BSS_CHANNEL;
    req->req_id = MLAN_IOCTL_BSS;
    req->action = MLAN_ACT_SET;
    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = -EFAULT;
        goto done;
    }
    if (MLAN_STATUS_SUCCESS !=
        woal_change_adhoc_chan(priv, bss->param.bss_chan.channel))
        ret = -EFAULT;

  done:
    if (req)
        kfree(req);
    LEAVE();
    return ret;
}

/**
 *  @brief Get frequency
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param fwrq                 A pointer to iw_freq structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_get_freq(struct net_device *dev, struct iw_request_info *info,
              struct iw_freq *fwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_ds_bss *bss = NULL;
    mlan_ioctl_req *req = NULL;

    ENTER();

    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_bss));
    if (req == NULL) {
        ret = -ENOMEM;
        goto done;
    }
    bss = (mlan_ds_bss *) req->pbuf;
    bss->sub_command = MLAN_OID_BSS_CHANNEL;
    req->req_id = MLAN_IOCTL_BSS;
    req->action = MLAN_ACT_GET;
    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = -EFAULT;
        goto done;
    }
    fwrq->m = (long) bss->param.bss_chan.freq * 100000;
    fwrq->i = (long) bss->param.bss_chan.channel;
    fwrq->e = 1;
  done:
    if (req)
        kfree(req);

    LEAVE();
    return ret;
}

/**
 *  @brief Set wlan mode
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param uwrq                 Wireless mode to set
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_set_bss_mode(struct net_device *dev, struct iw_request_info *info,
                  t_u32 * uwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_ds_bss *bss = NULL;
    mlan_ioctl_req *req = NULL;

    ENTER();

    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_bss));
    if (req == NULL) {
        ret = -ENOMEM;
        goto done;
    }
    bss = (mlan_ds_bss *) req->pbuf;
    bss->sub_command = MLAN_OID_BSS_MODE;
    req->req_id = MLAN_IOCTL_BSS;
    req->action = MLAN_ACT_SET;

    switch (*uwrq) {
    case IW_MODE_INFRA:
        bss->param.bss_mode = MLAN_BSS_MODE_INFRA;
        break;
    case IW_MODE_ADHOC:
        bss->param.bss_mode = MLAN_BSS_MODE_IBSS;
        break;
    case IW_MODE_AUTO:
        bss->param.bss_mode = MLAN_BSS_MODE_AUTO;
        break;
    default:
        ret = -EINVAL;
        break;
    }
    if (ret)
        goto done;
    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = -EFAULT;
        goto done;
    }
  done:
    if (req)
        kfree(req);
    LEAVE();
    return ret;
}

/**
 *  @brief Get current BSSID
 *
 *  @param dev          A pointer to net_device structure
 *  @param info         A pointer to iw_request_info structure
 *  @param awrq         A pointer to sockaddr structure
 *  @param extra        A pointer to extra data buf
 *
 *  @return             0 --success, otherwise fail
 */
static int
woal_get_wap(struct net_device *dev, struct iw_request_info *info,
             struct sockaddr *awrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_bss_info bss_info;

    ENTER();

    memset(&bss_info, 0, sizeof(bss_info));

    woal_get_bss_info(priv, MOAL_IOCTL_WAIT, &bss_info);

    if (bss_info.media_connected == MTRUE) {
        memcpy(awrq->sa_data, &bss_info.bssid, MLAN_MAC_ADDR_LENGTH);
    } else {
        memset(awrq->sa_data, 0, MLAN_MAC_ADDR_LENGTH);
    }
    awrq->sa_family = ARPHRD_ETHER;

    LEAVE();
    return ret;
}

/**
 *  @brief Connect to the AP or Ad-hoc Network with specific bssid
 *
 * NOTE: Scan should be issued by application before this function is called
 *
 *  @param dev          A pointer to net_device structure
 *  @param info         A pointer to iw_request_info structure
 *  @param awrq         A pointer to iw_param structure
 *  @param extra        A pointer to extra data buf
 *
 *  @return             0 --success, otherwise fail
 */
static int
woal_set_wap(struct net_device *dev, struct iw_request_info *info,
             struct sockaddr *awrq, char *extra)
{
    int ret = 0;
    const t_u8 bcast[MLAN_MAC_ADDR_LENGTH] = { 255, 255, 255, 255, 255, 255 };
    const t_u8 zero_mac[MLAN_MAC_ADDR_LENGTH] = { 0, 0, 0, 0, 0, 0 };
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_ssid_bssid ssid_bssid;
    mlan_bss_info bss_info;

    ENTER();

    if (awrq->sa_family != ARPHRD_ETHER) {
        ret = -EINVAL;
        goto done;
    }

    PRINTM(MINFO, "ASSOC: WAP: sa_data: %02x:%02x:%02x:%02x:%02x:%02x\n",
           (t_u8) awrq->sa_data[0], (t_u8) awrq->sa_data[1],
           (t_u8) awrq->sa_data[2], (t_u8) awrq->sa_data[3],
           (t_u8) awrq->sa_data[4], (t_u8) awrq->sa_data[5]);

    if (MLAN_STATUS_SUCCESS !=
        woal_get_bss_info(priv, MOAL_IOCTL_WAIT, &bss_info)) {
        ret = -EFAULT;
        goto done;
    }
#ifdef REASSOCIATION
    /* Cancel re-association */
    priv->reassoc_required = MFALSE;
#endif

    /* zero_mac means disconnect */
    if (!memcmp(zero_mac, awrq->sa_data, MLAN_MAC_ADDR_LENGTH)) {
        woal_disconnect(priv, MOAL_IOCTL_WAIT, NULL);
        goto done;
    }

    /* Broadcast MAC means search for best network */
    memset(&ssid_bssid, 0, sizeof(mlan_ssid_bssid));

    if (memcmp(bcast, awrq->sa_data, MLAN_MAC_ADDR_LENGTH)) {
        /* Check if we are already assoicated to the AP */
        if (bss_info.media_connected == MTRUE) {
            if (!memcmp(awrq->sa_data, &bss_info.bssid, ETH_ALEN))
                goto done;
            /* disconnect before try to assoicate to the new AP */
            woal_disconnect(priv, MOAL_IOCTL_WAIT, NULL);
        }
        memcpy(&ssid_bssid.bssid, awrq->sa_data, ETH_ALEN);
    }

    if (MLAN_STATUS_SUCCESS != woal_find_best_network(priv,
                                                      MOAL_IOCTL_WAIT,
                                                      &ssid_bssid)) {
        PRINTM(MERROR, "ASSOC: WAP: MAC address not found in BSSID List\n");
        ret = -ENETUNREACH;
        goto done;
    }
    /* Zero SSID implies use BSSID to connect */
    memset(&ssid_bssid.ssid, 0, sizeof(mlan_802_11_ssid));
    if (MLAN_STATUS_SUCCESS != woal_bss_start(priv,
                                              MOAL_IOCTL_WAIT, &ssid_bssid)) {
        ret = -EFAULT;
        goto done;
    }
#ifdef REASSOCIATION
    memset(&bss_info, 0, sizeof(bss_info));
    if (MLAN_STATUS_SUCCESS != woal_get_bss_info(priv,
                                                 MOAL_IOCTL_WAIT, &bss_info)) {
        ret = -EFAULT;
        goto done;
    }
    memcpy(&priv->prev_ssid_bssid.ssid, &bss_info.ssid,
           sizeof(mlan_802_11_ssid));
    memcpy(&priv->prev_ssid_bssid.bssid, &bss_info.bssid, MLAN_MAC_ADDR_LENGTH);
#endif /* REASSOCIATION */

  done:

    LEAVE();
    return ret;
}

/**
 *  @brief Get wlan mode
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param uwrq                 A pointer to t_u32 string
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_get_bss_mode(struct net_device *dev, struct iw_request_info *info,
                  t_u32 * uwrq, char *extra)
{
    moal_private *priv = (moal_private *) netdev_priv(dev);
    ENTER();
    *uwrq = woal_get_mode(priv, MOAL_IOCTL_WAIT);
    LEAVE();
    return 0;
}

/**
 *  @brief Set sensitivity
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param vwrq                 A pointer to iw_param structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_set_sens(struct net_device *dev, struct iw_request_info *info,
              struct iw_param *vwrq, char *extra)
{
    int ret = 0;

    ENTER();

    LEAVE();
    return ret;
}

/**
 *  @brief Get sensitivity
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param vwrq                 A pointer to iw_param structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_get_sens(struct net_device *dev, struct iw_request_info *info,
              struct iw_param *vwrq, char *extra)
{
    int ret = -1;

    ENTER();

    LEAVE();
    return ret;
}

/**
 *  @brief Set Tx power
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param vwrq                 A pointer to iw_param structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_set_txpow(struct net_device *dev, struct iw_request_info *info,
               struct iw_param *vwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_ds_power_cfg *pcfg = NULL;
    mlan_ioctl_req *req = NULL;

    ENTER();
    if (vwrq->disabled) {
        woal_set_radio(priv, 0);
        goto done;
    }
    woal_set_radio(priv, 1);
    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_power_cfg));
    if (req == NULL) {
        ret = -ENOMEM;
        goto done;
    }
    pcfg = (mlan_ds_power_cfg *) req->pbuf;
    pcfg->sub_command = MLAN_OID_POWER_CFG;
    req->req_id = MLAN_IOCTL_POWER_CFG;
    req->action = MLAN_ACT_SET;
    if (!vwrq->fixed)
        pcfg->param.power_cfg.is_power_auto = 1;
    else {
        pcfg->param.power_cfg.is_power_auto = 0;
        pcfg->param.power_cfg.power_level = vwrq->value;
    }
    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = -EFAULT;
        goto done;
    }
  done:
    if (req)
        kfree(req);
    LEAVE();
    return ret;
}

/**
 *  @brief Get Tx power
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param vwrq                 A pointer to iw_param structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_get_txpow(struct net_device *dev, struct iw_request_info *info,
               struct iw_param *vwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_ds_power_cfg *pcfg = NULL;
    mlan_ioctl_req *req = NULL;
    mlan_bss_info bss_info;

    ENTER();
    memset(&bss_info, 0, sizeof(bss_info));
    woal_get_bss_info(priv, MOAL_IOCTL_WAIT, &bss_info);

    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_power_cfg));
    if (req == NULL) {
        ret = -ENOMEM;
        goto done;
    }
    pcfg = (mlan_ds_power_cfg *) req->pbuf;
    pcfg->sub_command = MLAN_OID_POWER_CFG;
    req->req_id = MLAN_IOCTL_POWER_CFG;
    req->action = MLAN_ACT_GET;

    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = -EFAULT;
        goto done;
    }
    vwrq->value = pcfg->param.power_cfg.power_level;
    if (pcfg->param.power_cfg.is_power_auto)
        vwrq->fixed = 0;
    else
        vwrq->fixed = 1;
    if (bss_info.radio_on) {
        vwrq->disabled = 0;
        vwrq->flags = IW_TXPOW_DBM;
    } else {
        vwrq->disabled = 1;
    }

  done:
    if (req)
        kfree(req);

    LEAVE();
    return ret;
}

/** 
 *  @brief  Set power management 
 *   
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param vwrq                 A pointer to iw_param structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     MLAN_STATUS_SUCCESS --success, otherwise fail
 */
static int
woal_set_power(struct net_device *dev, struct iw_request_info *info,
               struct iw_param *vwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_ioctl_req *req = NULL;
    mlan_ds_pm_cfg *pm_cfg = NULL;

    ENTER();

    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_pm_cfg));
    if (req == NULL) {
        ret = -ENOMEM;
        goto done;
    }
    pm_cfg = (mlan_ds_pm_cfg *) req->pbuf;
    pm_cfg->sub_command = MLAN_OID_PM_CFG_IEEE_PS;
    req->req_id = MLAN_IOCTL_PM_CFG;
    req->action = MLAN_ACT_SET;

    PRINTM(MINFO, "PS_MODE set power disabled=%d\n", vwrq->disabled);
    if (vwrq->disabled)
        pm_cfg->param.ps_mode = 0;
    else {
        /* Check not support case only (vwrq->disabled == FALSE) */
        if ((vwrq->flags & IW_POWER_TYPE) == IW_POWER_TIMEOUT) {
            PRINTM(MERROR, "Setting power timeout command is not supported\n");
            ret = -EINVAL;
            goto done;
        } else if ((vwrq->flags & IW_POWER_TYPE) == IW_POWER_PERIOD) {
            PRINTM(MERROR, "Setting power period command is not supported\n");
            ret = -EINVAL;
            goto done;
        }
        pm_cfg->param.ps_mode = 1;
    }

    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = -EFAULT;
    }
  done:
    if (req)
        kfree(req);

    LEAVE();
    return ret;
}

/** 
 *  @brief  Get power management 
 *   
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param vwrq                 A pointer to iw_param structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     MLAN_STATUS_SUCCESS --success, otherwise fail
 */
static int
woal_get_power(struct net_device *dev, struct iw_request_info *info,
               struct iw_param *vwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_ioctl_req *req = NULL;
    mlan_ds_pm_cfg *pm_cfg = NULL;

    ENTER();

    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_pm_cfg));
    if (req == NULL) {
        ret = -ENOMEM;
        goto done;
    }
    pm_cfg = (mlan_ds_pm_cfg *) req->pbuf;
    pm_cfg->sub_command = MLAN_OID_PM_CFG_IEEE_PS;
    req->req_id = MLAN_IOCTL_PM_CFG;
    req->action = MLAN_ACT_GET;

    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = -EFAULT;
        goto done;
    }

    if (pm_cfg->param.ps_mode)
        vwrq->disabled = 0;
    else
        vwrq->disabled = 1;

    vwrq->value = 0;

  done:
    if (req)
        kfree(req);

    LEAVE();
    return ret;
}

/**
 *  @brief Set Tx retry count
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param vwrq                 A pointer to iw_param structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_set_retry(struct net_device *dev, struct iw_request_info *info,
               struct iw_param *vwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_ds_snmp_mib *mib = NULL;
    mlan_ioctl_req *req = NULL;

    ENTER();

    if (vwrq->flags == IW_RETRY_LIMIT) {
        /* 
         * The MAC has a 4-bit Total_Tx_Count register
         * Total_Tx_Count = 1 + Tx_Retry_Count
         */
/** Minimum transmission retry count */
#define TX_RETRY_MIN 0
/** Maximum transmission retry count */
#define TX_RETRY_MAX 14
        if (vwrq->value < TX_RETRY_MIN || vwrq->value > TX_RETRY_MAX) {
            ret = -EINVAL;
            goto done;
        }

        req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_snmp_mib));
        if (req == NULL) {
            ret = -ENOMEM;
            goto done;
        }

        mib = (mlan_ds_snmp_mib *) req->pbuf;
        mib->sub_command = MLAN_OID_SNMP_MIB_RETRY_COUNT;
        mib->param.retry_count = vwrq->value;
        req->req_id = MLAN_IOCTL_SNMP_MIB;
        req->action = MLAN_ACT_SET;

        if (MLAN_STATUS_SUCCESS !=
            woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
            ret = -EFAULT;
            goto done;
        }
    } else {
        ret = -EOPNOTSUPP;
        goto done;
    }

  done:
    if (req)
        kfree(req);

    LEAVE();
    return ret;
}

/**
 *  @brief Get Tx retry count
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param vwrq                 A pointer to iw_param structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_get_retry(struct net_device *dev, struct iw_request_info *info,
               struct iw_param *vwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_ds_snmp_mib *mib = NULL;
    mlan_ioctl_req *req = NULL;

    ENTER();

    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_snmp_mib));
    if (req == NULL) {
        ret = -ENOMEM;
        goto done;
    }

    mib = (mlan_ds_snmp_mib *) req->pbuf;
    mib->sub_command = MLAN_OID_SNMP_MIB_RETRY_COUNT;
    req->req_id = MLAN_IOCTL_SNMP_MIB;
    req->action = MLAN_ACT_GET;

    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = -EFAULT;
        goto done;
    }

    vwrq->disabled = 0;
    if (!vwrq->flags) {
        vwrq->flags = IW_RETRY_LIMIT;
        /* Get Tx retry count */
        vwrq->value = mib->param.retry_count;
    }
  done:
    if (req)
        kfree(req);

    LEAVE();
    return ret;
}

/**
 *  @brief Set encryption key
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param dwrq                 A pointer to iw_point structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_set_encode(struct net_device *dev, struct iw_request_info *info,
                struct iw_point *dwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_ds_sec_cfg *sec = NULL;
    mlan_ioctl_req *req = NULL;
    mlan_ds_encrypt_key *pkey = NULL;
    int index = 0;
    t_u32 auth_mode = 0;

    ENTER();

    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_sec_cfg));
    if (req == NULL) {
        ret = -ENOMEM;
        goto done;
    }
    sec = (mlan_ds_sec_cfg *) req->pbuf;
    sec->sub_command = MLAN_OID_SEC_CFG_ENCRYPT_KEY;
    req->req_id = MLAN_IOCTL_SEC_CFG;
    req->action = MLAN_ACT_SET;

    /* Check index */
    index = (dwrq->flags & IW_ENCODE_INDEX) - 1;
    if (index > 3) {
        PRINTM(MERROR, "Key index #%d out of range\n", index);
        ret = -EINVAL;
        goto done;
    }
    if (dwrq->length > MAX_WEP_KEY_SIZE) {
        pkey = (mlan_ds_encrypt_key *) extra;
        if (pkey->key_len <= MAX_WEP_KEY_SIZE) {
            dwrq->length = pkey->key_len;
            dwrq->flags = pkey->key_index + 1;
        }
    }
    sec->param.encrypt_key.key_len = 0;
    if (dwrq->length) {
        if (dwrq->length > MAX_WEP_KEY_SIZE) {
            PRINTM(MERROR, "Key length (%d) out of range\n", dwrq->length);
            ret = -EINVAL;
            goto done;
        }
        if (index < 0)
            sec->param.encrypt_key.is_current_wep_key = MTRUE;
        else
            sec->param.encrypt_key.key_index = index;
        if (!(dwrq->flags & IW_ENCODE_NOKEY)) {
            memcpy(sec->param.encrypt_key.key_material, extra, dwrq->length);
            /* Set the length */
            if (dwrq->length > MIN_WEP_KEY_SIZE)
                sec->param.encrypt_key.key_len = MAX_WEP_KEY_SIZE;
            else
                sec->param.encrypt_key.key_len = MIN_WEP_KEY_SIZE;
        }
    } else {
        /* 
         * No key provided so it is either enable key, 
         * on or off
         */
        if (dwrq->flags & IW_ENCODE_DISABLED) {
            PRINTM(MINFO, "*** iwconfig mlanX key off ***\n");
            sec->param.encrypt_key.key_disable = MTRUE;
        } else {
            /* 
             * iwconfig mlanX key [n]
             * iwconfig mlanX key on 
             * Do we want to just set the transmit key index ? 
             */
            if (index < 0) {
                PRINTM(MINFO, "*** iwconfig mlanX key on ***\n");
                sec->param.encrypt_key.is_current_wep_key = MTRUE;
            } else
                sec->param.encrypt_key.key_index = index;
        }
    }
    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = -EFAULT;
        goto done;
    }
    if (dwrq->flags & (IW_ENCODE_RESTRICTED | IW_ENCODE_OPEN)) {
        switch (dwrq->flags & 0xf000) {
        case IW_ENCODE_RESTRICTED:
            /* iwconfig mlanX restricted key [1] */
            auth_mode = MLAN_AUTH_MODE_SHARED;
            PRINTM(MINFO, "Auth mode restricted!\n");
            break;
        case IW_ENCODE_OPEN:
            /* iwconfig mlanX key [2] open */
            auth_mode = MLAN_AUTH_MODE_OPEN;
            PRINTM(MINFO, "Auth mode open!\n");
            break;
        case IW_ENCODE_RESTRICTED | IW_ENCODE_OPEN:
        default:
            /* iwconfig mlanX key [2] open restricted */
            auth_mode = MLAN_AUTH_MODE_AUTO;
            PRINTM(MINFO, "Auth mode auto!\n");
            break;
        }
        if (MLAN_STATUS_SUCCESS !=
            woal_set_auth_mode(priv, MOAL_IOCTL_WAIT, auth_mode))
            ret = -EFAULT;
    }
  done:
    if (req)
        kfree(req);

    LEAVE();
    return ret;
}

/**
 *  @brief Get encryption key
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param dwrq                 A pointer to iw_point structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_get_encode(struct net_device *dev, struct iw_request_info *info,
                struct iw_point *dwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_ds_sec_cfg *sec = NULL;
    mlan_ioctl_req *req = NULL;
    t_u32 auth_mode;
    int index = (dwrq->flags & IW_ENCODE_INDEX);

    ENTER();
    if (index < 0 || index > 4) {
        PRINTM(MERROR, "Key index #%d out of range\n", index);
        ret = -EINVAL;
        goto done;
    }
    if (MLAN_STATUS_SUCCESS !=
        woal_get_auth_mode(priv, MOAL_IOCTL_WAIT, &auth_mode)) {
        ret = -EFAULT;
        goto done;
    }
    dwrq->flags = 0;
    /* 
     * Check encryption mode 
     */
    switch (auth_mode) {
    case MLAN_AUTH_MODE_OPEN:
        dwrq->flags = IW_ENCODE_OPEN;
        break;

    case MLAN_AUTH_MODE_SHARED:
    case MLAN_AUTH_MODE_NETWORKEAP:
        dwrq->flags = IW_ENCODE_RESTRICTED;
        break;

    case MLAN_AUTH_MODE_AUTO:
        dwrq->flags = IW_ENCODE_OPEN | IW_ENCODE_RESTRICTED;
        break;

    default:
        dwrq->flags = IW_ENCODE_DISABLED | IW_ENCODE_OPEN;
        break;
    }
    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_sec_cfg));
    if (req == NULL) {
        ret = -ENOMEM;
        goto done;
    }

    sec = (mlan_ds_sec_cfg *) req->pbuf;
    sec->sub_command = MLAN_OID_SEC_CFG_ENCRYPT_KEY;
    req->req_id = MLAN_IOCTL_SEC_CFG;
    req->action = MLAN_ACT_GET;

    if (!index)
        sec->param.encrypt_key.is_current_wep_key = MTRUE;
    else
        sec->param.encrypt_key.key_index = index - 1;

    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = -EFAULT;
        goto done;
    }
    memset(extra, 0, 16);
    if (sec->param.encrypt_key.key_len) {
        memcpy(extra, sec->param.encrypt_key.key_material,
               sec->param.encrypt_key.key_len);
        dwrq->length = sec->param.encrypt_key.key_len;
        dwrq->flags |= (sec->param.encrypt_key.key_index + 1);
        dwrq->flags &= ~IW_ENCODE_DISABLED;
    } else if (sec->param.encrypt_key.key_disable)
        dwrq->flags |= IW_ENCODE_DISABLED;
    else
        dwrq->flags &= ~IW_ENCODE_DISABLED;

    dwrq->flags |= IW_ENCODE_NOKEY;
  done:
    if (req)
        kfree(req);

    LEAVE();
    return ret;
}

/**
 *  @brief Set data rate
 *
 *  @param dev          A pointer to net_device structure
 *  @param info         A pointer to iw_request_info structure
 *  @param vwrq         A pointer to iw_param structure
 *  @param extra        A pointer to extra data buf
 *
 *  @return             0 --success, otherwise fail
 */
static int
woal_set_rate(struct net_device *dev, struct iw_request_info *info,
              struct iw_param *vwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_ds_rate *rate = NULL;
    mlan_ioctl_req *req = NULL;

    ENTER();

    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_rate));
    if (req == NULL) {
        ret = -ENOMEM;
        goto done;
    }

    rate = (mlan_ds_rate *) req->pbuf;
    rate->sub_command = MLAN_OID_RATE_CFG;
    req->req_id = MLAN_IOCTL_RATE;
    req->action = MLAN_ACT_SET;
    if (vwrq->value == -1) {
        rate->param.rate_cfg.is_rate_auto = 1;
    } else {
        rate->param.rate_cfg.is_rate_auto = 0;
        rate->param.rate_cfg.rate_type = MLAN_RATE_VALUE;
        rate->param.rate_cfg.rate = vwrq->value / 500000;
    }
    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = -EFAULT;
        goto done;
    }
  done:
    if (req)
        kfree(req);
    LEAVE();
    return ret;
}

/**
 *  @brief Get data rate
 *
 *  @param dev          A pointer to net_device structure
 *  @param info         A pointer to iw_request_info structure
 *  @param vwrq         A pointer to iw_param structure
 *  @param extra        A pointer to extra data buf
 *
 *  @return             0 --success, otherwise fail
 */
static int
woal_get_rate(struct net_device *dev, struct iw_request_info *info,
              struct iw_param *vwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_ds_rate *rate = NULL;
    mlan_ioctl_req *req = NULL;

    ENTER();

    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_rate));
    if (req == NULL) {
        ret = -ENOMEM;
        goto done;
    }

    rate = (mlan_ds_rate *) req->pbuf;
    rate->param.rate_cfg.rate_type = MLAN_RATE_VALUE;
    rate->sub_command = MLAN_OID_RATE_CFG;
    req->req_id = MLAN_IOCTL_RATE;
    req->action = MLAN_ACT_GET;

    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = -EFAULT;
        goto done;
    }

    if (rate->param.rate_cfg.is_rate_auto)
        vwrq->fixed = 0;
    else
        vwrq->fixed = 1;
    vwrq->value = rate->param.rate_cfg.rate * 500000;
  done:
    if (req)
        kfree(req);
    LEAVE();
    return ret;
}

/**
 *  @brief Set RTS threshold
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param vwrq                 A pointer to iw_param structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_set_rts(struct net_device *dev, struct iw_request_info *info,
             struct iw_param *vwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_ds_snmp_mib *mib = NULL;
    mlan_ioctl_req *req = NULL;
    int rthr = vwrq->value;

    ENTER();

    if (vwrq->disabled) {
        rthr = MLAN_RTS_MAX_VALUE;
    } else {
        if (rthr < MLAN_RTS_MIN_VALUE || rthr > MLAN_RTS_MAX_VALUE) {
            ret = -EINVAL;
            goto done;
        }
    }

    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_snmp_mib));
    if (req == NULL) {
        ret = -ENOMEM;
        goto done;
    }

    mib = (mlan_ds_snmp_mib *) req->pbuf;
    mib->sub_command = MLAN_OID_SNMP_MIB_RTS_THRESHOLD;
    mib->param.rts_threshold = rthr;
    req->req_id = MLAN_IOCTL_SNMP_MIB;
    req->action = MLAN_ACT_SET;

    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = -EFAULT;
        goto done;
    }

  done:
    if (req)
        kfree(req);

    LEAVE();
    return ret;
}

/**
 *  @brief Get RTS threshold
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param vwrq                 A pointer to iw_param structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_get_rts(struct net_device *dev, struct iw_request_info *info,
             struct iw_param *vwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_ds_snmp_mib *mib = NULL;
    mlan_ioctl_req *req = NULL;

    ENTER();

    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_snmp_mib));
    if (req == NULL) {
        ret = -ENOMEM;
        goto done;
    }

    mib = (mlan_ds_snmp_mib *) req->pbuf;
    mib->sub_command = MLAN_OID_SNMP_MIB_RTS_THRESHOLD;
    req->req_id = MLAN_IOCTL_SNMP_MIB;
    req->action = MLAN_ACT_GET;

    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = -EFAULT;
        goto done;
    }

    vwrq->value = mib->param.rts_threshold;
    vwrq->disabled = ((vwrq->value < MLAN_RTS_MIN_VALUE)
                      || (vwrq->value > MLAN_RTS_MAX_VALUE));
    vwrq->fixed = 1;
  done:
    if (req)
        kfree(req);

    LEAVE();
    return ret;
}

/**
 *  @brief Set Fragment threshold
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param vwrq                 A pointer to iw_param structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_set_frag(struct net_device *dev, struct iw_request_info *info,
              struct iw_param *vwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_ds_snmp_mib *mib = NULL;
    mlan_ioctl_req *req = NULL;
    int fthr = vwrq->value;

    ENTER();

    if (vwrq->disabled) {
        fthr = MLAN_FRAG_MAX_VALUE;
    } else {
        if (fthr < MLAN_FRAG_MIN_VALUE || fthr > MLAN_FRAG_MAX_VALUE) {
            ret = -EINVAL;
            goto done;
        }
    }

    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_snmp_mib));
    if (req == NULL) {
        ret = -ENOMEM;
        goto done;
    }

    mib = (mlan_ds_snmp_mib *) req->pbuf;
    mib->sub_command = MLAN_OID_SNMP_MIB_FRAG_THRESHOLD;
    mib->param.frag_threshold = fthr;
    req->req_id = MLAN_IOCTL_SNMP_MIB;
    req->action = MLAN_ACT_SET;

    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = -EFAULT;
        goto done;
    }

  done:
    if (req)
        kfree(req);

    LEAVE();
    return ret;
}

/**
 *  @brief Get Fragment threshold
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param vwrq                 A pointer to iw_param structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_get_frag(struct net_device *dev, struct iw_request_info *info,
              struct iw_param *vwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_ds_snmp_mib *mib = NULL;
    mlan_ioctl_req *req = NULL;

    ENTER();

    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_snmp_mib));
    if (req == NULL) {
        ret = -ENOMEM;
        goto done;
    }
    mib = (mlan_ds_snmp_mib *) req->pbuf;
    mib->sub_command = MLAN_OID_SNMP_MIB_FRAG_THRESHOLD;
    req->req_id = MLAN_IOCTL_SNMP_MIB;
    req->action = MLAN_ACT_GET;

    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = -EFAULT;
        goto done;
    }

    vwrq->value = mib->param.frag_threshold;
    vwrq->disabled = ((vwrq->value < MLAN_FRAG_MIN_VALUE)
                      || (vwrq->value > MLAN_FRAG_MAX_VALUE));
    vwrq->fixed = 1;
  done:
    if (req)
        kfree(req);

    LEAVE();
    return ret;
}

/**
 *  @brief Get IE
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param dwrq                 A pointer to iw_point structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_get_gen_ie(struct net_device *dev, struct iw_request_info *info,
                struct iw_point *dwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_ds_misc_cfg *misc = NULL;
    mlan_ioctl_req *req = NULL;
    int copy_size = 0;

    ENTER();

    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_misc_cfg));
    if (req == NULL) {
        ret = -ENOMEM;
        goto done;
    }

    misc = (mlan_ds_misc_cfg *) req->pbuf;
    misc->sub_command = MLAN_OID_MISC_GEN_IE;
    req->req_id = MLAN_IOCTL_MISC_CFG;
    req->action = MLAN_ACT_GET;
    misc->param.gen_ie.type = MLAN_IE_TYPE_GEN_IE;

    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = -EFAULT;
        goto done;
    }
    copy_size = MIN(misc->param.gen_ie.len, dwrq->length);
    memcpy(extra, misc->param.gen_ie.ie_data, copy_size);
    dwrq->length = copy_size;
  done:
    if (req)
        kfree(req);

    LEAVE();
    return ret;
}

/**
 *  @brief Set IE
 *
 *  Pass an opaque block of data, expected to be IEEE IEs, to the driver 
 *    for eventual passthrough to the firmware in an associate/join 
 *    (and potentially start) command.
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param dwrq                 A pointer to iw_point structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_set_gen_ie(struct net_device *dev, struct iw_request_info *info,
                struct iw_point *dwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_ds_misc_cfg *misc = NULL;
    mlan_ioctl_req *req = NULL;

    ENTER();

    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_misc_cfg));
    if (req == NULL) {
        ret = -ENOMEM;
        goto done;
    }

    misc = (mlan_ds_misc_cfg *) req->pbuf;
    misc->sub_command = MLAN_OID_MISC_GEN_IE;
    req->req_id = MLAN_IOCTL_MISC_CFG;
    req->action = MLAN_ACT_SET;

    if (dwrq->length > MAX_IE_SIZE) {
        ret = -EFAULT;
        goto done;
    }
    misc->param.gen_ie.type = MLAN_IE_TYPE_GEN_IE;
    misc->param.gen_ie.len = dwrq->length;
    memcpy(misc->param.gen_ie.ie_data, extra, misc->param.gen_ie.len);
    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = -EFAULT;
        goto done;
    }
  done:
    if (req)
        kfree(req);

    LEAVE();
    return ret;
}

/** 
 *  @brief  Extended version of encoding configuration 
 *
 *  @param dev          A pointer to net_device structure
 *  @param info         A pointer to iw_request_info structure
 *  @param dwrq         A pointer to iw_point structure
 *  @param extra        A pointer to extra data buf
 *
 *  @return              0 --success, otherwise fail
 */
static int
woal_set_encode_ext(struct net_device *dev,
                    struct iw_request_info *info,
                    struct iw_point *dwrq, char *extra)
{
    struct iw_encode_ext *ext = (struct iw_encode_ext *) extra;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    int key_index;
    t_u8 *pkey_material = NULL;
    mlan_ioctl_req *req = NULL;
    mlan_ds_sec_cfg *sec = NULL;
    int ret = 0;

    ENTER();
    key_index = (dwrq->flags & IW_ENCODE_INDEX) - 1;
    if (key_index < 0 || key_index > 3) {
        ret = -EINVAL;
        goto done;
    }
    if (ext->key_len > (dwrq->length - sizeof(struct iw_encode_ext))) {
        ret = -EINVAL;
        goto done;
    }
    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_sec_cfg));
    if (req == NULL) {
        ret = -ENOMEM;
        goto done;
    }
    sec = (mlan_ds_sec_cfg *) req->pbuf;
    sec->sub_command = MLAN_OID_SEC_CFG_ENCRYPT_KEY;
    req->req_id = MLAN_IOCTL_SEC_CFG;
    req->action = MLAN_ACT_SET;
    pkey_material = (t_u8 *) (ext + 1);
    sec->param.encrypt_key.key_len = ext->key_len;
    /* Disable and Remove Key */
    if ((dwrq->flags & IW_ENCODE_DISABLED) && !ext->key_len) {
        sec->param.encrypt_key.key_remove = MTRUE;
        sec->param.encrypt_key.key_index = key_index;
    } else if (ext->key_len <= MAX_WEP_KEY_SIZE) {
        /* Set WEP key */
        sec->param.encrypt_key.key_index = key_index;
        memcpy(sec->param.encrypt_key.key_material, pkey_material,
               ext->key_len);
        if (MLAN_STATUS_SUCCESS !=
            woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
            ret = -EFAULT;
            goto done;
        }
        if (ext->ext_flags & IW_ENCODE_EXT_SET_TX_KEY) {
            sec->param.encrypt_key.key_len = 0;
        }
    } else {
        /* Set WPA key */
        sec->param.encrypt_key.key_index = key_index;
        memcpy(sec->param.encrypt_key.key_material, pkey_material,
               ext->key_len);
#define IW_ENCODE_ALG_SMS4   0x20
        /* Set WAPI key */
        if (ext->alg == IW_ENCODE_ALG_SMS4) {
            sec->param.encrypt_key.is_wapi_key = MTRUE;
            memcpy(sec->param.encrypt_key.mac_addr, (u8 *) ext->addr.sa_data,
                   ETH_ALEN);
            memcpy(sec->param.encrypt_key.pn, ext->tx_seq, PN_SIZE);
        } else if ((ext->ext_flags & IW_ENCODE_EXT_GROUP_KEY) && !key_index) {
            key_index = 1;
            sec->param.encrypt_key.key_index = key_index;
            PRINTM(MWARN, "Key index changed for GTK: %ld\n",
                   sec->param.encrypt_key.key_index);
        }
    }
    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT))
        ret = -EFAULT;
  done:
    if (req)
        kfree(req);
    LEAVE();
    return ret;
}

/** 
 *  @brief  Extended version of encoding configuration 
 *
 *  @param dev          A pointer to net_device structure
 *  @param info         A pointer to iw_request_info structure
 *  @param dwrq         A pointer to iw_point structure
 *  @param extra        A pointer to extra data buf
 *
 *  @return             -EOPNOTSUPP
 */
static int
woal_get_encode_ext(struct net_device *dev,
                    struct iw_request_info *info,
                    struct iw_point *dwrq, char *extra)
{
    ENTER();
    LEAVE();
    return -EOPNOTSUPP;
}

/** 
 *  @brief  Request MLME operation 
 *
 *  @param dev          A pointer to net_device structure
 *  @param info         A pointer to iw_request_info structure
 *  @param dwrq         A pointer to iw_point structure
 *  @param extra        A pointer to extra data buf
 *
 *  @return             0--success, otherwise fail
 */
static int
woal_set_mlme(struct net_device *dev,
              struct iw_request_info *info, struct iw_point *dwrq, char *extra)
{
    struct iw_mlme *mlme = (struct iw_mlme *) extra;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    int ret = 0;

    ENTER();
    if ((mlme->cmd == IW_MLME_DEAUTH) || (mlme->cmd == IW_MLME_DISASSOC)) {
        if (MLAN_STATUS_SUCCESS !=
            woal_disconnect(priv, MOAL_IOCTL_WAIT, (t_u8 *) mlme->addr.sa_data))
            ret = -EFAULT;
    }
    LEAVE();
    return ret;
}

/** @brief Set authentication mode parameters
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param vwrq                 A pointer to iw_param structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_set_auth(struct net_device *dev, struct iw_request_info *info,
              struct iw_param *vwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    t_u32 auth_mode = 0;
    t_u32 encrypt_mode = 0;
    ENTER();

    switch (vwrq->flags & IW_AUTH_INDEX) {
    case IW_AUTH_CIPHER_PAIRWISE:
    case IW_AUTH_CIPHER_GROUP:
        if (vwrq->value & IW_AUTH_CIPHER_NONE)
            encrypt_mode = MLAN_ENCRYPTION_MODE_NONE;
        else if (vwrq->value & IW_AUTH_CIPHER_WEP40)
            encrypt_mode = MLAN_ENCRYPTION_MODE_WEP40;
        else if (vwrq->value & IW_AUTH_CIPHER_WEP104)
            encrypt_mode = MLAN_ENCRYPTION_MODE_WEP104;
        else if (vwrq->value & IW_AUTH_CIPHER_TKIP)
            encrypt_mode = MLAN_ENCRYPTION_MODE_TKIP;
        else if (vwrq->value & IW_AUTH_CIPHER_CCMP)
            encrypt_mode = MLAN_ENCRYPTION_MODE_CCMP;
        if (MLAN_STATUS_SUCCESS !=
            woal_set_encrypt_mode(priv, MOAL_IOCTL_WAIT, encrypt_mode))
            ret = -EFAULT;
        break;
    case IW_AUTH_80211_AUTH_ALG:
        switch (vwrq->value) {
        case IW_AUTH_ALG_SHARED_KEY:
            PRINTM(MINFO, "Auth mode shared key!\n");
            auth_mode = MLAN_AUTH_MODE_SHARED;
            break;
        case IW_AUTH_ALG_LEAP:
            PRINTM(MINFO, "Auth mode LEAP!\n");
            auth_mode = MLAN_AUTH_MODE_NETWORKEAP;
            break;
        case IW_AUTH_ALG_OPEN_SYSTEM:
            PRINTM(MINFO, "Auth mode open!\n");
            auth_mode = MLAN_AUTH_MODE_OPEN;
            break;
        case IW_AUTH_ALG_SHARED_KEY | IW_AUTH_ALG_OPEN_SYSTEM:
        default:
            PRINTM(MINFO, "Auth mode auto!\n");
            auth_mode = MLAN_AUTH_MODE_AUTO;
            break;
        }
        if (MLAN_STATUS_SUCCESS !=
            woal_set_auth_mode(priv, MOAL_IOCTL_WAIT, auth_mode))
            ret = -EFAULT;
        break;
    case IW_AUTH_WPA_ENABLED:
        if (MLAN_STATUS_SUCCESS !=
            woal_set_wpa_enable(priv, MOAL_IOCTL_WAIT, vwrq->value))
            ret = -EFAULT;
        break;
#define IW_AUTH_WAPI_ENABLED    0x20
    case IW_AUTH_WAPI_ENABLED:
        if (MLAN_STATUS_SUCCESS !=
            woal_set_wapi_enable(priv, MOAL_IOCTL_WAIT, vwrq->value))
            ret = -EFAULT;
        break;
    case IW_AUTH_WPA_VERSION:
        /* set WPA_VERSION_DISABLED/VERSION_WPA/VERSION_WP2 */
        priv->wpa_version = vwrq->value;
        break;
    case IW_AUTH_KEY_MGMT:
        /* set KEY_MGMT_802_1X/KEY_MGMT_PSK */
        priv->key_mgmt = vwrq->value;
        break;
    case IW_AUTH_TKIP_COUNTERMEASURES:
    case IW_AUTH_DROP_UNENCRYPTED:
    case IW_AUTH_RX_UNENCRYPTED_EAPOL:
    case IW_AUTH_ROAMING_CONTROL:
    case IW_AUTH_PRIVACY_INVOKED:
        break;
    default:
        ret = -EOPNOTSUPP;
        break;
    }
    LEAVE();
    return ret;
}

/**
 *  @brief Get authentication mode parameters
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param vwrq                 A pointer to iw_param structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_get_auth(struct net_device *dev, struct iw_request_info *info,
              struct iw_param *vwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    t_u32 encrypt_mode = 0;
    t_u32 auth_mode;
    t_u32 wpa_enable;
    ENTER();
    switch (vwrq->flags & IW_AUTH_INDEX) {
    case IW_AUTH_CIPHER_PAIRWISE:
    case IW_AUTH_CIPHER_GROUP:
        if (MLAN_STATUS_SUCCESS !=
            woal_get_encrypt_mode(priv, MOAL_IOCTL_WAIT, &encrypt_mode))
            ret = -EFAULT;
        else {
            if (encrypt_mode == MLAN_ENCRYPTION_MODE_NONE)
                vwrq->value = IW_AUTH_CIPHER_NONE;
            else if (encrypt_mode == MLAN_ENCRYPTION_MODE_WEP40)
                vwrq->value = IW_AUTH_CIPHER_WEP40;
            else if (encrypt_mode == MLAN_ENCRYPTION_MODE_TKIP)
                vwrq->value = IW_AUTH_CIPHER_TKIP;
            else if (encrypt_mode == MLAN_ENCRYPTION_MODE_CCMP)
                vwrq->value = IW_AUTH_CIPHER_CCMP;
            else if (encrypt_mode == MLAN_ENCRYPTION_MODE_WEP104)
                vwrq->value = IW_AUTH_CIPHER_WEP104;
        }
        break;
    case IW_AUTH_80211_AUTH_ALG:
        if (MLAN_STATUS_SUCCESS !=
            woal_get_auth_mode(priv, MOAL_IOCTL_WAIT, &auth_mode))
            ret = -EFAULT;
        else {
            if (auth_mode == MLAN_AUTH_MODE_SHARED)
                vwrq->value = IW_AUTH_ALG_SHARED_KEY;
            else if (auth_mode == MLAN_AUTH_MODE_NETWORKEAP)
                vwrq->value = IW_AUTH_ALG_LEAP;
            else
                vwrq->value = IW_AUTH_ALG_OPEN_SYSTEM;
        }
        break;
    case IW_AUTH_WPA_ENABLED:
        if (MLAN_STATUS_SUCCESS !=
            woal_get_wpa_enable(priv, MOAL_IOCTL_WAIT, &wpa_enable))
            ret = -EFAULT;
        else
            vwrq->value = wpa_enable;
        break;
    case IW_AUTH_WPA_VERSION:
        vwrq->value = priv->wpa_version;
        break;
    case IW_AUTH_KEY_MGMT:
        vwrq->value = priv->key_mgmt;
        break;
    case IW_AUTH_TKIP_COUNTERMEASURES:
    case IW_AUTH_DROP_UNENCRYPTED:
    case IW_AUTH_RX_UNENCRYPTED_EAPOL:
    case IW_AUTH_ROAMING_CONTROL:
    case IW_AUTH_PRIVACY_INVOKED:
    default:
        ret = -EOPNOTSUPP;
        goto done;
    }

  done:
    LEAVE();
    return ret;
}

/* Data rate listing
 *      MULTI_BANDS:
 *              abg             a       b       b/g
 *  Infra       G(12)           A(8)    B(4)    G(12)
 *  Adhoc       A+B(12)         A(8)    B(4)    B(4)
 *      non-MULTI_BANDS:
                                        b       b/g
 *  Infra                               B(4)    G(12)
 *  Adhoc                               B(4)    B(4)
 */
/**
 *  @brief Get Range Info
 *
 *  @param dev                  A pointer to net_device structure
 *  @param info                 A pointer to iw_request_info structure
 *  @param dwrq                 A pointer to iw_point structure
 *  @param extra                A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_get_range(struct net_device *dev, struct iw_request_info *info,
               struct iw_point *dwrq, char *extra)
{
    int i;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    struct iw_range *range = (struct iw_range *) extra;
    moal_802_11_rates rates;
    mlan_chan_list *pchan_list = NULL;
    mlan_bss_info bss_info;

    ENTER();
    if (!(pchan_list = kmalloc(sizeof(mlan_chan_list), GFP_KERNEL))) {
        LEAVE();
        return -ENOMEM;
    }

    dwrq->length = sizeof(struct iw_range);
    memset(range, 0, sizeof(struct iw_range));

    range->min_nwid = 0;
    range->max_nwid = 0;

    memset(&rates, 0, sizeof(rates));
    woal_get_data_rates(priv, MOAL_IOCTL_WAIT, &rates);
    range->num_bitrates = rates.num_of_rates;

    for (i = 0; i < MIN(range->num_bitrates, IW_MAX_BITRATES) && rates.rates[i];
         i++) {
        range->bitrate[i] = (rates.rates[i] & 0x7f) * 500000;
    }
    range->num_bitrates = i;
    PRINTM(MINFO, "IW_MAX_BITRATES=%d num_bitrates=%d\n", IW_MAX_BITRATES,
           range->num_bitrates);

    range->num_frequency = 0;

    memset(pchan_list, 0, sizeof(mlan_chan_list));

    woal_get_channel_list(priv, MOAL_IOCTL_WAIT, pchan_list);

    range->num_frequency = MIN(pchan_list->num_of_chan, IW_MAX_FREQUENCIES);

    for (i = 0; i < range->num_frequency; i++) {
        range->freq[i].i = (long) pchan_list->cf[i].channel;
        range->freq[i].m = (long) pchan_list->cf[i].freq * 100000;
        range->freq[i].e = 1;
    }
    kfree(pchan_list);

    PRINTM(MINFO, "IW_MAX_FREQUENCIES=%d num_frequency=%d\n",
           IW_MAX_FREQUENCIES, range->num_frequency);

    range->num_channels = range->num_frequency;

    woal_sort_channels(&range->freq[0], range->num_frequency);

    /* 
     * Set an indication of the max TCP throughput in bit/s that we can
     * expect using this interface
     */
    if (i > 2)
        range->throughput = 5000 * 1000;
    else
        range->throughput = 1500 * 1000;

    range->min_rts = MLAN_RTS_MIN_VALUE;
    range->max_rts = MLAN_RTS_MAX_VALUE;
    range->min_frag = MLAN_FRAG_MIN_VALUE;
    range->max_frag = MLAN_FRAG_MAX_VALUE;

    range->encoding_size[0] = 5;
    range->encoding_size[1] = 13;
    range->num_encoding_sizes = 2;
    range->max_encoding_tokens = 4;

/** Minimum power period */
#define IW_POWER_PERIOD_MIN 1000000     /* 1 sec */
/** Maximum power period */
#define IW_POWER_PERIOD_MAX 120000000   /* 2 min */
/** Minimum power timeout value */
#define IW_POWER_TIMEOUT_MIN 1000       /* 1 ms */
/** Maximim power timeout value */
#define IW_POWER_TIMEOUT_MAX 1000000    /* 1 sec */

    /* Power Management duration & timeout */
    range->min_pmp = IW_POWER_PERIOD_MIN;
    range->max_pmp = IW_POWER_PERIOD_MAX;
    range->min_pmt = IW_POWER_TIMEOUT_MIN;
    range->max_pmt = IW_POWER_TIMEOUT_MAX;
    range->pmp_flags = IW_POWER_PERIOD;
    range->pmt_flags = IW_POWER_TIMEOUT;
    range->pm_capa = IW_POWER_PERIOD | IW_POWER_TIMEOUT | IW_POWER_ALL_R;

    /* 
     * Minimum version we recommend
     */
    range->we_version_source = 15;

    /* 
     * Version we are compiled with
     */
    range->we_version_compiled = WIRELESS_EXT;

    range->retry_capa = IW_RETRY_LIMIT;
    range->retry_flags = IW_RETRY_LIMIT | IW_RETRY_MAX;

    range->min_retry = MLAN_TX_RETRY_MIN;
    range->max_retry = MLAN_TX_RETRY_MAX;

    /* 
     * Set the qual, level and noise range values
     */
    /* 
     * need to put the right values here
     */
/** Maximum quality percentage */
#define IW_MAX_QUAL_PERCENT 5
/** Average quality percentage */
#define IW_AVG_QUAL_PERCENT 3
    range->max_qual.qual = IW_MAX_QUAL_PERCENT;
    range->max_qual.level = 0;
    range->max_qual.noise = 0;

    range->avg_qual.qual = IW_AVG_QUAL_PERCENT;
    range->avg_qual.level = 0;
    range->avg_qual.noise = 0;

    range->sensitivity = 0;
    /* 
     * Setup the supported power level ranges
     */
    memset(range->txpower, 0, sizeof(range->txpower));

    memset(&bss_info, 0, sizeof(bss_info));

    woal_get_bss_info(priv, MOAL_IOCTL_WAIT, &bss_info);

    range->txpower[0] = bss_info.min_power_level;
    range->txpower[1] = bss_info.max_power_level;
    range->num_txpower = 2;
    range->txpower_capa = IW_TXPOW_DBM | IW_TXPOW_RANGE;

    LEAVE();
    return 0;
}

/** 
 *  @brief get data rate 
 *
 *  @param priv                 A pointer to moal_private structure
 *  @param datarate             A pointer to mlan_ds_rate structure
 *
 *  @return                     MLAN_STATUS_SUCCESS -- success, otherwise fail          
 */
static mlan_status
woal_get_data_rate(moal_private * priv, mlan_ds_rate * datarate)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_ds_rate *rate = NULL;
    mlan_ioctl_req *req = NULL;

    ENTER();

    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_rate));
    if (req == NULL) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    rate = (mlan_ds_rate *) req->pbuf;
    rate->param.rate_cfg.rate_type = MLAN_RATE_VALUE;
    rate->sub_command = MLAN_OID_RATE_CFG;
    req->req_id = MLAN_IOCTL_RATE;
    req->action = MLAN_ACT_GET;

    if (MLAN_STATUS_SUCCESS == woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        if (datarate)
            memcpy(datarate, rate, sizeof(mlan_ds_rate));
    } else {
        ret = MLAN_STATUS_FAILURE;
    }
  done:
    if (req)
        kfree(req);
    LEAVE();
    return ret;
}

/** 
 *  @brief set scan type 
 *
 *  @param priv                 A pointer to moal_private structure
 *  @param scan_type            MLAN_SCAN_TYPE_ACTIVE/MLAN_SCAN_TYPE_PASSIVE
 *
 *  @return                     MLAN_STATUS_SUCCESS -- success, otherwise fail          
 */
static mlan_status
woal_set_scan_type(moal_private * priv, t_u32 scan_type)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_ds_scan *scan = NULL;
    mlan_ioctl_req *req = NULL;

    ENTER();

    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_scan));
    if (req == NULL) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
    scan = (mlan_ds_scan *) req->pbuf;
    scan->sub_command = MLAN_OID_SCAN_CONFIG;
    req->req_id = MLAN_IOCTL_SCAN;
    req->action = MLAN_ACT_SET;
    memset(&scan->param.scan_cfg, 0, sizeof(mlan_scan_cfg));
    scan->param.scan_cfg.scan_type = scan_type;

    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT))
        ret = MLAN_STATUS_FAILURE;
  done:
    if (req)
        kfree(req);
    LEAVE();
    return ret;
}

/** 
 *  @brief  Get band 
 *   
 *  @param priv                 A pointer to moal_private structure
 *  @param band                 A pointer to band buf
 * 
 *  @return                     MLAN_STATUS_SUCCESS -- success, otherwise fail          
 */
static mlan_status
woal_get_band(moal_private * priv, int *band)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_ioctl_req *req = NULL;
    mlan_ds_radio_cfg *radio_cfg = NULL;
    int support_band = 0;

    ENTER();

    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_radio_cfg));
    if (req == NULL) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
    radio_cfg = (mlan_ds_radio_cfg *) req->pbuf;
    radio_cfg->sub_command = MLAN_OID_BAND_CFG;
    req->req_id = MLAN_IOCTL_RADIO_CFG;
    /* Get config_bands, adhoc_start_band and adhoc_channel values from MLAN */
    req->action = MLAN_ACT_GET;
    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
    if (radio_cfg->param.band_cfg.config_bands & (BAND_B | BAND_G | BAND_GN))
        support_band |= WIFI_FREQUENCY_BAND_2GHZ;
    if (radio_cfg->param.band_cfg.config_bands & (BAND_A | BAND_AN))
        support_band |= WIFI_FREQUENCY_BAND_5GHZ;
    *band = support_band;
    if (support_band == WIFI_FREQUENCY_ALL_BAND)
        *band = WIFI_FREQUENCY_BAND_AUTO;
  done:
    if (req)
        kfree(req);

    LEAVE();
    return ret;
}

/** 
 *  @brief set band
 *
 *  @param priv            A pointer to moal_private structure
 *  @param pband            A pointer to band string.
 *
 *  @return                     MLAN_STATUS_SUCCESS -- success, otherwise fail          
 */
static mlan_status
woal_set_band(moal_private * priv, char *pband)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    int band = 0;
    mlan_ioctl_req *req = NULL;
    mlan_ds_radio_cfg *radio_cfg = NULL;

    ENTER();
    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_radio_cfg));
    if (req == NULL) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
    radio_cfg = (mlan_ds_radio_cfg *) req->pbuf;
    radio_cfg->sub_command = MLAN_OID_BAND_CFG;
    req->req_id = MLAN_IOCTL_RADIO_CFG;

    /* Get fw supported values from MLAN */
    req->action = MLAN_ACT_GET;
    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
    if (*pband == '0') {
        PRINTM(MIOCTL, "Set band to AUTO\n");
        band = radio_cfg->param.band_cfg.fw_bands;
    } else if (*pband == '1') {
        PRINTM(MIOCTL, "Set band to 5G\n");
        if (!(radio_cfg->param.band_cfg.fw_bands & BAND_A)) {
            PRINTM(MERROR, "Don't support 5G band\n");
            ret = MLAN_STATUS_FAILURE;
            goto done;
        }
        band = BAND_A;
        band |= BAND_AN;
    } else if (*pband == '2') {
        PRINTM(MIOCTL, "Set band to 2G\n");
        band = BAND_B | BAND_G;
        band |= BAND_GN;
    } else {
        PRINTM(MERROR, "unsupported band\n");
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
    /* Set config_bands to MLAN */
    req->action = MLAN_ACT_SET;
    memset(&radio_cfg->param.band_cfg, 0, sizeof(mlan_ds_band_cfg));
    radio_cfg->param.band_cfg.config_bands = band;
    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
  done:
    if (req)
        kfree(req);
    LEAVE();
    return ret;
}

/** 
 *  @brief  Get power mode 
 *   
 *  @param priv                 A pointer to moal_private structure
 *  @param powermode            A pointer to powermode buf   
 * 
 *  @return                     0 --success, otherwise fail
 */
static int
woal_get_powermode(moal_private * priv, int *powermode)
{
    int ret = 0;
    mlan_ioctl_req *req = NULL;
    mlan_ds_pm_cfg *pm_cfg = NULL;

    ENTER();

    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_pm_cfg));
    if (req == NULL) {
        ret = -ENOMEM;
        goto done;
    }
    pm_cfg = (mlan_ds_pm_cfg *) req->pbuf;
    pm_cfg->sub_command = MLAN_OID_PM_CFG_IEEE_PS;
    req->req_id = MLAN_IOCTL_PM_CFG;
    req->action = MLAN_ACT_GET;

    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = -EFAULT;
        goto done;
    }
#define DRIVER_POWER_MODE_AUTO      0
#define DRIVER_POWER_MODE_ACTIVE    1
    if (pm_cfg->param.ps_mode)
        *powermode = DRIVER_POWER_MODE_AUTO;
    else
        *powermode = DRIVER_POWER_MODE_ACTIVE;
  done:
    if (req)
        kfree(req);

    LEAVE();
    return ret;
}

/** 
 *  @brief set power mode
 *
 *  @param priv                 A pointer to moal_private structure
 *  @param powermode            A pointer to powermode string.
 *
 *  @return                     MLAN_STATUS_SUCCESS -- success, otherwise fail          
 */
static mlan_status
woal_set_powermode(moal_private * priv, char *powermode)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_ds_pm_cfg *pm_cfg = NULL;
    mlan_ioctl_req *req = NULL;

    ENTER();

    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_pm_cfg));
    if (req == NULL) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
    pm_cfg = (mlan_ds_pm_cfg *) req->pbuf;
    pm_cfg->sub_command = MLAN_OID_PM_CFG_IEEE_PS;
    req->req_id = MLAN_IOCTL_PM_CFG;
    req->action = MLAN_ACT_SET;

    if (*powermode == '1') {
        PRINTM(MIOCTL, "Disable power save\n");
        pm_cfg->param.ps_mode = 0;
    } else if (*powermode == '0') {
        PRINTM(MIOCTL, "Enable power save\n");
        pm_cfg->param.ps_mode = 1;
    } else {
        PRINTM(MERROR, "unsupported power mode\n");
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT))
        ret = MLAN_STATUS_FAILURE;
  done:
    if (req)
        kfree(req);
    LEAVE();
    return ret;
}

/** 
 *  @brief set scan time 
 *
 *  @param priv                 A pointer to moal_private structure
 *  @param passive_scan_time    passive scan time
 *  @param specific_scan_time   specific scan time
 *
 *  @return                     MLAN_STATUS_SUCCESS -- success, otherwise fail          
 */
static mlan_status
woal_set_scan_time(moal_private * priv, t_u16 passive_scan_time,
                   t_u16 specific_scan_time)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_ds_scan *scan = NULL;
    mlan_ioctl_req *req = NULL;

    ENTER();

    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_scan));
    if (req == NULL) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
    scan = (mlan_ds_scan *) req->pbuf;
    scan->sub_command = MLAN_OID_SCAN_CONFIG;
    req->req_id = MLAN_IOCTL_SCAN;
    req->action = MLAN_ACT_SET;
    memset(&scan->param.scan_cfg, 0, sizeof(mlan_scan_cfg));
    scan->param.scan_cfg.scan_time.specific_scan_time = specific_scan_time;
    scan->param.scan_cfg.scan_time.passive_scan_time = passive_scan_time;
    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT))
        ret = MLAN_STATUS_FAILURE;
  done:
    if (req)
        kfree(req);
    LEAVE();
    return ret;
}

/** 
 *  @brief Request user scan
 *
 *  @param priv                 A pointer to moal_private structure
 *  @param wait_option          Wait option  
 *  @param req_ssid             A pointer to mlan_802_11_ssid structure
 *
 *  @return                     MLAN_STATUS_SUCCESS -- success, otherwise fail          
 */
mlan_status
woal_request_userscan(moal_private * priv,
                      t_u8 wait_option, wlan_user_scan_cfg * scan_cfg)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_ioctl_req *ioctl_req = NULL;
    mlan_ds_scan *scan = NULL;
    mlan_status status = MLAN_STATUS_SUCCESS;
    ENTER();

    if (MOAL_ACQ_SEMAPHORE_BLOCK(&priv->async_sem)) {
        PRINTM(MERROR, "Acquire semaphore error, request_scan\n");
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }
    priv->scan_pending_on_block = MTRUE;

    /* Allocate an IOCTL request buffer */
    ioctl_req =
        woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_scan) +
                                  sizeof(wlan_user_scan_cfg));
    if (ioctl_req == NULL) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    scan = (mlan_ds_scan *) ioctl_req->pbuf;
    scan->sub_command = MLAN_OID_SCAN_USER_CONFIG;
    ioctl_req->req_id = MLAN_IOCTL_SCAN;
    ioctl_req->action = MLAN_ACT_SET;
    memcpy(scan->param.user_scan.scan_cfg_buf, scan_cfg,
           sizeof(wlan_user_scan_cfg));
    /* Send IOCTL request to MLAN */
    status = woal_request_ioctl(priv, ioctl_req, wait_option);
    if (status == MLAN_STATUS_FAILURE) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
  done:
    if ((ioctl_req) && (status != MLAN_STATUS_PENDING))
        kfree(ioctl_req);

    if (ret == MLAN_STATUS_FAILURE) {
        priv->scan_pending_on_block = MFALSE;
        MOAL_REL_SEMAPHORE(&priv->async_sem);
    }
    LEAVE();
    return ret;
}

/** 
 *  @brief request combo scan
 *
 *  @param priv                 A pointer to moal_private structure
 *  @param scan_cfg             A pointer to wlan_user_scan_cfg structure
 *
 *  @return                     MLAN_STATUS_SUCCESS -- success, otherwise fail          
 */
static mlan_status
woal_do_combo_scan(moal_private * priv, wlan_user_scan_cfg * scan_cfg)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
#ifdef REASSOCIATION
    moal_handle *handle = priv->phandle;
#endif
    ENTER();
    if (priv->scan_pending_on_block == MTRUE) {
        PRINTM(MINFO, "scan already in processing...\n");
        LEAVE();
        return ret;
    }
#ifdef REASSOCIATION
    if (MOAL_ACQ_SEMAPHORE_BLOCK(&handle->reassoc_sem)) {
        PRINTM(MERROR, "Acquire semaphore error, woal_do_combo_scan\n");
        LEAVE();
        return -EBUSY;
    }
#endif /* REASSOCIATION */
#ifdef ANDROID
/* titan ++ set scan wake_lock*/   
    PRINTM(MMSG, "woal_set_scan: wake_lock\n");
    wake_lock(&handle->scan_lock);
/* titan -- */
#endif  
    priv->report_scan_result = MTRUE;
    if (!scan_cfg) {
        ret = woal_request_scan(priv, MOAL_NO_WAIT, NULL);
    } else {
        ret = woal_request_userscan(priv, MOAL_NO_WAIT, scan_cfg);
    }
#ifdef REASSOCIATION
    MOAL_REL_SEMAPHORE(&handle->reassoc_sem);
#endif
    LEAVE();
    return ret;
}

/** 
 *  @brief set combo scan
 *
 *  @param priv                 A pointer to moal_private structure
 *  @param buf                  A pointer to scan command buf
 *  @param length               buf length
 *
 *  @return                     MLAN_STATUS_SUCCESS -- success, otherwise fail          
 */
static int
woal_set_combo_scan(moal_private * priv, char *buf, int length)
{
    int ret = 0;
    wlan_user_scan_cfg scan_cfg;
    t_u8 *ptr = buf + WEXT_CSCAN_HEADER_SIZE;
    int buf_left = length - WEXT_CSCAN_HEADER_SIZE;
    int num_ssid = 0;
    int num_chan = 0;
    int ssid_len = 0;
    int i = 0;
    t_u16 passive_scan_time = 0;
    t_u16 specific_scan_time = 0;

    ENTER();
    memset(&scan_cfg, 0, sizeof(scan_cfg));
    while (buf_left >= 2) {
        switch (*ptr) {
        case WEXT_CSCAN_SSID_SECTION:
            ssid_len = *(ptr + 1);
            if ((buf_left < (ssid_len + 2)) ||
                (ssid_len > MLAN_MAX_SSID_LENGTH)) {
                PRINTM(MERROR, "Invalid ssid, buf_left=%d, ssid_len=%d\n",
                       buf_left, ssid_len);
                buf_left = 0;
                break;
            }
            if (num_ssid < (MRVDRV_MAX_SSID_LIST_LENGTH - 1)) {
                strncpy(scan_cfg.ssid_list[num_ssid].ssid, ptr + 2, ssid_len);
                scan_cfg.ssid_list[num_ssid].max_len = 0;
                PRINTM(MIOCTL, "Combo scan: ssid=%s\n",
                       scan_cfg.ssid_list[num_ssid].ssid);
                num_ssid++;
            }
            buf_left -= ssid_len + 2;
            ptr += ssid_len + 2;
            break;
        case WEXT_CSCAN_CHANNEL_SECTION:
            num_chan = ptr[1];
            if ((buf_left < (num_chan + 2)) ||
                (num_chan > WLAN_USER_SCAN_CHAN_MAX)) {
                PRINTM(MERROR,
                       "Invalid channel list, buf_left=%d, num_chan=%d\n",
                       buf_left, num_chan);
                buf_left = 0;
                break;
            }
            for (i = 0; i < num_chan; i++) {
                scan_cfg.chan_list[i].chan_number = ptr[2 + i];
                PRINTM(MIOCTL, "Combo scan: chan=%d\n",
                       scan_cfg.chan_list[i].chan_number);
            }
            buf_left -= 2 + num_chan;
            ptr += 2 + num_chan;
            break;
        case WEXT_CSCAN_PASV_DWELL_SECTION:
            if (buf_left < 3) {
                PRINTM(MERROR, "Invalid PASV_DWELL_SECTION, buf_left=%d\n",
                       buf_left);
                buf_left = 0;
                break;
            }
            passive_scan_time = ptr[2] << 8 | ptr[1];
            ptr += 3;
            buf_left -= 3;
            break;
        case WEXT_CSCAN_HOME_DWELL_SECTION:
            if (buf_left < 3) {
                PRINTM(MERROR, "Invalid HOME_DWELL_SECTION, buf_left=%d\n",
                       buf_left);
                buf_left = 0;
                break;
            }
            specific_scan_time = ptr[2] << 8 | ptr[1];
            ptr += 3;
            buf_left -= 3;
            break;
        default:
            buf_left = 0;
            break;
        }
    }
    if (passive_scan_time || specific_scan_time) {
        PRINTM(MIOCTL, "Set passive_scan_time=%d specific_scan_time=%d\n",
               passive_scan_time, specific_scan_time);
        if (MLAN_STATUS_FAILURE ==
            woal_set_scan_time(priv, passive_scan_time, specific_scan_time)) {
            ret = -EFAULT;
            goto done;
        }
    }
    if (num_ssid || num_chan) {
        if (num_ssid) {
            /* Add broadcast scan to ssid_list */
            scan_cfg.ssid_list[num_ssid].max_len = 0xff;
        }
        if (MLAN_STATUS_FAILURE == woal_do_combo_scan(priv, &scan_cfg))
            ret = -EFAULT;
    } else {
        /* request broadcast scan */
        if (MLAN_STATUS_FAILURE == woal_do_combo_scan(priv, NULL))
            ret = -EFAULT;
    }
  done:
    LEAVE();
    return ret;
}

/** 
 *  @brief Add RX Filter
 *
 *  @param priv                 A pointer to moal_private structure
 *  @param rxfilter             A pointer to rxfilter string.
 *
 *  @return                     MLAN_STATUS_SUCCESS -- success, otherwise fail          
 */
static mlan_status
woal_add_rxfilter(moal_private * priv, char *rxfilter)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    ENTER();
    if (*rxfilter == '0') {
        PRINTM(MIOCTL, "Add unicast filter\n");
        priv->rx_filter |= RX_FILTER_UNICAST;
    } else if (*rxfilter == '1') {
        PRINTM(MIOCTL, "Add broadcast filter\n");
        priv->rx_filter |= RX_FILTER_BROADCAST;
    } else if (*rxfilter == '2') {
        PRINTM(MIOCTL, "Add IPV4 multicast filter\n");
        priv->rx_filter |= RX_FILTER_IPV4_MULTICAST;
    } else if (*rxfilter == '3') {
        PRINTM(MIOCTL, "Add IPV6 multicast fitler\n");
        priv->rx_filter |= RX_FILTER_IPV6_MULTICAST;
    } else {
        PRINTM(MERROR, "unsupported rx fitler\n");
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
  done:
    LEAVE();
    return ret;
}

/** 
 *  @brief Remove RX Filter
 *
 *  @param priv                 A pointer to moal_private structure
 *  @param rxfilter             A pointer to rxfilter string.
 *
 *  @return                     MLAN_STATUS_SUCCESS -- success, otherwise fail          
 */
static mlan_status
woal_remove_rxfilter(moal_private * priv, char *rxfilter)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    ENTER();
    if (*rxfilter == '0') {
        PRINTM(MIOCTL, "Remove unicast filter\n");
        priv->rx_filter &= ~RX_FILTER_UNICAST;
    } else if (*rxfilter == '1') {
        PRINTM(MIOCTL, "Remove broadcast filter\n");
        priv->rx_filter &= ~RX_FILTER_BROADCAST;
    } else if (*rxfilter == '2') {
        PRINTM(MIOCTL, "Remove IPV4 multicast filter\n");
        priv->rx_filter &= ~RX_FILTER_IPV4_MULTICAST;
    } else if (*rxfilter == '3') {
        PRINTM(MIOCTL, "Remove IPV6 multicast fitler\n");
        priv->rx_filter &= ~RX_FILTER_IPV6_MULTICAST;
    } else {
        PRINTM(MERROR, "unsupported rx fitler\n");
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
  done:
    LEAVE();
    return ret;
}

/**
 * @brief Set QoS configuration
 * 
 * @param priv     A pointer to moal_private structure
 * 
 * @return         MLAN_STATUS_SUCCESS -- success, otherwise fail 
 */
static mlan_status
woal_set_qos_cfg(moal_private * priv, char *qos_cfg)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_ds_wmm_cfg *cfg = NULL;
    mlan_ioctl_req *req = NULL;
    int qosinfo = 0;

    ENTER();
    if (MLAN_STATUS_SUCCESS != woal_atoi(&qosinfo, qos_cfg)) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
    PRINTM(MIOCTL, "set qosinfo=%d\n", qosinfo);
    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_wmm_cfg));
    if (req == NULL) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
    cfg = (mlan_ds_wmm_cfg *) req->pbuf;
    cfg->sub_command = MLAN_OID_WMM_CFG_QOS;
    req->req_id = MLAN_IOCTL_WMM_CFG;
    req->action = MLAN_ACT_SET;
    cfg->param.qos_cfg = (t_u8) qosinfo;
    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT))
        ret = MLAN_STATUS_FAILURE;
  done:
    if (req)
        kfree(req);
    LEAVE();
    return ret;
}

/**
 * @brief Set sleep period
 * 
 * @param priv     A pointer to moal_private structure
 * 
 * @return         MLAN_STATUS_SUCCESS -- success, otherwise fail 
 */
static int
woal_set_sleeppd(moal_private * priv, char *psleeppd)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_ds_pm_cfg *pm_cfg = NULL;
    mlan_ioctl_req *req = NULL;
    int sleeppd = 0;

    ENTER();

    if (MLAN_STATUS_SUCCESS != woal_atoi(&sleeppd, psleeppd)) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
    PRINTM(MIOCTL, "set sleeppd=%d\n", sleeppd);
    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_pm_cfg));
    if (req == NULL) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
    pm_cfg = (mlan_ds_pm_cfg *) req->pbuf;
    pm_cfg->sub_command = MLAN_OID_PM_CFG_SLEEP_PD;
    req->req_id = MLAN_IOCTL_PM_CFG;
    if ((sleeppd <= MAX_SLEEP_PERIOD && sleeppd >= MIN_SLEEP_PERIOD) ||
        (sleeppd == 0)
        || (sleeppd == SLEEP_PERIOD_RESERVED_FF)
        ) {
        req->action = MLAN_ACT_SET;
        pm_cfg->param.sleep_period = sleeppd;
    } else {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
  done:
    if (req)
        kfree(req);
    LEAVE();
    return ret;
}

/**
 *  @brief Set priv command 
 *
 *  @param dev          A pointer to net_device structure
 *  @param info         A pointer to iw_request_info structure
 *  @param dwrq         A pointer to iw_point structure
 *  @param extra        A pointer to extra data buf
 *
 *  @return                     0 --success, otherwise fail
 */
static int
woal_set_priv(struct net_device *dev, struct iw_request_info *info,
              struct iw_point *dwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    char *buf = NULL;
    char *scanblock = NULL;
    int power_mode = 0;
    int band = 0;
    char *pband = NULL;
    mlan_bss_info bss_info;
    mlan_ds_get_signal signal;
    mlan_ds_rate rate;
    char *pdata;
    t_u8 country_code[COUNTRY_CODE_LEN];
    int len = 0;
    ENTER();
    if (!(buf = kmalloc(dwrq->length + 1, GFP_KERNEL))) {
        ret = -ENOMEM;
        goto done;
    }
    memset(buf, 0, dwrq->length + 1);
    if (copy_from_user(buf, dwrq->pointer, dwrq->length)) {
        ret = -EFAULT;
        goto done;
    }
    PRINTM(MIOCTL, "SIOCSIWPRIV requst = %s\n", buf);
    if (strncmp(buf, "RSSI", strlen("RSSI")) == 0) {
        if (MLAN_STATUS_SUCCESS != woal_get_bss_info(priv,
                                                     MOAL_IOCTL_WAIT,
                                                     &bss_info)) {
            ret = -EFAULT;
            goto done;
        }
        if (bss_info.media_connected) {
            if (MLAN_STATUS_SUCCESS !=
                woal_get_signal_info(priv, MOAL_IOCTL_WAIT, &signal)) {
                ret = -EFAULT;
                goto done;
            }
            len =
                sprintf(buf, "%s rssi %d\n", bss_info.ssid.ssid,
                        signal.bcn_rssi_avg) + 1;
        } else {
            len = sprintf(buf, "OK\n") + 1;
        }
    } else if (strncmp(buf, "LINKSPEED", strlen("LINKSPEED")) == 0) {
        if (MLAN_STATUS_SUCCESS != woal_get_data_rate(priv, &rate)) {
            ret = -EFAULT;
            goto done;
        }
        PRINTM(MIOCTL, "tx rate=%d\n", (int) rate.param.rate_cfg.rate);
        len =
            sprintf(buf, "LinkSpeed %d\n",
                    (int) (rate.param.rate_cfg.rate * 500000 / 1000000)) + 1;
    } else if (strncmp(buf, "MACADDR", strlen("MACADDR")) == 0) {
        len =
            sprintf(buf, "Macaddr = %02X:%02X:%02X:%02X:%02X:%02X\n",
                    priv->current_addr[0], priv->current_addr[1],
                    priv->current_addr[2], priv->current_addr[3],
                    priv->current_addr[4], priv->current_addr[5]) + 1;
    } else if (strncmp(buf, "GETPOWER", strlen("GETPOWER")) == 0) {
        if (woal_get_powermode(priv, &power_mode)) {
            ret = -EFAULT;
            goto done;
        }
        len = sprintf(buf, "powermode = %d\n", power_mode) + 1;
    } else if (strncmp(buf, "SCAN-ACTIVE", strlen("SCAN-ACTIVE")) == 0) {
        if (MLAN_STATUS_SUCCESS !=
            woal_set_scan_type(priv, MLAN_SCAN_TYPE_ACTIVE)) {
            ret = -EFAULT;
            goto done;
        }
        PRINTM(MIOCTL, "Set Active Scan\n");
        len = sprintf(buf, "OK\n") + 1;
    } else if (strncmp(buf, "SCAN-PASSIVE", strlen("SCAN-PASSIVE")) == 0) {
        if (MLAN_STATUS_SUCCESS !=
            woal_set_scan_type(priv, MLAN_SCAN_TYPE_PASSIVE)) {
            ret = -EFAULT;
            goto done;
        }
        PRINTM(MIOCTL, "Set Passive Scan\n");
        len = sprintf(buf, "OK\n") + 1;
    } else if (strncmp(buf, "POWERMODE", strlen("POWERMODE")) == 0) {
        pdata = buf + strlen("POWERMODE") + 1;
        if (MLAN_STATUS_SUCCESS != woal_set_powermode(priv, pdata)) {
            ret = -EFAULT;
            goto done;
        }
        len = sprintf(buf, "OK\n") + 1;
    } else if (strncmp(buf, "COUNTRY", strlen("COUNTRY")) == 0) {
        memset(country_code, 0, sizeof(country_code));
        memcpy(country_code, buf + strlen("COUNTRY") + 1,
               strlen(buf) - strlen("COUNTRY") - 1);
        PRINTM(MIOCTL, "Set COUNTRY %s\n", country_code);
        if (MLAN_STATUS_SUCCESS != woal_set_regioncode(priv, country_code)) {
            ret = -EFAULT;
            goto done;
        }
        len = sprintf(buf, "OK\n") + 1;
    } else if (memcmp(buf, WEXT_CSCAN_HEADER, WEXT_CSCAN_HEADER_SIZE) == 0) {
        PRINTM(MIOCTL, "Set Combo Scan\n");
        if (MLAN_STATUS_SUCCESS != woal_set_combo_scan(priv, buf, dwrq->length)) {
            ret = -EFAULT;
            goto done;
        }
        len = sprintf(buf, "OK\n") + 1;
    } else if (strncmp(buf, "SCAN-BLOCK", strlen("SCAN-BLOCK")) == 0) {
        scanblock = buf + strlen("SCAN-BLOCK") + 1;
        if (*scanblock == '1') {
            PRINTM(MIOCTL, "Set SCAN-BLOCK ON\n");
            priv->scan_block_flag = MTRUE;
        } else if (*scanblock == '0') {
            PRINTM(MIOCTL, "Set SCAN-BLOCK OFF\n");
            priv->scan_block_flag = MFALSE;
        }
        len = sprintf(buf, "OK\n") + 1;
    } else if (strncmp(buf, "GETBAND", strlen("GETBAND")) == 0) {
        if (MLAN_STATUS_SUCCESS != woal_get_band(priv, &band)) {
            ret = -EFAULT;
            goto done;
        }
        len = sprintf(buf, "Band %d\n", band) + 1;
    } else if (strncmp(buf, "SETBAND", strlen("SETBAND")) == 0) {
        pband = buf + strlen("SETBAND") + 1;
        if (MLAN_STATUS_SUCCESS != woal_set_band(priv, pband)) {
            ret = -EFAULT;
            goto done;
        }
        len = sprintf(buf, "OK\n") + 1;
    } else if (strncmp(buf, "START", strlen("START")) == 0) {
    		woal_send_iwevcustom_event(priv, CUS_EVT_START); //titan
        len = sprintf(buf, "OK\n") + 1;
    } else if (strncmp(buf, "STOP", strlen("STOP")) == 0) {
				woal_disconnect(priv, MOAL_IOCTL_WAIT, NULL);	//titan
				woal_send_iwevcustom_event(priv, CUS_EVT_STOP);    	//titan    	
        len = sprintf(buf, "OK\n") + 1;
    } else if (strncmp(buf, "SETSUSPENDOPT", strlen("SETSUSPENDOPT")) == 0) {
        /* it will be done by GUI */
        len = sprintf(buf, "OK\n") + 1;
    } else if (strncmp(buf, "BTCOEXMODE", strlen("BTCOEXMODE")) == 0) {
        len = sprintf(buf, "OK\n") + 1;
    } else if (strncmp(buf, "BTCOEXSCAN-START", strlen("BTCOEXSCAN-START")) ==
               0) {
        len = sprintf(buf, "OK\n") + 1;
    } else if (strncmp(buf, "BTCOEXSCAN-STOP", strlen("BTCOEXSCAN-STOP")) == 0) {
        len = sprintf(buf, "OK\n") + 1;
    } else if (strncmp(buf, "BTCOEXSCAN-START", strlen("BTCOEXSCAN-START")) ==
               0) {
        len = sprintf(buf, "OK\n") + 1;
    } else if (strncmp(buf, "BTCOEXSCAN-STOP", strlen("BTCOEXSCAN-STOP")) == 0) {
        len = sprintf(buf, "OK\n") + 1;
    } else if (strncmp(buf, "BGSCAN-START", strlen("BGSCAN-START")) == 0) {
        // TODO
        len = sprintf(buf, "OK\n") + 1;
    } else if (strncmp(buf, "BGSCAN-STOP", strlen("BGSCAN-STOP")) == 0) {
        // TODO
        len = sprintf(buf, "OK\n") + 1;
    } else if (strncmp(buf, "RXFILTER-START", strlen("RXFILTER-START")) == 0) {
        // TODO
        len = sprintf(buf, "OK\n") + 1;
    } else if (strncmp(buf, "RXFILTER-STOP", strlen("RXFILTER-STOP")) == 0) {
        // TODO
        len = sprintf(buf, "OK\n") + 1;
    } else if (strncmp(buf, "RXFILTER-ADD", strlen("RXFILTER-ADD")) == 0) {
        pdata = buf + strlen("RXFILTER-ADD") + 1;
        if (MLAN_STATUS_SUCCESS != woal_add_rxfilter(priv, pdata)) {
            ret = -EFAULT;
            goto done;
        }
        len = sprintf(buf, "OK\n") + 1;
    } else if (strncmp(buf, "RXFILTER-REMOVE", strlen("RXFILTER-REMOVE")) == 0) {
        pdata = buf + strlen("RXFILTER-REMOVE") + 1;
        if (MLAN_STATUS_SUCCESS != woal_remove_rxfilter(priv, pdata)) {
            ret = -EFAULT;
            goto done;
        }
        len = sprintf(buf, "OK\n") + 1;
    } else if (strncmp(buf, "QOSINFO", strlen("QOSINFO")) == 0) {
        pdata = buf + strlen("QOSINFO") + 1;
        if (MLAN_STATUS_SUCCESS != woal_set_qos_cfg(priv, pdata)) {
            ret = -EFAULT;
            goto done;
        }
        len = sprintf(buf, "OK\n") + 1;
    } else if (strncmp(buf, "SLEEPPD", strlen("SLEEPPD")) == 0) {
        pdata = buf + strlen("SLEEPPD") + 1;
        if (MLAN_STATUS_SUCCESS != woal_set_sleeppd(priv, pdata)) {
            ret = -EFAULT;
            goto done;
        }
        len = sprintf(buf, "OK\n") + 1;
    } else {
        PRINTM(MIOCTL, "Unknow PRIVATE command: %s, ignored\n", buf);
        ret = -EFAULT;
        goto done;
    }
    PRINTM(MIOCTL, "PRIV Command return: %s, length=%d\n", buf, len);
    dwrq->length = len;
    if (copy_to_user(dwrq->pointer, buf, dwrq->length)) {
        ret = -EFAULT;
    }
  done:
    if (buf)
        kfree(buf);
    LEAVE();
    return ret;
}

/**
 *  @brief Scan Network
 *
 *  @param dev          A pointer to net_device structure
 *  @param info         A pointer to iw_request_info structure
 *  @param vwrq         A pointer to iw_param structure
 *  @param extra        A pointer to extra data buf
 *
 *  @return             0--success, otherwise fail
 */
int
woal_set_scan(struct net_device *dev, struct iw_request_info *info,
              struct iw_param *vwrq, char *extra)
{
    int ret = 0;
    moal_private *priv = (moal_private *) netdev_priv(dev);
#ifdef REASSOCIATION
    moal_handle *handle = priv->phandle;
#endif
#if WIRELESS_EXT >= 18
    struct iw_scan_req *req;
    struct iw_point *dwrq = (struct iw_point *) vwrq;
#endif
    mlan_802_11_ssid req_ssid;

    ENTER();
    if (priv->scan_pending_on_block == MTRUE) {
        PRINTM(MINFO, "scan already in processing...\n");
        LEAVE();
        return ret;
    }
    if ((priv->media_connected == MTRUE) && (priv->scan_block_flag == MTRUE)) {
        PRINTM(MIOCTL, "scan block on\n");
        LEAVE();
        return ret;
    }
#ifdef REASSOCIATION
    if (MOAL_ACQ_SEMAPHORE_BLOCK(&handle->reassoc_sem)) {
        PRINTM(MERROR, "Acquire semaphore error, woal_set_scan\n");
        LEAVE();
        return -EBUSY;
    }
#endif /* REASSOCIATION */
#ifdef ANDROID
/* titan ++ set scan wake_lock*/   
    PRINTM(MMSG, "woal_set_scan: wake_lock\n");
    wake_lock(&handle->scan_lock);
/* titan -- */
#endif 
    priv->report_scan_result = MTRUE;

    memset(&req_ssid, 0x00, sizeof(mlan_802_11_ssid));

#if WIRELESS_EXT >= 18
    if ((dwrq->flags & IW_SCAN_THIS_ESSID) &&
        (dwrq->length == sizeof(struct iw_scan_req))) {
        req = (struct iw_scan_req *) extra;

        if (req->essid_len <= MLAN_MAX_SSID_LENGTH) {

            req_ssid.ssid_len = req->essid_len;
            memcpy(req_ssid.ssid, (t_u8 *) req->essid, req->essid_len);
            if (MLAN_STATUS_SUCCESS !=
                woal_request_scan(priv, MOAL_NO_WAIT, &req_ssid)) {
                ret = -EFAULT;
                goto done;
            }
        }
    } else {
#endif
        if (MLAN_STATUS_SUCCESS != woal_request_scan(priv, MOAL_NO_WAIT, NULL)) {
            ret = -EFAULT;
            goto done;
        }
#if WIRELESS_EXT >= 18
    }
#endif

    if (priv->phandle->surprise_removed) {
        ret = -EFAULT;
        goto done;
    }

  done:
#ifdef REASSOCIATION
    MOAL_REL_SEMAPHORE(&handle->reassoc_sem);
#endif

    LEAVE();
    return ret;
}

/** MEAS report timeout value in seconds */

#define MEAS_REPORT_TIMEOUT 75*HZ
/**
 *  @brief Issue MLAN_OID_11H_CHANNEL_CHECK ioctl
 *
 *  @param priv     Pointer to the moal_private driver data struct
 *
 *  @return         0 --success, otherwise fail
 */
static int
woal_11h_channel_check_ioctl(moal_private * priv)
{
    int ret = 0;
    mlan_ioctl_req *req = NULL;
    mlan_ds_11h_cfg *ds_11hcfg = NULL;

    ENTER();

    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_11h_cfg));
    if (req == NULL) {
        ret = -ENOMEM;
        goto done;
    }
    ds_11hcfg = (mlan_ds_11h_cfg *) req->pbuf;

    ds_11hcfg->sub_command = MLAN_OID_11H_CHANNEL_CHECK;
    req->req_id = MLAN_IOCTL_11H_CFG;
    req->action = MLAN_ACT_SET;

    priv->phandle->meas_wait_q_woken = MFALSE;

    /* Send Channel Check command and wait until the report is ready */
    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT)) {
        ret = -EFAULT;
        goto done;
    }

    /* Wait until the report is ready */
    wait_event_interruptible_timeout(priv->phandle->meas_wait_q,
                                     priv->phandle->meas_wait_q_woken,
                                     MEAS_REPORT_TIMEOUT);

  done:
    if (req)
        kfree(req);

    LEAVE();
    return ret;
}

/**
 *  @brief find ssid in scan_table
 *
 *  @param priv         A pointer to moal_private
 *  @ssid_bssid         A pointer to mlan_ssid_bssid structure
 *  @return             MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
woal_find_essid(moal_private * priv, mlan_ssid_bssid * ssid_bssid)
{
    mlan_scan_resp scan_resp;
    struct timeval t;
    if (MLAN_STATUS_SUCCESS !=
        woal_get_scan_table(priv, MOAL_IOCTL_WAIT, &scan_resp))
        return MLAN_STATUS_FAILURE;
    do_gettimeofday(&t);
/** scan result timeout value */
#define SCAN_RESULT_AGEOUT      10
    if (t.tv_sec > (scan_resp.age_in_secs + SCAN_RESULT_AGEOUT))
        return MLAN_STATUS_FAILURE;
    return woal_find_best_network(priv, MOAL_IOCTL_WAIT, ssid_bssid);
}

/**
 *  @brief Set essid
 *
 *  @param dev          A pointer to net_device structure
 *  @param info         A pointer to iw_request_info structure
 *  @param dwrq         A pointer to iw_point structure
 *  @param extra        A pointer to extra data buf
 *
 *  @return             0--success, otherwise fail
 */
static int
woal_set_essid(struct net_device *dev, struct iw_request_info *info,
               struct iw_point *dwrq, char *extra)
{
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_802_11_ssid req_ssid;
    mlan_ssid_bssid ssid_bssid;
#ifdef REASSOCIATION
    moal_handle *handle = priv->phandle;
    mlan_bss_info bss_info;
#endif
    int ret = 0;
    t_u32 mode = 0;

    ENTER();
#ifdef ANDROID    
/* titan ++ set connect wake_lock */
    PRINTM(MMSG, "Woal_set_essid: wake_lock\n");
    wake_lock(&handle->assoc_lock);
/* titan -- */
#endif    
#ifdef REASSOCIATION
    /* Cancel re-association */
    priv->reassoc_required = MFALSE;

    if (MOAL_ACQ_SEMAPHORE_BLOCK(&handle->reassoc_sem)) {
        PRINTM(MERROR, "Acquire semaphore error, woal_set_essid\n");
        LEAVE();
        return -EBUSY;
    }
#endif /* REASSOCIATION */

    /* Check the size of the string */
    if (dwrq->length > IW_ESSID_MAX_SIZE + 1) {
        ret = -E2BIG;
        goto setessid_ret;
    }
    memset(&req_ssid, 0, sizeof(mlan_802_11_ssid));
    memset(&ssid_bssid, 0, sizeof(mlan_ssid_bssid));

    /* 
     * Check if we asked for `any' or 'particular'
     */
    if (!dwrq->flags) {
        /* Do normal SSID scanning */
        if (MLAN_STATUS_SUCCESS !=
            woal_request_scan(priv, MOAL_IOCTL_WAIT, NULL)) {
            ret = -EFAULT;
            goto setessid_ret;
        }
    } else {
        /* Set the SSID */
#if WIRELESS_EXT > 20
        req_ssid.ssid_len = dwrq->length;
#else
        req_ssid.ssid_len = dwrq->length - 1;
#endif
        memcpy(req_ssid.ssid, extra,
               MIN(req_ssid.ssid_len, MLAN_MAX_SSID_LENGTH));
        if (!req_ssid.ssid_len || (MFALSE == woal_ssid_valid(&req_ssid))) {
            PRINTM(MERROR, "Invalid SSID - aborting set_essid\n");
            ret = -EINVAL;
            goto setessid_ret;
        }
        PRINTM(MINFO, "Requested new SSID = %s\n", (char *) req_ssid.ssid);
        memcpy(&ssid_bssid.ssid, &req_ssid, sizeof(mlan_802_11_ssid));

        if (dwrq->flags != 0xFFFF) {
            if (MLAN_STATUS_SUCCESS != woal_find_essid(priv, &ssid_bssid)) {
                /* Do specific SSID scanning */
                if (MLAN_STATUS_SUCCESS !=
                    woal_request_scan(priv, MOAL_IOCTL_WAIT, &req_ssid)) {
                    ret = -EFAULT;
                    goto setessid_ret;
                }
            }
        }

    }

    /* disconnect before try to associate */
    woal_disconnect(priv, MOAL_IOCTL_WAIT, NULL);
    mode = woal_get_mode(priv, MOAL_IOCTL_WAIT);

    if (mode != IW_MODE_ADHOC) {
        if (MLAN_STATUS_SUCCESS !=
            woal_find_best_network(priv, MOAL_IOCTL_WAIT, &ssid_bssid)) {
            ret = -EFAULT;
            goto setessid_ret;
        }
    } else if (MLAN_STATUS_SUCCESS !=
               woal_find_best_network(priv, MOAL_IOCTL_WAIT, &ssid_bssid))
        /* Adhoc start, Check the channel command */
        woal_11h_channel_check_ioctl(priv);

    /* Connect to BSS by ESSID */
    memset(&ssid_bssid.bssid, 0, MLAN_MAC_ADDR_LENGTH);

    if (MLAN_STATUS_SUCCESS != woal_bss_start(priv,
                                              MOAL_IOCTL_WAIT, &ssid_bssid)) {
        ret = -EFAULT;
        goto setessid_ret;
    }
#ifdef REASSOCIATION
    memset(&bss_info, 0, sizeof(bss_info));
    if (MLAN_STATUS_SUCCESS != woal_get_bss_info(priv,
                                                 MOAL_IOCTL_WAIT, &bss_info)) {
        ret = -EFAULT;
        goto setessid_ret;
    }
    memcpy(&priv->prev_ssid_bssid.ssid, &bss_info.ssid,
           sizeof(mlan_802_11_ssid));
    memcpy(&priv->prev_ssid_bssid.bssid, &bss_info.bssid, MLAN_MAC_ADDR_LENGTH);
#endif /* REASSOCIATION */

  setessid_ret:

#ifdef REASSOCIATION
    MOAL_REL_SEMAPHORE(&handle->reassoc_sem);
#endif
#ifdef ANDROID
/* titan ++ unlock connect wake_lock */
    PRINTM(MMSG, "Woal_set_essid: wake_unlock\n");
    wake_unlock(&handle->assoc_lock);
/* titan -- */
#endif    
    LEAVE();
    return ret;
}

/** 
 *  @brief Get current essid 
 *   
 *  @param dev      A pointer to net_device structure
 *  @param info     A pointer to iw_request_info structure
 *  @param dwrq     A pointer to iw_point structure
 *  @param extra    A pointer to extra data buf
 *
 *  @return         0--success, otherwise fail
 */
static int
woal_get_essid(struct net_device *dev, struct iw_request_info *info,
               struct iw_point *dwrq, char *extra)
{
    moal_private *priv = (moal_private *) netdev_priv(dev);
    mlan_bss_info bss_info;
    int ret = 0;

    ENTER();

    memset(&bss_info, 0, sizeof(bss_info));

    if (MLAN_STATUS_SUCCESS !=
        woal_get_bss_info(priv, MOAL_IOCTL_WAIT, &bss_info)) {
        ret = -EFAULT;
        goto done;
    }

    if (bss_info.media_connected) {
        dwrq->length = MIN(dwrq->length, bss_info.ssid.ssid_len);
        memcpy(extra, bss_info.ssid.ssid, dwrq->length);
    } else
        dwrq->length = 0;

    if (bss_info.scan_table_idx)
        dwrq->flags = (bss_info.scan_table_idx + 1) & IW_ENCODE_INDEX;
    else
        dwrq->flags = 1;

  done:
    LEAVE();
    return ret;
}

/**
 *  @brief  Retrieve the scan table entries via wireless tools IOCTL call
 *
 *  @param dev          A pointer to net_device structure
 *  @param info         A pointer to iw_request_info structure
 *  @param dwrq         A pointer to iw_point structure
 *  @param extra        A pointer to extra data buf
 *
 *  @return             0--success, otherwise fail
 */
int
woal_get_scan(struct net_device *dev, struct iw_request_info *info,
              struct iw_point *dwrq, char *extra)
{
    moal_private *priv = (moal_private *) netdev_priv(dev);
    int ret = 0;
    char *current_ev = extra;
    char *end_buf = extra + IW_SCAN_MAX_DATA;
    char *current_val;          /* For rates */
    struct iw_event iwe;        /* Temporary buffer */
    unsigned int i;
    unsigned int j;
    mlan_scan_resp scan_resp;
    mlan_bss_info bss_info;
    BSSDescriptor_t *scan_table;
    mlan_ds_get_signal rssi;
    t_u16 buf_size = 16 + 256 * 2;
    char *buf = NULL;
    char *ptr;
    t_u8 *praw_data;
    int beacon_size;
    t_u8 *pbeacon;
    IEEEtypes_ElementId_e element_id;
    t_u8 element_len;

    ENTER();

    if (priv->scan_pending_on_block == MTRUE)
        return -EAGAIN;

    if (!(buf = kmalloc((buf_size), GFP_ATOMIC))) {
        PRINTM(MERROR, "Cannot allocate buffer!\n");
        ret = -EFAULT;
        goto done;
    }

    memset(&bss_info, 0, sizeof(bss_info));
    if (MLAN_STATUS_SUCCESS !=
        woal_get_bss_info(priv, MOAL_IOCTL_WAIT, &bss_info)) {
        ret = -EFAULT;
        goto done;
    }
    memset(&scan_resp, 0, sizeof(scan_resp));
    if (MLAN_STATUS_SUCCESS != woal_get_scan_table(priv,
                                                   MOAL_IOCTL_WAIT,
                                                   &scan_resp)) {
        ret = -EFAULT;
        goto done;
    }
    scan_table = (BSSDescriptor_t *) scan_resp.pscan_table;
    if (dwrq->length)
        end_buf = extra + dwrq->length;
    if (priv->media_connected == MTRUE) {
        PRINTM(MINFO, "Current Ssid: %-32s\n", bss_info.ssid.ssid);
    }
    PRINTM(MINFO, "Scan: Get: NumInScanTable = %d\n",
           (int) scan_resp.num_in_scan_table);

#if WIRELESS_EXT > 13
    /* The old API using SIOCGIWAPLIST had a hard limit of IW_MAX_AP. The new
       API using SIOCGIWSCAN is only limited by buffer size WE-14 -> WE-16 the
       buffer is limited to IW_SCAN_MAX_DATA bytes which is 4096. */
    for (i = 0; i < scan_resp.num_in_scan_table; i++) {
        if ((current_ev + MAX_SCAN_CELL_SIZE) >= end_buf) {
            PRINTM(MINFO, "i=%d break out: current_ev=%p end_buf=%p "
                   "MAX_SCAN_CELL_SIZE=%d\n",
                   i, current_ev, end_buf, MAX_SCAN_CELL_SIZE);
            ret = -E2BIG;
            break;
        }
        if (!scan_table[i].freq) {
            PRINTM(MERROR, "Invalid channel number %d\n",
                   (int) scan_table[i].channel);
            continue;
        }
        PRINTM(MINFO, "i=%d  Ssid: %-32s\n", i, scan_table[i].ssid.ssid);

        /* check ssid is valid or not, ex. hidden ssid will be filter out */
        if (woal_ssid_valid(&scan_table[i].ssid) == MFALSE) {
            continue;
        }

        /* First entry *MUST* be the AP MAC address */
        iwe.cmd = SIOCGIWAP;
        iwe.u.ap_addr.sa_family = ARPHRD_ETHER;
        memcpy(iwe.u.ap_addr.sa_data, &scan_table[i].mac_address, ETH_ALEN);

        iwe.len = IW_EV_ADDR_LEN;
        current_ev =
            IWE_STREAM_ADD_EVENT(info, current_ev, end_buf, &iwe, iwe.len);

        /* Add the ESSID */
        iwe.u.data.length = scan_table[i].ssid.ssid_len;

        if (iwe.u.data.length > 32) {
            iwe.u.data.length = 32;
        }

        iwe.cmd = SIOCGIWESSID;
        iwe.u.essid.flags = (i + 1) & IW_ENCODE_INDEX;
        iwe.len = IW_EV_POINT_LEN + iwe.u.data.length;
        current_ev =
            IWE_STREAM_ADD_POINT(info, current_ev, end_buf, &iwe,
                                 (t_s8 *) scan_table[i].ssid.ssid);

        /* Add mode */
        iwe.cmd = SIOCGIWMODE;
        if (scan_table[i].bss_mode == MLAN_BSS_MODE_IBSS)
            iwe.u.mode = IW_MODE_ADHOC;
        else if (scan_table[i].bss_mode == MLAN_BSS_MODE_INFRA)
            iwe.u.mode = IW_MODE_MASTER;
        else
            iwe.u.mode = IW_MODE_AUTO;

        iwe.len = IW_EV_UINT_LEN;
        current_ev =
            IWE_STREAM_ADD_EVENT(info, current_ev, end_buf, &iwe, iwe.len);

        /* Frequency */
        iwe.cmd = SIOCGIWFREQ;
        iwe.u.freq.m = (long) scan_table[i].freq * 100000;
        iwe.u.freq.e = 1;
        iwe.len = IW_EV_FREQ_LEN;
        current_ev =
            IWE_STREAM_ADD_EVENT(info, current_ev, end_buf, &iwe, iwe.len);

        memset(&iwe, 0, sizeof(iwe));
        /* Add quality statistics */
        iwe.cmd = IWEVQUAL;
        iwe.u.qual.level = SCAN_RSSI(scan_table[i].rssi);
        if (!bss_info.bcn_nf_last) {
            iwe.u.qual.noise = MRVDRV_NF_DEFAULT_SCAN_VALUE;
        } else {
            iwe.u.qual.noise = bss_info.bcn_nf_last;
        }
        if ((bss_info.bss_mode == MLAN_BSS_MODE_IBSS) &&
            !woal_ssid_cmp(&bss_info.ssid, &scan_table[i].ssid)
            && bss_info.adhoc_state == ADHOC_STARTED) {
            memset(&rssi, 0, sizeof(mlan_ds_get_signal));
            if (MLAN_STATUS_SUCCESS !=
                woal_get_signal_info(priv, MOAL_IOCTL_WAIT, &rssi)) {
                ret = -EFAULT;
                break;
            }
            iwe.u.qual.level = rssi.data_rssi_avg;
        }
        iwe.u.qual.qual =
            woal_rssi_to_quality((t_s16) (iwe.u.qual.level - 0x100));
        iwe.len = IW_EV_QUAL_LEN;
        current_ev =
            IWE_STREAM_ADD_EVENT(info, current_ev, end_buf, &iwe, iwe.len);

        /* Add encryption capability */
        iwe.cmd = SIOCGIWENCODE;
        if (scan_table[i].privacy) {
            iwe.u.data.flags = IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
        } else {
            iwe.u.data.flags = IW_ENCODE_DISABLED;
        }
        iwe.u.data.length = 0;
        iwe.len = IW_EV_POINT_LEN + iwe.u.data.length;
        current_ev =
            IWE_STREAM_ADD_POINT(info, current_ev, end_buf, &iwe, NULL);

        current_val = current_ev + IW_EV_LCP_LEN;

        iwe.cmd = SIOCGIWRATE;

        iwe.u.bitrate.fixed = 0;
        iwe.u.bitrate.disabled = 0;
        iwe.u.bitrate.value = 0;

        /* Bit rate given in 500 kb/s units (+ 0x80) */
        for (j = 0; j < sizeof(scan_table[i].supported_rates); j++) {
            if (!scan_table[i].supported_rates[j]) {
                break;
            }

            iwe.u.bitrate.value =
                (scan_table[i].supported_rates[j] & 0x7f) * 500000;
            iwe.len = IW_EV_PARAM_LEN;
            current_val =
                IWE_STREAM_ADD_VALUE(info, current_ev, current_val, end_buf,
                                     &iwe, iwe.len);

        }
        if ((bss_info.bss_mode == MLAN_BSS_MODE_IBSS) &&
            !woal_ssid_cmp(&bss_info.ssid, &scan_table[i].ssid)
            && bss_info.adhoc_state == ADHOC_STARTED) {
            iwe.u.bitrate.value = 22 * 500000;
            iwe.len = IW_EV_PARAM_LEN;
            current_val =
                IWE_STREAM_ADD_VALUE(info, current_ev, current_val, end_buf,
                                     &iwe, iwe.len);
        }

        /* Check if an event is added */
        if ((unsigned int) (current_val - current_ev) >= IW_EV_PARAM_LEN)
            current_ev = current_val;

        /* Beacon Interval */
        memset(&iwe, 0, sizeof(iwe));
        memset(buf, 0, buf_size);
        ptr = buf;
        ptr += sprintf(ptr, "Beacon interval=%d", scan_table[i].beacon_period);

        iwe.u.data.length = strlen(buf);
        iwe.cmd = IWEVCUSTOM;
        iwe.len = IW_EV_POINT_LEN + iwe.u.data.length;
        current_ev = IWE_STREAM_ADD_POINT(info, current_ev, end_buf, &iwe, buf);
        current_val = current_ev + IW_EV_LCP_LEN + strlen(buf);

        /* Parse and send the IEs */
        pbeacon = scan_table[i].pbeacon_buf;
        beacon_size = scan_table[i].beacon_buf_size;

        /* Skip time stamp, beacon interval and capability */
        if (pbeacon) {
            pbeacon += sizeof(scan_table[i].beacon_period) +
                sizeof(scan_table[i].time_stamp) +
                sizeof(scan_table[i].cap_info);
            beacon_size -= sizeof(scan_table[i].beacon_period) +
                sizeof(scan_table[i].time_stamp) +
                sizeof(scan_table[i].cap_info);
        }

        while ((unsigned int) beacon_size >= sizeof(IEEEtypes_Header_t)) {
            element_id = (IEEEtypes_ElementId_e) (*(t_u8 *) pbeacon);
            element_len = *((t_u8 *) pbeacon + 1);
            if ((unsigned int) beacon_size <
                (unsigned int) element_len + sizeof(IEEEtypes_Header_t)) {
                PRINTM(MERROR,
                       "Get scan: Error in processing IE, "
                       "bytes left < IE length\n");
                break;
            }

            switch (element_id) {
            case VENDOR_SPECIFIC_221:
            case RSN_IE:
            case WAPI_IE:
                praw_data = (t_u8 *) pbeacon;
                memset(&iwe, 0, sizeof(iwe));
                memset(buf, 0, buf_size);
                ptr = buf;
                memcpy(buf, praw_data,
                       element_len + sizeof(IEEEtypes_Header_t));
                iwe.cmd = IWEVGENIE;
                iwe.u.data.length = element_len + sizeof(IEEEtypes_Header_t);
                iwe.len = IW_EV_POINT_LEN + iwe.u.data.length;
                current_ev =
                    IWE_STREAM_ADD_POINT(info, current_ev, end_buf, &iwe, buf);
                current_val = current_ev + IW_EV_LCP_LEN + strlen(buf);
                break;
            default:
                break;
            }
            pbeacon += element_len + sizeof(IEEEtypes_Header_t);
            beacon_size -= element_len + sizeof(IEEEtypes_Header_t);
        }

#if WIRELESS_EXT > 14
        memset(&iwe, 0, sizeof(iwe));
        memset(buf, 0, buf_size);
        ptr = buf;
        ptr += sprintf(ptr, "band=");
        memset(&iwe, 0, sizeof(iwe));
        if (scan_table[i].bss_band == BAND_A)
            ptr += sprintf(ptr, "a");
        else
            ptr += sprintf(ptr, "bg");
        iwe.u.data.length = strlen(buf);
        PRINTM(MINFO, "iwe.u.data.length %d\n", iwe.u.data.length);
        PRINTM(MINFO, "BUF: %s \n", buf);
        iwe.cmd = IWEVCUSTOM;
        iwe.len = IW_EV_POINT_LEN + iwe.u.data.length;
        current_ev = IWE_STREAM_ADD_POINT(info, current_ev, end_buf, &iwe, buf);
        current_val = current_ev + IW_EV_LCP_LEN + strlen(buf);
#endif
        current_val = current_ev + IW_EV_LCP_LEN;

        /* 
         * Check if we added any event
         */
        if ((unsigned int) (current_val - current_ev) > IW_EV_LCP_LEN)
            current_ev = current_val;
    }

    dwrq->length = (current_ev - extra);
    dwrq->flags = 0;
#endif

  done:
    if (buf)
        kfree(buf);
#ifdef ANDROID        
/* titan ++ unlock scan wake_lock */        
    PRINTM(MMSG, "woal_get_scan: wake_unlock\n");
    wake_unlock(&priv->phandle->scan_lock);        
/* titan -- */
#endif           
    LEAVE();
    return ret;
}

/**
 * iwconfig settable callbacks
 */
static const iw_handler woal_handler[] = {
    (iw_handler) woal_config_commit,    /* SIOCSIWCOMMIT */
    (iw_handler) woal_get_name, /* SIOCGIWNAME */
    (iw_handler) NULL,          /* SIOCSIWNWID */
    (iw_handler) NULL,          /* SIOCGIWNWID */
    (iw_handler) woal_set_freq, /* SIOCSIWFREQ */
    (iw_handler) woal_get_freq, /* SIOCGIWFREQ */
    (iw_handler) woal_set_bss_mode,     /* SIOCSIWMODE */
    (iw_handler) woal_get_bss_mode,     /* SIOCGIWMODE */
    (iw_handler) woal_set_sens, /* SIOCSIWSENS */
    (iw_handler) woal_get_sens, /* SIOCGIWSENS */
    (iw_handler) NULL,          /* SIOCSIWRANGE */
    (iw_handler) woal_get_range,        /* SIOCGIWRANGE */
    (iw_handler) woal_set_priv, /* SIOCSIWPRIV */
    (iw_handler) NULL,          /* SIOCGIWPRIV */
    (iw_handler) NULL,          /* SIOCSIWSTATS */
    (iw_handler) NULL,          /* SIOCGIWSTATS */
#if WIRELESS_EXT > 15
#if 1
    (iw_handler) NULL,          /* SIOCSIWSPY */
    (iw_handler) NULL,          /* SIOCGIWSPY */
    (iw_handler) NULL,          /* SIOCSIWTHRSPY */
    (iw_handler) NULL,          /* SIOCGIWTHRSPY */
#else
    iw_handler_set_spy,         /* SIOCSIWSPY */
    iw_handler_get_spy,         /* SIOCGIWSPY */
    iw_handler_set_thrspy,      /* SIOCSIWTHRSPY */
    iw_handler_get_thrspy,      /* SIOCGIWTHRSPY */
#endif
#else /* WIRELESS_EXT > 15 */
#ifdef WIRELESS_SPY
    (iw_handler) NULL,          /* SIOCSIWSPY */
    (iw_handler) NULL,          /* SIOCGIWSPY */
#else /* WIRELESS_SPY */
    (iw_handler) NULL,          /* SIOCSIWSPY */
    (iw_handler) NULL,          /* SIOCGIWSPY */
#endif /* WIRELESS_SPY */
    (iw_handler) NULL,          /* -- hole -- */
    (iw_handler) NULL,          /* -- hole -- */
#endif /* WIRELESS_EXT > 15 */
    (iw_handler) woal_set_wap,  /* SIOCSIWAP */
    (iw_handler) woal_get_wap,  /* SIOCGIWAP */
#if WIRELESS_EXT >= 18
    (iw_handler) woal_set_mlme, /* SIOCSIWMLME */
#else
    (iw_handler) NULL,          /* -- hole -- */
#endif
    /* (iw_handler) wlan_get_aplist, *//* SIOCGIWAPLIST */
    NULL,                       /* SIOCGIWAPLIST */
#if WIRELESS_EXT > 13
    (iw_handler) woal_set_scan, /* SIOCSIWSCAN */
    (iw_handler) woal_get_scan, /* SIOCGIWSCAN */
#else /* WIRELESS_EXT > 13 */
    (iw_handler) NULL,          /* SIOCSIWSCAN */
    (iw_handler) NULL,          /* SIOCGIWSCAN */
#endif /* WIRELESS_EXT > 13 */
    (iw_handler) woal_set_essid,        /* SIOCSIWESSID */
    (iw_handler) woal_get_essid,        /* SIOCGIWESSID */
    (iw_handler) woal_set_nick, /* SIOCSIWNICKN */
    (iw_handler) woal_get_nick, /* SIOCGIWNICKN */
    (iw_handler) NULL,          /* -- hole -- */
    (iw_handler) NULL,          /* -- hole -- */
    (iw_handler) woal_set_rate, /* SIOCSIWRATE */
    (iw_handler) woal_get_rate, /* SIOCGIWRATE */
    (iw_handler) woal_set_rts,  /* SIOCSIWRTS */
    (iw_handler) woal_get_rts,  /* SIOCGIWRTS */
    (iw_handler) woal_set_frag, /* SIOCSIWFRAG */
    (iw_handler) woal_get_frag, /* SIOCGIWFRAG */
    (iw_handler) woal_set_txpow,        /* SIOCSIWTXPOW */
    (iw_handler) woal_get_txpow,        /* SIOCGIWTXPOW */
    (iw_handler) woal_set_retry,        /* SIOCSIWRETRY */
    (iw_handler) woal_get_retry,        /* SIOCGIWRETRY */
    (iw_handler) woal_set_encode,       /* SIOCSIWENCODE */
    (iw_handler) woal_get_encode,       /* SIOCGIWENCODE */
    (iw_handler) woal_set_power,        /* SIOCSIWPOWER */
    (iw_handler) woal_get_power,        /* SIOCGIWPOWER */
#if (WIRELESS_EXT >= 18)
    (iw_handler) NULL,          /* -- hole -- */
    (iw_handler) NULL,          /* -- hole -- */
    (iw_handler) woal_set_gen_ie,       /* SIOCSIWGENIE */
    (iw_handler) woal_get_gen_ie,       /* SIOCGIWGENIE */
    (iw_handler) woal_set_auth, /* SIOCSIWAUTH */
    (iw_handler) woal_get_auth, /* SIOCGIWAUTH */
    (iw_handler) woal_set_encode_ext,   /* SIOCSIWENCODEEXT */
    (iw_handler) woal_get_encode_ext,   /* SIOCGIWENCODEEXT */
#endif /* WIRELESSS_EXT >= 18 */
};

/**
 * iwpriv settable callbacks
 */
static const iw_handler woal_private_handler[] = {
    NULL,                       /* SIOCIWFIRSTPRIV */
};

/********************************************************
                Global Functions
********************************************************/

/** wlan_handler_def */
struct iw_handler_def woal_handler_def = {
  num_standard:sizeof(woal_handler) / sizeof(iw_handler),
  num_private:sizeof(woal_private_handler) / sizeof(iw_handler),
  num_private_args:sizeof(woal_private_args) / sizeof(struct iw_priv_args),
  standard:(iw_handler *) woal_handler,
  private:(iw_handler *) woal_private_handler,
  private_args:(struct iw_priv_args *) woal_private_args,
#if WIRELESS_EXT > 20
  get_wireless_stats:woal_get_wireless_stats,
#endif
};

/**
 *  @brief Get wireless statistics
 *
 *  @param dev          A pointer to net_device structure
 *
 *  @return             A pointer to iw_statistics buf
 */
struct iw_statistics *
woal_get_wireless_stats(struct net_device *dev)
{
    moal_private *priv = (moal_private *) netdev_priv(dev);
    t_u16 wait_option = MOAL_NO_WAIT;

    ENTER();

    /* 
     * Since schedule() is not allowed from an atomic context
     * such as when dev_base_lock for netdevices is acquired
     * for reading/writing in kernel before this call, HostCmd
     * is issued in non-blocking way in such contexts and
     * blocking in other cases.
     */
    if (write_can_lock(&dev_base_lock)
        && (!in_atomic() || current->exit_state))
        wait_option = MOAL_WSTATS_WAIT;

    priv->w_stats.status = woal_get_mode(priv, wait_option);
    priv->w_stats.discard.retries = priv->stats.tx_errors;
    priv->w_stats.qual.qual = 0;

    /* Send RSSI command to get beacon RSSI/NF, valid only if associated */
    if (priv->media_connected == MTRUE) {
        woal_get_signal_info(priv, wait_option, NULL);
        priv->w_stats.qual.qual = woal_rssi_to_quality((t_s16)
                                                       (priv->w_stats.qual.
                                                        level - 0x100));
    }
#if WIRELESS_EXT > 18
    priv->w_stats.qual.updated |= (IW_QUAL_ALL_UPDATED | IW_QUAL_DBM);
#else
    priv->w_stats.qual.updated |= 7;
#endif
    if (!priv->w_stats.qual.noise && priv->media_connected == MTRUE)
        priv->w_stats.qual.noise = MRVDRV_NF_DEFAULT_SCAN_VALUE;

    PRINTM(MINFO, "Link Quality = %#x\n", priv->w_stats.qual.qual);
    PRINTM(MINFO, "Signal Level = %#x\n", priv->w_stats.qual.level);
    PRINTM(MINFO, "Noise = %#x\n", priv->w_stats.qual.noise);
    priv->w_stats.discard.code = 0;
    woal_get_stats_info(priv, wait_option, NULL);

    LEAVE();
    return &priv->w_stats;
}
