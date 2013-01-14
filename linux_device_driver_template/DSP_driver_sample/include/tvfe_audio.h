/*******************************************************************************
** tvfe_audio.h :  This file contains APIs which are used by Middleware Layer
**
**
**
**
** $Id: tvfe_audio.h   2011-02-28 $
*******************************************************************************/
//Leon
#if 1
#ifndef _TVFE_AUDIO_H_
#define _TVFE_AUDIO_H_


//#include "types.h"

#include <asm/mach-spv9200/regmap_9200.h>
#include <asm/mach-spv9200/spv9200.h>
#include <asm/mach-spv9200/types.h>
#include "auddrv.h"

#ifdef __cplusplus
extern "C" {
#endif


#define DEBUG_TVFE_AUDIO_DBG
#ifdef DEBUG_TVFE_AUDIO_DBG
#define TVFE_AUDIO_DBG(...)                  diag_printf(__VA_ARGS__)
#else
#define TVFE_AUDIO_DBG(...)                  //
#endif


/*****************************************************************************
** Enumeration defined here
*****************************************************************************/

typedef unsigned char       TVFE_BOOL;
#define TVFE_TRUE    1
#define TVFE_FALSE   0

typedef  void (*TVFE_Audio_MuteFunctionPtr)(TVFE_BOOL Enable);


typedef enum _TVFE_Audio_Output_e
{
    TVFE_AUDIO_OUTPUT_INVALID	= 0,
    TVFE_AUDIO_OUTPUT_DAC0,
    TVFE_AUDIO_OUTPUT_DAC1,
    TVFE_AUDIO_OUTPUT_DAC2,
    TVFE_AUDIO_OUTPUT_I2S0,
    TVFE_AUDIO_OUTPUT_I2S1,
    TVFE_AUDIO_OUTPUT_I2S2,
    TVFE_AUDIO_OUTPUT_SPDIF,
    TVFE_AUDIO_OUTPUT_NR
} TVFE_Audio_Output_e;

typedef enum _TVFE_Audio_ProcessPath_e
{
    TVFE_AUDIO_PATH_INVALID                  = 0,
    TVFE_AUDIO_MAIN_PATH,
    TVFE_AUDIO_AUXILIARY_1_PATH,
    TVFE_AUDIO_AUXILIARY_2_PATH,
	TVFE_AUDIO_PATH_NR
} TVFE_Audio_Process_Path_e;


typedef struct _TVFE_Audio_Output_Config_t
{
	TVFE_Audio_Output_e Output_Port;
	TVFE_Audio_Process_Path_e Process_Path;
	TVFE_BOOL	If_Channel_Swap;
} TVFE_Audio_Output_Config_t;

typedef enum _TVFE_Audio_OutputSelection_e
{
    TVFE_AUDIO_OUTPUT_CURRENT_SOURCE,
    TVFE_AUDIO_OUTPUT_ANALOG_TV,
    TVFE_AUDIO_OUTPUT_SPDIF_BYPASS,
    TVFE_AUDIO_OUTPUT_SELECTION_NR
} TVFE_Audio_Output_Selection_e;

typedef struct _TVFE_Audio_Output_Selection_t
{
	//TVFE_Audio_Output_e Output_Port;
	TVFE_Audio_Process_Path_e Process_Path;
	TVFE_Audio_Output_Selection_e Selection;
} TVFE_Audio_Output_Selection_t;

typedef enum _TVFE_Audio_Status_e
{
   /* common status values */
   TVFE_AUDIO_SUCCESS,	/* successful outcome */
   TVFE_AUDIO_ERR_INVALID_PARAM,	/* invalid parameter */
   TVFE_AUDIO_ERR_INVALID_OP,	/* requested operation is invalid */
   TVFE_AUDIO_ERR_MEMORY_ALLOC,	/* problem allocating memory */
   TVFE_AUDIO_ERR_BUFFER_UNDERFLOW,	/* not enough stream data is available to decode the next video frame */
   TVFE_AUDIO_ERR_RESOURCE_UNAVAILABLE,	/* hardware resource is in use or otherwise unavailable */
   TVFE_AUDIO_ERR_FAILURE,
   TVFE_AUDIO_WARN_NO_ACTION,	/* the function completed successfully, but no action was taken */
   TVFE_AUDIO_WARN_PARAM_CLIPPED,	/* the function completed successfully, though a parameter was clipped to within a valid range.*/
   TVFE_AUDIO_WARN_BUFFER_EMPTY,	/* buffer became empty before the requested amount of data could be read */
   TVFE_AUDIO_WARN_BUFFER_FULL,	/* buffer became full before the requested amount of data could be written */
} TVFE_Audio_Status_e;

typedef struct _TVFE_Audio_Equalizer_Config_t
{
    TVFE_BOOL Enable_Bypass;
    INT8 Band1_0100HZ;
    INT8 Band2_0300HZ;
    INT8 Band3_1000HZ;
    INT8 Band4_3000HZ;
    INT8 Band5_8000HZ;
} TVFE_Audio_Equalizer_Config_t;

typedef struct _TVFE_Audio_AUX_Volume_t
{
	TVFE_Audio_Process_Path_e Process_Path;
	INT32 Volume;
} TVFE_Audio_AUX_Volume_t;

typedef enum _TVFE_Audio_Effect_e
{
    TVFE_AUDIO_EFFECT_MODE_NORMAL								= 0,
    /*only one effect can be chosen at one time*/
	TVFE_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_XT_DPL,
    TVFE_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_XT_PASSIVE_MATRIX,
    TVFE_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_HD_DPL,
	TVFE_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_HD_PASSIVE_MATRIX,
    TVFE_AUDIO_EFFECT_MODE_NOISE_SEQUENCING,
} TVFE_Audio_Effect_e;

typedef enum _TVFE_Audio_AVLMode_e
{
    TVFE_AUDIO_AVL_MODE_OFF    = 0,
    TVFE_AUDIO_AVL_MODE_20MS,
    TVFE_AUDIO_AVL_MODE_02S,
    TVFE_AUDIO_AVL_MODE_04S,
    TVFE_AUDIO_AVL_MODE_08S,
    TVFE_AUDIO_AVL_MODE_16S,
} TVFE_Audio_AVL_Decay_Time_e;

typedef enum _TVFE_Audio_AVLReferenceLevel_e
{
    TVFE_AUDIO_AVL_REFERENCE_LEVEL_MINUS_06_DB	= 0,
    TVFE_AUDIO_AVL_REFERENCE_LEVEL_MINUS_08_DB,
    TVFE_AUDIO_AVL_REFERENCE_LEVEL_MINUS_10_DB,
    TVFE_AUDIO_AVL_REFERENCE_LEVEL_MINUS_12_DB,
    TVFE_AUDIO_AVL_REFERENCE_LEVEL_MINUS_14_DB,
    TVFE_AUDIO_AVL_REFERENCE_LEVEL_MINUS_16_DB,
    TVFE_AUDIO_AVL_REFERENCE_LEVEL_MINUS_18_DB,
    TVFE_AUDIO_AVL_REFERENCE_LEVEL_MINUS_20_DB,
    TVFE_AUDIO_AVL_REFERENCE_LEVEL_MINUS_22_DB,
    TVFE_AUDIO_AVL_REFERENCE_LEVEL_MINUS_24_DB,
    TVFE_AUDIO_AVL_REFERENCE_LEVEL_MINUS_26_DB,
    TVFE_AUDIO_AVL_REFERENCE_LEVEL_MINUS_28_DB,
    TVFE_AUDIO_AVL_REFERENCE_LEVEL_MINUS_30_DB,
    TVFE_AUDIO_AVL_REFERENCE_LEVEL_MINUS_32_DB,
    TVFE_AUDIO_AVL_REFERENCE_LEVEL_MINUS_34_DB,
    TVFE_AUDIO_AVL_REFERENCE_LEVEL_MINUS_36_DB,
} TVFE_Audio_AVL_Reference_Level_e;

typedef enum _TVFE_Audio_SPDIF_Output_Config_e
{
    TVFE_AUDIO_SPDIF_OFF	= 0,
    TVFE_AUDIO_SPDIF_PCM,
    TVFE_AUDIO_SPDIF_BITSTREAM,
    TVFE_AUDIO_SPDIF_REENC,
    TVFE_AUDIO_SPDIF_IECRX,
} TVFE_Audio_SPDIF_Output_Config_e;

typedef enum _TVFE_Audio_SPDIF_Output_SmpRate_Config_e
{
    TVFE_AUDIO_SPDIF_Output_SmpRate_48K = 0,
    TVFE_AUDIO_SPDIF_Output_SmpRate_32K,
    TVFE_AUDIO_SPDIF_Output_SmpRate_44K,
    TVFE_AUDIO_SPDIF_Output_SmpRate_96K,
    TVFE_AUDIO_SPDIF_Output_SmpRate_192K,
} TVFE_Audio_SPDIF_Output_SmpRate_Config_e;

// port from plf_av_source.h 
typedef enum {
	PLF_INPUT_TYPE_NULL      = 0,
	PLF_INPUT_PIN_TYPE_ATV_TUNER,
	PLF_INPUT_PIN_TYPE_CVBS,
	PLF_INPUT_PIN_TYPE_SVIDEO,
	PLF_INPUT_PIN_TYPE_YPBPR,
	PLF_INPUT_PIN_TYPE_RGB,
	PLF_INPUT_PIN_TYPE_HDMI,
	PLF_INPUT_PIN_TYPE_DVI,
	PLF_INPUT_PIN_TYPE_SCART_AV,
	PLF_INPUT_PIN_TYPE_SCART_SV,
	PLF_INPUT_PIN_TYPE_SCART_AV_RGB,
	PLF_INPUT_PIN_TYPE_DTV,
	PLF_INPUT_PIN_TYPE_STORAGE,
	PLF_INPUT_PIN_TYPE_CCIR656,
	PLF_INPUT_PIN_TYPE_VIF,
	PLF_INPUT_PIN_TYPE_SIF,
	PLF_INPUT_PIN_TYPE_AUDIO_ADC,
	PLF_INPUT_PIN_TYPE_I2S,
	PLF_INPUT_PIN_TYPE_SPDIF,
	PLF_INPUT_PIN_TYPE_MAX,
} PLF_AVType_e;

typedef enum {
	PLF_NULL_PIN        = 0,
	PLF_INPUT_PIN_CVBS0,

	PLF_INPUT_PIN_CVBS1,

	PLF_INPUT_PIN_CVBS2,

	PLF_INPUT_PIN_CVBS3,

	PLF_INPUT_PIN_CVBS4,

	PLF_INPUT_PIN_C1,

	PLF_INPUT_PIN_C2,

	PLF_INPUT_PIN_RIN,
	PLF_INPUT_PIN_GIN,
	PLF_INPUT_PIN_BIN,
	PLF_INPUT_PIN_ADC_SOG,

	PLF_INPUT_PIN_PR1,
	PLF_INPUT_PIN_Y1,
	PLF_INPUT_PIN_PB1,
	PLF_INPUT_PIN_ADC_SOY1,

	PLF_INPUT_PIN_PR2,
	PLF_INPUT_PIN_Y2,
	PLF_INPUT_PIN_PB2,
	PLF_INPUT_PIN_ADC_SOY2,

	PLF_INPUT_PIN_SARADC0,

	PLF_INPUT_PIN_SARADC1,

	PLF_INPUT_PIN_SARADC2,

	PLF_INPUT_PIN_HDMI0,

	PLF_INPUT_PIN_HDMI1,

	PLF_INPUT_PIN_HDMI2,

	PLF_INPUT_PIN_DTV, /*including parallel/Serial/VIF depends on tuner driver*/

	PLF_INPUT_PIN_STORAGE,

	PLF_INPUT_PIN_AUD_SIF,

	PLF_INPUT_PIN_AUD_ADC0,

	PLF_INPUT_PIN_AUD_ADC1,

	PLF_INPUT_PIN_AUD_ADC2,

	PLF_INPUT_PIN_AUD_ADC3,

	PLF_INPUT_PIN_AUD_HDMI0,

	PLF_INPUT_PIN_AUD_HDMI1,

	PLF_INPUT_PIN_AUD_HDMI2,

	PLF_INPUT_PIN_AUD_DTV,

	PLF_INPUT_PIN_AUD_STORAGE,

	PLF_INPUT_PIN_AUD_SPDIFIN,

	PLF_OUTPUT_PIN_AUD_ADC0,

	PLF_OUTPUT_PIN_AUD_ADC1,

	PLF_OUTPUT_PIN_AUD_ADC2,

	PLF_OUTPUT_PIN_AUD_SPDIFOUT,
} PLF_AVPin_e;

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
		PLF_AVPin_e sog_pin;
	} rgb;
	struct {
		PLF_AVPin_e y_pin;
		PLF_AVPin_e pb_pin;
		PLF_AVPin_e pr_pin;
		PLF_AVPin_e soy_pin;
	} ypbpr;
	struct {
		PLF_AVPin_e pin;
	} hdmi;
	struct {
		PLF_AVPin_e pin;
	} digital;
} PLF_VideoConf_st;

/*****************************************************************************
** Structure defined here
*****************************************************************************/
typedef struct _TVFE_Audio_Aux_Path_t
{
    AUDIF_Aux1Path_e Aux1Path;
    AUDIF_Aux2Path_e Aux2Path;
} TVFE_Audio_Aux_Path_t;

typedef struct _TVFE_Audio_Dac_Mapping_t
{
    AUDIF_DacMapping_t mapping;
} TVFE_Audio_Dac_Mapping_t;


typedef struct _TVFE_Audio_AVL_Config_t
{
    TVFE_Audio_AVL_Decay_Time_e Mode;
    TVFE_Audio_AVL_Reference_Level_e Level;
    TVFE_BOOL Enable_Weight_Filter;
    TVFE_BOOL Enable_Peak_Mode_With_ClipManagement;
    TVFE_BOOL Enable_AVL_Func;
} TVFE_Audio_AVL_Config_t;

typedef struct _TVFE_Audio_Mute_t
{
	TVFE_BOOL Enable;
	TVFE_Audio_Process_Path_e Process_Path;
	UINT8 Mute_Delay;	//wait for ramp dwon mute
	TVFE_Audio_MuteFunctionPtr	pExternal_Mute_Control;	//mute control for external device
} TVFE_Audio_Mute_t;	//audio DSP needs 76 ms to ramp down

typedef struct _TVFE_Audio_Level_Adjust_t
{
	INT8 DEC_Adjust_Level;		//applied for stereo or dual path
    INT8 Mono_Adjust_Level;	//applied for FM analog mono path
    INT8 NICAM_Adjust_Level;	//applied for NICAM path
    INT8 SAP_Adjust_Level;		//applied for SAP path
    INT8 ADC_Adjust_Level;		//applied for line-in ADC path
    INT8 Digital_Adjust_Level;	//applied for digital path
} TVFE_Audio_Level_Adjust_t;

typedef struct _TVFE_Audio_SIF_Over_Modulation_Config_t
{
	TVFE_BOOL Enable_Adapter;
	UINT8 Filter_BandWidth;
	UINT8 Level_Thershold;
	UINT8 Attenuation;
} TVFE_Audio_SIF_Over_Modulation_Config_t;

typedef struct _TVFE_Audio_SIF_Magnitude_Detect_Config_t
{
	INT8  Carrier1_Magnitude_Threshold;
	UINT8 MPX_Magnitude_Threshold_Low;
	UINT8 MPX_Magnitude_Threshold_Up;
	UINT8 SAP_Magnitude_Threshold_Low;
	UINT8 SAP_Magnitude_Threshold_Up;
	TVFE_BOOL	 Enable_Sub_Carrier_Detect;
	UINT8 EIAJ_Sub_Carrier_Magnitude_Threshold_Low;
	UINT8 EIAJ_Sub_Carrier_Magnitude_Threshold_Up;
} TVFE_Audio_SIF_Detect_Config_t;

typedef struct _TVFE_Audio_Lip_Sync_Config_t
{
    TVFE_BOOL Enable;
	UINT16 Delay;	//0(0 ms) ~ 63(252 ms),(1 step is 4 ms)
} TVFE_Audio_Lip_Sync_Config_t;

typedef enum _TVFE_Audio_Pcm_Output_Phase_e
{
    TVFE_AUDIO_OUTPUT_PHASE_0	= 0,
    TVFE_AUDIO_OUTPUT_PHASE_180 ,
} TVFE_Audio_Pcm_Output_Phase_e;

typedef struct _TVFE_Audio_Pcm_Output_Phase_Config_t
{
    TVFE_Audio_Pcm_Output_Phase_e main_phase;
    TVFE_Audio_Pcm_Output_Phase_e aux1_phase;
    TVFE_Audio_Pcm_Output_Phase_e aux2_phase;
} TVFE_Audio_Pcm_Output_Phase_Config_t;


typedef enum _TVFE_Audio_SUNSUR_Gain_e
{
	TVFE_Audio_SUNSUR_GAIN_0P0,
	TVFE_Audio_SUNSUR_GAIN_0P4,
	TVFE_Audio_SUNSUR_GAIN_0P5,
	TVFE_Audio_SUNSUR_GAIN_0P6,
	TVFE_Audio_SUNSUR_GAIN_0P7,
	TVFE_Audio_SUNSUR_GAIN_0P8,
	TVFE_Audio_SUNSUR_GAIN_0P9,
	TVFE_Audio_SUNSUR_GAIN_1P0,
} TVFE_Audio_SUNSUR_Gain_e;

typedef struct
{
    TVFE_Audio_SUNSUR_Gain_e ambiance_gain;
    TVFE_Audio_SUNSUR_Gain_e reverb_gain;
    TVFE_Audio_SUNSUR_Gain_e direct_gain;
} TVFE_Audio_SUNSUR_Param_t;


typedef enum _TVFE_Audio_SIFHandler_Status
{
	TVFE_Audio_SIFHandler_Busy,
	TVFE_Audio_SIFHandler_Ready,
} TVFE_Audio_SIFHandler_Status_e;

typedef enum _TVFE_Audio_WAVE_FORMAT_e
{
	TVFE_Audio_WAVE_FORMAT_DOLBY_DIGITAL_PLUS,
	TVFE_Audio_WAVE_FORMAT_DOLBY_AC3,
	TVFE_Audio_WAVE_FORMAT_DOLBY_NA,
} TVFE_Audio_WAVE_FORMAT_e;


typedef enum
{
    AUDIO_MANAGER_SHUT_DOWN      = 0,
    AUDIO_MANAGER_CONNECT_INITIAL,
    AUDIO_MANAGER_CONNECTING,
    AUDIO_MANAGER_CONNECT_READY,
    AUDIO_MANAGER_CONNECT_TIME_OUT,
    AUDIO_MANAGER_STATE_MAX,
} AudioManagerState_t;

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

// port from plf_av_source.h 

typedef struct {
	PLF_AVType_e 		eType;
	PLF_VideoConf_st 	video;
	PLF_AVPin_e 		eAudioPri;
} PLF_AVSourcePinConfig_st;


//void TVFE_Audio_Set_Output_Config(const TVFE_Audio_Output_Config_t *pOutput_Config);

TVFE_Audio_Status_e TVFE_AudioParamInit(void);
TVFE_Audio_Status_e TVFE_Audio_Set_Output_Config(const TVFE_Audio_Output_Config_t *pOutput_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_Output_Selection(TVFE_Audio_Output_Selection_t *pOutput_Selection,PLF_AVType_e eSrcVideoType);

TVFE_Audio_Status_e TVFE_Audio_Set_Volume(const INT16 *pTable, UINT8 Size, UINT8 Index, INT8 OffsetStep);
TVFE_Audio_Status_e TVFE_Audio_Set_Bass(const INT8 *pTable, UINT8 Size, UINT8 Index);
TVFE_Audio_Status_e TVFE_Audio_Set_Treble(const INT8 *pTable, UINT8 Size, UINT8 Index);
TVFE_Audio_Status_e TVFE_Audio_Set_Main_Equlizer_Config(const TVFE_Audio_Equalizer_Config_t *pTable, UINT8 Size, UINT8 Index);
TVFE_Audio_Status_e TVFE_Audio_Get_Main_Equlizer_Config(TVFE_Audio_Equalizer_Config_t *pEQ);
TVFE_Audio_Status_e TVFE_Audio_Set_Balance(const INT8 *pTable, UINT8 Size, UINT8 LeftIndex, UINT8 RightIndex);
TVFE_Audio_Status_e TVFE_Audio_Set_AUX_Volume(TVFE_Audio_AUX_Volume_t *pAUX_Volume_Config);

TVFE_Audio_Status_e TVFE_Audio_Set_Effect_Mode(TVFE_Audio_Effect_e Effect_Mode);
TVFE_Audio_Status_e TVFE_Audio_Set_AVL_Config(TVFE_Audio_AVL_Config_t *pAVL_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_Mute_Config(TVFE_Audio_Mute_t *pMute_Config);
TVFE_Audio_Status_e TVFE_Audio_Initial(void);
TVFE_Audio_Status_e TVFE_Audio_Set_Level_Adjust_Config(TVFE_Audio_Level_Adjust_t *pLevel_Adjust_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_SIF_Over_Modulation_Config(TVFE_Audio_SIF_Over_Modulation_Config_t *pOver_Modulation_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_SIF_Detect_Config(TVFE_Audio_SIF_Detect_Config_t *pDetect_Config);
TVFE_Audio_Status_e TVFE_Audio_SPDIF_Output_Config(TVFE_Audio_SPDIF_Output_Config_e SPDIF_Output_Config,TVFE_Audio_SPDIF_Output_SmpRate_Config_e SPDIF_SmpRate);
TVFE_Audio_Status_e TVFE_Audio_Set_AtvRamp(UINT8 ramp_flag);
TVFE_Audio_Status_e TVFE_Audio_Lip_Sync_Config(TVFE_Audio_Lip_Sync_Config_t Lip_Sync_Config);
TVFE_Audio_Status_e TVFE_Audio_PCM_Output_Phase_Config(TVFE_Audio_Pcm_Output_Phase_Config_t Pcm_Output_Phase);
TVFE_Audio_Status_e TVFE_Audio_Set_SUNSUR_Config(TVFE_Audio_SUNSUR_Param_t SUNSUR_Param);
TVFE_Audio_Status_e TVFE_Audio_Set_HEAAC_Output(UINT8 OnOff_flag);
TVFE_Audio_Status_e TVFE_Audio_Set_Spdif_ChannelStatus(UINT8 CGMS_type , UINT8 bit_res, UINT8 disc_type);
TVFE_Audio_Status_e TVFE_Audio_Set_MasterMuteStatus(bool mute);
TVFE_Audio_Status_e TVFE_Audio_Get_MasterMuteStatus(bool *mute);
TVFE_Audio_Status_e TVFE_Audio_Set_AC3TransCoding(void);
TVFE_Audio_Status_e TVFE_Audio_Set_SIFHandler(void);
TVFE_Audio_Status_e TVFE_Audio_Get_SIFHandler_Status(TVFE_Audio_SIFHandler_Status_e *SIF_Status);
TVFE_Audio_Status_e TVFE_Audio_Set_SourceType(UINT32 Param);
UINT16 TVFE_Audio_Get_DdpFmt(void);
TVFE_Status_e _AM_SIFHandler(void *pParam);
TVFE_Audio_Status_e TVFE_Audio_Set_SIF_Source(void);
TVFE_Audio_Status_e TVFE_Audio_Set_Sound_Source(PLF_AVSourcePinConfig_st AVSourcePinConfig);

// port from tvfe_api.c
TVFE_Status_e TVFE_AudioPowerControl(TVFE_AudioPowerContro_e ePowerControl);
TVFE_Status_e TVFE_AudioSetSIFMode(TVFE_AudioPath_e ePath,TVFE_AudioSIFMode_e eMode);
TVFE_Status_e TVFE_AudioSetIFMode(DRV_AudSRCPath_t eSRCPath, DRV_AudProcessPath_t eProcessPath, TVFE_AudioSIFMode_e eIFMode);
TVFE_Status_e TVFE_AudioSetLevelAdjust(TVFE_AudioLevelAdjust_st *pLevelAdjust_config);
TVFE_Status_e TVFE_AudioSetSIFConnect(TVFE_AudioSIFStandardMask_e eStandardMask);
TVFE_Status_e TVFE_AudioSetSIFDetectReset(void);
TVFE_Status_e TVFE_AudioSetSIFStandardMask(	TVFE_AudioSIFStandardMask_e eStandardMask);

// part of tvfe_tvdec.c
TVFE_Status_e TVFE_SSDInit(void);
TVFE_Status_e TVFE_SSDUninit(void);
TVFE_Status_e _tvfe_AudioGetSIFStandard(DRV_AudFrontEndReg_t *pRegStatus, TVFE_AudioSIFStandard_t *pRetStandrad);
TVFE_Status_e _tvfe_AudioGetSIFModeStatus(DRV_AudFrontEndReg_t *pRegStatus,TVFE_AudioSIFAvailableMode_e *pRetMode);
TVFE_Status_e TVFE_AudioGetFrontEndParam(TVFE_AudioFrontEndParam_t *pFrontEndParam);
TVFE_Status_e TVFE_SetTunerValueType(TVFE_SoundStd_e eSoundStdType);
TVFE_Status_e TVFE_GetTunerValueType(TVFE_SoundStd_e *peSoundStdType);
TVFE_Status_e TVFE_GetSIFWorkingMode(TVFE_AudioSIFMode_e *peCurrentMode);
TVFE_Status_e TVFE_SetSifAvailableMode(TVFE_AudioSIFAvailableMode_e eSIFAvailableMode);
TVFE_Status_e TVFE_GetSifAvailableMode(TVFE_AudioSIFAvailableMode_e *peSifAvaiMode);
TVFE_Status_e TVFE_SIF_SetCurrentMode(TVFE_AudioSIFMode_e eSifMode);


#ifdef __cplusplus
}
#endif
#endif /* _TVFE_Audio_H_ */
#endif
/*****************************************************************************
**
**
*****************************************************************************/
