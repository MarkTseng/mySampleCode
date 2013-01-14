/** @file mlan_decl.h
 *
 *  @brief This file declares the generic data structures and APIs.
 *
 *  Copyright (C) 2008-2011, Marvell International Ltd. 
 *  All Rights Reserved
 */

/******************************************************
Change log:
    11/07/2008: initial version
******************************************************/

#ifndef _MLAN_DECL_H_
#define _MLAN_DECL_H_

/** MLAN release version */
#define MLAN_RELEASE_VERSION		"130.p44"

/** Re-define generic data types for MLAN/MOAL */
/** Signed char (1-byte) */
typedef char t_s8;
/** Unsigned char (1-byte) */
typedef unsigned char t_u8;
/** Signed short (2-bytes) */
typedef short t_s16;
/** Unsigned short (2-bytes) */
typedef unsigned short t_u16;
/** Signed long (4-bytes) */
typedef long t_s32;
/** Unsigned long (4-bytes) */
typedef unsigned long t_u32;
/** Signed long long 8-bytes) */
typedef long long t_s64;
/** Unsigned long long 8-bytes) */
typedef unsigned long long t_u64;
/** Void pointer (4-bytes) */
typedef void t_void;
#ifdef MLAN_64BIT
/** Pointer type (64bit) */
typedef t_u64 t_ptr;
#else
/** Pointer type (32bit) */
typedef t_u32 t_ptr;
#endif

/** Constants below */

#ifdef __GNUC__
/** Structure packing begins */
#define MLAN_PACK_START
/** Structure packeing end */
#define MLAN_PACK_END  __attribute__ ((packed))
#else /* !__GNUC__ */
#ifdef PRAGMA_PACK
/** Structure packing begins */
#define MLAN_PACK_START
/** Structure packeing end */
#define MLAN_PACK_END
#else /* !PRAGMA_PACK */
/** Structure packing begins */
#define MLAN_PACK_START   __packed
/** Structure packing end */
#define MLAN_PACK_END
#endif /* PRAGMA_PACK */
#endif /* __GNUC__ */

#ifndef INLINE
#ifdef __GNUC__
/** inline directive */
#define	INLINE	inline
#else
/** inline directive */
#define	INLINE	__inline
#endif
#endif

/** MLAN TRUE */
#define MTRUE                    (1)
/** MLAN FALSE */
#define MFALSE                   (0)

/** Macros for Data Alignment : size */
#define ALIGN_SZ(p, a)	\
	(((p) + ((a) - 1)) & ~((a) - 1))

/** Macros for Data Alignment : address */
#define ALIGN_ADDR(p, a)	\
	((((t_u32)(p)) + (((t_u32)(a)) - 1)) & ~(((t_u32)(a)) - 1))

/** Maximum BSS numbers */
#define MLAN_MAX_BSS_NUM         (2)

/** NET IP alignment */
#define MLAN_NET_IP_ALIGN        0

/** DMA alignment */
#define DMA_ALIGNMENT            64
/** max size of TxPD */
#define MAX_TXPD_SIZE            32
/** Minimum data header length */
#define MLAN_MIN_DATA_HEADER_LEN (DMA_ALIGNMENT+MAX_TXPD_SIZE)

/** rx data header length */
#define MLAN_RX_HEADER_LEN       MLAN_MIN_DATA_HEADER_LEN

/** This is current limit on Maximum Tx AMPDU allowed */
#define MLAN_MAX_TX_BASTREAM_SUPPORTED     2
/** This is current limit on Maximum Rx AMPDU allowed */
#define MLAN_MAX_RX_BASTREAM_SUPPORTED     16

/** Default Win size attached during ADDBA request */
#define MLAN_AMPDU_DEF_TXWINSIZE        32
/** Default Win size attached during ADDBA response */
#define MLAN_AMPDU_DEF_RXWINSIZE        16
/** Block ack timeout value */
#define MLAN_DEFAULT_BLOCK_ACK_TIMEOUT  0xffff
/** Maximum Tx Win size configured for ADDBA request [10 bits] */
#define MLAN_AMPDU_MAX_TXWINSIZE        0x3ff
/** Maximum Rx Win size configured for ADDBA request [10 bits] */
#define MLAN_AMPDU_MAX_RXWINSIZE        0x3ff

/** Rate index for HR/DSSS 0 */
#define MLAN_RATE_INDEX_HRDSSS0 0
/** Rate index for HR/DSSS 3 */
#define MLAN_RATE_INDEX_HRDSSS3 3
/** Rate index for OFDM 0 */
#define MLAN_RATE_INDEX_OFDM0   4
/** Rate index for OFDM 7 */
#define MLAN_RATE_INDEX_OFDM7   11
/** Rate index for MCS 0 */
#define MLAN_RATE_INDEX_MCS0    12
/** Rate index for MCS 7 */
#define MLAN_RATE_INDEX_MCS7    19
/** Rate index for MCS 32 */
#define MLAN_RATE_INDEX_MCS32   44
/** Rate index for MCS 127 */
#define MLAN_RATE_INDEX_MCS127  139

/** Rate bitmap for OFDM 0 */
#define MLAN_RATE_BITMAP_OFDM0  16
/** Rate bitmap for OFDM 7 */
#define MLAN_RATE_BITMAP_OFDM7  23
/** Rate bitmap for MCS 0 */
#define MLAN_RATE_BITMAP_MCS0   32
/** Rate bitmap for MCS 127 */
#define MLAN_RATE_BITMAP_MCS127 159

/** Size of rx data buffer */
#define MLAN_RX_DATA_BUF_SIZE     (4 * 1024)
/** Size of rx command buffer */
#define MLAN_RX_CMD_BUF_SIZE      (2 * 1024)

/** MLAN MAC Address Length */
#define MLAN_MAC_ADDR_LENGTH     (6)
/** MLAN 802.11 MAC Address */
typedef t_u8 mlan_802_11_mac_addr[MLAN_MAC_ADDR_LENGTH];

/** MLAN Maximum SSID Length */
#define MLAN_MAX_SSID_LENGTH     (32)

/** RTS/FRAG related defines */
/** Minimum RTS value */
#define MLAN_RTS_MIN_VALUE              (0)
/** Maximum RTS value */
#define MLAN_RTS_MAX_VALUE              (2347)
/** Minimum FRAG value */
#define MLAN_FRAG_MIN_VALUE             (256)
/** Maximum FRAG value */
#define MLAN_FRAG_MAX_VALUE             (2346)

/** Minimum tx retry count */
#define MLAN_TX_RETRY_MIN 		(0)
/** Maximum tx retry count */
#define MLAN_TX_RETRY_MAX 		(14)

/** define SDIO block size for firmware download */
#define MLAN_SDIO_BLOCK_SIZE_FW_DNLD	256

/** define SDIO block size for data Tx/Rx */
/* We support up to 480-byte block size due to FW buffer limitation. */
#define MLAN_SDIO_BLOCK_SIZE		256

/** define allocated buffer size */
#define ALLOC_BUF_SIZE           (4 * 1024)

/** Max retry number of CMD53 write */
#define MAX_WRITE_IOMEM_RETRY		2

/** SDIO IO Port mask */
#define MLAN_SDIO_IO_PORT_MASK		0xfffff
/** SDIO Block/Byte mode mask */
#define MLAN_SDIO_BYTE_MODE_MASK	0x80000000

/** IN parameter */
#define IN
/** OUT parameter */
#define OUT

/** BIT value */
#define MBIT(x)    (((t_u32)1) << (x))

/** Buffer flag for requeued packet */
#define MLAN_BUF_FLAG_REQUEUED_PKT      MBIT(0)
/** Buffer flag for transmit buf from moal */
#define MLAN_BUF_FLAG_MOAL_TX_BUF        MBIT(1)
/** Buffer flag for malloc mlan_buffer */
#define MLAN_BUF_FLAG_MALLOC_BUF        MBIT(2)

/** Buffer flag for bridge packet */
#define MLAN_BUF_FLAG_BRIDGE_BUF        MBIT(3)

#ifdef DEBUG_LEVEL1
/** Debug level bit definition */
#define	MMSG        MBIT(0)
#define MFATAL      MBIT(1)
#define MERROR      MBIT(2)
#define MDATA       MBIT(3)
#define MCMND       MBIT(4)
#define MEVENT      MBIT(5)
#define MINTR       MBIT(6)
#define MIOCTL      MBIT(7)

#define MDAT_D      MBIT(16)
#define MCMD_D      MBIT(17)
#define MFW_D       MBIT(18)

#define MENTRY      MBIT(28)
#define MWARN       MBIT(29)
#define MINFO       MBIT(30)
#define MHEX_DUMP   MBIT(31)
#endif /* DEBUG_LEVEL1 */

/** Memory allocation type: DMA */
#define	MLAN_MEM_DMA    MBIT(0)

/** Default memory allocation flag */
#define MLAN_MEM_DEF    0

/** mlan_status */
typedef enum _mlan_status
{
    MLAN_STATUS_FAILURE = 0xffffffff,
    MLAN_STATUS_SUCCESS = 0,
    MLAN_STATUS_PENDING,
    MLAN_STATUS_RESOURCE,
} mlan_status;

/** mlan_error_code */
typedef enum _mlan_error_code
{
    /** No error */
    MLAN_ERROR_NO_ERROR = 0,
    /** Firmware/device errors below (MSB=0) */
    MLAN_ERROR_FW_NOT_READY = 0x00000001,
    MLAN_ERROR_FW_BUSY,
    MLAN_ERROR_FW_CMDRESP,
    MLAN_ERROR_DATA_TX_FAIL,
    MLAN_ERROR_DATA_RX_FAIL,
    /** Driver errors below (MSB=1) */
    MLAN_ERROR_PKT_SIZE_INVALID = 0x80000001,
    MLAN_ERROR_PKT_TIMEOUT,
    MLAN_ERROR_PKT_INVALID,
    MLAN_ERROR_CMD_INVALID,
    MLAN_ERROR_CMD_TIMEOUT,
    MLAN_ERROR_CMD_DNLD_FAIL,
    MLAN_ERROR_CMD_CANCEL,
    MLAN_ERROR_CMD_RESP_FAIL,
    MLAN_ERROR_CMD_ASSOC_FAIL,
    MLAN_ERROR_CMD_SCAN_FAIL,
    MLAN_ERROR_IOCTL_INVALID,
    MLAN_ERROR_IOCTL_FAIL,
    MLAN_ERROR_EVENT_UNKNOWN,
    MLAN_ERROR_INVALID_PARAMETER,
    MLAN_ERROR_NO_MEM,
    /** More to add */
} mlan_error_code;

/** mlan_buf_type */
typedef enum _mlan_buf_type
{
    MLAN_BUF_TYPE_CMD = 1,
    MLAN_BUF_TYPE_DATA,
    MLAN_BUF_TYPE_EVENT,
} mlan_buf_type;

/** MLAN BSS type */
typedef enum _mlan_bss_type
{
    MLAN_BSS_TYPE_STA = 0,
    MLAN_BSS_TYPE_UAP = 1,
    MLAN_BSS_TYPE_ANY = 0xff,
} mlan_bss_type;

/** MLAN BSS role */
typedef enum _mlan_bss_role
{
    MLAN_BSS_ROLE_STA = 0,
    MLAN_BSS_ROLE_UAP = 1,
    MLAN_BSS_ROLE_ANY = 0xff,
} mlan_bss_role;

/** BSS role bit mask */
#define BSS_ROLE_BIT_MASK    MBIT(0)

/** Get BSS role */
#define GET_BSS_ROLE(priv)   ((priv)->bss_role & BSS_ROLE_BIT_MASK)

/** mlan_data_frame_type */
typedef enum _mlan_data_frame_type
{
    MLAN_DATA_FRAME_TYPE_ETH_II = 0,
    MLAN_DATA_FRAME_TYPE_802_11,
} mlan_data_frame_type;

/** mlan_event_id */
typedef enum _mlan_event_id
{
    /* Event generated by firmware (MSB=0) */
    MLAN_EVENT_ID_FW_UNKNOWN = 0x00000001,
    MLAN_EVENT_ID_FW_ADHOC_LINK_SENSED,
    MLAN_EVENT_ID_FW_ADHOC_LINK_LOST,
    MLAN_EVENT_ID_FW_DISCONNECTED,
    MLAN_EVENT_ID_FW_MIC_ERR_UNI,
    MLAN_EVENT_ID_FW_MIC_ERR_MUL,
    MLAN_EVENT_ID_FW_BCN_RSSI_LOW,
    MLAN_EVENT_ID_FW_BCN_RSSI_HIGH,
    MLAN_EVENT_ID_FW_BCN_SNR_LOW,
    MLAN_EVENT_ID_FW_BCN_SNR_HIGH,
    MLAN_EVENT_ID_FW_MAX_FAIL,
    MLAN_EVENT_ID_FW_DATA_RSSI_LOW,
    MLAN_EVENT_ID_FW_DATA_RSSI_HIGH,
    MLAN_EVENT_ID_FW_DATA_SNR_LOW,
    MLAN_EVENT_ID_FW_DATA_SNR_HIGH,
    MLAN_EVENT_ID_FW_LINK_QUALITY,
    MLAN_EVENT_ID_FW_PORT_RELEASE,
    MLAN_EVENT_ID_FW_PRE_BCN_LOST,
    MLAN_EVENT_ID_FW_WMM_CONFIG_CHANGE,
    MLAN_EVENT_ID_FW_HS_WAKEUP,
#ifdef STA_SUPPORT
    MLAN_EVENT_ID_FW_DS_AWAKE,
#endif /* STA_SUPPORT */
    MLAN_EVENT_ID_FW_BG_SCAN,
    MLAN_EVENT_ID_FW_WEP_ICV_ERR,
    MLAN_EVENT_ID_FW_STOP_TX,
    MLAN_EVENT_ID_FW_START_TX,
    MLAN_EVENT_ID_FW_CHANNEL_SWITCH_ANN,
    MLAN_EVENT_ID_FW_BW_CHANGED,
#ifdef UAP_SUPPORT
    MLAN_EVENT_ID_UAP_FW_BSS_START,
    MLAN_EVENT_ID_UAP_FW_BSS_ACTIVE,
    MLAN_EVENT_ID_UAP_FW_BSS_IDLE,
#endif

    /* Event generated by MLAN driver (MSB=1) */
    MLAN_EVENT_ID_DRV_CONNECTED = 0x80000001,
    MLAN_EVENT_ID_DRV_DEFER_HANDLING,
    MLAN_EVENT_ID_DRV_HS_ACTIVATED,
    MLAN_EVENT_ID_DRV_HS_DEACTIVATED,
#ifdef UAP_SUPPORT
    MLAN_EVENT_ID_DRV_MGMT_FRAME,
#endif
    MLAN_EVENT_ID_DRV_OBSS_SCAN_PARAM,
    MLAN_EVENT_ID_DRV_PASSTHRU,
    MLAN_EVENT_ID_DRV_SCAN_REPORT,
    MLAN_EVENT_ID_DRV_MEAS_REPORT,
    MLAN_EVENT_ID_DRV_REPORT_STRING,
} mlan_event_id;

/** Data Structures */
/** mlan_image data structure */
typedef struct _mlan_fw_image
{
    /** Helper image buffer pointer */
    t_u8 *phelper_buf;
    /** Helper image length */
    t_u32 helper_len;
    /** Firmware image buffer pointer */
    t_u8 *pfw_buf;
    /** Firmware image length */
    t_u32 fw_len;
} mlan_fw_image, *pmlan_fw_image;

/** Custom data structure */
typedef struct _mlan_init_param
{
    /** Cal data buffer pointer */
    t_u8 *pcal_data_buf;
    /** Cal data length */
    t_u32 cal_data_len;
    /** Other custom data */
} mlan_init_param, *pmlan_init_param;

/** mlan_event data structure */
typedef struct _mlan_event
{
    /** BSS index number for multiple BSS support */
    t_u32 bss_num;
    /** Event ID */
    mlan_event_id event_id;
    /** Event length */
    t_u32 event_len;
    /** Event buffer */
    t_u8 event_buf[1];
} mlan_event, *pmlan_event;

/** mlan_ioctl_req data structure */
typedef struct _mlan_ioctl_req
{
    /** Status code from firmware/driver */
    t_u32 status_code;
    /** BSS index number for multiple BSS support */
    t_u32 bss_num;
    /** Request id */
    t_u32 req_id;
    /** Action: set or get */
    t_u32 action;
    /** Pointer to buffer */
    t_u8 *pbuf;
    /** Length of buffer */
    t_u32 buf_len;
    /** Length of the data read/written in buffer */
    t_u32 data_read_written;
    /** Length of buffer needed */
    t_u32 buf_len_needed;
    /** Reserved for MOAL module */
    t_u32 reserved_1;
} mlan_ioctl_req, *pmlan_ioctl_req;

/** mlan_buffer data structure */
typedef struct _mlan_buffer
{
    /** Pointer to previous mlan_buffer */
    struct _mlan_buffer *pprev;
    /** Pointer to next mlan_buffer */
    struct _mlan_buffer *pnext;
    /** Status code from firmware/driver */
    t_u32 status_code;
    /** Flags for this buffer */
    t_u32 flags;
    /** BSS index number for multiple BSS support */
    t_u32 bss_num;
    /** Buffer descriptor, e.g. skb in Linux */
    t_void *pdesc;
    /** Pointer to buffer */
    t_u8 *pbuf;
    /** Offset to data */
    t_u32 data_offset;
    /** Data length */
    t_u32 data_len;
    /** Buffer type: data, cmd, event etc. */
    mlan_buf_type buf_type;

    /** Fields below are valid for data packet only */
    /** QoS priority */
    t_u32 priority;
    /** Time stamp when packet is received (seconds) */
    t_u32 in_ts_sec;
    /** Time stamp when packet is received (micro seconds) */
    t_u32 in_ts_usec;
    /** Time stamp when packet is processed (seconds) */
    t_u32 out_ts_sec;
    /** Time stamp when packet is processed (micro seconds) */
    t_u32 out_ts_usec;

    /** Fields below are valid for MLAN module only */
    /** Pointer to parent mlan_buffer */
    struct _mlan_buffer *pparent;
    /** Use count for this buffer */
    t_u32 use_count;
} mlan_buffer, *pmlan_buffer;

/** mlan_bss_attr data structure */
typedef struct _mlan_bss_attr
{
    /** BSS type */
    t_u32 bss_type;
    /** Data frame type: Ethernet II, 802.11, etc. */
    t_u32 frame_type;
    /** The BSS is active (non-0) or not (0). */
    t_u32 active;
    /** BSS Priority */
    t_u32 bss_priority;
    /** BSS number */
    t_u32 bss_num;
} mlan_bss_attr, *pmlan_bss_attr;

#ifdef PRAGMA_PACK
#pragma pack(push, 1)
#endif

/** Type enumeration for the command result */
typedef MLAN_PACK_START enum _mlan_cmd_result_e
{
    MLAN_CMD_RESULT_SUCCESS = 0,
    MLAN_CMD_RESULT_FAILURE = 1,
    MLAN_CMD_RESULT_TIMEOUT = 2,
    MLAN_CMD_RESULT_INVALID_DATA = 3
} MLAN_PACK_END mlan_cmd_result_e;

/** Type enumeration of WMM AC_QUEUES */
typedef MLAN_PACK_START enum _mlan_wmm_ac_e
{
    WMM_AC_BK,
    WMM_AC_BE,
    WMM_AC_VI,
    WMM_AC_VO
} MLAN_PACK_END mlan_wmm_ac_e;

/** Type enumeration for the action field in the Queue Config command */
typedef MLAN_PACK_START enum _mlan_wmm_queue_config_action_e
{
    MLAN_WMM_QUEUE_CONFIG_ACTION_GET = 0,
    MLAN_WMM_QUEUE_CONFIG_ACTION_SET = 1,
    MLAN_WMM_QUEUE_CONFIG_ACTION_DEFAULT = 2,
    MLAN_WMM_QUEUE_CONFIG_ACTION_MAX
} MLAN_PACK_END mlan_wmm_queue_config_action_e;

/** Type enumeration for the action field in the queue stats command */
typedef MLAN_PACK_START enum _mlan_wmm_queue_stats_action_e
{
    MLAN_WMM_STATS_ACTION_START = 0,
    MLAN_WMM_STATS_ACTION_STOP = 1,
    MLAN_WMM_STATS_ACTION_GET_CLR = 2,
    MLAN_WMM_STATS_ACTION_SET_CFG = 3,  /* Not currently used */
    MLAN_WMM_STATS_ACTION_GET_CFG = 4,  /* Not currently used */
    MLAN_WMM_STATS_ACTION_MAX
} MLAN_PACK_END mlan_wmm_queue_stats_action_e;

/**
 *  @brief IOCTL structure for a Traffic stream status.
 *
 */
typedef MLAN_PACK_START struct
{
    /** TSID: Range: 0->7 */
    t_u8 tid;
    /** TSID specified is valid */
    t_u8 valid;
    /** AC TSID is active on */
    t_u8 access_category;
    /** UP specified for the TSID */
    t_u8 user_priority;
    /** Power save mode for TSID: 0 (legacy), 1 (UAPSD) */
    t_u8 psb;
    /** Upstream(0), Downlink(1), Bidirectional(3) */
    t_u8 flow_dir;
    /** Medium time granted for the TSID */
    t_u16 medium_time;
} MLAN_PACK_END wlan_ioctl_wmm_ts_status_t,
/** Type definition of mlan_ds_wmm_ts_status for MLAN_OID_WMM_CFG_TS_STATUS */
  mlan_ds_wmm_ts_status, *pmlan_ds_wmm_ts_status;

/** Max Ie length */
#define MAX_IE_SIZE             256

/** custom IE */
typedef MLAN_PACK_START struct _custom_ie
{
    /** IE Index */
    t_u16 ie_index;
    /** Mgmt Subtype Mask */
    t_u16 mgmt_subtype_mask;
    /** IE Length */
    t_u16 ie_length;
    /** IE buffer */
    t_u8 ie_buffer[MAX_IE_SIZE];
} MLAN_PACK_END custom_ie;

/** Max Ie length */
#define MAX_MGMT_IE_INDEX       4

/** TLV buffer : custom IE */
typedef MLAN_PACK_START struct _tlvbuf_custom_ie
{
    /** Type */
    t_u16 type;
    /** Length */
    t_u16 len;
    /** IE data */
    custom_ie ie_data_list[MAX_MGMT_IE_INDEX];
} MLAN_PACK_END mlan_ds_misc_custom_ie;

#ifdef PRAGMA_PACK
#pragma pack(pop)
#endif

/** mlan_callbacks data structure */
typedef struct _mlan_callbacks
{
    /** moal_get_fw_data */
    mlan_status(*moal_get_fw_data) (IN t_void * pmoal_handle,
                                    IN t_u32 offset,
                                    IN t_u32 len, OUT t_u8 * pbuf);
    /** moal_init_fw_complete */
    mlan_status(*moal_init_fw_complete) (IN t_void * pmoal_handle,
                                         IN mlan_status status);
    /** moal_shutdown_fw_complete */
    mlan_status(*moal_shutdown_fw_complete) (IN t_void * pmoal_handle,
                                             IN mlan_status status);
    /** moal_send_packet_complete */
    mlan_status(*moal_send_packet_complete) (IN t_void * pmoal_handle,
                                             IN pmlan_buffer pmbuf,
                                             IN mlan_status status);
    /** moal_recv_complete */
    mlan_status(*moal_recv_complete) (IN t_void * pmoal_handle,
                                      IN pmlan_buffer pmbuf,
                                      IN t_u32 port, IN mlan_status status);
    /** moal_recv_packet */
    mlan_status(*moal_recv_packet) (IN t_void * pmoal_handle,
                                    IN pmlan_buffer pmbuf);
    /** moal_recv_event */
    mlan_status(*moal_recv_event) (IN t_void * pmoal_handle,
                                   IN pmlan_event pmevent);
    /** moal_ioctl_complete */
    mlan_status(*moal_ioctl_complete) (IN t_void * pmoal_handle,
                                       IN pmlan_ioctl_req pioctl_req,
                                       IN mlan_status status);
    /** moal_alloc_mlan_buffer */
    mlan_status(*moal_alloc_mlan_buffer) (IN t_void * pmoal_handle,
                                          IN t_u32 size,
                                          OUT pmlan_buffer * pmbuf);
    /** moal_free_mlan_buffer */
    mlan_status(*moal_free_mlan_buffer) (IN t_void * pmoal_handle,
                                         IN pmlan_buffer pmbuf);
    /** moal_write_reg */
    mlan_status(*moal_write_reg) (IN t_void * pmoal_handle,
                                  IN t_u32 reg, IN t_u32 data);
    /** moal_read_reg */
    mlan_status(*moal_read_reg) (IN t_void * pmoal_handle,
                                 IN t_u32 reg, OUT t_u32 * data);
    /** moal_udelay */
    t_void(*moal_udelay) (IN t_void * pmoal_handle, IN t_u32 udelay);
    /** moal_write_data_sync */
    mlan_status(*moal_write_data_sync) (IN t_void * pmoal_handle,
                                        IN pmlan_buffer pmbuf,
                                        IN t_u32 port, IN t_u32 timeout);
    /** moal_read_data_sync */
    mlan_status(*moal_read_data_sync) (IN t_void * pmoal_handle,
                                       IN OUT pmlan_buffer pmbuf,
                                       IN t_u32 port, IN t_u32 timeout);
    /** moal_malloc */
    mlan_status(*moal_malloc) (IN t_void * pmoal_handle,
                               IN t_u32 size, IN t_u32 flag, OUT t_u8 ** ppbuf);
    /** moal_mfree */
    mlan_status(*moal_mfree) (IN t_void * pmoal_handle, IN t_u8 * pbuf);
    /** moal_memset */
    t_void *(*moal_memset) (IN t_void * pmoal_handle,
                            IN t_void * pmem, IN t_u8 byte, IN t_u32 num);
    /** moal_memcpy */
    t_void *(*moal_memcpy) (IN t_void * pmoal_handle,
                            IN t_void * pdest,
                            IN const t_void * psrc, IN t_u32 num);
    /** moal_memmove */
    t_void *(*moal_memmove) (IN t_void * pmoal_handle,
                             IN t_void * pdest,
                             IN const t_void * psrc, IN t_u32 num);
    /** moal_memcmp */
      t_s32(*moal_memcmp) (IN t_void * pmoal_handle,
                           IN const t_void * pmem1,
                           IN const t_void * pmem2, IN t_u32 num);
    /** moal_get_system_time */
      mlan_status(*moal_get_system_time) (IN t_void * pmoal_handle,
                                          OUT t_u32 * psec, OUT t_u32 * pusec);
    /** moal_init_timer*/
      mlan_status(*moal_init_timer) (IN t_void * pmoal_handle,
                                     OUT t_void ** pptimer,
                                     IN t_void(*callback) (t_void * pcontext),
                                     IN t_void * pcontext);
    /** moal_free_timer */
      mlan_status(*moal_free_timer) (IN t_void * pmoal_handle,
                                     IN t_void * ptimer);
    /** moal_start_timer*/
      mlan_status(*moal_start_timer) (IN t_void * pmoal_handle,
                                      IN t_void * ptimer,
                                      IN t_u8 periodic, IN t_u32 msec);
    /** moal_stop_timer*/
      mlan_status(*moal_stop_timer) (IN t_void * pmoal_handle,
                                     IN t_void * ptimer);
    /** moal_init_lock */
      mlan_status(*moal_init_lock) (IN t_void * pmoal_handle,
                                    OUT t_void ** pplock);
    /** moal_free_lock */
      mlan_status(*moal_free_lock) (IN t_void * pmoal_handle,
                                    IN t_void * plock);
    /** moal_spin_lock */
      mlan_status(*moal_spin_lock) (IN t_void * pmoal_handle,
                                    IN t_void * plock);
    /** moal_spin_unlock */
      mlan_status(*moal_spin_unlock) (IN t_void * pmoal_handle,
                                      IN t_void * plock);
    /** moal_print */
      t_void(*moal_print) (IN t_void * pmoal_handle,
                           IN t_u32 level, IN t_s8 * pformat, IN ...);
    /** moal_assert */
      t_void(*moal_assert) (IN t_void * pmoal_handle, IN t_u32 cond);
} mlan_callbacks, *pmlan_callbacks;

/** Interrupt Mode SDIO */
#define INT_MODE_SDIO       0
/** Interrupt Mode GPIO */
#define INT_MODE_GPIO       1

/** Parameter unchanged, use MLAN default setting */
#define MLAN_INIT_PARA_UNCHANGED     0
/** Parameter enabled, override MLAN default setting */
#define MLAN_INIT_PARA_ENABLED       1
/** Parameter disabled, override MLAN default setting */
#define MLAN_INIT_PARA_DISABLED      2

/** mlan_device data structure */
typedef struct _mlan_device
{
    /** MOAL Handle */
    t_void *pmoal_handle;
    /** BSS Attributes */
    mlan_bss_attr bss_attr[MLAN_MAX_BSS_NUM];
    /** Callbacks */
    mlan_callbacks callbacks;
#ifdef MFG_CMD_SUPPORT
    /** MFG mode */
    t_u32 mfg_mode;
#endif
    /** SDIO interrupt mode (0: INT_MODE_SDIO, 1: INT_MODE_GPIO) */
    t_u32 int_mode;
    /** GPIO interrupt pin number */
    t_u32 gpio_pin;
#ifdef SDIO_MULTI_PORT_TX_AGGR
    /** SDIO MPA Tx */
    t_u32 mpa_tx_cfg;
#endif
#ifdef SDIO_MULTI_PORT_RX_AGGR
    /** SDIO MPA Rx */
    t_u32 mpa_rx_cfg;
#endif
    /** Auto deep sleep */
    t_u32 auto_ds;
    /** IEEE PS mode */
    t_u32 ps_mode;
    /** Max Tx buffer size */
    t_u32 max_tx_buf;
#if defined(STA_SUPPORT)
    /** 802.11d configuration */
    t_u32 cfg_11d;
#endif
    /** FW download CRC check flag */
    t_u32 fw_crc_check;
} mlan_device, *pmlan_device;

/** MLAN API function prototype */
#define MLAN_API

/** Registration */
MLAN_API mlan_status mlan_register(IN pmlan_device pmdevice,
                                   OUT t_void ** ppmlan_adapter);

/** Un-registration */
MLAN_API mlan_status mlan_unregister(IN t_void * pmlan_adapter);

/** Firmware Downloading */
MLAN_API mlan_status mlan_dnld_fw(IN t_void * pmlan_adapter,
                                  IN pmlan_fw_image pmfw);

/** Custom data pass API */
MLAN_API mlan_status mlan_set_init_param(IN t_void * pmlan_adapter,
                                         IN pmlan_init_param pparam);

/** Firmware Initialization */
MLAN_API mlan_status mlan_init_fw(IN t_void * pmlan_adapter);

/** Firmware Shutdown */
MLAN_API mlan_status mlan_shutdown_fw(IN t_void * pmlan_adapter);

/** Main Process */
MLAN_API mlan_status mlan_main_process(IN t_void * pmlan_adapter);

/** Packet Transmission */
MLAN_API mlan_status mlan_send_packet(IN t_void * pmlan_adapter,
                                      IN pmlan_buffer pmbuf);

/** Packet Reception complete callback */
MLAN_API mlan_status mlan_recv_packet_complete(IN t_void * pmlan_adapter,
                                               IN pmlan_buffer pmbuf,
                                               IN mlan_status status);

/** interrupt handler */
MLAN_API t_void mlan_interrupt(IN t_void * pmlan_adapter);

/** mlan ioctl */
MLAN_API mlan_status mlan_ioctl(IN t_void * pmlan_adapter,
                                IN pmlan_ioctl_req pioctl_req);
/** mlan select wmm queue */
MLAN_API t_u8 mlan_select_wmm_queue(IN t_void * pmlan_adapter,
                                    IN t_u8 bss_num, IN t_u8 tid);
#endif /* !_MLAN_DECL_H_ */
