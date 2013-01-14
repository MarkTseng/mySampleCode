#ifndef _PLAT_REG_H_
	#define _PLAT_REG_H_

#ifdef PLATFORM_REGISTER_IMPLEMENTATION
	#define PLAT_REG_EXTERN
#else
	#define PLAT_REG_EXTERN extern
	extern UINT32 gaPSReg[];
#endif

typedef enum
{
	////Setup Menu Entries
	PSREG_SETUP_TV_System=0,
	PSREG_SETUP_Downmix,
	PSREG_SETUP_Country_Code,
	PSREG_SETUP_DVD_Rating,
	PSREG_SETUP_Password,
	PSREG_SETUP_Aspect_Ratio,
	PSREG_SETUP_Display_Mode,
	PSREG_SETUP_Audio_Config,
	PSREG_SETUP_MenuLang,
	PSREG_SETUP_DiscAudio,
	PSREG_SETUP_DiscSubtitle,
	PSREG_SETUP_DiscMenu,

	PSREG_SETUP_Output_Component,
	PSREG_SETUP_YCbCrHDMI_Resolution,
	PSREG_SETUP_HDMI_1080p_24Hz,
	PSREG_SETUP_HDMI_Color_Setting,
	PSREG_SETUP_HDMI_Deep_Color,
	PSREG_SETUP_Pic_Full_Color_Range,

	PSREG_SETUP_Standby_Mode,
	PSREG_SETUP_AVC_Menu,
	PSREG_SETUP_Resume,
	PSREG_SETUP_Screen_Saver, //enable/disable screen saver by setup menu
	PSREG_SETUP_Screen_Fit,
	PSREG_SETUP_Skin,
	PSREG_SETUP_Dhcp,

	PSREG_SETUP_Region_Code,
	////Content Related
	PSREG_CONTENT_SUPPORT_XVCOLOR,		// Not Ready, Raymond 2010.08.11 Check , Not use so far
	////Peripheral Related
	PSREG_PERI_LoaderPhysicalFormat = 48,	//Ready.Raymond 2010.08.11 Check
	PSREG_PERI_UsbDevInserted,			// Not Ready, Raymond 2010.08.11 Check , Not use so far
	PSREG_PERI_CardDevInserted,			// Not Ready, Raymond 2010.08.11 Check , Not use so far

	////System Status Related
	PSREG_STAT_PlayMode = 64,			//Ready.Raymond 2010.08.11 Check
	PSREG_STAT_LOGICAL_DISCTYPE,		//Ready.Raymond 2010.08.11 Check
	PSREG_STAT_VideoFormat,			//Ready.Raymond 2010.08.11 Check
	PSREG_STAT_VideoFrameRate,		// Not Ready, Raymond 2010.08.11 Check
	PSREG_STAT_VideoAspectRatio,		// Not Ready, Raymond 2010.08.11 Check
	PSREG_STAT_AudioFormat,			//Ready.Raymond 2010.08.11 Check
	PSREG_STAT_AudioChn,				// Not Ready, Raymond 2010.08.11 Check
	PSREG_STAT_AudioVolumn,			//Ready.Raymond 2010.08.11 Check
	PSREG_STAT_AudioMuteCtrl,			//Ready.Raymond 2010.08.11 Check
	PSREG_STAT_ScrnSaverOn, //screen saver status now
	PSREG_STAT_ActiveSysApp,
	PSREG_STAT_MainAppStatus,
	PSREG_STAT_ShowScreenSaver,
	PSREG_STAT_IP,
	PSREG_STAT_NETIF,
	PSREG_SMART_UPGRADE,
	PSREG_FILE_LOADER, // 	SUPPORT_ISO_IMAGE
	PSREG_FileContainerDiscNav_TYPE,		//Ready.Raymond 2010.08.11 Check
	PSREG_BITRATE_Kbps,					//Ready.Raymond 2010.08.11 Check
	PSREG_STAT_VideoSrcHorizontalSize,		// Not Ready, Raymond 2010.08.11 Check
	PSREG_STAT_VideoSrcVerticalSize,		// Not Ready, Raymond 2010.08.11 Check
	PSREG_STAT_AudioSampleRate,			// Not Ready, Raymond 2010.08.12 Check
	PSREG_STAT_AudioBitPerSample,			// Not Ready, Raymond 2010.08.12 Check
	PSREG_STAT_DiscUniqueID,	//Raymond Add
        PSREG_STAT_CurPlayBackDeviceType,	//Raymond Add
       PSREG_STAT_RunAACSCertification,//Raymond Add
       PSREG_STAT_DiscLayer,

	PSREG_STAT_SVM_DiscVolumeID,
	PSREG_STAT_SVM_DiscSerialNum,
	//////////////////////////////////////

	PSREG_STAT_WIFI1_Dhcp,
	PSREG_STAT_WIFI2_Dhcp,
	PSREG_STAT_WIFI3_Dhcp,

	PSREG_STAT_ApplicationMode,	//Spike Add

#if 1//(SUPPORT_CEC_TUNER == 1)
	PSREG_TUNER_SIGNAL_STATUS,
	PSREG_SERVICE_INFO_NET_ID,
	PSREG_SERVICE_INFO_SERVICE_ID,
	PSREG_SERVICE_INFO_TS_ID,
#endif
	PSREG_STAT_DTVSource,
	PSREG_STAT_IrKeyRepeated,        // the lated IR repeated key
	PSREG_STAT_Language,
	PSREG_MaximumIndex
}PSRegIndex_e;


//Status value define
#include "status_reg_def.h"


//Return Value of the Interface
#define PSREG_MODULE_ALREADY_INIT	(1)
#define PSREG_SUCCESS 				(0)
#define PSREG_MODULE_NOT_INIT		(-1)
#define PSREG_INTERNAL_ERR			(-2)
#define PSREG_INDEX_ERR			(-3)
#define PSREG_MODULE_BUSY			(-4)
#define PSREG_PARAMETER_ERROR		(-5)

//Direct Use
#define PSREG_FULL(x) 			(gaPSReg[x])

//For Retrievers
#define PSREG_VALUE(x)			(PSREG_FULL(x)&PSREG_VALUE_MASK)
#define PSREG_IS_INVALID(x)		(PSREG_FULL(x)&PSREG_INVALID)
#define PSREG_IS_VALID(x)		(!(PSREG_IS_INVALID(x)))

//For Value Maintainers
#define PSREG_SET_VALID(x)		(PSREG_FULL(x)&=PSREG_VALUE_MASK)
#define PSREG_SET_INVALID(x)	(PSREG_FULL(x)|=PSREG_INVALID)
#define PSREG_SET_VALUE(x,y)	(PSREG_FULL(x)=(y&PSREG_VALUE_MASK)|PSREG_IS_INVALID(x))

#define PSREG_SVM_DISC_VOLUME_SERIAL_LEN	(16)

PLAT_REG_EXTERN int PLAT_REG_Init(void);
PLAT_REG_EXTERN int PLAT_REG_Finalize(void);

//For Non Release Module Use
PLAT_REG_EXTERN int PLAT_REG_SetPlaymode(UINT32 dValue);
PLAT_REG_EXTERN int PLAT_REG_SetPlaymodeValid(void);
PLAT_REG_EXTERN int PLAT_REG_SetPlaymodeInvalid(void);

PLAT_REG_EXTERN int PLAT_REG_SetVideoFormat(UINT32 dValue);
PLAT_REG_EXTERN int PLAT_REG_SetVideoFormatValid(void);
PLAT_REG_EXTERN int PLAT_REG_SetVideoFormatInvalid(void);

PLAT_REG_EXTERN int PLAT_REG_SetVideoFrameRate(UINT32 dValue);
PLAT_REG_EXTERN int PLAT_REG_SetVideoFrameRateValid(void);
PLAT_REG_EXTERN int PLAT_REG_SetVideoFrameRateInvalid(void);

PLAT_REG_EXTERN int PLAT_REG_SetVideoAspectRatio(UINT32 dValue);
PLAT_REG_EXTERN int PLAT_REG_SetVideoAspectRatioValid(void);
PLAT_REG_EXTERN int PLAT_REG_SetVideoAspectRatioInvalid(void);

PLAT_REG_EXTERN int PLAT_REG_SetAudioFormat(UINT32 dValue);
PLAT_REG_EXTERN int PLAT_REG_SetAudioFormatValid(void);
PLAT_REG_EXTERN int PLAT_REG_SetAudioFormatInvalid(void);

PLAT_REG_EXTERN int PLAT_REG_SetAudioChn(UINT32 dValue);
PLAT_REG_EXTERN int PLAT_REG_GetAudioChn(UINT32 *dValue);
PLAT_REG_EXTERN int PLAT_REG_SetAudioChnValid(void);
PLAT_REG_EXTERN int PLAT_REG_SetAudioChnInvalid(void);

PLAT_REG_EXTERN int PLAT_REG_SetAudioVolumn(UINT32 dValue);
PLAT_REG_EXTERN int PLAT_REG_SetAudioVolumnValid(void);
PLAT_REG_EXTERN int PLAT_REG_SetAudioVolumnInvalid(void);

PLAT_REG_EXTERN int PLAT_REG_SetAudioMuteCtrl(UINT32 MuteID);
PLAT_REG_EXTERN int PLAT_REG_SetAudioMuteCtrlValid(void);
PLAT_REG_EXTERN int PLAT_REG_SetAudioMuteCtrlInvalid(void);
//mute_enable : mute or demute option
//MuteID : choose the master ID to execute mute/demute control
//the related define is listed in status_reg_def.h
PLAT_REG_EXTERN int PLAT_REG_AddAudioMuteCtrl(UINT32 mute_enable ,UINT32 MuteID);
//*mute_status :  the mute status of whole system
PLAT_REG_EXTERN int PLAT_REG_GetAudioMuteStatus(UINT32 *mute_status);
PLAT_REG_EXTERN int PLAT_REG_GetAudio_MuteID_Status(UINT32 MuteID , UINT32 *mute_status);


PLAT_REG_EXTERN int PLAT_REG_SetFileContainerDiscNavType(UINT32 dValue);
PLAT_REG_EXTERN int PLAT_REG_SetFileContainerDiscNavTypeValid(void);
PLAT_REG_EXTERN int PLAT_REG_SetFileContainerDiscNavTypeInvalid(void);

PLAT_REG_EXTERN int PLAT_REG_SetBitRateKbps(UINT32 dKbps);
PLAT_REG_EXTERN int PLAT_REG_SetBitRateKbpsValid(void);
PLAT_REG_EXTERN int PLAT_REG_SetBitRateKbpsInvalid(void);


PLAT_REG_EXTERN int PLAT_REG_SetVideoSrcHorizontalSize(UINT32 dPixelNumber);
PLAT_REG_EXTERN int PLAT_REG_SetVideoSrcHorizontalSizeValid(void);
PLAT_REG_EXTERN int PLAT_REG_SetVideoSrcHorizontalSizeInvalid(void);


PLAT_REG_EXTERN int PLAT_REG_SetVideoSrcVerticalSize(UINT32 dPixelNumber);
PLAT_REG_EXTERN int PLAT_REG_SetVideoSrcVerticalSizeValid(void);
PLAT_REG_EXTERN int PLAT_REG_SetVideoSrcVerticalSizeInvalid(void);

PLAT_REG_EXTERN int PLAT_REG_SetUsbDevStatus(UINT32 dValue);
PLAT_REG_EXTERN int PLAT_REG_SetUsbDevStatusValid(void);
PLAT_REG_EXTERN int PLAT_REG_SetUsbDevStatusInvalid(void);
PLAT_REG_EXTERN int PLAT_REG_SetCardDevStatus(UINT32 dValue);
PLAT_REG_EXTERN int PLAT_REG_SetCardDevStatusValid(void);
PLAT_REG_EXTERN int PLAT_REG_SetCardDevStatusInvalid(void);

PLAT_REG_EXTERN int PLAT_REG_SetAudioSampleRate(UINT32 dHzNumber);
PLAT_REG_EXTERN int PLAT_REG_SetAudioSampleRateValid(void);
PLAT_REG_EXTERN int PLAT_REG_SetAudioSampleRateInvalid(void);

PLAT_REG_EXTERN int PLAT_REG_SetAudioBitPerSample(UINT32 dBitNumber);
PLAT_REG_EXTERN int PLAT_REG_SetAudioBitPerSampleValid(void);
PLAT_REG_EXTERN int PLAT_REG_SetAudioBitPerSampleInvalid(void);

PLAT_REG_EXTERN int PLAT_REG_GetDiscUniqueIDValid(bool *pbValidFlg);
PLAT_REG_EXTERN int PLAT_REG_GetDiscUniqueID(UINT32 *pdDiscUniqueID);

PLAT_REG_EXTERN int PLAT_REG_GetCurPlayBackDeviceType(UINT32 *pdCurPlayBackDeviceType);
PLAT_REG_EXTERN int PLAT_REG_SetCurPlayBackDeviceType(UINT32 dCurPlayBackDeviceType);
PLAT_REG_EXTERN int PLAT_REG_SetCurPlayBackDeviceTypeValid(void);
PLAT_REG_EXTERN int PLAT_REG_SetCurPlayBackDeviceTypeInvalid(void);


PLAT_REG_EXTERN int PLAT_REG_IsMatchMBDKingKongUniqueID(UINT32 dCurDiscUniqueID, bool* bMatchFlg);

PLAT_REG_EXTERN int PLAT_REG_Dump_SystemStatusRelatedRegs(void);
PLAT_REG_EXTERN int PLAT_REG_Dump_PeripheralRelatedRegs(void);

PLAT_REG_EXTERN int PLAT_REG_GetSvmDiscVolumeIDValid(bool *pbValidFlg);
PLAT_REG_EXTERN int PLAT_REG_SetSvmDiscVolumeIDValid(bool bValidFlg);
PLAT_REG_EXTERN int PLAT_REG_GetSvmDiscVolumeID(UINT8* pbID);
PLAT_REG_EXTERN int PLAT_REG_SetSvmDiscVolumeID(UINT8* pbID);

PLAT_REG_EXTERN int PLAT_REG_GetSvmDiscSerialNumValid(bool *pbValidFlg);
PLAT_REG_EXTERN int PLAT_REG_SetSvmDiscSerialNumValid(bool bValidFlg);
PLAT_REG_EXTERN int PLAT_REG_GetSvmDiscSerialNum(UINT8* pbNum);
PLAT_REG_EXTERN int PLAT_REG_SetSvmDiscSerialNum(UINT8* pbNum);

PLAT_REG_EXTERN int PLAT_REG_GetMediaType(UINT32 * ptype);
PLAT_REG_EXTERN int PLAT_REG_GetDiscLayer(UINT32 * player);
PLAT_REG_EXTERN int PLAT_REG_SetDTVSourceTableIdx(UINT32 dValue);
PLAT_REG_EXTERN int PLAT_REG_GetDTVSourceTableIdx(UINT32 *dValue);

PLAT_REG_EXTERN int PLAT_REG_SetDTVSourceSourceTableIdxValid(void);
PLAT_REG_EXTERN int PLAT_REG_SetDTVSourceSourceTableIdxInvalid(void);
PLAT_REG_EXTERN int PLAT_REG_SetLanguage(UINT32 dValue);
PLAT_REG_EXTERN int PLAT_REG_GetLanguage(UINT32 *dValue);
PLAT_REG_EXTERN int PLAT_REG_SetLanguageValid(void);
PLAT_REG_EXTERN int PLAT_REG_SetLanguageInvalid(void);
#endif
