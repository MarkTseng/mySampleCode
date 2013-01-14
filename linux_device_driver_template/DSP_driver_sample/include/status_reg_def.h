/**********************************************************************
ModuleName:	Platform Layer Status Register

Description:	Platform Status Register Value Define

Copyright:	Copyright @	2009 Sunplus, Inc.

History:		garylin@sunplus.com	 13/05/2009	Creation
			
**********************************************************************/

#ifndef PSR_DEF_H
#define PSR_DEF_H

#define PSREG_INVALID_BIT		(31)
#define PSREG_INVALID			(1<<(PSREG_INVALID_BIT))
#define PSREG_VALUE_MASK		(~PSREG_INVALID)	


//Defines for Individual PSREG
	//PSREG_SETUP_TV_System,
	#define PSREG_SETUP_PAL 0
	#define PSREG_SETUP_NTSC 1
	#define PSREG_SETUP_TV_System_Auto 2
	#define PSREG_SETUP_TV_System_24Hz 3 //temporarily used for recording thet 24Hz's really on

	//PSREG_SETUP_Downmix,
	#define PSREG_SETUP_Multi_channel 0
	#define PSREG_SETUP_Multi_Stereo 1
	#define PSREG_SETUP_Multi_LtRt 2
	
	//PSREG_SETUP_Country_Code,
	
	//PSREG_SETUP_DVD_Rating,
	//these values are specified
	#define PSREG_SETUP_Unlock 0xf
	#define PSREG_SETUP_8_Adult 8
	#define PSREG_SETUP_7_NC17 7
	#define PSREG_SETUP_6_R 6
	#define PSREG_SETUP_DVDRating_5 5
	#define PSREG_SETUP_4_PG13 4
	#define PSREG_SETUP_3_PG 3
	#define PSREG_SETUP_DVDRating_2 2
	#define PSREG_SETUP_1_G 1

	//PSREG_SETUP_Aspect_Ratio,
	#define PSREG_SETUP_43_Pan_Scan 0
	#define PSREG_SETUP_43_Letter_Box 1
	#define PSREG_SETUP_169_Normal 2
	#define PSREG_SETUP_169_Wide 3
	
	//PSREG_SETUP_Display_Mode,
	
	//PSREG_SETUP_Audio_Config,
	
	//PSREG_SETUP_MenuLang,
	#define PSREG_SETUP_MenuLang_English 1
	#define PSREG_SETUP_MenuLang_SimpChinese 2
	#define PSREG_SETUP_MenuLang_TradChinese 3
	#define PSREG_SETUP_MenuLang_Spanish 4
	#define PSREG_SETUP_MenuLang_French 5
	#define PSREG_SETUP_MenuLang_German 6
	#define PSREG_SETUP_MenuLang_Portuguese 7
	#define PSREG_SETUP_MenuLang_Japanese 8
	#define PSREG_SETUP_MenuLang_Korean 9
	#define PSREG_SETUP_MenuLang_Indonesian 10
	#define PSREG_SETUP_MenuLang_Russian 11
	#define PSREG_SETUP_MenuLang_Thai 12
	#define PSREG_SETUP_MenuLang_Arabic 13
	#define PSREG_SETUP_MenuLang_Italian 14
	#define PSREG_SETUP_MenuLang_Hungarian 15
	#define PSREG_SETUP_MenuLang_Polish 16
	#define PSREG_SETUP_MenuLang_Dutch 17
	#define PSREG_SETUP_MenuLang_Swedish 18
	#define PSREG_SETUP_MenuLang_Czech 19
	#define PSREG_SETUP_MenuLang_Greek 20
	#define PSREG_SETUP_MenuLang_Iranian 21
	#define PSREG_SETUP_MenuLang_Vietnamese 22
	#define PSREG_SETUP_MenuLang_Turkish 23
	
	//PSREG_SETUP_DiscAudio, //following values (from 1 to 14) should be fixed for _DVD_Mapping_LangCodeByPSR()
	#define PSREG_SETUP_DiscAudio_English 1
	#define PSREG_SETUP_DiscAudio_SimpChinese 2
	#define PSREG_SETUP_DiscAudio_TradChinese 3
	#define PSREG_SETUP_DiscAudio_Spanish 4
	#define PSREG_SETUP_DiscAudio_French 5
	#define PSREG_SETUP_DiscAudio_German 6
	#define PSREG_SETUP_DiscAudio_Portuguese 7
	#define PSREG_SETUP_DiscAudio_Japanese 8
	#define PSREG_SETUP_DiscAudio_Korean 9
	#define PSREG_SETUP_DiscAudio_Indonesian 10
	#define PSREG_SETUP_DiscAudio_Russian 11
	#define PSREG_SETUP_DiscAudio_Thai 12
	#define PSREG_SETUP_DiscAudio_Arabic 13
	#define PSREG_SETUP_DiscAudio_Default 14
			
	//PSREG_SETUP_DiscSubtitle, //following values (from 1 to 14) should be fixed for _DVD_Mapping_LangCodeByPSR()
	#define PSREG_SETUP_DiscSubtitle_English 1
	#define PSREG_SETUP_DiscSubtitle_SimpChinese 2
	#define PSREG_SETUP_DiscSubtitle_TradChinese 3
	#define PSREG_SETUP_DiscSubtitle_Spanish 4
	#define PSREG_SETUP_DiscSubtitle_French 5
	#define PSREG_SETUP_DiscSubtitle_German 6
	#define PSREG_SETUP_DiscSubtitle_Portuguese 7
	#define PSREG_SETUP_DiscSubtitle_Japanese 8
	#define PSREG_SETUP_DiscSubtitle_Korean 9
	#define PSREG_SETUP_DiscSubtitle_Indonesian 10
	#define PSREG_SETUP_DiscSubtitle_Russian 11
	#define PSREG_SETUP_DiscSubtitle_Thai 12
	#define PSREG_SETUP_DiscSubtitle_Arabic 13
	#define PSREG_SETUP_DiscSubtitle_Default 14
	#define PSREG_SETUP_DiscSubtitle_Off 0

	//PSREG_SETUP_DiscMenu, //following values (from 1 to 14) should be fixed for _DVD_Mapping_LangCodeByPSR()
	#define PSREG_SETUP_DiscMenu_English 1
	#define PSREG_SETUP_DiscMenu_SimpChinese 2
	#define PSREG_SETUP_DiscMenu_TradChinese 3
	#define PSREG_SETUP_DiscMenu_Spanish 4
	#define PSREG_SETUP_DiscMenu_French 5
	#define PSREG_SETUP_DiscMenu_German 6
	#define PSREG_SETUP_DiscMenu_Portuguese 7
	#define PSREG_SETUP_DiscMenu_Japanese 8
	#define PSREG_SETUP_DiscMenu_Korean 9
	#define PSREG_SETUP_DiscMenu_Indonesian 10
	#define PSREG_SETUP_DiscMenu_Russian 11
	#define PSREG_SETUP_DiscMenu_Thai 12
	#define PSREG_SETUP_DiscMenu_Arabic 13
	#define PSREG_SETUP_DiscMenu_Default 14
	
	//PSREG_SETUP_Output_Component,
	#define PSREG_SETUP_S_Video 0 
	#define PSREG_SETUP_RGB_SCART 1 
	#define PSREG_SETUP_YCbCr 2 
	#define PSREG_SETUP_VGA 3 
	
	//PSREG_SETUP_YCbCrHDMI_Resolution,
	#define PSREG_SETUP_Resolution_Auto 0 
	#define PSREG_SETUP_480i_576i 1	
	#define PSREG_SETUP_480p_576p 2	
	#define PSREG_SETUP_720p 3	
	#define PSREG_SETUP_1080i 4	
	#define PSREG_SETUP_1080p 5	
					
	//PSREG_SETUP_HDMI_1080p_24Hz,
	#define PSREG_SETUP_HDMI_24Hz_On 0
	#define PSREG_SETUP_HDMI_24Hz_Off 1

	//PSREG_SETUP_HDMI_Color_Setting,
	#define PSREG_SETUP_HDMI_Color_Auto 0
	#define PSREG_SETUP_HDMI_Color_YCbCr 1
	#define PSREG_SETUP_RGB 2
	#define PSREG_SETUP_xv_Color 3

	//PSREG_SETUP_HDMI_Deep_Color,
	#define PSREG_SETUP_HDMI_Deep_Color_Auto 0
	#define PSREG_SETUP_8bit 1
	#define PSREG_SETUP_10bit 2
	#define PSREG_SETUP_12bit 3

	//PSREG_SETUP_Pic_Full_Color_Range,
	#define PSREG_SETUP_Pic_Full_Range_On 0
	#define PSREG_SETUP_Pic_Full_Range_Off 1
	
	//PSREG_SETUP_Standby_Mode,
	#define PSREG_SETUP_Quick_Start 0
	#define PSREG_SETUP_Normal 1

	//PSREG_SETUP_AVC_Menu,
	#define PSREG_SETUP_AVC_Menu_On 0
	#define PSREG_SETUP_AVC_Menu_Off 1
	
	//PSREG_SETUP_Resume,
	#define PSREG_SETUP_Resume_On 0
	#define PSREG_SETUP_Resume_Off 1
	
	//PSREG_SETUP_Screen_Saver,
	#define PSREG_SETUP_Screen_Saver_On 0
	#define PSREG_SETUP_Screen_Saver_Off 1

	//PSREG_SETUP_Screen_Fit,
	#define PSREG_SETUP_Screen_Fit_100 0
	#define PSREG_SETUP_Screen_Fit_98 1
	#define PSREG_SETUP_Screen_Fit_96 2
	#define PSREG_SETUP_Screen_Fit_94 3
	#define PSREG_SETUP_Screen_Fit_92 4
	#define PSREG_SETUP_Screen_Fit_90 5

	//PSREG_SETUP_Skin,
	#define PSREG_SETUP_Background_1 0
	#define PSREG_SETUP_Background_2 1
	#define PSREG_SETUP_User_Background 2
	
	//PSREG_SETUP_Dhcp
	#define	PSREG_SETUP_DHCPDISABLE	0
	#define	PSREG_SETUP_DHCPENABLE	1
	
	//PSREG_SETUP_Region_Code,
	#define PSREG_SETUP_RegionCode_0 0
	#define PSREG_SETUP_RegionCode_1 1
	#define PSREG_SETUP_RegionCode_2 2
	#define PSREG_SETUP_RegionCode_3 3
	#define PSREG_SETUP_RegionCode_4 4
	#define PSREG_SETUP_RegionCode_5 5
	#define PSREG_SETUP_RegionCode_6 6
	#define PSREG_SETUP_RegionCode_7 7
	#define PSREG_SETUP_RegionCode_8 8
	
	//PSREG_CONTENT_SUPPORT_XVCOLOR,
	#define PSREG_CONTENT_SUPPORT_XVCOLOR_NO 0
	#define PSREG_CONTENT_SUPPORT_XVCOLOR_YES 1
	
	////Peripheral Related
	//PSREG_PERI_UsbDevInserted,
	#define PSREG_PERI_USBDEV_NON_INSERTED	(0)
	#define PSREG_PERI_USBDEV_INSERTED		(1)
	
	//PSREG_PERI_CardDevInserted,
	#define PSREG_PERI_CARDDEV_NON_INSERTED	(0)
	#define PSREG_PERI_CARDDEV_INSERTED		(1)
	
	//PSREG_PERI_LoaderPhysicalFormat ,
	#define PSREG_PHYSICAL_CD_ROM					(0x00)
	#define PSREG_PHYSICAL_CD_R					(0x01)
	#define PSREG_PHYSICAL_CD_RW					(0x02)
	#define PSREG_PHYSICAL_DVD_ROM				(0x03)
	#define PSREG_PHYSICAL_DVD_MINUS_R			(0x04)
	#define PSREG_PHYSICAL_DVD_RAM				(0x05)
	#define PSREG_PHYSICAL_DVD_MINUS_RW_R		(0x06)
	#define PSREG_PHYSICAL_DVD_MINUS_RW_S		(0x07)
	#define PSREG_PHYSICAL_DVD_MINUS_R_DL_I		(0x08)
	#define PSREG_PHYSICAL_DVD_MINUS_R_DL_L		(0x09)
	#define PSREG_PHYSICAL_DVD_PLUS_RW			(0x0A)
	#define PSREG_PHYSICAL_DVD_PLUS_R				(0x0B)
	#define PSREG_PHYSICAL_DVD_PLUS_R_DL			(0x0C)
	#define PSREG_PHYSICAL_BD_ROM					(0x0D)
	#define PSREG_PHYSICAL_BD_R_SRM				(0x0E)
	#define PSREG_PHYSICAL_BD_R_RRM				(0x0F)
	#define PSREG_PHYSICAL_BD_RE					(0x10)
	#define PSREG_PHYSICAL_UNKNOWN				(0x11)
	#define PSREG_PHYSICAL_NODISC					(0x12)

	////System Status Related
	//PSREG_STAT_PlayMode ,
	#define PSREG_PLAYMODE_IDLE				(0)
	#define PSREG_PLAYMODE_STOP				(1)
	#define PSREG_PLAYMODE_PLAY				(2)
	#define PSREG_PLAYMODE_PAUSE				(3)
	#define PSREG_PLAYMODE_BROWSE			(4)

	#define PSREG_PLAYMODE_FASTFORWARD		(5)
	#define PSREG_PLAYMODE_FASTBACKWARD		(6)
	#define PSREG_PLAYMODE_SLOWFORWARD		(7)
	#define PSREG_PLAYMODE_SLOWBACKWARD	(8)

	//PSREG_STAT_LOGICAL_DISCTYPE
	#define PSREG_LOGICAL_NODISC			(0)
	#define PSREG_LOGICAL_DVDVIDEO 			(1)
	#define PSREG_LOGICAL_DVDMINUSVR 		(2)
	#define PSREG_LOGICAL_DVDPLUSVR 		(3)
	#define PSREG_LOGICAL_VCD 				(4)
	#define PSREG_LOGICAL_SVCD				(5)
	#define PSREG_LOGICAL_CDDA				(6)
	#define PSREG_LOGICAL_DATADISC			(7)
	#define PSREG_LOGICAL_BDVIDEO			(8)
        #define PSREG_LOGICAL_KOK				(9)
	#define PSREG_LOGICAL_AVCHD				(10)
	#define PSREG_LOGICAL_UNKNOW			(11)

	//PSREG_STAT_VideoFormat,
	#define PSREG_VIDEO_MODE_NONE            (0)
	#define PSREG_VIDEO_MODE_UNKNOWN       (1)
	#define PSREG_VIDEO_MODE_MPEG1           	(2)
	#define PSREG_VIDEO_MODE_MPEG2           	(3)
	#define PSREG_VIDEO_MODE_MPEG4           	(4)
	#define PSREG_VIDEO_MODE_H264            	(5)
	#define PSREG_VIDEO_MODE_VC1             (6)
	#define PSREG_VIDEO_MODE_MJPEG             (7)
	#define PSREG_VIDEO_MODE_RV           		(8)	


	//PSREG_STAT_VideoFrameRate,
	#define PSREG_VIDEO_FRAME_RATE_OTHER        (0)	
	#define PSREG_VIDEO_FRAME_RATE_23_976    (1)
	#define PSREG_VIDEO_FRAME_RATE_24           (2)
	#define PSREG_VIDEO_FRAME_RATE_25            (3)
	#define PSREG_VIDEO_FRAME_RATE_29_97      (4)
	#define PSREG_VIDEO_FRAME_RATE_50             (5)
	#define PSREG_VIDEO_FRAME_RATE_59_94        (6)	

	//PSREG_STAT_VideoAspectRatio,
	#define PSREG_VIDEO_ASPECT_RATIO_OTHER	(0)
	#define PSREG_VIDEO_ASPECT_RATIO_16_9           (1)
	#define PSREG_VIDEO_ASPECT_RATIO_4_3           (2)
	#define PSREG_VIDEO_ASPECT_RATIO_EXTEND           (3)

	
	//PSREG_STAT_AudioFormat,
	#define PSREG_AUDIO_MODE_NONE            (0)
	#define PSREG_AUDIO_MODE_AC3             (1)
	#define PSREG_AUDIO_MODE_PPCM            (2)  
	#define PSREG_AUDIO_MODE_MP1             (3)  
	#define PSREG_AUDIO_MODE_MP2             (4)
	#define PSREG_AUDIO_MODE_LPCM            (5) 
	#define PSREG_AUDIO_MODE_JPG             (6)
	#define PSREG_AUDIO_MODE_DTS             (7)
	#define PSREG_AUDIO_MODE_SDDS            (8)
	#define PSREG_AUDIO_MODE_MP3             (9) 
	#define PSREG_AUDIO_MODE_PCM             (10)
	#define PSREG_AUDIO_MODE_MIDI            (11) 
	#define PSREG_AUDIO_MODE_NES             (12)
	#define PSREG_AUDIO_MODE_SPDIF           (13)
	#define PSREG_AUDIO_MODE_DTSCD           (14) 
	#define PSREG_AUDIO_MODE_WMA             (15) 
	#define PSREG_AUDIO_MODE_SPDIFINCD       (16)
	#define PSREG_AUDIO_MODE_SPDIFINAC3      (17)
	#define PSREG_AUDIO_MODE_SPDIFINDTS      (18)
	#define PSREG_AUDIO_MODE_TUNER           (19) 
	#define PSREG_AUDIO_MODE_NOISE           (20)
	#define PSREG_AUDIO_MODE_AAC             (21)  
	#define PSREG_AUDIO_MODE_CD_MP3_ENCODER  (22) 
	#define PSREG_AUDIO_MODE_OGG             (23)
	#define PSREG_AUDIO_MODE_MP3_6CH         (24)
	#define PSREG_AUDIO_MODE_MP3_MIX         (25)
	#define PSREG_AUDIO_MODE_VCD_MIC         (26)
	#define PSREG_AUDIO_MODE_DUAL_MP3        (27)
	#define PSREG_AUDIO_MODE_DolbyTrueHD     (28)
	#define PSREG_AUDIO_MODE_RA     	 (29)
	#define PSREG_AUDIO_MODE_EMU     	 (30)	
	#define PSREG_AUDIO_MODE_FLAC     	 (31)
	
	
	//PSREG_STAT_AudioChn,
	#define  PSREG_AUDIO_CHN_NONE			(0)
	#define PSREG_AUDIO_CHN_MONO			(1)
	#define PSREG_AUDIO_CHN_STEREO		(2)
	#define PSREG_AUDIO_CHN_2_1			(3)
	#define PSREG_AUDIO_CHN_4				(4)
	#define PSREG_AUDIO_CHN_5				(5)
	#define PSREG_AUDIO_CHN_6				(6)
	#define PSREG_AUDIO_CHN_7				(7)
	#define PSREG_AUDIO_CHN_8				(8)

	//PSREG_STAT_AudioMuteCtrl,
	//mute_enable flag
	#define PSREG_AUDIO_DEMUTE		(0)
	#define PSREG_AUDIO_MUTE			(1)
	//return mute status
	#define PSREG_AUDIO_MUTE_OFF		(0)
	#define PSREG_AUDIO_MUTE_ON		(1)
	//MuteID
	#define PSREG_AUDIO_MUTE_MAIN_APP				(1<<0)
	#define PSREG_AUDIO_MUTE_SYSTEM_APP				(1<<1)
	#define PSREG_AUDIO_MUTE_INTERACTIVE_APP		(1<<2)
	#define PSREG_AUDIO_MUTE_AVD						(1<<3)
	#define PSREG_AUDIO_MUTE_STREMING_PAUSE			(1<<4)

	
	//PSREG_STAT_ScrnSaverOn,
	//PSREG_STAT_ShowScreenSaver,
	#define PSREG_SCRN_SAVER_ON		(1)
	#define PSREG_SCRN_SAVER_OFF		(0)
	
	//PSREG_STAT_IP,
	#define PSREG_STAT_IPUnchanged 0 	
	#define PSREG_STAT_IPChanged 1
	#define PSREG_STAT_IPPppoeChanged 2
	#define PSREG_STAT_NETIF_UNCHANGED 0
	#define PSREG_STAT_NETIF_CHANGED 1
	
	//PSREG_STAT_ActiveSysApp,
	#define PSREG_ACTIVESYSAPP_NONE        (-1)
	
	//PSREG_STAT_MainAppStatus,
	#define PSREG_MAINAPPSTATUS_TRAYIN      (0)
	#define PSREG_MAINAPPSTATUS_TRAYOUT     (1)
	#define PSREG_MAINAPPSTATUS_DISCLOADING  (2)
	#define PSREG_MAINAPPSTATUS_NODISC	(3)
	#define PSREG_MAINAPPSTATUS_BDIndentify (4)
	#define PSREG_MAINAPPSTATUS_DVDIndentify  (5)
	#define PSREG_MAINAPPSTATUS_VCDIndentify  (6)
	#define PSREG_MAINAPPSTATUS_VRIndentify (7)
	#define PSREG_MAINAPPSTATUS_USB_ATTACH  (8)
	#define PSREG_MAINAPPSTATUS_USB_DETACH	(9)
	#define PSREG_MAINAPPSTATUS_CARD_ATTACH  (10)
	#define PSREG_MAINAPPSTATUS_CARD_DETACH  (11)
	#define PSREG_MAINAPPSTATUS_DATADISCIndentify  (12)
	#define PSREG_MAINAPPSTATUS_MAINMENU_SHOW  (13)
	#define PSREG_MAINAPPSTATUS_SETUPMENU_SHOW  (14)
	#define PSREG_MAINAPPSTATUS_SOURCEMENU_SHOW (15)
	#define PSREG_MAINAPPSTATUS_CDIndentify (16)
	#define PSREG_MAINAPPSTATUS_VFDSTART	(17)
	#define PSREG_MAINAPPSTATUS_VFD_END	(18)
	#define PSREG_MAINAPPSTATUS_DISC_ERROR (19)
	
	//PSREG_SMART_UPGRADE
	#define PSREG_SMART_UPGRADE_ON     	(1)
	#define PSREG_SMART_UPGRADE_OFF		(0)

	// PSREG_FILE_LOADER
	#define PSREG_FILE_LOADER_NO_INIT		(0) //SUPPORT_ISO_IMAGE
	#define PSREG_FILE_LOADER_INIT			(1)
	#define PSREG_FILE_LOADER_CLOSE			(2)
	#define PSREG_FILE_LOADER_OPEN    		(3)
	#define PSREG_FILE_LOADER_IDENTIFY_DISC	(4)
	#define PSREG_FILE_LOADER_PLAY			(5)
	#define PSREG_FILE_LOADER_STOP			(6)
	#define PSREG_FILE_LOADER_RESUME		(7)
	#define PSREG_FILE_LOADER_RESUME_CLOSE	(8)
	#define	PSREG_FILE_LOADER_DEV_REMOVE	(9)
	

	//PSREG	ContainerFileOrNavDiscFormat
	#include "file_def.h"	//Raymond Add
#define	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_UNKNOW	FILE_TYPE_UNKNOWN
#define	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_AVI			FILE_TYPE_AVI
#define	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_ASF			FILE_TYPE_ASF			
#define	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_QT			FILE_TYPE_QT				
#define	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_ND			FILE_TYPE_ND				
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_MPG		FILE_TYPE_MPG           		
#define	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_WAVE		FILE_TYPE_WAVE			
#define	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_CDXA		FILE_TYPE_CDXA			
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_OGG		FILE_TYPE_OGG           		
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_MP3			FILE_TYPE_MP3 				
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_AAC			FILE_TYPE_AAC 				
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_MKV			FILE_TYPE_MKV				
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_RMF			FILE_TYPE_RMF				
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_TS			FILE_TYPE_TS				
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_PSA			FILE_TYPE_TSPL_PSA		
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_PSV			FILE_TYPE_TSPL_PSV		
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_TSA			FILE_TYPE_TSPL_TSA		
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_TSV			FILE_TYPE_TSPL_TSV		
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_FLV			FILE_TYPE_FLV				
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_SWF		FILE_TYPE_SWF				
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_AC3			FILE_TYPE_AC3				
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_FLAC		FILE_TYPE_FLAC				
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_JPEG		FILE_TYPE_JPEG				
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_BMP			FILE_TYPE_BMP				
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_GIF			FILE_TYPE_GIF				
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_TIFF		FILE_TYPE_TIFF				
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_PNG			FILE_TYPE_PNG				
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_BD			FILE_TYPE_BD				
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_AVCHD		FILE_TYPE_AVCHD			
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_DVD_VIDEO	FILE_TYPE_DVD_VIDEO		
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_DVD_PLUS_VR	FILE_TYPE_DVD_PLUS_VR		
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_DVD_MINUS_VR	FILE_TYPE_DVD_MINUS_VR	
#define   PSREG_FILE_CONTAINER_NAV_DISC_TYPE_SVCD			FILE_TYPE_SVCD	
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_VCD20			FILE_TYPE_VCD20			
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_VCD11			FILE_TYPE_VCD11	
#define 	PSREG_FILE_CONTAINER_NAV_DISC_TYPE_CDDA			FILE_TYPE_CDDA			

	//PSREG_STAT_DiscUniqueID
	/*If don't need to cheat for specific disc, plz "remove" the specific definition, Don't worry about the compile issue. Raymond Add 2010.08.13*/
	#define PSREG_DISCID_MBD_KINGKONG	(0x40802334)	//NavData Use DTS Audio, However Stream is AC3
	#define PSREG_DISCID_INVALID			(PSREG_INVALID)

        //PSREG_STAT_CurPlayBackDeviceType
	#define  	PLAYBACK_DEVICE_TYPE_UNKNOWN 	(0)
	#define	PLAYBACK_DEVICE_TYPE_DISC		(1)
	#define	PLAYBACK_DEVICE_TYPE_USB			(2)
	#define	PLAYBACK_DEVICE_TYPE_CARD		(3)
	#define	PLAYBACK_DEVICE_TYPE_HDD			(4)
	#define	PLAYBACK_DEVICE_TYPE_NETWORK	(5)	
	#define	PLAYBACK_DEVICE_TYPE_FLASH		(6)
	#define	PLAYBACK_DEVICE_TYPE_MAX			(7)

	//PSREG_STAT_ApplicationMode
	#define PSREG_APPLICATION_MODE_PLAYBACK			(0)
	#define PSREG_APPLICATION_MODE_DVB				(1)
	#define PSREG_APPLICATION_MODE_DEFAULT			(PSREG_APPLICATION_MODE_PLAYBACK)

#if 1//(SUPPORT_CEC_TUNER == 1)
	// PSREG_TUNER_SIGNAL_STATUS
	#define PSREG_STAT_TUNER_NO_SIGNAL	(0) 
	#define PSREG_STAT_TUNER_SIGNAL		(1)
#endif	
//Defines for Individual PSREG Ends

#endif
