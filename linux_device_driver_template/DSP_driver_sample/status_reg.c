/**********************************************************************
ModuleName:	Platform Layer Status Register

Description:	Platform Register Interface Function Implement

Copyright:	Copyright @	2009 Sunplus, Inc.

History:		garylin@sunplus.com	 28/04/2009	Creation

**********************************************************************/
#define PLATFORM_REGISTER_IMPLEMENTATION
//#include "gsl.h"
//#include "types.h"
#include <asm/mach-spv9200/types.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include "include/status_reg.h"

#define diag_printf(fmt, arg...) printk(KERN_ALERT fmt, ##arg)

UINT32 gaPSReg[PSREG_MaximumIndex+1];
UINT8 gSvmDiscVolumeID[PSREG_SVM_DISC_VOLUME_SERIAL_LEN];
UINT8 gSvmDiscSerialNum[PSREG_SVM_DISC_VOLUME_SERIAL_LEN];

int PLAT_REG_Init(void)
{
	memset(gaPSReg,0xFF,sizeof(UINT32)*(PSREG_MaximumIndex+1));
	memset(gSvmDiscVolumeID,0,PSREG_SVM_DISC_VOLUME_SERIAL_LEN);
	memset(gSvmDiscSerialNum,0,PSREG_SVM_DISC_VOLUME_SERIAL_LEN);

	//Initial Values.
	PSREG_FULL(PSREG_STAT_PlayMode)=PSREG_PLAYMODE_IDLE;
	//PSREG_SET_INVALID(PSREG_STAT_PlayMode);

	PSREG_FULL(PSREG_STAT_ScrnSaverOn)=PSREG_SCRN_SAVER_OFF;
	//PSREG_SET_INVALID(PSREG_STAT_ScrnSaverOn);

	PSREG_FULL(PSREG_STAT_VideoFormat)=PSREG_VIDEO_MODE_NONE;
	//PSREG_SET_INVALID(PSREG_STAT_VideoFormat);

	PSREG_FULL(PSREG_STAT_VideoFrameRate)=PSREG_VIDEO_FRAME_RATE_OTHER;
	//PSREG_SET_INVALID(PSREG_STAT_VideoFrameRate);

	PSREG_FULL(PSREG_STAT_VideoAspectRatio)=PSREG_VIDEO_ASPECT_RATIO_OTHER;
	//PSREG_SET_INVALID(PSREG_STAT_VideoAspectRatio);

	PSREG_FULL(PSREG_STAT_AudioFormat)=PSREG_AUDIO_MODE_NONE;
	//PSREG_SET_INVALID(PSREG_STAT_AudioFormat);

	PSREG_FULL(PSREG_STAT_AudioChn)=PSREG_AUDIO_CHN_NONE;
	//PSREG_SET_INVALID(PSREG_STAT_AudioChn);

	PSREG_FULL(PSREG_STAT_AudioVolumn)=16;//Ref dirver\auddrv_8600\aud_static.c  aud_InitAudParams Set  aud_param.volume . Raymond
	//PSREG_SET_INVALID(PSREG_STAT_AudioVolumn);

	PSREG_FULL(PSREG_STAT_ShowScreenSaver)=PSREG_SCRN_SAVER_ON;
	//PSREG_SET_INVALID(PSREG_STAT_ShowScreenSaver);

	PSREG_FULL(PSREG_STAT_AudioMuteCtrl)=PSREG_AUDIO_MUTE_OFF;
	//PSREG_SET_INVALID(PSREG_STAT_AudioMuteCtrl);

	PSREG_FULL(PSREG_SMART_UPGRADE)=PSREG_SMART_UPGRADE_ON;
	//PSREG_SET_INVALID(PSREG_SMART_UPGRADE);

	PSREG_FULL(PSREG_PERI_LoaderPhysicalFormat)=PSREG_PHYSICAL_UNKNOWN;
	//PSREG_SET_INVALID(PSREG_PERI_LoaderPhysicalFormat);

	PSREG_FULL(PSREG_STAT_LOGICAL_DISCTYPE)=PSREG_LOGICAL_UNKNOW;
	//PSREG_SET_INVALID(PSREG_STAT_LOGICAL_DISCTYPE);

	PSREG_FULL(PSREG_PERI_UsbDevInserted)=PSREG_PERI_USBDEV_NON_INSERTED;
	//PSREG_SET_INVALID(PSREG_PERI_UsbDevInserted);

	PSREG_FULL(PSREG_PERI_CardDevInserted)=PSREG_PERI_CARDDEV_NON_INSERTED;
	//PSREG_SET_INVALID(PSREG_PERI_CardDevInserted);

	PSREG_FULL(PSREG_FileContainerDiscNav_TYPE)=PSREG_FILE_CONTAINER_NAV_DISC_TYPE_UNKNOW;
	//PSREG_SET_INVALID(PSREG_FileContainerDiscNav_TYPE);

	PSREG_FULL(PSREG_FILE_LOADER) = PSREG_FILE_LOADER_NO_INIT;
	//PSREG_SET_INVALID(PSREG_FILE_LOADER);

	PSREG_FULL(PSREG_BITRATE_Kbps) = 0;
	//PSREG_SET_INVALID(PSREG_BITRATE_Kbps);

	PSREG_FULL(PSREG_STAT_VideoSrcHorizontalSize) = 0;
	//PSREG_SET_INVALID(PSREG_STAT_VideoSrcHorizontalSize);

	PSREG_FULL(PSREG_STAT_VideoSrcVerticalSize) = 0;
	//PSREG_SET_INVALID(PSREG_STAT_VideoSrcVerticalSize);

	PSREG_FULL(PSREG_STAT_AudioSampleRate) = 0;
	//PSREG_SET_INVALID(PSREG_STAT_AudioSampleRate);

	PSREG_FULL(PSREG_STAT_AudioBitPerSample) = 0;
	//PSREG_SET_INVALID(PSREG_STAT_AudioBitPerSample);

	PSREG_FULL(PSREG_STAT_DiscUniqueID)=PSREG_DISCID_INVALID;
	//PSREG_SET_INVALID(PSREG_STAT_DiscUniqueID);

       PSREG_FULL(PSREG_STAT_CurPlayBackDeviceType)=PLAYBACK_DEVICE_TYPE_UNKNOWN;//Raymond Add
       //PSREG_SET_INVALID(PSREG_STAT_CurPlayBackDeviceType);

	PSREG_FULL(PSREG_STAT_SVM_DiscVolumeID) =PSREG_INVALID;

  	PSREG_FULL(PSREG_STAT_SVM_DiscSerialNum) =PSREG_INVALID;

	PSREG_FULL(PSREG_STAT_DiscLayer) =PSREG_INVALID;

	PSREG_FULL(PSREG_STAT_RunAACSCertification) =PSREG_INVALID;

	return PSREG_SUCCESS;
}

int PLAT_REG_Finalize(void)
{

	return PSREG_SUCCESS;
}


int PLAT_REG_SetPlaymode(UINT32 dValue)
{
	PSREG_FULL(PSREG_STAT_PlayMode)=dValue;
	return PSREG_SUCCESS;
}

int PLAT_REG_SetPlaymodeValid(void)
{
	PSREG_SET_VALID(PSREG_STAT_PlayMode);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetPlaymodeInvalid(void)
{
	PSREG_SET_INVALID(PSREG_STAT_PlayMode);
	return PSREG_SUCCESS;
}


int PLAT_REG_SetVideoFormat(UINT32 dValue)
{
	PSREG_FULL(PSREG_STAT_VideoFormat)=dValue;
	return PSREG_SUCCESS;
}

int PLAT_REG_SetVideoFormatValid(void)
{
	PSREG_SET_VALID(PSREG_STAT_VideoFormat);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetVideoFormatInvalid(void)
{
	PSREG_SET_INVALID(PSREG_STAT_VideoFormat);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetVideoFrameRate(UINT32 dValue)
{
	PSREG_FULL(PSREG_STAT_VideoFrameRate)=dValue;
	return PSREG_SUCCESS;
}

int PLAT_REG_SetVideoFrameRateValid(void)
{
	PSREG_SET_VALID(PSREG_STAT_VideoFrameRate);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetVideoFrameRateInvalid(void)
{
	PSREG_SET_INVALID(PSREG_STAT_VideoFrameRate);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetVideoAspectRatio(UINT32 dValue)
{
	PSREG_FULL(PSREG_STAT_VideoAspectRatio)=dValue;
	return PSREG_SUCCESS;
}

int PLAT_REG_SetVideoAspectRatioValid(void)
{
	PSREG_SET_VALID(PSREG_STAT_VideoAspectRatio);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetVideoAspectRatioInvalid(void)
{
	PSREG_SET_INVALID(PSREG_STAT_VideoAspectRatio);
	return PSREG_SUCCESS;
}



int PLAT_REG_SetAudioFormat(UINT32 dValue)
{
	PSREG_FULL(PSREG_STAT_AudioFormat)=dValue;
	return PSREG_SUCCESS;
}

int PLAT_REG_SetAudioFormatValid(void)
{
	PSREG_SET_VALID(PSREG_STAT_AudioFormat);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetAudioFormatInvalid(void)
{
	PSREG_SET_INVALID(PSREG_STAT_AudioFormat);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetAudioChn(UINT32 dValue)
{
	PSREG_FULL(PSREG_STAT_AudioChn)=dValue;
	return PSREG_SUCCESS;
}

int PLAT_REG_GetAudioChn(UINT32 *dValue)
{
	*dValue = PSREG_VALUE(PSREG_STAT_AudioChn);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetAudioChnValid(void)
{
	PSREG_SET_VALID(PSREG_STAT_AudioChn);
	return PSREG_SUCCESS;
}


int PLAT_REG_SetAudioChnInvalid(void)
{
	PSREG_SET_INVALID(PSREG_STAT_AudioChn);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetAudioVolumn(UINT32 dValue)
{
	PSREG_FULL(PSREG_STAT_AudioVolumn)=dValue;
	return PSREG_SUCCESS;
}

int PLAT_REG_SetAudioVolumnValid(void)
{
	PSREG_SET_VALID(PSREG_STAT_AudioVolumn);
	return PSREG_SUCCESS;
}


int PLAT_REG_SetAudioVolumnInvalid(void)
{
	PSREG_SET_INVALID(PSREG_STAT_AudioVolumn);
	return PSREG_SUCCESS;
}


int PLAT_REG_SetAudioMuteCtrl(UINT32 MuteID)
{
	PSREG_FULL(PSREG_STAT_AudioMuteCtrl)=MuteID;
	return PSREG_SUCCESS;
}


int PLAT_REG_SetAudioMuteCtrlValid(void)
{
	PSREG_SET_VALID(PSREG_STAT_AudioMuteCtrl);
	return PSREG_SUCCESS;
}


int PLAT_REG_SetAudioMuteCtrlInvalid(void)
{
	PSREG_SET_INVALID(PSREG_STAT_AudioMuteCtrl);
	return PSREG_SUCCESS;
}


int PLAT_REG_AddAudioMuteCtrl(UINT32 mute_enable ,UINT32 MuteID)
{
	if(mute_enable == PSREG_AUDIO_MUTE)
    		PLAT_REG_SetAudioMuteCtrl(PSREG_VALUE(PSREG_STAT_AudioMuteCtrl) | MuteID );
	else
    		PLAT_REG_SetAudioMuteCtrl(PSREG_VALUE(PSREG_STAT_AudioMuteCtrl) & (~MuteID) );

	PLAT_REG_SetAudioMuteCtrlValid();

	return PSREG_SUCCESS;
}


int PLAT_REG_GetAudioMuteStatus(UINT32 *mute_status)
{
	if(PSREG_VALUE(PSREG_STAT_AudioMuteCtrl) != 0)
		*mute_status = PSREG_AUDIO_MUTE_ON;
	else
		*mute_status = PSREG_AUDIO_MUTE_OFF;

	return PSREG_SUCCESS;
}


int PLAT_REG_GetAudio_MuteID_Status(UINT32 MuteID , UINT32 *mute_status)
{
	if((PSREG_VALUE(PSREG_STAT_AudioMuteCtrl) & MuteID) != 0)
		*mute_status = PSREG_AUDIO_MUTE_ON;
	else
		*mute_status = PSREG_AUDIO_MUTE_OFF;

	return PSREG_SUCCESS;
}

int PLAT_REG_SetFileContainerDiscNavType(UINT32 dType)
{
	PSREG_FULL(PSREG_FileContainerDiscNav_TYPE)=dType;
	return PSREG_SUCCESS;
}

int PLAT_REG_SetFileContainerDiscNavTypeValid(void)
{
	PSREG_SET_VALID(PSREG_FileContainerDiscNav_TYPE);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetFileContainerDiscNavTypeInvalid(void)
{
	PSREG_SET_INVALID(PSREG_FileContainerDiscNav_TYPE);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetBitRateKbps(UINT32 dKbps)
{
	PSREG_FULL(PSREG_BITRATE_Kbps)=dKbps;
	return PSREG_SUCCESS;
}

int PLAT_REG_SetBitRateKbpsValid(void)
{
	PSREG_SET_VALID(PSREG_BITRATE_Kbps);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetBitRateKbpsInvalid(void)
{
	PSREG_SET_INVALID(PSREG_BITRATE_Kbps);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetVideoSrcHorizontalSize(UINT32 dPixelNumber)
{
	PSREG_FULL(PSREG_STAT_VideoSrcHorizontalSize)=dPixelNumber;
	return PSREG_SUCCESS;
}

int PLAT_REG_SetVideoSrcHorizontalSizeValid(void)
{
	PSREG_SET_VALID(PSREG_STAT_VideoSrcHorizontalSize);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetVideoSrcHorizontalSizeInvalid(void)
{
	PSREG_SET_INVALID(PSREG_STAT_VideoSrcHorizontalSize);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetVideoSrcVerticalSize(UINT32 dPixelNumber)
{
	PSREG_FULL(PSREG_STAT_VideoSrcVerticalSize)=dPixelNumber;
	return PSREG_SUCCESS;
}

int PLAT_REG_SetVideoSrcVerticalSizeValid(void)
{
	PSREG_SET_VALID(PSREG_STAT_VideoSrcVerticalSize);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetVideoSrcVerticalSizeInvalid(void)
{
	PSREG_SET_INVALID(PSREG_STAT_VideoSrcVerticalSize);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetUsbDevStatus(UINT32 dValue)
{
	PSREG_FULL(PSREG_PERI_UsbDevInserted)=dValue;
	return PSREG_SUCCESS;
}

int PLAT_REG_SetUsbDevStatusValid(void)
{
	PSREG_SET_VALID(PSREG_PERI_UsbDevInserted);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetUsbDevStatusInvalid(void)
{
	PSREG_SET_INVALID(PSREG_PERI_UsbDevInserted);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetCardDevStatus(UINT32 dValue)
{
	PSREG_FULL(PSREG_PERI_CardDevInserted)=dValue;
	return PSREG_SUCCESS;
}

int PLAT_REG_SetCardDevStatusValid(void)
{
	PSREG_SET_VALID(PSREG_PERI_CardDevInserted);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetCardDevStatusInvalid(void)
{
	PSREG_SET_INVALID(PSREG_PERI_CardDevInserted);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetAudioSampleRate(UINT32 dHzNumber)
{
	PSREG_FULL(PSREG_STAT_AudioSampleRate)=dHzNumber;
	return PSREG_SUCCESS;
}
int PLAT_REG_SetAudioSampleRateValid(void)
{
	PSREG_SET_VALID(PSREG_STAT_AudioSampleRate);
	return PSREG_SUCCESS;
}
int PLAT_REG_SetAudioSampleRateInvalid(void)
{
	PSREG_SET_INVALID(PSREG_STAT_AudioSampleRate);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetAudioBitPerSample(UINT32 dBitNumber)
{
	PSREG_FULL(PSREG_STAT_AudioBitPerSample)=dBitNumber;
	return PSREG_SUCCESS;
}

int PLAT_REG_SetAudioBitPerSampleValid(void)
{
	PSREG_SET_VALID(PSREG_STAT_AudioBitPerSample);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetAudioBitPerSampleInvalid(void)
{
	PSREG_SET_INVALID(PSREG_STAT_AudioBitPerSample);
	return PSREG_SUCCESS;
}

int PLAT_REG_GetDiscUniqueIDValid(bool *pbValidFlg)
{
	if(!pbValidFlg)
		return PSREG_PARAMETER_ERROR;

	if(PSREG_IS_VALID(PSREG_STAT_DiscUniqueID))
		*pbValidFlg = true;
	else
		*pbValidFlg = false;

	return PSREG_SUCCESS;
}

int PLAT_REG_GetDiscUniqueID(UINT32 *pdDiscUniqueID)
{
	if(!pdDiscUniqueID)
		return PSREG_PARAMETER_ERROR;

	*pdDiscUniqueID = PSREG_VALUE(PSREG_STAT_DiscUniqueID) ;

	return PSREG_SUCCESS;
}

int PLAT_REG_GetCurPlayBackDeviceType(UINT32 *pdCurPlayBackDeviceType)
{
	if(!pdCurPlayBackDeviceType)
		return PSREG_PARAMETER_ERROR;

	*pdCurPlayBackDeviceType = PSREG_VALUE(PSREG_STAT_CurPlayBackDeviceType) ;

	return PSREG_SUCCESS;
}

int PLAT_REG_SetCurPlayBackDeviceType(UINT32 dCurPlayBackDeviceType)
{
	if(dCurPlayBackDeviceType >= PLAYBACK_DEVICE_TYPE_MAX)
		return PSREG_PARAMETER_ERROR;

	PSREG_FULL(PSREG_STAT_CurPlayBackDeviceType) = dCurPlayBackDeviceType;

	return PSREG_SUCCESS;
}

int PLAT_REG_SetCurPlayBackDeviceTypeValid(void)
{
	PSREG_SET_VALID(PSREG_STAT_CurPlayBackDeviceType);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetCurPlayBackDeviceTypeInvalid(void)
{
	PSREG_SET_INVALID(PSREG_STAT_CurPlayBackDeviceType);
	return PSREG_SUCCESS;
}

//-------------------------- For Lib use DiscUniqueID Handler		Raymond

int PLAT_REG_IsMatchMBDKingKongUniqueID(UINT32 dCurDiscUniqueID, bool* bMatchFlg)
{
	if(!bMatchFlg)
		return PSREG_PARAMETER_ERROR;

#ifdef PSREG_DISCID_MBD_KINGKONG

	if(dCurDiscUniqueID == PSREG_DISCID_MBD_KINGKONG)
	{
		*bMatchFlg = true;
		//printfk("\n\n\n\nWarning: [Platform]: KingKong MBD Hit! Are You sure that The Disc is KingKong MiniBD ? \n\n\n\n");
	}
	else
		*bMatchFlg = false;

#else

		*bMatchFlg = false;

#endif

	return PSREG_SUCCESS;
}

int PLAT_REG_GetSvmDiscVolumeIDValid(bool *pbValidFlg)
{
	if(!pbValidFlg)
		return PSREG_PARAMETER_ERROR;

	*pbValidFlg = PSREG_IS_VALID(PSREG_STAT_SVM_DiscVolumeID);

	return PSREG_SUCCESS;
}

int PLAT_REG_SetSvmDiscVolumeIDValid(bool bValidFlg)
{
	if(bValidFlg)
		PSREG_SET_VALID(PSREG_STAT_SVM_DiscVolumeID);
	else
		PSREG_SET_INVALID(PSREG_STAT_SVM_DiscVolumeID);
	return PSREG_SUCCESS;
}

int PLAT_REG_GetSvmDiscVolumeID(UINT8* pbID)
{
	if(!pbID)
		return PSREG_PARAMETER_ERROR;

	memcpy(pbID,gSvmDiscVolumeID,PSREG_SVM_DISC_VOLUME_SERIAL_LEN);

	return PSREG_SUCCESS;
}

int PLAT_REG_SetSvmDiscVolumeID(UINT8* pbID)
{
	if(!pbID)
		return PSREG_PARAMETER_ERROR;

	memcpy(gSvmDiscVolumeID,pbID,PSREG_SVM_DISC_VOLUME_SERIAL_LEN);

	return PSREG_SUCCESS;
}


int PLAT_REG_GetSvmDiscSerialNumValid(bool *pbValidFlg)
{
	if(!pbValidFlg)
		return PSREG_PARAMETER_ERROR;

	*pbValidFlg = PSREG_IS_VALID(PSREG_STAT_SVM_DiscSerialNum);

	return PSREG_SUCCESS;
}

int PLAT_REG_SetSvmDiscSerialNumValid(bool bValidFlg)
{
	if(bValidFlg)
		PSREG_SET_VALID(PSREG_STAT_SVM_DiscSerialNum);
	else
		PSREG_SET_INVALID(PSREG_STAT_SVM_DiscSerialNum);
	return PSREG_SUCCESS;
}

int PLAT_REG_GetSvmDiscSerialNum(UINT8* pbNum)
{
	if(!pbNum)
		return PSREG_PARAMETER_ERROR;

	memcpy(pbNum,gSvmDiscSerialNum,PSREG_SVM_DISC_VOLUME_SERIAL_LEN);

	return PSREG_SUCCESS;
}

int PLAT_REG_SetSvmDiscSerialNum(UINT8* pbNum)
{
	if(!pbNum)
		return PSREG_PARAMETER_ERROR;

	memcpy(gSvmDiscSerialNum,pbNum,PSREG_SVM_DISC_VOLUME_SERIAL_LEN);

	return PSREG_SUCCESS;
}

int PLAT_REG_GetMediaType(UINT32 * ptype)
{

	*ptype = PSREG_VALUE(PSREG_PERI_LoaderPhysicalFormat);
	return PSREG_SUCCESS;
}

int PLAT_REG_GetDiscLayer(UINT32 * player)
{
	*player = PSREG_VALUE(PSREG_STAT_DiscLayer);
	return PSREG_SUCCESS;
}


int PLAT_REG_Dump_SystemStatusRelatedRegs(void)
{
	diag_printf("\n * Define Value, Plz reference include/platform/status_reg_def.h *\n");
	diag_printf("PSREG_STAT_PlayMode				Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_STAT_PlayMode),PSREG_VALUE(PSREG_STAT_PlayMode));
	diag_printf("PSREG_STAT_LOGICAL_DISCTYPE	Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_STAT_LOGICAL_DISCTYPE),PSREG_VALUE(PSREG_STAT_LOGICAL_DISCTYPE));
	diag_printf("PSREG_STAT_VideoFormat			Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_STAT_VideoFormat),PSREG_VALUE(PSREG_STAT_VideoFormat));
	diag_printf("PSREG_STAT_VideoFrameRate		Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_STAT_VideoFrameRate),PSREG_VALUE(PSREG_STAT_VideoFrameRate));
	diag_printf("PSREG_STAT_VideoAspectRatio		Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_STAT_VideoAspectRatio),PSREG_VALUE(PSREG_STAT_VideoAspectRatio));
	diag_printf("PSREG_STAT_AudioFormat			Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_STAT_AudioFormat),PSREG_VALUE(PSREG_STAT_AudioFormat));
	diag_printf("PSREG_STAT_AudioVolumn			Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_STAT_AudioVolumn),PSREG_VALUE(PSREG_STAT_AudioVolumn));
	diag_printf("PSREG_STAT_AudioMuteCtrl			Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_STAT_AudioMuteCtrl),PSREG_VALUE(PSREG_STAT_AudioMuteCtrl));
	diag_printf("PSREG_STAT_ScrnSaverOn			Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_STAT_ScrnSaverOn),PSREG_VALUE(PSREG_STAT_ScrnSaverOn));
	diag_printf("PSREG_STAT_ActiveSysApp			Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_STAT_ActiveSysApp),PSREG_VALUE(PSREG_STAT_ActiveSysApp));
	diag_printf("PSREG_STAT_MainAppStatus			Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_STAT_MainAppStatus),PSREG_VALUE(PSREG_STAT_MainAppStatus));
	diag_printf("PSREG_STAT_ShowScreenSaver		Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_STAT_ShowScreenSaver),PSREG_VALUE(PSREG_STAT_ShowScreenSaver));
	diag_printf("PSREG_SMART_UPGRADE			Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_SMART_UPGRADE),PSREG_VALUE(PSREG_SMART_UPGRADE));
	diag_printf("PSREG_FILE_LOADER				Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_FILE_LOADER),PSREG_VALUE(PSREG_FILE_LOADER));
	diag_printf("PSREG_FileContainerDiscNav_TYPE	Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_FileContainerDiscNav_TYPE),PSREG_VALUE(PSREG_FileContainerDiscNav_TYPE));
	diag_printf("PSREG_BITRATE_Kbps				Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_BITRATE_Kbps),PSREG_VALUE(PSREG_BITRATE_Kbps));
	diag_printf("PSREG_STAT_VideoSrcHorizontalSize	Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_STAT_VideoSrcHorizontalSize),PSREG_VALUE(PSREG_STAT_VideoSrcHorizontalSize));
	diag_printf("PSREG_STAT_VideoSrcVerticalSize	Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_STAT_VideoSrcVerticalSize),PSREG_VALUE(PSREG_STAT_VideoSrcVerticalSize));
	diag_printf("PSREG_STAT_AudioSampleRate		Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_STAT_AudioSampleRate),PSREG_VALUE(PSREG_STAT_AudioSampleRate));
	diag_printf("PSREG_STAT_AudioBitPerSample		Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_STAT_AudioBitPerSample),PSREG_VALUE(PSREG_STAT_AudioBitPerSample));
	diag_printf("PSREG_STAT_DiscUniqueID			Valid [%d] Value [0x%x]\n",PSREG_IS_VALID(PSREG_STAT_DiscUniqueID),PSREG_VALUE(PSREG_STAT_DiscUniqueID));
	diag_printf("PSREG_STAT_CurPlayBackDeviceType	Valid [%d] Value [0x%x]\n",PSREG_IS_VALID(PSREG_STAT_CurPlayBackDeviceType),PSREG_VALUE(PSREG_STAT_CurPlayBackDeviceType));
	diag_printf("PSREG_STAT_RunAACSCertification	Valid [%d] Value [0x%x]\n",PSREG_IS_VALID(PSREG_STAT_RunAACSCertification),PSREG_VALUE(PSREG_STAT_RunAACSCertification));
	diag_printf("PSREG_STAT_DiscLayer				Valid [%d] Value [0x%x]\n",PSREG_IS_VALID(PSREG_STAT_DiscLayer),PSREG_VALUE(PSREG_STAT_DiscLayer));

	return PSREG_SUCCESS;
}

int PLAT_REG_Dump_PeripheralRelatedRegs(void)
{
	diag_printf("\n * Define Value, Plz reference include/platform/status_reg_def.h *\n");
	diag_printf("\nPSREG_PERI_LoaderPhysicalFormat	Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_PERI_LoaderPhysicalFormat),PSREG_VALUE(PSREG_PERI_LoaderPhysicalFormat));
	diag_printf("PSREG_PERI_UsbDevInserted		Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_PERI_UsbDevInserted),PSREG_VALUE(PSREG_PERI_UsbDevInserted));
	diag_printf("PSREG_PERI_CardDevInserted		Valid [%d] Value [%d]\n",PSREG_IS_VALID(PSREG_PERI_CardDevInserted),PSREG_VALUE(PSREG_PERI_CardDevInserted));

	return PSREG_SUCCESS;
}
int PLAT_REG_SetDTVSourceTableIdx(UINT32 dValue)
{
	PSREG_FULL(PSREG_STAT_DTVSource)=dValue;
	return PSREG_SUCCESS;
}

int PLAT_REG_GetDTVSourceTableIdx(UINT32 *dValue)
{
	*dValue = PSREG_FULL(PSREG_STAT_DTVSource);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetDTVSourceSourceTableIdxValid(void)
{
	PSREG_SET_VALID(PSREG_STAT_DTVSource);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetDTVSourceSourceTableIdxInvalid(void)
{
	PSREG_SET_INVALID(PSREG_STAT_DTVSource);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetLanguage(UINT32 dValue)
{
	PSREG_FULL(PSREG_STAT_Language) = dValue;
	return PSREG_SUCCESS;
}

int PLAT_REG_GetLanguage(UINT32 *dValue)
{
	*dValue = PSREG_FULL(PSREG_STAT_Language);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetLanguageValid(void)
{
	PSREG_SET_VALID(PSREG_STAT_Language);
	return PSREG_SUCCESS;
}

int PLAT_REG_SetLanguageInvalid(void)
{
	PSREG_SET_INVALID(PSREG_STAT_Language);
	return PSREG_SUCCESS;
}
