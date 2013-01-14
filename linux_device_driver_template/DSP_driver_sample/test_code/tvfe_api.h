/*******************************************************************************
** tvfe_api.h :  This file contains APIs which are used by Middleware Layer
**
** Copyright(c) 2010 Sunmedia Technologies - All Rights Reserved
** Author : anlzhao
**
** $Id: tvfe_api.h 321 2010-05-05 09:26:15Z b.yang $
*******************************************************************************/

#ifndef _TVFE_API_H_
#define _TVFE_API_H_

#ifdef __cplusplus
extern "C" {
#endif


#define TPI_SUM_REPORT_GROUP_NUM    4

#define TVFE_ADC_ENABLE_POLARITY_CHECK 1
#define TVFE_ADC_DISABLE_POLARITY_CHECK 0
#define TVFE_ADC_ENABLE_TABLE_PARAM 0
#define TVFE_ADC_DISABLE_TABLE_PARAM 1
#define TVFE_NO_AVAILABLE_TIMING_INDEX 0xffff

#define TVFE_AUDIO_MASTER_VOLUME_VALUE_MIN    (-672)    //-84 dB
#define TVFE_AUDIO_MASTER_VOLUME_VALUE_MAX    (192)     //+24 dB

#define DECIMAL_BASE     10//because of float issue
#define UP_SAMPLING_RATIO_1     10 // UP_SAMPLING_RATIO_1 / DECIMAL_BASE = 1
#define UP_SAMPLING_RATIO_1_5   15 // UP_SAMPLING_RATIO_1_5 / DECIMAL_BASE = 1.5
#define UP_SAMPLING_RATIO_2     20 // UP_SAMPLING_RATIO_2 / DECIMAL_BASE = 2
#define UP_SAMPLING_RATIO_4     40 // UP_SAMPLING_RATIO_4 / DECIMAL_BASE = 4

#define	DBG_MUTE_COMMAND 0x119  // a magic number to be compared with monflg

#define VIDEO_MUTE_DEBUG
#ifdef VIDEO_MUTE_DEBUG
#undef    MUTE_DEBF
#define	MUTE_DEBF(fmt, arg...)	/*if (monflg == DBG_MUTE_COMMAND)*/	diag_printf(fmt, ##arg)
#else
#define	MUTE_DEBF(fmt, arg...)
#endif
/*****************************************************************************
** Enumeration defined here
*****************************************************************************/

typedef enum
{
   /* common status values */
   TVFE_SUCCESS,	/* successful outcome */
   TVFE_ERR_INVALID_PARAM,	/* invalid parameter */
   TVFE_ERR_INVALID_OP,	/* requested operation is invalid */
   TVFE_ERR_MEMORY_ALLOC,	/* problem allocating memory */
   TVFE_ERR_BUFFER_UNDERFLOW,	/* not enough stream data is available to decode the next video frame */
   TVFE_ERR_RESOURCE_UNAVAILABLE,	/* hardware resource is in use or otherwise unavailable */
   TVFE_ERR_FAILURE,
   TVFE_WARN_NO_ACTION,	/* the function completed successfully, but no action was taken */
   TVFE_WARN_PARAM_CLIPPED,	/* the function completed successfully, though a parameter was clipped to within a valid range.*/
   TVFE_WARN_BUFFER_EMPTY,	/* buffer became empty before the requested amount of data could be read */
   TVFE_WARN_BUFFER_FULL,	/* buffer became full before the requested amount of data could be written */
} TVFE_Status_e;

/* Enumeration of source channel */
typedef enum {
	/* Currently only Channel 1 is used */
	TVFE_CHANNEL1,
	/* Channel 2 is not used, just reserved for future useage */
	TVFE_CHANNEL2,
} TVFE_SrcChannel_e;
#if 0
/* Enumeration of TVFE Resolution */
typedef enum {
	TVFE_RESOLUTION_VIDEO_MASK = 0x0300,
	TVFE_RESOLUTION_VIDEO525_NTSCM,
	TVFE_RESOLUTION_VIDEO525_NTSC443,
	TVFE_RESOLUTION_VIDEO525_PALM,
	TVFE_RESOLUTION_VIDEO525_PAL60,
	TVFE_RESOLUTION_VIDEO625_PALB,
	TVFE_RESOLUTION_VIDEO625_PALN,
	TVFE_RESOLUTION_VIDEO625_SECAM,
	TVFE_RESOLUTION_VIDEO625_BW,
	TVFE_RESOLUTION_VIDEO525_BW,
	TVFE_RESOLUTION_SNOW525,
	TVFE_RESOLUTION_SNOW625,
	TVFE_RESOLUTION_VIDEO525_720x240P,
	TVFE_RESOLUTION_VIDEO625_720x288P,
	TVFE_RESOLUTION_AVL_SIGNAL = 0x600,
	TVFE_RESOLUTION_NON_STD_SIGNAL = 0x601,
	TVFE_RESOLUTION_NO_SIGNAL = 0x900,
	TVFE_RESOLUTION_UNKNOW_FORMAT = 0xa00,
} TVFE_Resoultion_e;
#endif
typedef enum {
	TVFE_ADC_004_YPBPR_SW_AUTO_COLOR_16_235  = 1 << 0,
	TVFE_ADC_004_YPBPR_SW_AUTO_COLOR_00_235  = 1 << 1,
	TVFE_ADC_004_YPBPR_SW_AUTO_COLOR_00_255  = 1 << 2,
	TVFE_ADC_006_AUTO_COLOR_BY_MAX_MIN       = 1 << 3,
	TVFE_ADC_006_AUTO_COLOR_BY_PIXEL_GRAB    = 1 << 4,
	TVFE_ADC_007_NON_STD_MODE_DETECT         = 1 << 5,
	TVFE_ADC_009_AUTO_SOG                    = 1 << 6,
	TVFE_ADC_013_PC_DSS                      = 1 << 7,
	TVFE_ADC_013_PC_AUTO_SYNC_ENABLE         = 1 << 8,
	TVFE_ADC_013_PC_AUTO_SYNC_DSS_DCS_ACS    = 1 << 9,
	TVFE_ADC_015_AUTO_COLOR_PATTERN_SEARCH   = 1 << 10,
	TVFE_ADC_020_YPBPR_AUTO_PHASE            = 1 << 11,
	TVFE_ADC_021_SUPPORT_AUTO_COLOR_ONE_CH   = 1 << 12,
} TVFE_AdcFuncConf_e;

/* Enumeration of PC auto tuning command */
typedef enum {
	TVFE_AUTO_TUNE_PHASE = 0,
	TVFE_AUTO_TUNE_CLOCK,
	TVFE_AUTO_TUNE_POSITION,
	TVFE_AUTO_TUNE_DONE,
	TVFE_AUTO_TUNE_ALL,
} TVFE_PCAutoTuneCommand_e;

/* Enumeration of PC auto tuning status */
typedef enum {
	TVFE_AUTO_TUNE_RUNNING = 0,
	TVFE_AUTO_TUNE_COMPLETE,
	TVFE_AUTO_TUNE_NOT_PC,
} TVFE_PCAutoTuneStatus_e;

/* Enumeration of TVFE mpeg aspect ratio */
typedef enum {
	TVFE_MPEG_ASPECT_RATIO_SQUARE,
	TVFE_MPEG_ASPECT_RATIO_4_3,
	TVFE_MPEG_ASPECT_RATIO_16_9,
} TVFE_MpegAspectRatio_e;

/* Enumeration of TVFE mpeg frame ratio */
typedef enum {
	TVFE_MPEG_FRAME_RATIO_23,
	TVFE_MPEG_FRAME_RATIO_24,
	TVFE_MPEG_FRAME_RATIO_29,
	TVFE_MPEG_FRAME_RATIO_30,
	TVFE_MPEG_FRAME_RATIO_59,
	TVFE_MPEG_FRAME_RATIO_60,
} TVFE_MpegFrameRatio_e;

/* Enumeration of HDMI hot plug status */
typedef enum {
	TVFE_HDMIHPDLow,
	TVFE_HDMIHPDHigh,
} TVFE_HDMIHPD_e;

/* TV decoding format type */
typedef enum {
	TVFE_TvDecFORMAT_NTSC = 0,
	TVFE_TvDecFORMAT_PAL,
	TVFE_TvDecFORMAT_PAL_M,
	TVFE_TvDecFORMAT_PAL_N,
	TVFE_TvDecFORMAT_SECAM,
	TVFE_TvDecFORMAT_NTSC_443,
	TVFE_TvDecFORMAT_PAL_60,
	TVFE_TvDecFORMAT_BW_625,
	TVFE_TvDecFORMAT_BW_525,
	TVFE_TvDecFORMAT_UNKNOW,
} TVFE_TvDecFormatType_e;

/* Enumeration of TVFE tv decoder auto detect mask value */
typedef enum {
	TVFE_TVDEC_AUTO_DETECT_PALM		= 0x01, /* 0x01 << 0x00 */
	TVFE_TVDEC_AUTO_DETECT_PALCN	= 0x02, /* 0x01 << 0x01 */
	TVFE_TVDEC_AUTO_DETECT_SECAM	= 0x04, /* 0x01 << 0x02 */
	TVFE_TVDEC_AUTO_DETECT_NTSC443	= 0x08, /* 0x01 << 0x03 */
	TVFE_TVDEC_AUTO_DETECT_PAL60	= 0x10, /* 0x01 << 0x04 */
	TVFE_TVDEC_AUTO_DETECT_NTSC		= 0x20, /* 0x01 << 0x05 */
	TVFE_TVDEC_AUTO_DETECT_PALI		= 0x40, /* 0x01 << 0x06 */
	TVFE_TVDEC_AUTO_DETECT_ALL		= 0x80, /* 0x01 << 0x07 */
} TVFE_TvDecAutoDetectMask_e;

/* Enumeration of PC auto color pattern, and just used by facroty menu */
typedef enum {
	TVFE_ADC_AUTO_COLOR_BAR = 0,
	TVFE_ADC_AUTO_GRAY_BAR,
} TVFE_ADCAutoColorPattern_e;

/* Enumeration of TVFE events */
typedef enum {
	TVFE_EVENT_CH1_FSM_INITIAL_COMPLETE,
	/*
	** This event will sent one time when a available signal become a
	** unavailable signal
	*/
	TVFE_EVENT_CH1_NO_SIGNAL,
	TVFE_EVENT_CH1_DEBOUNCING_SIGNAL,
	/*
	** This event will sent one time when a new available signal is found
	*/
	TVFE_EVENT_CH1_AVAILABLE_SIGNAL,
	TVFE_EVENT_CH1_FSM_STOP,
	TVFE_EVENT_CH1_PC_AUTO_TUNE_COMPLETE,
	TVFE_EVENT_CH1_PC_AUTO_TUNE,
	TVFE_EVENT_CH1_HDCP_PROTECT,
	TVFE_EVENT_CH1_HDCP_UNPROTECT,
	TVFE_EVENT_CH1_SCART_RGB,
	TVFE_EVENT_CH1_SCART_CVBS,
	TVFE_EVENT_CH1_UNSUPPORT_MODE,
	TVFE_EVENT_CH1_HDMI_HDMI_MODE,
	TVFE_EVENT_CH1_HDMI_DVI_MODE,
	TVFE_EVENT_MAX,
} TVFE_Event_e;

/* Scart input type */
typedef enum
{
	TVFE_TvDec_SCART_CVBS = 0,
	TVFE_TvDec_SCART_RGB,
	TVFE_TvDec_SCART_MIX,
	TVFE_TvDec_SCART_SV,
} TVFE_TvDecScartInputType_e;

typedef enum
{
	TVFE_TvDec_SCART_AutoMode= 0,
	TVFE_TvDec_SCART_Force_RGB,
	TVFE_TvDec_SCART_Force_CVBS,
	TVFE_TvDec_SCART_Force_SV
} TVFE_TvDecForceScartType_t;

/*!
 * @brief   DRV_TvDecDetectAudioM() use for report Audio M or NTSC443 colorburst
 */
typedef enum
{
	TVFE_TvDec_AudioM = 0,
   	TVFE_TvDec_Colorburst
} TVFE_TvDecBurstType_t;

typedef enum
{
	TVFE_SIGNAL_UNKNOWN = 0,
	TVFE_SIGNAL_LOCK,
	TVFE_SIGNAL_UNLOCK,
} TVFE_SignalLock_e;

/* Enumeration of video timing info type */
typedef enum {
	TVFE_TVDEC_TIMING,
	TVFE_HDTV_TIMING,
	TVFE_CUSTOMER_HDTV_TIMING,
	TVFE_HDMI_VIDEO_TIMING,
	TVFE_CUSTOMER_HDMI_VIDEO_TIMING,
	TVFE_HDMI_PC_TIMING,
	TVFE_CUSTOMER_HDMI_PC_TIMING,
	TVFE_PC_TIMING,
	TVFE_CUSTOMER_PC_TIMING,
	TVFE_MPEG_TIMING,
	TVFE_TIMING_MAX,
} TVFE_VideoTiming_e;

/* Enumeration of FSM states */
typedef enum {
	FSM_VFE_INITIAL,
	FSM_VFE_NO_SIGNAL,
	FSM_VFE_NO_SIGNAL_CHECK_STATUS,
	FSM_VFE_SIGNAL_IN_DEBOUNCE,
	FSM_VFE_SIGNAL_OUT_DEBOUNCE,
	FSM_VFE_SIGNAL_STABLE_UPDATE_RESOLUTION,
	FSM_VFE_SIGNAL_STABLE,
	FSM_VFE_NO_SIGNAL_INFORM_VOM,
	FSM_VFE_NOT_NO_SIGNAL,
	FSM_VFE_SIGNAL_UNSUPPORT,
	FSM_VFE_VAFE_AUTOTUNE,
	FSM_VFE_VAFE_HW_AUTO_OFFSET,
	FSM_VFE_VAFE_HW_AUTO_GAIN,
	FSM_VFE_VAFE_HW_AUTOCALIBRATION,
	FSM_VFE_VAFE_MODE_DETECT,
	FSM_VFE_VAFE_AUTOCOLOR_OFFSET_ADJUST,
	FSM_VFE_TVDEC_TUNER_FIND_NEW_SIGNAL,
	FSM_VFE_TVDEC_TUNER_FIND_NEW_SIGNAL_DEBOUNCE,
	FSM_VFE_TVDEC_TUNER_SCANNING_INITIAL,
	FSM_VFE_TVDEC_TUNER_SCANNING,
	FSM_VFE_TVDEC_VCR,
	FSM_VFE_TVDEC_NON_STANDARD,
	FSM_VFE_VAFE_SOG_BOUNDARY_DETECT,
	FSM_VFE_UNUSED = 0xff,
} TVFE_CheckState_e;

/* Describe embedded front-end video digitizer */
typedef enum {
	TVFE_TV_DECODER,
	TVFE_ADC,
	TVFE_HDMI,
	TVFE_CCIR656,
	TVFE_MPEG,
	TVFE_DEVICE_MAX,
} TVFE_Device_e;

typedef enum {
	TVFE_FRAME_BUFFER_MODE = 0,
	TVFE_BYPASS_MODE,
	TVFE_MODE_MAX,
} TVFE_ByPassmode_e;

/* YPbPr Auto SOG Status */
typedef enum {
	TVFE_YPBPR_AUTOSOG_Idle,
	TVFE_YPBPR_AUTOSOG_Start,
	TVFE_YPBPR_AUTOSOG_Roughly,
	TVFE_YPBPR_AUTOSOG_Roughly_Debounce,
	TVFE_YPBPR_AUTOSOG_0x10,
	TVFE_YPBPR_AUTOSOG_0x10_Debounce,
	TVFE_YPBPR_AUTOSOG_0x08,
	TVFE_YPBPR_AUTOSOG_0x08_Debounce,
	TVFE_YPBPR_AUTOSOG_0x04,
	TVFE_YPBPR_AUTOSOG_0x04_Debounce,
	TVFE_YPBPR_AUTOSOG_0x02,
	TVFE_YPBPR_AUTOSOG_0x02_Debounce,
	TVFE_YPBPR_AUTOSOG_Error,
	TVFE_YPBPR_AUTOSOG_Finish
} TVFE_YPbPr_AutoSOGState_e;

/*!
 * @brief   YPbPr AutoSOG Boundary Detect Status
 */
typedef enum {
    TVFE_YPBPR_AUTOSOG_BoundaryDetect_Idle,
    TVFE_YPBPR_AUTOSOG_BoundaryDetect_Start,
    TVFE_YPBPR_AUTOSOG_BoundaryDetect_FineTune0,
    TVFE_YPBPR_AUTOSOG_BoundaryDetect_FineTune1,
    TVFE_YPBPR_AUTOSOG_BoundaryDetect_FineTune2,
    TVFE_YPBPR_AUTOSOG_BoundaryDetect_Error,
    TVFE_YPBPR_AUTOSOG_BoundaryDetect_Finish
} TVFE_YPbPr_AutoSOGBoundaryDetectState_e;

typedef enum
{
	VAFE_AUTOTUNE_NULL=0x000,
	VAFE_AUTOTUNE_IDLE,
	VAFE_AUTOTUNE_INITIAL,
	VAFE_AUTOTUNE_PRE_PROCESS,

	VAFE_AUTOTUNE_PHASE_MASK=0x100,
	VAFE_AUTOTUNE_PHASE_INIT,
	VAFE_AUTOTUNE_PHASE_ROUGH_TUNE,
	VAFE_AUTOTUNE_PHASE,
	VAFE_AUTOTUNE_PHASE_FINISH,

	VAFE_AUTOTUNE_CLOCK_MASK=0x200,
	VAFE_AUTOTUNE_CLOCK_INIT,
	VAFE_AUTOTUNE_CLOCK,
	VAFE_AUTOTUNE_CLOCK_ROUGH_TUNE,
	VAFE_AUTOTUNE_CLOCK_FINE_TUNE,
	VAFE_AUTOTUNE_CLOCK_FINISH,

	VAFE_AUTOTUNE_POSITION_MASK=0x300,
	VAFE_AUTOTUNE_POSITION_INIT,
	VAFE_AUTOTUNE_POSITION,
	VAFE_AUTOTUNE_POSITION_FINISH,

	VAFE_AUTOTUNE_FINAL_MASK=0x400,
	VAFE_AUTOTUNE_FINAL_OK,
	VAFE_AUTOTUNE_FINAL_ERR,
	VAFE_AUTOTUNE_POST_PROCESS,
} TVFE_AutoTuneState_e;

/* Enumeration of YPbPr phase status */
/* must stay here */
typedef enum {
	TVFE_YPBPR_AUTO_PHASE_RUNNING = 0,
	TVFE_YPBPR_AUTO_PHASE_COMPLETE,
	TVFE_YPBPR_AUTO_PHASE_NOT_YPBPR,
} TVFE_YPbPrAutoPhaseStatus_e;

typedef enum
{
    TVFE_AUDIO_LINE_IN_ADC_POWER_ON	= 0,
    TVFE_AUDIO_LINE_IN_ADC_POWER_OFF,
    TVFE_AUDIO_DAC_VRT_ON,
    TVFE_AUDIO_DAC_VRT_OFF,
    TVFE_AUDIO_DAC_VCOM_ON,
    TVFE_AUDIO_DAC_VCOM_OFF,
    TVFE_AUDIO_DAC_POWER_ON,
    TVFE_AUDIO_DAC_POWER_OFF,
    TVFE_AUDIO_DAC0_POWER_ON,
    TVFE_AUDIO_DAC0_POWER_OFF,
    TVFE_AUDIO_DAC1_POWER_ON,
    TVFE_AUDIO_DAC1_POWER_OFF,
    TVFE_AUDIO_DAC2_POWER_ON,
    TVFE_AUDIO_DAC2_POWER_OFF,
    TVFE_AUDIO_ALL_MODULE_POWER_ON,
    TVFE_AUDIO_ALL_MODULE_POWER_OFF,
} TVFE_AudioPowerContro_e;

typedef enum
{
    TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_BG		= (0x01),	/* B/G(H) (2 possible stereo standards)  5.5 MHz */
    TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_DK  	= (0x02),	/* D/K/K'(4 possible stereo standards)   6.5 MHz */
    TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_L       = (0x04),	/* L/L'                                  6.5 MHz */
    TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_I       = (0x08),	/* I                                     6.0 MHz */
    TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_M       = (0x10),	/* M(Korea A2 BTSC or EIAJ)              4.5 MHz */
    TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_BGDKI	= (TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_BG|TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_DK|TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_I),//todo:remove this
    TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_BGDKIM	= (TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_BG|TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_DK|TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_I|TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_M),//todo:remove this
    TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_ALL		= (0x1f),
    TVFE_AUDIO_SIF_STANDARD_SSS_MODE_BG,
    TVFE_AUDIO_SIF_STANDARD_SSS_MODE_DK,
    TVFE_AUDIO_SIF_STANDARD_SSS_MODE_M,
    TVFE_AUDIO_SIF_STANDARD_SSS_MODE_BG_A2,
    TVFE_AUDIO_SIF_STANDARD_SSS_MODE_BG_NICAM,
    TVFE_AUDIO_SIF_STANDARD_SSS_MODE_DK_A2_1,
    TVFE_AUDIO_SIF_STANDARD_SSS_MODE_DK_A2_2,
    TVFE_AUDIO_SIF_STANDARD_SSS_MODE_DK_A2_3,
    TVFE_AUDIO_SIF_STANDARD_SSS_MODE_DK_NICAM,
    TVFE_AUDIO_SIF_STANDARD_SSS_MODE_L_NICAM,
    TVFE_AUDIO_SIF_STANDARD_SSS_MODE_I_NICAM,
    TVFE_AUDIO_SIF_STANDARD_SSS_MODE_M_KOREA,
    TVFE_AUDIO_SIF_STANDARD_SSS_MODE_M_BTSC,
    TVFE_AUDIO_SIF_STANDARD_SSS_MODE_M_EIAJ,
    TVFE_AUDIO_SIF_STANDARD_SSS_MODE_FM_10_7_IF_50_US,
    TVFE_AUDIO_SIF_STANDARD_SSS_MODE_FM_10_7_IF_75_US,
    TVFE_AUDIO_SIF_STANDARD_SSS_MODE_FM_SELECTABLE_IF_50_US,        /* Need to check ?? */
    TVFE_AUDIO_SIF_STANDARD_SSS_MODE_FM_SELECTABLE_IF_75_US,        /* Need to check ?? */
} TVFE_AudioSIFStandardMask_e;

typedef enum
{
    TVFE_AUDIO_OUTPUT_TYPE_NULL	= 0,
    TVFE_AUDIO_OUTPUT_TYPE_DAC,
    TVFE_AUDIO_OUTPUT_TYPE_I2S,
    TVFE_AUDIO_OUTPUT_TYPE_SPDIF,
    TVFE_AUDIO_OUTPUT_TYPE_MAX
} TVFE_AudioOutputType_e;

typedef enum
{
    TVFE_AUDIO_OUTPUT_NULL	= 0,
    TVFE_AUDIO_OUTPUT_DAC0_L,
    TVFE_AUDIO_OUTPUT_DAC0_R,
    TVFE_AUDIO_OUTPUT_DAC1_L,
    TVFE_AUDIO_OUTPUT_DAC1_R,
    TVFE_AUDIO_OUTPUT_DAC2_L,
    TVFE_AUDIO_OUTPUT_DAC2_R,
    TVFE_AUDIO_OUTPUT_I2S0_L,
    TVFE_AUDIO_OUTPUT_I2S0_R,
    TVFE_AUDIO_OUTPUT_I2S1_L,
    TVFE_AUDIO_OUTPUT_I2S1_R,
    TVFE_AUDIO_OUTPUT_I2S2_L,
    TVFE_AUDIO_OUTPUT_I2S2_R,
    TVFE_AUDIO_OUTPUT_SPDIF_L,
    TVFE_AUDIO_OUTPUT_SPDIF_R,
    TVFE_AUDIO_OUTPUT_MAX
} TVFEAudioOutputConfig_e;

typedef enum
{
    TVFE_AUDIO_AUXACT_NULL			= 0,
    TVFE_AUDIO_AUXACT_FOLLOW_PRIMARY,
    TVFE_AUDIO_AUXACT_FOLLOW_TV,
    TVFE_AUDIO_AUXACT_MAX
} TVFE_AudioAUXAction_e;

typedef enum
{
    TVFE_AM_REQUEST_INITIAL_CONNECT      = 0,
} TVFE_AudioManagerRequst_e;

typedef enum {
	TVFE_1TIMES_SAMPLING = 0,
	TVFE_2TIMES_SAMPLING,
	TVFE_4TIMES_SAMPLING,
} TVFE_SamplingMode_e;

typedef enum
{
    TVFE_AUDIO_MUTE_PRIMARY_PATH	= 0,
    TVFE_AUDIO_MUTE_PRIMARY_LEFT_PATH,
    TVFE_AUDIO_MUTE_PRIMARY_RIGHT_PATH,
    TVFE_AUDIO_MUTE_AUX1_PATH,
    TVFE_AUDIO_MUTE_AUX2_PATH,
    TVFE_AUDIO_MUTE_AUX3_PATH,
    TVFE_AUDIO_MUTE_ALL_PATH,
    TVFE_AUDIO_MUTE_SUBWOOFER_PATH,
    TVFE_AUDIO_MUTE_CENTER_PATH,
    TVFE_AUDIO_MUTE_SURROUND_PATH,
    TVFE_AUDIO_MUTE_PATH_MAX,
} TVFE_AudioMutePath_e;

typedef enum
{
    TVFE_AUDIO_PATH_NULL                  = 0,
    TVFE_AUDIO_PATH_PRIMARY,
    TVFE_AUDIO_PATH_AUX1,
    TVFE_AUDIO_PATH_AUX2,
    TVFE_AUDIO_PATH_AUX3,
    TVFE_AUDIO_PATH_CENTER_SURROUND,
    TVFE_AUDIO_PATH_CENTER_SUBWOOFER,
    TVFE_AUDIO_PATH_SURROUND_SUBWOOFER,
    TVFE_AUDIO_PATH_MAX
} TVFE_AudioPath_e;

typedef enum
{
    TVFE_AUDIO_SIF_MODE_ANALOG_MONO		= 0,
    TVFE_AUDIO_SIF_MODE_NICAM_MONO,
    TVFE_AUDIO_SIF_MODE_FORCED_MONO,
    TVFE_AUDIO_SIF_MODE_STEREO,
    TVFE_AUDIO_SIF_MODE_DUAL_I,
    TVFE_AUDIO_SIF_MODE_DUAL_II,
    TVFE_AUDIO_SIF_MODE_DUAL_I_II,
    TVFE_AUDIO_SIF_MODE_SAP,
    TVFE_AUDIO_SIF_MODE_MAX,
} TVFE_AudioSIFMode_e;
typedef enum
{
    TVFE_AUDIO_SIF_STANDARD_FAIL	= 0,
    TVFE_AUDIO_SIF_STANDARD_BG,
    TVFE_AUDIO_SIF_STANDARD_DK,
    TVFE_AUDIO_SIF_STANDARD_M,
    TVFE_AUDIO_SIF_STANDARD_BG_A2,
    TVFE_AUDIO_SIF_STANDARD_BG_NICAM,
    TVFE_AUDIO_SIF_STANDARD_DK_A2_1,
    TVFE_AUDIO_SIF_STANDARD_DK_A2_2,
    TVFE_AUDIO_SIF_STANDARD_DK_A2_3,
    TVFE_AUDIO_SIF_STANDARD_DK_NICAM,
    TVFE_AUDIO_SIF_STANDARD_L_NICAM,
    TVFE_AUDIO_SIF_STANDARD_I_NICAM,
    TVFE_AUDIO_SIF_STANDARD_M_KOREA,
    TVFE_AUDIO_SIF_STANDARD_M_BTSC,
    TVFE_AUDIO_SIF_STANDARD_M_EIAJ,
    TVFE_AUDIO_SIF_STANDARD_FM_10_7_IF_50_US,
    TVFE_AUDIO_SIF_STANDARD_FM_10_7_IF_75_US,
    TVFE_AUDIO_SIF_STANDARD_FM_SELECTABLE_IF_50_US,        /* Need to check ?? */
    TVFE_AUDIO_SIF_STANDARD_FM_SELECTABLE_IF_75_US,        /* Need to check ?? */
    TVFE_AUDIO_SIF_STANDARD_BUSY,
} TVFE_AudioSIFStandard_t;
typedef enum
{
    TVFE_AUDIO_SIF_AVAIL_BUSY						= 0x00000000,
    TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO				= 0x00000001,
    TVFE_AUDIO_SIF_AVAIL_NICAM_MONO				= 0x00000002,
    TVFE_AUDIO_SIF_AVAIL_STEREO					= 0x00000004,
    TVFE_AUDIO_SIF_AVAIL_DUAL						= 0x00000008,
    TVFE_AUDIO_SIF_AVAIL_SAP						= 0x00000010,
    TVFE_AUDIO_SIF_AVAIL_NICAM						= 0x00000020,
    TVFE_AUDIO_SIF_AVAIL_FORCED_MONO				= 0x00000040,
    TVFE_AUDIO_SIF_AVAIL_BTSC_PILOT_OVERFLOW	= 0x80000000,
} TVFE_AudioSIFAvailableMode_e;


typedef enum
{
    TVFE_AUDIO_REPORT_INDIRECT	= 0,//report from audio manager task
    TVFE_AUDIO_REPORT_DIRECT,			//direct report from hardware status, for standard report when channel scanning
} TVFE_AudioReportMethod_t;

typedef enum
{
    AUDIO_MANAGER_SHUT_DOWN      = 0,
    AUDIO_MANAGER_CONNECT_INITIAL,
    AUDIO_MANAGER_CONNECTING,
    AUDIO_MANAGER_CONNECT_READY,
    AUDIO_MANAGER_CONNECT_TIME_OUT,
    AUDIO_MANAGER_STATE_MAX,
} AudioManagerState_t;
typedef enum {
	TVFE_SOUND_STANDARD_AUTO,
	TVFE_SOUND_STANDARD_BG,
	TVFE_SOUND_STANDARD_DK,
	TVFE_SOUND_STANDARD_I,
	TVFE_SOUND_STANDARD_L,
	TVFE_SOUND_STANDARD_L1,
	TVFE_SOUND_STANDARD_MAX,
} TVFE_SoundStd_e;

typedef enum {
	TVFE_CUSTOMER_HDMI_VIDEO_TIMING_TABLE	= 0,
	TVFE_HDMI_VIDEO_TIMING_TABLE,
	TVFE_CUSTOMER_HDMI_PC_TIMING_TABLE,
	TVFE_HDMI_PC_TIMING_TABLE,
	TVFE_HDMI_TIMING_TABLE_MAX,
} TVFE_HDMITimingTable_e;


typedef enum {
	TVFE_VIDEO_MUTE_ON_USER_DEFINED_COLOR,
	TVFE_VIDEO_MUTE_ON_BLACK,
	TVFE_VIDEO_AUTOMUTE_ON_BLACK,
	TVFE_VIDEO_UNMUTE,
} TVFE_VideoMuteStatus_e;
#if 0
/* Enumeration of FSM states */
typedef enum {
	MID_TVFE_NO_SIGNAL,
	MID_TVFE_NO_SIGNAL_CHECK_STATUS,
	MID_TVFE_SIGNAL_IN_DEBOUNCE,
	MID_TVFE_NO_SIGNAL_DEBOUNCE,
	MID_TVFE_SIGNAL_OUT_DEBOUNCE,
	MID_TVFE_SIGNAL_STABLE_UPDATE_RESOLUTION,
	MID_TVFE_SIGNAL_STABLE,
	MID_TVFE_SIGNAL_UNSUPPORT,
	MID_TVFE_VAFE_AUTOTUNE,
	MID_TVFE_VAFE_HW_AUTO_OFFSET,
	MID_TVFE_VAFE_HW_AUTO_GAIN,
	MID_TVFE_VAFE_HW_AUTOCALIBRATION,
	MID_TVFE_VAFE_MODE_DETECT,
	MID_TVFE_VAFE_AUTOCOLOR_OFFSET_ADJUST,
	MID_TVFE_TVDEC_TUNER_FIND_NEW_SIGNAL,
	MID_TVFE_TVDEC_TUNER_FIND_NEW_SIGNAL_DEBOUNCE,
	MID_TVFE_TVDEC_TUNER_SCANNING,
	MID_TVFE_TVDEC_VCR,
	MID_TVFE_TVDEC_NON_STANDARD,
	MID_TVFE_UNUSED = 0xff,
} MID_TVFE_CheckState_e;
#endif

typedef enum {
	TVFE_ADC_SYNC_POLARITY_HIGH = 0,
	TVFE_ADC_SYNC_POLARITY_LOW,
} TVFE_ADCSyncPolarity_t;

typedef enum {
	TVFE_HDMI_Color_Space_Auto = 0,
	TVFE_HDMI_Color_Space_YUV444,
	TVFE_HDMI_Color_Space_YUV422,
} TVFE_HDMI_Color_Space_t;

typedef enum
{
	TVFE_ATV_Input_Vsync_FreqRange_Unknown = 0,
	TVFE_ATV_Input_Vsync_FreqRange_50Hz,
	TVFE_ATV_Input_Vsync_FreqRange_60Hz,
	TVFE_ATV_Input_Vsync_FreqRange_Other,
} TVFE_ATV_INPUT_VSYNCFREQ_t;

typedef enum
{
    TVFE_ATV_SIF_CONNECT_TYPE = 0,
    TVFE_ATV_UNDEFINE = 0xff,
} TVFE_ATVCallbackType_t;

/*****************************************************************************
** Structure defined here
*****************************************************************************/
typedef struct {
	UINT16 wHStart_x1; /* H start position of 1X timing */
	UINT16 wHStart_x1_5; /*  H start position of 1.5X timing */
	INT8 bSVOffsetX; /* SVideo H start position offset */
	INT8 bSVOffsetY; /* SVideo V start position offset */
	INT8 bSCARTRgbOffsetX; /* SCART_RGB H start position offset */
	INT8 bSCARTRgbOffsetY; /* SCART_RGB V start position offset */
} tvdec_param_st;

typedef struct {
	UINT8 bTUNER_HUpSampling; /* H Upsampling mode for TUNER source */
	UINT8 bTUNER_10bitsPath; /* DRAM control mode for TUNER source */
	UINT8 bCVBS_HUpSampling; /* H Upsampling mode for CVBS source */
	UINT8 bCVBS_10bitsPath; /* DRAM control mode for CVBS source */
	UINT8 bSVIDEO_HUpSampling; /* H Upsampling mode for SVIDEO source */
	UINT8 bSVIDEO_10bitsPath; /* DRAM control mode for SVIDEO source */
	UINT8 bSCART_HUpSampling; /* H Upsampling mode for SCART source */
	UINT8 bSCART_10bitsPath; /* DRAM control mode for SCART source */
} tvdec_optimize_st;

typedef struct {
//weilung for SPV9107
	struct {
		UINT8 bfPolRef : 1;
		UINT8 bHPol : 1;
		UINT8 bVPol : 1;
		UINT8 bReserved : 5;
	} polarity;
	UINT8 bEnableFrameBufferMode;
	UINT8 bSamplingMode;
	UINT8 bPhase;
	struct {
		UINT8 bSOGTh : 5 ;
		UINT8 bReserved : 3;
	} sog;
	UINT8 bVCORange; //2-bits
	UINT8 bCurrent; //5-bits
	UINT8 bClampPlacement;
	UINT8 bClampDuration;
	UINT8 bAAFBandWidth; //5-bits
	UINT8 bPreCoast;
	UINT8 bPostCoast;
	UINT8 bModeDetectWithSyncWidth;
} adc_param_st;

/* 10bits or not */
typedef struct {
	UINT8 b10bitsPath; /* DRAM control mode */
} optimize_st;


/* YUV444 or not */
typedef struct {
	UINT8 bYUV;/*HDMI Output Color Space*/
} Color_Space_st;


typedef struct {
	UINT8 bEnableFrameBufferMode;
} tmds_param_st;

typedef void (*fTVFEPtr)(void);

typedef struct {
	TVFE_MpegAspectRatio_e eAspectRatio;
	TVFE_MpegFrameRatio_e eFrameRatio;
} mpeg_param_st;

typedef int (*fHPDandTerminationCallBack)(int iLevel);

/* HDMI callback */
typedef struct {
	fHPDandTerminationCallBack HPDandTerminationCallBack;
} TVFE_HDMICallBack_st;

/* MPEG global structure */
typedef struct {
} tvfe_mpeg_task_global_variable_st;

/* Scart global structure */
typedef struct {
} tvfe_scart_task_global_variable_st;

#if 0
/* Input pin config */
typedef union {
	struct {
		PLF_AVPin_e cvbs_pin;
		PLF_AVPin_e r_pin;
		PLF_AVPin_e g_pin;
		PLF_AVPin_e b_pin;
		PLF_AVPin_e fs_pin;
		PLF_AVPin_e fb_pin;
	} scart_av_rgb;
	struct {
		PLF_AVPin_e cvbs_pin;
		PLF_AVPin_e fs_pin;
	} scart_av;
	struct {
		PLF_AVPin_e y_pin;
		PLF_AVPin_e c_pin;
		PLF_AVPin_e fs_pin;
	} scart_sv;
	struct {
		PLF_AVPin_e pin;
	} cvbs;
	struct {
		PLF_AVPin_e y_pin;
		PLF_AVPin_e c_pin;
	} svideo;
	struct {
		PLF_AVPin_e r_pin;
		PLF_AVPin_e g_pin;
		PLF_AVPin_e b_pin;
	} rgb;
	struct {
		PLF_AVPin_e y_pin;
		PLF_AVPin_e pb_pin;
		PLF_AVPin_e pr_pin;
		PLF_AVPin_e soy_pin;
	} ypbpr;
	struct {
		PLF_AVPin_e pin;
		PLF_AVPin_e hpd;
	} hdmi;
	struct {
		PLF_AVPin_e pin;
	} digital;
} TVFE_InputPinConfig_st;
#endif

typedef struct
{
	TVFE_AudioPath_e ePath;
	TVFE_AudioOutputType_e eType;
	TVFEAudioOutputConfig_e eLPin;
	TVFEAudioOutputConfig_e eRPin;
} TVFE_AudioOutputPinConfig_st;

typedef struct
{
	TVFE_AudioPath_e ePath;
	TVFE_AudioAUXAction_e eAction;
} TVFE_AudioAUXConfig_st;

typedef struct {
	TVFE_AudioOutputPinConfig_st OutputConfig;
	TVFE_AudioAUXAction_e eAction;
} TVFE_AudioOutputConfig_st;

/* Audio */
typedef struct
{
	INT8 bPGA_gain;	                //!PGA gain control value : -25(mute) ~ 6(6 dB),(1 step is 1 dB)
	UINT32 dLipSyncMemoryStarAddress;   //!memory star address for lip sync (dely line unit)
	UINT32 dLipSyncMemorySize;		    //!memory size for lip sync (dely line unit)
	//Boolean fEnEventReport;		        //!reserved for enalbe/disable event reporter
	//UINT8   bAudioManagerConfig         //!reserved for audio manager config
} TVFE_AudioInitialConfig_st;

typedef struct
{
	INT8 bDECAdjustLevel;
	INT8 bMonoAdjustLevel;
	INT8 bNICAMAdjustLevel;
	INT8 bSAPAdjustLevel;
	INT8 bADCAdjustLevel;
	INT8 bDigitalAdjustLevel;
} TVFE_AudioLevelAdjust_st;

typedef struct
{
	TVFE_VideoMuteStatus_e fISRMuteStatus;
	Boolean fTVFEMuteFlag;
	Boolean fAppMuteFlag;
} TVFE_Mute_Flag_st;

/*!
 * @brief   Describe VAFE Driver Relative Variables for customer used
 */
typedef struct
{
    UINT16  wPC_ADCLK_MAX;
    UINT16  wPC_HSYNC_MAX;
    UINT16  wPC_HSYNC_MIN;
    UINT16  wPC_VSYNC_MAX;
    UINT16  wPC_VSYNC_MIN;
} TVFE_PC_Customer_Parameter_t;
typedef struct
{
    DRV_AudFrontEndReg_t RegInfo;
    TVFE_AudioSIFStandard_t   eStandard;
    TVFE_AudioSIFAvailableMode_e eMode;
    INT8                       bSIFAGCGain;
    UINT8                       bNICAMError;
} TVFE_AudioFrontEndStatus_t;

typedef struct
{
    Boolean fEn;
    Boolean fHysteresisFastMode;
    INT8 bGain;
} TVFE_AudioSIFAGCConfig_t;

typedef struct
{
	TVFE_AudioSIFAGCConfig_t  FM_AGCConfigParam;
    TVFE_AudioSIFAGCConfig_t  AM_AGCConfigParam;
} TVFE_AudioFrontEndControl_t;

typedef struct
{
    UINT8 bDebounceCounter;
    AudioManagerState_t eState;
    TVFE_AudioFrontEndStatus_t Status;
    TVFE_AudioFrontEndControl_t Control;
    Boolean fBTSCPilotLevelOverflow;  //use to record the pilot tone level overflow status
} AudioManagerParam_t;

typedef struct
{
	TVFE_AudioReportMethod_t		eMethod;
    TVFE_AudioSIFStandard_t   		eRetStandard;
    TVFE_AudioSIFAvailableMode_e 	eRetMode;
    UINT8	bRetSIFAGCGain;
	UINT8	bNICAMError;
} TVFE_AudioFrontEndParam_t;

#endif
