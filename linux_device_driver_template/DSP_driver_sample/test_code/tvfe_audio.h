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


