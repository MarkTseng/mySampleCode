/*
 * Sunplus DSP ioctl command test program
 * Author : Mark Tseng
 * DOCUMENT: 
 *		1. Create dsp device node
 *		# mknod /dev/dsp c 42 0
 *		2. execute dsp_ioctl
 * $Id: dsp_ioctl.h,v 0.1 2011/11/21 Mark Tseng Exp $
 */

/********************
* define type 
*********************/

//----- Sample Rate -----//
#define FS_8K                   0x5000
#define FS_11K                  0x6000
#define FS_12K                  0x7000
#define FS_16K                  0x1000
#define FS_22K                  0x2000
#define FS_24K                  0x4000
#define FS_32K                  0x0001
#define FS_44K                  0x0002
#define FS_48K                  0x0004
#define FS_64K                  0x0010
#define FS_88K                  0x0020
#define FS_96K                  0x0040
#define FS_128K                 0x0100
#define FS_176K                 0x0200
#define FS_192K                 0x0400
#define FS_MAX                  0x8000


/********************
* enum type 
*********************/
typedef enum
{
	/* common status values */
	AUD_ERR_INVALID_PARAM        = -6,   /* invalid parameter */
	AUD_ERR_INVALID_OP           = -5,   /* requested operation is invalid */
	AUD_ERR_MEMORY_ALLOC         = -4,   /* problem allocating memory */
	AUD_ERR_BUFFER_UNDERFLOW     = -3,   /* not enough stream data is available to decode the next video frame */
	AUD_ERR_RESOURCE_UNAVAILABLE = -2,   /* hardware resource is in use or otherwise unavailable */
	AUD_ERR_FAILURE              = -1,

	AUD_SUCCESS                  =  0,	/* successful outcome */

	AUD_WARN_NO_ACTION           =  1,	/* the function completed successfully, but no action was taken */
	AUD_WARN_PARAM_CLIPPED       =  2,	/* the function completed successfully, though a parameter was clipped to within a valid range.*/
	AUD_WARN_BUFFER_EMPTY        =  3,	/* buffer became empty before the requested amount of data could be read */
	AUD_WARN_BUFFER_FULL         =  4,	/* buffer became full before the requested amount of data could be written */
} AUDIF_Status_t;


typedef enum _AUDIF_Status_e
{
	/* generic error codes */
	AUDIF_ERR_Download_Failed = -6,
	AUDIF_ERR_Unknown_CodecID = -5,
	AUDIF_ERR_UnUsed = -4,
	AUDIF_ERR_INVALID_PARAM = -3,
	AUDIF_ERR_FIFOSpmCntMode = -2,
	AUDIF_ERR_BankID = -1,
	
	AUDIF_SUCCESS = 0x0,
	AUDIF_FAILURE,
	AUDIF_TIMEOUT,

	AUDIF_ERR_INVALID_OP,
	AUDIF_ERR_MEMORY_ALLOC,
	AUDIF_ERR_INVALID_USED,
	AUDIF_ERR_INVALID_State,

	AUDIF_ERR_BSBUF_FEED,
	AUDIF_ERR_BSBUF_ID,
	AUDIF_ERR_BS_WAIT,

	AUDIF_Warning_NonSupport_HWRamp,
} AUDIF_Status_e;

typedef enum
{
	AUDIF_ADC_SOURCE_ADC0   = 0x0,  /*!scart*/
	AUDIF_ADC_SOURCE_ADC1   = 0x5,  /*!default*/
	AUDIF_ADC_SOURCE_ADC2   = 0xA,
	AUDIF_ADC_SOURCE_ADC3   = 0xF,
	AUDIF_ADC_SOURCE_MAX    = 0xF,
} AUDIF_AdcSourceSelect_e;

typedef enum
{
    AUDIF_MAIN_PATH_MIXER       = 0x0,
    AUDIF_MAIN_PATH_SIF         = 0x1,
    AUDIF_MAIN_PATH_AUX         = 0x2,    
} AUDIF_MainPath_e;

typedef enum
{
    AUDIF_AUX1_PATH_MIXER     = 0x0,
    AUDIF_AUX1_PATH_SIF       = 0x1,    
    AUDIF_AUX1_PATH_AUX       = 0x2,    
    AUDIF_AUX1_PATH_OFF       = 0x3,        
    AUDIF_AUX1_PATH_LFE       = 0x4,            
    AUDIF_AUX1_PATH_MAX       = 0x4,                
} AUDIF_Aux1Path_e;

typedef enum
{
    AUDIF_AUX2_PATH_MIXER     = 0x0,
    AUDIF_AUX2_PATH_SIF       = 0x1,    
    AUDIF_AUX2_PATH_AUX       = 0x2,    
    AUDIF_AUX2_PATH_OFF       = 0x3,        
    AUDIF_AUX2_PATH_MAX       = 0x3,                    
} AUDIF_Aux2Path_e;

typedef enum
{
    AUDIF_DAC_MAP_PCM0_L    = 0x0,
    AUDIF_DAC_MAP_PCM0_R    = 0x1,
    AUDIF_DAC_MAP_PCM1_L    = 0x2,
    AUDIF_DAC_MAP_PCM1_R    = 0x3,
    AUDIF_DAC_MAP_PCM2_L    = 0x4,
    AUDIF_DAC_MAP_PCM2_R    = 0x5,
    AUDIF_DAC_MAP_PCM_MASK  = 0x7,
} AUDIF_DacMapping_e;

typedef enum
{
	AUD_AUDIO_EFFECT_MODE_NORMAL = 0,
	/*only one effect can be chosen at one time*/
	AUD_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_XT_DPL,
	AUD_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_XT_PASSIVE_MATRIX,
	AUD_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_HD_DPL,
	AUD_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_HD_PASSIVE_MATRIX,
	AUD_AUDIO_EFFECT_MODE_NOISE_SEQUENCING,
} Aud_AudioEffectMode_t;

typedef enum _AUDIF_SUNSUR_Switch_e
{
	SUNSUR_OFF = 0x0,
	SUNSUR_ON 	= 0x1,
}AUDIF_SUNSUR_Switch_e;

typedef enum
{
    AUD_AUDIO_AVL_MODE_OFF    = 0,
    AUD_AUDIO_AVL_MODE_20MS,
    AUD_AUDIO_AVL_MODE_02S,
    AUD_AUDIO_AVL_MODE_04S,
    AUD_AUDIO_AVL_MODE_08S,
    AUD_AUDIO_AVL_MODE_16S,
} Aud_AudioAVLMode_t;

typedef enum _AUDIF_SUNSUR_Gain_e
{
	SUNSUR_GAIN_0P0,
	SUNSUR_GAIN_0P4,
	SUNSUR_GAIN_0P5,
	SUNSUR_GAIN_0P6,
	SUNSUR_GAIN_0P7,
	SUNSUR_GAIN_0P8,
	SUNSUR_GAIN_0P9,
	SUNSUR_GAIN_1P0
} AUDIF_SUNSUR_Gain_e;

// order is important!
// must match with typedef eSource_Type_t in customer.h
typedef enum {
	AUD_SOURCE_ATV = 0,
	AUD_SOURCE_DTV,
	AUD_SOURCE_RADIO,
	AUD_SOURCE_SCART,
	AUD_SOURCE_SCART1,
	AUD_SOURCE_AV,
	AUD_SOURCE_AV1,
	AUD_SOURCE_AV2,
	AUD_SOURCE_SVIDEO,
	AUD_SOURCE_SVIDEO1,
	AUD_SOURCE_SVIDEO2,
	AUD_SOURCE_YPBPR,
	AUD_SOURCE_YPBPR1,
	AUD_SOURCE_YPBPR2,
	AUD_SOURCE_HDMI,
	AUD_SOURCE_HDMI1,
	AUD_SOURCE_HDMI2,
	AUD_SOURCE_PC,
	AUD_SOURCE_MEDIA,
	AUD_SOURCE_KARAOKE,
	AUD_SOURCE_NET,
	AUD_SOURCE_DVD,
	AUD_SOURCE_MAX,
} AUDIF_Source_Type_e;

/**
*  @brief BTSC DBX mode selection
*/
typedef enum
{
    DRV_AUD_STEREO_MODE,
    DRV_AUD_SAP_MODE,
} DRV_AudBTSCDBXMode_t;

typedef enum
{
    DRV_AUD_SRC_1       = 0,
    DRV_AUD_SRC_2,
} DRV_AudSRCPath_t;

/**
*  @brief enum for SSD source channel value
*/
typedef enum
{
    DRV_AUD_SRC_DISABLED                    = 0,
    DRV_AUD_SRC_DEC,
    DRV_AUD_SRC_MONO_SAP,
    DRV_AUD_SRC_DIGITAL_AUDIO_INPUT,
} DRV_AudSRCSource_t;

/**
*  @brief enum for PROCESS_CHANNEL
*/
typedef enum
{
    DRV_AUD_PROCESS_PATH_MAIN            = 0,
    DRV_AUD_PROCESS_PATH_SUBWOOFER,
    DRV_AUD_PROCESS_PATH_CENTER,
    DRV_AUD_PROCESS_PATH_SURROUND,
    DRV_AUD_PROCESS_PATH_AUX1,
    DRV_AUD_PROCESS_PATH_AUX2,
    DRV_AUD_PROCESS_PATH_AUX3,
} DRV_AudProcessPath_t;

/**
*  @brief enum for SSD digital matrix
*/
typedef enum
{
    DRV_AUD_DIGITAL_MATRIX_STEREO_AUTO_OFF          = 0,
    DRV_AUD_DIGITAL_MATRIX_MONO_AUTO_OFF,
    DRV_AUD_DIGITAL_MATRIX_LANGUAGEA_AUTO_OFF,
    DRV_AUD_DIGITAL_MATRIX_LANGUAGEB_AUTO_OFF,
    DRV_AUD_DIGITAL_MATRIX_SWAP_AUTO_OFF,
    DRV_AUD_DIGITAL_MATRIX_UNUSED,
    DRV_AUD_DIGITAL_MATRIX_LANGUAGEA_AUTO_ON,
    DRV_AUD_DIGITAL_MATRIX_LANGUAGEB_AUTO_ON,
} DRV_AudDigitalMatrix_t;



/********************
* struct type 
*********************/
typedef struct
{
    UINT8 DacMapping_Data0_L;
    UINT8 DacMapping_Data0_R;
    UINT8 DacMapping_Data1_L;
    UINT8 DacMapping_Data1_R;
    UINT8 DacMapping_Data2_L;
    UINT8 DacMapping_Data2_R;
}AUDIF_DacMapping_t;

typedef struct
{
    Boolean fEnBypass;
    INT8 bMainEQ_Band1_0100HZ;
    INT8 bMainEQ_Band2_0300HZ;
    INT8 bMainEQ_Band3_1000HZ;
    INT8 bMainEQ_Band4_3000HZ;
    INT8 bMainEQ_Band5_8000HZ;
} Aud_EqualizerConfig_t;

typedef struct
{
    Aud_AudioAVLMode_t eMode;
    INT8 bReference_level;
    Boolean fEnWeightFilter;
    Boolean fEnPeakModeWithClipManagement;
	Boolean Enable_AVL_Func;
} Aud_AudioAVLConfig_t;

typedef struct
{
    AUDIF_SUNSUR_Gain_e ambiance_gain;
    AUDIF_SUNSUR_Gain_e reverb_gain;
    AUDIF_SUNSUR_Gain_e direct_gain;
} Aud_SUNSUR_Param_t;

/****************************************
* HIOCTL command frmo dsp_main_driver.h
****************************************/

/*dsp device ioctl command structure */
typedef struct DSP_IOCTL_CMD {
    int	argument_1;
    int	argument_2;
    int argument_3;
    int argument_4;
    int argument_5;
    int argument_6;
    int argument_7;
    int argument_8;
    int argument_9;
    int argument_10;
} __attribute__((packed)) DSP_IOCTL_CMD;

/* TVFE ioctl command structure */
typedef union TVFE_Audio_CMD {
	struct {
		TVFE_Audio_Output_Config_t Output_Config;
	}Set_Output_Config;
	
	struct {
		TVFE_Audio_Output_Selection_t Output_Selection;
		PLF_AVType_e eSrcVideoType;
	}Set_Output_Selection;

	struct { 
		INT16 Table[128];
		UINT8 Size;
		UINT8 Index;
		INT8 OffsetStep;
	}Set_Volume;
	
	struct { 
		INT8 Table[128];
		UINT8 Size;
		UINT8 Index;
	}Set_Bass;

	struct { 
		INT8 Table[128];
		UINT8 Size;
		UINT8 Index;
	}Set_Treble;

	struct { 
		TVFE_Audio_Equalizer_Config_t Table;
		UINT8 Size;
		UINT8 Index;
	}Set_Main_Equlizer_Config;	
	
	struct { 
		TVFE_Audio_Equalizer_Config_t EQ;
	}Get_Main_Equlizer_Config;

	struct { 
		INT8 Table[128];
		UINT8 Size;
		UINT8 LeftIndex;
		UINT8 RightIndex;
	}Set_Balance;
	
	struct { 
		TVFE_Audio_AUX_Volume_t AUX_Volume_Config;
	}Set_AUX_Volume;

	struct { 
		TVFE_Audio_Effect_e Effect_Mode;
	}Set_Effect_Mode;

	struct { 
		TVFE_Audio_AVL_Config_t AVL_Config;
	}Set_AVL_Config;	
	
    struct {
	          TVFE_Audio_Mute_t Mute_Config;
	}Set_Mute_Config;

	struct { 
		TVFE_Audio_Level_Adjust_t Level_Adjust_Config;
	}Set_Level_Adjust_Config;

	struct { 
		TVFE_Audio_SIF_Over_Modulation_Config_t Over_Modulation_Config;
	}Set_SIF_Over_Modulation_Config;
	
	struct { 
		TVFE_Audio_SIF_Detect_Config_t Detect_Config;
	}Set_SIF_Detect_Config;
	
	struct { 
		TVFE_Audio_SPDIF_Output_Config_e SPDIF_Output_Config;
		TVFE_Audio_SPDIF_Output_SmpRate_Config_e SPDIF_SmpRate;
	}SPDIF_Output_Config;	
	
	struct { 
		UINT8 ramp_flag;
	}Set_AtvRamp;	

	struct { 
		TVFE_Audio_Lip_Sync_Config_t Lip_Sync_Config;
	}Lip_Sync_Config;
	
	struct { 
		TVFE_Audio_Pcm_Output_Phase_Config_t Pcm_Output_Phase;
	}PCM_Output_Phase_Config;
	
	struct { 
		TVFE_Audio_SUNSUR_Param_t SUNSUR_Param;
	}Set_SUNSUR_Config;
	
	struct { 
		UINT8 OnOFF_flag;
	}Set_HEAAC_Output;
	
	struct { 
		UINT8 CGMS_type;
		UINT8 bit_res;
		UINT8 disc_type;
	}Set_Spdif_ChannelStatus;
	
	struct { 
		TVFE_BOOL mute;
	}Set_MasterMuteStatus;
	
	struct { 
		TVFE_BOOL mute;
	}Get_MasterMuteStatus;

	struct { 
		UINT32 Param;
	}Set_SourceType;	

	// tvfe_api.h
	struct {
		TVFE_AudioPowerContro_e ePowerControl;
	}PowerControl;

	struct {
		TVFE_AudioSIFStandardMask_e eSIFStandardMask;
	}SIFStandardMask;
	
	struct {
		TVFE_AudioFrontEndParam_t FrontEndParam;
	}Get_FrontEndParam;

	struct {
		TVFE_SoundStd_e eTunerSoundStd;
	}TunerSoundStd;

	struct {
		TVFE_AudioSIFMode_e eSIFMode;
	}SIFMode;
	
	struct {
		TVFE_AudioSIFAvailableMode_e eSIFAvailableMode;
	}SIFAvailableMode;

	struct {
		PLF_AVSourcePinConfig_st PLF_AVSourcePinConfig;
	}Set_PLF_AVSourcePinConfig;

}__attribute__((packed)) TVFE_Audio_CMD_st;

/********************
* TVFE HAL API 
*********************/
typedef struct TVFE_Operation
{
	TVFE_Audio_Status_e (*Audio_Set_Output_Config)			(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Set_Output_Selection)		(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Set_Volume)					(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Set_Bass)					(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Set_Treble)					(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Set_Main_Equlizer_Config)	(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Get_Main_Equlizer_Config)	(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Set_Balance)				(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Set_AUX_Volume)				(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Set_Effect_Mode)			(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Set_AVL_Config)				(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Set_Mute_Config)			(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Initial)					(int Aud_fd);
	TVFE_Audio_Status_e (*Audio_Set_Level_Adjust_Config)		(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Set_SIF_Over_Modulation_Config)	(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Set_SIF_Detect_Config)			(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_SPDIF_Output_Config)		(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Set_AtvRamp)				(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Lip_Sync_Config)			(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_PCM_Output_Phase_Config)	(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Set_SUNSUR_Config)			(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Set_HEAAC_Output)			(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Set_Spdif_ChannelStatus)	(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Set_MasterMuteStatus)		(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Get_MasterMuteStatus)		(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Set_AC3TransCoding)			(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
	TVFE_Audio_Status_e (*Audio_Set_SourceType)				(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
}TVFE_Operation_st;

#define DSP_IOC_MAXNR 200

/* Use 'D' as magic number */
#define DSP_IOC_MAGIC  'D'

#define DSP_AUDIF_Set_MasterVolume	 		_IOW(DSP_IOC_MAGIC, 0, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_MainChannelOutput 	_IOW(DSP_IOC_MAGIC, 1, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_AuxChannelOutput 		_IOW(DSP_IOC_MAGIC, 2, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_AdcSourceSelect		_IOW(DSP_IOC_MAGIC, 3, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_Bass					_IOW(DSP_IOC_MAGIC, 4, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_Treble 				_IOW(DSP_IOC_MAGIC, 5, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_MainEqulizer			_IOW(DSP_IOC_MAGIC, 6, Aud_EqualizerConfig_t )
#define DSP_SET_MUTE 						_IOW(DSP_IOC_MAGIC, 7, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_Balance 				_IOW(DSP_IOC_MAGIC, 9, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_ChannelMap			_IOW(DSP_IOC_MAGIC, 10, AUDIF_DacMapping_t )
#define DSP_AUDIF_Set_Aux1Volume 			_IOW(DSP_IOC_MAGIC, 11, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_Aux2Volume 			_IOW(DSP_IOC_MAGIC, 12, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_EffectMode 			_IOW(DSP_IOC_MAGIC, 13, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_SUNSUR_OnOff 			_IOW(DSP_IOC_MAGIC, 14, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_AVL		 			_IOW(DSP_IOC_MAGIC, 15, Aud_AudioAVLConfig_t )
#define DSP_AUDIF_Set_MasterMuteStatus		_IOW(DSP_IOC_MAGIC, 16, DSP_IOCTL_CMD )
#define DSP_AUDIF_RampUp_PCM_Out			 _IO(DSP_IOC_MAGIC, 17)
#define DSP_AUDIF_Set_Aux1MuteStatus		_IOW(DSP_IOC_MAGIC, 18, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_Aux2MuteStatus		_IOW(DSP_IOC_MAGIC, 19, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_SPDIF					_IOW(DSP_IOC_MAGIC, 20, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_SPDIF_SmpRate			_IOW(DSP_IOC_MAGIC, 21, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_AtvRamp				_IOW(DSP_IOC_MAGIC, 22, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_LipSync_DelayTime		_IOW(DSP_IOC_MAGIC, 23, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_PcmOutputPhase		_IOW(DSP_IOC_MAGIC, 24, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_SUNSUR_Param			_IOW(DSP_IOC_MAGIC, 25, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_Decoder_OPT			_IOW(DSP_IOC_MAGIC, 26, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_Spdif_ChannelStatus	_IOW(DSP_IOC_MAGIC, 27, DSP_IOCTL_CMD )
#define DSP_AUDIF_Enable_AC3TransCoding		_IO(DSP_IOC_MAGIC, 28 )
#define DSP_AUDIF_Set_SourceType			_IOW(DSP_IOC_MAGIC, 29, DSP_IOCTL_CMD )
#define DSP_AUDIF_Get_MainEqulizer          _IOR(DSP_IOC_MAGIC, 30, Aud_EqualizerConfig_t )
#define DSP_AUDIF_Get_MasterMuteStatus      _IOR(DSP_IOC_MAGIC, 31, DSP_IOCTL_CMD )
#define DSP_AUDIF_Get_Aux1MuteStatus        _IOR(DSP_IOC_MAGIC, 32, DSP_IOCTL_CMD )
#define DSP_AUDIF_Get_Aux2MuteStatus        _IOR(DSP_IOC_MAGIC, 33, DSP_IOCTL_CMD )
#define DSP_AUDIF_Get_DdpFmt                _IOR(DSP_IOC_MAGIC, 34, DSP_IOCTL_CMD )
#define DSP_AUDIF_TestTone_mode             _IOW(DSP_IOC_MAGIC, 35, DSP_IOCTL_CMD )
#define DSP_AUDIF_UnInit_SpdifIn            _IO(DSP_IOC_MAGIC, 36 )
#define DSP_AUDIF_Set_SampleRate            _IOW(DSP_IOC_MAGIC, 37, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_FIFOEnable            _IO(DSP_IOC_MAGIC, 38 )
#define DSP_AUDIF_Set_FIFOReset             _IO(DSP_IOC_MAGIC, 39 )
#define DSP_AUDIF_Demute                    _IO(DSP_IOC_MAGIC, 40 )
#define DSP_AUDIF_Get_audBSbuf_start        _IOW(DSP_IOC_MAGIC, 41 ,DSP_IOCTL_CMD)
#define DSP_AUDIF_Get_audBSbuf_free         _IOWR(DSP_IOC_MAGIC, 42 ,DSP_IOCTL_CMD)
#define DSP_AUDIF_Get_audBSbuf_size         _IOWR(DSP_IOC_MAGIC, 43 ,DSP_IOCTL_CMD)
#define DSP_AUDIF_Set_Stop                  _IOW(DSP_IOC_MAGIC, 44 ,DSP_IOCTL_CMD)
#define DSP_AUDIF_Set_Play                  _IOW(DSP_IOC_MAGIC, 45 ,DSP_IOCTL_CMD)
#define DSP_AUDIF_Set_HDMI                  _IOW(DSP_IOC_MAGIC, 46 ,DSP_IOCTL_CMD)
#define DSP_AUDIF_Set_Pause                 _IOW(DSP_IOC_MAGIC, 47 ,DSP_IOCTL_CMD)
#define DSP_AUDIF_Get_BarrierPtr            _IOWR(DSP_IOC_MAGIC, 48 ,DSP_IOCTL_CMD)
#define DSP_AUDIF_Get_DumpPtr               _IOW(DSP_IOC_MAGIC, 49 ,DSP_IOCTL_CMD)
#define DSP_AUDIF_Set_DumpPtr               _IOW(DSP_IOC_MAGIC, 50 ,DSP_IOCTL_CMD)
#define DSP_AUDIF_Update_Dec1_SampleRate    _IO(DSP_IOC_MAGIC, 51 )
#define DSP_AUDIF_Clean_Audio_Buf           _IOW(DSP_IOC_MAGIC, 52 ,DSP_IOCTL_CMD)
#define DSP_AUDIF_Init_Wave_Audio           _IOW(DSP_IOC_MAGIC, 53 ,WAVFORMATMACRO)


/* SSD command */
#define SSD_DRV_AudSetSIFADCPGAGain		_IOW(DSP_IOC_MAGIC, 80, DSP_IOCTL_CMD )
#define SSD_DRV_AudEnableSSDROMReg		_IOW(DSP_IOC_MAGIC, 81, DSP_IOCTL_CMD )
#define SSD_DRV_AudSetBTSCDBXMode		_IOW(DSP_IOC_MAGIC, 82, DSP_IOCTL_CMD )
#define SSD_DRV_AudSetSRCPath			_IOW(DSP_IOC_MAGIC, 83, DSP_IOCTL_CMD )
#define SSD_DRV_AudSetDigitalMatrix					_IOW(DSP_IOC_MAGIC, 84, DSP_IOCTL_CMD )
#define SSD_DRV_AudSetPathAdjustLevel				_IOW(DSP_IOC_MAGIC, 85, DSP_IOCTL_CMD )
#define SSD_DRV_AudSetOverModulationAdapterConfig	_IOW(DSP_IOC_MAGIC, 86, DSP_IOCTL_CMD )
#define SSD_DRV_AudSetMagnitudeDetectConfig			_IOW(DSP_IOC_MAGIC, 87, DSP_IOCTL_CMD )

/* TVFE command */
#define DSP_TVFE_Audio_Set_Output_Config		_IOW(DSP_IOC_MAGIC, 150, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_Output_Selection		_IOW(DSP_IOC_MAGIC, 151, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_Effect_Mode			_IOW(DSP_IOC_MAGIC, 152, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_Volume				_IOW(DSP_IOC_MAGIC, 153, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_Bass					_IOW(DSP_IOC_MAGIC, 154, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_Treble				_IOW(DSP_IOC_MAGIC, 155, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_Main_Equlizer_Config	_IOW(DSP_IOC_MAGIC, 156, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Get_Main_Equlizer_Config	_IOR(DSP_IOC_MAGIC, 157, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_Balance				_IOW(DSP_IOC_MAGIC, 158, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_AUX_Volume			_IOW(DSP_IOC_MAGIC, 159, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_AVL_Config			_IOW(DSP_IOC_MAGIC, 160, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_Mute_Config			_IOW(DSP_IOC_MAGIC, 161, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_Level_Adjust_Config	_IOW(DSP_IOC_MAGIC, 162, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_SIF_Over_Modulation_Config	_IOW(DSP_IOC_MAGIC, 163, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_SIF_Detect_Config	_IOW(DSP_IOC_MAGIC, 164, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_SPDIF_Output_Config		_IOW(DSP_IOC_MAGIC, 165, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_AtvRamp				_IOW(DSP_IOC_MAGIC, 166, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Lip_Sync_Config			_IOW(DSP_IOC_MAGIC, 167, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_PCM_Output_Phase_Config	_IOW(DSP_IOC_MAGIC, 168, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_SUNSUR_Config		_IOW(DSP_IOC_MAGIC, 169, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_HEAAC_Output			_IOW(DSP_IOC_MAGIC, 170, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_Spdif_ChannelStatus	_IOW(DSP_IOC_MAGIC, 171, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_MasterMuteStatus		_IOW(DSP_IOC_MAGIC, 172, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_SourceType			_IOW(DSP_IOC_MAGIC, 173, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Get_MasterMuteStatus		_IOR(DSP_IOC_MAGIC, 174, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_AC3TransCoding       _IO(DSP_IOC_MAGIC, 175 )
#define DSP_TVFE_Audio_Initial                  _IO(DSP_IOC_MAGIC, 176 )
#define DSP_TVFE_AudioPowerControl              _IOW(DSP_IOC_MAGIC, 177, TVFE_Audio_CMD_st )

#define DSP_TVFE_Audio_Set_SIF_Source				_IOW(DSP_IOC_MAGIC, 178, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_SIF_Connect			_IOW(DSP_IOC_MAGIC, 179, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Get_FrontEndParam		_IOR(DSP_IOC_MAGIC, 180, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_SIFHandler			_IOW(DSP_IOC_MAGIC, 181, TVFE_Audio_CMD_st )
#define DSP_TVFE_SetTunerValueType				_IOW(DSP_IOC_MAGIC, 182, TVFE_Audio_CMD_st )
#define DSP_TVFE_SIF_SetCurrentMode			_IOW(DSP_IOC_MAGIC, 183, TVFE_Audio_CMD_st )
#define DSP_TVFE_GetTunerValueType				_IOR(DSP_IOC_MAGIC, 184, TVFE_Audio_CMD_st )
#define DSP_TVFE_GetSIFWorkingMode			_IOR(DSP_IOC_MAGIC, 185, TVFE_Audio_CMD_st )
#define DSP_TVFE_SetSifAvailableMode			_IOW(DSP_IOC_MAGIC, 186, TVFE_Audio_CMD_st )
#define DSP_TVFE_GetSifAvailableMode			_IOR(DSP_IOC_MAGIC, 187, TVFE_Audio_CMD_st )

#define DSP_TVFE_Audio_Set_Sound_Source		_IOW(DSP_IOC_MAGIC, 188, TVFE_Audio_CMD_st )


