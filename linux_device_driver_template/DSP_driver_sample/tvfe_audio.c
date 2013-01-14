/*!
 * @addtogroup audio
 * @{
 *	 @addtogroup audio
 *	 @{
 */

/*!
 * @file	Tvfe_audio.c
 * @brief	tv decoder timing monitor task
 * @note	Copyright (c) 2009 Sunplus Technology Co., Ltd.
 *			All rights reserved.
 *			19, Innovation First Road, Hsinchu Science Park, 300 Taiwan
 * @author	Leon
 */
/*******************************************************************************
 * Header Files
 ******************************************************************************/
//#include "tvfe_internal.h"
#include <asm/mach-spv9200/types.h>
#include <asm/mach-spv9200/ic-spv9200/hw_public.h>
#include <asm/mach-spv9200/regmap_9200.h>
#include <linux/types.h>
#include "drv_common.h"
#include "drv_ssd.h"
#include "include/aud_global.h"
#include "include/auddrv.h"
#include "include/tvfe_api.h"
#include "include/tvfe_audio.h"



//Tvfe_api.c

UINT8 g_u8SifCurrentMode;
UINT8 g_u8SifAvailableMode=TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
static TVFE_SoundStd_e geSoundStdType = TVFE_SOUND_STANDARD_AUTO;

#define _CHECK_VAILDPARAM(x) { \
	if ((x) == DTV_NULL) { \
		return TVFE_ERR_INVALID_PARAM; \
	}\
}

/*****************************************************************************
** FUNCTION : TVFE_SetTunerValueType
**
** DESCRIPTION :
**				Set tuner standard enum.
**
** PARAMETERS :
**				eSoundStdType: drv sound standard.
**
** RETURN VALUES:
**				TVFE_SUCCESS
**				TVFE_ERR_INVALID_PARAM
*****************************************************************************/
TVFE_Status_e TVFE_SetTunerValueType(TVFE_SoundStd_e eSoundStdType)
{
	/* This function should be called by playback_play of mid */
	geSoundStdType = eSoundStdType;
	return TVFE_SUCCESS;
}

/*****************************************************************************
** FUNCTION : TVFE_GetTunerValueType
**
** DESCRIPTION :
**				Get tuner standard enum.
**
** PARAMETERS :
**				*peSoundStdType: drv tuner standard.
**
** RETURN VALUES:
**				TVFE_SUCCESS
**				TVFE_ERR_INVALID_PARAM
*****************************************************************************/
TVFE_Status_e TVFE_GetTunerValueType(TVFE_SoundStd_e *peSoundStdType)
{
	_CHECK_VAILDPARAM(peSoundStdType);
	*peSoundStdType = geSoundStdType;
	return TVFE_SUCCESS;
}

/*****************************************************************************
** FUNCTION : TVFE_GetSIFWorkingMode
**
** DESCRIPTION :
**				Get current SIF working mode.
**
** PARAMETERS :
**				*peMode		current working sif mode
**
** RETURN VALUES:
**				TVFE_SUCCESS
**				TVFE_ERR_INVALID_PARAM
**
*****************************************************************************/
TVFE_Status_e TVFE_GetSIFWorkingMode(TVFE_AudioSIFMode_e *peCurrentMode)
{
	_CHECK_VAILDPARAM(peCurrentMode);
	*peCurrentMode = g_u8SifCurrentMode;
	return TVFE_SUCCESS;
}

/*****************************************************************************
** FUNCTION : TVFE_SetSifAvailableMode
**
** DESCRIPTION :
**				Set sif available mode.
**
** PARAMETERS :
**				eSIFAvailableMode: Sif available mode.
**
** RETURN VALUES:
**				TVFE_SUCCESS
**				TVFE_ERR_INVALID_PARAM
*****************************************************************************/
TVFE_Status_e TVFE_SetSifAvailableMode(TVFE_AudioSIFAvailableMode_e eSIFAvailableMode)
{
	g_u8SifAvailableMode = eSIFAvailableMode;
	return TVFE_SUCCESS;
}

/*****************************************************************************
** FUNCTION : TVFE_GetSifAvailableMode
**
** DESCRIPTION :
**				Get sif available mode.
**
** PARAMETERS :
**				*peSifAvaiMode: Sif available mode.
**
** RETURN VALUES:
**				TVFE_SUCCESS
**				TVFE_ERR_INVALID_PARAM
*****************************************************************************/
TVFE_Status_e TVFE_GetSifAvailableMode(TVFE_AudioSIFAvailableMode_e *peSifAvaiMode)
{
	_CHECK_VAILDPARAM(peSifAvaiMode);
	*peSifAvaiMode = g_u8SifAvailableMode;
	return TVFE_SUCCESS;
}

/*****************************************************************************
** FUNCTION : TVFE_SIF_SetCurrentMode
**
** DESCRIPTION :
**				This function is used to Set current SIF mode
**
** PARAMETERS :
**				eSifMode
**
** RETURN VALUES:
**				TVFE_SUCCESS
**
*****************************************************************************/
TVFE_Status_e TVFE_SIF_SetCurrentMode(TVFE_AudioSIFMode_e eSifMode)
{
	if((eSifMode != TVFE_AUDIO_SIF_MODE_MAX) && (g_u8SifCurrentMode != eSifMode))
	{
		/* Mute Audio */
//		TVFE_Audio_SetMute(TVFE_AUDIO_MUTE_PRIMARY_PATH, DTV_TRUE);
//		TVFE_Audio_SetMute(TVFE_AUDIO_MUTE_AUX2_PATH, DTV_TRUE);
		/* 090624 Fasky added for mantis 59976 */
		//GL_TaskSleep(60);
		TVFE_AudioSetSIFMode(TVFE_AUDIO_PATH_PRIMARY, eSifMode);
		/* Edward add 20090423 for apply sound type in line out */
		TVFE_AudioSetSIFMode(TVFE_AUDIO_PATH_AUX1, eSifMode);
		/* 090624 Fasky added for mantis 59979 */
		//GL_TaskSleep(150);

//		TVFE_Audio_SetMute(TVFE_AUDIO_MUTE_PRIMARY_PATH, DTV_FALSE);
//		TVFE_Audio_SetMute(TVFE_AUDIO_MUTE_AUX2_PATH, DTV_FALSE);
	}
	g_u8SifCurrentMode = eSifMode;

	return TVFE_SUCCESS;
}




// part of tvfe_tvdec.c

AudioManagerParam_t gAudioPrimaryParam;

#if 0
typedef struct _TVFE_Audio_Aux_Path_t
{
	//GL_Mutex_t mutexID;
	AUDIF_Aux1Path_e Aux1Path;
	AUDIF_Aux2Path_e Aux2Path;
} TVFE_Audio_Aux_Path_t;

typedef struct _TVFE_Audio_Dac_Mapping_t
{
	//GL_Mutex_t mutexID;
    AUDIF_DacMapping_t mapping;
} TVFE_Audio_Dac_Mapping_t;
#endif

#if 0
/*******************************************************************************
 * Constant
 ******************************************************************************/

#define AUDIO_MANAGER_CONNECTING_COUNTER    (3)


/*******************************************************************************
 * Enumeration
 ******************************************************************************/


/*******************************************************************************
 * structure
 ******************************************************************************/

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
UINT8 StopPolling;
AudioManagerParam_t gAudioPrimaryParam;
UINT8 g_u8SifAvailableMode;
/*******************************************************************************
 * Macro - for each source
 ******************************************************************************/
#define __NONE__STATE__EXECUTION__FUNC__
#define __NONE__STATE__TRANSITION__FUNC__


/*******************************************************************************
 * Program
 ******************************************************************************/
TVFE_Status_e TVFE_SSDInit(void)
{
    /* enable ssd rom chip*/
    DRV_AudEnableSSDROMReg(DTV_TRUE);
	DRV_AudInit();
	GL_MutexCreate("gAudioPrimaryParam", &(gAudioPrimaryParam.mutex));
	return TVFE_SUCCESS;
}
TVFE_Status_e TVFE_SSDUninit(void)
{
    /* enable ssd rom chip*/
    DRV_AudEnableSSDROMReg(DTV_FALSE);
	GL_MutexDelete(gAudioPrimaryParam.mutex);
	return TVFE_SUCCESS;
}


/******************************************************************************/
/**
* @brief    Get SIF standard
* @param    pRegStatus : Registers report from hardware
* @param    pRetStandrad : Report standard according "pRegStatus"
* @return   TVFE_SUCCESS if successful
*			TVFE_ERR_INVALID_PARAM if parameters is unknown
*           TVFE_ERR_FAILURE if pointer is DTV_NULL
******************************************************************************/
static TVFE_Status_e _tvfe_AudioGetSIFStandard(DRV_AudFrontEndReg_t *pRegStatus,
                                    TVFE_AudioSIFStandard_t *pRetStandrad)
{
	if((pRegStatus == DTV_NULL)
        ||(pRetStandrad == DTV_NULL))
    {
		return TVFE_ERR_FAILURE;
    }

    switch(pRegStatus->INF_DEV_STA_REG.STDRES)
    {
        case DRV_AUD_STANDARD_FAIL:
		case DRV_AUD_STANDARD_BUSY:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_FAIL;
            break;

        case DRV_AUD_STANDARD_BG:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_BG;
            break;

        case DRV_AUD_STANDARD_DK:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_DK;
            break;

        case DRV_AUD_STANDARD_M:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_M;
            break;

        case DRV_AUD_STANDARD_BG_A2:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_BG_A2;
            break;

        case DRV_AUD_STANDARD_BG_NICAM:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_BG_NICAM;
            break;

        case DRV_AUD_STANDARD_DK_A2_1:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_DK_A2_1;
            break;

        case DRV_AUD_STANDARD_DK_A2_2:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_DK_A2_2;
            break;

        case DRV_AUD_STANDARD_DK_A2_3:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_DK_A2_3;
            break;

        case DRV_AUD_STANDARD_DK_NICAM:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_DK_NICAM;
            break;

        case DRV_AUD_STANDARD_L_NICAM:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_L_NICAM;
            break;

        case DRV_AUD_STANDARD_I_NICAM:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_I_NICAM;
            break;

        case DRV_AUD_STANDARD_M_KOREA:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_M_KOREA;
            break;

        case DRV_AUD_STANDARD_M_BTSC:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_M_BTSC;
            break;

        case DRV_AUD_STANDARD_M_EIAJ:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_M_EIAJ;
            break;

        case DRV_AUD_STANDARD_FM_10_7_IF_50_US:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_FM_10_7_IF_50_US;
            break;

        case DRV_AUD_STANDARD_FM_10_7_IF_75_US:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_FM_10_7_IF_75_US;
            break;

        case DRV_AUD_STANDARD_FM_SELECTABLE_IF_50_US:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_FM_SELECTABLE_IF_50_US;
            break;

        case DRV_AUD_STANDARD_FM_SELECTABLE_IF_75_US:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_FM_SELECTABLE_IF_75_US;
            break;
		default:
			return TVFE_ERR_INVALID_PARAM;
			break;
    }
	return TVFE_SUCCESS;
}
/******************************************************************************/
/**
* @brief    Get SIF mode detect status
* @param    pRegStatus : Registers report from hardware
* @param    pRetMode : Report standard according "pRegStatus"
* @return   TVFE_SUCCESS if successful
*           TVFE_ERR_FAILURE if pointer is DTV_NULL
******************************************************************************/
static TVFE_Status_e _tvfe_AudioGetSIFModeStatus(DRV_AudFrontEndReg_t *pRegStatus,
                                        TVFE_AudioSIFAvailableMode_e *pRetMode)
{
#if 1/*get status from variables from audio manager*/
    Boolean fsap = DTV_FALSE,fdual = DTV_FALSE,fstereo = DTV_FALSE,fnicam = DTV_FALSE;

	if(pRetMode == DTV_NULL)
    {
		return TVFE_ERR_FAILURE;
    }

    *pRetMode = 0;

    /*check SAP*/
    if(pRegStatus->INF_DEV_STA_REG.SAPDET)
    {
        if(pRegStatus->INF_DEV_STA_REG.SAMUT)
        {
            fsap=DTV_FALSE;
        }
        else
        {
            fsap=DTV_TRUE;
        }
    }
    else
    {
        fsap=DTV_FALSE;
    }

    /*check dual*/
    if(pRegStatus->INF_DEV_STA_REG.GDU)
    {
        fdual=DTV_TRUE;
    }
    else
    {
        fdual=DTV_FALSE;
    }

    /*check stereo*/
    if (pRegStatus->INF_DEV_STA_REG.STDRES == REG_SSD_STDRES_VALUE_M_BTSC)
    {
        if ((pRegStatus->INF_DEV_STA_REG.BPILOT)
            && (!pRegStatus->INF_DEV_STA_REG.BAMUT))
        {
            fstereo=DTV_TRUE;
        }
        else
        {
            fstereo=DTV_FALSE;
        }
    }
    else
    {
        if(pRegStatus->INF_DEV_STA_REG.GST)
        {
            fstereo=DTV_TRUE;
        }
        else
        {
            fstereo=DTV_FALSE;
        }
    }

    /*check NICAM*/
    if(pRegStatus->INF_DEV_STA_REG.VDSP_C)
    {
        if(pRegStatus->INF_DEV_STA_REG.NAMUT)
        {
            fnicam=DTV_FALSE;
        }
        else
        {
            fnicam=DTV_TRUE;
        }
    }
    else
    {
        fnicam=DTV_FALSE;
    }

    if (fnicam == DTV_TRUE)
    {
        *pRetMode |= TVFE_AUDIO_SIF_AVAIL_NICAM;
        *pRetMode |= TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
        if (fdual == DTV_TRUE)
        {
            *pRetMode |= TVFE_AUDIO_SIF_AVAIL_DUAL;
        }
        else if (fstereo == DTV_TRUE)
        {
            *pRetMode |= TVFE_AUDIO_SIF_AVAIL_STEREO;
        }
        else
        {
            *pRetMode |= TVFE_AUDIO_SIF_AVAIL_NICAM_MONO;
        }
    }
    else
    {
        if (fsap == DTV_TRUE)
        {
            *pRetMode |= TVFE_AUDIO_SIF_AVAIL_SAP;
            if (fstereo == DTV_TRUE)
            {
                *pRetMode |= TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
                *pRetMode |= TVFE_AUDIO_SIF_AVAIL_STEREO;
            }
            else
            {
                *pRetMode |= TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
            }
        }
        else
        {
            if (fstereo == DTV_TRUE)
            {
	        	if((pRegStatus->INF_DEV_STA_REG.STDRES == DRV_AUD_STANDARD_BG_A2)
					||(pRegStatus->INF_DEV_STA_REG.STDRES == DRV_AUD_STANDARD_DK_A2_1)
					||(pRegStatus->INF_DEV_STA_REG.STDRES == DRV_AUD_STANDARD_DK_A2_2)
					||(pRegStatus->INF_DEV_STA_REG.STDRES == DRV_AUD_STANDARD_DK_A2_3))

	        	{/*if get A2 standard, and dual or stereo is available, mono mode should be unavailable*/
	            	*pRetMode |= TVFE_AUDIO_SIF_AVAIL_STEREO;
				}
				else
				{

	            	*pRetMode |= TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
				}
            }
            else
            {
                if (fdual == DTV_TRUE)
                {
	            	if((pRegStatus->INF_DEV_STA_REG.STDRES == DRV_AUD_STANDARD_BG_A2)
						||(pRegStatus->INF_DEV_STA_REG.STDRES == DRV_AUD_STANDARD_DK_A2_1)
						||(pRegStatus->INF_DEV_STA_REG.STDRES == DRV_AUD_STANDARD_DK_A2_2)
						||(pRegStatus->INF_DEV_STA_REG.STDRES == DRV_AUD_STANDARD_DK_A2_3))

	            	{/*if get A2 standard, and dual or stereo is available, mono mode should be unavailable*/
                    	*pRetMode |= TVFE_AUDIO_SIF_AVAIL_DUAL;
					}
                	else
                	{
                    	*pRetMode |= TVFE_AUDIO_SIF_AVAIL_DUAL;
						*pRetMode |= TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
                	}
                }
				else
				{
                	*pRetMode |= TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
				}
            }
        }
    }
	return TVFE_SUCCESS;
#else/*get status via audio drivers*/
    Boolean fsap = DTV_FALSE,fdual = DTV_FALSE,fstereo = DTV_FALSE,fnicam = DTV_FALSE;

	if(pRetMode == DTV_NULL)
		return TVFE_ERR_INVALID_PARAM;

    *pRetMode = 0;

    DRV_AudGetIfSAP(&fsap);
    DRV_AudGetIfDual(&fdual);
    DRV_AudGetIfStereo(&fstereo);
    DRV_AudGetIfNICAM(&fnicam);

    if (fnicam == DTV_TRUE)
    {
        *pRetMode |= TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
        if (fdual == DTV_TRUE)
        {
            *pRetMode |= TVFE_AUDIO_SIF_AVAIL_DUAL;
        }
        else if (fstereo == DTV_TRUE)
        {
            *pRetMode |= TVFE_AUDIO_SIF_AVAIL_STEREO;
        }
        else
        {
            *pRetMode |= TVFE_AUDIO_SIF_AVAIL_NICAM_MONO;
        }
    }
    else
    {
        if (fsap == DTV_TRUE)
        {
            *pRetMode |= TVFE_AUDIO_SIF_AVAIL_SAP;
            if (fstereo == DTV_TRUE)
            {
                *pRetMode |= TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
                *pRetMode |= TVFE_AUDIO_SIF_AVAIL_STEREO;
            }
            else
            {
                *pRetMode |= TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
            }
        }
        else
        {
            if (fstereo == DTV_TRUE)
            {
                *pRetMode |= TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
                *pRetMode |= TVFE_AUDIO_SIF_AVAIL_STEREO;
            }
            else
            {
                *pRetMode |= TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
                if (fdual == DTV_TRUE)
                {
                    *pRetMode |= TVFE_AUDIO_SIF_AVAIL_DUAL;
                }
            }
        }
    }
	return TVFE_SUCCESS;
#endif
}
/******************************************************************************/
/**
* @brief    Get Regsiter status
* @param    pRetFrontEndStatus : Front end status report from hardware
* @return   TVFE_SUCCESS if successful
*           TVFE_ERR_FAILURE if pointer is DTV_NULL
******************************************************************************/
static TVFE_Status_e _tvfe_AudioGetFrontEndStatus(TVFE_AudioFrontEndStatus_t *pRetFrontEndStatus)
{
	if (pRetFrontEndStatus == DTV_NULL)
		return TVFE_ERR_INVALID_PARAM;
    DRV_AudGetFrontEndRegInfo(&(pRetFrontEndStatus->RegInfo));
    _tvfe_AudioGetSIFStandard(&(pRetFrontEndStatus->RegInfo),&(pRetFrontEndStatus->eStandard));
    _tvfe_AudioGetSIFModeStatus(&(pRetFrontEndStatus->RegInfo),&(pRetFrontEndStatus->eMode));
    DRV_AudGetSIFAGCGain(&(pRetFrontEndStatus->bSIFAGCGain));
    pRetFrontEndStatus->bNICAMError = pRetFrontEndStatus->RegInfo.INF_NIC_STA_REG.ERR_OUT;
	return  TVFE_SUCCESS;
}
/******************************************************************************/
/**
* @brief    Get SIF front end status
* @param    pFrontEndParam : Report front end status according assigned report method
* @return   TVFE_SUCCESS if successful
*           TVFE_ERR_FAILURE if pointer is DTV_NULL
*			TVFE_WARN_NO_ACTION if no front end source is not connected to SIF signal
******************************************************************************/
 TVFE_Status_e TVFE_AudioGetFrontEndParam(TVFE_AudioFrontEndParam_t *pFrontEndParam)
{
    TVFE_Status_e pl_ret = TVFE_SUCCESS;

	if(pFrontEndParam == DTV_NULL)
    {
		return TVFE_ERR_FAILURE;
    }

	/*get status from register immediately, for channel scanning*/
	if(pFrontEndParam->eMethod == TVFE_AUDIO_REPORT_DIRECT)
	{//get register value from registers
	    //GL_MutexLock((gAudioPrimaryParam.mutex));/*resource protection begin*/
	    DRV_AudGetFrontEndRegInfo(&(gAudioPrimaryParam.Status.RegInfo));
	    _tvfe_AudioGetSIFStandard(&(gAudioPrimaryParam.Status.RegInfo),&(gAudioPrimaryParam.Status.eStandard));
	    _tvfe_AudioGetSIFModeStatus(&(gAudioPrimaryParam.Status.RegInfo),&(gAudioPrimaryParam.Status.eMode));
        DRV_AudGetSIFAGCGain(&(gAudioPrimaryParam.Status.bSIFAGCGain));
		pFrontEndParam->eRetStandard = gAudioPrimaryParam.Status.eStandard;
		pFrontEndParam->eRetMode = gAudioPrimaryParam.Status.eMode;
        pFrontEndParam->bRetSIFAGCGain = gAudioPrimaryParam.Status.bSIFAGCGain;
		pFrontEndParam->bNICAMError = gAudioPrimaryParam.Status.bNICAMError;
	    //GL_MutexUnlock((gAudioPrimaryParam.mutex));/*resource protection end*/
	}
	else
	{//get status after audio manager debounce
        if((gAudioPrimaryParam.eState == AUDIO_MANAGER_CONNECT_READY)
            ||(gAudioPrimaryParam.eState == AUDIO_MANAGER_CONNECT_TIME_OUT))
        {
            pFrontEndParam->eRetStandard = gAudioPrimaryParam.Status.eStandard;
            pFrontEndParam->bRetSIFAGCGain = gAudioPrimaryParam.Status.bSIFAGCGain;
			pFrontEndParam->bNICAMError = gAudioPrimaryParam.Status.bNICAMError;
            if(gAudioPrimaryParam.fBTSCPilotLevelOverflow == DTV_TRUE)
        	{
                pFrontEndParam->eRetMode = (gAudioPrimaryParam.Status.eMode|TVFE_AUDIO_SIF_AVAIL_BTSC_PILOT_OVERFLOW);
        	}
        	else
            {
                pFrontEndParam->eRetMode = gAudioPrimaryParam.Status.eMode;
    		}
        }
        else
        {
            pFrontEndParam->eRetStandard = TVFE_AUDIO_SIF_STANDARD_BUSY;
	        pFrontEndParam->eRetMode = TVFE_AUDIO_SIF_AVAIL_BUSY;
        }
    }
    return pl_ret;
}
/********************************************************************
**Prototype: SIF_SoundDetect()
**Parameters: None
**Returns: None
**Description: This function is used to detect the change of sound mode
**Restrictions:
********************************************************************/
void SIF_SoundDetect(void)
{
	UINT8 g_u8SifNextMode = TVFE_AUDIO_SIF_MODE_MAX;
	TVFE_AudioSIFMode_e eSifCurrentMode = TVFE_AUDIO_SIF_MODE_ANALOG_MONO;
	TVFE_AudioSIFStandard_t SifStandrad;
	TVFE_AudioSIFAvailableMode_e eSifAvailableMode;
	//gjshen@20090618_begin
	static Boolean fChangeLevelAdjust = DTV_FALSE;
	TVFE_AudioLevelAdjust_st LevelAdjust_config;
	//gjshen@20090618_end

	/* add by b.yang 20100723 */
	TVFE_SoundStd_e eSoundStdType = TVFE_SOUND_STANDARD_MAX;

	/* Check detection process is ready */
	TVFE_AudioFrontEndParam_t FrontEndParam;
	FrontEndParam.eMethod = TVFE_AUDIO_REPORT_INDIRECT;


	TVFE_AudioGetFrontEndParam(&FrontEndParam);
	SifStandrad = FrontEndParam.eRetStandard;
	TVFE_GetTunerValueType(&eSoundStdType);


	if(SifStandrad == TVFE_AUDIO_SIF_STANDARD_FAIL)
	{
		if((eSoundStdType == TVFE_SOUND_STANDARD_L)
			||(eSoundStdType == TVFE_SOUND_STANDARD_L1))
		{
			TVFE_AudioSetSIFConnect(TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_L);
		}
		else
		{
			if(eSoundStdType == TVFE_SOUND_STANDARD_BG)
			{
				TVFE_AudioSetSIFConnect( \
					TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_BG);
			}
			else if(eSoundStdType == TVFE_SOUND_STANDARD_DK)
			{
				TVFE_AudioSetSIFConnect( \
					TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_DK);
			}
			else if(eSoundStdType == TVFE_SOUND_STANDARD_I)
			{
				TVFE_AudioSetSIFConnect( \
					TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_I);
			}
			else
			{
				TVFE_AudioSetSIFConnect( \
					TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_BGDKI);
			}
		}
	}
	else if(SifStandrad != TVFE_AUDIO_SIF_STANDARD_BUSY)
	{
		//gjshen@20090618_begin
		if(fChangeLevelAdjust == DTV_FALSE)
		{
			switch(SifStandrad)
			{
				case TVFE_AUDIO_SIF_STANDARD_I_NICAM:
					LevelAdjust_config.bDECAdjustLevel		= 1;
					LevelAdjust_config.bMonoAdjustLevel		= 2;
					LevelAdjust_config.bNICAMAdjustLevel	= 2;
					LevelAdjust_config.bSAPAdjustLevel		= 0;
					LevelAdjust_config.bADCAdjustLevel		= -1;
					LevelAdjust_config.bDigitalAdjustLevel	= -3;
					TVFE_AudioSetLevelAdjust(&LevelAdjust_config);
					break;

				case TVFE_AUDIO_SIF_STANDARD_L_NICAM:
					LevelAdjust_config.bDECAdjustLevel		= 1;
					LevelAdjust_config.bMonoAdjustLevel		= 0;
					LevelAdjust_config.bNICAMAdjustLevel	= 2;
					LevelAdjust_config.bSAPAdjustLevel		= 0;
					LevelAdjust_config.bADCAdjustLevel		= -1;
					LevelAdjust_config.bDigitalAdjustLevel	= -3;
					TVFE_AudioSetLevelAdjust(&LevelAdjust_config);
					break;

				default :
					LevelAdjust_config.bDECAdjustLevel		= 2;
					LevelAdjust_config.bMonoAdjustLevel		= 2;
					LevelAdjust_config.bNICAMAdjustLevel	= 1;
					LevelAdjust_config.bSAPAdjustLevel		= 0;
					LevelAdjust_config.bADCAdjustLevel		= -1;
					LevelAdjust_config.bDigitalAdjustLevel	= -3;
					break;
			}
			fChangeLevelAdjust = DTV_TRUE;
		}
		//gjshen@20090618_end

		/* Decide the available sound modes for the current channel */
		TVFE_AudioGetFrontEndParam(&FrontEndParam);
		eSifAvailableMode = FrontEndParam.eRetMode;

		if (eSifAvailableMode != g_u8SifAvailableMode)
		{
			diag_printf("Detect sound changed!!\n");

			if (g_u8SifNextMode == TVFE_AUDIO_SIF_MODE_MAX)
			{
				if ((eSifAvailableMode & TVFE_AUDIO_SIF_AVAIL_STEREO) != 0)
				{
					g_u8SifNextMode = TVFE_AUDIO_SIF_MODE_STEREO;
				}
				else if ((eSifAvailableMode & TVFE_AUDIO_SIF_AVAIL_DUAL) != 0)
				{
					g_u8SifNextMode = TVFE_AUDIO_SIF_MODE_DUAL_I;
				}
				else if ((eSifAvailableMode & TVFE_AUDIO_SIF_AVAIL_NICAM_MONO) != 0)
				{
					g_u8SifNextMode = TVFE_AUDIO_SIF_MODE_NICAM_MONO;
				}
				else if ((eSifAvailableMode & TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO) != 0)
				{
					g_u8SifNextMode = TVFE_AUDIO_SIF_MODE_ANALOG_MONO;
				}
			}
		}

		TVFE_GetSIFWorkingMode(&eSifCurrentMode);
		if(eSifCurrentMode != g_u8SifNextMode)
		{
			if(g_u8SifNextMode != TVFE_AUDIO_SIF_MODE_MAX)
			{
				TVFE_SIF_SetCurrentMode(g_u8SifNextMode);
			}
		}
		g_u8SifAvailableMode = eSifAvailableMode;
	}
}

TVFE_Status_e _AM_SIF_INICAMProcess(AudioManagerParam_t *pParam, Boolean *pfFinish)
{
    //1. I NICAM workaround
    TVFE_Status_e pl_ret = TVFE_SUCCESS;
    static DRV_AudNICAMRotateAngle_t eRotateAngle = DRV_AUD_NICAM_QPSK_ROTATE_0_DEGREE;
    static UINT8 average_counter = 0;
    static UINT32 ErrorSum = 0,PreErrorAverage = 0, ErrorAverage = 0;

    if(pParam == DTV_NULL)
    {
        return TVFE_ERR_FAILURE;
    }

    ////TVFE_AUDIO_DBG("_AM_SIFINICAMProcess\n");

    //diag_printf("NICAMError:%d\n",pParam->Status.bNICAMError);

#if 1
    if(eRotateAngle < DRV_AUD_NICAM_QPSK_ROTATE_MAX)
    {
        if(average_counter < INICAM_PROCESS_COUNTER_MAX)
        {//summation and add counter
            average_counter++;
            ErrorSum += pParam->Status.bNICAMError;
        }
        else
        {//reset variable and calculate average
            ErrorAverage = ErrorSum/average_counter;
            average_counter = 0;
            ErrorSum = 0;

            /*decide to rotate angle*/
            if(ErrorAverage > PreErrorAverage)
            {
                /*rotate the angle*/
                eRotateAngle++;
                DRV_AudSetINICAMRotateAngle(eRotateAngle);
                PreErrorAverage = ErrorAverage;
            }
            else
            {
                *pfFinish = DTV_TRUE;

                /*reset variable*/
                average_counter = 0;
                ErrorSum = 0;
                ErrorAverage = 0;
                PreErrorAverage = 0;
                eRotateAngle = DRV_AUD_NICAM_QPSK_ROTATE_0_DEGREE;
            }
        }
    }
    else
    {
        /*reset to default*/
        eRotateAngle = DRV_AUD_NICAM_QPSK_ROTATE_0_DEGREE;
        DRV_AudSetINICAMRotateAngle(eRotateAngle);
        *pfFinish = DTV_TRUE;
    }
#else
    if(!(pParam->pFrontEndInfo->Status.eMode & TVFE_AUDIO_SIF_AVAIL_NICAM)
        &&(pParam->pFrontEndInfo->Status.bNICAMError >= INICAM_PROCESS_NICAM_ERROR_THRESHOLD))
    {
        if(eRotateAngle < DRV_AUD_NICAM_QPSK_ROTATE_MAX)
        {
            /*rotate the angle*/
            DRV_AudSetINICAMRotateAngle(eRotateAngle++);
        }
        else
        {
            /*reset to default*/
            eRotateAngle = DRV_AUD_NICAM_QPSK_ROTATE_0_DEGREE;
            DRV_AudSetINICAMRotateAngle(eRotateAngle);
            *pfFinish = DTV_TRUE;
        }
    }
    else
    {
        eRotateAngle = DRV_AUD_NICAM_QPSK_ROTATE_0_DEGREE;
        *pfFinish = DTV_TRUE;
    }
#endif

    if(*pfFinish == DTV_TRUE)
    {
        /*reset debounce counter*/
        pParam->bDebounceCounter = AUDIO_MANAGER_CONNECTING_COUNTER;
    }

    return pl_ret;
}
#define BTSC_PROCESS_MPX_LEVEL_THRESHOLD    (0xe000)
#define BTSC_PROCESS_MPX_LEVEL_ADJUST       (BTSC_PROCESS_MPX_LEVEL_THRESHOLD-2)
#define BTSC_PROCESS_MPX_LEVEL_UPPER_VALID  (0x3ff00)
#define BTSC_PROCESS_MPX_LEVEL_MAX          (0x1ffff)
#define BTSC_PROCESS_MPX_LEVEL_MSB          (0x20000)

TVFE_Status_e _AM_SIF_BTSCProcess(AudioManagerParam_t *pParam, Boolean *pfFinish)
{
    //1. BTSC workaround
    TVFE_Status_e pl_ret = TVFE_SUCCESS;
    static UINT8 average_counter = 0;
    static UINT32 dMPX_level_average = BTSC_PROCESS_MPX_LEVEL_ADJUST,
                    dMPX_level_sum = 0;

    if(pParam == DTV_NULL)
    {
        return TVFE_ERR_FAILURE;
    }

    ////TVFE_AUDIO_DBG("_AM_SIFBTSCProcess before summation MPXPLEV=%x\n,BPILOT=%x dMPX_level_sum=%x,average_counter=%d\n",pParam->pFrontEndInfo->Status.RegInfo.INF_MPX_LEVEL_REG.MPXPLEV,pParam->pFrontEndInfo->Status.RegInfo.INF_DEV_STA_REG.BPILOT,dMPX_level_sum,average_counter);
#if 1
    if((pParam->Status.RegInfo.INF_DEV_STA_REG.BPILOT)
    	&&(pParam->Status.RegInfo.INF_MPX_LEVEL_REG.MPXPLEV < BTSC_PROCESS_MPX_LEVEL_UPPER_VALID))
    {//if pilot tone is detected and pilot level is not over upper valid value
        dMPX_level_sum += pParam->Status.RegInfo.INF_MPX_LEVEL_REG.MPXPLEV;
    }
    else
    {//if pilot tone is not detected
//        if((pParam->pFrontEndStatus->RegStatus.INF_MPX_LEVEL_REG.MPXPLEV & BTSC_PROCESS_MPX_LEVEL_MSB)
//        	&&(pParam->pFrontEndStatus->RegStatus.INF_MPX_LEVEL_REG.MPXPLEV < BTSC_PROCESS_MPX_LEVEL_UPPER_VALID))
//        {//pilot level is overflow
//		        dMPX_level_sum += BTSC_PROCESS_MPX_LEVEL_THRESHOLD;
//	      }
//	      else
//	      {//pilot level is not overflow
	          dMPX_level_sum += BTSC_PROCESS_MPX_LEVEL_ADJUST;
//	      }
    }
#else
    if(((pParam->pFrontEndStatus->RegStatus.INF_MPX_LEVEL_REG.MPXPLEV & BTSC_PROCESS_MPX_LEVEL_MSB) == 0)
        &&(pParam->pFrontEndStatus->RegStatus.INF_MPX_LEVEL_REG.MPXPLEV >= BTSC_PROCESS_MPX_LEVEL_THRESHOLD)
        &&(pParam->pFrontEndStatus->RegStatus.INF_DEV_STA_REG.BPILOT))
    {//if MPX level is not overflow && MPX level is too large && pilot tone is detected
        dMPX_level_sum += pParam->pFrontEndStatus->RegStatus.INF_MPX_LEVEL_REG.MPXPLEV;
    }
    else if (pParam->pFrontEndStatus->RegStatus.INF_MPX_LEVEL_REG.MPXPLEV & BTSC_PROCESS_MPX_LEVEL_MSB)
    {//if MPX level is overflow
        dMPX_level_sum += pParam->pFrontEndStatus->RegStatus.INF_MPX_LEVEL_REG.MPXPLEV;
    }
    else
    {
        dMPX_level_sum += BTSC_PROCESS_MPX_LEVEL_ADJUST;
    }
#endif

    /*add average counter*/
    average_counter++;
    ////TVFE_AUDIO_DBG("_AM_SIFBTSCProcess after summation MPXPLEV=%x\n,BPILOT=%x dMPX_level_sum=%x,average_counter=%d\n",pParam->pFrontEndInfo->Status.RegInfo.INF_MPX_LEVEL_REG.MPXPLEV,pParam->pFrontEndInfo->Status.RegInfo.INF_DEV_STA_REG.BPILOT,dMPX_level_sum,average_counter);

    switch(pParam->bDebounceCounter)
    {
        case AUDIO_MANAGER_CONNECTING_COUNTER://first time enter process
            /*initial average value*/
            dMPX_level_average = BTSC_PROCESS_MPX_LEVEL_ADJUST;

            /*initial overflow flag*/
            pParam->fBTSCPilotLevelOverflow = DTV_FALSE;
            break;

        case (AUDIO_MANAGER_CONNECTING_COUNTER-1)://second time enter process
            dMPX_level_average = dMPX_level_sum/average_counter;//average MPX stereo pilot level
            ////TVFE_AUDIO_DBG("_AM_SIFBTSCProcess dMPX_level_sum=%x,dMPX_level_average=%x\n",dMPX_level_sum,dMPX_level_average);
            if(dMPX_level_average >= BTSC_PROCESS_MPX_LEVEL_THRESHOLD)
            {
                TVFE_AudioSetSIFConnect(TVFE_AUDIO_SIF_STANDARD_SSS_MODE_M);
                ////TVFE_AUDIO_DBG("_AM_SIFBTSCProcess force standard mask to TVFE_AUDIO_SIF_STANDARD_SSS_MODE_M\n");
                pParam->fBTSCPilotLevelOverflow = DTV_TRUE;
            }

            /*update finish flag*/
            *pfFinish = DTV_TRUE;
            break;

        default :
            break;
    }

    if(*pfFinish == DTV_TRUE)
    {
        /*reset debounce counter*/
        pParam->bDebounceCounter = AUDIO_MANAGER_CONNECTING_COUNTER;

        /*reset MPX level summation*/
        dMPX_level_sum = 0;
        average_counter = 0;

    }

    return pl_ret;
}


TVFE_Status_e _AM_SIFHandler(void *pParam)
{
//1. reset process(switch channel, signal recovery)
//2. debounce standard and channel mode status
//3. AGC control
//4. post stable event after reset process
//5. MPX stereo bug workaround
//6. NICAM error rate check with rotate frequency process
//7. dynamic finetune detect level according noise report???
    TVFE_Status_e pl_ret = TVFE_SUCCESS;
    AudioManagerParam_t *pHandlerParam;
    static TVFE_AudioFrontEndStatus_t PreFrontEndInfo;
    static Boolean fIfProcessDone = DTV_FALSE,fIfFMTypeAGCConfig = DTV_FALSE;
    static UINT8 bTimeOutCounter = AUDIO_MANAGER_CONNECTING_COUNTER;

//    //TVFE_AUDIO_DBG("_AM_SIFHandler\n");

    if(pParam == DTV_NULL)
    {
        return TVFE_ERR_FAILURE;
    }


    pHandlerParam = (AudioManagerParam_t *)pParam;

    //GL_MutexLock((pHandlerParam->mutex));/*resource protection begin*/

    switch(pHandlerParam->eState)
    {
        case AUDIO_MANAGER_CONNECT_INITIAL:
            fIfProcessDone = DTV_FALSE;

            pHandlerParam->bDebounceCounter = AUDIO_MANAGER_CONNECTING_COUNTER;
            pHandlerParam->eState = AUDIO_MANAGER_CONNECTING;
			g_u8SifAvailableMode = TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
            bTimeOutCounter = AUDIO_MANAGER_CONNECTING_COUNTER;
            DRV_AudSetINICAMRotateAngle(DRV_AUD_NICAM_QPSK_ROTATE_0_DEGREE);
            break;

        case AUDIO_MANAGER_CONNECTING:
            /*config AGC*/
            if((PreFrontEndInfo.eStandard == TVFE_AUDIO_SIF_STANDARD_L_NICAM)
                &&(pHandlerParam->Status.eStandard == TVFE_AUDIO_SIF_STANDARD_L_NICAM))
            {
            	if(fIfFMTypeAGCConfig == DTV_TRUE)
				{//For AM standard, turn off AGC if AGC is active
                    DRV_AudSetSIFAGCConfig(pHandlerParam->Control.AM_AGCConfigParam.fEn,\
                                                            pHandlerParam->Control.AM_AGCConfigParam.fHysteresisFastMode,\
                                                            pHandlerParam->Control.AM_AGCConfigParam.bGain);
					fIfFMTypeAGCConfig = DTV_FALSE;
				}
            }
            else
            {
            	if(fIfFMTypeAGCConfig == DTV_FALSE)
				{//For FM standard, turn on AGC if AGC is de-active
	                    DRV_AudSetSIFAGCConfig(pHandlerParam->Control.FM_AGCConfigParam.fEn,\
	                                                            pHandlerParam->Control.FM_AGCConfigParam.fHysteresisFastMode,\
	                                                            pHandlerParam->Control.FM_AGCConfigParam.bGain);
					fIfFMTypeAGCConfig = DTV_TRUE;
				}
            }

            if(fIfProcessDone == DTV_FALSE)
            {
                if((pHandlerParam->Status.eStandard == PreFrontEndInfo.eStandard)
                    &&(pHandlerParam->Status.eMode == PreFrontEndInfo.eMode))
                {//if stable, counter count down
                    //TVFE_AUDIO_DBG("pHandlerParam->pFrontEndStatus->eStandard=%d\n",pHandlerParam->Status.eStandard);
                    if(pHandlerParam->Status.eStandard == TVFE_AUDIO_SIF_STANDARD_I_NICAM)
                    {
                        _AM_SIF_INICAMProcess(pHandlerParam,&fIfProcessDone);
                    }
                    else if(pHandlerParam->Status.eStandard == TVFE_AUDIO_SIF_STANDARD_M_BTSC)
                    {
                        _AM_SIF_BTSCProcess(pHandlerParam,&fIfProcessDone);
                    }
                    else
                    {
                        fIfProcessDone = DTV_TRUE;
                    }

                    /*minus counter after NICAM/BTSC process*/
                    pHandlerParam->bDebounceCounter --;
                }
                else
                {//if signal different, counter reset and update status
                    //TVFE_AUDIO_DBG("AUDIO_MANAGER_CONNECTING reset\n");
                    pHandlerParam->bDebounceCounter = AUDIO_MANAGER_CONNECTING_COUNTER;
                }
            }
            else
            {
                if((pHandlerParam->Status.eStandard == PreFrontEndInfo.eStandard)
                    &&(pHandlerParam->Status.eMode == PreFrontEndInfo.eMode))
                {//if signal stable, counter count down
                    if(pHandlerParam->bDebounceCounter -- == 0)
                    {
                        pHandlerParam->eState = AUDIO_MANAGER_CONNECT_READY;
                        pHandlerParam->bDebounceCounter = AUDIO_MANAGER_CONNECTING_COUNTER;

                    }
                }
                else
                {//if signal different, counter reset and update status
                    pHandlerParam->bDebounceCounter = AUDIO_MANAGER_CONNECTING_COUNTER;
                }

                /*time out process*/
                if((bTimeOutCounter-- == 0)
                    &&(pHandlerParam->bDebounceCounter == AUDIO_MANAGER_CONNECTING_COUNTER))
                {
                    pHandlerParam->eState = AUDIO_MANAGER_CONNECT_TIME_OUT;
                    pHandlerParam->bDebounceCounter = AUDIO_MANAGER_CONNECTING_COUNTER;
                    bTimeOutCounter = AUDIO_MANAGER_CONNECTING_COUNTER;
                }
            }
            break;

        case AUDIO_MANAGER_CONNECT_READY:
        case AUDIO_MANAGER_CONNECT_TIME_OUT:
            if((PreFrontEndInfo.eMode != pHandlerParam->Status.eMode)
                &&(pHandlerParam->bDebounceCounter -- == 0))
            {
                pHandlerParam->bDebounceCounter = AUDIO_MANAGER_CONNECTING_COUNTER;
                //TVFE_AudioPostEvent(TVFE_AUDIO_EVENT_SIF_MODE_STATUS_CHANGE);
            }

            if((PreFrontEndInfo.eStandard != pHandlerParam->Status.eStandard)
                &&(pHandlerParam->bDebounceCounter -- == 0))
            {
                pHandlerParam->bDebounceCounter = AUDIO_MANAGER_CONNECTING_COUNTER;
                //TVFE_AudioPostEvent(TVFE_AUDIO_EVENT_SIF_STANDARD_CHANGE);
            }
            break;

        default :

            break;

    }

    /*reocrd current information*/
    memcpy(&PreFrontEndInfo,&pHandlerParam->Status,sizeof(TVFE_AudioFrontEndStatus_t));
    //GL_MutexUnlock(pHandlerParam->mutex);/*resource protection end*/

    return pl_ret;
}

#endif

TVFE_Audio_Aux_Path_t gAudioAuxPath;
TVFE_Audio_Dac_Mapping_t gAudioDacMapping;
bool gMasterMuteStatus = TVFE_FALSE;

/********************************************************************
Prototype: TVFE_AudioParamInit()
Parameters:
Returns:TVFE_Audio_Status_e
Description: This function is used to initiate audio global parameters
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_AudioParamInit(void)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

//	GL_MutexCreate("AudioAuxPath", &gAudioAuxPath.mutexID);
//    //GL_MutexLock(gAudioAuxPath.mutexID);
    gAudioAuxPath.Aux1Path=AUDIF_AUX1_PATH_OFF;
    gAudioAuxPath.Aux2Path=AUDIF_AUX2_PATH_OFF;
//    //GL_MutexUnlock(gAudioAuxPath.mutexID);

//    GL_MutexCreate("AudioDacMapping", &gAudioDacMapping.mutexID);
//    //GL_MutexLock(gAudioDacMapping.mutexID);
    gAudioDacMapping.mapping.DacMapping_Data0_L=AUDIF_DAC_MAP_PCM2_L;
    gAudioDacMapping.mapping.DacMapping_Data0_R=AUDIF_DAC_MAP_PCM2_R;
    gAudioDacMapping.mapping.DacMapping_Data1_L=AUDIF_DAC_MAP_PCM2_L;
    gAudioDacMapping.mapping.DacMapping_Data1_R=AUDIF_DAC_MAP_PCM2_R;
    gAudioDacMapping.mapping.DacMapping_Data2_L=AUDIF_DAC_MAP_PCM2_L;
    gAudioDacMapping.mapping.DacMapping_Data2_R=AUDIF_DAC_MAP_PCM2_R;
//    //GL_MutexUnlock(gAudioDacMapping.mutexID);
    return ret;

}

/********************************************************************
Prototype: TVFE_Audio_Set_Output_Config()
Parameters:
            pOutput_Config->Output_Port
            pOutput_Config->Process_Path
            pOutput_Config->If_Channel_Swap
Returns:TVFE_Audio_Status_e
Description: This function is used to set output path by input parameters
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Output_Config(const TVFE_Audio_Output_Config_t *pOutput_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

    if(pOutput_Config->Process_Path==TVFE_AUDIO_MAIN_PATH)
    {
        if((pOutput_Config->Output_Port==TVFE_AUDIO_OUTPUT_DAC0)||
            (pOutput_Config->Output_Port==TVFE_AUDIO_OUTPUT_I2S0))
        {
            if (pOutput_Config->If_Channel_Swap == DTV_FALSE)
            {
                //GL_MutexLock(gAudioDacMapping.mutexID);
                gAudioDacMapping.mapping.DacMapping_Data0_L=AUDIF_DAC_MAP_PCM0_L;
                gAudioDacMapping.mapping.DacMapping_Data0_R=AUDIF_DAC_MAP_PCM0_R;
                //GL_MutexUnlock(gAudioDacMapping.mutexID);
            }
            else
            {
                //GL_MutexLock(gAudioDacMapping.mutexID);
                gAudioDacMapping.mapping.DacMapping_Data0_L=AUDIF_DAC_MAP_PCM0_R;
                gAudioDacMapping.mapping.DacMapping_Data0_R=AUDIF_DAC_MAP_PCM0_L;
                //GL_MutexUnlock(gAudioDacMapping.mutexID);
            }
        }
        else if((pOutput_Config->Output_Port==TVFE_AUDIO_OUTPUT_DAC1)||
            (pOutput_Config->Output_Port==TVFE_AUDIO_OUTPUT_I2S1))
        {
            if (pOutput_Config->If_Channel_Swap == DTV_FALSE)
            {
                //GL_MutexLock(gAudioDacMapping.mutexID);
                gAudioDacMapping.mapping.DacMapping_Data1_L=AUDIF_DAC_MAP_PCM0_L;
                gAudioDacMapping.mapping.DacMapping_Data1_R=AUDIF_DAC_MAP_PCM0_R;
                //GL_MutexUnlock(gAudioDacMapping.mutexID);
            }
            else
            {
                //GL_MutexLock(gAudioDacMapping.mutexID);
                gAudioDacMapping.mapping.DacMapping_Data1_L=AUDIF_DAC_MAP_PCM0_R;
                gAudioDacMapping.mapping.DacMapping_Data1_R=AUDIF_DAC_MAP_PCM0_L;
                //GL_MutexUnlock(gAudioDacMapping.mutexID);
            }
        }
        else if((pOutput_Config->Output_Port==TVFE_AUDIO_OUTPUT_DAC2)||
            (pOutput_Config->Output_Port==TVFE_AUDIO_OUTPUT_I2S2))
        {
            if (pOutput_Config->If_Channel_Swap == DTV_FALSE)
            {
                //GL_MutexLock(gAudioDacMapping.mutexID);
                gAudioDacMapping.mapping.DacMapping_Data2_L=AUDIF_DAC_MAP_PCM0_L;
                gAudioDacMapping.mapping.DacMapping_Data2_R=AUDIF_DAC_MAP_PCM0_R;
                //GL_MutexUnlock(gAudioDacMapping.mutexID);
            }
            else
            {
                //GL_MutexLock(gAudioDacMapping.mutexID);
                gAudioDacMapping.mapping.DacMapping_Data2_L=AUDIF_DAC_MAP_PCM0_R;
                gAudioDacMapping.mapping.DacMapping_Data2_R=AUDIF_DAC_MAP_PCM0_L;
                //GL_MutexUnlock(gAudioDacMapping.mutexID);
            }
        }
        else
        {
        }

    }
    else if(pOutput_Config->Process_Path==TVFE_AUDIO_AUXILIARY_1_PATH)
    {
        if((pOutput_Config->Output_Port==TVFE_AUDIO_OUTPUT_DAC0)||
            (pOutput_Config->Output_Port==TVFE_AUDIO_OUTPUT_I2S0))
        {
            if (pOutput_Config->If_Channel_Swap == DTV_FALSE)
            {
                //GL_MutexLock(gAudioDacMapping.mutexID);
                gAudioDacMapping.mapping.DacMapping_Data0_L=AUDIF_DAC_MAP_PCM1_L;
                gAudioDacMapping.mapping.DacMapping_Data0_R=AUDIF_DAC_MAP_PCM1_R;
                //GL_MutexUnlock(gAudioDacMapping.mutexID);
            }
            else
            {
                //GL_MutexLock(gAudioDacMapping.mutexID);
                gAudioDacMapping.mapping.DacMapping_Data0_L=AUDIF_DAC_MAP_PCM1_R;
                gAudioDacMapping.mapping.DacMapping_Data0_R=AUDIF_DAC_MAP_PCM1_L;
                //GL_MutexUnlock(gAudioDacMapping.mutexID);
            }
        }
        else if((pOutput_Config->Output_Port==TVFE_AUDIO_OUTPUT_DAC1)||
            (pOutput_Config->Output_Port==TVFE_AUDIO_OUTPUT_I2S1))
        {
            if (pOutput_Config->If_Channel_Swap == DTV_FALSE)
            {
                //GL_MutexLock(gAudioDacMapping.mutexID);
                gAudioDacMapping.mapping.DacMapping_Data1_L=AUDIF_DAC_MAP_PCM1_L;
                gAudioDacMapping.mapping.DacMapping_Data1_R=AUDIF_DAC_MAP_PCM1_R;
                //GL_MutexUnlock(gAudioDacMapping.mutexID);
            }
            else
            {
                //GL_MutexLock(gAudioDacMapping.mutexID);
                gAudioDacMapping.mapping.DacMapping_Data1_L=AUDIF_DAC_MAP_PCM1_R;
                gAudioDacMapping.mapping.DacMapping_Data1_R=AUDIF_DAC_MAP_PCM1_L;
                //GL_MutexUnlock(gAudioDacMapping.mutexID);
            }
        }
        else if((pOutput_Config->Output_Port==TVFE_AUDIO_OUTPUT_DAC2)||
            (pOutput_Config->Output_Port==TVFE_AUDIO_OUTPUT_I2S2))
        {
            if (pOutput_Config->If_Channel_Swap == DTV_FALSE)
            {
                //GL_MutexLock(gAudioDacMapping.mutexID);
                gAudioDacMapping.mapping.DacMapping_Data2_L=AUDIF_DAC_MAP_PCM1_L;
                gAudioDacMapping.mapping.DacMapping_Data2_R=AUDIF_DAC_MAP_PCM1_R;
                //GL_MutexUnlock(gAudioDacMapping.mutexID);
            }
            else
            {
                //GL_MutexLock(gAudioDacMapping.mutexID);
                gAudioDacMapping.mapping.DacMapping_Data2_L=AUDIF_DAC_MAP_PCM1_R;
                gAudioDacMapping.mapping.DacMapping_Data2_R=AUDIF_DAC_MAP_PCM1_L;
                //GL_MutexUnlock(gAudioDacMapping.mutexID);
            }
        }
        else
        {
        }
    }
    else if(pOutput_Config->Process_Path==TVFE_AUDIO_AUXILIARY_2_PATH)
    {
        if((pOutput_Config->Output_Port==TVFE_AUDIO_OUTPUT_DAC0)||
            (pOutput_Config->Output_Port==TVFE_AUDIO_OUTPUT_I2S0))
        {
            if (pOutput_Config->If_Channel_Swap == DTV_FALSE)
            {
                //GL_MutexLock(gAudioDacMapping.mutexID);
                gAudioDacMapping.mapping.DacMapping_Data0_L=AUDIF_DAC_MAP_PCM2_L;
                gAudioDacMapping.mapping.DacMapping_Data0_R=AUDIF_DAC_MAP_PCM2_R;
                //GL_MutexUnlock(gAudioDacMapping.mutexID);
            }
            else
            {
                //GL_MutexLock(gAudioDacMapping.mutexID);
                gAudioDacMapping.mapping.DacMapping_Data0_L=AUDIF_DAC_MAP_PCM2_R;
                gAudioDacMapping.mapping.DacMapping_Data0_R=AUDIF_DAC_MAP_PCM2_L;
                //GL_MutexUnlock(gAudioDacMapping.mutexID);
            }
        }
        else if((pOutput_Config->Output_Port==TVFE_AUDIO_OUTPUT_DAC1)||
            (pOutput_Config->Output_Port==TVFE_AUDIO_OUTPUT_I2S1))
        {
            if (pOutput_Config->If_Channel_Swap == DTV_FALSE)
            {
                //GL_MutexLock(gAudioDacMapping.mutexID);
                gAudioDacMapping.mapping.DacMapping_Data1_L=AUDIF_DAC_MAP_PCM2_L;
                gAudioDacMapping.mapping.DacMapping_Data1_R=AUDIF_DAC_MAP_PCM2_R;
                //GL_MutexUnlock(gAudioDacMapping.mutexID);
            }
            else
            {
                //GL_MutexLock(gAudioDacMapping.mutexID);
                gAudioDacMapping.mapping.DacMapping_Data1_L=AUDIF_DAC_MAP_PCM2_R;
                gAudioDacMapping.mapping.DacMapping_Data1_R=AUDIF_DAC_MAP_PCM2_L;
                //GL_MutexUnlock(gAudioDacMapping.mutexID);
            }
        }
        else if((pOutput_Config->Output_Port==TVFE_AUDIO_OUTPUT_DAC2)||
            (pOutput_Config->Output_Port==TVFE_AUDIO_OUTPUT_I2S2))
        {
            if (pOutput_Config->If_Channel_Swap == DTV_FALSE)
            {
                //GL_MutexLock(gAudioDacMapping.mutexID);
                gAudioDacMapping.mapping.DacMapping_Data2_L=AUDIF_DAC_MAP_PCM2_L;
                gAudioDacMapping.mapping.DacMapping_Data2_R=AUDIF_DAC_MAP_PCM2_R;
                //GL_MutexUnlock(gAudioDacMapping.mutexID);
            }
            else
            {
                //GL_MutexLock(gAudioDacMapping.mutexID);
                gAudioDacMapping.mapping.DacMapping_Data2_L=AUDIF_DAC_MAP_PCM2_R;
                gAudioDacMapping.mapping.DacMapping_Data2_R=AUDIF_DAC_MAP_PCM2_L;
                //GL_MutexUnlock(gAudioDacMapping.mutexID);
            }
        }
        else
        {
        }

    }
    #if 0
    diag_printf("Leon gData0_L=%x\n",gAudioDacMapping.mapping.DacMapping_Data0_L);
    diag_printf("Leon gData0_R=%x\n",gAudioDacMapping.mapping.DacMapping_Data0_R);
    diag_printf("Leon gData1_L=%x\n",gAudioDacMapping.mapping.DacMapping_Data1_L);
    diag_printf("Leon gData1_R=%x\n",gAudioDacMapping.mapping.DacMapping_Data1_R);
    diag_printf("Leon gData2_L=%x\n",gAudioDacMapping.mapping.DacMapping_Data2_L);
    diag_printf("Leon gData2_R=%x\n",gAudioDacMapping.mapping.DacMapping_Data2_R);
    #endif
    if(AUD_SUCCESS != AUDIF_Set_ChannelMap(&gAudioDacMapping.mapping))
    {
        ret=TVFE_AUDIO_ERR_FAILURE;
    }
    return ret;

}

/********************************************************************
Prototype: TVFE_Audio_Set_Output_Selection()
Parameters:
            pOutput_Selection->Process_Path
            pOutput_Selection->Selection
Returns:TVFE_Audio_Status_e
Description: This function is used to set input source of aux1 aux2
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Output_Selection(TVFE_Audio_Output_Selection_t *pOutput_Selection,PLF_AVType_e eSrcVideoType)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

    if(pOutput_Selection->Process_Path==TVFE_AUDIO_AUXILIARY_1_PATH)
    {
    	if (pOutput_Selection->Selection == TVFE_AUDIO_OUTPUT_CURRENT_SOURCE)
    	{
	            //GL_MutexLock(gAudioAuxPath.mutexID);

                if(eSrcVideoType == PLF_INPUT_PIN_TYPE_ATV_TUNER)
                {
                    gAudioAuxPath.Aux1Path=AUDIF_AUX1_PATH_SIF;
    			    AUDIF_Init_SifIn();
                }
                else if((eSrcVideoType == PLF_INPUT_PIN_TYPE_DTV) || (eSrcVideoType == PLF_INPUT_PIN_TYPE_STORAGE)
                    || (eSrcVideoType == PLF_INPUT_PIN_TYPE_HDMI))
                {
                    gAudioAuxPath.Aux1Path=AUDIF_AUX1_PATH_MIXER;
                }
                else if((eSrcVideoType == PLF_INPUT_PIN_TYPE_CVBS) || (eSrcVideoType == PLF_INPUT_PIN_TYPE_SVIDEO)
                    || (eSrcVideoType == PLF_INPUT_PIN_TYPE_YPBPR) || (eSrcVideoType == PLF_INPUT_PIN_TYPE_RGB)
                    || (eSrcVideoType == PLF_INPUT_PIN_TYPE_SCART_AV) || (eSrcVideoType == PLF_INPUT_PIN_TYPE_SCART_SV)
                    || (eSrcVideoType == PLF_INPUT_PIN_TYPE_SCART_AV_RGB))
                {
                    gAudioAuxPath.Aux1Path=AUDIF_AUX1_PATH_AUX;
                }

                //AUDIF_Set_AuxChannelOutput(gAudioAuxPath.Aux1Path, gAudioAuxPath.Aux2Path);
	            //GL_MutexUnlock(gAudioAuxPath.mutexID);

    	}
    	else if (pOutput_Selection->Selection == TVFE_AUDIO_OUTPUT_ANALOG_TV)
    	{
                //GL_MutexLock(gAudioAuxPath.mutexID);
                if(eSrcVideoType == PLF_INPUT_PIN_TYPE_DTV)
                {
                    gAudioAuxPath.Aux1Path=AUDIF_AUX1_PATH_MIXER;
                }
                else
                {
                    gAudioAuxPath.Aux1Path=AUDIF_AUX1_PATH_SIF;
            	    AUDIF_Init_SifIn();
                    //AUDIF_Set_AuxChannelOutput(gAudioAuxPath.Aux1Path, gAudioAuxPath.Aux2Path);
                }
                //GL_MutexUnlock(gAudioAuxPath.mutexID);


        }
    	else
    	{

    	}
    }
    else if(pOutput_Selection->Process_Path==TVFE_AUDIO_AUXILIARY_2_PATH)
    {
    	if (pOutput_Selection->Selection == TVFE_AUDIO_OUTPUT_CURRENT_SOURCE)
    	{
	            //GL_MutexLock(gAudioAuxPath.mutexID);
                if(eSrcVideoType == PLF_INPUT_PIN_TYPE_ATV_TUNER)
                {
                		//AUDIF_Init_SpdifIn(1);
				//AUDIF_UnInit_SpdifIn();
    			    AUDIF_Init_SifIn();
                    gAudioAuxPath.Aux2Path=AUDIF_AUX2_PATH_SIF;
                }
                else if((eSrcVideoType == PLF_INPUT_PIN_TYPE_DTV) || (eSrcVideoType == PLF_INPUT_PIN_TYPE_STORAGE)
                    || (eSrcVideoType == PLF_INPUT_PIN_TYPE_HDMI))
                {
                    gAudioAuxPath.Aux2Path=AUDIF_AUX2_PATH_MIXER;
                }
                else if((eSrcVideoType == PLF_INPUT_PIN_TYPE_CVBS) || (eSrcVideoType == PLF_INPUT_PIN_TYPE_SVIDEO)
                    || (eSrcVideoType == PLF_INPUT_PIN_TYPE_YPBPR) || (eSrcVideoType == PLF_INPUT_PIN_TYPE_RGB)
                    || (eSrcVideoType == PLF_INPUT_PIN_TYPE_SCART_AV) || (eSrcVideoType == PLF_INPUT_PIN_TYPE_SCART_SV)
                    || (eSrcVideoType == PLF_INPUT_PIN_TYPE_SCART_AV_RGB))
                {
                    gAudioAuxPath.Aux2Path=AUDIF_AUX2_PATH_AUX;
                }
                //AUDIF_Set_AuxChannelOutput(gAudioAuxPath.Aux1Path, gAudioAuxPath.Aux2Path);
	            //GL_MutexUnlock(gAudioAuxPath.mutexID);
    	}
    	else if (pOutput_Selection->Selection == TVFE_AUDIO_OUTPUT_ANALOG_TV)
    	{
	            //GL_MutexLock(gAudioAuxPath.mutexID);
                if(eSrcVideoType == PLF_INPUT_PIN_TYPE_DTV)
                {
                    gAudioAuxPath.Aux2Path=AUDIF_AUX1_PATH_MIXER;
                }
                else
                {
                    gAudioAuxPath.Aux2Path=AUDIF_AUX2_PATH_SIF;
			//AUDIF_Init_SpdifIn(1);
			//AUDIF_UnInit_SpdifIn();
            	    AUDIF_Init_SifIn();
                    //AUDIF_Set_AuxChannelOutput(gAudioAuxPath.Aux1Path, gAudioAuxPath.Aux2Path);
                }
                //GL_MutexUnlock(gAudioAuxPath.mutexID);


        }
    	else
    	{

    	}
    }
    else
    {
    }
    #if 1
    diag_printf("Leon Aux1Path=%x\n",gAudioAuxPath.Aux1Path);
    diag_printf("Leon Aux2Path=%x\n",gAudioAuxPath.Aux2Path);
    #endif
    if(AUD_SUCCESS != AUDIF_Set_AuxChannelOutput(gAudioAuxPath.Aux1Path, gAudioAuxPath.Aux2Path))
    {
        ret=TVFE_AUDIO_ERR_FAILURE;
    }
    return ret;

}

/********************************************************************
Prototype: TVFE_Audio_Set_Volume()
Parameters:
            pTable:volume table address
            Size:the size of the volume table
            Index:the index of the volume table
            OffsetStep:the offset by the source
Returns:TVFE_Audio_Status_e
Description: This function is used to set master volume
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Volume(const INT16 *pTable, UINT8 Size, UINT8 Index, INT8 OffsetStep)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;
	INT32 PL_Volume = 0;

	/*assign volume from table pointer and table index*/
	PL_Volume = (INT32)*(pTable+Index);
//diag_printf("jaosn index %d-- PL_Volume%d -- OffsetStep%d\n",Index,PL_Volume,OffsetStep);
    //TVFE_AUDIO_DBG("Volume = %d\n",PL_Volume);

	/*add offset step*/
	PL_Volume += OffsetStep;

    TVFE_AUDIO_DBG("Volume + Volume Offset = %d\n",PL_Volume);


	if (Index > Size)
	{
		//TVFE_AUDIO_DBG("Size=%d, Index=%d,\n",Size,Index);
		//TVFE_AUDIO_DBG("index over range\n");
	}

	if (pTable == NULL)
	{
		TVFE_AUDIO_DBG("NULL pointer\n");
		return TVFE_AUDIO_ERR_FAILURE;
	}

    if (PL_Volume < -672)
    {
        PL_Volume = -672;
    }

    if (PL_Volume > 192)
    {
        PL_Volume = 192;
    }

	if (AUD_SUCCESS != AUDIF_Set_MasterVolume(PL_Volume))
	{//not success
		//TVFE_AUDIO_DBG("not success\n");
		ret = TVFE_AUDIO_ERR_FAILURE;
	}
	else
	{
		//TVFE_AUDIO_DBG("volume=%d\n",PL_Volume);
	}

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_Bass()
Parameters:
            pTable:bass table address
            Size:the size of the bass table
            Index:the index of the bass table
Returns:TVFE_Audio_Status_e
Description: This function is used to set bass
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Bass(const INT8 *pTable, UINT8 Size, UINT8 Index)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	if (Index > Size)
	{
		//TVFE_AUDIO_DBG("Size=%d, Index=%d,\n",Size,Index);
		//TVFE_AUDIO_DBG("index over range\n");
        return TVFE_AUDIO_ERR_INVALID_PARAM;
	}

	if (pTable == NULL)
	{
		TVFE_AUDIO_DBG("NULL pointer\n");
		return TVFE_AUDIO_ERR_FAILURE;
	}

	if (AUD_SUCCESS != AUDIF_Set_Bass((INT32)*(pTable+Index)))
	{//not success
		//TVFE_AUDIO_DBG("not success\n");
		ret = TVFE_AUDIO_ERR_FAILURE;
	}
	else
	{
		TVFE_AUDIO_DBG("bass=%d\n",(INT32)*(pTable+Index));
	}

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_Treble()
Parameters:
            pTable:treble table address
            Size:the size of the treble table
            Index:the index of the treble table
Returns:TVFE_Audio_Status_e
Description: This function is used to set treble
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Treble(const INT8 *pTable, UINT8 Size, UINT8 Index)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	if (Index > Size)
	{
		//TVFE_AUDIO_DBG("Size=%d, Index=%d,\n",Size,Index);
		//TVFE_AUDIO_DBG("index over range\n");
        return TVFE_AUDIO_ERR_INVALID_PARAM;
	}

	if (pTable == NULL)
	{
		//TVFE_AUDIO_DBG("NULL pointer\n");
		return TVFE_AUDIO_ERR_FAILURE;
	}

	if (AUD_SUCCESS != AUDIF_Set_Treble((INT32)*(pTable+Index)))
	{//not success
		//TVFE_AUDIO_DBG("not success\n");
		ret = TVFE_AUDIO_ERR_FAILURE;
	}
	else
	{
		TVFE_AUDIO_DBG("treble=%d\n",(INT32)*(pTable+Index));
	}

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_Main_Equlizer_Config()
Parameters:
            pTable:equlizer table address
            Size:the size of the equlizer table
            Index:the index of the equlizer table
Returns:TVFE_Audio_Status_e
Description: This function is used to set equlizer
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Main_Equlizer_Config(const TVFE_Audio_Equalizer_Config_t *pTable, UINT8 Size, UINT8 Index)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;
	const TVFE_Audio_Equalizer_Config_t *pEQ;
	Aud_EqualizerConfig_t EQConfig;

	if (Index > Size)
	{
		//TVFE_AUDIO_DBG("Size=%d, Index=%d,\n",Size,Index);
		//TVFE_AUDIO_DBG("index over range\n");
        return TVFE_AUDIO_ERR_INVALID_PARAM;
	}

	if (pTable == NULL)
	{
		//TVFE_AUDIO_DBG("NULL pointer\n");
		return TVFE_AUDIO_ERR_FAILURE;
	}
	else
	{
		pEQ = pTable + Index;
	}

	EQConfig.fEnBypass = (Boolean)(pEQ->Enable_Bypass);
	EQConfig.bMainEQ_Band1_0100HZ = (INT8)(pEQ->Band1_0100HZ);
	EQConfig.bMainEQ_Band2_0300HZ = (INT8)(pEQ->Band2_0300HZ);
	EQConfig.bMainEQ_Band3_1000HZ = (INT8)(pEQ->Band3_1000HZ);
	EQConfig.bMainEQ_Band4_3000HZ = (INT8)(pEQ->Band4_3000HZ);
	EQConfig.bMainEQ_Band5_8000HZ = (INT8)(pEQ->Band5_8000HZ);

	if (AUD_SUCCESS != AUDIF_Set_MainEqulizer(&EQConfig))
	{
		//TVFE_AUDIO_DBG("not success\n");
		ret = TVFE_AUDIO_ERR_FAILURE;
	}
	else
	{
		TVFE_AUDIO_DBG("EQConfig.bMainEQ_Band1_0100HZ=%d\n",EQConfig.bMainEQ_Band1_0100HZ);
		TVFE_AUDIO_DBG("EQConfig.bMainEQ_Band2_0300HZ=%d\n",EQConfig.bMainEQ_Band2_0300HZ);
		TVFE_AUDIO_DBG("EQConfig.bMainEQ_Band3_1000HZ=%d\n",EQConfig.bMainEQ_Band3_1000HZ);
		TVFE_AUDIO_DBG("EQConfig.bMainEQ_Band4_3000HZ=%d\n",EQConfig.bMainEQ_Band4_3000HZ);
		TVFE_AUDIO_DBG("EQConfig.bMainEQ_Band5_8000HZ=%d\n",EQConfig.bMainEQ_Band5_8000HZ);
	}

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Get_Main_Equlizer_Config()
Parameters:
            pTable:equlizer table address

Returns:TVFE_Audio_Status_e
Description: This function is used to get equlizer config
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Get_Main_Equlizer_Config(TVFE_Audio_Equalizer_Config_t *pEQ)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;
	Aud_EqualizerConfig_t EQConfig;

    AUDIF_Get_MainEqulizer(&EQConfig);
    pEQ->Enable_Bypass = EQConfig.fEnBypass;
    pEQ->Band1_0100HZ = EQConfig.bMainEQ_Band1_0100HZ;
    pEQ->Band2_0300HZ = EQConfig.bMainEQ_Band2_0300HZ;
    pEQ->Band3_1000HZ = EQConfig.bMainEQ_Band3_1000HZ;
    pEQ->Band4_3000HZ = EQConfig.bMainEQ_Band4_3000HZ;
    pEQ->Band5_8000HZ = EQConfig.bMainEQ_Band5_8000HZ;
	
	TVFE_AUDIO_DBG("EQConfig.bMainEQ_Band1_0100HZ=%d\n",EQConfig.bMainEQ_Band1_0100HZ);
	TVFE_AUDIO_DBG("EQConfig.bMainEQ_Band2_0300HZ=%d\n",EQConfig.bMainEQ_Band2_0300HZ);
	TVFE_AUDIO_DBG("EQConfig.bMainEQ_Band3_1000HZ=%d\n",EQConfig.bMainEQ_Band3_1000HZ);
	TVFE_AUDIO_DBG("EQConfig.bMainEQ_Band4_3000HZ=%d\n",EQConfig.bMainEQ_Band4_3000HZ);
	TVFE_AUDIO_DBG("EQConfig.bMainEQ_Band5_8000HZ=%d\n",EQConfig.bMainEQ_Band5_8000HZ);

    return ret;

}
/********************************************************************
Prototype: TVFE_Audio_Set_Balance()
Parameters:
            pTable:balance table address
            Size:the size of the balance table
            LeftIndex:the left index of the balance table
            RightIndex:the right index of the balance table
Returns:TVFE_Audio_Status_e
Description: This function is used to set balance
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Balance(const INT8 *pTable, UINT8 Size, UINT8 LeftIndex, UINT8 RightIndex)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	if ((LeftIndex > Size)
		||(RightIndex > Size))
	{
		//TVFE_AUDIO_DBG("Size=%d, LeftIndex=%d, RightIndex=%d\n",Size,LeftIndex,RightIndex);
		//TVFE_AUDIO_DBG("index over range\n");
        return TVFE_AUDIO_ERR_INVALID_PARAM;
	}

	if (pTable == NULL)
	{
		//TVFE_AUDIO_DBG("NULL pointer\n");
		return TVFE_AUDIO_ERR_FAILURE;
	}

	if (AUD_SUCCESS != AUDIF_Set_Balance((INT32)*(pTable+LeftIndex), (INT32)*(pTable+RightIndex)))
	{//not success
		//TVFE_AUDIO_DBG("not success\n");
		ret = TVFE_AUDIO_ERR_FAILURE;
	}
	else
	{
		TVFE_AUDIO_DBG("LeftVolume=%d, RightVolume=%d\n",(INT32)*(pTable+LeftIndex),(INT32)*(pTable+RightIndex));
	}

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_AUX_Volume()
Parameters:
            pAUX_Volume_Config->Process_Path
            pAUX_Volume_Config->Volume
Returns:TVFE_Audio_Status_e
Description: This function is used to set aux1 volume and aux2 volume
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_AUX_Volume(TVFE_Audio_AUX_Volume_t *pAUX_Volume_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	switch(pAUX_Volume_Config->Process_Path)
	{
		case TVFE_AUDIO_AUXILIARY_1_PATH:
            if (AUD_SUCCESS !=AUDIF_Set_Aux1Volume(pAUX_Volume_Config->Volume))
            {
                ret = TVFE_AUDIO_ERR_FAILURE;
            }
            break;
		case TVFE_AUDIO_AUXILIARY_2_PATH:
            if (AUD_SUCCESS !=AUDIF_Set_Aux2Volume(pAUX_Volume_Config->Volume))
            {
                ret = TVFE_AUDIO_ERR_FAILURE;
            }
            break;
		default:
			return TVFE_AUDIO_ERR_INVALID_PARAM;
	}

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_Effect_Mode()
Parameters:
            Effect_Mode:TVFE_Audio_Effect_e
Returns:TVFE_Audio_Status_e
Description: This function is used to set sound effect
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Effect_Mode(TVFE_Audio_Effect_e Effect_Mode)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	switch(Effect_Mode)
	{
		case TVFE_AUDIO_EFFECT_MODE_NORMAL:
			AUDIF_Set_EffectMode(AUD_AUDIO_EFFECT_MODE_NORMAL);
			AUDIF_Set_SUNSUR_OnOff(SUNSUR_OFF);
		    break;

		case TVFE_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_XT_DPL:
			AUDIF_Set_EffectMode(AUD_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_XT_DPL);
		    break;

		case TVFE_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_XT_PASSIVE_MATRIX:
			AUDIF_Set_EffectMode(AUD_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_XT_PASSIVE_MATRIX);
		    break;

		case TVFE_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_HD_DPL:
			AUDIF_Set_EffectMode(AUD_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_HD_DPL);
		    break;

		case TVFE_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_HD_PASSIVE_MATRIX:
			AUDIF_Set_EffectMode(AUD_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_HD_PASSIVE_MATRIX);
			AUDIF_Set_SUNSUR_OnOff(SUNSUR_ON);
		    break;

		case TVFE_AUDIO_EFFECT_MODE_NOISE_SEQUENCING:
			AUDIF_Set_EffectMode(AUD_AUDIO_EFFECT_MODE_NOISE_SEQUENCING);
		    break;

		default:
			//TVFE_AUDIO_DBG("not success\n");
			ret = TVFE_AUDIO_ERR_INVALID_PARAM;
		    break;
	}

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_AVL_Config()
Parameters:
            pAVL_Config->Mode
            pAVL_Config->Enable_Weight_Filter
            pAVL_Config->Enable_Peak_Mode_With_ClipManagement
            pAVL_Config->Enable_AVL_Func
Returns:TVFE_Audio_Status_e
Description: This function is used to set AVL
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_AVL_Config(TVFE_Audio_AVL_Config_t *pAVL_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;
	Aud_AudioAVLConfig_t AVLConfig;

	AVLConfig.eMode = pAVL_Config->Mode;
	AVLConfig.bReference_level = (INT8)pAVL_Config->Level;
	AVLConfig.fEnWeightFilter = (Boolean)pAVL_Config->Enable_Weight_Filter;
	AVLConfig.fEnPeakModeWithClipManagement = (Boolean)pAVL_Config->Enable_Peak_Mode_With_ClipManagement;
	AVLConfig.Enable_AVL_Func = (Boolean)pAVL_Config->Enable_AVL_Func;

	TVFE_AUDIO_DBG("Mode:%d\n",AVLConfig.eMode);
	TVFE_AUDIO_DBG("Level::%d\n",AVLConfig.bReference_level);
	TVFE_AUDIO_DBG("Enable_Weight_Filter:%d\n",AVLConfig.fEnWeightFilter);
	TVFE_AUDIO_DBG("Enable_Peak_Mode_With_ClipManagement:%d\n",AVLConfig.fEnPeakModeWithClipManagement);
	TVFE_AUDIO_DBG("Enable_AVL_Func:%d\n",AVLConfig.Enable_AVL_Func);

	if (AUD_SUCCESS != AUDIF_Set_AVL(&AVLConfig))
	{
		//TVFE_AUDIO_DBG("not success\n");
		ret = TVFE_AUDIO_ERR_FAILURE;
	}

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_Mute_Config()
Parameters:
            pMute_Config->pExternal_Mute_Control
            pMute_Config->Process_Path
            pMute_Config->Enable
            pMute_Config->Mute_Delay
Returns:TVFE_Audio_Status_e
Description: This function is used to set mute main, aux1 and aux2
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Mute_Config(TVFE_Audio_Mute_t *pMute_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;
    UINT8 mute_status;

	TVFE_AUDIO_DBG("Enable:%d\n",pMute_Config->Enable);
	TVFE_AUDIO_DBG("Process_Path:%d\n",pMute_Config->Process_Path);
	TVFE_AUDIO_DBG("External_Mute_Control:%d\n",pMute_Config->pExternal_Mute_Control);

	if (pMute_Config == NULL)
	{
		//TVFE_AUDIO_DBG("NULL pointer\n");
		return TVFE_AUDIO_ERR_FAILURE;
	}

	//find process and mute control path with ouput config table

	if (pMute_Config->pExternal_Mute_Control != NULL)
	{//mute control for external device
		pMute_Config->pExternal_Mute_Control(pMute_Config->Enable);
	}
	else
	{
		//TVFE_AUDIO_DBG("NULL pointer\n");
	}

    if(pMute_Config->Process_Path == TVFE_AUDIO_MAIN_PATH)
    {
        AUDIF_Get_MasterMuteStatus(&mute_status);
     	if (pMute_Config->Enable == TVFE_TRUE)
    	{
    	    if(mute_status == TVFE_FALSE)
            {
                AUDIF_Set_MasterMuteStatus(MUTE_STATUS_ON);
            }
    	}
        else
        {
            AUDIF_RampUp_PCM_Out();
    	    if(mute_status == TVFE_TRUE)
            {
                AUDIF_Set_MasterMuteStatus(MUTE_STATUS_OFF);
            }
        }
    }
    else if(pMute_Config->Process_Path == TVFE_AUDIO_AUXILIARY_1_PATH)
    {
        AUDIF_Get_Aux1MuteStatus(&mute_status);
     	if (pMute_Config->Enable == TVFE_TRUE)
    	{
            if(mute_status == TVFE_FALSE)
            {
                AUDIF_Set_Aux1MuteStatus(MUTE_STATUS_ON);
            }
    	}
        else
        {
    	    if(mute_status == TVFE_TRUE)
            {
                AUDIF_Set_Aux1MuteStatus(MUTE_STATUS_OFF);
            }
        }
    }
    else if(pMute_Config->Process_Path == TVFE_AUDIO_AUXILIARY_2_PATH)
    {
        AUDIF_Get_Aux2MuteStatus(&mute_status);
     	if (pMute_Config->Enable == TVFE_TRUE)
    	{
            if(mute_status == TVFE_FALSE)
            {
                AUDIF_Set_Aux2MuteStatus(MUTE_STATUS_ON);
            }
    	}
        else
        {
    	    if(mute_status == TVFE_TRUE)
            {
                AUDIF_Set_Aux2MuteStatus(MUTE_STATUS_OFF);
            }
        }
    }
    else
    {
        ret = TVFE_AUDIO_ERR_INVALID_PARAM;
    }

	//delay for ramp down
	if(pMute_Config->Mute_Delay != 0)
	{
        //GL_TaskSleep(pMute_Config->Mute_Delay);
        msleep(pMute_Config->Mute_Delay);
	}
	return ret;
}


/********************************************************************
Prototype: TVFE_Audio_Initial()
Parameters:void
Returns:TVFE_Audio_Status_e
Description: This function is used to initiate the audio module
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Initial()
{
	TVFE_Audio_Status_e  ret = TVFE_AUDIO_SUCCESS;

	TVFE_AudioInitialConfig_st InitialParam;
	TVFE_AudioLevelAdjust_st LevelAdjust_config;
    TVFE_Audio_SIF_Over_Modulation_Config_t pOver_Modulation_Config;

    AUDIF_Init_Audio();

    TVFE_AudioParamInit();

	TVFE_AudioPowerControl(TVFE_AUDIO_ALL_MODULE_POWER_ON);

    InitialParam.bPGA_gain = 0;
    DRV_AudSetSIFADCPGAGain(InitialParam.bPGA_gain);
    DRV_AudEnableSSDROMReg(DTV_TRUE);
	DRV_AudInit();

//	TVFE_Audio_MainChannel();
//    TVFE_AudioSetSIFMode(TVFE_AUDIO_PATH_PRIMARY, TVFE_AUDIO_SIF_MODE_ANALOG_MONO);
    TVFE_AudioSetSIFMode(TVFE_AUDIO_PATH_AUX1, TVFE_AUDIO_SIF_MODE_ANALOG_MONO);

	//TVFE_AudioSetSIFConnect(TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_ALL);

    LevelAdjust_config.bDECAdjustLevel		= 0;
	LevelAdjust_config.bMonoAdjustLevel		= 0;
	LevelAdjust_config.bNICAMAdjustLevel	= 0;
	LevelAdjust_config.bSAPAdjustLevel		= 0;
	LevelAdjust_config.bADCAdjustLevel		= 0;
	LevelAdjust_config.bDigitalAdjustLevel	= 0;
	TVFE_AudioSetLevelAdjust(&LevelAdjust_config);

    pOver_Modulation_Config.Attenuation     = 0;
    pOver_Modulation_Config.Enable_Adapter  = TVFE_FALSE;
    pOver_Modulation_Config.Filter_BandWidth = 0;
    pOver_Modulation_Config.Level_Thershold = 0;
    TVFE_Audio_Set_SIF_Over_Modulation_Config( &pOver_Modulation_Config);

#if 0//Test code

    TVFE_AudioSetSIFMode(TVFE_AUDIO_PATH_AUX1, TVFE_AUDIO_SIF_MODE_ANALOG_MONO);

DRV_AudSetBeeperConfig(DRV_AUD_PROCESS_PATH_AUX1,0,DRV_AUD_BEEPERFREQUENCY_01000HZ,DRV_AUD_BEEPER_GRAPHIC_MODE);
	
#endif
	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_Level_Adjust_Config()
Parameters:
            pLevel_Adjust_Config->DEC_Adjust_Level
            pLevel_Adjust_Config->Mono_Adjust_Level
            pLevel_Adjust_Config->NICAM_Adjust_Level
            pLevel_Adjust_Config->SAP_Adjust_Level
Returns:TVFE_Audio_Status_e
Description: This function is used to set the level adjustment for DEC, Mono, NICAM, SAP
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Level_Adjust_Config(TVFE_Audio_Level_Adjust_t *pLevel_Adjust_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;
	TVFE_AudioLevelAdjust_st PL_LevelAdjust;

	if (pLevel_Adjust_Config == NULL)
	{
		//TVFE_AUDIO_DBG("NULL pointer\n");
		return TVFE_AUDIO_ERR_FAILURE;
	}

	PL_LevelAdjust.bDECAdjustLevel = (INT8)(pLevel_Adjust_Config->DEC_Adjust_Level);
	PL_LevelAdjust.bMonoAdjustLevel = (INT8)(pLevel_Adjust_Config->Mono_Adjust_Level);
	PL_LevelAdjust.bNICAMAdjustLevel = (INT8)(pLevel_Adjust_Config->NICAM_Adjust_Level);
	PL_LevelAdjust.bSAPAdjustLevel = (INT8)(pLevel_Adjust_Config->SAP_Adjust_Level);
	PL_LevelAdjust.bADCAdjustLevel = (INT8)(pLevel_Adjust_Config->ADC_Adjust_Level);
	PL_LevelAdjust.bDigitalAdjustLevel = (INT8)(pLevel_Adjust_Config->Digital_Adjust_Level);

	if (TVFE_SUCCESS != TVFE_AudioSetLevelAdjust(&PL_LevelAdjust))
	{//not success
		//TVFE_AUDIO_DBG("not success\n");
		ret = TVFE_AUDIO_ERR_FAILURE;
	}

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_SIF_Over_Modulation_Config()
Parameters:
            pOver_Modulation_Config->Enable_Adapter
            pOver_Modulation_Config->Filter_BandWidth
            pOver_Modulation_Config->Level_Thershold
            pOver_Modulation_Config->Attenuation
Returns:TVFE_Audio_Status_e
Description: This function is used to set the over modulation of SIF path
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_SIF_Over_Modulation_Config(TVFE_Audio_SIF_Over_Modulation_Config_t *pOver_Modulation_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;
	//TVFE_Audio_SIF_Over_Modulation_Config_t OverModulationConfig;

	if (pOver_Modulation_Config == NULL)
	{
		//TVFE_AUDIO_DBG("NULL pointer\n");
		return TVFE_AUDIO_ERR_FAILURE;
	}

	//OverModulationConfig.fEnAdapter = (Boolean)pOver_Modulation_Config->Enable_Adapter;
	//OverModulationConfig.bFilterBanndWidth = (UINT8)pOver_Modulation_Config->Filter_BandWidth;
	//OverModulationConfig.bLevelThershold = (UINT8)pOver_Modulation_Config->Level_Thershold;
	//OverModulationConfig.bAttenuation = (UINT8)pOver_Modulation_Config->Attenuation;

	if (DRV_SUCCESS != DRV_AudSetOverModulationAdapterConfig((Boolean)pOver_Modulation_Config->Enable_Adapter,(UINT8)pOver_Modulation_Config->Filter_BandWidth,
        (UINT8)pOver_Modulation_Config->Level_Thershold,(UINT8)pOver_Modulation_Config->Attenuation))
	{//not success
		//TVFE_AUDIO_DBG("not success\n");
		ret = TVFE_AUDIO_ERR_FAILURE;
	}

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_SIF_Detect_Config()
Parameters:
            pDetect_Config->Carrier1_Magnitude_Threshold
            pDetect_Config->MPX_Magnitude_Threshold_Low
            pDetect_Config->MPX_Magnitude_Threshold_Up
            pDetect_Config->SAP_Magnitude_Threshold_Low
            pDetect_Config->SAP_Magnitude_Threshold_Up
            pDetect_Config->Enable_Sub_Carrier_Detect
            pDetect_Config->EIAJ_Sub_Carrier_Magnitude_Threshold_Low
            pDetect_Config->EIAJ_Sub_Carrier_Magnitude_Threshold_Up
Returns:TVFE_Audio_Status_e
Description: This function is used to set detect level
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_SIF_Detect_Config(TVFE_Audio_SIF_Detect_Config_t *pDetect_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;
	//TVFE_Audio_SIF_Detect_Config_t MagnitudeDetectConfig;

	if (pDetect_Config == NULL)
	{
		//TVFE_AUDIO_DBG("NULL pointer\n");
		return TVFE_AUDIO_ERR_FAILURE;
	}

	//MagnitudeDetectConfig.bCarrier1_threshold = (INT8)pDetect_Config->Carrier1_Magnitude_Threshold;
	//MagnitudeDetectConfig.bMPX_threshold_low = (UINT8)pDetect_Config->MPX_Magnitude_Threshold_Low;
	//MagnitudeDetectConfig.bMPX_threshold_up = (UINT8)pDetect_Config->MPX_Magnitude_Threshold_Up;
	//MagnitudeDetectConfig.bSAP_threshold_low = (UINT8)pDetect_Config->SAP_Magnitude_Threshold_Low;
	//MagnitudeDetectConfig.bSAP_threshold_up = (UINT8)pDetect_Config->SAP_Magnitude_Threshold_Up;
	//MagnitudeDetectConfig.fEnSub_carrier_detect = (Boolean)pDetect_Config->Enable_Sub_Carrier_Detect;
	//MagnitudeDetectConfig.bEIAJ_sub_carrier_threshold_low = (UINT8)pDetect_Config->EIAJ_Sub_Carrier_Magnitude_Threshold_Low;
	//MagnitudeDetectConfig.bEIAJ_sub_carrier_threshold_up = (UINT8)pDetect_Config->EIAJ_Sub_Carrier_Magnitude_Threshold_Up;

	if (DRV_SUCCESS != DRV_AudSetMagnitudeDetectConfig((INT8)pDetect_Config->Carrier1_Magnitude_Threshold,(UINT8)pDetect_Config->MPX_Magnitude_Threshold_Low,
       (UINT8)pDetect_Config->MPX_Magnitude_Threshold_Up, (UINT8)pDetect_Config->SAP_Magnitude_Threshold_Low,(UINT8)pDetect_Config->SAP_Magnitude_Threshold_Up,
       (Boolean)pDetect_Config->Enable_Sub_Carrier_Detect, (UINT8)pDetect_Config->EIAJ_Sub_Carrier_Magnitude_Threshold_Low, (UINT8)pDetect_Config->EIAJ_Sub_Carrier_Magnitude_Threshold_Up))
	{//not success
		//TVFE_AUDIO_DBG("not success\n");
		ret = TVFE_AUDIO_ERR_FAILURE;
	}

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_SPDIF_Output_Config()
Parameters:
           SPDIF_Output_Config
                0:TVFE_AUDIO_SPDIF_OFF
                1:TVFE_AUDIO_SPDIF_PCM
                2:TVFE_AUDIO_SPDIF_BITSTREAM
                3:TVFE_AUDIO_SPDIF_REENC
           SPDIF_SmpRate
Returns:TVFE_Audio_Status_e
Description: This function is used to set SPDIF output
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_SPDIF_Output_Config(TVFE_Audio_SPDIF_Output_Config_e SPDIF_Output_Config,TVFE_Audio_SPDIF_Output_SmpRate_Config_e SPDIF_SmpRate)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;
	AUDIF_Set_SPDIF(SPDIF_Output_Config);
    switch(SPDIF_SmpRate)
    {
        case TVFE_AUDIO_SPDIF_Output_SmpRate_48K:
            AUDIF_Set_SPDIF_SmpRate(FS_48K);
            break;
        case TVFE_AUDIO_SPDIF_Output_SmpRate_32K:
            AUDIF_Set_SPDIF_SmpRate(FS_32K);
            break;
        case TVFE_AUDIO_SPDIF_Output_SmpRate_44K:
            AUDIF_Set_SPDIF_SmpRate(FS_44K);
            break;
        case TVFE_AUDIO_SPDIF_Output_SmpRate_96K:
            AUDIF_Set_SPDIF_SmpRate(FS_96K);
            break;
        case TVFE_AUDIO_SPDIF_Output_SmpRate_192K:
            AUDIF_Set_SPDIF_SmpRate(FS_192K);
            break;
        default:
            AUDIF_Set_SPDIF_SmpRate(FS_32K);
            break;
    }
	return ret;
}

/*!
        @}
   @}
 ******************************************************************************/

/********************************************************************
Prototype: TVFE_Audio_Set_AtvRamp()
Parameters:
            ramp_flag
            0:ramp off
            1:ramp on
Returns:TVFE_Audio_Status_e
Description: This function is used to on/off SIF Ramp output
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_AtvRamp(UINT8 ramp_flag) //add by jason
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;
	AUDIF_Set_AtvRamp(ramp_flag);
	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Lip_Sync_Config()
Parameters:
            Lip_Sync_Config.Enable
            1:enable
            0:disable
            Lip_Sync_Config.Delay
            0~252ms
Returns:TVFE_Audio_Status_e
Description: This function is used to set lip sync
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Lip_Sync_Config(TVFE_Audio_Lip_Sync_Config_t Lip_Sync_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

    if(Lip_Sync_Config.Enable==TVFE_TRUE)
    {
        ret=AUDIF_Set_LipSync_DelayTime(Lip_Sync_Config.Delay);
    }
    else
    {
        ret=AUDIF_Set_LipSync_DelayTime(0);
    }
	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_PCM_Output_Phase_Config()
Parameters:
            Pcm_Output_Phase.main_phase
            0:TVFE_AUDIO_OUTPUT_PHASE_0
            1:TVFE_AUDIO_OUTPUT_PHASE_180
            Pcm_Output_Phase.aux1_phase
            0:TVFE_AUDIO_OUTPUT_PHASE_0
            1:TVFE_AUDIO_OUTPUT_PHASE_180
            Pcm_Output_Phase.aux2_phase
            0:TVFE_AUDIO_OUTPUT_PHASE_0
            1:TVFE_AUDIO_OUTPUT_PHASE_180
Returns:TVFE_Audio_Status_e
Description: This function is used to set the PCM phase of main aux1 aux2
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_PCM_Output_Phase_Config(TVFE_Audio_Pcm_Output_Phase_Config_t Pcm_Output_Phase)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;
    UINT8 main_phase;
    UINT8 aux1_phase;
    UINT8 aux2_phase;

    if(Pcm_Output_Phase.main_phase==TVFE_AUDIO_OUTPUT_PHASE_0)
    {
        main_phase=0;
    }
    else if(Pcm_Output_Phase.main_phase==TVFE_AUDIO_OUTPUT_PHASE_180)
    {
        main_phase=1;
    }
    else
    {
        main_phase=0;
    }

    if(Pcm_Output_Phase.aux1_phase==TVFE_AUDIO_OUTPUT_PHASE_0)
    {
        aux1_phase=0;
    }
    else if(Pcm_Output_Phase.aux1_phase==TVFE_AUDIO_OUTPUT_PHASE_180)
    {
        aux1_phase=1;
    }
    else
    {
        aux1_phase=0;
    }

    if(Pcm_Output_Phase.aux2_phase==TVFE_AUDIO_OUTPUT_PHASE_0)
    {
        aux2_phase=0;
    }
    else if(Pcm_Output_Phase.aux2_phase==TVFE_AUDIO_OUTPUT_PHASE_180)
    {
        aux2_phase=1;
    }
    else
    {
        aux2_phase=0;
    }

    ret=AUDIF_Set_PcmOutputPhase(main_phase, aux1_phase, aux2_phase);
	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_SUNSUR_Config()
Parameters:
            SUNSUR_Param
Returns:TVFE_Audio_Status_e
Description: This function is used to set the gain of ambiance, reverb, direct in
             sunplus surround
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_SUNSUR_Config(TVFE_Audio_SUNSUR_Param_t SUNSUR_Param)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;
    Aud_SUNSUR_Param_t SUNSUR_Gain;

    SUNSUR_Gain.reverb_gain = SUNSUR_Param.reverb_gain;
	SUNSUR_Gain.ambiance_gain = SUNSUR_Param.ambiance_gain;
	SUNSUR_Gain.direct_gain = SUNSUR_Param.direct_gain;
    ret=AUDIF_Set_SUNSUR_Param(&SUNSUR_Gain);
	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_HEAAC_Output()
Parameters:
            OnOFF_flag: on/off
Returns:TVFE_Audio_Status_e
Description: This function is used to enable/disable the output of digital stream in HEAAC format
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_HEAAC_Output(UINT8 OnOFF_flag)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;
    if (OnOFF_flag == 0)
    {
        AUDIF_Set_Decoder_OPT(WAVE_FORMAT_AAC,NotSupport_LCAAC_HEAAC);
    }
    else if (OnOFF_flag == 1)
    {
        AUDIF_Set_Decoder_OPT(WAVE_FORMAT_AAC,Support_LCAAC_HEAAC);
    }
    return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_Spdif_ChannelStatus()
Parameters:
     CGMS_type:
*	          |_bit2_    bit15
*	0 0       |   1        x   unlimited copies allowed
*	0 1       |   -        -    not used
*	1 0       |   0        0   one copy allowed
*	1 1       |   0        1   no copy allowed
*
*	bit_res
*	0: SPDIF/PCM output 16-bit
*	1: SPDIF/PCM output 24-bit
*
*	disc_type: byte1 category code
*	0: set CD
*	1: set DVD

Returns:TVFE_Audio_Status_e
Description: Set the SCMS signal of the SPDIF output and Copy Right flag
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Spdif_ChannelStatus(UINT8 CGMS_type , UINT8 bit_res, UINT8 disc_type)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;
    AUDIF_Set_Spdif_ChannelStatus(CGMS_type , bit_res, disc_type);
    return ret;
}



TVFE_Audio_Status_e TVFE_Audio_Set_MasterMuteStatus(bool mute)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;
    gMasterMuteStatus = mute;
    return ret;
}
/********************************************************************
Prototype: TVFE_Audio_Get_MasterMuteStatus()
Parameters:

Returns: gMasterMuteStatus status, 0:unmute, 1:mute
Description: This function report main path mute status
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Get_MasterMuteStatus(bool *mute)
{

    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;
    *mute = gMasterMuteStatus;
    return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_AC3TransCoding()
Parameters:
Returns: TVFE_Audio_Status_e
Description:init encoder mode to avoid download encoder failed when mainprogram state changed.
Restrictions:call this function after TVFE_Audio_Initial()
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_AC3TransCoding(void)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;
    AUDIF_Enable_AC3TransCoding();
    return ret;
}

#if 0 // don't need this
//extern AudioManagerParam_t gAudioPrimaryParam;
/********************************************************************
Prototype: TVFE_Audio_Set_SIFHandler()
Parameters:
Returns: TVFE_Audio_Status_e
Description:Set SIF Handler
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_SIFHandler(void)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

    /*Audio manager move here by b.yang 20100826*/
    /*update frontend status*/
    //GL_MutexLock((gAudioPrimaryParam.mutex));/*resource protection begin*/ //william: do we need this
    //_tvfe_AudioGetFrontEndStatus(&(gAudioPrimaryParam.Status));
    //msleep(500);			
    //GL_MutexUnlock((gAudioPrimaryParam.mutex));/*resource protection end*/ //william: do we need this
    _AM_SIFHandler(&gAudioPrimaryParam);
    msleep(500);		


    return ret;
}
#endif

#if 0
/********************************************************************
Prototype: TVFE_Audio_Get_SIFHandler_Status()
Parameters:
Returns: TVFE_Audio_Status_e
Description:Get SIF Handler Status
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Get_SIFHandler_Status(TVFE_Audio_SIFHandler_Status_e *SIF_Status)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

    if((gAudioPrimaryParam.eState == AUDIO_MANAGER_CONNECT_READY)
        ||(gAudioPrimaryParam.eState == AUDIO_MANAGER_CONNECT_TIME_OUT))
    {
        *SIF_Status=TVFE_Audio_SIFHandler_Ready;
    }
    else
    {
        *SIF_Status=TVFE_Audio_SIFHandler_Busy;
    }
    return ret;
}
#endif



TVFE_Audio_Status_e TVFE_Audio_Set_SourceType(UINT32 Param)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

    AUDIF_Set_SourceType(Param);

    return ret;
}

/********************************************************************
Prototype: AUDIF_Get_DdpFmt()
Parameters:
Returns:   TVFE_Audio_WAVE_FORMAT_DOLBY_DIGITAL_PLUS: DDP type
           TVFE_Audio_WAVE_FORMAT_DOLBY_AC3: AC3 type
           TVFE_Audio_WAVE_FORMAT_DOLBY_NA: neither DDP nor AC3
Description: Application layer polling this function to get the stream
             type when playing stream.
Restrictions:
********************************************************************/
UINT16 TVFE_Audio_Get_DdpFmt(void)
{
    UINT16 type;
    UINT16 stream_type;
    AUDIF_Get_DdpFmt(&type);
    switch(type)
    {
        case WAVE_FORMAT_DOLBY_DIGITAL_PLUS:
            stream_type = TVFE_Audio_WAVE_FORMAT_DOLBY_DIGITAL_PLUS;
            break;
        case WAVE_FORMAT_DOLBY_AC3:
            stream_type = TVFE_Audio_WAVE_FORMAT_DOLBY_AC3;
            break;
        case 0:
            stream_type = TVFE_Audio_WAVE_FORMAT_DOLBY_NA;
            break;
        default:
            stream_type = TVFE_Audio_WAVE_FORMAT_DOLBY_NA;
            break;
    }
    return stream_type;
}

// part of tvfe_api.c
/*****************************************************************************
** FUNCTION : TVFE_AudioPowerControl
**
** DESCRIPTION :
**				Audio module power control.
**
** PARAMETERS :
**				NONE
**
** RETURN VALUES:
**				TVFE_SUCCESS
**				TVFE_ERR_INVALID_PARAM
*****************************************************************************/
TVFE_Status_e TVFE_AudioPowerControl(TVFE_AudioPowerContro_e ePowerControl)
{
	TVFE_AUDIO_DBG("ePowerControl:%d\n",ePowerControl);

	switch(ePowerControl)
	{
		case TVFE_AUDIO_LINE_IN_ADC_POWER_ON:
			DRV_AudSetPowerControl(DRV_AUD_LINE_IN_ADC_POWER_ON);
			break;

		case TVFE_AUDIO_LINE_IN_ADC_POWER_OFF:
			DRV_AudSetPowerControl(DRV_AUD_LINE_IN_ADC_POWER_OFF);
			break;

        case TVFE_AUDIO_DAC_VCOM_ON:
			DRV_AudSetPowerControl(DRV_AUD_DAC_VCOM_ON);
			break;

		case TVFE_AUDIO_DAC_VCOM_OFF:
			DRV_AudSetPowerControl(DRV_AUD_DAC_VCOM_OFF);
			break;

		case TVFE_AUDIO_DAC_VRT_ON:
			DRV_AudSetPowerControl(DRV_AUD_DAC_VRT_ON);
			break;

		case TVFE_AUDIO_DAC_VRT_OFF:
			DRV_AudSetPowerControl(DRV_AUD_DAC_VRT_OFF);
			break;

        case TVFE_AUDIO_DAC_POWER_ON:
			DRV_AudSetPowerControl(DRV_AUD_DAC_POWER_ON);
			break;

		case TVFE_AUDIO_DAC_POWER_OFF:
			DRV_AudSetPowerControl(DRV_AUD_DAC_POWER_OFF);
			break;

        case TVFE_AUDIO_DAC0_POWER_ON:
			DRV_AudSetPowerControl(DRV_AUD_DAC0_POWER_ON);
			break;
		case TVFE_AUDIO_DAC0_POWER_OFF:
			DRV_AudSetPowerControl(DRV_AUD_DAC0_POWER_OFF);
			break;

		case TVFE_AUDIO_DAC1_POWER_ON:
			DRV_AudSetPowerControl(DRV_AUD_DAC1_POWER_ON);
			break;
		case TVFE_AUDIO_DAC1_POWER_OFF:
			DRV_AudSetPowerControl(DRV_AUD_DAC1_POWER_OFF);
			break;

		case TVFE_AUDIO_DAC2_POWER_ON:
			DRV_AudSetPowerControl(DRV_AUD_DAC2_POWER_ON);
			break;
		case TVFE_AUDIO_DAC2_POWER_OFF:
			DRV_AudSetPowerControl(DRV_AUD_DAC2_POWER_OFF);
			break;

		case TVFE_AUDIO_ALL_MODULE_POWER_ON:
			DRV_AudSetPowerControl(DRV_AUD_ALL_POWER_ON);
			break;

		case TVFE_AUDIO_ALL_MODULE_POWER_OFF:
			DRV_AudSetPowerControl(DRV_AUD_ALL_POWER_OFF);
			break;

		default :
			return TVFE_ERR_INVALID_PARAM;
	}
    return TVFE_SUCCESS;
}

/*****************************************************************************
** FUNCTION : TVFE_AudioSetSIFMode
**
** DESCRIPTION :
**				Set SIF mode for TV audio path
**
** PARAMETERS :
**				ePath : Assign process path to switch sound type
**				eMode : Assign sound type selection
**
** RETURN VALUES:
**				TVFE_SUCCESS
**				TVFE_ERR_FAILURE
*****************************************************************************/
TVFE_Status_e TVFE_AudioSetSIFMode(TVFE_AudioPath_e ePath,TVFE_AudioSIFMode_e eMode)
{
#if 0
	/*Set Main process path*/
	_TVFE_AudioSetIFMode(DRV_AUD_SRC_1,DRV_AUD_PROCESS_PATH_MAIN,eMode);

	/*Update AUX1 process path*/
	if(TVFE_AUDIO_AUXACT_FOLLOW_TV == gAudioAUX1Path.param.action.eAUX)
	{
		_TVFE_AudioSetIFMode(DRV_AUD_SRC_2,DRV_AUD_PROCESS_PATH_AUX1,eMode);
    }

	/*Update AUX2 process path*/
	if(TVFE_AUDIO_AUXACT_FOLLOW_TV == gAudioAUX2Path.param.action.eAUX)
    {
		_TVFE_AudioSetIFMode(DRV_AUD_SRC_2,DRV_AUD_PROCESS_PATH_AUX2,eMode);
	}

	/*Update AUX3 process path*/
	if(TVFE_AUDIO_AUXACT_FOLLOW_TV == gAudioAUX3Path.param.action.eAUX)
	{
		_TVFE_AudioSetIFMode(DRV_AUD_SRC_2,DRV_AUD_PROCESS_PATH_AUX3,eMode);
    }

    /*Update Primary Path Internal Variable*/
	GL_MutexLock(gAudioPrimaryPath.mutex);
	gAudioPrimaryPath.eSIFMode = eMode;
	GL_MutexUnlock(gAudioPrimaryPath.mutex);
	return TVFE_SUCCESS;
#else
	TVFE_Status_e ret = TVFE_SUCCESS;
	switch(ePath)
	{
		case TVFE_AUDIO_PATH_PRIMARY:
			ret=TVFE_AudioSetIFMode(DRV_AUD_SRC_1,DRV_AUD_PROCESS_PATH_MAIN,eMode);
#if  0
			/*Update Primary Path Internal Variable*/
			GL_MutexLock(gAudioPathInfo.mutex);
			gAudioPathInfo.PrimaryPath.eSIFMode = eMode;
			GL_MutexUnlock(gAudioPathInfo.mutex);
#endif
			break;

		case TVFE_AUDIO_PATH_AUX1:
			ret=TVFE_AudioSetIFMode(DRV_AUD_SRC_2,DRV_AUD_PROCESS_PATH_AUX1,eMode);
			break;

		case TVFE_AUDIO_PATH_AUX2:
			ret=TVFE_AudioSetIFMode(DRV_AUD_SRC_2,DRV_AUD_PROCESS_PATH_AUX2,eMode);
			break;

		case TVFE_AUDIO_PATH_AUX3:
			ret=TVFE_AudioSetIFMode(DRV_AUD_SRC_2,DRV_AUD_PROCESS_PATH_AUX3,eMode);
			break;

		default:
			ret = TVFE_ERR_INVALID_PARAM;
			break;

	}
	return ret;
#endif
}

/*****************************************************************************
** FUNCTION : TVFE_AudioSetIFMode
**
** DESCRIPTION :
**				Set IF Mode
**
** PARAMETERS :
**				eSRCPath : SRC selection to switch sound type
**				eProcessPath : Specify which process path to switch sound type
**				eIFMode : Sound type selection
**
** RETURN VALUES:
**				TVFE_SUCCESS if successful
**				TVFE_ERR_INVALID_PARAM if parameters is unknown
*****************************************************************************/
TVFE_Status_e TVFE_AudioSetIFMode(DRV_AudSRCPath_t eSRCPath,
	DRV_AudProcessPath_t eProcessPath, TVFE_AudioSIFMode_e eIFMode)
{
	TVFE_Status_e ret = TVFE_SUCCESS;

	if(eIFMode == TVFE_AUDIO_SIF_MODE_SAP)
	{
		DRV_AudSetBTSCDBXMode(DRV_AUD_SAP_MODE);
	}
	else
	{
		DRV_AudSetBTSCDBXMode(DRV_AUD_STEREO_MODE);
	}

	switch(eIFMode)
	{
		case TVFE_AUDIO_SIF_MODE_ANALOG_MONO:
			DRV_AudSetSRCPath(eSRCPath, DRV_AUD_SRC_MONO_SAP);
			DRV_AudSetDigitalMatrix(eProcessPath, \
				DRV_AUD_DIGITAL_MATRIX_LANGUAGEA_AUTO_OFF);
			break;

		case TVFE_AUDIO_SIF_MODE_NICAM_MONO:
			DRV_AudSetSRCPath(eSRCPath, DRV_AUD_SRC_DEC);
			DRV_AudSetDigitalMatrix(eProcessPath, \
				DRV_AUD_DIGITAL_MATRIX_LANGUAGEA_AUTO_OFF);
			break;

		case TVFE_AUDIO_SIF_MODE_FORCED_MONO:
			DRV_AudSetSRCPath(eSRCPath, DRV_AUD_SRC_DEC);
			DRV_AudSetDigitalMatrix(eProcessPath, \
				DRV_AUD_DIGITAL_MATRIX_MONO_AUTO_OFF);
			break;

		case TVFE_AUDIO_SIF_MODE_STEREO:
			DRV_AudSetSRCPath(eSRCPath, DRV_AUD_SRC_DEC);
			DRV_AudSetDigitalMatrix(eProcessPath, \
				DRV_AUD_DIGITAL_MATRIX_STEREO_AUTO_OFF);
			break;

		case TVFE_AUDIO_SIF_MODE_DUAL_I:
			DRV_AudSetSRCPath(eSRCPath, DRV_AUD_SRC_DEC);
			DRV_AudSetDigitalMatrix(eProcessPath, \
				DRV_AUD_DIGITAL_MATRIX_LANGUAGEA_AUTO_OFF);
			break;

		case TVFE_AUDIO_SIF_MODE_DUAL_II:
			DRV_AudSetSRCPath(eSRCPath, DRV_AUD_SRC_DEC);
			DRV_AudSetDigitalMatrix(eProcessPath, \
				DRV_AUD_DIGITAL_MATRIX_LANGUAGEB_AUTO_OFF);
			break;

		case TVFE_AUDIO_SIF_MODE_SAP:
			DRV_AudSetSRCPath(eSRCPath, DRV_AUD_SRC_MONO_SAP);
			DRV_AudSetDigitalMatrix(eProcessPath, \
				DRV_AUD_DIGITAL_MATRIX_LANGUAGEB_AUTO_OFF);
			break;

		case TVFE_AUDIO_SIF_MODE_DUAL_I_II:
			DRV_AudSetSRCPath(eSRCPath, DRV_AUD_SRC_DEC);
			DRV_AudSetDigitalMatrix(eProcessPath, \
				DRV_AUD_DIGITAL_MATRIX_STEREO_AUTO_OFF);
			break;

		default:
			ret = TVFE_ERR_INVALID_PARAM;
			break;
	}

	return ret;
}


#define TVFE_AUDIO_LEVEL_ADJUST_VALUE_MIN     (-16)     //-16 dB
#define TVFE_AUDIO_LEVEL_ADJUST_VALUE_MAX     (15)      //+15 dB

#define TVFE_AUDIO_MASTER_VOLUME_VALUE_MIN    (-672)    //-84 dB
#define TVFE_AUDIO_MASTER_VOLUME_VALUE_MAX    (192)     //+24 dB

#define TVFE_AUDIO_MAIN_VOLUME_VALUE_MIN      (-84)     //-84 dB
#define TVFE_AUDIO_MAIN_VOLUME_VALUE_MAX      (24)      //+24 dB

#define TVFE_AUDIO_AUX_VOLUME_VALUE_MIN       (-84)     //-84 dB
#define TVFE_AUDIO_AUX_VOLUME_VALUE_MAX       (24)      //+24 dB

#define TVFE_AUDIO_MAIN_BASS_VALUE_MIN        (-16)     //-16 dB
#define TVFE_AUDIO_MAIN_BASS_VALUE_MAX        (15)      //+15 dB

#define TVFE_AUDIO_MAIN_TREBLE_VALUE_MIN      (-16)     //-16 dB
#define TVFE_AUDIO_MAIN_TREBLE_VALUE_MAX      (15)      //+15 dB

#define TVFE_AUDIO_EQUALIZER_VALUE_MIN        (-12)     //-12 dB
#define TVFE_AUDIO_EQUALIZER_VALUE_MAX        (12)      //+12 dB

#define TVFE_AUDIO_LIP_SYNC_VALUE_MIN         (0)       // 0 ms
#define TVFE_AUDIO_LIP_SYNC_VALUE_MAX         (63)      //252 ms

#ifdef TVFE_AUDIO_WARNING_MSG_ENABLE//(TVFE_AUDIO_WARNING_MSG_ENABLE)
#define TVFE_AUDIO_WAR(fmt, arg...)           diag_printf( \
								"\n@TVFE_Audio Warning:\n%s %s %d:"fmt,\
								__FILE__, __FUNCTION__, __LINE__, ##arg)
#else
#define TVFE_AUDIO_WAR(fmt, arg...)
#endif

#define _TVFE_AudioCheckValueRange(value,min,max)\
{\
	if((value < min)\
        ||(value > max))\
    {\
        TVFE_AUDIO_WAR("value=%d min=%d max=%d is out of range",value,min,max);\
        return TVFE_ERR_INVALID_PARAM;\
    }\
}\

/*****************************************************************************
** FUNCTION : TVFE_AudioSetLevelAdjust
**
** DESCRIPTION :
**				Set level adjust configuration
**
** PARAMETERS :
**			pLevelAdjust_config : 	Level adjust configuration parameters
**			pLevelAdjust_config->bDECAdjustLevel :
**								-16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 dB)
**			pLevelAdjust_config->bMonoAdjustLevel :
**								-16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 dB)
**			pLevelAdjust_config->bNICAMAdjustLevel :
**								-16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 dB)
**			pLevelAdjust_config->bSAPAdjustLevel :
**								-16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 dB)
**			pLevelAdjust_config->bADCAdjustLevel :
**								-16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 dB)
**			pLevelAdjust_config->bDigitalAdjustLevel :
**								-16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 dB)
**
** RETURN VALUES:
**				TVFE_SUCCESS
**				TVFE_ERR_INVALID_PARAM
**				TVFE_ERR_FAILURE
*****************************************************************************/
TVFE_Status_e TVFE_AudioSetLevelAdjust(
	TVFE_AudioLevelAdjust_st *pLevelAdjust_config)
{
	TVFE_Status_e ret = TVFE_SUCCESS;

	if(pLevelAdjust_config == DTV_NULL)
	{
		return TVFE_ERR_FAILURE;
	}

	_TVFE_AudioCheckValueRange(pLevelAdjust_config->bDECAdjustLevel, \
		TVFE_AUDIO_LEVEL_ADJUST_VALUE_MIN,TVFE_AUDIO_LEVEL_ADJUST_VALUE_MAX);
	_TVFE_AudioCheckValueRange(pLevelAdjust_config->bMonoAdjustLevel, \
		TVFE_AUDIO_LEVEL_ADJUST_VALUE_MIN,TVFE_AUDIO_LEVEL_ADJUST_VALUE_MAX);
	_TVFE_AudioCheckValueRange(pLevelAdjust_config->bNICAMAdjustLevel, \
		TVFE_AUDIO_LEVEL_ADJUST_VALUE_MIN,TVFE_AUDIO_LEVEL_ADJUST_VALUE_MAX);
	_TVFE_AudioCheckValueRange(pLevelAdjust_config->bSAPAdjustLevel, \
		TVFE_AUDIO_LEVEL_ADJUST_VALUE_MIN,TVFE_AUDIO_LEVEL_ADJUST_VALUE_MAX);
	_TVFE_AudioCheckValueRange(pLevelAdjust_config->bADCAdjustLevel, \
		TVFE_AUDIO_LEVEL_ADJUST_VALUE_MIN,TVFE_AUDIO_LEVEL_ADJUST_VALUE_MAX);
	_TVFE_AudioCheckValueRange(pLevelAdjust_config->bDigitalAdjustLevel, \
		TVFE_AUDIO_LEVEL_ADJUST_VALUE_MIN,TVFE_AUDIO_LEVEL_ADJUST_VALUE_MAX);

	DRV_AudSetPathAdjustLevel(pLevelAdjust_config->bDECAdjustLevel,
	                                pLevelAdjust_config->bMonoAdjustLevel,
	                                pLevelAdjust_config->bNICAMAdjustLevel,
	                                pLevelAdjust_config->bSAPAdjustLevel,
	                                pLevelAdjust_config->bADCAdjustLevel,
	                                pLevelAdjust_config->bDigitalAdjustLevel);
	return ret;
}


/*****************************************************************************
** FUNCTION : TVFE_AudioSetSIFStandardMask
**
** DESCRIPTION :
**				Configure SIF detect standard mask
**
** PARAMETERS :
**				eStandardMask :
**				Specify standard mask for demodulater and decoder to recognize
**
** RETURN VALUES:
**				TVFE_SUCCESS if successful
**				TVFE_ERR_INVALID_PARAM if parameters is unknown or out of range
**
*****************************************************************************/
TVFE_Status_e TVFE_AudioSetSIFStandardMask(	TVFE_AudioSIFStandardMask_e eStandardMask)
{
#if 1
	TVFE_Status_e ret = TVFE_SUCCESS;
	DRV_AudAutoStandardGroup_t StandardMask = 0;
	UINT8 index = 0;
	UINT8 mapping_table[][2] =
	{
		{TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_BG,   DRV_AUD_AUTO_BG_GROUP	},
		{TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_DK,   DRV_AUD_AUTO_DK_GROUP    },
		{TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_L,    DRV_AUD_AUTO_L_GROUP    	},
		{TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_I,    DRV_AUD_AUTO_I_GROUP   	},
		{TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_M,    DRV_AUD_AUTO_M_GROUP	},
	};

	if(eStandardMask <= TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_ALL)
	{
		for(index = 0; index < (sizeof(mapping_table)/2) ; index++)
		{
			if((eStandardMask & (mapping_table[index][0])) != 0)
			{
				StandardMask |= (mapping_table[index][1]);
			}
		}

		DRV_AUDIO_DBG("StandardMask=%x\n",StandardMask);

		DRV_AudSetAutoStandardSelection(StandardMask);
	}
	else
	{
		switch(eStandardMask)
		{
			case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_BG:
				DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_BG);
				break;

			case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_DK:
				DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_DK);
				break;

			case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_M:
				DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_M);
				break;

			case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_BG_A2:
				DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_BG_A2);
				break;

			case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_BG_NICAM:
				DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_BG_NICAM);
				break;

			case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_DK_A2_1:
				DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_DK_A2_1);
				break;

			case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_DK_A2_2:
				DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_DK_A2_2);
				break;

			case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_DK_A2_3:
				DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_DK_A2_3);
				break;

			case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_DK_NICAM:
				DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_DK_NICAM);
				break;

			case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_L_NICAM:
				DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_L_NICAM);
				break;

			case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_I_NICAM:
				DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_I_NICAM);
				break;

			case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_M_KOREA:
				DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_M_KOREA);
				break;

			case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_M_BTSC:
				DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_M_BTSC);
				break;

			case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_M_EIAJ:
				DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_M_EIAJ);
				break;

			case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_FM_10_7_IF_50_US:
				DRV_AudSetStaticStandardSelection( \
					DRV_AUD_STATIC_FM_RADIO_10_7_MHZ_50_US_DEMPHASIS);
				break;

			case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_FM_10_7_IF_75_US:
				DRV_AudSetStaticStandardSelection( \
					DRV_AUD_STATIC_FM_RADIO_10_7_MHZ_75_US_DEMPHASIS);
				break;

			case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_FM_SELECTABLE_IF_50_US:
				DRV_AudSetStaticStandardSelection( \
					DRV_AUD_STATIC_FM_RADIO_SELECTABLE_50_US_DEMPHASIS);
				break;

			case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_FM_SELECTABLE_IF_75_US:
				DRV_AudSetStaticStandardSelection( \
					DRV_AUD_STATIC_FM_RADIO_SELECTABLE_75_US_DEMPHASIS);
				break;

			default :
				ret = TVFE_ERR_INVALID_PARAM;
				break;
		}
	}
	return ret;
#else
    TVFE_Status_e ret = TVFE_SUCCESS;

    switch(eStandardMask)
    {
        case TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_BG:
            DRV_AudSetAutoStandardSelection(DRV_AUD_AUTO_BG_GROUP);
            break;

        case TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_DK:
            DRV_AudSetAutoStandardSelection(DRV_AUD_AUTO_DK_GROUP);
            break;

        case TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_L:
            DRV_AudSetAutoStandardSelection(DRV_AUD_AUTO_L_GROUP);
            break;

        case TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_I:
            DRV_AudSetAutoStandardSelection(DRV_AUD_AUTO_I_GROUP);
            break;

        case TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_M:
            DRV_AudSetAutoStandardSelection(DRV_AUD_AUTO_M_GROUP);
            break;

        case TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_BGDKI:
//            DRV_AudSetAutoStandardSelection(DRV_AUD_AUTO_BGDKI_GROUP);
			DRV_AudSetAutoStandardSelection(DRV_AUD_AUTO_BG_GROUP|DRV_AUD_AUTO_DK_GROUP|DRV_AUD_AUTO_I_GROUP);
            break;

        case TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_BGDKIM:
//            DRV_AudSetAutoStandardSelection(DRV_AUD_AUTO_BGDKIM_GROUP);
			DRV_AudSetAutoStandardSelection(DRV_AUD_AUTO_BG_GROUP|DRV_AUD_AUTO_DK_GROUP|DRV_AUD_AUTO_I_GROUP|DRV_AUD_AUTO_M_GROUP);
            break;

        case TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_ALL:
//            DRV_AudSetAutoStandardSelection(DRV_AUD_AUTO_ALL_GROUP);
            DRV_AudSetAutoStandardSelection(DRV_AUD_AUTO_BG_GROUP|DRV_AUD_AUTO_DK_GROUP|DRV_AUD_AUTO_L_GROUP|DRV_AUD_AUTO_I_GROUP|DRV_AUD_AUTO_M_GROUP);
            break;

		case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_BG:
			DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_BG);
            break;

		case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_DK:
			DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_DK);
            break;

		case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_M:
			DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_M);
            break;

	    case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_BG_A2:
			DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_BG_A2);
			break;

	    case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_BG_NICAM:
			DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_BG_NICAM);
			break;

	    case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_DK_A2_1:
			DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_DK_A2_1);
			break;

	    case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_DK_A2_2:
			DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_DK_A2_2);
			break;

	    case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_DK_A2_3:
			DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_DK_A2_3);
			break;

	    case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_DK_NICAM:
			DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_DK_NICAM);
			break;

	    case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_L_NICAM:
			DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_L_NICAM);
			break;

	    case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_I_NICAM:
			DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_I_NICAM);
			break;

	    case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_M_KOREA:
			DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_M_KOREA);
			break;

	    case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_M_BTSC:
			DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_M_BTSC);
			break;

	    case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_M_EIAJ:
			DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_M_EIAJ);
			break;

	    case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_FM_10_7_IF_50_US:
			DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_FM_RADIO_10_7_MHZ_50_US_DEMPHASIS);
			break;

	    case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_FM_10_7_IF_75_US:
			DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_FM_RADIO_10_7_MHZ_75_US_DEMPHASIS);
			break;

	    case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_FM_SELECTABLE_IF_50_US:
			DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_FM_RADIO_SELECTABLE_50_US_DEMPHASIS);
			break;

	    case TVFE_AUDIO_SIF_STANDARD_SSS_MODE_FM_SELECTABLE_IF_75_US:
			DRV_AudSetStaticStandardSelection(DRV_AUD_STATIC_FM_RADIO_SELECTABLE_75_US_DEMPHASIS);
			break;

        default :
            ret = TVFE_ERR_INVALID_PARAM;
            break;
    }

    return ret;
#endif
}


/*****************************************************************************
** FUNCTION : TVFE_AudioSetSIFDetectReset
**
** DESCRIPTION :
**				Set SIF detect standard
**
** PARAMETERS :
**
** RETURN VALUES:
**				TVFE_SUCCESS if successful
**				TVFE_WARN_NO_ACTION
**						if hardware is busy
**						or no front end source connect to SIF input
**
*****************************************************************************/
TVFE_Status_e TVFE_AudioSetSIFDetectReset(void)
{
/* with task active */
	TVFE_Status_e ret = TVFE_SUCCESS;
		TVFE_AUDIO_DBG("SIFDetectReset1111\n");
	if(gAudioPrimaryParam.Status.eStandard != TVFE_AUDIO_SIF_STANDARD_BUSY)
	{
		/* avoid repeat reset when audio manager is busy to get standard */
		DRV_AudSetDetectStandard(DRV_AUD_DEMDEC_RESET_STAR);

		/*resource protection begin*/
		gAudioPrimaryParam.Status.eStandard = TVFE_AUDIO_SIF_STANDARD_BUSY;
		gAudioPrimaryParam.Status.eMode= TVFE_AUDIO_SIF_AVAIL_BUSY;
		/*resource protection end*/

		/*resource protection begin*/
		gAudioPrimaryParam.eState = AUDIO_MANAGER_CONNECT_INITIAL;
		/*resource protection end*/
		/*initial handler state*/

		DRV_AudSetDetectStandard(DRV_AUD_DEMDEC_RESET_RELEASE);

		TVFE_AUDIO_DBG("SIFDetectReset2222\n");
		msleep(500);
		
	}
	else
	{
		ret = TVFE_WARN_NO_ACTION;
	}


	return ret;

}


/*****************************************************************************
** FUNCTION : TVFE_AudioSetSIFConnect
**
** DESCRIPTION :
**				Set SIF demodulator to connect and demodulate/decode input SIF signal
**
** PARAMETERS :
**				eStandardMask : Specify which standard mask for demodulater
**					and decoder to recognize, you can get SIF standard and mode
**            			once the standard report is not TVFE_AUDIO_SIF_STANDARD_BUSY
**
** RETURN VALUES:
**				TVFE_SUCCESS
**				TVFE_ERR_INVALID_PARAM
**				TVFE_ERR_FAILURE
*****************************************************************************/
TVFE_Status_e TVFE_AudioSetSIFConnect(TVFE_AudioSIFStandardMask_e eStandardMask)
{
	TVFE_Status_e ret = TVFE_SUCCESS;

	/*reset NICAM rottate angle to default value*/
	DRV_AudSetINICAMRotateAngle(DRV_AUD_NICAM_QPSK_ROTATE_0_DEGREE);

	/*set standard mask and re-detect*/
	TVFE_AudioSetSIFStandardMask(eStandardMask);
	TVFE_AudioSetSIFDetectReset();
	return ret;
}

/***********************
* part of tvfe_tvdec.c
************************/
#define AUDIO_MANAGER_CONNECTING_COUNTER    (3)

static UINT32 g_u32SIFConnectTimer = 0;

TVFE_Status_e TVFE_SSDInit(void)
{
    /* enable ssd rom chip*/
    DRV_AudEnableSSDROMReg(DTV_TRUE);
	DRV_AudInit();
	return TVFE_SUCCESS;
}

TVFE_Status_e TVFE_SSDUninit(void)
{
    /* enable ssd rom chip*/
    DRV_AudEnableSSDROMReg(DTV_FALSE);
	return TVFE_SUCCESS;
}


/******************************************************************************/
/**
* @brief    Get SIF standard
* @param    pRegStatus : Registers report from hardware
* @param    pRetStandrad : Report standard according "pRegStatus"
* @return   TVFE_SUCCESS if successful
*			TVFE_ERR_INVALID_PARAM if parameters is unknown
*           TVFE_ERR_FAILURE if pointer is DTV_NULL
******************************************************************************/
TVFE_Status_e _tvfe_AudioGetSIFStandard(DRV_AudFrontEndReg_t *pRegStatus,
                                    TVFE_AudioSIFStandard_t *pRetStandrad)
{
	if((pRegStatus == DTV_NULL)
        ||(pRetStandrad == DTV_NULL))
    {
		return TVFE_ERR_FAILURE;
    }

    switch(pRegStatus->INF_DEV_STA_REG.STDRES)
    {
        case DRV_AUD_STANDARD_FAIL:
		case DRV_AUD_STANDARD_BUSY:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_FAIL;
            break;

        case DRV_AUD_STANDARD_BG:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_BG;
            break;

        case DRV_AUD_STANDARD_DK:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_DK;
            break;

        case DRV_AUD_STANDARD_M:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_M;
            break;

        case DRV_AUD_STANDARD_BG_A2:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_BG_A2;
            break;

        case DRV_AUD_STANDARD_BG_NICAM:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_BG_NICAM;
            break;

        case DRV_AUD_STANDARD_DK_A2_1:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_DK_A2_1;
            break;

        case DRV_AUD_STANDARD_DK_A2_2:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_DK_A2_2;
            break;

        case DRV_AUD_STANDARD_DK_A2_3:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_DK_A2_3;
            break;

        case DRV_AUD_STANDARD_DK_NICAM:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_DK_NICAM;
            break;

        case DRV_AUD_STANDARD_L_NICAM:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_L_NICAM;
            break;

        case DRV_AUD_STANDARD_I_NICAM:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_I_NICAM;
            break;

        case DRV_AUD_STANDARD_M_KOREA:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_M_KOREA;
            break;

        case DRV_AUD_STANDARD_M_BTSC:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_M_BTSC;
            break;

        case DRV_AUD_STANDARD_M_EIAJ:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_M_EIAJ;
            break;

        case DRV_AUD_STANDARD_FM_10_7_IF_50_US:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_FM_10_7_IF_50_US;
            break;

        case DRV_AUD_STANDARD_FM_10_7_IF_75_US:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_FM_10_7_IF_75_US;
            break;

        case DRV_AUD_STANDARD_FM_SELECTABLE_IF_50_US:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_FM_SELECTABLE_IF_50_US;
            break;

        case DRV_AUD_STANDARD_FM_SELECTABLE_IF_75_US:
            *pRetStandrad = TVFE_AUDIO_SIF_STANDARD_FM_SELECTABLE_IF_75_US;
            break;
		default:
			return TVFE_ERR_INVALID_PARAM;
			break;
    }
	return TVFE_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Get SIF mode detect status
* @param    pRegStatus : Registers report from hardware
* @param    pRetMode : Report standard according "pRegStatus"
* @return   TVFE_SUCCESS if successful
*           TVFE_ERR_FAILURE if pointer is DTV_NULL
******************************************************************************/
TVFE_Status_e _tvfe_AudioGetSIFModeStatus(DRV_AudFrontEndReg_t *pRegStatus,
                                        TVFE_AudioSIFAvailableMode_e *pRetMode)
{
#if 1/*get status from variables from audio manager*/
    Boolean fsap = DTV_FALSE,fdual = DTV_FALSE,fstereo = DTV_FALSE,fnicam = DTV_FALSE;

	if(pRetMode == DTV_NULL)
    {
		return TVFE_ERR_FAILURE;
    }

    *pRetMode = 0;

    /*check SAP*/
    if(pRegStatus->INF_DEV_STA_REG.SAPDET)
    {
        if(pRegStatus->INF_DEV_STA_REG.SAMUT)
        {
            fsap=DTV_FALSE;
        }
        else
        {
            fsap=DTV_TRUE;
        }
    }
    else
    {
        fsap=DTV_FALSE;
    }

    /*check dual*/
    if(pRegStatus->INF_DEV_STA_REG.GDU)
    {
        fdual=DTV_TRUE;
    }
    else
    {
        fdual=DTV_FALSE;
    }

    /*check stereo*/
    if (pRegStatus->INF_DEV_STA_REG.STDRES == REG_SSD_STDRES_VALUE_M_BTSC)
    {
        if ((pRegStatus->INF_DEV_STA_REG.BPILOT)
            && (!pRegStatus->INF_DEV_STA_REG.BAMUT))
        {
            fstereo=DTV_TRUE;
        }
        else
        {
            fstereo=DTV_FALSE;
        }
    }
    else
    {
        if(pRegStatus->INF_DEV_STA_REG.GST)
        {
            fstereo=DTV_TRUE;
        }
        else
        {
            fstereo=DTV_FALSE;
        }
    }

    /*check NICAM*/
    if(pRegStatus->INF_DEV_STA_REG.VDSP_C)
    {
        if(pRegStatus->INF_DEV_STA_REG.NAMUT)
        {
            fnicam=DTV_FALSE;
        }
        else
        {
            fnicam=DTV_TRUE;
        }
    }
    else
    {
        fnicam=DTV_FALSE;
    }

    if (fnicam == DTV_TRUE)
    {
        *pRetMode |= TVFE_AUDIO_SIF_AVAIL_NICAM;
        *pRetMode |= TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
        if (fdual == DTV_TRUE)
        {
            *pRetMode |= TVFE_AUDIO_SIF_AVAIL_DUAL;
        }
        else if (fstereo == DTV_TRUE)
        {
            *pRetMode |= TVFE_AUDIO_SIF_AVAIL_STEREO;
        }
        else
        {
            *pRetMode |= TVFE_AUDIO_SIF_AVAIL_NICAM_MONO;
        }
    }
    else
    {
        if (fsap == DTV_TRUE)
        {
            *pRetMode |= TVFE_AUDIO_SIF_AVAIL_SAP;
            if (fstereo == DTV_TRUE)
            {
                *pRetMode |= TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
                *pRetMode |= TVFE_AUDIO_SIF_AVAIL_STEREO;
            }
            else
            {
                *pRetMode |= TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
            }
        }
        else
        {
            if (fstereo == DTV_TRUE)
            {
	        	if((pRegStatus->INF_DEV_STA_REG.STDRES == DRV_AUD_STANDARD_BG_A2)
					||(pRegStatus->INF_DEV_STA_REG.STDRES == DRV_AUD_STANDARD_DK_A2_1)
					||(pRegStatus->INF_DEV_STA_REG.STDRES == DRV_AUD_STANDARD_DK_A2_2)
					||(pRegStatus->INF_DEV_STA_REG.STDRES == DRV_AUD_STANDARD_DK_A2_3))

	        	{/*if get A2 standard, and dual or stereo is available, mono mode should be unavailable*/
	            	*pRetMode |= TVFE_AUDIO_SIF_AVAIL_STEREO;
				}
				else
				{

	            	*pRetMode |= TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
				}
            }
            else
            {
                if (fdual == DTV_TRUE)
                {
	            	if((pRegStatus->INF_DEV_STA_REG.STDRES == DRV_AUD_STANDARD_BG_A2)
						||(pRegStatus->INF_DEV_STA_REG.STDRES == DRV_AUD_STANDARD_DK_A2_1)
						||(pRegStatus->INF_DEV_STA_REG.STDRES == DRV_AUD_STANDARD_DK_A2_2)
						||(pRegStatus->INF_DEV_STA_REG.STDRES == DRV_AUD_STANDARD_DK_A2_3))

	            	{/*if get A2 standard, and dual or stereo is available, mono mode should be unavailable*/
                    	*pRetMode |= TVFE_AUDIO_SIF_AVAIL_DUAL;
					}
                	else
                	{
                    	*pRetMode |= TVFE_AUDIO_SIF_AVAIL_DUAL;
						*pRetMode |= TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
                	}
                }
				else
				{
                	*pRetMode |= TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
				}
            }
        }
    }
	return TVFE_SUCCESS;
#else/*get status via audio drivers*/
    Boolean fsap = DTV_FALSE,fdual = DTV_FALSE,fstereo = DTV_FALSE,fnicam = DTV_FALSE;

	if(pRetMode == DTV_NULL)
		return TVFE_ERR_INVALID_PARAM;

    *pRetMode = 0;

    DRV_AudGetIfSAP(&fsap);
    DRV_AudGetIfDual(&fdual);
    DRV_AudGetIfStereo(&fstereo);
    DRV_AudGetIfNICAM(&fnicam);

    if (fnicam == DTV_TRUE)
    {
        *pRetMode |= TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
        if (fdual == DTV_TRUE)
        {
            *pRetMode |= TVFE_AUDIO_SIF_AVAIL_DUAL;
        }
        else if (fstereo == DTV_TRUE)
        {
            *pRetMode |= TVFE_AUDIO_SIF_AVAIL_STEREO;
        }
        else
        {
            *pRetMode |= TVFE_AUDIO_SIF_AVAIL_NICAM_MONO;
        }
    }
    else
    {
        if (fsap == DTV_TRUE)
        {
            *pRetMode |= TVFE_AUDIO_SIF_AVAIL_SAP;
            if (fstereo == DTV_TRUE)
            {
                *pRetMode |= TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
                *pRetMode |= TVFE_AUDIO_SIF_AVAIL_STEREO;
            }
            else
            {
                *pRetMode |= TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
            }
        }
        else
        {
            if (fstereo == DTV_TRUE)
            {
                *pRetMode |= TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
                *pRetMode |= TVFE_AUDIO_SIF_AVAIL_STEREO;
            }
            else
            {
                *pRetMode |= TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
                if (fdual == DTV_TRUE)
                {
                    *pRetMode |= TVFE_AUDIO_SIF_AVAIL_DUAL;
                }
            }
        }
    }
	return TVFE_SUCCESS;
#endif
}

/******************************************************************************/
/**
* @brief    Get SIF front end status
* @param    pFrontEndParam : Report front end status according assigned report method
* @return   TVFE_SUCCESS if successful
*           TVFE_ERR_FAILURE if pointer is DTV_NULL
*			TVFE_WARN_NO_ACTION if no front end source is not connected to SIF signal
******************************************************************************/
TVFE_Status_e TVFE_AudioGetFrontEndParam(TVFE_AudioFrontEndParam_t *pFrontEndParam)
{
    TVFE_Status_e pl_ret = TVFE_SUCCESS;

	if(pFrontEndParam == DTV_NULL)
    {
		return TVFE_ERR_FAILURE;
    }

	/*get status from register immediately, for channel scanning*/
	if(pFrontEndParam->eMethod == TVFE_AUDIO_REPORT_DIRECT)
	{//get register value from registers
	    DRV_AudGetFrontEndRegInfo(&(gAudioPrimaryParam.Status.RegInfo));
	    _tvfe_AudioGetSIFStandard(&(gAudioPrimaryParam.Status.RegInfo),&(gAudioPrimaryParam.Status.eStandard));
	    _tvfe_AudioGetSIFModeStatus(&(gAudioPrimaryParam.Status.RegInfo),&(gAudioPrimaryParam.Status.eMode));
        DRV_AudGetSIFAGCGain(&(gAudioPrimaryParam.Status.bSIFAGCGain));
		pFrontEndParam->eRetStandard = gAudioPrimaryParam.Status.eStandard;
		pFrontEndParam->eRetMode = gAudioPrimaryParam.Status.eMode;
        pFrontEndParam->bRetSIFAGCGain = gAudioPrimaryParam.Status.bSIFAGCGain;
		pFrontEndParam->bNICAMError = gAudioPrimaryParam.Status.bNICAMError;

		TVFE_AUDIO_DBG("tvfe.eStandard=%x\n",gAudioPrimaryParam.Status.eStandard);
		TVFE_AUDIO_DBG("tvfe.eMode=%x\n",gAudioPrimaryParam.Status.eMode);
		TVFE_AUDIO_DBG("tvfe.bSIFAGCGain=%x\n",gAudioPrimaryParam.Status.bSIFAGCGain);

		
	}
	else
	{//get status after audio manager debounce
        if((gAudioPrimaryParam.eState == AUDIO_MANAGER_CONNECT_READY)
            ||(gAudioPrimaryParam.eState == AUDIO_MANAGER_CONNECT_TIME_OUT))
        {
            pFrontEndParam->eRetStandard = gAudioPrimaryParam.Status.eStandard;
            pFrontEndParam->bRetSIFAGCGain = gAudioPrimaryParam.Status.bSIFAGCGain;
			pFrontEndParam->bNICAMError = gAudioPrimaryParam.Status.bNICAMError;
            if(gAudioPrimaryParam.fBTSCPilotLevelOverflow == DTV_TRUE)
        	{
                pFrontEndParam->eRetMode = (gAudioPrimaryParam.Status.eMode|TVFE_AUDIO_SIF_AVAIL_BTSC_PILOT_OVERFLOW);
        	}
        	else
            {
                pFrontEndParam->eRetMode = gAudioPrimaryParam.Status.eMode;
    		}
        }
        else
        {
            pFrontEndParam->eRetStandard = TVFE_AUDIO_SIF_STANDARD_BUSY;
	        pFrontEndParam->eRetMode = TVFE_AUDIO_SIF_AVAIL_BUSY;
        }
    }
    return pl_ret;
}


#define INICAM_PROCESS_NICAM_ERROR_THRESHOLD    0xe0
#define INICAM_PROCESS_COUNTER_MAX              10

TVFE_Status_e _AM_SIF_INICAMProcess(AudioManagerParam_t *pParam, Boolean *pfFinish)
{
    //1. I NICAM workaround
    TVFE_Status_e pl_ret = TVFE_SUCCESS;
    static DRV_AudNICAMRotateAngle_t eRotateAngle = DRV_AUD_NICAM_QPSK_ROTATE_0_DEGREE;
    static UINT8 average_counter = 0;
    static UINT32 ErrorSum = 0,PreErrorAverage = 0, ErrorAverage = 0;

    if(pParam == DTV_NULL)
    {
        return TVFE_ERR_FAILURE;
    }

    ////TVFE_AUDIO_DBG("_AM_SIFINICAMProcess\n");

    //diag_printf("NICAMError:%d\n",pParam->Status.bNICAMError);

#if 1
    if(eRotateAngle < DRV_AUD_NICAM_QPSK_ROTATE_MAX)
    {
        if(average_counter < INICAM_PROCESS_COUNTER_MAX)
        {//summation and add counter
            average_counter++;
            ErrorSum += pParam->Status.bNICAMError;
        }
        else
        {//reset variable and calculate average
            ErrorAverage = ErrorSum/average_counter;
            average_counter = 0;
            ErrorSum = 0;

            /*decide to rotate angle*/
            if(ErrorAverage > PreErrorAverage)
            {
                /*rotate the angle*/
                eRotateAngle++;
                DRV_AudSetINICAMRotateAngle(eRotateAngle);
                PreErrorAverage = ErrorAverage;
            }
            else
            {
                *pfFinish = DTV_TRUE;

                /*reset variable*/
                average_counter = 0;
                ErrorSum = 0;
                ErrorAverage = 0;
                PreErrorAverage = 0;
                eRotateAngle = DRV_AUD_NICAM_QPSK_ROTATE_0_DEGREE;
            }
        }
    }
    else
    {
        /*reset to default*/
        eRotateAngle = DRV_AUD_NICAM_QPSK_ROTATE_0_DEGREE;
        DRV_AudSetINICAMRotateAngle(eRotateAngle);
        *pfFinish = DTV_TRUE;
    }
#else
    if(!(pParam->pFrontEndInfo->Status.eMode & TVFE_AUDIO_SIF_AVAIL_NICAM)
        &&(pParam->pFrontEndInfo->Status.bNICAMError >= INICAM_PROCESS_NICAM_ERROR_THRESHOLD))
    {
        if(eRotateAngle < DRV_AUD_NICAM_QPSK_ROTATE_MAX)
        {
            /*rotate the angle*/
            DRV_AudSetINICAMRotateAngle(eRotateAngle++);
        }
        else
        {
            /*reset to default*/
            eRotateAngle = DRV_AUD_NICAM_QPSK_ROTATE_0_DEGREE;
            DRV_AudSetINICAMRotateAngle(eRotateAngle);
            *pfFinish = DTV_TRUE;
        }
    }
    else
    {
        eRotateAngle = DRV_AUD_NICAM_QPSK_ROTATE_0_DEGREE;
        *pfFinish = DTV_TRUE;
    }
#endif

    if(*pfFinish == DTV_TRUE)
    {
        /*reset debounce counter*/
        pParam->bDebounceCounter = AUDIO_MANAGER_CONNECTING_COUNTER;
    }

    return pl_ret;
}

#define BTSC_PROCESS_MPX_LEVEL_THRESHOLD    (0xe000)
#define BTSC_PROCESS_MPX_LEVEL_ADJUST       (BTSC_PROCESS_MPX_LEVEL_THRESHOLD-2)
#define BTSC_PROCESS_MPX_LEVEL_UPPER_VALID  (0x3ff00)
#define BTSC_PROCESS_MPX_LEVEL_MAX          (0x1ffff)
#define BTSC_PROCESS_MPX_LEVEL_MSB          (0x20000)

TVFE_Status_e _AM_SIF_BTSCProcess(AudioManagerParam_t *pParam, Boolean *pfFinish)
{
    //1. BTSC workaround
    TVFE_Status_e pl_ret = TVFE_SUCCESS;
    static UINT8 average_counter = 0;
    static UINT32 dMPX_level_average = BTSC_PROCESS_MPX_LEVEL_ADJUST,
                    dMPX_level_sum = 0;

    if(pParam == DTV_NULL)
    {
        return TVFE_ERR_FAILURE;
    }

    ////TVFE_AUDIO_DBG("_AM_SIFBTSCProcess before summation MPXPLEV=%x\n,BPILOT=%x dMPX_level_sum=%x,average_counter=%d\n",pParam->pFrontEndInfo->Status.RegInfo.INF_MPX_LEVEL_REG.MPXPLEV,pParam->pFrontEndInfo->Status.RegInfo.INF_DEV_STA_REG.BPILOT,dMPX_level_sum,average_counter);
#if 1
    if((pParam->Status.RegInfo.INF_DEV_STA_REG.BPILOT)
    	&&(pParam->Status.RegInfo.INF_MPX_LEVEL_REG.MPXPLEV < BTSC_PROCESS_MPX_LEVEL_UPPER_VALID))
    {//if pilot tone is detected and pilot level is not over upper valid value
        dMPX_level_sum += pParam->Status.RegInfo.INF_MPX_LEVEL_REG.MPXPLEV;
    }
    else
    {//if pilot tone is not detected
//        if((pParam->pFrontEndStatus->RegStatus.INF_MPX_LEVEL_REG.MPXPLEV & BTSC_PROCESS_MPX_LEVEL_MSB)
//        	&&(pParam->pFrontEndStatus->RegStatus.INF_MPX_LEVEL_REG.MPXPLEV < BTSC_PROCESS_MPX_LEVEL_UPPER_VALID))
//        {//pilot level is overflow
//		        dMPX_level_sum += BTSC_PROCESS_MPX_LEVEL_THRESHOLD;
//	      }
//	      else
//	      {//pilot level is not overflow
	          dMPX_level_sum += BTSC_PROCESS_MPX_LEVEL_ADJUST;
//	      }
    }
#else
    if(((pParam->pFrontEndStatus->RegStatus.INF_MPX_LEVEL_REG.MPXPLEV & BTSC_PROCESS_MPX_LEVEL_MSB) == 0)
        &&(pParam->pFrontEndStatus->RegStatus.INF_MPX_LEVEL_REG.MPXPLEV >= BTSC_PROCESS_MPX_LEVEL_THRESHOLD)
        &&(pParam->pFrontEndStatus->RegStatus.INF_DEV_STA_REG.BPILOT))
    {//if MPX level is not overflow && MPX level is too large && pilot tone is detected
        dMPX_level_sum += pParam->pFrontEndStatus->RegStatus.INF_MPX_LEVEL_REG.MPXPLEV;
    }
    else if (pParam->pFrontEndStatus->RegStatus.INF_MPX_LEVEL_REG.MPXPLEV & BTSC_PROCESS_MPX_LEVEL_MSB)
    {//if MPX level is overflow
        dMPX_level_sum += pParam->pFrontEndStatus->RegStatus.INF_MPX_LEVEL_REG.MPXPLEV;
    }
    else
    {
        dMPX_level_sum += BTSC_PROCESS_MPX_LEVEL_ADJUST;
    }
#endif

    /*add average counter*/
    average_counter++;
    ////TVFE_AUDIO_DBG("_AM_SIFBTSCProcess after summation MPXPLEV=%x\n,BPILOT=%x dMPX_level_sum=%x,average_counter=%d\n",pParam->pFrontEndInfo->Status.RegInfo.INF_MPX_LEVEL_REG.MPXPLEV,pParam->pFrontEndInfo->Status.RegInfo.INF_DEV_STA_REG.BPILOT,dMPX_level_sum,average_counter);

    switch(pParam->bDebounceCounter)
    {
        case AUDIO_MANAGER_CONNECTING_COUNTER://first time enter process
            /*initial average value*/
            dMPX_level_average = BTSC_PROCESS_MPX_LEVEL_ADJUST;

            /*initial overflow flag*/
            pParam->fBTSCPilotLevelOverflow = DTV_FALSE;
            break;

        case (AUDIO_MANAGER_CONNECTING_COUNTER-1)://second time enter process
            dMPX_level_average = dMPX_level_sum/average_counter;//average MPX stereo pilot level
            ////TVFE_AUDIO_DBG("_AM_SIFBTSCProcess dMPX_level_sum=%x,dMPX_level_average=%x\n",dMPX_level_sum,dMPX_level_average);
            if(dMPX_level_average >= BTSC_PROCESS_MPX_LEVEL_THRESHOLD)
            {
                TVFE_AudioSetSIFConnect(TVFE_AUDIO_SIF_STANDARD_SSS_MODE_M);
                ////TVFE_AUDIO_DBG("_AM_SIFBTSCProcess force standard mask to TVFE_AUDIO_SIF_STANDARD_SSS_MODE_M\n");
                pParam->fBTSCPilotLevelOverflow = DTV_TRUE;
            }

            /*update finish flag*/
            *pfFinish = DTV_TRUE;
            break;

        default :
            break;
    }

    if(*pfFinish == DTV_TRUE)
    {
        /*reset debounce counter*/
        pParam->bDebounceCounter = AUDIO_MANAGER_CONNECTING_COUNTER;

        /*reset MPX level summation*/
        dMPX_level_sum = 0;
        average_counter = 0;

    }

    return pl_ret;
}


TVFE_Status_e _AM_SIFHandler(void *pParam)
{
//1. reset process(switch channel, signal recovery)
//2. debounce standard and channel mode status
//3. AGC control
//4. post stable event after reset process
//5. MPX stereo bug workaround
//6. NICAM error rate check with rotate frequency process
//7. dynamic finetune detect level according noise report???
    TVFE_Status_e pl_ret = TVFE_SUCCESS;
    AudioManagerParam_t *pHandlerParam;
    static TVFE_AudioFrontEndStatus_t PreFrontEndInfo;
    static Boolean fIfProcessDone = DTV_FALSE,fIfFMTypeAGCConfig = DTV_FALSE;
    static UINT8 bTimeOutCounter = AUDIO_MANAGER_CONNECTING_COUNTER;

//    //TVFE_AUDIO_DBG("_AM_SIFHandler\n");

    if(pParam == DTV_NULL)
    {
        return TVFE_ERR_FAILURE;
    }


    pHandlerParam = (AudioManagerParam_t *)pParam;

    //GL_MutexLock((pHandlerParam->mutex));/*resource protection begin*/

	TVFE_AUDIO_DBG("AM_SIFHandler state=%x\n",pHandlerParam->eState);

    switch(pHandlerParam->eState)
    {
        case AUDIO_MANAGER_SHUT_DOWN:   
        case AUDIO_MANAGER_CONNECT_INITIAL:
            fIfProcessDone = DTV_FALSE;

            pHandlerParam->bDebounceCounter = AUDIO_MANAGER_CONNECTING_COUNTER;
            pHandlerParam->eState = AUDIO_MANAGER_CONNECTING;
			//g_u8SifAvailableMode = TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO;
            bTimeOutCounter = AUDIO_MANAGER_CONNECTING_COUNTER;
            DRV_AudSetINICAMRotateAngle(DRV_AUD_NICAM_QPSK_ROTATE_0_DEGREE);
            break;

        case AUDIO_MANAGER_CONNECTING:
			/*config AGC*/
			TVFE_AUDIO_DBG("PreFrontEndInfo.eStandard=%d\n",PreFrontEndInfo.eStandard);
			TVFE_AUDIO_DBG("pHandlerParam->pFrontEndStatus->eStandard=%d\n",pHandlerParam->Status.eStandard);

            if((PreFrontEndInfo.eStandard == TVFE_AUDIO_SIF_STANDARD_L_NICAM)
                &&(pHandlerParam->Status.eStandard == TVFE_AUDIO_SIF_STANDARD_L_NICAM))
            {
            	if(fIfFMTypeAGCConfig == DTV_TRUE)
				{//For AM standard, turn off AGC if AGC is active
                    DRV_AudSetSIFAGCConfig(pHandlerParam->Control.AM_AGCConfigParam.fEn,\
                                                            pHandlerParam->Control.AM_AGCConfigParam.fHysteresisFastMode,\
                                                            pHandlerParam->Control.AM_AGCConfigParam.bGain);
					fIfFMTypeAGCConfig = DTV_FALSE;
				}
            }
            else
            {
            	if(fIfFMTypeAGCConfig == DTV_FALSE)
				{//For FM standard, turn on AGC if AGC is de-active
	                    DRV_AudSetSIFAGCConfig(pHandlerParam->Control.FM_AGCConfigParam.fEn,\
	                                                            pHandlerParam->Control.FM_AGCConfigParam.fHysteresisFastMode,\
	                                                            pHandlerParam->Control.FM_AGCConfigParam.bGain);
					fIfFMTypeAGCConfig = DTV_TRUE;
				}
            }

            if(fIfProcessDone == DTV_FALSE)
            {
                if((pHandlerParam->Status.eStandard == PreFrontEndInfo.eStandard)
                    &&(pHandlerParam->Status.eMode == PreFrontEndInfo.eMode)
                    &&(pHandlerParam->Status.eStandard != TVFE_AUDIO_SIF_STANDARD_FAIL))
                {//if stable, counter count down
                    TVFE_AUDIO_DBG("pHandlerParam->pFrontEndStatus->eStandard=%d\n",pHandlerParam->Status.eStandard);
                    if(pHandlerParam->Status.eStandard == TVFE_AUDIO_SIF_STANDARD_I_NICAM)
                    {
                        _AM_SIF_INICAMProcess(pHandlerParam,&fIfProcessDone);

                    }
                    else if(pHandlerParam->Status.eStandard == TVFE_AUDIO_SIF_STANDARD_M_BTSC)
                    {
                        _AM_SIF_BTSCProcess(pHandlerParam,&fIfProcessDone);
                    }
                    else
                    {
                        fIfProcessDone = DTV_TRUE;
                    }

                    /*minus counter after NICAM/BTSC process*/
                    pHandlerParam->bDebounceCounter --;
                }
                else
                {//if signal different, counter reset and update status
                    TVFE_AUDIO_DBG("AUDIO_MANAGER_CONNECTING reset\n");
                    pHandlerParam->bDebounceCounter = AUDIO_MANAGER_CONNECTING_COUNTER;

                    if(g_u32SIFConnectTimer>=2)
                    {
                        pHandlerParam->eState = AUDIO_MANAGER_CONNECT_TIME_OUT;
                        fIfProcessDone = DTV_TRUE;
                    }		
                    else if((pHandlerParam->Status.eStandard == TVFE_AUDIO_SIF_STANDARD_FAIL)&&(g_u32SIFConnectTimer<2))
                    {
    	                TVFE_SoundStd_e eSoundStdType = TVFE_SOUND_STANDARD_MAX;
    	                TVFE_GetTunerValueType(&eSoundStdType);
                        if((eSoundStdType == TVFE_SOUND_STANDARD_L)
                                ||(eSoundStdType == TVFE_SOUND_STANDARD_L1))
                        {
                                TVFE_AudioSetSIFConnect(TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_L);
                        }
                        else
                        {
                                if(eSoundStdType == TVFE_SOUND_STANDARD_BG)
                                {
                                        TVFE_AudioSetSIFConnect( \
                                                TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_BG);
                                }
                                else if(eSoundStdType == TVFE_SOUND_STANDARD_DK)
                                {
                                        TVFE_AudioSetSIFConnect( \
                                                TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_DK);
                                }
                                else if(eSoundStdType == TVFE_SOUND_STANDARD_I)
                                {
                                        TVFE_AudioSetSIFConnect( \
                                                TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_I);
                                }
                                else
                                {
                                        TVFE_AudioSetSIFConnect( \
                                                TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_BGDKI);
                                }
                        }
                        g_u32SIFConnectTimer++;
                    }

                }
            }
            else
            {
                if((pHandlerParam->Status.eStandard == PreFrontEndInfo.eStandard)
                    &&(pHandlerParam->Status.eMode == PreFrontEndInfo.eMode))
                {//if signal stable, counter count down
                    if(pHandlerParam->bDebounceCounter -- == 0)
                    {
                        pHandlerParam->eState = AUDIO_MANAGER_CONNECT_READY;
                        pHandlerParam->bDebounceCounter = AUDIO_MANAGER_CONNECTING_COUNTER;

                    }
                }
                else
                {//if signal different, counter reset and update status
                    pHandlerParam->bDebounceCounter = AUDIO_MANAGER_CONNECTING_COUNTER;
                }

                /*time out process*/
                if((bTimeOutCounter-- == 0)
                    &&(pHandlerParam->bDebounceCounter == AUDIO_MANAGER_CONNECTING_COUNTER))
                {
                    pHandlerParam->eState = AUDIO_MANAGER_CONNECT_TIME_OUT;
                    pHandlerParam->bDebounceCounter = AUDIO_MANAGER_CONNECTING_COUNTER;
                    bTimeOutCounter = AUDIO_MANAGER_CONNECTING_COUNTER;
                }
            }
            break;

        case AUDIO_MANAGER_CONNECT_READY:
        case AUDIO_MANAGER_CONNECT_TIME_OUT:
            if((PreFrontEndInfo.eMode != pHandlerParam->Status.eMode)
                &&(pHandlerParam->bDebounceCounter -- == 0))
            {
                pHandlerParam->bDebounceCounter = AUDIO_MANAGER_CONNECTING_COUNTER;
                //TVFE_AudioPostEvent(TVFE_AUDIO_EVENT_SIF_MODE_STATUS_CHANGE);
            }

            if((PreFrontEndInfo.eStandard != pHandlerParam->Status.eStandard)
                &&(pHandlerParam->bDebounceCounter -- == 0))
            {
                pHandlerParam->bDebounceCounter = AUDIO_MANAGER_CONNECTING_COUNTER;
                //TVFE_AudioPostEvent(TVFE_AUDIO_EVENT_SIF_STANDARD_CHANGE);
            }
            g_u32SIFConnectTimer=0;
            break;

        default :

            break;

    }

    /*reocrd current information*/
    memcpy(&PreFrontEndInfo,&pHandlerParam->Status,sizeof(TVFE_AudioFrontEndStatus_t));
    //GL_MutexUnlock(pHandlerParam->mutex);/*resource protection end*/

    return pl_ret;
}






/******************************************************************************/
/**
* @brief    TVFE_Audio_Set_SIF_Source
* @param    pFrontEndParam : Report front end status according assigned report method
* @return   TVFE_SUCCESS if successful
*           TVFE_ERR_FAILURE if pointer is DTV_NULL
*			TVFE_WARN_NO_ACTION if no front end source is not connected to SIF signal
******************************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_SIF_Source(void)
{
	TVFE_Audio_Status_e  ret = TVFE_AUDIO_SUCCESS;
	AUDIF_UnInit_SpdifIn();
       AUDIF_Init_SifIn();
	AUDIF_Set_SampleRate(FS_48K);
       AUDIF_Set_MainChannelOutput(AUDIF_MAIN_PATH_SIF);
	//msleep(100);
	//TVFE_AudioSetSIFConnect(TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_BG);
	//TVFE_Audio_Set_SIFHandler();
	return ret;
}

/******************************************************************************/
/**
* @brief    Get Regsiter status
* @param    pRetFrontEndStatus : Front end status report from hardware
* @return   TVFE_SUCCESS if successful
*           TVFE_ERR_FAILURE if pointer is DTV_NULL
******************************************************************************/
//static TVFE_Status_e _tvfe_AudioGetFrontEndStatus(TVFE_AudioFrontEndStatus_t *pRetFrontEndStatus)
TVFE_Status_e _tvfe_AudioGetFrontEndStatus(TVFE_AudioFrontEndStatus_t *pRetFrontEndStatus)
{

	if (pRetFrontEndStatus == DTV_NULL)
	{
		TVFE_AUDIO_DBG("_tvfe_AudioGetFrontEndStatus fail\n");
	
		return TVFE_ERR_INVALID_PARAM;
	}
			
    DRV_AudGetFrontEndRegInfo(&(pRetFrontEndStatus->RegInfo));
    _tvfe_AudioGetSIFStandard(&(pRetFrontEndStatus->RegInfo),&(pRetFrontEndStatus->eStandard));
    _tvfe_AudioGetSIFModeStatus(&(pRetFrontEndStatus->RegInfo),&(pRetFrontEndStatus->eMode));
    DRV_AudGetSIFAGCGain(&(pRetFrontEndStatus->bSIFAGCGain));
    pRetFrontEndStatus->bNICAMError = pRetFrontEndStatus->RegInfo.INF_NIC_STA_REG.ERR_OUT;
	return  TVFE_SUCCESS;
}

#if 1 // don't need this
//extern AudioManagerParam_t gAudioPrimaryParam;
/********************************************************************
Prototype: TVFE_Audio_Set_SIFHandler()
Parameters:
Returns: TVFE_Audio_Status_e
Description:Set SIF Handler
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_SIFHandler(void)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

    /*Audio manager move here by b.yang 20100826*/
    /*update frontend status*/
    //GL_MutexLock((gAudioPrimaryParam.mutex));/*resource protection begin*/ //william: do we need this
    _tvfe_AudioGetFrontEndStatus(&(gAudioPrimaryParam.Status));
    //msleep(500);
    //GL_MutexUnlock((gAudioPrimaryParam.mutex));/*resource protection end*/ //william: do we need this
    _AM_SIFHandler(&gAudioPrimaryParam);	
		
    //msleep(500);			
	
    return ret;
}
#endif


//Mid_dtv_display.c
TVFE_Audio_Status_e _set_audio_adc_source(PLF_AVPin_e eAudPin)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	AUDIF_Set_MainChannelOutput(AUDIF_MAIN_PATH_AUX);
	if (eAudPin == PLF_INPUT_PIN_AUD_ADC0)
		AUDIF_Set_AdcSourceSelect(AUDIF_ADC_SOURCE_ADC0);
	else if (eAudPin == PLF_INPUT_PIN_AUD_ADC1)
		AUDIF_Set_AdcSourceSelect(AUDIF_ADC_SOURCE_ADC1);
	else if (eAudPin == PLF_INPUT_PIN_AUD_ADC2)
		AUDIF_Set_AdcSourceSelect(AUDIF_ADC_SOURCE_ADC2);
	else if (eAudPin == PLF_INPUT_PIN_AUD_ADC3)
		AUDIF_Set_AdcSourceSelect(AUDIF_ADC_SOURCE_ADC3);
	else
		return TVFE_AUDIO_ERR_INVALID_PARAM;

	return ret;
}
/******************************************************************************/
/**
* @brief    TVFE_Audio_Set_Sound_Source
* @param   
* @return   TVFE_SUCCESS if successful
*           TVFE_ERR_FAILURE if pointer is DTV_NULL
*			TVFE_WARN_NO_ACTION if no front end source is not connected to SIF signal
******************************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Sound_Source(PLF_AVSourcePinConfig_st AVSourcePinConfig)
{
	//PLF_AVSourcePinConfig_st  AVSourcePinConfig;
	TVFE_Audio_Status_e  ret = TVFE_AUDIO_SUCCESS;
	switch (AVSourcePinConfig.eType)
	{
		case PLF_INPUT_PIN_TYPE_DTV:
		case PLF_INPUT_PIN_TYPE_STORAGE:
			AUDIF_UnInit_SpdifIn();
			AUDIF_Set_MainChannelOutput(AUDIF_MAIN_PATH_MIXER);
			break;
		case PLF_INPUT_PIN_TYPE_HDMI:
			AUDIF_Init_SpdifIn(1);
			AUDIF_Set_MainChannelOutput(AUDIF_MAIN_PATH_MIXER);
			break;

		case PLF_INPUT_PIN_TYPE_ATV_TUNER:
			AUDIF_UnInit_SpdifIn();
			AUDIF_Init_SifIn();
			AUDIF_Set_SampleRate(FS_48K);
			AUDIF_Set_MainChannelOutput(AUDIF_MAIN_PATH_SIF);
			break;
			
		case PLF_INPUT_PIN_TYPE_CVBS:
		case PLF_INPUT_PIN_TYPE_SVIDEO:
		case PLF_INPUT_PIN_TYPE_YPBPR:
		case PLF_INPUT_PIN_TYPE_RGB:
		case PLF_INPUT_PIN_TYPE_SCART_AV:
		case PLF_INPUT_PIN_TYPE_SCART_SV:
		case PLF_INPUT_PIN_TYPE_SCART_AV_RGB:
			AUDIF_UnInit_SpdifIn();
			AUDIF_Set_SampleRate(FS_48K);
			AUDIF_Set_FIFOEnable();
			if (TVFE_AUDIO_SUCCESS != _set_audio_adc_source(AVSourcePinConfig.eAudioPri))
				return TVFE_AUDIO_ERR_INVALID_PARAM;
			break;
		default:
			return TVFE_AUDIO_ERR_INVALID_PARAM;
	}
 
	return ret;
}

