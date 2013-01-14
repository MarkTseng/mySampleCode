/*! \addtogroup HDMI_Interface
 *  @{
 */

/******************************************************************************/
/**
*
* \file		drv_hdmitx.h
*
*
* \brief	\n
*
* \note		Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
* 
* 
* \author	spike.yang@sunplus.com	
* 
******************************************************************************/
#ifndef _HDMI_H
#define _HDMI_H

///#include <stdio.h>
///#include <string.h>
//#include <ctype.h>
///#include <types.h>
///#include <gsl.h>

//ccyang
#undef EXTERN
#define	EXTERN extern

#ifdef	HDMI_EXTERN_HERE
#define	HDMIIF_EXTERN
#else
#define	HDMIIF_EXTERN		extern
#endif

//Test
//HDMI Configuration 
#define HDMI_OS_PRIORITY_BASE			(5)
#define HDMI_STACK_SIZE					(4096)

//HDMI ISR Configuration
#define HDMI_ISR_OS_PRIORITY_BASE		(5)
#define HDMI_ISR_STACK_SIZE				(2048)

//ISR Return Message definition
#define HDMI_ISR_CREATE_FAILED			(-1)		
#define HDMI_ISR_OPERATION_SUCCESS		0

//ISR Type
#define HISR_NONE	0
#define HISR_LINK	1
#define HISR_RI		2
#define HISR_AUTO_RI	3

/*
* Define a finction pointer.
*/
typedef int (*Func0)(int, int);///< fucntion pointer0


//HDMI Return Message definition
#define HDMI_ERROR_PARA					(-5)
#define HDMI_MODE_INVALID				(-4)
#define HDMI_EDID_INVALID				(-3)
#define HDMI_OPERATION_FAIL				(-2)
#define HDMI_CREATE_FAILED				(-1)
#define HDMI_OPERATION_SUCCESS			0


typedef enum HdmiEvent_t_
{
	HDMI_EVENT_EDID_VALID,
	HDMI_EVENT_EDID_INVALID,
	
} HdmiEvent_t_;

typedef enum HdmiFuncIntrOccur_e_
{
	HDMI_FUNC_IntrUnOccur,
	HDMI_FUNC_IntrOccur,
	
} HdmiFuncIntrOccur_e;


typedef enum HdmiSpecificEdidType_e_
{
	EDID_TYPE_VideoTiming,
	EDID_TYPE_ColorSpace,
	EDID_TYPE_DeepColor,
	
} HdmiSpecificEdidType_e;

typedef struct HdmiSpecificEdid_t_
{
	UINT32 						dId;
	HdmiSpecificEdidType_e		eType;
	UINT32						dSpecificFormat;
	UINT32						dExpectedFormat;
	
} HdmiSpecificEdid_t;

typedef struct Hdmi_MsgBody_t_ {
	
	UINT32 dMsg;
	UINT32 dParam;

}Hdmi_MsgBody_t;

typedef struct HdmiIntrGroup_t_ {
	
	UINT8 Intr1;
	UINT8 Intr2;
	UINT8 Intr3;
	UINT8 fIntrServiced;

} HdmiIntrGroup_t;

#define HDMI_MSG_BODY_SIZE		(sizeof(Hdmi_MsgBody_t))

#define HDMI_MSG_MASK			(0x0000FFFF)
#define HDMI_MSG_TYPE_MASK		(0xFFFF0000)

#define HDMI_ISR_MSG_TYPE		(1<<28)
#define HDMI_CMD_MSG_TYPE		(0)

//HDMI ISR Msg
#define HDMI_HPD_IN_MSG			(1)
#define HDMI_HPD_OUT_MSG		(2)
#define HDMI_RSEN_IN_MSG		(3)
#define HDMI_RSEN_OUT_MSG		(4)
#define HDMI_RI128_ERR_MSG		(5)
#define HDMI_RI_ERR_MSG			(6)
#define HDMI_HDCP_FAIL_RETRY	(7)
#define HDMI_RSEN_POWER_ON_MSG	(8)

//HDMI CMD Msg
#define HDMI_CMD_INIT			(1)
#define HDMI_CMD_SET_DISP		(2)
#define HDMI_CMD_SET_AUD		(3)
#define HDMI_ASK_CHANGE_DISP	(4)
#define HDMI_ASK_CHANGE_AUDIO	(5)
#define HDMI_ACK_CHANGE_DISP	(6)
#define HDMI_ACK_CHANGE_AUDIO	(7)
#define HDMI_CMD_CSS_ENABLE		(8)
#define HDMI_CMD_CSS_DISABLE	(9)
#define HDMI_CMD_SET_DEEPCOLOR	(10)
#define HDMI_CMD_SET_COLORSPACE	(11)
#define HDMI_CMD_SRM_ENABLE		(12)
#define HDMI_CMD_SRM_DISABLE	(13)
#define HDMI_CMD_POWER_ON		(14)
#define HDMI_CMD_POWER_OFF		(15)
#define HDMI_CMD_SET_AFD		(16)
#define HDMI_CMD_TMDS_ON		(17)
#define HDMI_CMD_TMDS_OFF		(18)
#define HDMI_CMD_HDCP_ENABLE	(19)
#define HDMI_CMD_HDCP_DISABLE	(20)
#define HDMI_CMD_STATE_RECOVER	(21)

/**
 *	\brief	HDMI internal state. Only for HDMI programer.
 */
typedef enum HdmiTxState_t_
{
	HDMI_INIT = 0,		///< 
	HDMI_HPD,			///< HDMI Modual in HPD State, it means HDMI connected
	HDMI_RSEN,			///< HDMI Modual in RSEN State, it means Tx and Rx in same port.
	HDMI_HDCP,			///< HDMI Modual in HDCP State, it means HDCP is working.
	HDMI_MAX,
	
}HdmiTxState_t;



/**
 * \defgroup HDMI_MODUAL_CTRL HDMI Basic
 * \brief It provides interface to control HDMI Modual Status.
 * @{ */

/**
 *	\brief	HDMI Model Parameter Initialized.\n
 */
typedef struct {
	
	Func0 pfHdmiCallback;	///< HDMI Callback Function hooked.

} HdmiMoualConfig_t;

/**
 *	\brief	HDMI internal fucntions (SRM, CPS etc) enabled/disabled enum.
 */
typedef enum HdmiFuncEn_e_
{
	FC_DISABLE = 0,			///< Disable HDMI internal function.
	FC_ENABLE,				///< Enable HDMI internal fucntion.
} HdmiFuncEn_e;

typedef enum HdmiPowerCtrl_e_
{
	HDMI_POWER_OFF = 0,
	HDMI_POWER_ON,
	
} HdmiPowerCtrl_e;

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Initialize(HdmiMoualConfig_t *tConfig)
 *
 * \brief		Initialize HDMI Modual.
 *
 * \param		tConfig: Configuration for HDMI Modual
 *
 * \return		\e HDMI_OPERATION_SUCCESS when initialization process is successfully completed.\n
 *				\e HDMI_CREATE_FAILED when initialization process is fail.\n
 *
 * \note		HDMI_IF_Initialize must be called before using other HDMI_IF.
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Initialize(HdmiMoualConfig_t *tConfig);
 
 
/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Finalize(void)
 *
 * \brief		Finalize HDMI Modual.
 *
 * \return		\e HDMI_OPERATION_SUCCESS when un-initialization process is successfully completed.\n
 *				\e HDMI_CREATE_FAILED problem occurs during un-initialization process.\n
 *
 * \note		HDMI_IF_Finalize must be called after HDMI_IF_Initialize successfully.
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Finalize(void);

/******************************************************************************************/
/**
 * \fn			void HDMI_IF_GetVersion(UINT32* pdVersion)
 *
 * \brief		Check HDMI Module Version.
 *
 * \param		pdVersion: Return Version.
 ******************************************************************************************/
HDMIIF_EXTERN void HDMI_IF_GetVersion(UINT32* pdVersion);

/*! @} end of HDMI_MODUAL_CTRL*/ 


/**
 * \defgroup HDMI_MODUAL_OTHER The Others
 * \brief It provides interface to control the Other HDMI Modual Status.
 * @{ */

/******************************************************************************************/
/**
 * \fn			UINT8 HDMI_IF_Get_Connection(void)
 *
 * \brief		Check the connection of HDMI.
 *
 * \return		\e 1: HDMI Connected, 0: HDMI un-Connected\n
 *
 * \note		HDMI_IF_Finalize must be called after HDMI_IF_Initialize successfully.
 ******************************************************************************************/
HDMIIF_EXTERN UINT8 HDMI_IF_Get_Connection(void);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Get_HDCP(HdmiFuncEn_e *eReturnOnOff)
 *
 * \brief		Check HDCP is Enable/Disable in HDMI.
 *
 * \param		eReturnOnOff: return HDCP function is on/off.
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *
 * \note
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Get_HDCP(HdmiFuncEn_e *eReturnOnOff);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Set_NES(HdmiFuncEn_e eFcOn)
 *
 * \brief		HDMI enter/exit NES game.
 *
 * \param		eFcOn: enter/exit NES Controled
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *
 * \note
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Set_NES(HdmiFuncEn_e eFcOn);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Set_Power(HdmiPowerCtrl_e ePowerOn)
 *
 * \brief		Turn On/Off HDMI
 *
 * \param		ePowerOn: Turn On/Off HDMI.
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *
 * \note
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Set_Power(HdmiPowerCtrl_e ePowerOn);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Set_TMDSOff(void)
 *
 * \brief		For Display Middleware. Close TMDS Signal to aviod video garbege
 *
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *
 * \note		It will also disable HDMI Interrupt Routine.
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Set_TMDSOff(void);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Set_Cec(HdmiFuncEn_e eFcOn)
 *
 * \brief		CEC function Enable/Disable.\n
 *
 * \param		eFcOn: Enable/Disable CEC Controled.
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *
 * \note
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Set_Cec(HdmiFuncEn_e eFcOn);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_SpecificEdidTablechecked(HdmiSpecificEdidType_e eType, UINT32 dFmt, UINT32 *RetExpectedFmt, UINT8 *RetHit)
 *
 * \brief		HDMI Check Specific EDID Table for Special Rx.\n
 *
 * \param		eType: Check Which kind of Edid Block Type\n
 *				dFmt: Check which kind of format\n
 *				RetExpectedFmt: Good choice for dFmt\n
 *				RetHit: dFmt is hit in Specific EDID Table\n
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *
 * \note
 ******************************************************************************************/
int HDMI_IF_SpecificEdidTablechecked(HdmiSpecificEdidType_e eType, UINT32 dFmt, UINT32 *RetExpectedFmt, UINT8 *RetHit);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Set_SpecificEdidTable(HdmiSpecificEdid_t *apstSpEdidTbl, UINT32 dSize)
 *
 * \brief		Asign HDMI a specific table for checking special TVs(or Rx)'s rule.
 *
 * \param		apstSpEdidTbl: Enable/Disable SRM Controled
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when Table is illegal\n
 *
 * \note
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Set_SpecificEdidTable(HdmiSpecificEdid_t *apstSpEdidTbl, UINT32 dSize);

/*! @} end of HDMI_MODUAL_OTHER*/ 



/**
 * \defgroup HDMI_CONTENT_PROTECTION Content Protection
 * \brief It provides interface to control Content Protection of HDMI Modual.
 * @{ */

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Set_CPS(HdmiFuncEn_e eFcOn)
 *
 * \brief		HDMI (Digital) CPS (Content Proction System) function Enable/Disable.
 *
 * \param		eFcOn: Enable/Disable CPS Controled
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *
 * \note
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Set_CPS(HdmiFuncEn_e eFcOn);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Set_HDCP(HdmiFuncEn_e eFcOn)
 *
 * \brief		HDMI HDCP function Enable/Disable.
 *
 * \param		eFcOn: Enable/Disable HDCP Controled
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *
 * \note
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Set_HDCP(HdmiFuncEn_e eFcOn);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Set_SRM(HdmiFuncEn_e eFcOn)
 *
 * \brief		HDMI SRM (System renewability Message) function Enable/Disable.
 *
 * \param		eFcOn: Enable/Disable SRM Controled
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *
 * \note
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Set_SRM(HdmiFuncEn_e eFcOn);

/*! @} end of HDMI_MODUAL_OTHER*/ 



/**
 * \defgroup HDMI_DISPLAY HDMI Display
 * \brief It provides interface to control HDMI Display and get some information about it.
 * \{ */

/**
 *	\brief	HDMI Color Space Enumberation.\n
 *			Please refer to HDMI 1.3 Spec.
 */
typedef enum ColorSpaceFormat_t_
{
	HDMI_CSF_RGB = 0,		///< RGB
	HDMI_CSF_YC422,			///< YCbCr 4:2:2
	HDMI_CSF_YC444,			///< YCbCr 4:4:4
	HDMI_CSF_XVCOLOR,		///< xvColor
	HDMI_CSF_MAX,
	
}ColorSpaceFormat_t;

/**
 *	\brief	HDMI Deep Color(Pixel depth) Enumberation.\n
 *			Please refer to HDMI 1.3 Spec.
 */
typedef enum DeepColorFormat_t_
{
	HDMI_DCF_8_BITS = 0x00,		///< 24 bits per pixel.\n
	HDMI_DCF_10_BITS = 0x01,	///< 30 bits per pixel.\n
	HDMI_DCF_12_BITS = 0x02,	///< 36 bits per pixel.\n
	HDMI_DCF_16_BITS = 0x03,	///< 48 bits per pixel, unsupported!\n
	HDMI_DCF_MAX,
	
}DeepColorFormat_t;

/**
 *	\brief 	HDMI Resolution Enumberation.\n
 *			Please refer to CEA-861D, Video ID Code.
 */
typedef enum ResolutionFormat_t_
{
	//NTSC
	//VIDC_640x480p_60hz_43 = 1,
	VIDC_720x480p_60hz_43 = 2,			///< Video ID Code 2
	VIDC_720x480p_60hz_169 = 3,			///< Video ID Code 3
	VIDC_1280x720p_60hz_169 = 4,		///< Video ID Code 4
	VIDC_1920x1080i_60hz_169 = 5,		///< Video ID Code 5
	VIDC_720x480i_60hz_43 = 6,			///< Video ID Code 6
	VIDC_720x480i_60hz_169 = 7,			///< Video ID Code 7
	VIDC_1920x1080p_60hz_169 = 16,		///< Video ID Code 16
	
	//PAL
	VIDC_720x576p_50hz_43 = 17,			///< Video ID Code 17
	VIDC_720x576p_50hz_169 = 18,		///< Video ID Code 18
	VIDC_1280x720p_50hz_169 = 19,		///< Video ID Code 19
	VIDC_1920x1080i_50hz_169 = 20,		///< Video ID Code 20
	VIDC_720x576i_50hz_43 = 21,			///< Video ID Code 21
	VIDC_720x576i_50hz_169 = 22,		///< Video ID Code 22
	VIDC_1920x1080p_50hz_169 = 31,		///< Video ID Code 31
	
	//1080p 24Hz
	VIDC_1920x1080p_24hz_169 = 32,		///< Video ID Code 32
	
	VIDC_MAX = 128,
} ResolutionFormat_t;


/**
 *	\brief 	HDMI System Resolution Enumberation.\n
 *			Please refer to HDMI 1.3a Spec, Page 105, Video Format Timing Table.
 */
typedef enum VideoFormatTiming_t_
{
	VFT_VGA,
	VFT_480_576i_PixRep2 = 1,
	VFT_480_576i_PixRep4 = 2,
	VFT_240_288p_PixRep2,
	VFT_240_288p_PixRep4,
	VFT_480_576p_PixRep0 = 5,
	VFT_480_576p_PixRep2,
	VFT_480_576p_PixRep4,
	VFT_720p_PixRep0 = 8,
	VFT_1080i_PixRep0 = 9,
	VFT_1080p_PixRep0 = 10,
	VFT_1080i_1250_PixRep0,
	VFT_480_576i_x2Freq_PixRep2,
	VFT_480_576p_x2Freq_PixRep0,
	VFT_MAX,

} VideoFormatTiming_t;


/**
 * \defgroup HDMI_FORMAT Color Format
 * \brief It provides interface to control HDMI Color Format and get some information about it.
 * \{ */

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_ColorSpaceChecked(ColorSpaceFormat_t ColorSpaceFormat, UINT8* RetSupported)
 *
 * \brief		Check whether can Receiver support this (ColorSpace) format.
 *
 * \param		ColorSpaceFormat:	Which format to be checked?
 * \param		RetSupported:		*RetSupported 1:supported, 0:unsupported.
 *
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_EDID_INVALID HDMI don't get EDID from Reciver(TV)\n
 *
 * \note		
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_ColorSpaceChecked(ColorSpaceFormat_t ColorSpaceFormat, UINT8* RetSupported);


/******************************************************************************************/
/**
 * \fn			int HDMI_IF_DeepColorChecked(DeepColorFormat_t DeepColorFormat, UINT8* RetSupported)
 *
 * \brief		Check whether can Receiver support this (DeepColor) format.
 *
 * \param		DeepColorFormat: 	Which format to be checked?
 * \param		RetSupported:		*RetSupported 1:supported, 0:unsupported.
 *
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_EDID_INVALID HDMI don't get EDID from Reciver(TV)\n
 *
 * \note		
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_DeepColorChecked(DeepColorFormat_t DeepColorFormat, UINT8* RetSupported);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Get_PreferColorSpace(ColorSpaceFormat_t *eReturnFmt)
 *
 * \brief		Get Prefer Color Space by Rx and Tx.
 *
 * \param		*eReturnFmt:	Return Prefer Color Space Format.
 *
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_EDID_INVALID HDMI don't get EDID from Reciver(TV)\n
 *
 * \note		Returned Format is in Parameter.
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Get_PreferColorSpace(ColorSpaceFormat_t *eReturnFmt);

HDMIIF_EXTERN int HDMI_IF_Get_PreferColorSpaceByDC(ColorSpaceFormat_t *eReturnFmt, DeepColorFormat_t DeepColorFormat);
/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Get_PreferDeepColor(DeepColorFormat_t *eReturnFmt)
 *
 * \brief		Get Prefer Deep Color by Rx and Tx.
 *
 * \param		*eReturnFmt:	Return Prefer Deep Color Format.
 *
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_EDID_INVALID HDMI don't get EDID from Reciver(TV)\n
 *
 * \note		Returned Format is in Parameter.
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Get_PreferDeepColor(DeepColorFormat_t *eReturnFmt);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Get_ColorSpace(ColorSpaceFormat_t *eReturnFmt)
 *
 * \brief		Get Color Space.
 *
 * \param		*eReturnFmt:	Return Color Space at this time.
 *
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *
 * \note		Returned Format is in Parameter.
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Get_ColorSpace(ColorSpaceFormat_t *eReturnFmt);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Get_DeepColor(DeepColorFormat_t *eReturnFmt)
 *
 * \brief		Get Deep Color.
 *
 * \param		*eReturnFmt:	Return Deep Color at this time.
 *
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *
 * \note		Returned Format is in Parameter.
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Get_DeepColor(DeepColorFormat_t *eReturnFmt);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Set_ColorSpaceMode(ColorSpaceFormat_t HdmiColorSpaceMode)
 *
 * \brief		Check whether can Receiver support this (Resolution) format.
 *
 * \param		HdmiColorSpaceMode:	Change to which (ColorSpace) format?
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *
 * \note		HDMI Machine won't reset.
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Set_ColorSpaceMode(ColorSpaceFormat_t HdmiColorSpaceMode);


/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Set_DeepColorMode(DeepColorFormat_t HdmiDeepColorMode)
 *
 * \brief		Config HDMI to the DeepColor format choosed.
 *
 * \param		HdmiDeepColorMode:	Change to which (DeepColor) format?
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *
 * \note		HDMI Machine will reset.
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Set_DeepColorMode(DeepColorFormat_t HdmiDeepColorMode);

/** 
 * \}
 * end of HDMI_FORMAT*/ 


/**
 * \defgroup HDMI_RESOLUTION Resolution
 * \brief It provides interface to control HDMI Resolution and Timing and get some information about it.
 * \{ */

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_ResolutionChecked(ResolutionFormat_t ResolutionFormat, UINT8* RetSupported)
 *
 * \brief		Check whether can Receiver support this (Resolution) format.
 *
 * \param		ResolutionFormat:	Which format to be checked?
 * \param		RetSupported:		*RetSupported 1:supported, 0:unsupported.
 *
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_EDID_INVALID HDMI don't get EDID from Reciver(TV)\n
 *
 * \note		
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_ResolutionChecked(ResolutionFormat_t ResolutionFormat, UINT8* RetSupported);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Get_PreferResolution(ResolutionFormat_t *eReturnFmt)
 *
 * \brief		Get Prefer Resolution by Rx and Tx.
 *
 * \param		*eReturnFmt:	Return Prefer Resolution Format.
 *
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_EDID_INVALID HDMI don't get EDID from Reciver(TV)\n
 *
 * \note		Returned Format is in Parameter.
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Get_PreferResolution(ResolutionFormat_t *eReturnFmt);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Get_VideoTiming(ResolutionFormat_t *eReturnFmt)
 *
 * \brief		Get Video Timing.
 *
 * \param		*eReturnFmt:	Return Video Timing at this time.
 *
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *
 * \note		Returned Format is in Parameter.
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Get_VideoTiming(ResolutionFormat_t *eReturnFmt);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Set_VideoTiming(ResolutionFormat_t ResolutionFormat)
 *
 * \brief		For Display Middleware. Config HDMI to the (resolution) format choosed.
 *
 * \param		ResolutionFormat:	Change to which (Resolution) format?
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *
 * \note		HDMI Machine will reset.
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Set_VideoTiming(ResolutionFormat_t ResolutionFormat);

/** 
 * \}
 * end of HDMI_RESOLUTION*/ 

/**
 * \defgroup HDMI_PACKET HDMI Packet
 * \brief It provides interface to control HDMI Display and get some information about it.
 * \{ */

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Set_XVContent(HdmiFuncEn_e eFcOn)
 *
 * \brief		HDMI xvColor Content function Enable/Disable.\n
 *				It also need to check if Rx can support it, and System's colorspace configuration.
 *
 * \param		eFcOn: Set xvColor content
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *				\e HDMI_EDID_INVALID when Rx compability is unknown.\n
 *				\e HDMI_MODE_INVALID when xvColor compabiltiy is fail. It also happen if Auto/Xv Colorspace isn't choosed.
 *
 * \note
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Set_XVContent(HdmiFuncEn_e eFcOn);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Set_AFD(UINT8 bAFDValue)
 *
 * \brief		AFD
 *
 * \param		bAFDValue:	AFD Value
 *
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *
 * \note		
 ******************************************************************************************/
int HDMI_IF_Set_AFD(UINT8 bAFDValue);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Set_AVMute(void)
 *
 * \brief		For Display Middleware. Mute HDMI A/V Mute 
 *
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *
 * \note		it will pause HDMI Machine.\n
 				step: 1.Set Mute 2.Set TMDS Off 3.Set Video Timing\n
 				Be careful if any other HDMI IF let HDMI Machine run.
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Set_AVMute(void);

/** 
 * \}
 * end of HDMI_PACKET*/ 

/** 
 * \}
 * end of HDMI_DISPLAY*/ 
 
 


/**
 * \defgroup HDMI_AUDIO HDMI Audio
 * \brief It provides interface to control HDMI Modual Status.
 * \{ */

typedef enum HDMI_CGMS_Para_e_
{
	CGMS_UnlimitCopy	=0x00,
	CGMS_NotUsed,
	CGMS_OneCopyAllowed,
	CGMS_NoCopyAllowed,
	
} HDMI_CGMS_Para_e;

/**
 *	\brief	HDMI Audio Sample Rate Enumberation.\n
 *			Config HDMI Audio Sample Rate.
 */
typedef enum HdmiAudioSampleRate_e_
{
	HDMI_AUD_FS_32K,		///< Sample rate 32k
	HDMI_AUD_FS_44K,		///< Sample rate 44k
	HDMI_AUD_FS_48K,		///< Sample rate 48k
	HDMI_AUD_FS_88K,		///< Sample rate 88k
	HDMI_AUD_FS_96K,		///< Sample rate 96k
	HDMI_AUD_FS_176K,		///< Sample rate 176k
	HDMI_AUD_FS_192K,		///< Sample rate 192k
#if 1	//#ifdef HDMI_AUDIO_SUPPORT_128K //hdmi_cfg.h is not included
	HDMI_AUD_FS_128K,		///< Sample rate 128k
#endif
	//HDMI_AUD_FS_MAX,

}HdmiAudioSampleRate_e;

/**
 *	\brief	HDMI Audio Sample Ratio Enumberation.\n
 *			Config HDMI Audio Sample Ratio.
 */
typedef enum HdmiAudioSampleRatio_e_
{
	HDMI_AUD_SAMPLE_RATIO_1,		///< Sample Ratio x1
	HDMI_AUD_SAMPLE_RATIO_1_2,		///< Sample Ratio x1/2
	HDMI_AUD_SAMPLE_RATIO_1_4,		///< Sample Ratio x1/4
	HDMI_AUD_SAMPLE_RATIO_MAX,

}HdmiAudioSampleRatio_e;

typedef enum HdmiAudioFrameRate_e_
{
	HDMI_AUD_FR_192,
	HDMI_AUD_FR_384,
	HDMI_AUD_FR_768,

}HdmiAudioFrameRate_e;

/**
 *	\brief	HDMI Audio Input Enumberation.\n
 *			Config HDMI Audio input path.
 */
typedef enum HdmiAudioInput_e_
{
	HDMI_AUD_IN_SPDIF,		///< HDMI Audio Input by SPDIF
	HDMI_AUD_IN_I2S,		///< HDMI Audio Input by I2S
	//HDMI_AUD_IN_MAX,

}HdmiAudioInput_e;

/**
 *	\brief	HDMI Audio Output Enumberation.\n
 *			Config HDMI Audio output path.
 */
typedef enum HdmiAudOutput_e_
{
	HDMI_AUD_OUT_PCM2,		///< HDMI Audio Output 2 channels PCM
	HDMI_AUD_OUT_PCMx,		///< HDMI Audio Output n channels PCM(2<n<=8)
	HDMI_AUD_OUT_HBR,		///< HDMI Audio Output high bit rate(HBR)
	
} HdmiAudOutput_e;

/**
 *	\brief	HDMI Audio Channels Enumberation.\n
 *			Config HDMI Audio input/output channel number.
 */
typedef enum HdmiAudChannels_e_
{
	//HDMI_AUD_1Ch = 0,		//Unused, Can transform
	HDMI_AUD_2Ch = 1,		///< 2 channels 
	HDMI_AUD_3Ch = 2,		///< 3 channels 
	HDMI_AUD_4Ch = 3,		///< 4 channels 
	HDMI_AUD_5Ch = 4,		///< 5 channels 
	HDMI_AUD_6Ch = 5,		///< 6 channels 
	HDMI_AUD_7Ch = 6,		///< 7 channels 
	HDMI_AUD_8Ch = 7,		///< 8 channels 
	
}HdmiAudChannels_e;


/**
 *	\brief	HDMI AudioCode Enumberation.\n
 *			Please refer to CEA-861D.
 */
typedef enum AudioCodeFormat_e_
{
	HDMI_ACF_Rsvd0 = 0,		///< Reversed,
	HDMI_ACF_LPCM,			///< LPCM
	HDMI_ACF_AC3,			///< AC3
	HDMI_ACF_MPEG1,			///< MPEG-1
	HDMI_ACF_MP3,			///< MP3
	HDMI_ACF_MPEG2,			///< MPEG-2
	HDMI_ACF_AAC,			///< AAC
	HDMI_ACF_DTS,			///< DTS
	HDMI_ACF_ATRAC,			///< ATRAC
	HDMI_ACF_OneBitAud,		///< One bit audio
	HDMI_ACF_DDPlus,		///< DD+
	HDMI_ACF_DtsHd,			///< DTS-HD
	HDMI_ACF_MAT,			///< MAT
	HDMI_ACF_DST,			///< DST
	HDMI_ACF_WMAPro,		///< WMA Pro
	HDMI_ACF_Rsvd15 = 15,	///< Reversed,
	HDMI_ACF_ExDtsHdMaster = 32, ///< DTS-HD Master
	HDMI_ACF_None = 63,		///< None
}AudioCodeFormat_e;

typedef enum
{
	HDMI_MAUD_Format = 0,
	HDMI_MAUD_Channels,
	HDMI_MAUD_Sample,
	HDMI_MAUD_Full,
	
} HdmiAudMaskType_e;

typedef struct
{
	UINT32 dAudioCodeInfo[2];
	HdmiAudChannels_e bMaxChannels;
	UINT8 bSampleRateInfo;
	
} AudioFormMask_t;

typedef struct
{
	AudioCodeFormat_e eAudFmt;
	HdmiAudChannels_e bMaxChannels;
	UINT8 bSampleRateInfo;
	UINT8 bExData;
	
} ShortAudioForm_t;

/**
 *	\brief HDMI Set Audio Config.\n
 */
typedef struct {

	HdmiAudioInput_e 		eInput;				///<HDMI Audio Input Foramt(I2S/SPDIF)
	HdmiAudOutput_e			eOutput;			///<HDMI Audio Output Format(PCM 2Channel/PCM xChannel/High Bit Rate)
	HdmiAudioSampleRate_e	eSampleRate;		///<Sample rate
	HdmiAudChannels_e		eChannels;			///<Channel Counts
	UINT8					bChannelsAlloc;		///<Reversed, 0x00~0x1F
	UINT8					bLvShift;			///<Reversed, 0x00~0x0f
	UINT8					fDownMixInhibit;	///<Reversed,
	
} HdmiAudConfig_t;

typedef struct HdmiAudLimitByVideo_st_
{
	HdmiAudioSampleRate_e	eMaxFsIn8Ch;
	HdmiAudioFrameRate_e	eMaxFrIn2Ch;
	
} HdmiAudLimitByVideo_st;

EXTERN HdmiAudConfig_t hdmi_audcfg;	//ccyang

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Get_AudioLimitation(HdmiAudLimitByVideo_st *pstAudioLimitation)
 *
 * \brief		Get HDMI Audio Limitation by Video.
 *
 * \param		pstAudioLimitation:	HDMI Audio Limitation.
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *
 * \note		None.
 ******************************************************************************************/
int HDMI_IF_Get_AudioLimitation(HdmiAudLimitByVideo_st *pstAudioLimitation);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Set_Audio(HdmiAudConfig_t tHdmiAud)
 *
 * \brief		Config HDMI Audio.
 *
 * \param		tHdmiAud:	HDMI Audio Configuration
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *
 * \note		before setting, Mute HDMI Audio First.
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Set_Audio(HdmiAudConfig_t tHdmiAud);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_AudioCodeChecked(AudioCodeFormat_e eAudioCodeFormat, UINT8* RetSupported)
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_AudioCodeChecked(AudioCodeFormat_e eAudioCodeFormat, UINT8* RetSupported);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_AudioCodeDetail(AudioCodeFormat_e eAudioCodeFormat, ShortAudioForm_t *ptRetAudDetail)
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_AudioCodeDetail(AudioCodeFormat_e eAudioCodeFormat, ShortAudioForm_t *ptRetAudDetail);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Set_AudioDisabled(void)
 *
 * \brief		For avoid audio pop noise
 *
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *
 * \note
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Set_AudioDisabled(void);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Set_AudioEnabled(void)
 *
 * \brief		For avoid audio pop noise
 *
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *
 * \note
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Set_AudioEnabled(void);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Set_AudioGMS(HDMI_CGMS_Para_e eCGMSPara)
 *
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Set_AudioGMS(HDMI_CGMS_Para_e eCGMSPara);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_Set_AudioMute(void)
 *
 * \brief		Mute HDMI Audio
 *
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 *
 * \note
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Set_AudioMute(void);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_AudioMasked(HdmiAudMaskType_e eType, AudioFormMask_t tSetAudMask)
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_AudioMasked(HdmiAudMaskType_e eType, AudioFormMask_t tSetAudMask);

/******************************************************************************************/
/**
 * \fn			int HDMI_IF_AudioMaskGet(AudioFormMask_t *tGetAudMask)
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_AudioMaskGet(AudioFormMask_t *tGetAudMask);

/******************************************************************************************/
/**
 * \fn			nt HDMI_IF_Set_AudioSampleRatio(HdmiAudioSampleRatio_e eRatio)
 *
 * \brief		Setup Audio Down Sample Ratio
 *
 * \param		eRatio: Audio Down Sample Ratio 1, 1/2, 1/4.
 *
 * \return		\e HDMI_OPERATION_SUCCESS when process is successfully completed.\n
 *				\e HDMI_OPERATION_FAIL when process is fail! HDMI un-initialized!\n
 ******************************************************************************************/
HDMIIF_EXTERN int HDMI_IF_Set_AudioSampleRatio(HdmiAudioSampleRatio_e eRatio);

/** 
 * \}
 * end of HDMI_AUDIO*/ 







HDMIIF_EXTERN int HDMI_IF_TestISR(void); //SpikeYang Test Function
HDMIIF_EXTERN int HDMI_IF_TestISR2(void);

#endif

/*! @} end of addtogroup HDMI*/

