/** @file mlan_ioctl.h
 *
 *  @brief This file declares the IOCTL data structures and APIs.
 *
 *  Copyright (C) 2008-2011, Marvell International Ltd. 
 *  All Rights Reserved
 */

/******************************************************
Change log:
    11/07/2008: initial version
******************************************************/

#ifndef _MLAN_IOCTL_H_
#define _MLAN_IOCTL_H_

/** mlan_802_11_ssid data structure */
typedef struct _mlan_802_11_ssid
{
    /** SSID Length */
    t_u32 ssid_len;
    /** SSID information field */
    t_u8 ssid[MLAN_MAX_SSID_LENGTH];
} mlan_802_11_ssid, *pmlan_802_11_ssid;

/** Enumeration for IOCTL request ID */
enum
{
    /* Scan Group */
    MLAN_IOCTL_SCAN = 0x00010000,
    MLAN_OID_SCAN_NORMAL,
    MLAN_OID_SCAN_SPECIFIC_SSID,
    MLAN_OID_SCAN_USER_CONFIG,
    MLAN_OID_SCAN_CONFIG,

    /* BSS Configuration Group */
    MLAN_IOCTL_BSS = 0x00020000,
    MLAN_OID_BSS_START,
    MLAN_OID_BSS_STOP,
    MLAN_OID_BSS_MODE,
    MLAN_OID_BSS_CHANNEL,
    MLAN_OID_BSS_CHANNEL_LIST,
    MLAN_OID_BSS_MAC_ADDR,
    MLAN_OID_BSS_MULTICAST_LIST,
    MLAN_OID_BSS_FIND_BSS,
    MLAN_OID_IBSS_BCN_INTERVAL,
    MLAN_OID_IBSS_ATIM_WINDOW,
    MLAN_OID_IBSS_CHANNEL,
#ifdef UAP_SUPPORT
    MLAN_OID_UAP_BSS_CONFIG,
    MLAN_OID_UAP_DEAUTH_STA,
    MLAN_OID_UAP_BSS_RESET,
#endif

    /* Radio Configuration Group */
    MLAN_IOCTL_RADIO_CFG = 0x00030000,
    MLAN_OID_RADIO_CTRL,
    MLAN_OID_BAND_CFG,
    MLAN_OID_ANT_CFG,

    /* SNMP MIB Group */
    MLAN_IOCTL_SNMP_MIB = 0x00040000,
    MLAN_OID_SNMP_MIB_RTS_THRESHOLD,
    MLAN_OID_SNMP_MIB_FRAG_THRESHOLD,
    MLAN_OID_SNMP_MIB_RETRY_COUNT,

    /* Status Information Group */
    MLAN_IOCTL_GET_INFO = 0x00050000,
    MLAN_OID_GET_STATS,
    MLAN_OID_GET_SIGNAL,
    MLAN_OID_GET_FW_INFO,
    MLAN_OID_GET_VER_EXT,
    MLAN_OID_GET_BSS_INFO,
    MLAN_OID_GET_DEBUG_INFO,
#ifdef UAP_SUPPORT
    MLAN_OID_UAP_STA_LIST,
#endif

    /* Security Configuration Group */
    MLAN_IOCTL_SEC_CFG = 0x00060000,
    MLAN_OID_SEC_CFG_AUTH_MODE,
    MLAN_OID_SEC_CFG_ENCRYPT_MODE,
    MLAN_OID_SEC_CFG_WPA_ENABLED,
    MLAN_OID_SEC_CFG_ENCRYPT_KEY,
    MLAN_OID_SEC_CFG_PASSPHRASE,
    MLAN_OID_SEC_CFG_EWPA_ENABLED,
    MLAN_OID_SEC_CFG_ESUPP_MODE,
    MLAN_OID_SEC_CFG_WAPI_ENABLED,
    MLAN_OID_SEC_CFG_PORT_CTRL_ENABLED,

    /* Rate Group */
    MLAN_IOCTL_RATE = 0x00070000,
    MLAN_OID_RATE_CFG,
    MLAN_OID_GET_DATA_RATE,
    MLAN_OID_SUPPORTED_RATES,

    /* Power Configuration Group */
    MLAN_IOCTL_POWER_CFG = 0x00080000,
    MLAN_OID_POWER_CFG,
    MLAN_OID_POWER_CFG_EXT,

    /* Power Management Configuration Group */
    MLAN_IOCTL_PM_CFG = 0x00090000,
    MLAN_OID_PM_CFG_IEEE_PS,
    MLAN_OID_PM_CFG_HS_CFG,
    MLAN_OID_PM_CFG_INACTIVITY_TO,
    MLAN_OID_PM_CFG_DEEP_SLEEP,
    MLAN_OID_PM_CFG_SLEEP_PD,
    MLAN_OID_PM_CFG_PS_CFG,
    MLAN_OID_PM_CFG_SLEEP_PARAMS,
#ifdef UAP_SUPPORT
    MLAN_OID_PM_CFG_PS_MODE,
#endif /* UAP_SUPPORT */
    MLAN_OID_PM_INFO,

    /* WMM Configuration Group */
    MLAN_IOCTL_WMM_CFG = 0x000A0000,
    MLAN_OID_WMM_CFG_ENABLE,
    MLAN_OID_WMM_CFG_QOS,
    MLAN_OID_WMM_CFG_ADDTS,
    MLAN_OID_WMM_CFG_DELTS,
    MLAN_OID_WMM_CFG_QUEUE_CONFIG,
    MLAN_OID_WMM_CFG_QUEUE_STATS,
    MLAN_OID_WMM_CFG_QUEUE_STATUS,
    MLAN_OID_WMM_CFG_TS_STATUS,

    /* WPS Configuration Group */
    MLAN_IOCTL_WPS_CFG = 0x000B0000,
    MLAN_OID_WPS_CFG_SESSION,

    /* 802.11n Configuration Group */
    MLAN_IOCTL_11N_CFG = 0x000C0000,
    MLAN_OID_11N_CFG_TX,
    MLAN_OID_11N_HTCAP_CFG,
    MLAN_OID_11N_CFG_ADDBA_REJECT,
    MLAN_OID_11N_CFG_AGGR_PRIO_TBL,
    MLAN_OID_11N_CFG_ADDBA_PARAM,
    MLAN_OID_11N_CFG_MAX_TX_BUF_SIZE,
    MLAN_OID_11N_CFG_AMSDU_AGGR_CTRL,
    MLAN_OID_11N_CFG_TX_BF_CAP,

#ifdef STA_SUPPORT
    /* 802.11d Configuration Group */
    MLAN_IOCTL_11D_CFG = 0x000D0000,
    MLAN_OID_11D_CFG_ENABLE,
    MLAN_OID_11D_CLR_CHAN_TABLE,
#endif /* STA_SUPPORT */

    /* Register Memory Access Group */
    MLAN_IOCTL_REG_MEM = 0x000E0000,
    MLAN_OID_REG_RW,
    MLAN_OID_EEPROM_RD,
    MLAN_OID_MEM_RW,

    /* Bluetooth Coalescing Configuration Group */
    MLAN_IOCTL_BCA_CFG = 0x000F0000,
    MLAN_OID_BCA_TS,

    /* 802.11h Configuration Group */
    MLAN_IOCTL_11H_CFG = 0x00110000,
    MLAN_OID_11H_CHANNEL_CHECK,
    MLAN_OID_11H_LOCAL_POWER_CONSTRAINT,

    /* Miscellaneous Configuration Group */
    MLAN_IOCTL_MISC_CFG = 0x00200000,
    MLAN_OID_MISC_GEN_IE,
    MLAN_OID_MISC_REGION,
    MLAN_OID_MISC_WARM_RESET,
#if defined(SDIO_MULTI_PORT_TX_AGGR) || defined(SDIO_MULTI_PORT_RX_AGGR)
    MLAN_OID_MISC_SDIO_MPA_CTRL,
#endif
    MLAN_OID_MISC_HOST_CMD,
    MLAN_OID_MISC_LDO,
    MLAN_OID_MISC_SYS_CLOCK,
    MLAN_OID_MISC_SOFT_RESET,
    MLAN_OID_MISC_WWS,
    MLAN_OID_MISC_VS_IE,
    MLAN_OID_MISC_INIT_SHUTDOWN,
    MLAN_OID_MISC_CUSTOM_IE,
    MLAN_OID_MISC_IP_ADDR,
    MLAN_OID_MISC_MAC_CONTROL,
};

/** Sub command size */
#define MLAN_SUB_COMMAND_SIZE	4

/** Enumeration for the action of IOCTL request */
enum
{
    MLAN_ACT_SET = 1,
    MLAN_ACT_GET,
    MLAN_ACT_CANCEL
};

/** Enumeration for generic enable/disable */
enum
{
    MLAN_ACT_DISABLE = 0,
    MLAN_ACT_ENABLE = 1
};

/** Enumeration for scan mode */
enum
{
    MLAN_SCAN_MODE_UNCHANGED = 0,
    MLAN_SCAN_MODE_BSS,
    MLAN_SCAN_MODE_IBSS,
    MLAN_SCAN_MODE_ANY
};

/** Enumeration for scan type */
enum
{
    MLAN_SCAN_TYPE_UNCHANGED = 0,
    MLAN_SCAN_TYPE_ACTIVE,
    MLAN_SCAN_TYPE_PASSIVE
};

/** RSSI scan */
#define SCAN_RSSI(RSSI)			(0x100 - ((t_u8)(RSSI)))

/** Max passive scan time for each channel in milliseconds */
#define MRVDRV_MAX_PASSIVE_SCAN_CHAN_TIME   2000

/** Max active scan time for each channel in milliseconds  */
#define MRVDRV_MAX_ACTIVE_SCAN_CHAN_TIME    500

/** Maximum number of probes to send on each channel */
#define MAX_PROBES      4

/** Default number of probes to send on each channel */
#define DEFAULT_PROBES  4

/**
 *  @brief Sub-structure passed in wlan_ioctl_get_scan_table_entry for each BSS
 *
 *  Fixed field information returned for the scan response in the IOCTL
 *    response.
 */
typedef struct _wlan_get_scan_table_fixed
{
    /** BSSID of this network */
    t_u8 bssid[MLAN_MAC_ADDR_LENGTH];
    /** Channel this beacon/probe response was detected */
    t_u8 channel;
    /** RSSI for the received packet */
    t_u8 rssi;
    /** TSF value from the firmware at packet reception */
    t_u64 network_tsf;
} wlan_get_scan_table_fixed;

/**
 *  Sructure to retrieve the scan table
 */
typedef struct
{
    /**
     *  - Zero based scan entry to start retrieval in command request
     *  - Number of scans entries returned in command response
     */
    t_u32 scan_number;
    /**
     * Buffer marker for multiple wlan_ioctl_get_scan_table_entry structures.
     *   Each struct is padded to the nearest 32 bit boundary.
     */
    t_u8 scan_table_entry_buf[1];
} wlan_ioctl_get_scan_table_info;

/**
 *  Structure passed in the wlan_ioctl_get_scan_table_info for each
 *    BSS returned in the WLAN_GET_SCAN_RESP IOCTL
 */
typedef struct _wlan_ioctl_get_scan_table_entry
{
    /**
     *  Fixed field length included in the response.
     *
     *  Length value is included so future fixed fields can be added to the
     *   response without breaking backwards compatibility.  Use the length
     *   to find the offset for the bssInfoLength field, not a sizeof() calc.
     */
    t_u32 fixed_field_length;

    /**
     *  Length of the BSS Information (probe resp or beacon) that
     *    follows after the fixed_field_length
     */
    t_u32 bss_info_length;

    /**
     *  Always present, fixed length data fields for the BSS
     */
    wlan_get_scan_table_fixed fixed_fields;

    /* 
     *  Probe response or beacon scanned for the BSS.
     *
     *  Field layout:
     *   - TSF              8 octets
     *   - Beacon Interval  2 octets
     *   - Capability Info  2 octets
     *
     *   - IEEE Infomation Elements; variable number & length per 802.11 spec
     */
    /* t_u8 bss_info_buffer[0]; */
} wlan_ioctl_get_scan_table_entry;

/** Type definition of mlan_scan_time_params */
typedef struct _mlan_scan_time_params
{
    /** Scan channel time for specific scan */
    t_u32 specific_scan_time;
    /** Scan channel time for active scan */
    t_u32 active_scan_time;
    /** Scan channel time for passive scan */
    t_u32 passive_scan_time;
} mlan_scan_time_params, *pmlan_scan_time_params;

/** Type definition of mlan_user_scan */
typedef struct _mlan_user_scan
{
    /** Length of scan_cfg_buf */
    t_u32 scan_cfg_len;
    /** Buffer of scan config */
    t_u8 scan_cfg_buf[1];
} mlan_user_scan, *pmlan_user_scan;

/** Type definition of mlan_scan_req */
typedef struct _mlan_scan_req
{
    /** BSS mode for scanning */
    t_u32 scan_mode;
    /** Scan type */
    t_u32 scan_type;
    /** SSID */
    mlan_802_11_ssid scan_ssid;
    /** Scan time parameters */
    mlan_scan_time_params scan_time;
    /** Scan config parameters in user scan */
    mlan_user_scan user_scan;
} mlan_scan_req, *pmlan_scan_req;

/** Type defnition of mlan_scan_resp */
typedef struct _mlan_scan_resp
{
    /** Number of scan result */
    t_u32 num_in_scan_table;
    /** Scan table */
    t_u8 *pscan_table;
    /* Age */
    t_u32 age_in_secs;
} mlan_scan_resp, *pmlan_scan_resp;

/** Type definition of mlan_scan_cfg */
typedef struct _mlan_scan_cfg
{
    /** Scan type */
    t_u32 scan_type;
    /** BSS mode for scanning */
    t_u32 scan_mode;
    /** Scan probe */
    t_u32 scan_probe;
    /** Scan time parameters */
    mlan_scan_time_params scan_time;
} mlan_scan_cfg, *pmlan_scan_cfg;

/** Type defnition of mlan_ds_scan for MLAN_IOCTL_SCAN */
typedef struct _mlan_ds_scan
{
    /** Sub-command */
    t_u32 sub_command;
    /** Scan request/response */
    union
    {
        /** Scan request */
        mlan_scan_req scan_req;
        /** Scan response */
        mlan_scan_resp scan_resp;
        /** Scan config parameters in user scan */
        mlan_user_scan user_scan;
        /** Scan config parameters */
        mlan_scan_cfg scan_cfg;
    } param;
} mlan_ds_scan, *pmlan_ds_scan;

/*-----------------------------------------------------------------*/
/** BSS Configuration Group */
/*-----------------------------------------------------------------*/
/** Enumeration for BSS mode */
enum
{
    MLAN_BSS_MODE_INFRA = 1,
    MLAN_BSS_MODE_IBSS,
    MLAN_BSS_MODE_AUTO
};

/** Maximum atim window */
#define MLAN_MAX_ATIM_WINDOW		50

/** Minimum beacon interval */
#define MLAN_MIN_BEACON_INTERVAL        20
/** Maximum beacon interval */
#define MLAN_MAX_BEACON_INTERVAL        1000
/** Default beacon interval */
#define MLAN_BEACON_INTERVAL            100

/** Receive all packets */
#define MLAN_PROMISC_MODE     	1
/** Receive multicast packets in multicast list */
#define MLAN_MULTICAST_MODE		2
/** Receive all multicast packets */
#define	MLAN_ALL_MULTI_MODE		4

/** Maximum size of multicast list */
#define MLAN_MAX_MULTICAST_LIST_SIZE	32

/** mlan_multicast_list data structure for MLAN_OID_BSS_MULTICAST_LIST */
typedef struct _mlan_multicast_list
{
    /** Multicast mode */
    t_u32 mode;
    /** Number of multicast addresses in the list */
    t_u32 num_multicast_addr;
    /** Multicast address list */
    mlan_802_11_mac_addr mac_list[MLAN_MAX_MULTICAST_LIST_SIZE];
} mlan_multicast_list, *pmlan_multicast_list;

/** Maximum channel number */
#define MLAN_MAX_CHANNEL_NUM	128

/** Channel/frequence for MLAN_OID_BSS_CHANNEL */
typedef struct _chan_freq
{
    /** Channel Number */
    t_u32 channel;
    /** Frequency of this Channel */
    t_u32 freq;
} chan_freq;

/** mlan_chan_list data structure for MLAN_OID_BSS_CHANNEL_LIST */
typedef struct _mlan_chan_list
{
    /** Number of channel */
    t_u32 num_of_chan;
    /** Channel-Frequency table */
    chan_freq cf[MLAN_MAX_CHANNEL_NUM];
} mlan_chan_list;

/** mlan_ssid_bssid  data structure for MLAN_OID_BSS_START and MLAN_OID_BSS_FIND_BSS */
typedef struct _mlan_ssid_bssid
{
    /** SSID */
    mlan_802_11_ssid ssid;
    /** BSSID */
    mlan_802_11_mac_addr bssid;
    /** index in BSSID list, start from 1 */
    t_u32 idx;
} mlan_ssid_bssid;

#ifdef UAP_SUPPORT
/** Maximum BEACON period */
#define MAX_BEACON_PERIOD 4000
/** Minimum BEACON period */
#define MIN_BEACON_PERIOD 50
/** Maximum DTIM period */
#define MAX_DTIM_PERIOD 100
/** Minimum DTIM period */
#define MIN_DTIM_PERIOD 1
/** Maximum TX Power Limit */
#define MAX_TX_POWER    20
/** Minimum TX Power Limit */
#define MIN_TX_POWER    0
/** MAX station count */
#define MAX_STA_COUNT   10
/** Maximum RTS threshold */
#define MAX_RTS_THRESHOLD   2347
/** Maximum fragmentation threshold */
#define MAX_FRAG_THRESHOLD 2346
/** Minimum fragmentation threshold */
#define MIN_FRAG_THRESHOLD 256
/** data rate 54 M */
#define DATA_RATE_54M   108
/** antenna A */
#define ANTENNA_MODE_A      0
/** antenna B */
#define ANTENNA_MODE_B      1
/** transmit antenna */
#define TX_ANTENNA          1
/** receive antenna */
#define RX_ANTENNA          0
/** Maximum stage out time */
#define MAX_STAGE_OUT_TIME  864000
/** Minimum stage out time */
#define MIN_STAGE_OUT_TIME  300
/** Maximum Retry Limit */
#define MAX_RETRY_LIMIT         14

/** Maximum group key timer */
#define MAX_GRP_TIMER           86400

/** Maximum value of 4 byte configuration */
#define MAX_VALID_DWORD         0x7FFFFFFF      /* (1 << 31) - 1 */

/** Band config ACS mode */
#define BAND_CONFIG_ACS_MODE    0x40
/** Band config manual */
#define BAND_CONFIG_MANUAL      0x00
/** Max scan channel number */
#define MLAN_SCAN_CHANNEL_NUM    165

/** Maximum data rates */
#define MAX_DATA_RATES          14

/** auto data rate */
#define DATA_RATE_AUTO       0

/** Max channel */
#define MAX_CHANNEL    165

/**filter mode: disable */
#define MAC_FILTER_MODE_DISABLE         0
/**filter mode: block mac address */
#define MAC_FILTER_MODE_ALLOW_MAC       1
/**filter mode: block mac address */
#define MAC_FILTER_MODE_BLOCK_MAC       2
/** Maximum mac filter num */
#define MAX_MAC_FILTER_NUM           16

/* Bitmap for protocol to use */
/** No security */
#define PROTOCOL_NO_SECURITY        0x01
/** Static WEP */
#define PROTOCOL_STATIC_WEP         0x02
/** WPA */
#define PROTOCOL_WPA                0x08
/** WPA2 */
#define PROTOCOL_WPA2               0x20
/** WP2 Mixed */
#define PROTOCOL_WPA2_MIXED         0x28
/** WAPI */
#define PROTOCOL_WAPI               0x80

/** Key_mgmt_psk */
#define KEY_MGMT_NONE   0x04
/** Key_mgmt_none */
#define KEY_MGMT_PSK    0x02

/** TKIP */
#define CIPHER_TKIP                 0x04
/** AES CCMP */
#define CIPHER_AES_CCMP             0x08

/** Valid cipher bitmap */
#define VALID_CIPHER_BITMAP         0x0c

/** scan_chan_list data structure */
typedef struct _scan_chan_list
{
    /** Number of channel */
    t_u32 num_of_chan;
    /** Channel number*/
    t_u16 chan[MLAN_SCAN_CHANNEL_NUM];
} scan_chan_list;

/** mac_filter data structure */
typedef struct _mac_filter
{
    /** mac filter mode */
    t_u16 filter_mode;
    /** mac adress count */
    t_u16 mac_count;
    /** mac address list */
    mlan_802_11_mac_addr mac_list[MAX_MAC_FILTER_NUM];
} mac_filter;

/** wpa parameter */
typedef struct _wpa_param
{
    /** Pairwise cipher WPA */
    t_u8 pairwise_cipher_wpa;
    /** Pairwise cipher WPA2 */
    t_u8 pairwise_cipher_wpa2;
    /** group cipher */
    t_u8 group_cipher;
    /** RSN replay protection */
    t_u8 rsn_protection;
    /** passphrase length */
    t_u32 length;
    /** passphrase */
    t_u8 passphrase[64];
    /**group key rekey time */
    t_u32 gk_rekey_time;
} wpa_param;

/** wep key */
typedef struct _wep_key
{
    /** key index 0-3 */
    t_u8 key_index;
    /** is default */
    t_u8 is_default;
    /** length */
    t_u16 length;
    /** key data */
    t_u8 key[26];
} wep_key;

/** wep param */
typedef struct _wep_param
{
    /** key 0 */
    wep_key key0;
    /** key 1 */
    wep_key key1;
    /** key 2 */
    wep_key key2;
    /** key 3 */
    wep_key key3;
} wep_param;

/** mlan_bss_param
 * Note: For each entry you must enter an invalid value
 * in the MOAL function woal_set_sys_config_invalid_data().
 * Otherwise for a valid data an unwanted TLV will be
 * added to that command.
 */
typedef struct _mlan_uap_bss_param
{
    /** AP mac addr */
    mlan_802_11_mac_addr mac_addr;
    /** SSID */
    mlan_802_11_ssid ssid;
    /** Broadcast ssid control */
    t_u8 bcast_ssid_ctl;
    /** Radio control: on/off */
    t_u8 radio_ctl;
    /** dtim period */
    t_u8 dtim_period;
    /** beacon period */
    t_u16 beacon_period;
    /** rates */
    t_u8 rates[MAX_DATA_RATES];
    /** Tx data rate */
    t_u16 tx_data_rate;
    /** multicast/broadcast data rate */
    t_u16 mcbc_data_rate;
    /** Tx power level */
    t_u8 tx_power_level;
    /** Tx antenna */
    t_u8 tx_antenna;
    /** Rx anteena */
    t_u8 rx_antenna;
    /** packet forward control */
    t_u8 pkt_forward_ctl;
    /** max station count */
    t_u16 max_sta_count;
    /** mac filter */
    mac_filter filter;
    /** station ageout timer in the unit of 100ms  */
    t_u32 sta_ageout_timer;
    /** PS station ageout timer in the unit of 100ms  */
    t_u32 ps_sta_ageout_timer;
    /** RTS threshold */
    t_u16 rts_threshold;
    /** fragmentation threshold */
    t_u16 frag_threshold;
    /**  retry_limit */
    t_u16 retry_limit;
    /**  pairwise update timeout */
    t_u32 pairwise_update_timeout;
    /** pairwise handshake retries */
    t_u32 pwk_retries;
    /**  groupwise update timeout */
    t_u32 groupwise_update_timeout;
    /** groupwise handshake retries */
    t_u32 gwk_retries;
    /** preamble type */
    t_u8 preamble_type;
    /** band cfg */
    t_u8 band_cfg;
    /** channel */
    t_u8 channel;
    /** auth mode */
    t_u16 auth_mode;
    /** encryption protocol */
    t_u16 protocol;
    /** key managment type */
    t_u16 key_mgmt;
    /** wep param */
    wep_param wep_cfg;
    /** wpa param */
    wpa_param wpa_cfg;
    /** Mgmt IE passthru mask */
    t_u32 mgmt_ie_passthru_mask;
    /* 
     * 11n HT Cap  HTCap_t  ht_cap
     */
    /** HT Capabilities Info field */
    t_u16 ht_cap_info;
    /** A-MPDU Parameters field */
    t_u8 ampdu_param;
    /** Supported MCS Set field */
    t_u8 supported_mcs_set[16];
    /** HT Extended Capabilities field */
    t_u16 ht_ext_cap;
    /** Transmit Beamforming Capabilities field */
    t_u32 tx_bf_cap;
    /** Antenna Selection Capability field */
    t_u8 asel;
    /** BSS status */
    t_u16 bss_status;
    /** scan channel list in ACS mode */
    scan_chan_list chan_list;
} mlan_uap_bss_param;

/** mlan_deauth_param */
typedef struct _mlan_deauth_param
{
    /** STA mac addr */
    t_u8 mac_addr[MLAN_MAC_ADDR_LENGTH];
    /** deauth reason */
    t_u16 reason_code;
} mlan_deauth_param;
#endif

/** Type definition of mlan_ds_bss for MLAN_IOCTL_BSS */
typedef struct _mlan_ds_bss
{
    /** Sub-command */
    t_u32 sub_command;
    /** BSS parameter */
    union
    {
        /** SSID-BSSID for MLAN_OID_BSS_START */
        mlan_ssid_bssid ssid_bssid;
        /** BSSID for MLAN_OID_BSS_STOP */
        mlan_802_11_mac_addr bssid;
        /** BSS mode for MLAN_OID_BSS_MODE */
        t_u32 bss_mode;
        /** BSS channel/frequency for MLAN_OID_BSS_CHANNEL */
        chan_freq bss_chan;
        /** BSS channel list for MLAN_OID_BSS_CHANNEL_LIST */
        mlan_chan_list chanlist;
        /** MAC address for MLAN_OID_BSS_MAC_ADDR */
        mlan_802_11_mac_addr mac_addr;
        /** Multicast list for MLAN_OID_BSS_MULTICAST_LIST */
        mlan_multicast_list multicast_list;
        /** Beacon interval for MLAN_OID_IBSS_BCN_INTERVAL */
        t_u32 bcn_interval;
        /** ATIM window for MLAN_OID_IBSS_ATIM_WINDOW */
        t_u32 atim_window;
#ifdef UAP_SUPPORT
        /** BSS param for AP mode */
        mlan_uap_bss_param bss_config;
        /** deauth param for MLAN_OID_UAP_DEAUTH_STA */
        mlan_deauth_param deauth_param;
#endif
    } param;
} mlan_ds_bss, *pmlan_ds_bss;

/** Enumeration for band */
enum
{
    BAND_B = 1,
    BAND_G = 2,
    BAND_A = 4,
    BAND_GN = 8,
    BAND_AN = 16,
};

/*-----------------------------------------------------------------*/
/** Radio Control Group */
/*-----------------------------------------------------------------*/
/** NO secondary channel */
#define NO_SEC_CHANNEL               0
/** secondary channel is above primary channel */
#define SEC_CHANNEL_ABOVE            1
/** secondary channel is below primary channel */
#define SEC_CHANNEL_BELOW            3

/** Type definition of mlan_ds_band_cfg for MLAN_OID_BAND_CFG */
typedef struct _mlan_ds_band_cfg
{
    /** Infra band */
    t_u32 config_bands;
    /** Ad-hoc start band */
    t_u32 adhoc_start_band;
    /** Ad-hoc start channel */
    t_u32 adhoc_channel;
    /** Ad-hoc secondary channel offset */
    t_u32 sec_chan_offset;
    /** fw supportedd band */
    t_u32 fw_bands;
} mlan_ds_band_cfg;

/** Type definition of mlan_ds_radio_cfg for MLAN_IOCTL_RADIO_CFG */
typedef struct _mlan_ds_radio_cfg
{
    /** Sub-command */
    t_u32 sub_command;
    /** Radio control parameter */
    union
    {
        /** Radio on/off for MLAN_OID_RADIO_CTRL */
        t_u32 radio_on_off;
        /** Band info for MLAN_OID_BAND_CFG */
        mlan_ds_band_cfg band_cfg;
        /** Antenna info for MLAN_OID_ANT_CFG */
        t_u32 antenna;
    } param;
} mlan_ds_radio_cfg, *pmlan_ds_radio_cfg;

/*-----------------------------------------------------------------*/
/** SNMP MIB Group */
/*-----------------------------------------------------------------*/
/** Type definition of mlan_ds_snmp_mib for MLAN_IOCTL_SNMP_MIB */
typedef struct _mlan_ds_snmp_mib
{
    /** Sub-command */
    t_u32 sub_command;
    /** SNMP MIB parameter */
    union
    {
        /** RTS threshold for MLAN_OID_SNMP_MIB_RTS_THRESHOLD */
        t_u32 rts_threshold;
        /** Fragment threshold for MLAN_OID_SNMP_MIB_FRAG_THRESHOLD */
        t_u32 frag_threshold;
        /** Retry count for MLAN_OID_SNMP_MIB_RETRY_COUNT */
        t_u32 retry_count;
    } param;
} mlan_ds_snmp_mib, *pmlan_ds_snmp_mib;

/*-----------------------------------------------------------------*/
/** Status Information Group */
/*-----------------------------------------------------------------*/
/** Enumeration for ad-hoc status */
enum
{
    ADHOC_IDLE,
    ADHOC_STARTED,
    ADHOC_JOINED,
    ADHOC_COALESCED
};

/** Type definition of mlan_ds_get_stats for MLAN_OID_GET_STATS */
typedef struct _mlan_ds_get_stats
{
    /** Statistics counter */
    /** Multicast transmitted frame count */
    t_u32 mcast_tx_frame;
    /** Failure count */
    t_u32 failed;
    /** Retry count */
    t_u32 retry;
    /** Multi entry count */
    t_u32 multi_retry;
    /** Duplicate frame count */
    t_u32 frame_dup;
    /** RTS success count */
    t_u32 rts_success;
    /** RTS failure count */
    t_u32 rts_failure;
    /** Ack failure count */
    t_u32 ack_failure;
    /** Rx fragmentation count */
    t_u32 rx_frag;
    /** Multicast Tx frame count */
    t_u32 mcast_rx_frame;
    /** FCS error count */
    t_u32 fcs_error;
    /** Tx frame count */
    t_u32 tx_frame;
    /** WEP ICV error count */
    t_u32 wep_icv_error[4];
} mlan_ds_get_stats, *pmlan_ds_get_stats;

/** Type definition of mlan_ds_uap_stats for MLAN_OID_GET_STATS */
typedef struct _mlan_ds_uap_stats
{
    /** tkip mic failures */
    t_u32 tkip_mic_failures;
    /** ccmp decrypt errors */
    t_u32 ccmp_decrypt_errors;
    /** wep undecryptable count */
    t_u32 wep_undecryptable_count;
    /** wep icv error count */
    t_u32 wep_icv_error_count;
    /** decrypt failure count */
    t_u32 decrypt_failure_count;
    /** dot11 multicast tx count */
    t_u32 mcast_tx_count;
    /** dot11 failed count */
    t_u32 failed_count;
    /** dot11 retry count */
    t_u32 retry_count;
    /** dot11 multi retry count */
    t_u32 multi_retry_count;
    /** dot11 frame duplicate count */
    t_u32 frame_dup_count;
    /** dot11 rts success count */
    t_u32 rts_success_count;
    /** dot11 rts failure count */
    t_u32 rts_failure_count;
    /** dot11 ack failure count */
    t_u32 ack_failure_count;
    /** dot11 rx ragment count */
    t_u32 rx_fragment_count;
    /** dot11 mcast rx frame count */
    t_u32 mcast_rx_frame_count;
    /** dot11 fcs error count */
    t_u32 fcs_error_count;
    /** dot11 tx frame count */
    t_u32 tx_frame_count;
    /** dot11 rsna tkip cm invoked */
    t_u32 rsna_tkip_cm_invoked;
    /** dot11 rsna 4way handshake failures */
    t_u32 rsna_4way_hshk_failures;
} mlan_ds_uap_stats, *pmlan_ds_uap_stats;

/** Mask of last beacon RSSI */
#define BCN_RSSI_LAST_MASK              0x00000001
/** Mask of average beacon RSSI */
#define BCN_RSSI_AVG_MASK               0x00000002
/** Mask of last data RSSI */
#define DATA_RSSI_LAST_MASK             0x00000004
/** Mask of average data RSSI */
#define DATA_RSSI_AVG_MASK              0x00000008
/** Mask of last beacon SNR */
#define BCN_SNR_LAST_MASK               0x00000010
/** Mask of average beacon SNR */
#define BCN_SNR_AVG_MASK                0x00000020
/** Mask of last data SNR */
#define DATA_SNR_LAST_MASK              0x00000040
/** Mask of average data SNR */
#define DATA_SNR_AVG_MASK               0x00000080
/** Mask of last beacon NF */
#define BCN_NF_LAST_MASK                0x00000100
/** Mask of average beacon NF */
#define BCN_NF_AVG_MASK                 0x00000200
/** Mask of last data NF */
#define DATA_NF_LAST_MASK               0x00000400
/** Mask of average data NF */
#define DATA_NF_AVG_MASK                0x00000800
/** Mask of all RSSI_INFO */
#define ALL_RSSI_INFO_MASK              0x00000fff

/** Type definition of mlan_ds_get_signal for MLAN_OID_GET_SIGNAL */
typedef struct _mlan_ds_get_signal
{
    /** Selector of get operation */
    /* 
     * Bit0:  Last Beacon RSSI,  Bit1:  Average Beacon RSSI,
     * Bit2:  Last Data RSSI,    Bit3:  Average Data RSSI,
     * Bit4:  Last Beacon SNR,   Bit5:  Average Beacon SNR,
     * Bit6:  Last Data SNR,     Bit7:  Average Data SNR,
     * Bit8:  Last Beacon NF,    Bit9:  Average Beacon NF,
     * Bit10: Last Data NF,      Bit11: Average Data NF
     */
    t_u16 selector;

    /** RSSI */
    /** RSSI of last beacon */
    t_s16 bcn_rssi_last;
    /** RSSI of beacon average */
    t_s16 bcn_rssi_avg;
    /** RSSI of last data packet */
    t_s16 data_rssi_last;
    /** RSSI of data packet average */
    t_s16 data_rssi_avg;

    /** SNR */
    /** SNR of last beacon */
    t_s16 bcn_snr_last;
    /** SNR of beacon average */
    t_s16 bcn_snr_avg;
    /** SNR of last data packet */
    t_s16 data_snr_last;
    /** SNR of data packet average */
    t_s16 data_snr_avg;

    /** NF */
    /** NF of last beacon */
    t_s16 bcn_nf_last;
    /** NF of beacon average */
    t_s16 bcn_nf_avg;
    /** NF of last data packet */
    t_s16 data_nf_last;
    /** NF of data packet average */
    t_s16 data_nf_avg;
} mlan_ds_get_signal, *pmlan_ds_get_signal;

/** mlan_fw_info data structure for MLAN_OID_GET_FW_INFO */
typedef struct _mlan_fw_info
{
    /** Firmware version */
    t_u32 fw_ver;
    /** MAC address */
    mlan_802_11_mac_addr mac_addr;
} mlan_fw_info, *pmlan_fw_info;

/** Version string buffer length */
#define MLAN_MAX_VER_STR_LEN    128

/** mlan_ver_ext data structure for MLAN_OID_GET_VER_EXT */
typedef struct _mlan_ver_ext
{
    /** Selected version string */
    t_u32 version_str_sel;
    /** Version string */
    char version_str[MLAN_MAX_VER_STR_LEN];
} mlan_ver_ext, *pmlan_ver_ext;

/** mlan_bss_info data structure for MLAN_OID_GET_BSS_INFO */
typedef struct _mlan_bss_info
{
    /** BSS mode */
    t_u32 bss_mode;
    /** SSID */
    mlan_802_11_ssid ssid;
    /** Table index */
    t_u32 scan_table_idx;
    /** Channel */
    t_u32 bss_chan;
    /** Region code */
    t_u32 region_code;
    /** Connection status */
    t_u32 media_connected;
    /** Radio on */
    t_u32 radio_on;
    /** Max power level */
    t_u32 max_power_level;
    /** Min power level */
    t_u32 min_power_level;
    /** Adhoc state */
    t_u32 adhoc_state;
    /** NF of last beacon */
    t_s32 bcn_nf_last;
    /** wep status */
    t_u32 wep_status;
     /** Host Sleep configured flag */
    t_u32 is_hs_configured;
#ifdef STA_SUPPORT
    /** Deep Sleep flag */
    t_u32 is_deep_sleep;
#endif                          /* STA_SUPPORT */
    /** BSSID */
    mlan_802_11_mac_addr bssid;
} mlan_bss_info, *pmlan_bss_info;

/** MAXIMUM number of TID */
#define MAX_NUM_TID     8

/** Max RX Win size */
#define MAX_RX_WINSIZE 		64

/** rx_reorder_tbl */
typedef struct
{
    /** TID */
    t_u16 tid;
    /** TA */
    t_u8 ta[MLAN_MAC_ADDR_LENGTH];
    /** Start window */
    t_u32 start_win;
    /** Window size */
    t_u32 win_size;
    /** buffer status */
    t_u32 buffer[MAX_RX_WINSIZE];
} rx_reorder_tbl;

/** tx_ba_stream_tbl */
typedef struct
{
    /** TID */
    t_u16 tid;
    /** RA */
    t_u8 ra[MLAN_MAC_ADDR_LENGTH];
} tx_ba_stream_tbl;

/** Debug command number */
#define DBG_CMD_NUM	5

/** mlan_debug_info data structure for MLAN_OID_GET_DEBUG_INFO */
typedef struct _mlan_debug_info
{
    /** Corresponds to packets_out member of mlan_private.wmm */
    t_u32 packets_out[MAX_NUM_TID];
    /** Corresponds to packets_in member of mlan_private.wmm */
    t_u32 packets_in[MAX_NUM_TID];
    /** Corresponds to pkts_queued member of mlan_private.wmm */
    t_u32 pkts_queued[MAX_NUM_TID];
    /** Corresponds to max_tx_buf_size member of mlan_adapter*/
    t_u32 max_tx_buf_size;
     /** Corresponds to tx_buf_size member of mlan_adapter*/
    t_u32 tx_buf_size;
    /** Corresponds to curr_tx_buf_size member of mlan_adapter*/
    t_u32 curr_tx_buf_size;
    /** Tx table num */
    t_u32 tx_tbl_num;
    /** Tx ba stream table */
    tx_ba_stream_tbl tx_tbl[MLAN_MAX_TX_BASTREAM_SUPPORTED];
    /** Rx table num */
    t_u32 rx_tbl_num;
    /** Rx reorder table*/
    rx_reorder_tbl rx_tbl[MLAN_MAX_RX_BASTREAM_SUPPORTED];
    /** Corresponds to ps_mode member of mlan_adapter */
    t_u16 ps_mode;
    /** Corresponds to ps_state member of mlan_adapter */
    t_u32 ps_state;
#ifdef STA_SUPPORT
    /** Corresponds to is_deep_sleep member of mlan_adapter */
    t_u8 is_deep_sleep;
#endif /** STA_SUPPORT */
    /** Corresponds to pm_wakeup_card_req member of mlan_adapter */
    t_u8 pm_wakeup_card_req;
    /** Corresponds to pm_wakeup_fw_try member of mlan_adapter */
    t_u32 pm_wakeup_fw_try;
    /** Corresponds to is_hs_configured member of mlan_adapter */
    t_u8 is_hs_configured;
    /** Corresponds to hs_activated member of mlan_adapter */
    t_u8 hs_activated;
    /** Corresponds to pps_uapsd_mode member of mlan_adapter */
    t_u16 pps_uapsd_mode;
    /** Corresponds to sleep_period.period member of mlan_adapter */
    t_u16 sleep_pd;
    /** Corresponds to wmm_qosinfo member of mlan_private */
    t_u8 qos_cfg;
    /** Corresponds to tx_lock_flag member of mlan_adapter */
    t_u8 tx_lock_flag;
    /** Corresponds to port_open member of mlan_private */
    t_u8 port_open;
    /** Corresponds to scan_processing member of mlan_adapter */
    t_u32 scan_processing;
    /** Number of host to card command failures */
    t_u32 num_cmd_host_to_card_failure;
    /** Number of host to card sleep confirm failures */
    t_u32 num_cmd_sleep_cfm_host_to_card_failure;
    /** Number of host to card Tx failures */
    t_u32 num_tx_host_to_card_failure;
    /** Number of card to host command/event failures */
    t_u32 num_cmdevt_card_to_host_failure;
    /** Number of card to host Rx failures */
    t_u32 num_rx_card_to_host_failure;
    /** Number of interrupt read failures */
    t_u32 num_int_read_failure;
    /** Last interrupt status */
    t_u32 last_int_status;
    /** Number of deauthentication events */
    t_u32 num_event_deauth;
    /** Number of disassosiation events */
    t_u32 num_event_disassoc;
    /** Number of link lost events */
    t_u32 num_event_link_lost;
    /** Number of deauthentication commands */
    t_u32 num_cmd_deauth;
    /** Number of association comamnd successes */
    t_u32 num_cmd_assoc_success;
    /** Number of association command failures */
    t_u32 num_cmd_assoc_failure;
    /** Number of Tx timeouts */
    t_u32 num_tx_timeout;
    /** Number of command timeouts */
    t_u32 num_cmd_timeout;
    /** Timeout command ID */
    t_u16 timeout_cmd_id;
    /** Timeout command action */
    t_u16 timeout_cmd_act;
    /** List of last command IDs */
    t_u16 last_cmd_id[DBG_CMD_NUM];
    /** List of last command actions */
    t_u16 last_cmd_act[DBG_CMD_NUM];
    /** Last command index */
    t_u16 last_cmd_index;
    /** List of last command response IDs */
    t_u16 last_cmd_resp_id[DBG_CMD_NUM];
    /** Last command response index */
    t_u16 last_cmd_resp_index;
    /** List of last events */
    t_u16 last_event[DBG_CMD_NUM];
    /** Last event index */
    t_u16 last_event_index;

    /** Corresponds to data_sent member of mlan_adapter */
    t_u8 data_sent;
    /** Corresponds to cmd_sent member of mlan_adapter */
    t_u8 cmd_sent;
    /** multiport tx aggr enable flag */
    t_u8 mpa_tx_enable;
    /** multiport rx aggr enable flag */
    t_u8 mpa_rx_enable;
    /** SDIO multiple port read bitmap */
    t_u16 mp_rd_bitmap;
    /** SDIO multiple port write bitmap */
    t_u16 mp_wr_bitmap;
    /** Current available port for read */
    t_u8 curr_rd_port;
    /** Current available port for write */
    t_u8 curr_wr_port;
    /** Corresponds to cmdresp_received member of mlan_adapter */
    t_u8 cmd_resp_received;
    /** Corresponds to event_received member of mlan_adapter */
    t_u8 event_received;
} mlan_debug_info, *pmlan_debug_info;

#ifdef UAP_SUPPORT
/** Maximum number of clients supported by AP */
#define MAX_NUM_CLIENTS         16

/** station info */
typedef struct _sta_info
{
    /** STA MAC address */
    t_u8 mac_address[MLAN_MAC_ADDR_LENGTH];
    /** Power mfg status */
    t_u8 power_mfg_status;
    /** RSSI */
    t_s8 rssi;
} sta_info;

/** mlan_ds_sta_list structure for MLAN_OID_UAP_STA_LIST */
typedef struct _mlan_ds_sta_list
{
    /** station count */
    t_u16 sta_count;
    /** station list */
    sta_info info[MAX_NUM_CLIENTS];
} mlan_ds_sta_list, *pmlan_ds_sta_list;
#endif

/** Type definition of mlan_ds_get_info for MLAN_IOCTL_GET_INFO */
typedef struct _mlan_ds_get_info
{
    /** Sub-command */
    t_u32 sub_command;

    /** Status information parameter */
    union
    {
        /** Signal information for MLAN_OID_GET_SIGNAL */
        mlan_ds_get_signal signal;
        /** Statistics information for MLAN_OID_GET_STATS */
        mlan_ds_get_stats stats;
        /** Firmware information for MLAN_OID_GET_FW_INFO */
        mlan_fw_info fw_info;
        /** Extended version information for MLAN_OID_GET_VER_EXT */
        mlan_ver_ext ver_ext;
        /** BSS information for MLAN_OID_GET_BSS_INFO */
        mlan_bss_info bss_info;
        /** Debug information for MLAN_OID_GET_DEBUG_INFO */
        mlan_debug_info debug_info;
#ifdef UAP_SUPPORT
        /** UAP Statistics information for MLAN_OID_GET_STATS */
        mlan_ds_uap_stats ustats;
        /** UAP station list for MLAN_OID_UAP_STA_LIST */
        mlan_ds_sta_list sta_list;
#endif
    } param;
} mlan_ds_get_info, *pmlan_ds_get_info;

/*-----------------------------------------------------------------*/
/** Security Configuration Group */
/*-----------------------------------------------------------------*/
/** Enumeration for authentication mode */
enum
{
    MLAN_AUTH_MODE_OPEN = 0x00,
    MLAN_AUTH_MODE_SHARED = 0x01,
    MLAN_AUTH_MODE_NETWORKEAP = 0x80,
    MLAN_AUTH_MODE_AUTO = 0xFF,
};

/** Enumeration for encryption mode */
enum
{
    MLAN_ENCRYPTION_MODE_NONE = 0,
    MLAN_ENCRYPTION_MODE_WEP40 = 1,
    MLAN_ENCRYPTION_MODE_TKIP = 2,
    MLAN_ENCRYPTION_MODE_CCMP = 3,
    MLAN_ENCRYPTION_MODE_WEP104 = 4,
};

/** Enumeration for PSK */
enum
{
    MLAN_PSK_PASSPHRASE = 1,
    MLAN_PSK_PMK,
    MLAN_PSK_CLEAR,
    MLAN_PSK_QUERY,
};

/** The bit to indicate the key is for unicast */
#define MLAN_KEY_INDEX_UNICAST        0x40000000
/** Maximum key length */
#define MLAN_MAX_KEY_LENGTH           32
/** Minimum passphrase length */
#define MLAN_MIN_PASSPHRASE_LENGTH    8
/** Maximum passphrase length */
#define MLAN_MAX_PASSPHRASE_LENGTH    63
/** PMK length */
#define MLAN_PMK_HEXSTR_LENGTH        64
/* A few details needed for WEP (Wireless Equivalent Privacy) */
/** 104 bits */
#define MAX_WEP_KEY_SIZE	13
/** 40 bits RC4 - WEP */
#define MIN_WEP_KEY_SIZE	5
/** packet number size */
#define PN_SIZE			16

/** Type definition of mlan_ds_encrypt_key for MLAN_OID_SEC_CFG_ENCRYPT_KEY */
typedef struct _mlan_ds_encrypt_key
{
    /** Key disable */
    t_u32 key_disable;
    /** key removed flag, when this flag is set to MTRUE, only key_index will be check */
    t_u32 key_remove;
    /** Key index */
    t_u32 key_index;
    /** Current WEP key flag */
    t_u32 is_current_wep_key;
    /** Key length */
    t_u32 key_len;
    /** Key */
    t_u8 key_material[MLAN_MAX_KEY_LENGTH];
    /** mac address */
    t_u8 mac_addr[MLAN_MAC_ADDR_LENGTH];
    /** wapi key flag */
    t_u32 is_wapi_key;
    /** Initial packet number */
    t_u8 pn[PN_SIZE];
} mlan_ds_encrypt_key, *pmlan_ds_encrypt_key;

/** Type definition of mlan_passphrase_t */
typedef struct _mlan_passphrase_t
{
    /** Length of passphrase */
    t_u32 passphrase_len;
    /** Passphrase */
    t_u8 passphrase[MLAN_MAX_PASSPHRASE_LENGTH];
} mlan_passphrase_t;

/** Type defnition of mlan_pmk_t */
typedef struct _mlan_pmk_t
{
    /** PMK */
    t_u8 pmk[MLAN_MAX_KEY_LENGTH];
} mlan_pmk_t;

/** Type definition of mlan_ds_passphrase for MLAN_OID_SEC_CFG_PASSPHRASE */
typedef struct _mlan_ds_passphrase
{
    /** SSID may be used */
    mlan_802_11_ssid ssid;
    /** BSSID may be used */
    mlan_802_11_mac_addr bssid;
    /** Flag for passphrase or pmk used */
    t_u16 psk_type;
    /** Passphrase or PMK */
    union
    {
        /** Passphrase */
        mlan_passphrase_t passphrase;
        /** PMK */
        mlan_pmk_t pmk;
    } psk;
} mlan_ds_passphrase, *pmlan_ds_passphrase;

/** Type definition of mlan_ds_esupp_mode for MLAN_OID_SEC_CFG_ESUPP_MODE */
typedef struct _mlan_ds_ewpa_mode
{
    /** RSN mode */
    t_u32 rsn_mode;
    /** Active pairwise cipher */
    t_u32 act_paircipher;
    /** Active pairwise cipher */
    t_u32 act_groupcipher;
} mlan_ds_esupp_mode, *pmlan_ds_esupp_mode;

/** Type definition of mlan_ds_sec_cfg for MLAN_IOCTL_SEC_CFG */
typedef struct _mlan_ds_sec_cfg
{
    /** Sub-command */
    t_u32 sub_command;
    /** Security configuration parameter */
    union
    {
        /** Authentication mode for MLAN_OID_SEC_CFG_AUTH_MODE */
        t_u32 auth_mode;
        /** Encryption mode for MLAN_OID_SEC_CFG_ENCRYPT_MODE */
        t_u32 encrypt_mode;
        /** WPA enabled flag for MLAN_OID_SEC_CFG_WPA_ENABLED */
        t_u32 wpa_enabled;
        /** WAPI enabled flag for MLAN_OID_SEC_CFG_WAPI_ENABLED */
        t_u32 wapi_enabled;
        /** Port Control enabled flag for MLAN_OID_SEC_CFG_PORT_CTRL */
        t_u32 port_ctrl_enabled;
        /** Encryption key for MLAN_OID_SEC_CFG_ENCRYPT_KEY */
        mlan_ds_encrypt_key encrypt_key;
        /** Passphrase for MLAN_OID_SEC_CFG_PASSPHRASE */
        mlan_ds_passphrase passphrase;
        /** Embedded supplicant WPA enabled flag for MLAN_OID_SEC_CFG_EWPA_ENABLED */
        t_u32 ewpa_enabled;
        /** Embedded supplicant mode for MLAN_OID_SEC_CFG_ESUPP_MODE */
        mlan_ds_esupp_mode esupp_mode;
    } param;
} mlan_ds_sec_cfg, *pmlan_ds_sec_cfg;

/*-----------------------------------------------------------------*/
/** Rate Configuration Group */
/*-----------------------------------------------------------------*/
/** Max number of supported rates */
#define MLAN_SUPPORTED_RATES	32

/** Enumeration for rate type */
enum
{
    MLAN_RATE_INDEX,
    MLAN_RATE_VALUE
};

/** Type definition of mlan_rate_cfg_t for MLAN_OID_RATE_CFG */
typedef struct _mlan_rate_cfg_t
{
    /** Fixed rate: 0, auto rate: 1 */
    t_u32 is_rate_auto;
    /** Rate type */
    t_u32 rate_type;
    /** Rate index or rate value if fixed rate */
    t_u32 rate;
} mlan_rate_cfg_t;

/** HT channel bandwidth */
typedef enum _mlan_ht_bw
{
    MLAN_HT_BW20,
    MLAN_HT_BW40,
} mlan_ht_bw;

/** HT guard interval */
typedef enum _mlan_ht_gi
{
    MLAN_HT_LGI,
    MLAN_HT_SGI,
} mlan_ht_gi;

/** Type definition of mlan_data_rate for MLAN_OID_GET_DATA_RATE */
typedef struct _mlan_data_rate
{
    /** Tx data rate */
    t_u32 tx_data_rate;
    /** Rx data rate */
    t_u32 rx_data_rate;

    /** Tx channel bandwidth */
    t_u32 tx_ht_bw;
    /** Tx guard interval */
    t_u32 tx_ht_gi;
    /** Rx channel bandwidth */
    t_u32 rx_ht_bw;
    /** Rx guard interval */
    t_u32 rx_ht_gi;
} mlan_data_rate;

/** Type definition of mlan_ds_rate for MLAN_IOCTL_RATE */
typedef struct _mlan_ds_rate
{
    /** Sub-command */
    t_u32 sub_command;
    /** Rate configuration parameter */
    union
    {
        /** Rate configuration for MLAN_OID_RATE_CFG */
        mlan_rate_cfg_t rate_cfg;
        /** Data rate for MLAN_OID_GET_DATA_RATE */
        mlan_data_rate data_rate;
        /** Supported rates for MLAN_OID_SUPPORTED_RATES */
        t_u8 rates[MLAN_SUPPORTED_RATES];
    } param;
} mlan_ds_rate, *pmlan_ds_rate;

/*-----------------------------------------------------------------*/
/** Power Configuration Group */
/*-----------------------------------------------------------------*/

/** Type definition of mlan_power_cfg_t for MLAN_OID_POWER_CFG */
typedef struct _mlan_power_cfg_t
{
    /** Is power auto */
    t_u32 is_power_auto;
    /** Power level */
    t_u32 power_level;
} mlan_power_cfg_t;

/** max power table size */
#define MAX_POWER_TABLE_SIZE 	128

/** The HT BW40 bit in Tx rate index */
#define TX_RATE_HT_BW40_BIT 	MBIT(7)

/** Type definition of mlan_power_cfg_ext for MLAN_OID_POWER_CFG_EXT */
typedef struct _mlan_power_cfg_ext
{
    /** Length of power_data */
    t_u32 len;
    /** Buffer of power configuration data */
    t_u32 power_data[MAX_POWER_TABLE_SIZE];
} mlan_power_cfg_ext;

/** Type definition of mlan_ds_power_cfg for MLAN_IOCTL_POWER_CFG */
typedef struct _mlan_ds_power_cfg
{
    /** Sub-command */
    t_u32 sub_command;
    /** Power configuration parameter */
    union
    {
        /** Power configuration for MLAN_OID_POWER_CFG */
        mlan_power_cfg_t power_cfg;
        /** Extended power configuration for MLAN_OID_POWER_CFG_EXT */
        mlan_power_cfg_ext power_ext;
    } param;
} mlan_ds_power_cfg, *pmlan_ds_power_cfg;

/*-----------------------------------------------------------------*/
/** Power Management Configuration Group */
/*-----------------------------------------------------------------*/
/** Host sleep config conditions : Cancel */
#define HOST_SLEEP_CFG_CANCEL   0xffffffff

/** Host sleep config conditions: Default */
#define HOST_SLEEP_DEF_COND     0xf
/** Host sleep config GPIO : Default */
#define HOST_SLEEP_DEF_GPIO     0xff
/** Host sleep config gap : Default */
#define HOST_SLEEP_DEF_GAP      200

/** Type definition of mlan_ds_hs_cfg for MLAN_OID_PM_CFG_HS_CFG */
typedef struct _mlan_ds_hs_cfg
{
    /** MTRUE to invoke the HostCmd, MFALSE otherwise */
    t_u32 is_invoke_hostcmd;
    /** Host sleep config condition */
    /** Bit0: non-unicast data
     *  Bit1: unicast data
     *  Bit2: mac events
     *  Bit3: magic packet 
     */
    t_u32 conditions;
    /** GPIO */
    t_u32 gpio;
    /** Gap in milliseconds */
    t_u32 gap;
} mlan_ds_hs_cfg, *pmlan_ds_hs_cfg;

/** Enable deep sleep mode */
#define DEEP_SLEEP_ON  1
/** Disable deep sleep mode */
#define DEEP_SLEEP_OFF 0

/** Default idle time for auto deep sleep */
#define DEEP_SLEEP_IDLE_TIME	100

typedef struct _mlan_ds_auto_ds
{
    /** auto ds mode, 0 - disable, 1 - enable */
    t_u16 auto_ds;
    /** auto ds idle time */
    t_u16 idletime;
} mlan_ds_auto_ds;

/** Type definition of mlan_ds_inactivity_to for MLAN_OID_PM_CFG_INACTIVITY_TO */
typedef struct _mlan_ds_inactivity_to
{
    /** Timeout unit in microsecond, 0 means 1000us (1ms) */
    t_u32 timeout_unit;
    /** Inactivity timeout for unicast data */
    t_u32 unicast_timeout;
    /** Inactivity timeout for multicast data */
    t_u32 mcast_timeout;
    /** Timeout for additional Rx traffic after Null PM1 packet exchange */
    t_u32 ps_entry_timeout;
} mlan_ds_inactivity_to, *pmlan_ds_inactivity_to;

/** Minimum sleep period */
#define MIN_SLEEP_PERIOD    10
/** Maximum sleep period */
#define MAX_SLEEP_PERIOD    60
/** Special setting for UPSD certification tests */
#define SLEEP_PERIOD_RESERVED_FF    0xFF

/** PS null interval disable */
#define PS_NULL_DISABLE         (-1)

/** Local listen interval disable */
#define MRVDRV_LISTEN_INTERVAL_DISABLE   (-1)
/** Minimum listen interval */
#define MRVDRV_MIN_LISTEN_INTERVAL       0

/** Minimum multiple DTIM */
#define MRVDRV_MIN_MULTIPLE_DTIM                0
/** Maximum multiple DTIM */
#define MRVDRV_MAX_MULTIPLE_DTIM                5
/** Ignore multiple DTIM */
#define MRVDRV_IGNORE_MULTIPLE_DTIM             0xfffe
/** Match listen interval to closest DTIM */
#define MRVDRV_MATCH_CLOSEST_DTIM               0xfffd

/** Minimum adhoc awake period */
#define MIN_ADHOC_AWAKE_PD      0
/** Maximum adhoc awake period */
#define MAX_ADHOC_AWAKE_PD      31
/** Special adhoc awake period */
#define SPECIAL_ADHOC_AWAKE_PD  255

/** Minimum beacon miss timeout */
#define MIN_BCN_MISS_TO         0
/** Maximum beacon miss timeout */
#define MAX_BCN_MISS_TO         50
/** Disable beacon miss timeout */
#define DISABLE_BCN_MISS_TO     65535

/** Minimum delay to PS */
#define MIN_DELAY_TO_PS         0
/** Maximum delay to PS */
#define MAX_DELAY_TO_PS         65535
/** Delay to PS unchanged */
#define DELAY_TO_PS_UNCHANGED   (-1)
/** Default delay to PS */
#define DELAY_TO_PS_DEFAULT     1000

/** PS mode: Unchanged */
#define PS_MODE_UNCHANGED       0
/** PS mode: Auto */
#define PS_MODE_AUTO            1
/** PS mode: Poll */
#define PS_MODE_POLL            2
/** PS mode: Null */
#define PS_MODE_NULL            3

/** Type definition of mlan_ds_ps_cfg for MLAN_OID_PM_CFG_PS_CFG */
typedef struct _mlan_ds_ps_cfg
{
    /** PS null interval in seconds */
    t_u32 ps_null_interval;
    /** Multiple DTIM interval */
    t_u32 multiple_dtim_interval;
    /** Listen interval */
    t_u32 listen_interval;
    /** Adhoc awake period */
    t_u32 adhoc_awake_period;
    /** Beacon miss timeout */
    t_u32 bcn_miss_timeout;
    /** Delay to PS */
    t_s32 delay_to_ps;
    /** PS mode */
    t_u32 ps_mode;
} mlan_ds_ps_cfg, *pmlan_ds_ps_cfg;

/** Type definition of mlan_ds_sleep_params for MLAN_OID_PM_CFG_SLEEP_PARAMS */
typedef struct _mlan_ds_sleep_params
{
    /** Error */
    t_u32 error;
    /** Offset */
    t_u32 offset;
    /** Stable time */
    t_u32 stable_time;
    /** Calibration control */
    t_u32 cal_control;
    /** External sleep clock */
    t_u32 ext_sleep_clk;
    /** Reserved */
    t_u32 reserved;
} mlan_ds_sleep_params, *pmlan_ds_sleep_params;

/** sleep_param */
typedef struct _ps_sleep_param
{
    /** control bitmap */
    t_u32 ctrl_bitmap;
    /** minimum sleep period (micro second) */
    t_u32 min_sleep;
    /** maximum sleep period (micro second) */
    t_u32 max_sleep;
} ps_sleep_param;

/** inactivity sleep_param */
typedef struct _inact_sleep_param
{
    /** inactivity timeout (micro second) */
    t_u32 inactivity_to;
    /** miniumu awake period (micro second) */
    t_u32 min_awake;
    /** maximum awake period (micro second) */
    t_u32 max_awake;
} inact_sleep_param;

/** flag for ps mode */
#define PS_FLAG_PS_MODE                 1
/** flag for sleep param */
#define PS_FLAG_SLEEP_PARAM             2
/** flag for inactivity sleep param */
#define PS_FLAG_INACT_SLEEP_PARAM       4

/** Disable power mode */
#define PS_MODE_DISABLE                      0
/** Enable periodic dtim ps */
#define PS_MODE_PERIODIC_DTIM                1
/** Enable inactivity ps */
#define PS_MODE_INACTIVITY                   2

/** mlan_ds_ps_mgmt */
typedef struct _mlan_ds_ps_mgmt
{
    /** flags for valid field */
    t_u16 flags;
    /** power mode */
    t_u16 ps_mode;
    /** sleep param */
    ps_sleep_param sleep_param;
    /** inactivity sleep param */
    inact_sleep_param inact_param;
} mlan_ds_ps_mgmt;

/** mlan_ds_ps_info */
typedef struct _mlan_ds_ps_info
{
    /** suspend allowed flag */
    t_u32 is_suspend_allowed;
} mlan_ds_ps_info;

/** Type definition of mlan_ds_pm_cfg for MLAN_IOCTL_PM_CFG */
typedef struct _mlan_ds_pm_cfg
{
    /** Sub-command */
    t_u32 sub_command;
    /** Power management parameter */
    union
    {
        /** Power saving mode for MLAN_OID_PM_CFG_IEEE_PS */
        t_u32 ps_mode;
        /** Host Sleep configuration for MLAN_OID_PM_CFG_HS_CFG */
        mlan_ds_hs_cfg hs_cfg;
        /** Deep sleep mode for MLAN_OID_PM_CFG_DEEP_SLEEP */
        mlan_ds_auto_ds auto_deep_sleep;
        /** Inactivity timeout for MLAN_OID_PM_CFG_INACTIVITY_TO */
        mlan_ds_inactivity_to inactivity_to;
        /** Sleep period for MLAN_OID_PM_CFG_SLEEP_PD */
        t_u32 sleep_period;
        /** PS configuration parameters for MLAN_OID_PM_CFG_PS_CFG */
        mlan_ds_ps_cfg ps_cfg;
        /** PS configuration parameters for MLAN_OID_PM_CFG_SLEEP_PARAMS */
        mlan_ds_sleep_params sleep_params;
        /** PS configuration parameters for MLAN_OID_PM_CFG_PS_MODE */
        mlan_ds_ps_mgmt ps_mgmt;
        /** power info for MLAN_OID_PM_INFO */
        mlan_ds_ps_info ps_info;
    } param;
} mlan_ds_pm_cfg, *pmlan_ds_pm_cfg;

/*-----------------------------------------------------------------*/
/** WMM Configuration Group */
/*-----------------------------------------------------------------*/

/** WMM TSpec size */
#define MLAN_WMM_TSPEC_SIZE             63
/** WMM Add TS extra IE bytes */
#define MLAN_WMM_ADDTS_EXTRA_IE_BYTES   256
/** WMM statistics for packets hist bins */
#define MLAN_WMM_STATS_PKTS_HIST_BINS   7
/** Maximum number of AC QOS queues available */
#define MLAN_WMM_MAX_AC_QUEUES          4

/**
 *  @brief IOCTL structure to send an ADDTS request and retrieve the response.
 *
 *  IOCTL structure from the application layer relayed to firmware to 
 *    instigate an ADDTS management frame with an appropriate TSPEC IE as well
 *    as any additional IEs appended in the ADDTS Action frame.
 *
 *  @sa woal_wmm_addts_req_ioctl
 */
typedef struct
{
    mlan_cmd_result_e cmd_result;     /**< Firmware execution result */

    t_u32 timeout_ms;                 /**< Timeout value in milliseconds */
    t_u8 ieee_status_code;            /**< IEEE status code */

    t_u32 ie_data_len;                /**< Length of ie block in ie_data */
    t_u8 ie_data[MLAN_WMM_TSPEC_SIZE  /**< TSPEC to send in the ADDTS */
                 + MLAN_WMM_ADDTS_EXTRA_IE_BYTES];  /**< Extra IE buf*/
} wlan_ioctl_wmm_addts_req_t;

/**
 *  @brief IOCTL structure to send a DELTS request.
 *
 *  IOCTL structure from the application layer relayed to firmware to 
 *    instigate an DELTS management frame with an appropriate TSPEC IE.
 *
 *  @sa woal_wmm_delts_req_ioctl
 */
typedef struct
{
    mlan_cmd_result_e cmd_result; /**< Firmware execution result */
    t_u8 ieee_reason_code;        /**< IEEE reason code sent, unused for WMM */
    t_u32 ie_data_len;            /**< Length of ie block in ie_data */
    t_u8 ie_data[MLAN_WMM_TSPEC_SIZE]; /**< TSPEC to send in the DELTS */
} wlan_ioctl_wmm_delts_req_t;

/**
 *  @brief IOCTL structure to configure a specific AC Queue's parameters
 *
 *  IOCTL structure from the application layer relayed to firmware to 
 *    get, set, or default the WMM AC queue parameters.
 *
 *  - msdu_lifetime_expiry is ignored if set to 0 on a set command
 *
 *  @sa woal_wmm_queue_config_ioctl
 */
typedef struct
{
    mlan_wmm_queue_config_action_e action;    /**< Set, Get, or Default */
    mlan_wmm_ac_e access_category;            /**< WMM_AC_BK(0) to WMM_AC_VO(3) */
    t_u16 msdu_lifetime_expiry;               /**< lifetime expiry in TUs */
    t_u8 supported_rates[10];                 /**< Not supported yet */
} wlan_ioctl_wmm_queue_config_t;

/**
 *  @brief IOCTL structure to start, stop, and get statistics for a WMM AC
 *
 *  IOCTL structure from the application layer relayed to firmware to
 *    start or stop statistical collection for a given AC.  Also used to
 *    retrieve and clear the collected stats on a given AC.
 *
 *  @sa woal_wmm_queue_stats_ioctl
 */
typedef struct
{
    /** Action of Queue Config : Start, Stop, or Get */
    mlan_wmm_queue_stats_action_e action;
    /** WMM Access Category: WMM_AC_BK(0) to WMM_AC_VO(3) */
    mlan_wmm_ac_e access_category;
    /** Number of successful packets transmitted */
    t_u16 pkt_count;
    /** Packets lost; not included in pkt_count */
    t_u16 pkt_loss;
    /** Average Queue delay in microseconds */
    t_u32 avg_queue_delay;
    /** Average Transmission delay in microseconds */
    t_u32 avg_tx_delay;
    /** Calculated used time in units of 32 microseconds */
    t_u16 used_time;
    /** Calculated policed time in units of 32 microseconds */
    t_u16 policed_time;
    /** Queue Delay Histogram; number of packets per queue delay range
     *
     *  [0] -  0ms <= delay < 5ms
     *  [1] -  5ms <= delay < 10ms
     *  [2] - 10ms <= delay < 20ms
     *  [3] - 20ms <= delay < 30ms
     *  [4] - 30ms <= delay < 40ms
     *  [5] - 40ms <= delay < 50ms
     *  [6] - 50ms <= delay < msduLifetime (TUs)
     */
    t_u16 delay_histogram[MLAN_WMM_STATS_PKTS_HIST_BINS];
} wlan_ioctl_wmm_queue_stats_t,
/** Type definition of mlan_ds_wmm_queue_stats for MLAN_OID_WMM_CFG_QUEUE_STATS */
  mlan_ds_wmm_queue_stats, *pmlan_ds_wmm_queue_stats;

/** 
 *  @brief IOCTL sub structure for a specific WMM AC Status
 */
typedef struct
{
    /** WMM Acm */
    t_u8 wmm_acm;
    /** Flow required flag */
    t_u8 flow_required;
    /** Flow created flag */
    t_u8 flow_created;
    /** Disabled flag */
    t_u8 disabled;
} wlan_ioctl_wmm_queue_status_ac_t;

/**
 *  @brief IOCTL structure to retrieve the WMM AC Queue status
 *
 *  IOCTL structure from the application layer to retrieve:
 *     - ACM bit setting for the AC
 *     - Firmware status (flow required, flow created, flow disabled)
 *
 *  @sa woal_wmm_queue_status_ioctl
 */
typedef struct
{
    /** WMM AC queue status */
    wlan_ioctl_wmm_queue_status_ac_t ac_status[MLAN_WMM_MAX_AC_QUEUES];
} wlan_ioctl_wmm_queue_status_t,
/** Type definition of mlan_ds_wmm_queue_status for MLAN_OID_WMM_CFG_QUEUE_STATUS */
  mlan_ds_wmm_queue_status, *pmlan_ds_wmm_queue_status;

/** Type definition of mlan_ds_wmm_addts for MLAN_OID_WMM_CFG_ADDTS */
typedef struct _mlan_ds_wmm_addts
{
    /** Result of ADDTS request */
    mlan_cmd_result_e result;
    /** Timeout value in milliseconds */
    t_u32 timeout;
    /** IEEE status code */
    t_u32 status_code;
    /** Dialog token */
    t_u8 dialog_tok;
    /** TSPEC data length */
    t_u8 ie_data_len;
    /** TSPEC to send in the ADDTS + buffering for any extra IEs */
    t_u8 ie_data[MLAN_WMM_TSPEC_SIZE + MLAN_WMM_ADDTS_EXTRA_IE_BYTES];
} mlan_ds_wmm_addts, *pmlan_ds_wmm_addts;

/** Type definition of mlan_ds_wmm_delts for MLAN_OID_WMM_CFG_DELTS */
typedef struct _mlan_ds_wmm_delts
{
    /** Result of DELTS request */
    mlan_cmd_result_e result;
    /** IEEE status code */
    t_u32 status_code;
    /** TSPEC data length */
    t_u8 ie_data_len;
    /** TSPEC to send in the DELTS */
    t_u8 ie_data[MLAN_WMM_TSPEC_SIZE];
} mlan_ds_wmm_delts, *pmlan_ds_wmm_delts;

/** Type definition of mlan_ds_wmm_queue_config for MLAN_OID_WMM_CFG_QUEUE_CONFIG */
typedef struct _mlan_ds_wmm_queue_config
{
    /** Action of Queue Config : Set, Get, or Default */
    mlan_wmm_queue_config_action_e action;
    /** WMM Access Category: WMM_AC_BK(0) to WMM_AC_VO(3) */
    mlan_wmm_ac_e access_category;
    /** Lifetime expiry in TUs */
    t_u16 msdu_lifetime_expiry;
    /** Reserve for future use */
    t_u8 reserved[10];
} mlan_ds_wmm_queue_config, *pmlan_ds_wmm_queue_config;

/** Type definition of mlan_ds_wmm_cfg for MLAN_IOCTL_WMM_CFG */
typedef struct _mlan_ds_wmm_cfg
{
    /** Sub-command */
    t_u32 sub_command;
    /** WMM configuration parameter */
    union
    {
        /** WMM enable for MLAN_OID_WMM_CFG_ENABLE */
        t_u32 wmm_enable;
        /** QoS configuration for MLAN_OID_WMM_CFG_QOS */
        t_u8 qos_cfg;
        /** WMM add TS for MLAN_OID_WMM_CFG_ADDTS */
        mlan_ds_wmm_addts addts;
        /** WMM delete TS for MLAN_OID_WMM_CFG_DELTS */
        mlan_ds_wmm_delts delts;
        /** WMM queue configuration for MLAN_OID_WMM_CFG_QUEUE_CONFIG */
        mlan_ds_wmm_queue_config q_cfg;
        /** WMM queue status for MLAN_OID_WMM_CFG_QUEUE_STATS */
        mlan_ds_wmm_queue_stats q_stats;
        /** WMM queue status for MLAN_OID_WMM_CFG_QUEUE_STATUS */
        mlan_ds_wmm_queue_status q_status;
        /** WMM TS status for MLAN_OID_WMM_CFG_TS_STATUS */
        mlan_ds_wmm_ts_status ts_status;
    } param;
} mlan_ds_wmm_cfg, *pmlan_ds_wmm_cfg;

/*-----------------------------------------------------------------*/
/** WPS Configuration Group */
/*-----------------------------------------------------------------*/
/** Enumeration for WPS session */
enum
{
    MLAN_WPS_CFG_SESSION_START = 1,
    MLAN_WPS_CFG_SESSION_END = 0
};

/** Type definition of mlan_ds_wps_cfg for MLAN_IOCTL_WPS_CFG */
typedef struct _mlan_ds_wps_cfg
{
    /** Sub-command */
    t_u32 sub_command;
    /** WPS configuration parameter */
    union
    {
        /** WPS session for MLAN_OID_WPS_CFG_SESSION */
        t_u32 wps_session;
    } param;
} mlan_ds_wps_cfg, *pmlan_ds_wps_cfg;

/*-----------------------------------------------------------------*/
/** 802.11n Configuration Group */
/*-----------------------------------------------------------------*/
/** Type definition of mlan_ds_11n_addba_param for MLAN_OID_11N_CFG_ADDBA_PARAM */
typedef struct _mlan_ds_11n_addba_param
{
    /** Timeout */
    t_u32 timeout;
    /** Buffer size for ADDBA request */
    t_u32 txwinsize;
    /** Buffer size for ADDBA response */
    t_u32 rxwinsize;
} mlan_ds_11n_addba_param, *pmlan_ds_11n_addba_param;

/** Type definition of mlan_ds_11n_tx_cfg for MLAN_OID_11N_CFG_TX */
typedef struct _mlan_ds_11n_tx_cfg
{
    /** HTTxCap */
    t_u16 httxcap;
    /** HTTxInfo */
    t_u16 httxinfo;
} mlan_ds_11n_tx_cfg, *pmlan_ds_11n_tx_cfg;

/** Type definition of mlan_ds_11n_amsdu_aggr_ctrl for 
 * MLAN_OID_11N_AMSDU_AGGR_CTRL*/
typedef struct _mlan_ds_11n_amsdu_aggr_ctrl
{
    /** Enable/Disable */
    t_u16 enable;
    /** Current AMSDU size valid */
    t_u16 curr_buf_size;
} mlan_ds_11n_amsdu_aggr_ctrl, *pmlan_ds_11n_amsdu_aggr_ctrl;

/** Type definition of mlan_ds_11n_aggr_prio_tbl for MLAN_OID_11N_CFG_AGGR_PRIO_TBL */
typedef struct _mlan_ds_11n_aggr_prio_tbl
{
    /** ampdu priority table */
    t_u8 ampdu[MAX_NUM_TID];
    /** amsdu priority table */
    t_u8 amsdu[MAX_NUM_TID];
} mlan_ds_11n_aggr_prio_tbl, *pmlan_ds_11n_aggr_prio_tbl;

/** Type definition of mlan_ds_11n_cfg for MLAN_IOCTL_11N_CFG */
typedef struct _mlan_ds_11n_cfg
{
    /** Sub-command */
    t_u32 sub_command;
    /** 802.11n configuration parameter */
    union
    {
        /** Tx param for 11n for MLAN_OID_11N_CFG_TX */
        mlan_ds_11n_tx_cfg tx_cfg;
        /** Aggr priority table for MLAN_OID_11N_CFG_AGGR_PRIO_TBL */
        mlan_ds_11n_aggr_prio_tbl aggr_prio_tbl;
        /** Add BA param for MLAN_OID_11N_CFG_ADDBA_PARAM */
        mlan_ds_11n_addba_param addba_param;
        /** Add BA Reject paramters for MLAN_OID_11N_CFG_ADDBA_REJECT */
        t_u8 addba_reject[MAX_NUM_TID];
        /** Tx buf size for MLAN_OID_11N_CFG_MAX_TX_BUF_SIZE */
        t_u32 tx_buf_size;
        /** HT cap info configuration for MLAN_OID_11N_HTCAP_CFG */
        t_u32 htcap_cfg;
        /** Tx param for 11n for MLAN_OID_11N_AMSDU_AGGR_CTRL */
        mlan_ds_11n_amsdu_aggr_ctrl amsdu_aggr_ctrl;
        /** Transmit Beamforming Capabilities field */
        t_u32 tx_bf_cap;
    } param;
} mlan_ds_11n_cfg, *pmlan_ds_11n_cfg;

#ifdef STA_SUPPORT
/*-----------------------------------------------------------------*/
/** 802.11d Configuration Group */
/*-----------------------------------------------------------------*/
/** Type definition of mlan_ds_11d_cfg for MLAN_IOCTL_11D_CFG */
typedef struct _mlan_ds_11d_cfg
{
    /** Sub-command */
    t_u32 sub_command;
    /** 802.11d configuration parameter */
    union
    {
        /** Enable for MLAN_OID_11D_CFG_ENABLE */
        t_u32 enable_11d;
    } param;
} mlan_ds_11d_cfg, *pmlan_ds_11d_cfg;
#endif /* STA_SUPPORT */

/*-----------------------------------------------------------------*/
/** Register Memory Access Group */
/*-----------------------------------------------------------------*/
/** Enumeration for register type */
enum
{
    MLAN_REG_MAC = 1,
    MLAN_REG_BBP,
    MLAN_REG_RF,
    MLAN_REG_PMIC,
    MLAN_REG_CAU,
};

/** Type definition of mlan_ds_reg_rw for MLAN_OID_REG_RW */
typedef struct _mlan_ds_reg_rw
{
    /** Register type */
    t_u32 type;
    /** Offset */
    t_u32 offset;
    /** Value */
    t_u32 value;
} mlan_ds_reg_rw;

/** Maximum EEPROM data */
#define MAX_EEPROM_DATA 256

/** Type definition of mlan_ds_read_eeprom for MLAN_OID_EEPROM_RD */
typedef struct _mlan_ds_read_eeprom
{
    /** Multiples of 4 */
    t_u16 offset;
    /** Number of bytes */
    t_u16 byte_count;
    /** Value */
    t_u8 value[MAX_EEPROM_DATA];
} mlan_ds_read_eeprom;

/** Type definition of mlan_ds_mem_rw for MLAN_OID_MEM_RW */
typedef struct _mlan_ds_mem_rw
{
    /** Address */
    t_u32 addr;
    /** Value */
    t_u32 value;
} mlan_ds_mem_rw;

/** Type definition of mlan_ds_reg_mem for MLAN_IOCTL_REG_MEM */
typedef struct _mlan_ds_reg_mem
{
    /** Sub-command */
    t_u32 sub_command;
    /** Register memory access parameter */
    union
    {
        /** Register access for MLAN_OID_REG_RW */
        mlan_ds_reg_rw reg_rw;
        /** EEPROM access for MLAN_OID_EEPROM_RD */
        mlan_ds_read_eeprom rd_eeprom;
        /** Memory access for MLAN_OID_MEM_RW */
        mlan_ds_mem_rw mem_rw;
    } param;
} mlan_ds_reg_mem, *pmlan_ds_reg_mem;

/*-----------------------------------------------------------------*/
/** BCA Configuration Group */
/*-----------------------------------------------------------------*/
/** BCA Traffic type low */
#define TRAFFIC_TYPE_LOW        0x0
/** BCA Traffic type high */
#define TRAFFIC_TYPE_HIGH       0x1
/** BCA Traffic type medium */
#define TRAFFIC_TYPE_MEDIUM     0x02
/** BCA Traffic type medium high */
#define TRAFFIC_TYPE_MEDIUMHIGH 0x03
/** BCA Reset Traffic type */
#define TRAFFIC_TYPE_RESET      0xffff
/** BCA Minimum Timeshare interval value */
#define MIN_TIMESHARE_INTERVAL  20
/** BCA Maximum Timeshare interval value */
#define MAX_TIMESHARE_INTERVAL  60000

/** Type definition of mlan_ds_bca_ts for MLAN_OID_BCA_TS */
typedef struct _mlan_ds_bca_ts
{
    /** Type: WLAN, BT */
    t_u32 traffic_type;
    /** Interval: 20msec - 60000msec */
    t_u32 timeshare_interval;
    /** PTA arbiter time in msec */
    t_u32 bt_time;
} mlan_ds_bca_ts;

/** Type definition of mlan_ds_bca_cfg for MLAN_IOCTL_BCA_CFG */
typedef struct _mlan_ds_bca_cfg
{
    /** Sub-command */
    t_u32 sub_command;
    /** BCA parameters */
    union
    {
        /** BCS time share for MLAN_OID_BCA_TS */
        mlan_ds_bca_ts bca_ts;
    } param;
} mlan_ds_bca_cfg, *pmlan_ds_bca_cfg;

/*-----------------------------------------------------------------*/
/** 802.11h Configuration Group */
/*-----------------------------------------------------------------*/
/** Type definition of mlan_ds_11h_cfg for MLAN_IOCTL_11H_CFG */
typedef struct _mlan_ds_11h_cfg
{
    /** Sub-command */
    t_u32 sub_command;
    union
    {
        /* Local power constraint for MLAN_OID_11H_LOCAL_POWER_CONSTRAINT */
        t_s8 usr_local_power_constraint;
    } param;
} mlan_ds_11h_cfg, *pmlan_ds_11h_cfg;

/*-----------------------------------------------------------------*/
/** Miscellaneous Configuration Group */
/*-----------------------------------------------------------------*/

/** CMD buffer size */
#define MLAN_SIZE_OF_CMD_BUFFER 2048

/** LDO Internal */
#define LDO_INTERNAL            0
/** LDO External */
#define LDO_EXTERNAL            1

/** Enumeration for IE type */
enum
{
    MLAN_IE_TYPE_GEN_IE = 0,
#ifdef STA_SUPPORT
    MLAN_IE_TYPE_ARP_FILTER,
#endif /* STA_SUPPORT */
};

/** Type definition of mlan_ds_misc_gen_ie for MLAN_OID_MISC_GEN_IE */
typedef struct _mlan_ds_misc_gen_ie
{
    /** IE type */
    t_u32 type;
    /** IE length */
    t_u32 len;
    /** IE buffer */
    t_u8 ie_data[MAX_IE_SIZE];
} mlan_ds_misc_gen_ie;

#if defined(SDIO_MULTI_PORT_TX_AGGR) || defined(SDIO_MULTI_PORT_RX_AGGR)
/** Type definition of mlan_ds_misc_sdio_mpa_ctrl for MLAN_OID_MISC_SDIO_MPA_CTRL */
typedef struct _mlan_ds_misc_sdio_mpa_ctrl
{
    /** SDIO MP-A TX enable/disable */
    t_u16 tx_enable;
    /** SDIO MP-A RX enable/disable */
    t_u16 rx_enable;
    /** SDIO MP-A TX buf size */
    t_u16 tx_buf_size;
    /** SDIO MP-A RX buf size */
    t_u16 rx_buf_size;
    /** SDIO MP-A TX Max Ports */
    t_u16 tx_max_ports;
    /** SDIO MP-A RX Max Ports */
    t_u16 rx_max_ports;
} mlan_ds_misc_sdio_mpa_ctrl;
#endif

/** Type definition of mlan_ds_misc_cmd for MLAN_OID_MISC_HOST_CMD */
typedef struct _mlan_ds_misc_cmd
{
    /** Command length */
    t_u32 len;
    /** Command buffer */
    t_u8 cmd[MLAN_SIZE_OF_CMD_BUFFER];
} mlan_ds_misc_cmd;

/** Maximum number of system clocks */
#define MLAN_MAX_CLK_NUM     	16

/** Clock type : Configurable */
#define MLAN_CLK_CONFIGURABLE	0
/** Clock type : Supported */
#define MLAN_CLK_SUPPORTED   	1

/** Type definition of mlan_ds_misc_sys_clock for MLAN_OID_MISC_SYS_CLOCK */
typedef struct _mlan_ds_misc_sys_clock
{
    /** Current system clock */
    t_u16 cur_sys_clk;
    /** Clock type */
    t_u16 sys_clk_type;
    /** Number of clocks */
    t_u16 sys_clk_num;
    /** System clocks */
    t_u16 sys_clk[MLAN_MAX_CLK_NUM];
} mlan_ds_misc_sys_clock;

/** Maximum length of Vendor specific IE */
#define MLAN_MAX_VSIE_LEN       (256)
/** Maximum number of Vendor specific IE */
#define MLAN_MAX_VSIE_NUM       (8)
/** VSIE bit mask for scan */
#define MLAN_VSIE_MASK_SCAN     0x01
/** VSIE bit mask for associate */
#define MLAN_VSIE_MASK_ASSOC    0x02
/** VSIE bit mask for adhoc */
#define MLAN_VSIE_MASK_ADHOC    0x04

/** Type definition of mlan_ds_misc_vs_ie for MLAN_OID_MISC_VS_IE */
typedef struct _mlan_ds_misc_vs_ie
{
    /** Position in the vendor specific IE array */
    t_u32 id;
    /** Bit 0-2: scan/assoc/ad-hoc */
    t_u32 mask;
    /** Information element */
    t_u8 ie[MLAN_MAX_VSIE_LEN];
} mlan_ds_misc_vs_ie;

/** Enumeration for function init/shutdown */
enum
{
    MLAN_FUNC_INIT = 1,
    MLAN_FUNC_SHUTDOWN,
};

/** IP address length */
#define IPADDR_LEN                  (16)
/** Max number of ip */
#define MAX_IPADDR                  (4)
/** IP address type - IPv4*/
#define IPADDR_TYPE_IPV4            (1)
/** IP operation remove */
#define MLAN_IPADDR_OP_IP_REMOVE    (0)
/** IP operation ARP filter */
#define MLAN_IPADDR_OP_ARP_FILTER   MBIT(0)
/** IP operation ARP response */
#define MLAN_IPADDR_OP_AUTO_ARP_RESP    MBIT(1)

/** Type definition of mlan_ds_misc_ipaddr_cfg for MLAN_OID_MISC_IP_ADDR */
typedef struct _mlan_ds_misc_ipaddr_cfg
{
    /** Operation code */
    t_u32 op_code;
    /** IP address type */
    t_u32 ip_addr_type;
    /** Number of IP */
    t_u32 ip_addr_num;
    /** IP address */
    t_u8 ip_addr[MAX_IPADDR][IPADDR_LEN];
} mlan_ds_misc_ipaddr_cfg;

/** Type definition of mlan_ds_misc_cfg for MLAN_IOCTL_MISC_CFG */
typedef struct _mlan_ds_misc_cfg
{
    /** Sub-command */
    t_u32 sub_command;
    /** Miscellaneous configuration parameter */
    union
    {
        /** Generic IE for MLAN_OID_MISC_GEN_IE */
        mlan_ds_misc_gen_ie gen_ie;
        /** Region code for MLAN_OID_MISC_REGION */
        t_u32 region_code;
#if defined(SDIO_MULTI_PORT_TX_AGGR) || defined(SDIO_MULTI_PORT_RX_AGGR)
        /** SDIO MP-A Ctrl command for MLAN_OID_MISC_SDIO_MPA_CTRL */
        mlan_ds_misc_sdio_mpa_ctrl mpa_ctrl;
#endif
        /** Hostcmd for MLAN_OID_MISC_HOST_CMD */
        mlan_ds_misc_cmd hostcmd;
        /** LDO configuration for MLAN_OID_MISC_LDO */
        t_u16 ldo_cfg;
        /** System clock for MLAN_OID_MISC_SYS_CLOCK */
        mlan_ds_misc_sys_clock sys_clock;
        /** WWS set/get for MLAN_OID_MISC_WWS */
        t_u32 wws_cfg;
        /** Get/add/delete VSIE for MLAN_OID_MISC_VS_IE */
        mlan_ds_misc_vs_ie vsie;
        /** Function init/shutdown for MLAN_OID_MISC_INIT_SHUTDOWN */
        t_u32 func_init_shutdown;
        /** Custom IE for MLAN_OID_MISC_CUSTOM_IE */
        mlan_ds_misc_custom_ie cust_ie;
        /** IP address configuration */
        mlan_ds_misc_ipaddr_cfg ipaddr_cfg;
        /** MAC control for MLAN_OID_MISC_MAC_CONTROL */
        t_u32 mac_ctrl;
    } param;
} mlan_ds_misc_cfg, *pmlan_ds_misc_cfg;

#endif /* !_MLAN_IOCTL_H_ */
