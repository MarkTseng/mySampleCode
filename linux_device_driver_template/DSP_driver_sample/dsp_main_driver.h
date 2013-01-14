/* DEBUG message define */
#ifdef DEBUG
#define DSP_DBGMSG(string, args...) printk(KERN_ALERT "DSP: " string, ##args)
#else
#define DSP_DBGMSG(string, args...)
#endif

/* char device info */
#define DSP_MAJOR			42
#define DSP_DEVICE_NAME		"dsp"


/* define to use readb and writeb to read/write data */
#define DSP_BYTE_WISE



/*dsp device structure */
typedef struct DSP_DEV {
	void __iomem *virtual_mem_base;
	unsigned long virtual_size;	
	unsigned long phys_addr;
	struct task_struct *dsp_kthread_task; /* task struct for kthread. */
	struct sock *dsp_netlink_sock;
} __attribute__((packed)) DSP_DEV;

/*
 * Prototypes for shared functions
 */
int spv9200_dsp_hw_initial(void);
static void DSP_Codec_Status(void);

/*
 * Ioctl definitions
 */

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
}__attribute__((packed)) DSP_IOCTL_CMD;


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
		bool mute;
	}Set_MasterMuteStatus;

	struct { 
		bool mute;
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
#define DSP_AUDIF_Get_MainEqulizer			_IOR(DSP_IOC_MAGIC, 30, Aud_EqualizerConfig_t )
#define DSP_AUDIF_Get_MasterMuteStatus		_IOR(DSP_IOC_MAGIC, 31, DSP_IOCTL_CMD )
#define DSP_AUDIF_Get_Aux1MuteStatus		_IOR(DSP_IOC_MAGIC, 32, DSP_IOCTL_CMD )
#define DSP_AUDIF_Get_Aux2MuteStatus		_IOR(DSP_IOC_MAGIC, 33, DSP_IOCTL_CMD )
#define DSP_AUDIF_Get_DdpFmt				_IOR(DSP_IOC_MAGIC, 34, DSP_IOCTL_CMD )
#define DSP_AUDIF_TestTone_mode				_IOW(DSP_IOC_MAGIC, 35, DSP_IOCTL_CMD )
#define DSP_AUDIF_UnInit_SpdifIn			_IO(DSP_IOC_MAGIC, 36 )
#define DSP_AUDIF_Set_SampleRate			_IOW(DSP_IOC_MAGIC, 37, DSP_IOCTL_CMD )
#define DSP_AUDIF_Set_FIFOEnable			_IO(DSP_IOC_MAGIC, 38 )
#define DSP_AUDIF_Set_FIFOReset				_IO(DSP_IOC_MAGIC, 39 )
#define DSP_AUDIF_Demute					_IO(DSP_IOC_MAGIC, 40 )
#define DSP_AUDIF_Get_audBSbuf_start		_IOW(DSP_IOC_MAGIC, 41 ,DSP_IOCTL_CMD)
#define DSP_AUDIF_Get_audBSbuf_free			_IOWR(DSP_IOC_MAGIC, 42 ,DSP_IOCTL_CMD)
#define DSP_AUDIF_Get_audBSbuf_size			_IOWR(DSP_IOC_MAGIC, 43 ,DSP_IOCTL_CMD)
#define DSP_AUDIF_Set_Stop					_IOW(DSP_IOC_MAGIC, 44 ,DSP_IOCTL_CMD)
#define DSP_AUDIF_Set_Play					_IOW(DSP_IOC_MAGIC, 45 ,DSP_IOCTL_CMD)
#define DSP_AUDIF_Set_HDMI					_IOW(DSP_IOC_MAGIC, 46 ,DSP_IOCTL_CMD)
#define DSP_AUDIF_Set_Pause					_IOW(DSP_IOC_MAGIC, 47 ,DSP_IOCTL_CMD)
#define DSP_AUDIF_Get_BarrierPtr			_IOWR(DSP_IOC_MAGIC, 48 ,DSP_IOCTL_CMD)
#define DSP_AUDIF_Get_DumpPtr				_IOW(DSP_IOC_MAGIC, 49 ,DSP_IOCTL_CMD)
#define DSP_AUDIF_Set_DumpPtr				_IOW(DSP_IOC_MAGIC, 50 ,DSP_IOCTL_CMD)
#define DSP_AUDIF_Update_Dec1_SampleRate	_IO(DSP_IOC_MAGIC, 51 )
#define DSP_AUDIF_Clean_Audio_Buf			_IOW(DSP_IOC_MAGIC, 52 ,DSP_IOCTL_CMD)
#define DSP_AUDIF_Init_Wave_Audio			_IOW(DSP_IOC_MAGIC, 53 ,WAVFORMATMACRO)

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
#define DSP_TVFE_Audio_Set_AC3TransCoding		_IO(DSP_IOC_MAGIC, 175 )
#define DSP_TVFE_Audio_Initial					_IO(DSP_IOC_MAGIC, 176 )
#define DSP_TVFE_AudioPowerControl				_IOW(DSP_IOC_MAGIC, 177, TVFE_Audio_CMD_st )

#define DSP_TVFE_Audio_Set_SIF_Source				_IOW(DSP_IOC_MAGIC, 178, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_SIF_Connect			_IOW(DSP_IOC_MAGIC, 179, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Get_FrontEndParam		_IOR(DSP_IOC_MAGIC, 180, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_SIFHandler			_IOW(DSP_IOC_MAGIC, 181, TVFE_Audio_CMD_st )
#define DSP_TVFE_SetTunerValueType				_IOW(DSP_IOC_MAGIC, 182, TVFE_Audio_CMD_st )
#define DSP_TVFE_SIF_SetCurrentMode				_IOW(DSP_IOC_MAGIC, 183, TVFE_Audio_CMD_st )
#define DSP_TVFE_GetTunerValueType				_IOR(DSP_IOC_MAGIC, 184, TVFE_Audio_CMD_st )
#define DSP_TVFE_GetSIFWorkingMode				_IOR(DSP_IOC_MAGIC, 185, TVFE_Audio_CMD_st )
#define DSP_TVFE_SetSifAvailableMode			_IOW(DSP_IOC_MAGIC, 186, TVFE_Audio_CMD_st )
#define DSP_TVFE_GetSifAvailableMode			_IOR(DSP_IOC_MAGIC, 187, TVFE_Audio_CMD_st )
#define DSP_TVFE_Audio_Set_Sound_Source		_IOW(DSP_IOC_MAGIC, 188, TVFE_Audio_CMD_st )


