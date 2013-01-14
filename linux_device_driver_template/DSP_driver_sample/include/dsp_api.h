/*!
*************************************************************************
* \file    dsp_api.h
*
* \brief   audio dsp common APIs header file
* \note
*
* \date    2008/12/16
*
* \author  - jason.su        <jason.su@sunplus.com>
*
*************************************************************************
*/

#ifndef __DSP_H__
#define __DSP_H__

/******************************************************************************
  includes
 *****************************************************************************/
#include "dsp_mem.h"
#include "dsp_util.h"
#include "dsp_msg.h"
#include "dsp_cmd.h"
#include <asm/mach-spv9200/types.h>
#include "dsp_code.h"
#include "hw_ssd_def.h"
/******************************************************************************
  Defines
 *****************************************************************************/

//
// Mainprogram states
//
#define MAIN_STATE_INIT                        0
#define MAIN_STATE_READY                       1
#define MAIN_STATE_IDLE                        2
#define MAIN_STATE_RUN                         3
#define MAIN_STATE_DEAD                        4

//
// Task states
//
#define TASK_INIT                              0
#define TASK_READY                             1
#define TASK_STOP                              2
#define TASK_PLAY                              3
#define TASK_FF                                4
#define TASK_PAUSE                             5

#define DSP_STATE_SYSTEM                       0
#define DSP_STATE_DEC1                         1
#define DSP_STATE_DEC2                         2
#define DSP_STATE_ENC                          3

// BankID
#define CODEC_BANK_SYSTEM                      0
#define CODEC_BANK_DEC1                        1
#define CODEC_BANK_DEC2                        2
#define CODEC_BANK_ENC                         3

//others
#define __UNUSED__ __attribute__((unused))


// Sample Rate Specified by RISC parameters
// Sample Rate ID
// 44.1k: 0x0000 88.2k: 0x0001 22.05k: 0x0002 176.4k: 0x0003 11.025k: 0x0004 5.5125k: 0x0005
// 48  k: 0x0010 96  k: 0x0011 24   k: 0x0012 192  k: 0x0013 12    k: 0x0014      6k: 0x0015
// 32  k: 0x0020 64  k: 0x0021 16   k: 0x0022 128  k: 0x0023  8    k: 0x0024      4k: 0x0025
// 768 k: 0x0030 Not indicated: 0x0031
#define K_DSP_SAMPLE_RATE_ID_44K          0x0000
#define K_DSP_SAMPLE_RATE_ID_88K          0x0001
#define K_DSP_SAMPLE_RATE_ID_22K          0x0002
#define K_DSP_SAMPLE_RATE_ID_176K         0x0003
#define K_DSP_SAMPLE_RATE_ID_11K          0x0004
#define K_DSP_SAMPLE_RATE_ID_5K           0x0005
#define K_DSP_SAMPLE_RATE_ID_48K          0x0010
#define K_DSP_SAMPLE_RATE_ID_96K          0x0011
#define K_DSP_SAMPLE_RATE_ID_24K          0x0012
#define K_DSP_SAMPLE_RATE_ID_192K         0x0013
#define K_DSP_SAMPLE_RATE_ID_12K          0x0014
#define K_DSP_SAMPLE_RATE_ID_6K           0x0015
#define K_DSP_SAMPLE_RATE_ID_32K          0x0020
#define K_DSP_SAMPLE_RATE_ID_64K          0x0021
#define K_DSP_SAMPLE_RATE_ID_16K          0x0022
#define K_DSP_SAMPLE_RATE_ID_128K         0x0023
#define K_DSP_SAMPLE_RATE_ID_8K           0x0024
#define K_DSP_SAMPLE_RATE_ID_4K           0x0025
#define K_DSP_SAMPLE_RATE_ID_768K         0x0030
#define K_DSP_SAMPLE_RATE_ID_UNKNOWN      0x0031



typedef enum _DspReturnID
{
	DSP_ERROR_NONE 			= 0,
	DSP_ERROR_DOWNLOAD,
	DSP_ERROR_INIT_SYSTEM,
	DSP_ERROR_DRAM_INIT,
	DSP_ERROR_RISC_PARAM,
	DSP_ERROR_DSP_PARAM,
	DSP_ERROR_CODEC_PARAM,
	DSP_ERROR_CODEC_BANKID,
	DSP_ERROR_BSBUF_ID,
	DSP_ERROR_BSBUF_FEED,
	DSP_ERROR_SET_DUMP,
	DSP_ERROR_GET_DUMP,
	DSP_ERROR_SET_BARRIER,
	DSP_ERROR_GET_BARRIER,

	DSP_ERROR_STATE,
	DSP_ERROR_WAIT_STATE,

	DSP_ERROR_CMD_WAIT,
	DSP_ERROR_CMD_FAIL,

	DSP_ERROR_BS_FEED,
	DSP_ERROR_BS_WAIT,
	DSP_ERROR_BS_INIT,
	DSP_ERROR_BS_READ,
	DSP_ERROR_BS_END,
	DSP_ERROR_BS_FILE_END,
	DSP_ERROR_NO_MESSAGE,

}DspReturnID;


typedef enum _BSID
{
	BSBUF_ID_DEC1 			= 1,
	BSBUF_ID_DEC2 			= 2,
	BSBUF_ID_ENC 			= 3,
}BSID;


typedef enum _CODECID
{
	ID_MAIN 			= 0x00,
	ID_DECODER_PCM		= 0x01,
	ID_DECODER_DDP		= 0x02,
	ID_DECODER_TRUEHD	= 0x03,
	ID_DECODER_DTSLBR	= 0x04,
	ID_DECODER_DTSHD	= 0x05,
	ID_DECODER_MPEG		= 0x06,
	ID_DECODER_AAC		= 0x07,
	ID_DECODER_WMA		= 0x08,
	ID_DECODER_RA		= 0x09,
	ID_DECODER_OGG		= 0x0A,
	ID_DECODER_EMU		= 0x0B,
	ID_DECODER_WMAPRO   = 0x0C,
	ID_DECODER_FLAC     = 0x0D,
	ID_DECODER_AMR      = 0x0E,
	ID_DECODER_MIDI     = 0x0F,

	ID_DECODER_APE     =  0x13,
    ID_DECODER_ALAC     = 0x14,
	ID_DECODER_AVS     =  0x15,

	ID_ENCODER_MPEG		= 0x10,
	ID_ENCODER_AC3		= 0x11,
	ID_ENCODER_DTS		= 0x12,
	ID_MAX_ENCID        = 0x12,

	ID_MAX_DECID        = 0x15,  // note should refine
	ID_MAX_CODECID      = 0x15,

	ID_SWAPP            = 0x1f,
}CODECID;


#define DSP_SetSSDParam(param, value)   DSP_SetSSDBit(param##_OFFSET, param##_BIT_MASK, param##_BIT_IDX, value)
#define DSP_GetSSDParam(param)          DSP_GetSSDBit(param##_OFFSET, param##_BIT_MASK, param##_BIT_IDX)

/******************************************************************************/
/**
* @brief Check SSD register value
******************************************************************************/
#define DSP_CheckParamSign(param, ret)\
{\
    ret = DSP_GetSSDBit(param##_OFFSET, param##_BIT_MASK, param##_BIT_IDX);\
	if (((ret) & (param##_SIGNED_BIT_MASK)) != 0)\
    {\
		ret |= ~param##_MASK;\
    }\
    else\
    {\
        ret &= param##_MASK;\
    }\
}\

/*
******************************
*
*   Enums for 9200
*
******************************
*/
typedef enum
{
	/* common status values */
	DSP_DRV_SUCCESS,				/*!< successful outcome					*/
	DSP_DRV_ERR_FAILURE,			/*!< operation failed					*/
	DSP_DRV_ERR_INVALID_HANDLE,		/*!< invalid handle						*/
	DSP_DRV_ERR_INVALID_ID,			/*!< invalid identifier					*/
	DSP_DRV_ERR_INVALID_PARAM,		/*!< invalid parameter					*/
	DSP_DRV_ERR_INVALID_OP,			/*!< requested operation is invalid		*/
	DSP_DRV_ERR_MEMORY_ALLOC,		/*!< problem allocating memory			*/
	DSP_DRV_ERR_MEMORY_SIZE,		/*!< problem with the size of memory	*/
									/* < supplied							*/
	DSP_DRV_ERR_RESOURCE_UNAVAILABLE,
	DSP_DRV_ERR_TIMEOUT,			/*!< timeout							*/
	DSP_DRV_WARN_NO_ACTION,			/* < the function completed successfully,*/
	                                /* < but no action was taken            */
	DSP_DRV_WARN_PARAM_CLIPPED,		/*!< the function has completed			*/
									/*!< successfully, though a parameter was	*/
									/*!< clipped to within a valid range.		*/
	DSP_DRV_WARN_BUFFER_EMPTY,
	DSP_DRV_WARN_BUFFER_FULL,
	DSP_DRV_WARN_UNINITED,			/*!< driver has not been initialized yet */
	DSP_DRV_WARN_INITED			/*!< driver has been initialized already */
} DSP_DRV_Status_t;

typedef enum
{
    DSP_DRV_AUD_EFFECT_MODE_DEFAULT                                 = REG_SSD_SNDMOD_VALUE_MONO_STEREO_DEFAULT,
	DSP_DRV_AUD_EFFECT_MODE_SRS_TRUSURROUND_XT_DPL                  = REG_SSD_SNDMOD_VALUE_SRS_TRUSURROUND_XT_DPL,
    DSP_DRV_AUD_EFFECT_MODE_SRS_TRUSURROUND_XT_PASSIVE_MATRIX       = REG_SSD_SNDMOD_VALUE_SRS_TRUSURROUND_XT_PASSIVE_MATRIX,
	DSP_DRV_AUD_EFFECT_MODE_SRS_TRUSURROUND_HD_DPL                  = REG_SSD_SNDMOD_VALUE_SRS_TRUSURROUND_HD_DPL,
    DSP_DRV_AUD_EFFECT_MODE_SRS_TRUSURROUND_HD_PASSIVE_MATRIX       = REG_SSD_SNDMOD_VALUE_SRS_TRUSURROUND_HD_PASSIVE_MATRIX,
	DSP_DRV_AUD_EFFECT_MODE_NOISE_SEQUENCING                        = REG_SSD_SNDMOD_VALUE_NOISE_SEQUENCING,
} DSP_DRV_AudEffectMode_t;

typedef enum
{
    DSP_DRV_AUD_MUTE_PATH_MAIN               = 0,
    DSP_DRV_AUD_MUTE_PATH_MAIN_LEFT,
    DSP_DRV_AUD_MUTE_PATH_MAIN_RIGHT,
    DSP_DRV_AUD_MUTE_PATH_SUBWOOFER,
    DSP_DRV_AUD_MUTE_PATH_CENTER,
    DSP_DRV_AUD_MUTE_PATH_SURROUND,
    DSP_DRV_AUD_MUTE_PATH_AUX1,
    DSP_DRV_AUD_MUTE_PATH_AUX2,
    DSP_DRV_AUD_MUTE_PATH_AUX3,
    DSP_DRV_AUD_MUTE_PATH_ALL
} DSP_DRV_AudMutePath_t;

/**
*  @brief enum for SSD output channel
*/
typedef enum
{
    DSP_DRV_AUD_SSD_OUTPUT_DAC0          = 0,
    DSP_DRV_AUD_SSD_OUTPUT_DAC1,
    DSP_DRV_AUD_SSD_OUTPUT_I2S,
    DSP_DRV_AUD_SSD_OUTPUT_SPDIF,
} DSP_DRV_AudSSDOutputPath_t;

/**
*  @brief enum for SSD output Source
*/
typedef enum
{
    DSP_DRV_AUD_SSD_OUTPUT_SOURCE_MAIN_L               = 0,
    DSP_DRV_AUD_SSD_OUTPUT_SOURCE_MAIN_R,
    DSP_DRV_AUD_SSD_OUTPUT_SOURCE_SUBWOOFER,
    DSP_DRV_AUD_SSD_OUTPUT_SOURCE_CENTER,
    DSP_DRV_AUD_SSD_OUTPUT_SOURCE_SURROUND,
    DSP_DRV_AUD_SSD_OUTPUT_SOURCE_AUX1_L,
    DSP_DRV_AUD_SSD_OUTPUT_SOURCE_AUX1_R,
    DSP_DRV_AUD_SSD_OUTPUT_SOURCE_AUX2_L,
    DSP_DRV_AUD_SSD_OUTPUT_SOURCE_AUX2_R,
    DSP_DRV_AUD_SSD_OUTPUT_SOURCE_AUX3_L,
    DSP_DRV_AUD_SSD_OUTPUT_SOURCE_AUX3_R,
    DSP_DRV_AUD_SSD_OUTPUT_SOURCE_MAIN_SUM,
    DSP_DRV_AUD_SSD_OUTPUT_SOURCE_DIGITAL_SILENCE,
} DSP_DRV_AudSSDOutputSource_t;

/**
*  @brief enum for PROCESS_CHANNEL
*/
typedef enum
{
    DSP_DRV_AUD_PROCESS_PATH_MAIN            = 0,
    DSP_DRV_AUD_PROCESS_PATH_SUBWOOFER,
    DSP_DRV_AUD_PROCESS_PATH_CENTER,
    DSP_DRV_AUD_PROCESS_PATH_SURROUND,
    DSP_DRV_AUD_PROCESS_PATH_AUX1,
    DSP_DRV_AUD_PROCESS_PATH_AUX2,
    DSP_DRV_AUD_PROCESS_PATH_AUX3,
} DSP_DRV_AudProcessPath_t;

typedef enum
{
    DSP_DRV_AUD_MAIN_AUX_PATH_SOURCE_ADC     = 0,
    DSP_DRV_AUD_MAIN_AUX_PATH_SOURCE_SRC1,
    DSP_DRV_AUD_MAIN_AUX_PATH_SOURCE_SRC2,
    DSP_DRV_AUD_MAIN_AUX_PATH_SOURCE_NOISE_SILENCE,
} DSP_DRV_AudMainAUXSource_t;

/**
*  @brief enum for Beeper Mode
*/
typedef enum
{
    DSP_DRV_AUD_BEEPER_GRAPHIC_MODE     = 0,
    DSP_DRV_AUD_BEEPER_PARAMETRIC_MODE,
} DSP_DRV_AudBeeperMode_t;

/**
*  @brief enum for Spatial Effect
*/
typedef enum
{
    DSP_DRV_AUD_SPATIAL_EFFECT_OFF                = REG_SSD_SOMOCTRL_VALUE_OFF,
    DSP_DRV_AUD_SPATIAL_EFFECT_I_STEREO           = REG_SSD_SOMOCTRL_VALUE_I_STEREO,
    DSP_DRV_AUD_SPATIAL_EFFECT_I_MONO             = REG_SSD_SOMOCTRL_VALUE_I_MONO,
    DSP_DRV_AUD_SPATIAL_EFFECT_SRS_SPATIALIZER    = REG_SSD_SOMOCTRL_VALUE_SRS_SPATIALIZER,
    DSP_DRV_AUD_SPATIAL_EFFECT_AUTO_I_STEREO_I_MONO,
} DSP_DRV_AudSpatialEffect_t;

/**
*  @brief enum for I Sound Mode
*/
typedef enum
{
    DSP_DRV_AUD_ISOUND_MODE_MANUAL  = 0,
    DSP_DRV_AUD_ISOUND_MODE_AUTO,
} DSP_DRV_AudISoundMode_t;

/**
*  @brief enum for I Sound Effect
*/
typedef enum
{
    DSP_DRV_AUD_ISOUND_INTENSITY_EFFECT_0    = REG_SSD_INSOEF_VALUE_1_MIN,
    DSP_DRV_AUD_ISOUND_INTENSITY_EFFECT_1    = REG_SSD_INSOEF_VALUE_2,
    DSP_DRV_AUD_ISOUND_INTENSITY_EFFECT_2    = REG_SSD_INSOEF_VALUE_3,
    DSP_DRV_AUD_ISOUND_INTENSITY_EFFECT_3    = REG_SSD_INSOEF_VALUE_4,
    DSP_DRV_AUD_ISOUND_INTENSITY_EFFECT_4    = REG_SSD_INSOEF_VALUE_5,
    DSP_DRV_AUD_ISOUND_INTENSITY_EFFECT_5    = REG_SSD_INSOEF_VALUE_6_MAX,
} DSP_DRV_AudISoundIntensityEffect_t;

/**
*  @brief enum for I Sound Fade Time
*/
typedef enum
{
    DSP_DRV_AUD_ISOUND_FADETIME_01_1_SEC   = 0,
    DSP_DRV_AUD_ISOUND_FADETIME_02_2_SEC,
    DSP_DRV_AUD_ISOUND_FADETIME_03_3_SEC,
    DSP_DRV_AUD_ISOUND_FADETIME_04_5_SEC,
    DSP_DRV_AUD_ISOUND_FADETIME_06_8_SEC,
    DSP_DRV_AUD_ISOUND_FADETIME_09_0_SEC,
    DSP_DRV_AUD_ISOUND_FADETIME_14_5_SEC,
    DSP_DRV_AUD_ISOUND_FADETIME_19_0_SEC,
    DSP_DRV_AUD_ISOUND_FADETIME_136_MSEC,
    DSP_DRV_AUD_ISOUND_FADETIME_272_MSEC,
    DSP_DRV_AUD_ISOUND_FADETIME_410_MSEC,
    DSP_DRV_AUD_ISOUND_FADETIME_546_MSEC,
    DSP_DRV_AUD_ISOUND_FADETIME_820_MSEC,
    DSP_DRV_AUD_ISOUND_FADETIME_01_8_SEC,
    DSP_DRV_AUD_ISOUND_FADETIME_02_4SEC,
} DSP_DRV_AudISoundFadeTime_t;

/**
*  @brief enum for I Sound Detectot Time
*/
typedef enum
{
    DSP_DRV_AUD_ISOUND_DETECTORTIME_01SEC   = REG_SSD_ISOUNDDETTIME_VALUE_1SEC,
    DSP_DRV_AUD_ISOUND_DETECTORTIME_05SEC   = REG_SSD_ISOUNDDETTIME_VALUE_5SEC,
    DSP_DRV_AUD_ISOUND_DETECTORTIME_08SEC   = REG_SSD_ISOUNDDETTIME_VALUE_8SEC,
    DSP_DRV_AUD_ISOUND_DETECTORTIME_14SEC   = REG_SSD_ISOUNDDETTIME_VALUE_14SEC,
} DSP_DRV_AudISoundDetectorTime_t;

/**
*  @brief enum for SRS 3D Center
*/
typedef enum
{
    DSP_DRV_AUD_SRS3D_CENTER_MINUS_09DB  = REG_SSD_SRS3DCENTER_VALUE_MINUS_9DB,
    DSP_DRV_AUD_SRS3D_CENTER_MINUS_14DB  = REG_SSD_SRS3DCENTER_VALUE_MINUS_14DB,
    DSP_DRV_AUD_SRS3D_CENTER_MINUS_15DB  = REG_SSD_SRS3DCENTER_VALUE_MINUS_15DB,
    DSP_DRV_AUD_SRS3D_CENTER_MINUS_16DB  = REG_SSD_SRS3DCENTER_VALUE_MINUS_16DB,
    DSP_DRV_AUD_SRS3D_CENTER_MINUS_17DB  = REG_SSD_SRS3DCENTER_VALUE_MINUS_17DB,
    DSP_DRV_AUD_SRS3D_CENTER_MINUS_18DB  = REG_SSD_SRS3DCENTER_VALUE_MINUS_18DB,
    DSP_DRV_AUD_SRS3D_CENTER_MINUS_19DB  = REG_SSD_SRS3DCENTER_VALUE_MINUS_19DB,
    DSP_DRV_AUD_SRS3D_CENTER_MINUS_20DB  = REG_SSD_SRS3DCENTER_VALUE_MINUS_20DB,
    DSP_DRV_AUD_SRS3D_CENTER_MINUS_21DB  = REG_SSD_SRS3DCENTER_VALUE_MINUS_21DB,
    DSP_DRV_AUD_SRS3D_CENTER_MINUS_22DB  = REG_SSD_SRS3DCENTER_VALUE_MINUS_22DB,
    DSP_DRV_AUD_SRS3D_CENTER_MINUS_23DB  = REG_SSD_SRS3DCENTER_VALUE_MINUS_23DB,
    DSP_DRV_AUD_SRS3D_CENTER_MINUS_24DB  = REG_SSD_SRS3DCENTER_VALUE_MINUS_24DB,
    DSP_DRV_AUD_SRS3D_CENTER_MINUS_25DB  = REG_SSD_SRS3DCENTER_VALUE_MINUS_25DB,
    DSP_DRV_AUD_SRS3D_CENTER_MINUS_26DB  = REG_SSD_SRS3DCENTER_VALUE_MINUS_26DB,
    DSP_DRV_AUD_SRS3D_CENTER_MINUS_27DB  = REG_SSD_SRS3DCENTER_VALUE_MINUS_27DB,
    DSP_DRV_AUD_SRS3D_CENTER_MINUS_OFF   = REG_SSD_SRS3DCENTER_VALUE_OFF,
} DSP_DRV_AudSRS3DCenter_t;

/**
*  @brief enum for SRS 3D Space
*/
typedef enum
{
    DSP_DRV_AUD_SRS3D_SPACE_MINUS_04DB  = REG_SSD_SRS3DSPACE_VALUE_MINUS_4DB,
    DSP_DRV_AUD_SRS3D_SPACE_MINUS_05DB  = REG_SSD_SRS3DSPACE_VALUE_MINUS_5DB,
    DSP_DRV_AUD_SRS3D_SPACE_MINUS_06DB  = REG_SSD_SRS3DSPACE_VALUE_MINUS_6DB,
    DSP_DRV_AUD_SRS3D_SPACE_MINUS_07DB  = REG_SSD_SRS3DSPACE_VALUE_MINUS_7DB,
    DSP_DRV_AUD_SRS3D_SPACE_MINUS_08DB  = REG_SSD_SRS3DSPACE_VALUE_MINUS_8DB,
    DSP_DRV_AUD_SRS3D_SPACE_MINUS_09DB  = REG_SSD_SRS3DSPACE_VALUE_MINUS_9DB,
    DSP_DRV_AUD_SRS3D_SPACE_MINUS_10DB  = REG_SSD_SRS3DSPACE_VALUE_MINUS_10DB,
    DSP_DRV_AUD_SRS3D_SPACE_MINUS_11DB  = REG_SSD_SRS3DSPACE_VALUE_MINUS_11DB,
    DSP_DRV_AUD_SRS3D_SPACE_MINUS_12DB  = REG_SSD_SRS3DSPACE_VALUE_MINUS_12DB,
    DSP_DRV_AUD_SRS3D_SPACE_MINUS_13DB  = REG_SSD_SRS3DSPACE_VALUE_MINUS_13DB,
    DSP_DRV_AUD_SRS3D_SPACE_MINUS_14DB  = REG_SSD_SRS3DSPACE_VALUE_MINUS_14DB,
    DSP_DRV_AUD_SRS3D_SPACE_MINUS_15DB  = REG_SSD_SRS3DSPACE_VALUE_MINUS_15DB,
    DSP_DRV_AUD_SRS3D_SPACE_MINUS_16DB  = REG_SSD_SRS3DSPACE_VALUE_MINUS_16DB,
    DSP_DRV_AUD_SRS3D_SPACE_MINUS_17DB  = REG_SSD_SRS3DSPACE_VALUE_MINUS_17DB,
    DSP_DRV_AUD_SRS3D_SPACE_MINUS_18DB  = REG_SSD_SRS3DSPACE_VALUE_MINUS_18DB,
    DSP_DRV_AUD_SRS3D_SPACE_MINUS_OFF   = REG_SSD_SRS3DSPACE_VALUE_OFF,
} DSP_DRV_AudSRS3DSpace_t;

/**
*  @brief enum for SRS Focus Level
*/
typedef enum
{
	DSP_DRV_AUD_FOCUS_LEVEL_VALUE_MINUS_4DB = REG_SSD_FOCUS_LEVEL_VALUE_MINUS_4DB,
	DSP_DRV_AUD_FOCUS_LEVEL_VALUE_MINUS_5DB = REG_SSD_FOCUS_LEVEL_VALUE_MINUS_5DB,
	DSP_DRV_AUD_FOCUS_LEVEL_VALUE_MINUS_6DB = REG_SSD_FOCUS_LEVEL_VALUE_MINUS_6DB,
	DSP_DRV_AUD_FOCUS_LEVEL_VALUE_MINUS_7DB = REG_SSD_FOCUS_LEVEL_VALUE_MINUS_7DB,
	DSP_DRV_AUD_FOCUS_LEVEL_VALUE_MINUS_8DB = REG_SSD_FOCUS_LEVEL_VALUE_MINUS_8DB,
	DSP_DRV_AUD_FOCUS_LEVEL_VALUE_MINUS_9DB = REG_SSD_FOCUS_LEVEL_VALUE_MINUS_9DB,
	DSP_DRV_AUD_FOCUS_LEVEL_VALUE_MINUS_10DB = REG_SSD_FOCUS_LEVEL_VALUE_MINUS_10DB,
	DSP_DRV_AUD_FOCUS_LEVEL_VALUE_MINUS_11DB = REG_SSD_FOCUS_LEVEL_VALUE_MINUS_11DB,
	DSP_DRV_AUD_FOCUS_LEVEL_VALUE_MINUS_12DB = REG_SSD_FOCUS_LEVEL_VALUE_MINUS_12DB,
	DSP_DRV_AUD_FOCUS_LEVEL_VALUE_MINUS_13DB = REG_SSD_FOCUS_LEVEL_VALUE_MINUS_13DB,
	DSP_DRV_AUD_FOCUS_LEVEL_VALUE_MINUS_14DB = REG_SSD_FOCUS_LEVEL_VALUE_MINUS_14DB,
	DSP_DRV_AUD_FOCUS_LEVEL_VALUE_MINUS_15DB = REG_SSD_FOCUS_LEVEL_VALUE_MINUS_15DB,
	DSP_DRV_AUD_FOCUS_LEVEL_VALUE_MINUS_16DB = REG_SSD_FOCUS_LEVEL_VALUE_MINUS_16DB,
	DSP_DRV_AUD_FOCUS_LEVEL_VALUE_MINUS_17DB = REG_SSD_FOCUS_LEVEL_VALUE_MINUS_17DB,
	DSP_DRV_AUD_FOCUS_LEVEL_VALUE_MINUS_18DB = REG_SSD_FOCUS_LEVEL_VALUE_MINUS_18DB,
} DSP_DRV_AudSRSFocusLevel_t;

/**
*  @brief enum for SRS Limiter Level
*/
typedef enum
{
	DSP_DRV_AUD_LIMITER_LEVEL_VALUE_MINUS_4DB = REG_SSD_LIMITER_LEVEL_VALUE_MINUS_4DB,
	DSP_DRV_AUD_LIMITER_LEVEL_VALUE_MINUS_5DB = REG_SSD_LIMITER_LEVEL_VALUE_MINUS_5DB,
	DSP_DRV_AUD_LIMITER_LEVEL_VALUE_MINUS_6DB = REG_SSD_LIMITER_LEVEL_VALUE_MINUS_6DB,
	DSP_DRV_AUD_LIMITER_LEVEL_VALUE_MINUS_7DB = REG_SSD_LIMITER_LEVEL_VALUE_MINUS_7DB,
	DSP_DRV_AUD_LIMITER_LEVEL_VALUE_MINUS_8DB = REG_SSD_LIMITER_LEVEL_VALUE_MINUS_8DB,
	DSP_DRV_AUD_LIMITER_LEVEL_VALUE_MINUS_9DB = REG_SSD_LIMITER_LEVEL_VALUE_MINUS_9DB,
	DSP_DRV_AUD_LIMITER_LEVEL_VALUE_MINUS_10DB = REG_SSD_LIMITER_LEVEL_VALUE_MINUS_10DB,
	DSP_DRV_AUD_LIMITER_LEVEL_VALUE_MINUS_11DB = REG_SSD_LIMITER_LEVEL_VALUE_MINUS_11DB,
	DSP_DRV_AUD_LIMITER_LEVEL_VALUE_MINUS_12DB = REG_SSD_LIMITER_LEVEL_VALUE_MINUS_12DB,
	DSP_DRV_AUD_LIMITER_LEVEL_VALUE_MINUS_13DB = REG_SSD_LIMITER_LEVEL_VALUE_MINUS_13DB,
	DSP_DRV_AUD_LIMITER_LEVEL_VALUE_MINUS_14DB = REG_SSD_LIMITER_LEVEL_VALUE_MINUS_14DB,
	DSP_DRV_AUD_LIMITER_LEVEL_VALUE_MINUS_15DB = REG_SSD_LIMITER_LEVEL_VALUE_MINUS_15DB,
	DSP_DRV_AUD_LIMITER_LEVEL_VALUE_MINUS_16DB = REG_SSD_LIMITER_LEVEL_VALUE_MINUS_16DB,
	DSP_DRV_AUD_LIMITER_LEVEL_VALUE_MINUS_17DB = REG_SSD_LIMITER_LEVEL_VALUE_MINUS_17DB,
	DSP_DRV_AUD_LIMITER_LEVEL_VALUE_MINUS_18DB = REG_SSD_LIMITER_LEVEL_VALUE_MINUS_18DB,
} DSP_DRV_AudSRSLimiterLevel_t;

/**
*  @brief enum for SRS WOW Brightness
*/
typedef enum
{
    DSP_DRV_AUD_SRS_3DENABLE_VALUE_ON       = REG_SSD_SRS_3DENABLE_VALUE_ON,
    DSP_DRV_AUD_SRS_3DENABLE_VALUE_OFF      = REG_SSD_SRS_3DENABLE_VALUE_OFF,
    DSP_DRV_AUD_SRS_MODE_VALUE_3D_STEREO    = REG_SSD_SRS_MODE_VALUE_3D_STEREO,
	DSP_DRV_AUD_SRS_MODE_VALUE_EXTREME		= REG_SSD_SRS_MODE_VALUE_EXTREME,
	DSP_DRV_AUD_SRS_WOWBITRATE_VALUE_LOW	= REG_SSD_WOWBITRATE_VALUE_LOW,
	DSP_DRV_AUD_SRS_WOWBITRATE_VALUE_HIGH	= REG_SSD_WOWBITRATE_VALUE_HIGH,
	DSP_DRV_AUD_SRS_FOCUSENABLE_VALUE_OFF	= REG_SSD_SRS_FOCUSENABLE_VALUE_OFF,
	DSP_DRV_AUD_SRS_FOCUSENABLE_VALUE_ON	= REG_SSD_SRS_FOCUSENABLE_VALUE_ON,
	DSP_DRV_AUD_SRS_LIMITERENABLE_VALUE_OFF = REG_SSD_SRS_LIMITERENABLE_VALUE_OFF,
} DSP_DRV_AudSRSWOWConfig_t;

/**
*  @brief enum for silence detection channel
*/
typedef enum
{
    DSP_DRV_AUD_SILENCE_DETECTION_CHANNEL_MAIN               = 0,
    DSP_DRV_AUD_SILENCE_DETECTION_CHANNEL_AUX1,
} DSP_DRV_AudSilenceDetectionChannel_t;

/**
*  @brief enum for Bass Management Mode
*/
typedef enum
{
    DSP_DRV_AUD_BASSMANAGEMENT_OFF          = REG_SSD_BAMAMO_VALUE_OFF_WIDE_CENTRE_MODE,
    DSP_DRV_AUD_BASSMANAGEMENT_TYPE1_MODE   = REG_SSD_BAMAMO_VALUE_TYP1_CONFIGURATION_NORMAL_CENTRE_MODE,
    DSP_DRV_AUD_BASSMANAGEMENT_TYPE2_MODE   = REG_SSD_BAMAMO_VALUE_TYP2_CONFIGURATION_NORMAL_CENTRE_MODE,
} DSP_DRV_AudBassManagementMode_t;

/**
*  @brief enum for I2S format
*/
typedef enum
{
    DSP_DRV_AUD_I2S_OUTPUT_PHILIPS24BIT		= 0,
    DSP_DRV_AUD_I2S_OUTPUT_SONY24BIT,
    DSP_DRV_AUD_I2S_OUTPUT_JAPANESE24BIT,
} DSP_DRV_AudI2SOutputFormat_t;

/**
*  @brief enum for Bass Feature Control
*/
typedef enum
{
    DSP_DRV_AUD_BASS_FEATURECONTROL_OFF            = REG_SSD_BASSFEATURECTRL_VALUE_DBB_DVB_AND_BBE_OFF,
    DSP_DRV_AUD_BASS_FEATURECONTROL_DBB_MAIN       = REG_SSD_BASSFEATURECTRL_VALUE_DBB_MAIN_CHANNEL_ON,
    DSP_DRV_AUD_BASS_FEATURECONTROL_DVB_MAIN       = REG_SSD_BASSFEATURECTRL_VALUE_DVB_MAIN_CHANNEL_ON,
    DSP_DRV_AUD_BASS_FEATURECONTROL_DBB_SUBWOOFER  = REG_SSD_BASSFEATURECTRL_VALUE_DBB_SUBWOOFER_CHANNEL_ON,
    DSP_DRV_AUD_BASS_FEATURECONTROL_DVB_SUBWOOFER  = REG_SSD_BASSFEATURECTRL_VALUE_DVB_SUBWOOFER_CHANNEL_ON,
} DSP_DRV_AudBassFeatureControl_t;

/**
*  @brief enum for Bass Management Filter Characteristic
*/
typedef enum
{
    DSP_DRV_AUD_BASSMANAGEMENT_FILTER_CHARACTERISTIC_050HZ = REG_SSD_BAMAFC_VALUE_50_HZ,
    DSP_DRV_AUD_BASSMANAGEMENT_FILTER_CHARACTERISTIC_060HZ = REG_SSD_BAMAFC_VALUE_60_HZ,
    DSP_DRV_AUD_BASSMANAGEMENT_FILTER_CHARACTERISTIC_070HZ = REG_SSD_BAMAFC_VALUE_70_HZ,
    DSP_DRV_AUD_BASSMANAGEMENT_FILTER_CHARACTERISTIC_080HZ = REG_SSD_BAMAFC_VALUE_80_HZ,
    DSP_DRV_AUD_BASSMANAGEMENT_FILTER_CHARACTERISTIC_090HZ = REG_SSD_BAMAFC_VALUE_90_HZ,
    DSP_DRV_AUD_BASSMANAGEMENT_FILTER_CHARACTERISTIC_100HZ = REG_SSD_BAMAFC_VALUE_100_HZ,
    DSP_DRV_AUD_BASSMANAGEMENT_FILTER_CHARACTERISTIC_110HZ = REG_SSD_BAMAFC_VALUE_110_HZ,
    DSP_DRV_AUD_BASSMANAGEMENT_FILTER_CHARACTERISTIC_120HZ = REG_SSD_BAMAFC_VALUE_120_HZ,
    DSP_DRV_AUD_BASSMANAGEMENT_FILTER_CHARACTERISTIC_130HZ = REG_SSD_BAMAFC_VALUE_130_HZ,
    DSP_DRV_AUD_BASSMANAGEMENT_FILTER_CHARACTERISTIC_140HZ = REG_SSD_BAMAFC_VALUE_140_HZ,
    DSP_DRV_AUD_BASSMANAGEMENT_FILTER_CHARACTERISTIC_150HZ = REG_SSD_BAMAFC_VALUE_150_HZ,
    DSP_DRV_AUD_BASSMANAGEMENT_FILTER_CHARACTERISTIC_200HZ = REG_SSD_BAMAFC_VALUE_200_HZ,
    DSP_DRV_AUD_BASSMANAGEMENT_FILTER_CHARACTERISTIC_250HZ = REG_SSD_BAMAFC_VALUE_250_HZ,
    DSP_DRV_AUD_BASSMANAGEMENT_FILTER_CHARACTERISTIC_300HZ = REG_SSD_BAMAFC_VALUE_300_HZ,
    DSP_DRV_AUD_BASSMANAGEMENT_FILTER_CHARACTERISTIC_350HZ = REG_SSD_BAMAFC_VALUE_350_HZ,
    DSP_DRV_AUD_BASSMANAGEMENT_FILTER_CHARACTERISTIC_400HZ = REG_SSD_BAMAFC_VALUE_400_HZ,
} DSP_DRV_AudBassManagementFilterCharacteristic_t;

/**
*  @brief enum for Subwoofer Mode
*/
typedef enum
{
    DSP_DRV_AUD_NORMAL_SUBWOOFER_MODE       = 0,
    DSP_DRV_AUD_ECONOMIC_SUBWOOFER_MODE,
} DSP_DRV_AudSubwooferMode_t;

/**
*  @brief enum for AVL mode
*/
typedef enum
{
    DSP_DRV_AUD_AVL_MODE_OFF    = REG_SSD_AVLMOD_VALUE_OFF,
    DSP_DRV_AUD_AVL_MODE_20MS   = REG_SSD_AVLMOD_VALUE_VERY_SHORT_DECAY_20_MS,
    DSP_DRV_AUD_AVL_MODE_02S    = REG_SSD_AVLMOD_VALUE_SHORT_DECAY_2_SEC,
    DSP_DRV_AUD_AVL_MODE_04S    = REG_SSD_AVLMOD_VALUE_MEDIUM_DECAY_4_SEC,
    DSP_DRV_AUD_AVL_MODE_08S    = REG_SSD_AVLMOD_VALUE_LONG_DECAY_8_SEC,
    DSP_DRV_AUD_AVL_MODE_16S    = REG_SSD_AVLMOD_VALUE_VERY_LONG_DECAY_16_SEC,
} DSP_DRV_AudAVLMode_t;

/**
*  @brief enum for silence detection threshold
*/
typedef enum
{
    DSP_DRV_AUD_SILENCE_DETECTION_THRESHOLD_MINUS_30_DBFS   = 0,
    DSP_DRV_AUD_SILENCE_DETECTION_THRESHOLD_MINUS_33_DBFS   = 1,
    DSP_DRV_AUD_SILENCE_DETECTION_THRESHOLD_MINUS_36_DBFS   = 2,
    DSP_DRV_AUD_SILENCE_DETECTION_THRESHOLD_MINUS_45_DBFS   = 5,
    DSP_DRV_AUD_SILENCE_DETECTION_THRESHOLD_MINUS_60_DBFS   = 10,
    DSP_DRV_AUD_SILENCE_DETECTION_THRESHOLD_MINUS_75_DBFS   = 15,
    DSP_DRV_AUD_SILENCE_DETECTION_THRESHOLD_MINUS_90_DBFS   = 20,
    DSP_DRV_AUD_SILENCE_DETECTION_THRESHOLD_MINUS_120_DBFS  = 30,
} DSP_DRV_AudSilenceDetectionThreshold_t;

/**
*  @brief enum for delay (in samples) of soft-mute after silence detected
*/
typedef enum
{
    DSP_DRV_AUD_SILENCE_DETECTION_DELAY_32_SAMPLES      = 0,
    DSP_DRV_AUD_SILENCE_DETECTION_DELAY_64_SAMPLES      = 1,
    DSP_DRV_AUD_SILENCE_DETECTION_DELAY_128_SAMPLES     = 2,
    DSP_DRV_AUD_SILENCE_DETECTION_DELAY_256_SAMPLES     = 3,
    DSP_DRV_AUD_SILENCE_DETECTION_DELAY_1024_SAMPLES    = 5,
    DSP_DRV_AUD_SILENCE_DETECTION_DELAY_8192_SAMPLES    = 8,
    DSP_DRV_AUD_SILENCE_DETECTION_DELAY_32768_SAMPLES   = 10,
} DSP_DRV_AudSilenceDetectionDelay_t;

/**
*  @brief enum for Clip Management
*/
typedef enum
{
    DSP_DRV_AUD_CLIP_MANAGEMENT_OFF                     = REG_SSD_CLIPMANAGE_VALUE_CLIP_MANAGEMENT_OFF_DEFAULT,
    DSP_DRV_AUD_CLIP_MANAGEMENT_STATIC_VOLUME_MODE      = REG_SSD_CLIPMANAGE_VALUE_STATIC_VOLUME_MODE,
    DSP_DRV_AUD_CLIP_MANAGEMENT_STATIC_CONTROL_MODE     = REG_SSD_CLIPMANAGE_VALUE_STATIC_CONTROL_MODE,
    DSP_DRV_AUD_CLIP_MANAGEMENT_DYNAMIC_CONTROL_MODE    = REG_SSD_CLIPMANAGE_VALUE_DYNAMIC_CONTROL_MODE,
    DSP_DRV_AUD_CLIP_MANAGEMENT_DYNAMIC_VOLUME_MODE     = REG_SSD_CLIPMANAGE_VALUE_DYNAMIC_VOLUME_MODE,
} DSP_DRV_AudClipManagementMode_t;

/**
*  @brief enum for Clip Management DVM peak detection selection
*/
typedef enum
{
    DSP_DRV_AUD_DVM_PEAK_DETECTION_SELECT_MAIN                  = 0,
    DSP_DRV_AUD_DVM_PEAK_DETECTION_SELECT_MAIN_CENTER_SUBWOOFER,
} DSP_DRV_AudDVMPeakDetectionSelection_t;

typedef enum
{
    DSP_DRV_AUD_MAINLOUDNESS_VOLUME_MINUS_15DB  = 0,
    DSP_DRV_AUD_MAINLOUDNESS_VOLUME_MINUS_12DB,
    DSP_DRV_AUD_MAINLOUDNESS_VOLUME_MINUS_09DB,
    DSP_DRV_AUD_MAINLOUDNESS_VOLUME_MINUS_06DB,
    DSP_DRV_AUD_MAINLOUDNESS_VOLUME_MINUS_03DB,
    DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_00DB,
    DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_03DB,
    DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_06DB,
    DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_09DB,
    DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_12DB,
    DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_15DB,
    DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_18DB,
    DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_21DB,
    DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_24DB,
    DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_27DB,
    DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_30DB,
} DSP_DRV_AudMainLoudnessVolumeLevel_t;
/***************************************************************************
	Structure
 ***************************************************************************/
typedef struct
{
    UINT32 dLipSyncMemoryStarAddress;//!memory star address for lip sync (dely line unit)
	UINT32 dLipSyncMemorySize;		//!memory size for lip sync (dely line unit)
}DSP_DRV_AudInitialConfig_t;

/******************************************************************************
        Globals
 *****************************************************************************/

/******************************************************************************
  Macros
 *****************************************************************************/
#define CACHED_ADDR(addr)     (((unsigned int) ((addr)) | SDRAM_A_BASE_CACHED ) & (~0x20000000))
#define UNCACHED_ADDR(addr)   (((unsigned int) (addr)) | SDRAM_A_BASE_UNCACHED)

#define DRAMB_UNCACHED_ADDR(addr)   (((unsigned int) (addr)) | SDRAM_B_BASE_UNCACHED)
#define DRAMB_CACHED_ADDR(addr)     (((unsigned int) ((addr)) | SDRAM_B_BASE_CACHED ) & (~0x20000000))

#define IS_DRAM_BANKA(x)		(((x&0xf0000000) == 0x80000000)|((x&0xf0000000)==0xa0000000 ))
#define IS_DRAM_BANKB(x)		(((x&0xf0000000) == 0x90000000)|((x&0xf0000000)==0xb0000000 ))
#define IS_CACHED_ADDRESS(x)    (((x&0xf0000000) == 0x80000000)|((x&0xf0000000)==0x90000000 ))


#define DSP_GO()              (HWREG_R(rdif_ctrl) = 0)
#define DSP_GO_2T_ROM()       (HWREG_R(rdif_ctrl) = 1<<4)
#define DSP_GO_2T_PREFETCH_4()(HWREG_R(rdif_ctrl) = 1 << 6 | 1 <<4)
#define DSP_RESET()           (HWREG_R(rdif_ctrl) = 1)
#define DSP_STALL()           (HWREG_R(rdif_ctrl) = 1<<1)

// before DSP_RESET, need to set DSP_SHUT_DOWN
// check if status == 0, if yes DSP is ready to reset
#define DSP_SHUTDOWN()        HWREG_W(rdif_oh_ctrl, 1<<1); \
                              while (HWREG_R(dsp_status)) ;

#define DIS_CODE_SCRAMBLE()   (HWREG_R(rdif_cfg) |= 1<<1)
#define CHANGE_CODE_KEY(x)    HWREG_W(rdif_oh_ctrl, 0x1);  \
                              HWREG_R(rdif_code_key) = x

#define ENABLE_JTAG()         HWREG_1(rdif_cfg, (1<<0))
#define DISABLE_JTAG()        HWREG_0(rdif_cfg, (~(1<<0)))


#define GET_CMD_VALID_BIT()   (HWREG_R(dsp_port[0])&0x80000000)
#define GET_CMD_PARAM_PORT()  (HWREG_R(dsp_port[1]))
#define SET_CMD_CODE_PORT(x)  (HWREG_R(dsp_port[0]) = x)
#define SET_CMD_PARAM_PORT(x) (HWREG_R(dsp_port[1]) = x)

#define GET_MSG_VALID_BIT()   (HWREG_R(dsp_port[10])&0x80000000)

#define GET_DSP_INT0()        (HWREG_R(rdif_int_ctrl)&0x1)
#define GET_DSP_INT1()        (HWREG_R(rdif_int_ctrl)&(0x1<<1))
#define GET_DSP_INT2()        (HWREG_R(rdif_int_ctrl)&(0x1<<2))
#define GET_DSP_INT3()        (HWREG_R(rdif_int_ctrl)&(0x1<<3))
#define GET_DSP_NMI()         (HWREG_R(rdif_int_ctrl)&(0x1<<4))
#define GET_DSP_VINT()        (HWREG_R(rdif_int_ctrl)&(0x1<<5))

//#define SET_DSP_INT0()      (HWREG_R(rdif_int_ctrl) = 0x1)
#define SET_DSP_INT0()        (HWREG_R(rdif_int_ctrl) |= 0x1)
//#define SET_DSP_INT1()      (HWREG_R(rdif_int_ctrl) = 0x1<<1) // RO:External interrupt
//#define SET_DSP_INT2()      (HWREG_R(rdif_int_ctrl) = 0x1<<2) // RO:Timer interrupt
#define SET_DSP_INT3()        (HWREG_R(rdif_int_ctrl) = 0x1<<3)
#define SET_DSP_NMI()         (HWREG_R(rdif_int_ctrl) = 0x1<<4)
#define SET_DSP_VINT()        (HWREG_R(rdif_int_ctrl) = 0x1<<5)

// Function macros
/*
#define DSP_GET_MAIN_STATE()  (UINT8)(HWREG_R(dsp_port[1])2&0xF)
#define DSP_GET_DEC1_STATE()  (UINT8)((HWREG_R(dsp_port[12])>>4)&0xF)
#define DSP_GET_DEC2_STATE()  (UINT8)((HWREG_R(dsp_port[12])>>8)&0xF)
#define DSP_GET_ENC_STATE()   (UINT8)((HWREG_R(dsp_port[12])>>16)&0xF)
*/

// Endian convertion
#define WORD16(x)             (((x&0xFF)<<8)|((x>>8)&0xFF))
#define WORD32(x)             ( x>>24 | ((x>>8)&0xFF00) | (x&0xff00)<<8| (x<<24) )

#define BS_GET_USED(aBsbufLen,aDump,aBarrier) (aDump >= aBarrier) ? (aDump-aBarrier):(aBsbufLen+aDump-aBarrier)
#define BS_GET_FREE(aBsbufLen,aDump,aBarrier) (aBsbufLen - (BS_GET_USED(aBsbufLen,aDump,aBarrier)))


#define OCEM_BUSY_WAIT()  do { while (HWREG_R(ocm_stat_reg)) ; } while (0)
#define GET_DSP_PC()      ocm_io_read(OCEM_TKL3_D2_PC)


#define UNUSED_ARGUMENT(x) (void)x


#define IS_DEC1_ENABLE(x)					(x&(1<<MODE_DEC1_ENABLE_BIT))
#define IS_DEC2_ENABLE(x)					(x&(1<<MODE_DEC2_ENABLE_BIT))
#define IS_EFF_ENABLE(x)					(x&(1<<MODE_EFF_ENABLE_BIT))
#define IS_APP_ENABLE(x)					(x&(1<<MODE_APP_ENABLE_BIT))
#define IS_ENC_ENABLE(x)					(x&(1<<MODE_ENC_ENABLE_BIT))
#define IS_MIC_ENABLE(x)					(x&(1<<MODE_MIC_ENABLE_BIT))
#define IS_AUXIN_ENABLE(x)					(x&(1<<MODE_AUXIN_ENABLE_BIT))
#define IS_SPDIFIN_ENABLE(x)				(x&(1<<MODE_SPDIFIN_ENABLE_BIT))


/******************************************************************************
        C Funciton Prototype
 *****************************************************************************/
/*!
  \brief  initial audio dsp system and run to MAIN_STATE_RUN state (mainprogram)
  \par    detail description:\n
          Initial audio dsp system will download dsp mainprogram and run to MAIN_STATE_RUN state.\n
          Before the audio dsp system is safe to download, it must be properly shutdown first\n
          And also, after mainprogram entering MAIN_STATE_IDLE state, it will get corresponding\n
          dsp dram layout from dsp parameters.\n
  \par    The mainprgram system downloading procedure:
          1. DSP_SHUTDOWN()\n
          2. DSP_RESET()\n
          3. Check and set if scramble is enabled\n
          4. dspDownloadCode():download dsp mainprogram (bank0) dsp code\n
          5. DSP_GO() and wait MAIN_STATE_READY state\n
          6. dspSetSystemIdle() and wait MAIN_STATE_IDLE state\n
          7. Get dsp dram memory layout information\n
          8. dspSetSystemRun() and wait MAIN_STATE_RUN state\n
  \return DSP_ERROR_NONE: succeed and system is running\n
          others: failed
  \note
 */
UINT32 DSP_InitSystem(void);


/*!
  \brief  initial dsp codecs and run to TASK_PLAY state (codec)
  \par    detail description:\n
          Initial codec (decoder&encoder) will download the desired dsp codecs\n
          into codec banks. Codec banks are actually hardware registers that audio \n
          hardware will make use of dsp code execution. They were allocateded enough\n
          dram resource while DSP_Initial() executed. For now, the predefined codec \n
          banks are as:\n
          bank1: primary decoder\n
          bank2: secondary decoder\n
          bank3: encoder\n
          (bank0 reserved for dsp mainprogram system, bank4-bank7 are reserved)\n
          During execution, the dsp code will be download into codec banks and dram\n
          romtable will be downloaded into codec working memory too. It should be noted \n
          that the desired codec is specified by risc parameters, not from input parameters.\n
  \par    codec download procedure:
           1. Check if task already enabled\n
           2. dspSetCodecStop() and wait TASK_PLAY state\n
           3. Reset bitstream buffer (if needed?)\n
           4. dspSetCodecReady() and wait TASK_READY state\n
           5. dspSetSystemIdle() and wait MAIN_STATE_IDLE state\n
           6. dspSetSystemReady() and wait MAIN_STATE_READY state (if dynamically dram allocation)\n
           7. dspDownloadCode(): download code \n
           8. dspDownloadData(): download dram romtable\n
           9. dspSetSystemIdle() and wait MAIN_STATE_IDLE state\n
           9. dspSetSystemRun() and wait MAIN_STATE_RUN state\n
          10. dspSetCodecStop() and wait TASK_STOP state\n
          11. dspSetCodecPlay() and wait TASK_PLAY state\n

  \param  codecBankID [IN]initial which codecBank, allowd enum value:\n
          CODEC_BANK_DEC1:Primary decoder\n
          CODEC_BANK_DEC2:Secondary decoder\n
          CODEC_BANK_ENC :Encoder\n
  \return DSP_ERROR_NONE: succeed and system is running\n
          others: failed
  \note   Tasks should be enabled first, or it will always failed.\n
          (Tasks are initialed by risc parameters: RISC_MAIN_MODE)
  \todo   1. check if reset bitstream buffer (moved into dspSetCodecStop()?)
          2. check if needs dynamically dram allocation according to different codec\n
             if yes, may need to set dspSetSystemReady()
 */
UINT32 DSP_InitCodec(UINT8 codecBankID);

/*!
  \brie   download dsp code and romtable into desird code banks
  \par    detail description:\n
  \param  CodecID    [IN]codec id
  \param  codeBankID [IN]code bank id
  \return DSP_ERROR_NONE: succeed and system is running\n
          others: failed
 */
UINT32 DSP_Download(UINT16 CodecID, UINT8 codeBankID);


/*!
  \brief  Get dsp system and codec states
  \par    detail description:\n
  \param  type[IN]1st input variable
          DSP_STATE_SYSTEM: Get mainprogram system state\n
	      DSP_STATE_DEC1: Get primary decoder (dec1) state\n
		  DSP_STATE_DEC2: Get secondary decoder (dec2) state\n
		  DSP_STATE_ENC: Get encoder state\n
  \return desired states
  \note
 */
UINT8 DSP_GetState(UINT8 type);

/*!
  \brief  wait dsp system an codec states
  \par    detail description:\n
  \param  type[IN]1st input variable
          DSP_STATE_SYSTEM: Get mainprogram system state\n
	      DSP_STATE_DEC1: Get primary decoder (dec1) state\n
		  DSP_STATE_DEC2: Get secondary decoder (dec2) state\n
		  DSP_STATE_ENC: Get encoder state\n
  \param  state[IN]state to wait\n
          valid states selection of DSP_STATE_SYSTEM\n
          MAIN_STATE_READY/MAIN_STATE_IDLE/MAIN_STATE_RUN\n
          valid states selection of DSP_STATE_DEC1/DSP_STATE_DEC2/DSP_STATE_ENC\n
          TASK_READY/TASK_STOP/TASK_PLAY/TASK_FF/TASK_PAUSE\n

  \return DSP_ERROR_NONE: succeed and system is running\n
          others: failed
  \note   always wait until state is reached
  \todo   add timeout or other mechanisms to avoid infinitely wait
 */
UINT32 DSP_WaitState(UINT8 type, UINT8 state);

/*!
  \brief  if dsp change state failed(timeout), set this flag to force reset dsp in audio MainRoutine
  \par    detail description:\n
  \param  value [IN]
  			1: would reset dsp\n
  			0: clean flag
  \return none
 */
void DSP_Set_Timeout_Flag( UINT16 value);

/*!
  \brief  Get this flag to check if need to force reset dsp or not
  \par    void
  \return flag_status
 */
UINT16 DSP_Get_Timeout_Flag(void);

/*!
  \brief  set dsp system state to MAIN_STATE_IDLE
  \par    detail description:\n
          1. Check current state, if already MAIN_STATE_IDLE, just leave\n
          2. Check valid state:MAIN_STATE_READY/MAIN_STATE_RUN\n
          3. Send idle command by DSP_SendCmd()\n
          4. wait dsp system run to MAIN_STATE_IDLE state\n
  \return DSP_ERROR_NONE: succeed and system is running\n
          others: failed
  \note   always wait until MAIN_STATE_IDLE state is reached
  \todo   add timeout or other mechanisms to avoid infinitely wait
 */
UINT32 DSP_SetSystemIdle(void);

/*!
  \brief  set dsp system state to MAIN_STATE_READY
  \par    detail description:\n
          1. Check current state, if already MAIN_STATE_IDLE, just leave\n
          2. Check valid state:MAIN_STATE_INIT/MAIN_STATE_IDLE\n
          3. Send ready command by DSP_SendCmd()\n
          4. wait dsp system run to MAIN_STATE_READY state\n
  \return DSP_ERROR_NONE: succeed and system is running\n
          others: failed
  \note   always wait until MAIN_STATE_READY state is reached
  \todo   add timeout or other mechanisms to avoid infinitely wait
 */
UINT32 DSP_SetSystemReady(void);

/*!
  \brief  set dsp system state to MAIN_STATE_RUN
  \par    detail description:\n
          1. Check current state, if already MAIN_STATE_RUN, just leave\n
          2. Check valid state:MAIN_STATE_IDLE\n
          3. Send run command by DSP_SendCmd()\n
          4. wait dsp system run to MAIN_STATE_RUN state\n
  \return DSP_ERROR_NONE: succeed and system is running\n
          others: failed
  \note   always wait until MAIN_STATE_RUN state is reached
  \todo   add timeout or other mechanisms to avoid infinitely wait
 */
UINT32 DSP_SetSystemRun(void);

/*!
  \brief  set dsp codec state to TASK_READY
  \par    detail description:\n
          1. Check current state, if already TASK_READY, just leave\n
          2. Check valid state:TASK_STOP/TASK_INIT\n
          3. Send ready command by DSP_SendCmd()\n
          4. wait dsp system run to TASK_READY state\n
  \param  codecBankID [IN]initial which codecBank, allowd enum value:\n
          CODEC_BANK_DEC1:Primary decoder\n
          CODEC_BANK_DEC2:Secondary decoder\n
          CODEC_BANK_ENC :Encoder\n
  \return DSP_ERROR_NONE: succeed and system is running\n
          others: failed
  \note   always wait until TASK_READY state is reached
  \todo   add timeout or other mechanisms to avoid infinitely wait
 */
UINT32 DSP_SetCodecReady(UINT8 codecBankID);

/*!
  \brief  set dsp codec state to TASK_STOP
  \par    detail description:\n
          1. Check current state, if already TASK_STOP, just leave\n
          2. Check valid state:TASK_READY/TASK_PLAY/TASK_FF/TASK_PAUSE\n
          3. Send stop command by DSP_SendCmd()\n
          4. wait dsp system run to TASK_STOP state\n
  \param  codecBankID [IN]initial which codecBank, allowd enum value:\n
          CODEC_BANK_DEC1:Primary decoder\n
          CODEC_BANK_DEC2:Secondary decoder\n
          CODEC_BANK_ENC :Encoder\n
  \return DSP_ERROR_NONE: succeed and system is running\n
          others: failed
  \note   always wait until TASK_STOP state is reached
  \todo   add timeout or other mechanisms to avoid infinitely wait
 */
UINT32 DSP_SetCodecStop(UINT8 codecBankID);

/*!
  \brief  set dsp codec state to TASK_PLAY
  \par    detail description:\n
          1. Check current state, if already TASK_PLAY, just leave\n
          2. Check valid state:TASK_STOP/TASK_FF/TASK_PAUSE\n
          3. Send play command by DSP_SendCmd()\n
          4. wait dsp system run to TASK_PLAY state\n
  \param  codecBankID [IN]initial which codecBank, allowd enum value:\n
          CODEC_BANK_DEC1:Primary decoder\n
          CODEC_BANK_DEC2:Secondary decoder\n
          CODEC_BANK_ENC :Encoder\n
  \return DSP_ERROR_NONE: succeed and system is running\n
          others: failed
  \note   always wait until TASK_PLAY state is reached
  \todo   add timeout or other mechanisms to avoid infinitely wait
 */
UINT32 DSP_SetCodecPlay(UINT8 codecBankID);

/*!
  \brief  set dsp codec state to TASK_PAUSE
  \par    detail description:\n
          1. Check current state, if already TASK_PAUSE, just leave\n
          2. Check valid state:TASK_STOP/TASK_FF/TASK_PLAY\n
          3. Send pause command by DSP_SendCmd()\n
          4. wait dsp system run to TASK_PAUSE state\n
  \param  codecBankID [IN]initial which codecBank, allowd enum value:\n
          CODEC_BANK_DEC1:Primary decoder\n
          CODEC_BANK_DEC2:Secondary decoder\n
          CODEC_BANK_ENC :Encoder\n
  \return DSP_ERROR_NONE: succeed and system is running\n
          others: failed
  \note   always wait until TASK_PAUSE state is reached
  \todo   add timeout or other mechanisms to avoid infinitely wait
 */
UINT32 DSP_SetCodecPause(UINT8 codecBankID);

/*!
  \brief  set dsp codec state to TASK_FF
  \par    detail description:\n
          1. Check current state, if already TASK_FF, just leave\n
          2. Check valid state:TASK_STOP/TASK_PAUSE/TASK_PLAY\n
          3. Send ff command by DSP_SendCmd()\n
          4. wait dsp system run to TASK_FF state\n
  \param  codecBankID [IN]initial which codecBank, allowd enum value:\n
          CODEC_BANK_DEC1:Primary decoder\n
          CODEC_BANK_DEC2:Secondary decoder\n
          CODEC_BANK_ENC :Encoder\n
  \return DSP_ERROR_NONE: succeed and system is running\n
          others: failed
  \note   always wait until TASK_FF state is reached
  \todo   add timeout or other mechanisms to avoid infinitely wait
 */
UINT32 DSP_SetCodecFastForward(UINT8 codecBankID,UINT16 ffratio);

UINT32 DSP_BsBufFeed(UINT8 bsBufID,UINT8* pBsSrcBuf, UINT32 aFeedSize);
UINT32 DSP_BsBufFeed_Cached(UINT8 bsBufID,UINT8* pBsSrcBuf, UINT32 aFeedSize);
UINT8* DSP_GetBsBufStart(UINT8 bsBufID);
UINT32 DSP_GetBsBufSize(UINT8 bsBufID);
UINT32 DSP_GetBsBufFree(UINT8 bsBufID);
UINT32 DSP_GetBsBufUsed(UINT8 bsBufID);
BsBufInfo* DSP_UpdateBsBufInfo(UINT8 bsBufID);
UINT32 DSP_SetDumpPtr(UINT8 bsBufID, UINT32 aDump);
UINT32 DSP_SetBarrierPtr(UINT8 bsBufID, UINT32 aBarrier);
UINT32 DSP_FlushBsBuf(BSID bsBufID);
UINT32 DSP_GetSampleRate( void );
void DSP_SetSampleRate( UINT32 val );

#ifdef ENABLE_DSP_INTERRUPT
cyg_uint32 dsp_int0_isr(cyg_vector_t vector, cyg_addrword_t data __UNUSED__);
void dsp_int0_dsr(cyg_vector_t vector __UNUSED__, cyg_ucount32 count __UNUSED__, cyg_addrword_t data __UNUSED__);
cyg_uint32 dsp_int1_isr(cyg_vector_t vector, cyg_addrword_t data __UNUSED__);
void dsp_int1_dsr(cyg_vector_t vector __UNUSED__, cyg_ucount32 count __UNUSED__, cyg_addrword_t data __UNUSED__);
#endif

void DSP_Waitloop(UINT32 nCount);
void DSP_Waitlock(UINT32 nCount);

UINT16 DSP_WaitEndingDone(UINT8 codecBankID);
UINT32 DSP_SetBsBufEnd(UINT8 codecBankID);

void DSP_PC_Status(void);
#endif //__DSP_H__
