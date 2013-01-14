/******************************************************************************/
/*! @addtogroup Audio
*  @{
******************************************************************************/
/******************************************************************************/
/*!
* @file    drv_audio.c
*
* @brief   This file specifies the APIs provided to control audio input device.
*
* @note     Copyright (c) 2007 Sunplus Technology Co., Ltd. \n
*          All rights reserved.
*
********************************************************************************/

/*******************************************************************************
* Header include
******************************************************************************/
#define _REGDEF_C_
#include <asm/mach-spv9200/ic-spv9200/hw_public.h>
#include <asm/mach-spv9200/regmap_9200.h>


#include "drv_ssd_resource.h"
#include "drv_ssd_access.h"
#include "drv_ssd_config.h"
#include "drv_ssd.h"
#include "hw_ssd_def.h"
/*******************************************************************************
* Constant
******************************************************************************/

/*******************************************************************************
* Structure
******************************************************************************/

/*******************************************************************************
* Variable
******************************************************************************/

/*******************************************************************************
* Program
******************************************************************************/

/******************************************************************************/
/**
* @brief    This function enable SSD ROM chip
* @param    fEn : enable SSD rom access if fEn = DTV_TRUE
*                 disable SSD for power saving if fEn = DTV_FALSE
* @return   DTV_DRV_SUCCESS if successful
*			DTV_DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudEnableSSDROMReg(Boolean fEn)

{
    if(fEn == DTV_TRUE)
    {
        HW_SetReg(reg_ssd_ssd_rom_cen, REG_SSD_SSD_ROM_CEN_V_ENABLE_SSD_ROM_ACCESS);
    }
    else
    {
        HW_SetReg(reg_ssd_ssd_rom_cen, REG_SSD_SSD_ROM_CEN_V_DISABLE_SSD_ROM_FOR_POWER_SAVING);
    }
    return DRV_SUCCESS;
}



/******************************************************************************/
/**
* @brief    This function read data from SSD with assigned address
* @param    dSSDAddr : SSD DTL Address
* @param    dSSDDataBitMask : SSD data mask
* @param    bSSDDataBitIndex : lowest bit index of SSD data (0..31)
* @param    pRetData : return SSD data
* @return   DTV_DRV_SUCCESS if successful
*			DTV_DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudReadSSDReg(UINT32 dSSDAddr,
                    UINT32 dSSDDataBitMask,
                    UINT8 bSSDDataBitIndex,
                    UINT32 *pRetData)

{
    if(pRetData == DTV_NULL)
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        *pRetData = HW_SSDGetBit(dSSDAddr,dSSDDataBitMask,bSSDDataBitIndex);
        return DRV_SUCCESS;
    }
}

/******************************************************************************/
/**
* @brief    This function read data from SSD via DTL interface
* @param    dDTLAddr : SSD DTL Address
* @param    pdRetDTLData : SSD data read from assigned address
* @param    pRetData : return SSD data
* @return   DTV_DRV_SUCCESS if successful
*			DTV_DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_SSD_DTL_Read(UINT32 dDTLAddr, UINT32 *pdRetDTLData)
{
    if(pdRetDTLData == DTV_NULL)
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
    	HW_DTL_Read(dDTLAddr, pdRetDTLData);
        return DRV_SUCCESS;
    }
}

/******************************************************************************/
/**
* @brief    This function write data from SSD via DTL interface
* @param    dDTLAddr : SSD DTL Address
* @param    dDTLData : SSD write data with assigned address
* @return   DTV_DRV_SUCCESS if successful
******************************************************************************/
DRV_Status_t DRV_SSD_DTL_Write(UINT32 dDTLAddr, UINT32 dDTLData)
{
	HW_DTL_Write(dDTLAddr, dDTLData);
    return DRV_SUCCESS;
}
#if 0 //william need to update
DRV_Status_t DRV_AudSetPLLEnable(Boolean fEn)
{
	if(fEn == DTV_TRUE)
	{//turn on
	    regs0->sft_cfg47 |= (0x01<<12);
	}
	else
	{//turn off
	    regs0->sft_cfg47 &= ~(0x01<<12);
	}
	return DRV_SUCCESS;
}

DRV_Status_t DRV_AudSetLineInADCPower(Boolean fPowerOnOff)
{
	if(fPowerOnOff == DTV_TRUE)
	{//turn on
		/*toggle bit 5 to initialize ADC*/
		regs0->sft_cfg51 &= ~(0x0000000f0);
		regs0->sft_cfg51 |= (0x000000020);
	}
	else
	{//turn off
		regs0->sft_cfg51 |= (0x0000000d0);
	}
	return DRV_SUCCESS;
}

DRV_Status_t DRV_AudSetSIFADCPower(Boolean fPowerOnOff)
{
	if(fPowerOnOff == DTV_TRUE)
	{//turn on
	    regs0->sft_cfg52 &= ~(0x1f0000);
	}
	else
	{//turn off
	    regs0->sft_cfg52 |= (0x1f0000);
	}
	return DRV_SUCCESS;
}

DRV_Status_t DRV_AudSetDACPower(Boolean fPowerOnOff)
{
	if(fPowerOnOff == DTV_TRUE)
	{//turn on
	    regs0->sft_cfg50 |= (0x0c);

		regs0->sft_cfg50 &= ~(0x0003c0003);
	}
	else
	{//turn off
		regs0->sft_cfg50 &= ~(0x0c);

		regs0->sft_cfg50 |= (0x0003c0003);
	}
	return DRV_SUCCESS;
}

DRV_Status_t DRV_AudSetDAC0Power(Boolean fPowerOnOff)
{
	if(fPowerOnOff == DTV_TRUE)
	{//turn on
	    regs0->sft_cfg50 |= (0x0c);

		regs0->sft_cfg50 &= ~(0x000000003);
	}
	else
	{//turn off
		regs0->sft_cfg50 &= ~(0x0c);

		regs0->sft_cfg50 |= (0x000000003);
	}
	return DRV_SUCCESS;
}

DRV_Status_t DRV_AudSetDAC1Power(Boolean fPowerOnOff)
{
	if(fPowerOnOff == DTV_TRUE)
	{//turn on
	    regs0->sft_cfg50 |= (0x0c);

		regs0->sft_cfg50 &= ~(0x0000c0000);
	}
	else
	{//turn off
		regs0->sft_cfg50 &= ~(0x0c);

		regs0->sft_cfg50 |= (0x0000c0000);
	}
	return DRV_SUCCESS;
}

DRV_Status_t DRV_AudSetDAC2Power(Boolean fPowerOnOff)
{
	if(fPowerOnOff == DTV_TRUE)
	{//turn on
	    regs0->sft_cfg50 |= (0x0c);

		regs0->sft_cfg50 &= ~(0x000300000);
	}
	else
	{//turn off
		regs0->sft_cfg50 &= ~(0x0c);

		regs0->sft_cfg50 |= (0x000300000);
	}
	return DRV_SUCCESS;
}
#else
DRV_Status_t DRV_AudSetPLLEnable(Boolean fEn)
{
	return DRV_SUCCESS;
}

DRV_Status_t DRV_AudSetLineInADCPower(Boolean fPowerOnOff)
{
	return DRV_SUCCESS;
}

DRV_Status_t DRV_AudSetSIFADCPower(Boolean fPowerOnOff)
{
	return DRV_SUCCESS;
}
DRV_Status_t DRV_AudSetDACVCOM(Boolean fPowerOnOff)
{
	if(fPowerOnOff == DTV_TRUE)
	{//Enable
	    HWREG_W(sft_cfg_39_dtv, (HWREG_R(sft_cfg_39_dtv) | (0x000008)));
	}
	else
	{//Disable
	    HWREG_W(sft_cfg_39_dtv, (HWREG_R(sft_cfg_39_dtv) & (~0x000008)));
	}
	return DRV_SUCCESS;
}

DRV_Status_t DRV_AudSetDACVRT(Boolean fPowerOnOff)
{
	if(fPowerOnOff == DTV_TRUE)
	{//Enable
	    HWREG_W(sft_cfg_39_dtv, (HWREG_R(sft_cfg_39_dtv) | (0x000004)));
	}
	else
	{//Disable
	    HWREG_W(sft_cfg_39_dtv, (HWREG_R(sft_cfg_39_dtv) & (~0x000004)));
	}
	return DRV_SUCCESS;
}

DRV_Status_t DRV_AudSetDACPower(Boolean fPowerOnOff)
{
	if(fPowerOnOff == DTV_TRUE)
	{//turn on
	    HWREG_W(sft_cfg_39_dtv, (HWREG_R(sft_cfg_39_dtv)& (~0x3C0003)));
	}
	else
	{//turn off
	    HWREG_W(sft_cfg_39_dtv, (HWREG_R(sft_cfg_39_dtv) | (0x3C0003)));
	}
	return DRV_SUCCESS;
}

DRV_Status_t DRV_AudSetDAC0Power(Boolean fPowerOnOff)
{
	if(fPowerOnOff == DTV_TRUE)
	{//turn on
	    HWREG_W(sft_cfg_39_dtv, (HWREG_R(sft_cfg_39_dtv)& (~0x3)));
	}
	else
	{//turn off
	    HWREG_W(sft_cfg_39_dtv, (HWREG_R(sft_cfg_39_dtv) | (0x3)));
	}
	return DRV_SUCCESS;
}

DRV_Status_t DRV_AudSetDAC1Power(Boolean fPowerOnOff)
{
	if(fPowerOnOff == DTV_TRUE)
	{//turn on
	    HWREG_W(sft_cfg_39_dtv, (HWREG_R(sft_cfg_39_dtv)& (~0xC0000)));
	}
	else
	{//turn off
	    HWREG_W(sft_cfg_39_dtv, (HWREG_R(sft_cfg_39_dtv) | (0xC0000)));
	}
	return DRV_SUCCESS;
}

DRV_Status_t DRV_AudSetDAC2Power(Boolean fPowerOnOff)
{
	if(fPowerOnOff == DTV_TRUE)
	{//turn on
	    HWREG_W(sft_cfg_39_dtv, (HWREG_R(sft_cfg_39_dtv)& (~0x300000)));
	}
	else
	{//turn off
	    HWREG_W(sft_cfg_39_dtv, (HWREG_R(sft_cfg_39_dtv) | (0x300000)));
	}
	return DRV_SUCCESS;
}

#endif
/******************************************************************************/
/**
* @brief    Set power control configuration
* @param    ePowerControl : power control parameter
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetPowerControl(DRV_AudPowerControl_t ePowerControl)
{
	switch(ePowerControl)
	{
		case DRV_AUD_SIF_ADC_POWER_ON:
			DRV_AudSetSIFADCPower(DTV_TRUE);
			break;

		case DRV_AUD_SIF_ADC_POWER_OFF:
			DRV_AudSetSIFADCPower(DTV_FALSE);
			break;

		case DRV_AUD_LINE_IN_ADC_POWER_ON:
			DRV_AudSetLineInADCPower(DTV_TRUE);
			break;

		case DRV_AUD_LINE_IN_ADC_POWER_OFF:
			DRV_AudSetLineInADCPower(DTV_FALSE);
			break;

        case DRV_AUD_DAC_VCOM_ON:
			DRV_AudSetDACVCOM(DTV_TRUE);
			break;

		case DRV_AUD_DAC_VCOM_OFF:
			DRV_AudSetDACVCOM(DTV_FALSE);
			break;

		case DRV_AUD_DAC_VRT_ON:
			DRV_AudSetDACVRT(DTV_TRUE);
			break;

		case DRV_AUD_DAC_VRT_OFF:
			DRV_AudSetDACVRT(DTV_FALSE);
			break;

		case DRV_AUD_DAC_POWER_ON:
			DRV_AudSetDACPower(DTV_TRUE);
			break;

		case DRV_AUD_DAC_POWER_OFF:
			DRV_AudSetDACPower(DTV_FALSE);
			break;


		case DRV_AUD_DAC0_POWER_ON:
			DRV_AudSetDAC0Power(DTV_TRUE);
			break;

		case DRV_AUD_DAC0_POWER_OFF:
			DRV_AudSetDAC0Power(DTV_FALSE);
			break;

		case DRV_AUD_DAC1_POWER_ON:
			DRV_AudSetDAC1Power(DTV_TRUE);
			break;

		case DRV_AUD_DAC1_POWER_OFF:
			DRV_AudSetDAC1Power(DTV_FALSE);
			break;

		case DRV_AUD_DAC2_POWER_ON:
			DRV_AudSetDAC2Power(DTV_TRUE);
			break;

		case DRV_AUD_DAC2_POWER_OFF:
			DRV_AudSetDAC2Power(DTV_FALSE);
			break;

		case DRV_AUD_ALL_POWER_ON:
#ifdef SPV9107_DEVELOP_VERSION
			DRV_AudSetPLLEnable(DTV_TRUE);
#else
			/*PLL enable procedure on SPV9100*/
			DRV_AudSetPLLEnable(DTV_FALSE);
		    GL_DelayUs(1000);
			DRV_AudSetPLLEnable(DTV_TRUE);
			DRV_AudSetPLLEnable(DTV_FALSE);
		    GL_DelayUs(1000);
			DRV_AudSetPLLEnable(DTV_TRUE);
		    GL_DelayUs(100000);
#endif
			//enable SIF ADC power, SSD need clock from SIF ADC
			DRV_AudSetSIFADCPower(DTV_TRUE);

			//enable line in ADC power
			DRV_AudSetLineInADCPower(DTV_TRUE);

			//enable DAC Vcom and VRT voltage
			DRV_AudSetDACPower(DTV_TRUE);
			break;

		case DRV_AUD_ALL_POWER_OFF:
			DRV_AudSetPLLEnable(DTV_FALSE);
			DRV_AudSetSIFADCPower(DTV_FALSE);
			DRV_AudSetLineInADCPower(DTV_FALSE);
			DRV_AudSetDACPower(DTV_FALSE);
			break;

		default:
			return DRV_ERR_INVALID_PARAM;
	}
	return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Audio driver initial function
* @param    pInitialParam : initial parameters for audio driver initial
*		        pInitialParam->LipSyncMemoryStarAddress : memory star address for lip sync
*		        pInitialParam->LipSyncMemorySize : memory size for lip sync
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudInit(void)
{
    //power up audio PLLA
	HWREG_W(sft_cfg_36_dtv, HWREG_R(sft_cfg_36_dtv)|(1<<12));
	HWREG_W(sft_cfg_41_dtv, HWREG_R(sft_cfg_41_dtv)&(~0x1f0000));
	HWREG_W(sft_cfg_56_dtv, HWREG_R(sft_cfg_56_dtv)|(1<<26));
    /*Initial SSD*/
	HW_DTL_Init();

    /* reset DTL interface & EPICS program counter */
    HW_SSDSetReg(REG_Ssd_reset_hw, REG_SSD_RESET_HW_VALUE_RESET);
    HW_SSDSetReg(REG_Ssd_pc_reset, REG_SSD_PC_RESET_VALUE_RESET);
	HW_SSDSetReg(REG_Ssd_reset_hw, REG_SSD_RESET_HW_VALUE_NO_RESET);
	HW_SSDSetReg(REG_Ssd_pc_reset, REG_SSD_PC_RESET_VALUE_NO_RESET);

    DRV_AudSetMainAUXPath(DRV_AUD_PROCESS_PATH_AUX1,DRV_AUD_MAIN_AUX_PATH_SOURCE_SRC2);
    DRV_AudSetOutputConfig(DRV_AUD_SPDIF_SSD_OUTPUT,DRV_AUD_SSD_OUTPUT_SOURCE_AUX1_L,DRV_AUD_SSD_OUTPUT_SOURCE_AUX1_R);
    DRV_AudSetMutePath(DRV_AUD_MUTE_PATH_AUX1,DTV_FALSE);

   return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Audio driver un-initial function
* @param    pUnInitialParam : Un-initial parameters for audio driver initial
*		        pUnInitialParam->reserved : reserved parameter
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudUninit(DRV_AudUnInitialConfig_t *pUnInitialParam)
{
    if(pUnInitialParam == DTV_NULL)
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        HW_DTL_Uninit();
#if 0
        if(gfAudInitFlag==DTV_TRUE)
        {
            cyg_mutex_destroy(&gAudSem);
            gfAudInitFlag=DTV_FALSE;
        }
#endif
    return DRV_SUCCESS;
    }
}

/******************************************************************************/
/**
* @brief    Get feature version
* @param    pRetFeatureVersion : feature verion data
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudGetFeatureVersion(DRV_AudFeatureVersion_t *pRetFeatureVersion)
{
    	return DRV_SUCCESS;
#if 0
    UINT32 FeartureBits;

    if(pRetFeatureVersion == DTV_NULL)
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        /*reset return value*/
        memset(pRetFeatureVersion,DTV_FALSE,sizeof(DRV_AudFeatureVersion_t));

        FeartureBits = HW_SSDGetBit(REG_Ssd_xr_vds_OFFSET,0xffffffff,0);

        if(FeartureBits & REG_Ssd_xr_vds_BIT_MASK)
        {
            pRetFeatureVersion->fEnVDS = DTV_TRUE;
        }
        if(FeartureBits & REG_Ssd_xr_srs_ts_pa_BIT_MASK)
        {
            pRetFeatureVersion->fEnSRSTruSurroundPassiveMatrix = DTV_TRUE;
        }
        if(FeartureBits & REG_Ssd_xr_srs_3ds_BIT_MASK)
        {
            pRetFeatureVersion->fEnSRS3D = DTV_TRUE;
        }
    //    if(FeartureBits & REG_Ssd_xr_bbe_BIT_MASK)
    //    {
    //        RetFeatureVersion->fEnBBE = DTV_TRUE;
    //    }
        if(FeartureBits & REG_Ssd_xr_srs_ts_ac_BIT_MASK)
        {
            pRetFeatureVersion->fEnSRSTruSurroundXT = DTV_TRUE;
        }
        if(FeartureBits & REG_Ssd_xr_srs_wow_BIT_MASK)
        {
            pRetFeatureVersion->fEnSRSWOW = DTV_TRUE;
        }
    	return DRV_SUCCESS;
    }
   #endif
}

/******************************************************************************/
/**
* @brief    Set Overmodulation configuration
* @param    fEnAdapter : DTV_TRUE : enable FM overmodulation adaptation
*                        DTV_FALSE : disable FM overmodulation adaptation
* @param    bFilterBanndWidth : active if bFilterBanndWidth = DTV_TRUE
*                               0 = narrow
*                               2 = medium
*								3 = wide
*                               1 = extra wide (only ch. 1 active)
* @param    bLevelThershold : active if bFilterBanndWidth = DTV_TRUE
*                               0 = +3 dB = -12 dBFS
*                               1 = +6 dB = - 9 dBFS
*                               2 = +9 dB = -6 dBFS
*                               3 = +12 dB= -3 dBFS
* @param    bAttenuation : active if bFilterBanndWidth = DTV_FALSE
*                               0(0 dB) ~ 31(31 dB) attenuation of analog standard
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetOverModulationAdapterConfig(Boolean fEnAdapter,
															UINT8 bFilterBanndWidth,
															UINT8 bLevelThershold,
															UINT8 bAttenuation)
{

    if((bFilterBanndWidth > REG_Ssd_filtbw_MAX)
		||(bLevelThershold > REG_Ssd_ovmthr_MAX)
		||(bAttenuation > REG_Ssd_ovmatten_MAX))
        return DRV_ERR_INVALID_PARAM;

	if(fEnAdapter == DTV_TRUE)
	{
		HW_SSDSetReg(REG_Ssd_ovmadapt, REG_SSD_OVMADAPT_VALUE_ENABLED);
		HW_SSDSetReg(REG_Ssd_filtbw, bFilterBanndWidth);
		HW_SSDSetReg(REG_Ssd_ovmthr, bLevelThershold);
		HW_SSDSetReg(REG_Ssd_ovmatten, bAttenuation);
	}
	else
	{
		HW_SSDSetReg(REG_Ssd_ovmadapt, REG_SSD_OVMADAPT_VALUE_DISABLED);
		HW_SSDSetReg(REG_Ssd_filtbw, bFilterBanndWidth);
		HW_SSDSetReg(REG_Ssd_ovmthr, bLevelThershold);
		HW_SSDSetReg(REG_Ssd_ovmatten, bAttenuation);
	}
	return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set magnitude detect configuration
* @param    bCarrier1_threshold : -16(-16 dB) ~ 15(+15 dB),(1 step is 1 dB),threshold for detection of first sound carrier (SC1) during ASD first step, relative to -30 dBFS. -16 prevents ASD "failure" to produce output regardless of carrier level.
* @param    bMPX_threshold_low : 0(0 dB) ~ 15(+15 dB),(1 step is 1 dB),lower threshold for MPX pilot detection (BTSC, FM RADIO) in dB below nominal level
* @param    bMPX_threshold_up : 0(0 dB) ~ 15(+15 dB),(1 step is 1 dB),upper threshold for MPX pilot detection (BTSC, FM RADIO) in dB below nominal level
* @param    bSAP_threshold_low : 0(0 dB) ~ 15(+15 dB),(1 step is 1 dB),lower threshold for SAP carrier detection in dB below nominal level
* @param    bSAP_threshold_up : 0(0 dB) ~ 15(+15 dB),(1 step is 1 dB),upper threshold for SAP carrier detection in dB below nominal level
* @param    fEnSub_carrier_detect : DTV_TRUE : enable EIAJ sub carrier detect
*                                   DTV_FALSE : disable EIAJ sub carrier detect
* @param    bEIAJ_sub_carrier_threshold_low : 0(0 dB) ~ 15(+15),(1 step is 1 dB),lower threshold for EIAJ SUB carrier detection in dB below nominal level
* @param    bEIAJ_sub_carrier_threshold_up : 0(0 dB) ~ 15(+15),(1 step is 1 dB),upper threshold for EIAJ SUB carrier detection in dB below nominal level
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetMagnitudeDetectConfig(INT8 bCarrier1_threshold,
                                                        UINT8 bMPX_threshold_low,
                                                        UINT8 bMPX_threshold_up,
                                                        UINT8 bSAP_threshold_low,
                                                        UINT8 bSAP_threshold_up,
                                                        Boolean fEnSub_carrier_detect,
                                                        UINT8 bEIAJ_sub_carrier_threshold_low,
                                                        UINT8 bEIAJ_sub_carrier_threshold_up)
{
	HW_SSDCheckRegRange(REG_Ssd_asd_sc1_thr,bCarrier1_threshold);
#if 1
    if((bMPX_threshold_low > REG_Ssd_mpx_pilot_thr_lo_MAX)
        ||(bMPX_threshold_up > REG_Ssd_mpx_pilot_thr_up_MAX)
        ||(bSAP_threshold_low > REG_Ssd_sap_car_thr_lo_MAX)
        ||(bSAP_threshold_up > REG_Ssd_sap_car_thr_up_MAX)
        ||(bEIAJ_sub_carrier_threshold_low > REG_Ssd_eiaj_car_thr_lo_MAX)
        ||(bEIAJ_sub_carrier_threshold_up > REG_Ssd_eiaj_car_thr_up_MAX))
        return DRV_ERR_INVALID_PARAM;
#else

	HW_SSDCheckRegRange(REG_Ssd_mpx_pilot_thr_lo,bMPX_threshold_low);
	HW_SSDCheckRegRange(REG_Ssd_mpx_pilot_thr_up,bMPX_threshold_up);
	HW_SSDCheckRegRange(REG_Ssd_sap_car_thr_lo,bSAP_threshold_low);
	HW_SSDCheckRegRange(REG_Ssd_sap_car_thr_up,bSAP_threshold_up);
    HW_SSDCheckRegRange(REG_Ssd_eiaj_car_thr_lo, bEIAJ_sub_carrier_threshold_low);
    HW_SSDCheckRegRange(REG_Ssd_eiaj_car_thr_up, bEIAJ_sub_carrier_threshold_up);
#endif

    /*set sound carrier1 detect threshold value*/
    HW_SSDSetReg(REG_Ssd_asd_sc1_thr, bCarrier1_threshold);

    /*set MPX detect threshold value*/
    HW_SSDSetReg(REG_Ssd_mpx_pilot_thr_lo, bMPX_threshold_low);
    HW_SSDSetReg(REG_Ssd_mpx_pilot_thr_up, bMPX_threshold_up);

    /*set SAP detect threshold value*/
    HW_SSDSetReg(REG_Ssd_sap_car_thr_lo, bSAP_threshold_low);
    HW_SSDSetReg(REG_Ssd_sap_car_thr_up, bSAP_threshold_up);

    /*Set EIAJ sub carrier detect*/
    if(fEnSub_carrier_detect == DTV_TRUE)
    {
        HW_SSDSetReg(REG_Ssd_eiaj_car_detect, REG_SSD_EIAJ_CAR_DETECT_VALUE_ENABLED);
    }
    else
    {
        HW_SSDSetReg(REG_Ssd_eiaj_car_detect, REG_SSD_EIAJ_CAR_DETECT_VALUE_DISABLED);
    }

    /*set EIAJ subcarrier detect threshold value*/
    HW_SSDSetReg(REG_Ssd_eiaj_car_thr_lo, bEIAJ_sub_carrier_threshold_low);
    HW_SSDSetReg(REG_Ssd_eiaj_car_thr_up, bEIAJ_sub_carrier_threshold_up);

    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set SAP auto mute configuration
* @param    bSAP_nmute_threshold : -16(-16 dB) ~ 15(+15 dB),(1 step is 1 dB),noise threshold for automute of SAP (-16 means automute off)
* @param    bSAP_nmute_hysteresis : 0(0 dB) ~ 15(+15 dB),(1 step is 1 dB),hysteresis size [dB] for automute of SAP
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetSAPAutoMuteConfig(INT8 bSAP_nmute_threshold,
                                                    UINT8 bSAP_nmute_hysteresis)
{
	HW_SSDCheckRegRange(REG_Ssd_nmute_sap_thr,bSAP_nmute_threshold);
#if 1
    if(bSAP_nmute_hysteresis > REG_Ssd_nmute_sap_hyst_MAX)
        return DRV_ERR_INVALID_PARAM;
#else
	HW_SSDCheckRegRange(REG_Ssd_nmute_sap_hyst,SAP_nmute_hysteresis);
#endif

    HW_SSDSetReg(REG_Ssd_nmute_sap_thr, bSAP_nmute_threshold);
    HW_SSDSetReg(REG_Ssd_nmute_sap_hyst, bSAP_nmute_hysteresis);

    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set FM/A2 sound carrier 2 auto mute configuration
* @param    bFM_A2_sc2_nmute_threshold : -16(-16 dB) ~ 15(+15 dB),(1 step is 1 dB),noise threshold for automute of SC2 in FM A2 standards     (-16 means automute off)
* @param    bFM_A2_sc2_nmute_hysteresis : 0(0 dB) ~ 15(+15 dB),(1 step is 1 dB),hysteresis size [dB] for automute of SC2 in FM A2 standards
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetFMA2Carrier2AutoMuteConfig(INT8 bFM_A2_sc2_nmute_threshold,
                                                            UINT8 bFM_A2_sc2_nmute_hysteresis)
{
	HW_SSDCheckRegRange(REG_Ssd_nmute_sc2_thr,bFM_A2_sc2_nmute_threshold);
#if 1
    if(bFM_A2_sc2_nmute_hysteresis > REG_Ssd_nmute_sc2_hyst_MAX)
        return DRV_ERR_INVALID_PARAM;
#else
	HW_SSDCheckRegRange(REG_Ssd_nmute_sc2_hyst,bFM_A2_sc2_nmute_hysteresis);
#endif

    HW_SSDSetReg(REG_Ssd_nmute_sc2_thr, bFM_A2_sc2_nmute_threshold);
    HW_SSDSetReg(REG_Ssd_nmute_sc2_hyst, bFM_A2_sc2_nmute_hysteresis);

    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set BTSC auto mute configuration
* @param    bBTSC_nmute_threshold : -16(-16 dB) ~ 15(+15 dB),(1 step is 1 dB),noise threshold for automute of BTSC stereo carrier (-16 means automute off)
* @param    bBTSC_nmute_hysteresis : 0(0 dB) ~ 15(+15 dB),(1 step is 1 dB),hysteresis size [dB] for automute of BTSC stereo
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetBTSCAutoMuteConfig(INT8 bBTSC_nmute_threshold,
                                                    UINT8 bBTSC_nmute_hysteresis)
{
	HW_SSDCheckRegRange(REG_Ssd_nmute_btsc_thr,bBTSC_nmute_threshold);
#if 1
    if(bBTSC_nmute_hysteresis > REG_Ssd_nmute_btsc_hyst_MAX)
        return DRV_ERR_INVALID_PARAM;
#else
	HW_SSDCheckRegRange(REG_Ssd_nmute_btsc_hyst,bBTSC_nmute_hysteresis);
#endif

    HW_SSDSetReg(REG_Ssd_nmute_btsc_thr, bBTSC_nmute_threshold);
    HW_SSDSetReg(REG_Ssd_nmute_btsc_hyst, bBTSC_nmute_hysteresis);

    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set FM radio auto mute configuration
* @param    bFM_radio_nmute_threshold : -16(-16 dB) ~ 15(+15 dB),(1 step is 1 dB),noise threshold for automute of FM RADIO stereo carrier (-16 means automute off)
* @param    bFM_radio_nmute_hysteresis : 0(0 dB) ~ 15(+15 dB),(1 step is 1 dB),hysteresis size [dB] for automute of FM RADIO stereo
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetFMRadioAutoMuteConfig(INT8 bFM_radio_nmute_threshold,
                                                        UINT8 bFM_radio_nmute_hysteresis)
{
	HW_SSDCheckRegRange(REG_Ssd_nmute_fmra_thr,bFM_radio_nmute_threshold);
#if 1
    if(bFM_radio_nmute_hysteresis > REG_Ssd_nmute_fmra_hyst_MAX)
        return DRV_ERR_INVALID_PARAM;
#else
	HW_SSDCheckRegRange(REG_Ssd_nmute_fmra_hyst,bFM_radio_nmute_hysteresis);
#endif

    HW_SSDSetReg(REG_Ssd_nmute_fmra_thr, bFM_radio_nmute_threshold);
    HW_SSDSetReg(REG_Ssd_nmute_fmra_hyst, bFM_radio_nmute_hysteresis);

    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set EIAJ sub carrier auto mute configuration
* @param    bSub_carrier_nmute_threshold : -16(-16 dB) ~ 15(+15),(1 step is 1 dB),noise threshold for automute of EIAJ FM subcarrier (-16 means automute off)
* @param    bSub_carrier_nmute_hysteresis : 0(0 dB) ~ 15(+15),(1 step is 1 dB),hysteresis size [dB] for automute of EIAJ FM subcarrier
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetEIAJSubCarrierAutoMuteConfig(INT8 bSub_carrier_nmute_threshold,
                                                            UINT8 bSub_carrier_nmute_hysteresis)
{
    HW_SSDCheckRegRange(REG_Ssd_nmute_eiaj_thr, bSub_carrier_nmute_threshold);
#if 1
    if(bSub_carrier_nmute_hysteresis > REG_Ssd_nmute_eiaj_hyst_MAX)
        return DRV_ERR_INVALID_PARAM;
#else
    HW_SSDCheckRegRange(REG_Ssd_nmute_eiaj_hyst, bSub_carrier_nmute_hysteresis);
#endif

    /*set EIAJ subcarrier automute threshold value*/
    HW_SSDSetReg(REG_Ssd_nmute_eiaj_thr, bSub_carrier_nmute_threshold);
    HW_SSDSetReg(REG_Ssd_nmute_eiaj_hyst, bSub_carrier_nmute_hysteresis);

    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set NICAM configuration
* @param    fEnOnly_related : DTV_TRUE : enable to reproduce only related NICAM on DEC output (DDEP only)
*                             DTV_FALSE : disable to reproduce only related NICAM on DEC output (DDEP only)
* @param    fEnDeemphasis : DTV_TRUE : enable NICAM deemphasis (J17) (all modes)
*                           DTV_FALSE : disable NICAM deemphasis (J17) (all modes)
* @param    fEnAuto_mute : DTV_TRUE : enable NICAM auto mute function depending on bit error rate (DDEP only)
*                          DTV_FALSE : disable NICAM auto mute function depending on bit error rate (DDEP only)
* @param    bLower_error_limit : 0 ~ 255,NICAM lower error limit (DDEP only)
* @param    bUpper_error_limit : 0 ~ 255,NICAM upper error limit (DDEP only)
* @return   DRV_SUCCESS if successful
******************************************************************************/
DRV_Status_t DRV_AudSetNICAMConfig(Boolean fEnOnly_related,
                                            Boolean fEnDeemphasis,
                                            Boolean fEnAuto_mute,
                                            UINT8 bLower_error_limit,
                                            UINT8 bUpper_error_limit)
{
    /*Set NICAM configuration*/
    if(fEnOnly_related == DTV_TRUE)
    {
        HW_SSDSetReg(REG_Ssd_only_related,REG_SSD_ONLY_RELATED_VALUE_TRUE);
    }
    else
    {
        HW_SSDSetReg(REG_Ssd_only_related,REG_SSD_ONLY_RELATED_VALUE_FALSE);
    }

    if(fEnDeemphasis == DTV_TRUE)
    {
        HW_SSDSetReg(REG_Ssd_nicdeem,REG_SSD_NICDEEM_VALUE_ON);
    }
    else
    {
        HW_SSDSetReg(REG_Ssd_nicdeem,REG_SSD_NICDEEM_VALUE_OFF);
    }

    if(fEnAuto_mute == DTV_TRUE)
    {
        HW_SSDSetReg(REG_Ssd_nic_amute,REG_SSD_NICDEEM_VALUE_ON);
    }
    else
    {
        HW_SSDSetReg(REG_Ssd_nic_amute,REG_SSD_NICDEEM_VALUE_OFF);
    }

    HW_SSDSetReg(REG_Ssd_nicloerrlim,bLower_error_limit);
    HW_SSDSetReg(REG_Ssd_nicuperrlim,bUpper_error_limit);

    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set DEMDEC standard detection Action
* @param    eAction : star/release DEMDEC stardard detect
* @return   DRV_SUCCESS if successful
******************************************************************************/
DRV_Status_t DRV_AudSetDetectStandard(DRV_AudDEMDECResetAction_t eAction)
{
    HW_SSDSetReg(REG_Ssd_rest, eAction);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Get detected SIF Standard from DEMDEC
* @param    pRetAudioStandard : report IF sound standard
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudGetStandard(DRV_AudStandard_t *pRetAudioStandard)
{
    if(pRetAudioStandard == DTV_NULL)
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        *pRetAudioStandard=HW_SSDGetReg(REG_Ssd_stdres);
        return DRV_SUCCESS;
    }
}

/******************************************************************************/
/**
* @brief    Get if SAP available
* @param    pfRetSAP : report if SAP detected
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudGetIfSAP(Boolean *pfRetSAP)
{
    if(pfRetSAP == DTV_NULL)
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        if(HW_SSDGetReg(REG_Ssd_sapdet)==REG_SSD_SAPDET_VALUE_TRUE)
        {
            if(HW_SSDGetReg(REG_Ssd_samut)==REG_SSD_SAMUT_VALUE_TRUE)
            {
                *pfRetSAP=DTV_FALSE;
            }
            else
            {
                *pfRetSAP=DTV_TRUE;
            }
        }
        else
        {
            *pfRetSAP=DTV_FALSE;
        }
        return DRV_SUCCESS;
    }
}

/******************************************************************************/
/**
* @brief    Get If dual(bilingual) available
* @param    pfRetDual : report if (bilingual) detected
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
 ******************************************************************************/
DRV_Status_t DRV_AudGetIfDual(Boolean *pfRetDual)
{
    if(pfRetDual == DTV_NULL)
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        if(HW_SSDGetReg(REG_Ssd_gdu)==REG_SSD_GDU_VALUE_DUAL_MODE_DETECTED)
        {
            *pfRetDual=DTV_TRUE;
        }
        else
        {
            *pfRetDual=DTV_FALSE;
        }
        return DRV_SUCCESS;
    }
}

/******************************************************************************/
/**
* @brief    Get if stereo available
* @param    pfRetStereo : report if stereo detected
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudGetIfStereo(Boolean *pfRetStereo)
{
    if(pfRetStereo == DTV_NULL)
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        if (HW_SSDGetReg(REG_Ssd_stdres) == REG_SSD_STDRES_VALUE_M_BTSC)
        {
            if ((HW_SSDGetReg(REG_Ssd_bpilot) == REG_SSD_BPILOT_VALUE_TRUE)
                && (HW_SSDGetReg(REG_Ssd_bamut) == REG_SSD_BAMUT_VALUE_FALSE))
            {
                *pfRetStereo=DTV_TRUE;
            }
            else
            {
                *pfRetStereo=DTV_FALSE;
            }
        }
        else
        {
            if(HW_SSDGetReg(REG_Ssd_gst)==REG_SSD_GST_VALUE_STEREO_MODE_DETECTED)
            {
                *pfRetStereo=DTV_TRUE;
            }
            else
            {
                *pfRetStereo=DTV_FALSE;
            }
        }
        return DRV_SUCCESS;
    }
}

/******************************************************************************/
/**
* @brief    Get if NICAM available
* @param    pfRetNicam : report if NICAM detected
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudGetIfNICAM(Boolean *pfRetNicam)
{
    if(pfRetNicam == DTV_NULL)
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        /* Check NICAM decoder VDSP flag */
        if(HW_SSDGetReg(REG_Ssd_vdsp_c)==REG_SSD_VDSP_C_VALUE_SOUND)
        {
            if(HW_SSDGetReg(REG_Ssd_namut)==REG_SSD_NAMUT_VALUE_MUTED_FALLBACK_TO_ANALOG_SOUND_CARRIER)
            {
                *pfRetNicam=DTV_FALSE;
            }
            else
            {
                *pfRetNicam=DTV_TRUE;
            }
        }
        else
        {
            *pfRetNicam=DTV_FALSE;
        }
        return DRV_SUCCESS;
    }
}

/******************************************************************************/
/**
* @brief    Get front end status from registers
* @param    pfRetFrontEndReg : report if front end register values
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudGetFrontEndRegInfo(DRV_AudFrontEndReg_t *pfRetFrontEndReg)
{
    if(pfRetFrontEndReg == DTV_NULL)
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        pfRetFrontEndReg->_INF_DEV_STA_REG      = HW_SSDGetBit(REG_Ssd_stdres_OFFSET,0xffffffff,0);
        pfRetFrontEndReg->_INF_NIC_STA_REG      = HW_SSDGetBit(REG_Ssd_err_out_OFFSET,0xffffffff,0);
        pfRetFrontEndReg->_INF_NIC_ADD_REG      = HW_SSDGetBit(REG_Ssd_adw_OFFSET,0xffffffff,0);
        pfRetFrontEndReg->_INF_LEV_MON_REG      = HW_SSDGetBit(REG_Ssd_monlevel_OFFSET,0xffffffff,0);
        pfRetFrontEndReg->_INF_MPX_LEVEL_REG    = HW_SSDGetBit(REG_Ssd_hw_ast_OFFSET,0xffffffff,0);
        pfRetFrontEndReg->_INF_NOISELEVEL_REG   = HW_SSDGetBit(REG_Ssd_ndetch_stat_OFFSET,0xffffffff,0);
        pfRetFrontEndReg->_INF_OVMADAPT_REG     = HW_SSDGetBit(REG_Ssd_filtbw_stat_OFFSET,0xffffffff,0);
	DRV_AUDIO_DBG("INF_DEV_STA_REG: %x\n",pfRetFrontEndReg->_INF_DEV_STA_REG);
	DRV_AUDIO_DBG("INF_NIC_STA_REG: %x\n",pfRetFrontEndReg->_INF_NIC_STA_REG);
	DRV_AUDIO_DBG("INF_NIC_ADD_REG: %x\n",pfRetFrontEndReg->_INF_NIC_ADD_REG);
	DRV_AUDIO_DBG("INF_LEV_MON_REG: %x\n",pfRetFrontEndReg->_INF_LEV_MON_REG);
	DRV_AUDIO_DBG("INF_MPX_LEVEL_REG: %x\n",pfRetFrontEndReg->_INF_MPX_LEVEL_REG);

        return DRV_SUCCESS;
    }
}

/******************************************************************************/
/**
* @brief    Set BTSC DBX mode
* @param    eMode : select DBX NR mode
* @return   DRV_SUCCESS if successful
******************************************************************************/
DRV_Status_t DRV_AudSetBTSCDBXMode(DRV_AudBTSCDBXMode_t eMode)
{
	if(eMode == DRV_AUD_STEREO_MODE)
	{
		HW_SSDSetReg(REG_Ssd_sapnr, REG_SSD_SAPNR_VALUE_150_US_DEEMPHASIS_FOR_SAP_NOISE_REDUCTION_FOR_BTSC_STEREO);
	}
	else if(eMode == DRV_AUD_SAP_MODE)
	{
		HW_SSDSetReg(REG_Ssd_sapnr, REG_SSD_SAPNR_VALUE_NOISE_REDUCTION_USED_FOR_SAP_BTSC_STEREO_FORCED_TO_MONO);
	}
	return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set standard detect mask for DEMDEC ADS(Auto Standard Detect) mode
* @param    eAutoStandardGroup : specify standard recognization mask
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetAutoStandardSelection(DRV_AudAutoStandardGroup_t eAutoStandardGroup)
{
#if 1
	DRV_Status_t ret = DRV_SUCCESS;
	HW_SSDSetReg(REG_Ssd_epmode, REG_SSD_EPMODE_VALUE_ASD);//ASD mode
	UINT8 StandardSelection = 0,index = 0;
	UINT8 mapping_table[][2] =
		{
			{DRV_AUD_AUTO_BG_GROUP,   REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_B_G	},
			{DRV_AUD_AUTO_DK_GROUP,   REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_D_K    },
			{DRV_AUD_AUTO_L_GROUP,    REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_L_L    },
			{DRV_AUD_AUTO_I_GROUP,    REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_I   	},
			{DRV_AUD_AUTO_M_GROUP,    REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_M		},
		};

//		diag_printf("++++++DRV_AudSetAutoStandardSelection eAutoStandardGroup=%x, size=%d\n",eAutoStandardGroup,(sizeof(mapping_table)>>1));

		for(index = 0; index < (sizeof(mapping_table)>>1);index++)
		{
			if((eAutoStandardGroup & (mapping_table[index][0])) != 0)
			{
//				diag_printf("++++++DRV_AudSetAutoStandardSelection index=%d data1=%x data2=%x\n",index,(mapping_table[index][0]),(mapping_table[index][1]));
				StandardSelection |= (mapping_table[index][1]);
			}
		}

		DRV_AUDIO_DBG("++++++DRV_AudSetAutoStandardSelection StandardSelection=%x\n",StandardSelection);

		HW_SSDSetReg(REG_Ssd_stdsel_asd_mode,StandardSelection);

	return ret;
#else
	DRV_Status_t ret = DRV_SUCCESS;
	HW_SSDSetReg(REG_Ssd_epmode, REG_SSD_EPMODE_VALUE_ASD);//ASD mode

	switch(eAutoStandardGroup)
	{
		case DRV_AUD_AUTO_BG_GROUP:
			HW_SSDSetReg(REG_Ssd_stdsel_asd_mode, REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_B_G);
			break;

		case DRV_AUD_AUTO_DK_GROUP:
			HW_SSDSetReg(REG_Ssd_stdsel_asd_mode, REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_D_K);
			break;

		case DRV_AUD_AUTO_L_GROUP:
			HW_SSDSetReg(REG_Ssd_stdsel_asd_mode, REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_L_L);
			break;

		case DRV_AUD_AUTO_I_GROUP:
			HW_SSDSetReg(REG_Ssd_stdsel_asd_mode, REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_I);
			break;

		case DRV_AUD_AUTO_M_GROUP:
			HW_SSDSetReg(REG_Ssd_stdsel_asd_mode, REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_M);
			break;

		case DRV_AUD_AUTO_BGDKI_GROUP:
			HW_SSDSetReg(REG_Ssd_stdsel_asd_mode, REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_B_G
													|REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_D_K
													|REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_I);
			break;

		case DRV_AUD_AUTO_BGDKIM_GROUP:
			HW_SSDSetReg(REG_Ssd_stdsel_asd_mode, REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_B_G
													|REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_D_K
													|REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_I
													|REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_M);
			break;

		case DRV_AUD_AUTO_ALL_GROUP:
			HW_SSDSetReg(REG_Ssd_stdsel_asd_mode, REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_B_G
													|REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_D_K
													|REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_L_L
													|REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_I
													|REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_M);
			break;

		default:
            ret = DRV_ERR_INVALID_PARAM;
			break;
	}
	return ret;
#endif
}

/******************************************************************************/
/**
* @brief    Set standard detect mask for DEMDEC SSS(Static Standard Selection) mode
* @param    eStaticStandard :
* @return   DRV_SUCCESS if successful
******************************************************************************/
DRV_Status_t DRV_AudSetStaticStandardSelection(DRV_AudStaticStandardSelection_t eStaticStandard)
{
	HW_SSDSetReg(REG_Ssd_epmode, REG_SSD_EPMODE_VALUE_SSS);//SSS mode
	HW_SSDCheckRegRange(REG_Ssd_stdsel,eStaticStandard);
	HW_SSDSetReg(REG_Ssd_stdsel, eStaticStandard);
	return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set NICAM QPSK rotate angle, only for I NICAM standard
* @param    DRV_AudNICAMRotateAngle_t eRotateAngel : rotate angle for I NICAM
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetINICAMRotateAngle(DRV_AudNICAMRotateAngle_t eRotateAngle)
{
#if 1
    switch(eRotateAngle)
    {
        case DRV_AUD_NICAM_QPSK_ROTATE_0_DEGREE:
            /*rotate angle*/
            HW_SetReg(reg_ssd_selclip2, REG_SSD_SELCLIP2_V__DEFAULT_VALUE___NO_ROTATION);

            /*frequency offset*/

            break;

        case DRV_AUD_NICAM_QPSK_ROTATE_PLUS_90_DEGREE:
            /*rotate angle*/
            HW_SetReg(reg_ssd_selclip2, REG_SSD_SELCLIP2_V_ROTATE__90_DEGREES__CARRIER2_NICAMI_23_1);

            /*frequency offset*/
            HW_SetReg(reg_ssd_carrier2_nicami2, (INICAM_QPSK_PLUS_90_DEGREE_CARRIER2_MINUS_90_POINT_72KHZ >> 16));
            HW_SetReg(reg_ssd_carrier2_nicami1, (UINT8)(INICAM_QPSK_PLUS_90_DEGREE_CARRIER2_MINUS_90_POINT_72KHZ >> 8));
            HW_SetReg(reg_ssd_carrier2_nicami0, (UINT8)(INICAM_QPSK_PLUS_90_DEGREE_CARRIER2_MINUS_90_POINT_72KHZ));

            break;

        case DRV_AUD_NICAM_QPSK_ROTATE_MINUS_90_DEGREE:
		case DRV_AUD_NICAM_QPSK_ROTATE_MAX:
            /*rotate angle*/
            HW_SetReg(reg_ssd_selclip2, REG_SSD_SELCLIP2_V_ROTATE__90_DEGREES__CARRIER2_NICAMI_23_2);

            /*frequency offset*/
            HW_SetReg(reg_ssd_carrier2_nicami2, (INICAM_QPSK_MINUS_90_DEGREE_CARRIER2_PLUS_90_POINT_72KHZ >> 16));
            HW_SetReg(reg_ssd_carrier2_nicami1, (UINT8)(INICAM_QPSK_MINUS_90_DEGREE_CARRIER2_PLUS_90_POINT_72KHZ >> 8));
            HW_SetReg(reg_ssd_carrier2_nicami0, (UINT8)(INICAM_QPSK_MINUS_90_DEGREE_CARRIER2_PLUS_90_POINT_72KHZ));

            break;

    }
#endif
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set adjust level configuration
* @param    bDECAdjustLevel : -16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 db),
* @param    bMonoAdjustLevel : -16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 db),
* @param    bNICAMAdjustLevel : -16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 db),
* @param    bSAPAdjustLevel : -16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 db),
* @param    bADCAdjustLevel : -16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 db),
* @param    bDigitalAdjustLevel : -16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 db),
* @return   DRV_SUCCESS if successful
******************************************************************************/
DRV_Status_t DRV_AudSetPathAdjustLevel(INT8 bDECAdjustLevel,
                                                INT8 bMonoAdjustLevel,
                                                INT8 bNICAMAdjustLevel,
                                                INT8 bSAPAdjustLevel,
                                                INT8 bADCAdjustLevel,
                                                INT8 bDigitalAdjustLevel)
{

	HW_SSDCheckRegRange(REG_Ssd_declev,bDECAdjustLevel);
	HW_SSDCheckRegRange(REG_Ssd_monolev,bMonoAdjustLevel);
	HW_SSDCheckRegRange(REG_Ssd_niclev,bNICAMAdjustLevel);
	HW_SSDCheckRegRange(REG_Ssd_saplev,bSAPAdjustLevel);
	HW_SSDCheckRegRange(REG_Ssd_adclev,bADCAdjustLevel);
	HW_SSDCheckRegRange(REG_Ssd_diginlev,bDigitalAdjustLevel);

    HW_SSDSetReg(REG_Ssd_declev, bDECAdjustLevel);
    HW_SSDSetReg(REG_Ssd_monolev, bMonoAdjustLevel);
    HW_SSDSetReg(REG_Ssd_niclev, bNICAMAdjustLevel);
    HW_SSDSetReg(REG_Ssd_saplev, bSAPAdjustLevel);
    HW_SSDSetReg(REG_Ssd_adclev, bADCAdjustLevel);
    HW_SSDSetReg(REG_Ssd_diginlev, bDigitalAdjustLevel);

	return DRV_SUCCESS;
}
#if 0
/******************************************************************************/
/**
* @brief    Get adjust level configuration
* @param    *pbRetDECAdjustLevel : -16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 db),
* @param    *pbRetMonoAdjustLevel : -16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 db),
* @param    *pbRetSAPAdjustLevel : -16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 db),
* @param    *pbRetSAPAdjustLevel : -16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 db),
* @param    *pbRetADCAdjustLevel : -16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 db),
* @param    *pbDigitalAdjustLevel : -16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 db),
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudGetPathAdjustLevel(SINT8 *pbRetDECAdjustLevel,
                                                SINT8 *pbRetMonoAdjustLevel,
                                                SINT8 *pbRetNICAMAdjustLevel,
                                                SINT8 *pbRetSAPAdjustLevel,
                                                SINT8 *pbRetADCAdjustLevel,
                                                SINT8 *pbRetDigitalAdjustLevel)
{
	if((pbRetDECAdjustLevel == DTV_NULL)
		||(pbRetMonoAdjustLevel == DTV_NULL)
		||(pbRetNICAMAdjustLevel == DTV_NULL)
		||(pbRetSAPAdjustLevel == DTV_NULL)
		||(pbRetADCAdjustLevel == DTV_NULL)
		||(pbRetDigitalAdjustLevel == DTV_NULL))
	{
        return DRV_ERR_FAILURE;
	}

	HW_SSDSignExpand(REG_Ssd_declev,*pbRetDECAdjustLevel);
    HW_SSDSignExpand(REG_Ssd_monolev,*pbRetMonoAdjustLevel);
    HW_SSDSignExpand(REG_Ssd_niclev,*pbRetNICAMAdjustLevel);
    HW_SSDSignExpand(REG_Ssd_saplev,*pbRetSAPAdjustLevel);
    HW_SSDSignExpand(REG_Ssd_adclev,*pbRetADCAdjustLevel);
    HW_SSDSignExpand(REG_Ssd_diginlev,*pbRetDigitalAdjustLevel);

	return DRV_SUCCESS;
}
#endif
/******************************************************************************/
/**
* @brief    Set digital divider for digital audio input
* @param    eDigitalDividerMode : select sample rate divider
* @return   DRV_SUCCESS if successful
******************************************************************************/
DRV_Status_t DRV_AudSetDigitalDivider(DRV_AudDigitalDivider_t eDigitalDividerMode)
{
	HW_SSDCheckRegRange(REG_Ssd_digindiv,eDigitalDividerMode);
    HW_SSDSetReg(REG_Ssd_digindiv, eDigitalDividerMode);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set digital input de-emphasis
* @param    eDigitalDeemphasisMode : config de-emphasis mode
* @return   DRV_SUCCESS if successful
******************************************************************************/
DRV_Status_t DRV_AudSetDigitalInputDeemphasis(DRV_AudDigitalDeemphasis_t eDigitalDeemphasisMode)
{
    /*Set SPDIF Deemphasis*/
	HW_SSDCheckRegRange(REG_Ssd_digin_deem,eDigitalDeemphasisMode);
	HW_SSDSetReg(REG_Ssd_digin_deem, eDigitalDeemphasisMode);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set digital matrix configuration
* @param    eProcessPath : specify process path
* @param    eDigitalMatrix : specify Digital Matrix
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetDigitalMatrix(DRV_AudProcessPath_t eProcessPath,
                                 DRV_AudDigitalMatrix_t eDigitalMatrix)
{
	DRV_Status_t ret = DRV_SUCCESS;
    switch(eProcessPath)
    {
        case DRV_AUD_PROCESS_PATH_MAIN:
            HW_SSDSetReg(REG_Ssd_maindm, eDigitalMatrix);
            break;
        case DRV_AUD_PROCESS_PATH_AUX1:
            HW_SSDSetReg(REG_Ssd_aux1dm, eDigitalMatrix);
            break;
        case DRV_AUD_PROCESS_PATH_AUX2:
            HW_SSDSetReg(REG_Ssd_aux2dm, eDigitalMatrix);
            break;
        case DRV_AUD_PROCESS_PATH_AUX3:
            HW_SSDSetReg(REG_Ssd_aux3dm, eDigitalMatrix);
            break;

        default:
            ret =  DRV_ERR_INVALID_PARAM;
            break;
    }
	return ret;
}

/******************************************************************************/
/**
* @brief    Set SIF input selection
* @param    eIFSource : select SIF input source
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetIFInput(DRV_AudIFInput_t eIFSource)
{
    if((eIFSource < REG_SSD_SIFADC_MUX_V_MIN)
        ||(eIFSource > REG_SSD_SIFADC_MUX_V_MAX))
    {
        return DRV_ERR_INVALID_PARAM;
    }
    else
    {
        /*set IF switch*/
		if(eIFSource == DRV_AUD_INPUT_SIF0)
		{
        	HW_SetReg(reg_ssd_sifadc_mux, REG_SSD_SIFADC_MUX_V_SIFAIN0);
		}
		else
		{
			HW_SetReg(reg_ssd_sifadc_mux, REG_SSD_SIFADC_MUX_V_SIFAIN1);
		}
        return DRV_SUCCESS;
    }
}


/******************************************************************************//**
* @brief    Set line-in ADC input selection
* @param    eADCSource : line-in ADC Source for analog input
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetADCInput(DRV_AudADCInput_t eADCSource)
{
	#if 0
	DRV_Status_t ret = DRV_SUCCESS;
    switch (eADCSource)
    {
        case DRV_AUD_INPUT_MPEG_DSP_PCM:
            HW_SetReg(reg_ssd_is_pcm,REG_SSD_IS_PCM_V_DSP_);
            break;

        /* Albert, 2008/05/26, ADC input selection is moved to MOON2 */
        case DRV_AUD_INPUT_ADC0:
            HW_SetReg(reg_ssd_is_pcm,REG_SSD_IS_PCM_V_ADC_);
            /*set ADC switch*/
            regs0->sft_cfg51&=(~0x0F);
            regs0->sft_cfg51|=0x00;
            break;

        case DRV_AUD_INPUT_ADC1:
            HW_SetReg(reg_ssd_is_pcm,REG_SSD_IS_PCM_V_ADC_);
            /*set ADC switch*/
            regs0->sft_cfg51&=(~0x0F);
            regs0->sft_cfg51|=0x05;
            break;

        case DRV_AUD_INPUT_ADC2:
            HW_SetReg(reg_ssd_is_pcm,REG_SSD_IS_PCM_V_ADC_);
            /*set ADC switch*/
            regs0->sft_cfg51&=(~0x0F);
            regs0->sft_cfg51|=0x0A;
            break;

        case DRV_AUD_INPUT_ADC3:
            HW_SetReg(reg_ssd_is_pcm,REG_SSD_IS_PCM_V_PCM_INPUT_IS_FROM_ADC);
            /*set ADC switch*/
            regs0->sft_cfg51&=(~0x0F);
            regs0->sft_cfg51|=0x0F;
            break;

        default:
            ret = DRV_ERR_INVALID_PARAM;
            break;
    }
	#endif
	return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set digital input selection
* @param    eDigitalSource : digital input selection
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetDigitalInput(DRV_AudDigitalInput_t eDigitalSource)
{
	DRV_Status_t ret = DRV_SUCCESS;
    /* Set Digital channel according channel description */
    switch(eDigitalSource)
    {
        case DRV_AUD_INPUT_HDMI_I2S:
            /* Albert, 2008/05/13, marked because I2S/HDMI Mux is moved to B608 */
            //HW_SSDSetReg(REG_Ssd_i2sin_sel, REG_SSD_I2SIN_SEL_VALUE_HDMI);
            HW_SSDSetReg(REG_Ssd_diginsel, REG_SSD_DIGINSEL_VALUE_I2S);

            HW_SetReg(reg_ssd_i2s_sel, REG_SSD_I2S_SEL_V_I2S_INPUT_FROM_HDMI);
            //Set I2S format
            //            HW_SSDSetReg(DAUD_SSD_DIG_IO_CONFIG,7,7,1);
            //Set Digital processing control
            //            HW_SSDSetReg(DAUD_SSD_DIG_IO_CONFIG,7,7,1);
            break;
        case DRV_AUD_INPUT_HDMI_SPDIF:
            HW_SSDSetReg(REG_Ssd_spdifin_sel, REG_SSD_SPDIFIN_SEL_VALUE_HDMI);
            HW_SSDSetReg(REG_Ssd_diginsel, REG_SSD_DIGINSEL_VALUE_SPDIF);

            //Set Digital processing control
            //            HW_SSDSetReg(DAUD_SSD_DIG_IO_CONFIG,7,7,1);
            break;

        case DRV_AUD_INPUT_MPEG_DSP_I2S:
            /* Albert, 2008/05/13, marked because I2S/HDMI Mux is moved to B608 */
            //HW_SSDSetReg(REG_Ssd_i2sin_sel, REG_SSD_I2SIN_SEL_VALUE_EXTERNAL);
            HW_SSDSetReg(REG_Ssd_diginsel, REG_SSD_DIGINSEL_VALUE_I2S);
            HW_SetReg(reg_ssd_i2s_sel, REG_SSD_I2S_SEL_V_I2S_INPUT_FROM_DVD_DSP);
            //Set I2S format
            //            HW_SSDSetReg(DAUD_SSD_DIG_IO_CONFIG,7,7,1);
            //Set Digital processing control
            //            HW_SSDSetReg(DAUD_SSD_DIG_IO_CONFIG,7,7,1);
            break;

        case DRV_AUD_INPUT_EXTERNAL_SPDIF:
            HW_SSDSetReg(REG_Ssd_spdifin_sel, REG_SSD_SPDIFIN_SEL_VALUE_EXTERNAL);
            HW_SSDSetReg(REG_Ssd_diginsel, REG_SSD_DIGINSEL_VALUE_SPDIF);

            //Set Digital processing control
            //            HW_SSDSetReg(DAUD_SSD_DIG_IO_CONFIG,7,7,1);
            break;
        default:
            ret = DRV_ERR_INVALID_PARAM;
            break;
    }
    return ret;
}

/******************************************************************************/
/**
* @brief    Specify SRC(sample rate converter) with source selection
* @param    eSRCPath : SRC path selection
* @param    eSRCSource : SRC source selection
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetSRCPath(DRV_AudSRCPath_t eSRCPath,
                           DRV_AudSRCSource_t eSRCSource)
{
	DRV_Status_t ret = DRV_SUCCESS;

    switch(eSRCPath)
    {
        case DRV_AUD_SRC_1:
            HW_SSDSetReg(REG_Ssd_src1_source, eSRCSource);
            break;

        case DRV_AUD_SRC_2:
            HW_SSDSetReg(REG_Ssd_src2_source, eSRCSource);
            break;

        default:
            ret = DRV_ERR_INVALID_PARAM;
            break;
    }
    return ret;
}

/******************************************************************************/
/**
* @brief    Set source for sound process path
* @param    eMainAUXPath : process path
* @param    eSource : source selection for process path
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetMainAUXPath(DRV_AudProcessPath_t eMainAUXPath,
                               DRV_AudMainAUXSource_t eSource)
{
	DRV_Status_t ret = DRV_SUCCESS;

    switch(eMainAUXPath)
    {
        case DRV_AUD_PROCESS_PATH_MAIN:
            HW_SSDSetReg(REG_Ssd_mainss, eSource);
            break;

        case DRV_AUD_PROCESS_PATH_AUX1:
            HW_SSDSetReg(REG_Ssd_aux1ss, eSource);
            break;

        case DRV_AUD_PROCESS_PATH_AUX2:
            HW_SSDSetReg(REG_Ssd_aux2ss, eSource);
            break;

        case DRV_AUD_PROCESS_PATH_AUX3:
            HW_SSDSetReg(REG_Ssd_aux3ss, eSource);
            break;

        default:
            ret = DRV_ERR_INVALID_PARAM;
            break;
    }
    return ret;
}

/******************************************************************************/
/**
* @brief    Get source for sound process path
* @param    eMainAUXPath : specify process path
* @param    *peRetMainAUXSource : get source selection for main/AUX1/AUX2/AUX3 process path
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudGetMainAUXPath(DRV_AudProcessPath_t eMainSource,
											DRV_AudMainAUXSource_t *peRetMainAUXSource)
{
	DRV_Status_t ret = DRV_SUCCESS;

	if(peRetMainAUXSource == DTV_NULL)
	{
        return DRV_ERR_FAILURE;
	}

    switch(eMainSource)
    {
        case DRV_AUD_PROCESS_PATH_MAIN:
			*peRetMainAUXSource = HW_SSDGetReg(REG_Ssd_mainss);
            break;

        case DRV_AUD_PROCESS_PATH_AUX1:
			*peRetMainAUXSource = HW_SSDGetReg(REG_Ssd_aux1ss);
            break;

        case DRV_AUD_PROCESS_PATH_AUX2:
			*peRetMainAUXSource = HW_SSDGetReg(REG_Ssd_aux2ss);
            break;

        case DRV_AUD_PROCESS_PATH_AUX3:
			*peRetMainAUXSource = HW_SSDGetReg(REG_Ssd_aux3ss);
            break;

        default:
            ret = DRV_ERR_INVALID_PARAM;
            break;
    }

    return ret;
}

/******************************************************************************/
/**
* @brief    Set configuration for center and surround process path
* @param    eCenterSurroundPath : assign center/surround process path
* @param    eSource :  assign source for center/surround process path
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetCenterSurroundPath(DRV_AudProcessPath_t eCenterSurroundPath,
                               DRV_AudCenterSurroundSource_t eSource)
{
	DRV_Status_t ret = DRV_SUCCESS;

    switch(eCenterSurroundPath)
    {
        case DRV_AUD_PROCESS_PATH_CENTER:
            HW_SSDSetReg(REG_Ssd_centerss, eSource);
            break;

        case DRV_AUD_PROCESS_PATH_SURROUND:
            HW_SSDSetReg(REG_Ssd_surroundss, eSource);
            break;

        default:
            ret = DRV_ERR_INVALID_PARAM;
            break;
    }
    return ret;
}

/******************************************************************************/
/**
* @brief    Get source for sound process path
* @param    eCenterSurroundPath : assign center/surround process path
* @param    *peRetCenterSource : get source selection for center/surround process path
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudGetCenterSurroundPath(DRV_AudProcessPath_t eCenterSurroundPath,
													DRV_AudCenterSurroundSource_t *peRetSurroundSource)
{
	DRV_Status_t ret = DRV_SUCCESS;

	if(peRetSurroundSource == DTV_NULL)
	{
        return DRV_ERR_FAILURE;
	}


    switch(eCenterSurroundPath)
    {
        case DRV_AUD_PROCESS_PATH_CENTER:
			*peRetSurroundSource = HW_SSDGetReg(REG_Ssd_centerss);
            break;

        case DRV_AUD_PROCESS_PATH_SURROUND:
			*peRetSurroundSource = HW_SSDGetReg(REG_Ssd_surroundss);
            break;

        default:
            ret = DRV_ERR_INVALID_PARAM;
            break;
    }

    return ret;
}

/******************************************************************************/
/**
* @brief    Enable/disable auto-sleep function of DAC
* @param    fEn : DTV_TRUE : enable DAC auto sleep
*                 DTV_FALSE : disable DAC auto sleep
* @return   DRV_SUCCESS if successful
******************************************************************************/
DRV_Status_t DRV_AudSetDACAutoSleep(Boolean fEn)
{
    if(fEn==DTV_TRUE)
    {
	    //william need to update regs0->sft_cfg50&=(~0x20);
    }
    else
    {
	    //william need to update regs0->sft_cfg50|=0x20;
    }

    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Initial I2S output cross bar
* @param    void
* @return   DRV_SUCCESS if successful
******************************************************************************/
DRV_Status_t DRV_AudI2SOutputCrossbarInit(void)
{
	#if 0
    DRV_Status_t ret = DRV_SUCCESS;

    /*set audio master clock output selection*/
    HW_SetReg(reg_fsaradc_global_amclksel, REG_FSARADC_GLOBAL_AMCLKSEL_V_256FS);

    /*set audio WS output selection*/
    HW_SetReg(reg_fsaradc_global_awssel, REG_FSARADC_GLOBAL_AWSSEL_V_SSD_I2S_WS_OUTPUT);

    /*set audio bit clock output as not inverted*/
//    HW_SetReg(REG_Ctrl_abclksel_inv, REG_CTRL_ABCLKSEL_INV_VALUE_OUTPUT_CLOCK_NOT_INVERTED);

    /*set audio bit clock output selction */
    HW_SetReg(reg_fsaradc_global_abclksel, REG_FSARADC_GLOBAL_ABCLKSEL_V_SSD_I2S_BIT_CLOCK_OUTPUT);

    HW_SetReg(reg_ssd_os_i2s_clk_s, REG_SSD_OS_I2S_CLK_S_V_SSD_);
//    HW_SetReg(reg_ssd_os_dac_clk_s, REG_SSD_OS_DAC_CLK_S_V_SSD_);
    HW_SetReg(reg_ssd_os_sd0, REG_SSD_OS_SD0_V_SSD_SD0_);
    HW_SetReg(reg_ssd_os_sd1, REG_SSD_OS_SD1_V_SSD_SD1_);
    HW_SetReg(reg_ssd_os_sd2, REG_SSD_OS_SD2_V_SSD_SD2_);

#ifdef SPV9107_DEVELOP_VERSION

#else
    /*set source of I2S PAD*/
    HW_SetReg(REG_Ctrl_os_i2s_clk_s, REG_CTRL_OS_I2S_CLK_S_VALUE_I2S_PAD_S_BIT_CLOCK_AND_WS_IS_FROM_SSD);
#endif
    /*set pin mux control to I2S select*/
//    (//william need to update regs0->sft_cfg0)|=0x8000;

    return ret;
#else
    return DRV_SUCCESS;
#endif
}

#ifdef NEW_OUTPUT_CONFIG_METHOD
/******************************************************************************/
/**
* @brief    Set I2S output format configuration
* @param    fEnBitClockInvert : DTV_TRUE : enable bit clock inverse
*                           	DTV_FALSE : disable bit clock inverse
* @param    eOutPut : select SSD output channel to change I2S format,
* @param    eFormat : I2S format selection
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
* @note     SSD SD0 will output via DAC0 or I2S SD0
* @note     SSD SD1 will output via DAC1 or I2S SD1
* @note     SSD SD2 will output via DAC2 or I2S SD2
* @note     DRV_AUD_OUTPUT_I2S_SD0 and DRV_AUD_OUTPUT_I2S_SD1 will be the same format
******************************************************************************/
DRV_Status_t DRV_AudSetI2SOutputFormat(Boolean fEnBitClockInvert,
												DRV_AudOutputPath_t eOutPut,
												DRV_AudI2SOutputFormat_t eFormat)
{
    if(fEnBitClockInvert==DTV_TRUE)
    {
        HW_SetReg(reg_fsaradc_global_abclksel_inv, REG_FSARADC_GLOBAL_ABCLKSEL_INV_V_MAX);
    }
    else
    {
		HW_SetReg(reg_fsaradc_global_abclksel_inv, REG_FSARADC_GLOBAL_ABCLKSEL_INV_V_MIN);
    }

	switch(eOutPut)
	{
		case DRV_AUD_OUTPUT_DAC0:
		case DRV_AUD_OUTPUT_DAC1:
		case DRV_AUD_OUTPUT_I2S_SD0:
		case DRV_AUD_OUTPUT_I2S_SD1:
			if(eFormat == DRV_AUD_I2S_OUTPUT_PHILIPS24BIT)
			{
				HW_SSDSetReg(REG_Ssd_i2sout_format_dac, REG_SSD_I2SOUT_FORMAT_DAC_VALUE_PHILIPS);
			}
			else if(eFormat == DRV_AUD_I2S_OUTPUT_SONY24BIT)
			{
				HW_SSDSetReg(REG_Ssd_i2sout_format_dac, REG_SSD_I2SOUT_FORMAT_DAC_VALUE_SONY);
			}
			else if(eFormat == DRV_AUD_I2S_OUTPUT_JAPANESE24BIT)
			{
				HW_SSDSetReg(REG_Ssd_i2sout_format_dac, REG_SSD_I2SOUT_FORMAT_DAC_VALUE_JAPANESE_24_BIT);
			}
			break;

        case DRV_AUD_OUTPUT_I2S_SD2:
		case DRV_AUD_OUTPUT_DAC2:
			if(eFormat == DRV_AUD_I2S_OUTPUT_PHILIPS24BIT)
			{
				HW_SSDSetReg(REG_Ssd_i2sout_format_ext, REG_SSD_I2SOUT_FORMAT_EXT_VALUE_PHILIPS);
			}
			else if(eFormat == DRV_AUD_I2S_OUTPUT_SONY24BIT)
			{
				HW_SSDSetReg(REG_Ssd_i2sout_format_ext, REG_SSD_I2SOUT_FORMAT_EXT_VALUE_SONY);
			}
			else if(eFormat == DRV_AUD_I2S_OUTPUT_JAPANESE24BIT)
			{
				HW_SSDSetReg(REG_Ssd_i2sout_format_ext, REG_SSD_I2SOUT_FORMAT_EXT_VALUE_JAPANESE_24_BIT);
			}
			break;

		default :
            return DRV_ERR_INVALID_PARAM;
			break;

	}
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Config audio output corss bar
* @param    eSource : specify which source input to crossbar
* @param    eDestination : specify channel to output audio data
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetOutputCrossbasrConfig(DRV_AudOutputCrossbarSource_t eSource,
                                                        DRV_AudOutputPath_t eDestination)
{
	#if 0
	switch(eDestination)
	{
	    case DRV_AUD_OUTPUT_DAC0:
			HW_SetReg(reg_ssd_os_dac0,eSource);
			break;

	    case DRV_AUD_OUTPUT_DAC1:
			HW_SetReg(reg_ssd_os_dac1,eSource);
			break;

	    case DRV_AUD_OUTPUT_DAC2:
			HW_SetReg(reg_ssd_os_dac2,eSource);
			break;

	    case DRV_AUD_OUTPUT_I2S_SD0:
			HW_SetReg(reg_ssd_os_sd0,eSource);
			break;

	    case DRV_AUD_OUTPUT_I2S_SD1:
			HW_SetReg(reg_ssd_os_sd1,eSource);
			break;

	    case DRV_AUD_OUTPUT_I2S_SD2:
			HW_SetReg(reg_ssd_os_sd2,eSource);
			break;

	    case DRV_AUD_OUTPUT_SPDIF:
			if(eSource == DRV_AUD_SPDIF_EXTERNAL_INPUT)
			{
				HW_SetReg(reg_ssd_os_spdif,REG_SSD_OS_SPDIF_V_SPDIF_);
			}
			else if(eSource == DRV_AUD_SPDIF_HDMI_OUTPUT)
			{
				HW_SetReg(reg_ssd_os_spdif,REG_SSD_OS_SPDIF_V_HDMI_);
			}
			else if(eSource == DRV_AUD_SPDIF_SSD_OUTPUT)
			{
				HW_SetReg(reg_ssd_os_spdif,REG_SSD_OS_SPDIF_V_SSD_);
			}
			else if (eSource == DRV_AUD_SPDIF_DVD_DSP_OUTPUT)
			{
				HW_SetReg(reg_ssd_os_spdif,REG_SSD_OS_SPDIF_V_DSP_);
			}
            else
            {
                return DRV_ERR_INVALID_PARAM;
            }
			break;

        default:
                return DRV_ERR_INVALID_PARAM;
            break;
	}
	#endif
    return DRV_SUCCESS;
}
#if 1
/******************************************************************************/
/**
* @brief    Set output path configuration
* @param    eCrossbarSource : SSD ouput port selection
* @param    eOutputLeftChannelSource : source for output left channel
* @param    eOutputRightChannelSource : source for output right channel
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetOutputConfig(DRV_AudOutputCrossbarSource_t eCrossbarSource,
                                                DRV_AudSSDOutputSource_t eOutputLeftChannelSource,
                                                DRV_AudSSDOutputSource_t eOutputRightChannelSource)
{
	DRV_Status_t ret = DRV_SUCCESS;

    switch(eCrossbarSource)
    {
        case DRV_AUD_I2S_SSD_OUTPUT_SD0:
            /* assign SSD SD0 output */
            HW_SSDSetReg(REG_Ssd_asdac1l, eOutputLeftChannelSource);
            HW_SSDSetReg(REG_Ssd_asdac1r, eOutputRightChannelSource);

            /* set I2S output crossbar */
//            HW_SetReg(reg_ssd_os_dac0, REG_SSD_OS_DAC0_V_SD0_);

            /* set I2S output crossbar, SSD SD0 output via DAC0 */
//            DRV_AudSetOutputCrossbasrConfig(DRV_AUD_I2S_SSD_OUTPUT_SD0,DRV_AUD_OUTPUT_DAC0);
            break;

        case DRV_AUD_I2S_SSD_OUTPUT_SD1:
            /* assign SSD SD1 output */
            HW_SSDSetReg(REG_Ssd_asdac2l, eOutputLeftChannelSource);
            HW_SSDSetReg(REG_Ssd_asdac2r, eOutputRightChannelSource);

            /* set I2S output crossbar */
//            HW_SetReg(reg_ssd_os_dac1, REG_SSD_OS_DAC1_V_SD1_);

            /* set I2S output crossbar, SSD SD1 output via DAC1 */
//            DRV_AudSetOutputCrossbasrConfig(DRV_AUD_I2S_SSD_OUTPUT_SD1,DRV_AUD_OUTPUT_DAC1);
            break;

        case DRV_AUD_I2S_SSD_OUTPUT_SD2:
            /* assign SSD SD2 output */
            HW_SSDSetReg(REG_Ssd_asi2s1l, eOutputLeftChannelSource);
            HW_SSDSetReg(REG_Ssd_asi2s1r, eOutputRightChannelSource);

            /* set I2S output crossbar */
//            HW_SetReg(reg_ssd_os_dac2, REG_SSD_OS_SD0_V_SSD_SD2_);

            /* set I2S output crossbar, SSD SD2 output via DAC2 */
//            DRV_AudSetOutputCrossbasrConfig(DRV_AUD_I2S_SSD_OUTPUT_SD2,DRV_AUD_OUTPUT_DAC2);
            break;

        case DRV_AUD_SPDIF_SSD_OUTPUT:
            /* assign SSD SPDIF output */
            HW_SSDSetReg(REG_Ssd_spdif1l, eOutputLeftChannelSource);
            HW_SSDSetReg(REG_Ssd_spdif1r, eOutputRightChannelSource);

            /* set SPDIF output crossbar */
//            HW_SetReg(reg_ssd_os_spdif, REG_SSD_OS_SPDIF_V_SSD_);
//            DRV_AudSetOutputCrossbasrConfig(DRV_AUD_SPDIF_SSD_OUTPUT,DRV_AUD_OUTPUT_SPDIF);
            break;

        default:
            ret = DRV_ERR_INVALID_PARAM;
            break;
    }
    return ret;
}
#else
/******************************************************************************/
/**
* @brief    Set output path configuration
* @param    eOutputChannel : ouput channel selection
* @param    eOutputLeftChannelSource : source for output left channel
* @param    eOutputRightChannelSource : source for output right channel
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
* @note     SSD SD0 will output via DAC0 or I2S SD0
* @note     SSD SD1 will output via DAC1 or I2S SD1
* @note     SSD SD2 will output via DAC2 or I2S SD2
******************************************************************************/
DRV_Status_t DRV_AudSetOutputConfig(DRV_AudOutputPath_t eOutputChannel,
                                                DRV_AudSSDOutputSource_t eOutputLeftChannelSource,
                                                DRV_AudSSDOutputSource_t eOutputRightChannelSource)
{
	DRV_Status_t ret = DRV_SUCCESS;

    switch(eOutputChannel)
    {
        case DRV_AUD_OUTPUT_DAC0:
            /* assign SSD SD0 output */
            HW_SSDSetReg(REG_Ssd_asdac1l, eOutputLeftChannelSource);
            HW_SSDSetReg(REG_Ssd_asdac1r, eOutputRightChannelSource);

            /* set I2S output crossbar */
//            HW_SetReg(reg_ssd_os_dac0, REG_SSD_OS_DAC0_V_SD0_);

            /* set I2S output crossbar, SSD SD0 output via DAC0 */
//            DRV_AudSetOutputCrossbasrConfig(DRV_AUD_I2S_SSD_OUTPUT_SD0,DRV_AUD_OUTPUT_DAC0);
            break;

        case DRV_AUD_OUTPUT_DAC1:
            /* assign SSD SD1 output */
            HW_SSDSetReg(REG_Ssd_asdac2l, eOutputLeftChannelSource);
            HW_SSDSetReg(REG_Ssd_asdac2r, eOutputRightChannelSource);

            /* set I2S output crossbar */
//            HW_SetReg(reg_ssd_os_dac1, REG_SSD_OS_DAC1_V_SD1_);

            /* set I2S output crossbar, SSD SD1 output via DAC1 */
//            DRV_AudSetOutputCrossbasrConfig(DRV_AUD_I2S_SSD_OUTPUT_SD1,DRV_AUD_OUTPUT_DAC1);
            break;

        case DRV_AUD_OUTPUT_DAC2:
            /* assign SSD SD2 output */
            HW_SSDSetReg(REG_Ssd_asi2s1l, eOutputLeftChannelSource);
            HW_SSDSetReg(REG_Ssd_asi2s1r, eOutputRightChannelSource);

            /* set I2S output crossbar */
//            HW_SetReg(reg_ssd_os_dac2, REG_SSD_OS_SD0_V_SSD_SD2_);

            /* set I2S output crossbar, SSD SD2 output via DAC2 */
//            DRV_AudSetOutputCrossbasrConfig(DRV_AUD_I2S_SSD_OUTPUT_SD2,DRV_AUD_OUTPUT_DAC2);
            break;

        case DRV_AUD_OUTPUT_I2S_SD0:
            /* assign SSD SD0 output */
            HW_SSDSetReg(REG_Ssd_asdac1l, eOutputLeftChannelSource);
            HW_SSDSetReg(REG_Ssd_asdac1r, eOutputRightChannelSource);

            /* set I2S output crossbar */
//            HW_SetReg(reg_ssd_os_sd0, REG_SSD_OS_SD0_V_SSD_SD0_);

            /* set I2S output crossbar, SSD SD0 output via I2S SD0 */
//            DRV_AudSetOutputCrossbasrConfig(DRV_AUD_I2S_SSD_OUTPUT_SD0,DRV_AUD_OUTPUT_I2S_SD0);
            break;

        case DRV_AUD_OUTPUT_I2S_SD1:
            /* assign SSD SD1 output */
            HW_SSDSetReg(REG_Ssd_asdac2l, eOutputLeftChannelSource);
            HW_SSDSetReg(REG_Ssd_asdac2r, eOutputRightChannelSource);

            /* set I2S output crossbar */
//            HW_SetReg(reg_ssd_os_sd1, REG_SSD_OS_SD0_V_SSD_SD1_);

            /* set I2S output crossbar, SSD SD1 output via I2S SD1 */
//            DRV_AudSetOutputCrossbasrConfig(DRV_AUD_I2S_SSD_OUTPUT_SD1,DRV_AUD_OUTPUT_I2S_SD1);
            break;

        case DRV_AUD_OUTPUT_I2S_SD2:
            /* assign SSD SD2 output */
            HW_SSDSetReg(REG_Ssd_asi2s1l, eOutputLeftChannelSource);
            HW_SSDSetReg(REG_Ssd_asi2s1r, eOutputRightChannelSource);

            /* set I2S output crossbar */
//            HW_SetReg(reg_ssd_os_sd2, REG_SSD_OS_SD0_V_SSD_SD2_);

            /* set I2S output crossbar, SSD SD2 output via I2S SD2 */
//            DRV_AudSetOutputCrossbasrConfig(DRV_AUD_I2S_SSD_OUTPUT_SD2,DRV_AUD_OUTPUT_I2S_SD2);
            break;

        case DRV_AUD_OUTPUT_SPDIF:
            /* assign SSD SPDIF output */
            HW_SSDSetReg(REG_Ssd_spdif1l, eOutputLeftChannelSource);
            HW_SSDSetReg(REG_Ssd_spdif1r, eOutputRightChannelSource);

            /* set SPDIF output crossbar */
//            HW_SetReg(reg_ssd_os_spdif, REG_SSD_OS_SPDIF_V_SSD_);
//            DRV_AudSetOutputCrossbasrConfig(DRV_AUD_SPDIF_SSD_OUTPUT,DRV_AUD_OUTPUT_SPDIF);
            break;

        default:
            ret = DRV_ERR_INVALID_PARAM;
            break;
    }
    return ret;
}
#endif

#else
/******************************************************************************/
/**
* @brief    Set I2S output format configuration
* @param    fEnBitClockInvert : DTV_TRUE : enable bit clock inverse
*                           	DTV_FALSE : disable bit clock inverse
* @param    eOutPut : select SSD output channel to change I2S format,
* @param    eFormat : I2S format selection
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
* @note     SSD SD0 will output via DAC0 or I2S SD0
* @note     SSD SD1 will output via DAC1 or I2S SD1
* @note     SSD SD2 will output via DAC2 or I2S SD2
* @note     DRV_AUD_OUTPUT_I2S_SD0 and DRV_AUD_OUTPUT_I2S_SD1 will be the same format
******************************************************************************/
DRV_Status_t DRV_AudSetI2SOutputFormat(Boolean fEnBitClockInvert,
												DRV_AudSSDOutputPath_t eOutPut,
												DRV_AudI2SOutputFormat_t eFormat)
{
    if(fEnBitClockInvert==DTV_TRUE)
    {
        HW_SetReg(reg_fsaradc_global_abclksel_inv, REG_FSARADC_GLOBAL_ABCLKSEL_INV_V_OUTPUT_CLOCK_INVERTED);
    }
    else
    {
		HW_SetReg(reg_fsaradc_global_abclksel_inv, REG_FSARADC_GLOBAL_ABCLKSEL_INV_V_OUTPUT_CLOCK_NOT_INVERTED);
    }

	switch(eOutPut)
	{
		case DRV_AUD_OUTPUT_DAC0:
		case DRV_AUD_OUTPUT_DAC1:
		case DRV_AUD_OUTPUT_I2S_SD0:
		case DRV_AUD_OUTPUT_I2S_SD1:
			if(eFormat == DRV_AUD_I2S_OUTPUT_PHILIPS24BIT)
			{
				HW_SSDSetReg(REG_Ssd_i2sout_format_dac, REG_SSD_I2SOUT_FORMAT_DAC_VALUE_PHILIPS);
			}
			else if(eFormat == DRV_AUD_I2S_OUTPUT_SONY24BIT)
			{
				HW_SSDSetReg(REG_Ssd_i2sout_format_dac, REG_SSD_I2SOUT_FORMAT_DAC_VALUE_SONY);
			}
			else if(eFormat == DRV_AUD_I2S_OUTPUT_JAPANESE24BIT)
			{
				HW_SSDSetReg(REG_Ssd_i2sout_format_dac, REG_SSD_I2SOUT_FORMAT_DAC_VALUE_JAPANESE_24_BIT);
			}
			break;

        case DRV_AUD_OUTPUT_I2S_SD2:
		case DRV_AUD_OUTPUT_DAC2:
			if(eFormat == DRV_AUD_I2S_OUTPUT_PHILIPS24BIT)
			{
				HW_SSDSetReg(REG_Ssd_i2sout_format_ext, REG_SSD_I2SOUT_FORMAT_EXT_VALUE_PHILIPS);
			}
			else if(eFormat == DRV_AUD_I2S_OUTPUT_SONY24BIT)
			{
				HW_SSDSetReg(REG_Ssd_i2sout_format_ext, REG_SSD_I2SOUT_FORMAT_EXT_VALUE_SONY);
			}
			else if(eFormat == DRV_AUD_I2S_OUTPUT_JAPANESE24BIT)
			{
				HW_SSDSetReg(REG_Ssd_i2sout_format_ext, REG_SSD_I2SOUT_FORMAT_EXT_VALUE_JAPANESE_24_BIT);
			}
			break;

		default :
            return DRV_ERR_INVALID_PARAM;
			break;

	}
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Config audio output corss bar
* @param    eSource : specify which source input to crossbar
* @param    eDestination : specify channel to output audio data
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetOutputCrossbasrConfig(DRV_AudOutputCrossbarSource_t eSource,
                                                        DRV_AudOutputCrossbarDestination_t eDestination)
{
	switch(eDestination)
	{
	    case DRV_AUD_OUTPUT_DAC0:
			HW_SetReg(reg_ssd_os_dac0,eSource);
			break;

	    case DRV_AUD_OUTPUT_DAC1:
			HW_SetReg(reg_ssd_os_dac1,eSource);
			break;

	    case DRV_AUD_OUTPUT_DAC2:
			HW_SetReg(reg_ssd_os_dac2,eSource);
			break;

	    case DRV_AUD_OUTPUT_I2S_SD0:
			HW_SetReg(reg_ssd_os_sd0,eSource);
			break;

	    case DRV_AUD_OUTPUT_I2S_SD1:
			HW_SetReg(reg_ssd_os_sd1,eSource);
			break;

	    case DRV_AUD_OUTPUT_I2S_SD2:
			HW_SetReg(reg_ssd_os_sd2,eSource);
			break;

	    case DRV_AUD_OUTPUT_SPDIF:
			if(eSource == DRV_AUD_SPDIF_EXTERNAL_INPUT)
			{
				HW_SetReg(reg_ssd_os_spdif,REG_SSD_OS_SPDIF_V_SPDIF_);
			}
			else if(eSource == DRV_AUD_SPDIF_HDMI_OUTPUT)
			{
				HW_SetReg(reg_ssd_os_spdif,REG_SSD_OS_SPDIF_V_HDMI_);
			}
			else if(eSource == DRV_AUD_SPDIF_SSD_OUTPUT)
			{
				HW_SetReg(reg_ssd_os_spdif,REG_SSD_OS_SPDIF_V_SSD_);
			}
			else if (eSource == DRV_AUD_SPDIF_DVD_DSP_OUTPUT)
			{
				HW_SetReg(reg_ssd_os_spdif,REG_SSD_OS_SPDIF_V_DSP_);
			}
            else
            {
                return DRV_ERR_INVALID_PARAM;
            }
			break;

        default:
                return DRV_ERR_INVALID_PARAM;
            break;
	}
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set output path configuration
* @param    eOutputChannel : ouput channel selection
* @param    eOutputLeftChannelSource : source for output left channel
* @param    eOutputRightChannelSource : source for output right channel
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetOutputPath(DRV_AudSSDOutputPath_t eOutputChannel,
                                            DRV_AudSSDOutputSource_t eOutputLeftChannelSource,
                                            DRV_AudSSDOutputSource_t eOutputRightChannelSource)
{
	DRV_Status_t ret = DRV_SUCCESS;

    switch(eOutputChannel)
    {
        case DRV_AUD_SSD_OUTPUT_DAC0:
            /* need to check for SPIN2 and 9100 because RL swap bugs */
            HW_SSDSetReg(REG_Ssd_asdac1l, eOutputLeftChannelSource);
            HW_SSDSetReg(REG_Ssd_asdac1r, eOutputRightChannelSource);
            /* set I2S output crossbar */
            HW_SetReg(reg_ssd_os_dac0, REG_SSD_OS_DAC0_V_SD0_);
            break;

        case DRV_AUD_SSD_OUTPUT_DAC1:
            /* need to check for SPIN2 and 9100 because RL swap bugs */
            HW_SSDSetReg(REG_Ssd_asdac2l, eOutputLeftChannelSource);
            HW_SSDSetReg(REG_Ssd_asdac2r, eOutputRightChannelSource);
            /* set I2S output crossbar */
            HW_SetReg(reg_ssd_os_dac1, REG_SSD_OS_DAC1_V_SD1_);
            break;

        case DRV_AUD_SSD_OUTPUT_DAC2:
            /* need to check for SPIN2 and 9100 because RL swap bugs */
            HW_SSDSetReg(REG_Ssd_asi2s1l, eOutputLeftChannelSource);
            HW_SSDSetReg(REG_Ssd_asi2s1r, eOutputRightChannelSource);
            /* set I2S output crossbar */
            HW_SetReg(reg_ssd_os_dac2, REG_SSD_OS_SD0_V_SSD_SD2_);

        case DRV_AUD_SSD_OUTPUT_I2S0:
            /* need to check for SPIN2 and 9100 because RL swap bugs */
            HW_SSDSetReg(REG_Ssd_asdac1l, eOutputLeftChannelSource);
            HW_SSDSetReg(REG_Ssd_asdac1r, eOutputRightChannelSource);
            /* set I2S output crossbar */
            HW_SetReg(reg_ssd_os_sd0, REG_SSD_OS_SD0_V_SSD_SD0_);
            break;

        case DRV_AUD_SSD_OUTPUT_I2S1:
            /* need to check for SPIN2 and 9100 because RL swap bugs */
            HW_SSDSetReg(REG_Ssd_asdac2l, eOutputLeftChannelSource);
            HW_SSDSetReg(REG_Ssd_asdac2r, eOutputRightChannelSource);
            /* set I2S output crossbar */
            HW_SetReg(reg_ssd_os_sd1, REG_SSD_OS_SD0_V_SSD_SD1_);
            break;

        case DRV_AUD_SSD_OUTPUT_I2S2:
            /* need to check for SPIN2 and 9100 because RL swap bugs */
            HW_SSDSetReg(REG_Ssd_asi2s1l, eOutputLeftChannelSource);
            HW_SSDSetReg(REG_Ssd_asi2s1r, eOutputRightChannelSource);
            /* set I2S output crossbar */
            HW_SetReg(reg_ssd_os_sd2, REG_SSD_OS_SD0_V_SSD_SD2_);
            break;

        case DRV_AUD_SSD_OUTPUT_SPDIF:
            /* need to check for SPIN2 and 9100 because RL swap bugs */
            HW_SSDSetReg(REG_Ssd_spdif1l, eOutputLeftChannelSource);
            HW_SSDSetReg(REG_Ssd_spdif1r, eOutputRightChannelSource);
            /* set SPDIF output crossbar */
            HW_SetReg(reg_ssd_os_spdif, REG_SSD_OS_SPDIF_V_SSD_);
            break;

        default:
            ret = DRV_ERR_INVALID_PARAM;
            break;
    }
    return ret;
}
#endif

/******************************************************************************/
/**
* @brief    Enable/disable SSD SPDIF bypass
* @param    fEn : DTV_TRUE : enable bypass SPDIF,
*                 DTV_FALSE : disable bypass SPDIF
* @return   DRV_SUCCESS if successful
******************************************************************************/
DRV_Status_t DRV_AudSetSPDIFPathBypass(Boolean fEn)
{
    if(fEn==DTV_TRUE)
    {
        HW_SSDSetReg(REG_Ssd_spdifin_bypass,REG_SSD_SPDIFIN_BYPASS_VALUE_ENABLE);
    }
    else
    {
        HW_SSDSetReg(REG_Ssd_spdifin_bypass,REG_SSD_SPDIFIN_BYPASS_VALUE_DISABLE);
    }
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Get if SSD bypass SPDIF input
* @param    fRetEn : DTV_TRUE : SPDIF bypass is enable,
*                    DTV_FALSE : SPDIF bypass is disable,
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudGetIfSPDIFPathBypass(Boolean* fRetEn)
{
    UINT8 IfBypass = HW_SSDGetReg(REG_Ssd_spdifin_bypass);

    if(fRetEn == DTV_NULL)
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        if(IfBypass==REG_SSD_SPDIFIN_BYPASS_VALUE_ENABLE)
        {
            *fRetEn = DTV_TRUE;
        }
        else
        {
            *fRetEn = DTV_FALSE;
        }
        return DRV_SUCCESS;
    }
}

/******************************************************************************/
/**
* @brief    Enable/disable SSD I2S bypass
* @param    fEn : DTV_TRUE : enable bypass I2S,
*                 DTV_FALSE : disable bypass I2S
* @return   DRV_SUCCESS if successful
******************************************************************************/
DRV_Status_t DRV_AudSetI2SPathBypass(Boolean fEn)
{
    if(fEn==DTV_TRUE)
    {
        HW_SSDSetReg(REG_Ssd_i2sin_bypass,REG_SSD_I2SIN_BYPASS_VALUE_ENABLE);
    }
    else
    {
        HW_SSDSetReg(REG_Ssd_i2sin_bypass,REG_SSD_I2SIN_BYPASS_VALUE_ENABLE);
    }
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set main path mixer configuration
* @param    eMainPathMixer : main path mixer selection
* @return   DRV_SUCCESS if successful
******************************************************************************/
DRV_Status_t DRV_AudSetMainPathMixer(DRV_AudMainPathMixer_t eMainPathMixer)
{
	HW_SSDCheckRegRange(REG_Ssd_mixersel,eMainPathMixer);
    HW_SSDSetReg(REG_Ssd_mixersel,eMainPathMixer);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Enable/disable DEMDEC mute
* @param    fEn : DTV_TRUE : Mute On
*                 DTV_FALSE : Mute Off
* @return   DRV_SUCCESS if successful
******************************************************************************/
DRV_Status_t DRV_AudSetDEMDECMute(Boolean fEn)
{
    if(fEn==DTV_TRUE)
    {
        HW_SSDSetReg(REG_Ssd_ddmute, REG_SSD_DDMUTE_VALUE_MUTE);
    }
    else
    {
        HW_SSDSetReg(REG_Ssd_ddmute, REG_SSD_DDMUTE_VALUE_NO_MUTE);
    }
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Get if DEMDEC output mute
* @param    pfRetMute : DTV_TRUE : Mute On
*                       DTV_FALSE : Mute Off
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudGetDEMDECMute(Boolean *pfRetMute)
{
    UINT8 mute = HW_SSDGetReg(REG_Ssd_ddmute);

    if(pfRetMute == DTV_NULL)
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        if(mute==REG_SSD_DDMUTE_VALUE_MUTE)
        {
            *pfRetMute = DTV_TRUE;
        }
        else
        {
            *pfRetMute = DTV_FALSE;
        }
        return DRV_SUCCESS;
    }
}

/******************************************************************************/
/**
* @brief    Mute assigned process path
* @param    eMutePath :  specify process path for mute control
* @param    fEn : DTV_TRUE : mute selected path
*                 DTV_FALSE : unmute selected path
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetMutePath(DRV_AudMutePath_t eMutePath,
                                        Boolean fEn)
{
	DRV_Status_t ret = DRV_SUCCESS;

    switch(eMutePath)
    {
        case DRV_AUD_MUTE_PATH_ALL:
            if(fEn==DTV_TRUE)
            {
                HW_SSDSetBit(REG_Ssd_mainmut_OFFSET,
                             0x1FF,
                             REG_Ssd_mainmut_BIT_IDX,
                             0x1FF);
            }
            else
            {
                HW_SSDSetBit(REG_Ssd_mainmut_OFFSET,
                             0x1FF,
                             REG_Ssd_mainmut_BIT_IDX,
                             0x0);
            }
            break;

        case DRV_AUD_MUTE_PATH_MAIN:
            if(fEn==DTV_TRUE)
            {
                HW_SSDSetReg(REG_Ssd_mainmut, REG_SSD_MAINMUT_VALUE_ON);
            }
            else
            {
                HW_SSDSetReg(REG_Ssd_mainmut, REG_SSD_MAINMUT_VALUE_OFF);
            }
            break;

        case DRV_AUD_MUTE_PATH_MAIN_LEFT:
            if(fEn==DTV_TRUE)
            {
                HW_SSDSetReg(REG_Ssd_mainlmut, REG_SSD_MAINLMUT_VALUE_ON);
            }
            else
            {
                HW_SSDSetReg(REG_Ssd_mainlmut, REG_SSD_MAINLMUT_VALUE_OFF);
            }
            break;

        case DRV_AUD_MUTE_PATH_MAIN_RIGHT:
            if(fEn==DTV_TRUE)
            {
                HW_SSDSetReg(REG_Ssd_mainrmut, REG_SSD_MAINRMUT_VALUE_ON);
            }
            else
            {
                HW_SSDSetReg(REG_Ssd_mainrmut, REG_SSD_MAINRMUT_VALUE_OFF);
            }
            break;

        case DRV_AUD_MUTE_PATH_SUBWOOFER:
            if(fEn==DTV_TRUE)
            {
                HW_SSDSetReg(REG_Ssd_subwmut, REG_SSD_SUBWMUT_VALUE_ON);
            }
            else
            {
                HW_SSDSetReg(REG_Ssd_subwmut, REG_SSD_SUBWMUT_VALUE_OFF);
            }
            break;

        case DRV_AUD_MUTE_PATH_CENTER:
            if(fEn==DTV_TRUE)
            {
                HW_SSDSetReg(REG_Ssd_centermut, REG_SSD_CENTERMUT_VALUE_ON);
            }
            else
            {
                HW_SSDSetReg(REG_Ssd_centermut, REG_SSD_CENTERMUT_VALUE_OFF);
            }
            break;

        case DRV_AUD_MUTE_PATH_SURROUND:
            if(fEn==DTV_TRUE)
            {
                HW_SSDSetReg(REG_Ssd_surroundmut, REG_SSD_SURROUNDMUT_VALUE_ON);
            }
            else
            {
                HW_SSDSetReg(REG_Ssd_surroundmut, REG_SSD_SURROUNDMUT_VALUE_OFF);
            }
            break;

        case DRV_AUD_MUTE_PATH_AUX1:
            if(fEn==DTV_TRUE)
            {
                HW_SSDSetReg(REG_Ssd_aux1mut, REG_SSD_AUX1MUT_VALUE_ON);
            }
            else
            {
                HW_SSDSetReg(REG_Ssd_aux1mut, REG_SSD_AUX1MUT_VALUE_OFF);
            }
            break;

        case DRV_AUD_MUTE_PATH_AUX2:
            if(fEn==DTV_TRUE)
            {
                HW_SSDSetReg(REG_Ssd_aux2mut, REG_SSD_AUX2MUT_VALUE_ON);
            }
            else
            {
                HW_SSDSetReg(REG_Ssd_aux2mut, REG_SSD_AUX2MUT_VALUE_OFF);
            }
            break;

        case DRV_AUD_MUTE_PATH_AUX3:
            if(fEn==DTV_TRUE)
            {
                HW_SSDSetReg(REG_Ssd_aux3mut, REG_SSD_AUX3MUT_VALUE_ON);
            }
            else
            {
                HW_SSDSetReg(REG_Ssd_aux3mut, REG_SSD_AUX3MUT_VALUE_OFF);
            }
            break;
        default:
            ret = DRV_ERR_INVALID_PARAM;
            break;
    }
    return ret;
}
/******************************************************************************/
/**
* @brief    Set Master Volume
* @param    wMasterVolume : -672(mute) ~ 192 (+192 dB),(1 step is 0.125 dB), controls MAIN, SW, C and S in 1/8dB steps
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetMasterVolume(INT16 wMasterVolume)
{
	HW_SSDCheckRegRange(REG_Ssd_mastervol,wMasterVolume);
    HW_SSDSetReg(REG_Ssd_mastervol, wMasterVolume);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Get master volume value
* @param    pwRetMasterVolume : return master volume value
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudGetMasterVolume(INT16 *pwRetMasterVolume)
{
    if(pwRetMasterVolume == DTV_NULL)
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        HW_SSDSignExpand(REG_Ssd_mastervol,*pwRetMasterVolume);
        return DRV_SUCCESS;
    }
}

/******************************************************************************/
/**
* @brief    Set main volume values
* @param    bLeftMainVolume : -84(mute) ~ 24(+24 dB),(1 step is 1 dB)
* @param    bRightMainVolume : -84(mute) ~ 24(+24 dB),(1 step is 1 dB)
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetMainVolume(INT8 bLeftMainVolume,INT8 bRightMainVolume)
{
	HW_SSDCheckRegRange(REG_Ssd_mainvoll,bLeftMainVolume);
	HW_SSDCheckRegRange(REG_Ssd_mainvolr,bRightMainVolume);
    HW_SSDSetReg(REG_Ssd_mainvoll, bLeftMainVolume);
    HW_SSDSetReg(REG_Ssd_mainvolr, bRightMainVolume);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Get main volume values
* @param    pbRetLeftMainVolume : return main left channel volume value
* @param    pbRetRightMainVolume : return main right channel volume value
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudGetMainVolume(INT8 *pbRetLeftMainVolume,INT8 *pbRetRightMainVolume)
{
    if((pbRetLeftMainVolume == DTV_NULL)
        ||(pbRetRightMainVolume == DTV_NULL))
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        HW_SSDSignExpand(REG_Ssd_mainvoll,*pbRetLeftMainVolume);
        HW_SSDSignExpand(REG_Ssd_mainvolr,*pbRetRightMainVolume);
        return DRV_SUCCESS;
    }
}

/******************************************************************************/
/**
* @brief    Set subwoofer path volume
* @param    bSubwooferVolume : -84(mute) ~ 24(+24 dB),(1 step is 1 dB)
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetSubwooferVolume(INT8 bSubwooferVolume)
{
	HW_SSDCheckRegRange(REG_Ssd_subwvol,bSubwooferVolume);
    HW_SSDSetReg(REG_Ssd_subwvol, bSubwooferVolume);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set center path volume
* @param    bCenterVolume : -84(mute) ~ 24(+24 dB),(1 step is 1 dB)
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetCenterVolume(INT8 bCenterVolume)
{
	HW_SSDCheckRegRange(REG_Ssd_centervol, bCenterVolume);
	HW_SSDSetReg(REG_Ssd_centervol, bCenterVolume);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set surround path volume
* @param    bSurroundVolume : -84(mute) ~ 24(+24 dB),(1 step is 1 dB)
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetSurroundVolume(INT8 bSurroundVolume)
{
	HW_SSDCheckRegRange(REG_Ssd_surroundvol,bSurroundVolume);
    HW_SSDSetReg(REG_Ssd_surroundvol, bSurroundVolume);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set AUX1 path volume
* @param    bLeftAUX1Volume : -84(mute) ~ 24(+24 dB),(1 step is 1 dB)
* @param    bRightAUX1Volume : -84(mute) ~ 24(+24 dB),(1 step is 1 dB)
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetAUX1Volume(INT8 bLeftAUX1Volume,INT8 bRightAUX1Volume)
{
	HW_SSDCheckRegRange(REG_Ssd_aux1voll,bLeftAUX1Volume);
	HW_SSDCheckRegRange(REG_Ssd_aux1volr,bRightAUX1Volume);
    HW_SSDSetReg(REG_Ssd_aux1voll, bLeftAUX1Volume);
    HW_SSDSetReg(REG_Ssd_aux1volr, bRightAUX1Volume);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set AUX2 path volume
* @param    bLeftAUX2Volume : -84(mute) ~ 24(+24 dB),(1 step is 1 dB)
* @param    bRightAUX2Volume : -84(mute) ~ 24(+24 dB),(1 step is 1 dB)
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetAUX2Volume(INT8 bLeftAUX2Volume,INT8 bRightAUX2Volume)
{
	HW_SSDCheckRegRange(REG_Ssd_aux2voll,bLeftAUX2Volume);
	HW_SSDCheckRegRange(REG_Ssd_aux2volr,bRightAUX2Volume);
    HW_SSDSetReg(REG_Ssd_aux2voll, bLeftAUX2Volume);
    HW_SSDSetReg(REG_Ssd_aux2volr, bRightAUX2Volume);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set AUX3 path volume
* @param    bLeftAUX3Volume : -84(mute) ~ 24(+24 dB),(1 step is 1 dB)
* @param    bRightAUX3Volume : -84(mute) ~ 24(+24 dB),(1 step is 1 dB)
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetAUX3Volume(INT8 bLeftAUX3Volume,INT8 bRightAUX3Volume)
{
	HW_SSDCheckRegRange(REG_Ssd_aux3voll,bLeftAUX3Volume);
	HW_SSDCheckRegRange(REG_Ssd_aux3volr,bRightAUX3Volume);
    HW_SSDSetReg(REG_Ssd_aux3voll, bLeftAUX3Volume);
    HW_SSDSetReg(REG_Ssd_aux3volr, bRightAUX3Volume);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set main bass
* @param    bMainBass : -16(-16 dB) ~ 15(+15dB),(1 step is 1 dB)
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetMainBass(INT8 bMainBass)
{
	HW_SSDCheckRegRange(REG_Ssd_mainbass,bMainBass);
    HW_SSDSetReg(REG_Ssd_mainbass, bMainBass);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Get main bass value
* @param    pbRetMainBass : return main bass value
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudGetMainBass(INT8 *pbRetMainBass)
{
    if(pbRetMainBass == DTV_NULL)
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        HW_SSDSignExpand(REG_Ssd_mainbass,*pbRetMainBass);
        return DRV_SUCCESS;
    }
}

/******************************************************************************/
/**
* @brief    Set center bass
* @param    bCenterBass : -16(-16 dB) ~ 15(+15dB),(1 step is 1 dB)
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetCenterBass(INT8 bCenterBass)
{
	HW_SSDCheckRegRange(REG_Ssd_centerbass,bCenterBass);
    HW_SSDSetReg(REG_Ssd_centerbass, bCenterBass);
    return DRV_SUCCESS;
}
/******************************************************************************/
/**
* @brief    Get center bass value
* @param    pbRetCenterBass : return center bass value
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudGetCenterBass(INT8 *pbRetCenterBass)
{
    if(pbRetCenterBass == DTV_NULL)
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        HW_SSDSignExpand(REG_Ssd_centerbass,*pbRetCenterBass);
        return DRV_SUCCESS;
    }
}
/******************************************************************************/
/**
* @brief    Set surround bass
* @param    bSurroundBass : -16(-16 dB) ~ 15(+15dB),(1 step is 1 dB)
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetSurroundBass(INT8 bSurroundBass)
{
	HW_SSDCheckRegRange(REG_Ssd_surroundbass,bSurroundBass);
    HW_SSDSetReg(REG_Ssd_surroundbass, bSurroundBass);
    return DRV_SUCCESS;
}
/******************************************************************************/
/**
* @brief    Get surround bass value
* @param    pbRetSurroundBass : return Surround bass value
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudGetSurroundBass(INT8 *pbRetSurroundBass)
{
    if(pbRetSurroundBass == DTV_NULL)
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        HW_SSDSignExpand(REG_Ssd_surroundbass,*pbRetSurroundBass);
        return DRV_SUCCESS;
    }
}
/******************************************************************************/
/**
* @brief    Set main treble
* @param    bMainTreble : -16(-16 dB) ~ 15(+15dB),(1 step is 1 dB)
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetMainTreble(INT8 bMainTreble)
{
	HW_SSDCheckRegRange(REG_Ssd_maintreb,bMainTreble);
    HW_SSDSetReg(REG_Ssd_maintreb, bMainTreble);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Get main treble values
* @param    pbRetMainTreble : return main treble vaule
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudGetMainTreble(INT8 *pbRetMainTreble)
{
    if(pbRetMainTreble == DTV_NULL)
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        HW_SSDSignExpand(REG_Ssd_maintreb,*pbRetMainTreble);
        return DRV_SUCCESS;
    }
}

/******************************************************************************/
/**
* @brief    Set center treble
* @param    bCenterTreble : -16(-16 dB) ~ 15(+15dB),(1 step is 1 dB)
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetCenterTreble(INT8 bCenterTreble)
{
	HW_SSDCheckRegRange(REG_Ssd_centertreb,bCenterTreble);
    HW_SSDSetReg(REG_Ssd_centertreb, bCenterTreble);
    return DRV_SUCCESS;
}
/******************************************************************************/
/**
* @brief    Get main center values
* @param    pbRetCenterTreble : return center treble vaule
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudGetCenterTreble(INT8 *pbRetCenterTreble)
{
    if(pbRetCenterTreble == DTV_NULL)
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        HW_SSDSignExpand(REG_Ssd_centertreb,*pbRetCenterTreble);
        return DRV_SUCCESS;
    }
}

/******************************************************************************/
/**
* @brief    Set surround treble
* @param    bSurroundTreble : -16(-16 dB) ~ 15(+15dB),(1 step is 1 dB)
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetSurroundTreble(INT8 bSurroundTreble)
{
	HW_SSDCheckRegRange(REG_Ssd_surroundtreb,bSurroundTreble);
    HW_SSDSetReg(REG_Ssd_surroundtreb, bSurroundTreble);
    return DRV_SUCCESS;
}
/******************************************************************************/
/**
* @brief    Get surround treble values
* @param    pbRetMainTreble : return main treble vaule
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudGetSurroundTreble(INT8 *pbRetSurroundTreble)
{
    if(pbRetSurroundTreble == DTV_NULL)
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        HW_SSDSignExpand(REG_Ssd_surroundtreb,*pbRetSurroundTreble);
        return DRV_SUCCESS;
    }
}

#if 1
/******************************************************************************/
/**
* @brief    Set main equalizer configuration
* @param    bBand1_0100Hz : -12(-12 dB) ~ 12(+12dB),(1 step is 1 dB)
* @param    bBand2_000Hz : -12(-12 dB) ~ 12(+12dB),(1 step is 1 dB)
* @param    bBand3_1000Hz : -12(-12 dB) ~ 12(+12dB),(1 step is 1 dB)
* @param    bBand4_3000Hz : -12(-12 dB) ~ 12(+12dB),(1 step is 1 dB)
* @param    bBand5_8000Hz : -12(-12 dB) ~ 12(+12dB),(1 step is 1 dB)
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetMainEqualizer(INT8 bBand1_0100Hz,
                                                INT8 bBand2_000Hz,
                                                INT8 bBand3_1000Hz,
                                                INT8 bBand4_3000Hz,
                                                INT8 bBand5_8000Hz)
{
	HW_SSDCheckRegRange(REG_Ssd_eqchm1,bBand1_0100Hz);
	HW_SSDCheckRegRange(REG_Ssd_eqchm2,bBand2_000Hz);
	HW_SSDCheckRegRange(REG_Ssd_eqchm3,bBand3_1000Hz);
	HW_SSDCheckRegRange(REG_Ssd_eqchm4,bBand4_3000Hz);
	HW_SSDCheckRegRange(REG_Ssd_eqchm5,bBand5_8000Hz);

    HW_SSDSetReg(REG_Ssd_eqchm1, bBand1_0100Hz);
    HW_SSDSetReg(REG_Ssd_eqchm2, bBand2_000Hz);
    HW_SSDSetReg(REG_Ssd_eqchm3, bBand3_1000Hz);
    HW_SSDSetReg(REG_Ssd_eqchm4, bBand4_3000Hz);
    HW_SSDSetReg(REG_Ssd_eqchm5, bBand5_8000Hz);

    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Get main equalizer configuration
* @param    pbRetBand1_0100Hz : return value of 0100 Hz main equalizer band
* @param    pbRetBand2_0300Hz : return value of 0300 Hz main equalizer band
* @param    pbRetBand3_1000Hz : return value of 1000 Hz main equalizer band
* @param    pbRetBand4_3000Hz : return value of 3000 Hz main equalizer band
* @param    pbRetBand5_8000Hz : return value of 8000 Hz main equalizer band
* @return   DRV_SUCCESS if successful
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudGetMainEqualizer(INT8 *pbRetBand1_0100Hz,
                                                INT8 *pbRetBand2_0300Hz,
                                                INT8 *pbRetBand3_1000Hz,
                                                INT8 *pbRetBand4_3000Hz,
                                                INT8 *pbRetBand5_8000Hz)
{
    if((pbRetBand1_0100Hz == DTV_NULL)
        ||(pbRetBand2_0300Hz == DTV_NULL)
        ||(pbRetBand3_1000Hz == DTV_NULL)
        ||(pbRetBand4_3000Hz == DTV_NULL)
        ||(pbRetBand5_8000Hz == DTV_NULL))
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        HW_SSDSignExpand(REG_Ssd_eqchm1,*pbRetBand1_0100Hz);
        HW_SSDSignExpand(REG_Ssd_eqchm2,*pbRetBand2_0300Hz);
        HW_SSDSignExpand(REG_Ssd_eqchm3,*pbRetBand3_1000Hz);
        HW_SSDSignExpand(REG_Ssd_eqchm4,*pbRetBand4_3000Hz);
        HW_SSDSignExpand(REG_Ssd_eqchm5,*pbRetBand5_8000Hz);
        return DRV_SUCCESS;
    }
}

/******************************************************************************/
/**
* @brief    Set center equalizer configuration
* @param    bBand1_0100Hz : -12(-12 dB) ~ 12(+12dB),(1 step is 1 dB)
* @param    bBand2_0300Hz : -12(-12 dB) ~ 12(+12dB),(1 step is 1 dB)
* @param    bBand3_1000Hz : -12(-12 dB) ~ 12(+12dB),(1 step is 1 dB)
* @param    bBand4_3000Hz : -12(-12 dB) ~ 12(+12dB),(1 step is 1 dB)
* @param    bBand5_8000Hz : -12(-12 dB) ~ 12(+12dB),(1 step is 1 dB)
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetCenterEqualizer(INT8 bBand1_0100Hz,
                                                INT8 bBand2_0300Hz,
                                                INT8 bBand3_1000Hz,
                                                INT8 bBand4_3000Hz,
                                                INT8 bBand5_8000Hz)
{
	HW_SSDCheckRegRange(REG_Ssd_eqchc1,bBand1_0100Hz);
	HW_SSDCheckRegRange(REG_Ssd_eqchc2,bBand2_0300Hz);
	HW_SSDCheckRegRange(REG_Ssd_eqchc3,bBand3_1000Hz);
	HW_SSDCheckRegRange(REG_Ssd_eqchc4,bBand4_3000Hz);
	HW_SSDCheckRegRange(REG_Ssd_eqchc5,bBand5_8000Hz);

    HW_SSDSetReg(REG_Ssd_eqchc1, bBand1_0100Hz);
    HW_SSDSetReg(REG_Ssd_eqchc2, bBand2_0300Hz);
    HW_SSDSetReg(REG_Ssd_eqchc3, bBand3_1000Hz);
    HW_SSDSetReg(REG_Ssd_eqchc4, bBand4_3000Hz);
    HW_SSDSetReg(REG_Ssd_eqchc5, bBand5_8000Hz);

    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Get center equalizer configuration
* @param    pbRetBand1_0100Hz : return value of 0100 Hz main equalizer band
* @param    pbRetBand2_0300Hz : return value of 0300 Hz main equalizer band
* @param    pbRetBand3_1000Hz : return value of 1000 Hz main equalizer band
* @param    pbRetBand4_3000Hz : return value of 3000 Hz main equalizer band
* @param    pbRetBand5_8000Hz : return value of 8000 Hz main equalizer band
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudGetCenterEqualizer(INT8 *pbRetBand1_0100Hz,
                                                INT8 *pbRetBand2_0300Hz,
                                                INT8 *pbRetBand3_1000Hz,
                                                INT8 *pbRetBand4_3000Hz,
                                                INT8 *pbRetBand5_8000Hz)
{
    if((pbRetBand1_0100Hz == DTV_NULL)
        ||(pbRetBand2_0300Hz == DTV_NULL)
        ||(pbRetBand3_1000Hz == DTV_NULL)
        ||(pbRetBand4_3000Hz == DTV_NULL)
        ||(pbRetBand5_8000Hz == DTV_NULL))
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        HW_SSDSignExpand(REG_Ssd_eqchc1,*pbRetBand1_0100Hz);
        HW_SSDSignExpand(REG_Ssd_eqchc2,*pbRetBand2_0300Hz);
        HW_SSDSignExpand(REG_Ssd_eqchc3,*pbRetBand3_1000Hz);
        HW_SSDSignExpand(REG_Ssd_eqchc4,*pbRetBand4_3000Hz);
        HW_SSDSignExpand(REG_Ssd_eqchc5,*pbRetBand5_8000Hz);
        return DRV_SUCCESS;
    }
}
#else
/******************************************************************************/
/**
* @brief Set Main Equalizer Band1_0100HZ
* @param INT8 bEQValue : -12(-12 dB) ~ 12(+12dB),(1 step is 1 dB)
******************************************************************************/
DRV_Status_t DRV_AudSetMainEqualizerBand1_0100Hz(INT8 bEQValue)
{
	HW_SSDCheckRegRange(REG_Ssd_eqchm1,bEQValue);
    HW_SSDSetReg(REG_Ssd_eqchm1, bEQValue);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief Set Main Equalizer Band2_0300HZ
* @param INT8 bEQValue : -12(-12 dB) ~ 12(+12dB),(1 step is 1 dB)
******************************************************************************/
DRV_Status_t DRV_AudSetMainEqualizerBand2_0300Hz(INT8 bEQValue)
{
	HW_SSDCheckRegRange(REG_Ssd_eqchm2,bEQValue);
    HW_SSDSetReg(REG_Ssd_eqchm2, bEQValue);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief Set Main Equalizer Band3_1000Hz
* @param INT8 bEQValue : -12(-12 dB) ~ 12(+12dB),(1 step is 1 dB)
******************************************************************************/
DRV_Status_t DRV_AudSetMainEqualizerBand3_1000Hz(INT8 bEQValue)
{
	HW_SSDCheckRegRange(REG_Ssd_eqchm3,bEQValue);
	HW_SSDSetReg(REG_Ssd_eqchm3, bEQValue);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief Set Main Equalizer Band4_0100Hz
* @param INT8 bEQValue : -12(-12 dB) ~ 12(+12dB),(1 step is 1 dB)
******************************************************************************/
DRV_Status_t DRV_AudSetMainEqualizerBand4_3000Hz(INT8 bEQValue)
{
	HW_SSDCheckRegRange(REG_Ssd_eqchm4,bEQValue);
    HW_SSDSetReg(REG_Ssd_eqchm4, bEQValue);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief Set Main Equalizer Band5_8000Hz
* @param INT8 bEQValue : -12(-12 dB) ~ 12(+12dB),(1 step is 1 dB)
******************************************************************************/
DRV_Status_t DRV_AudSetMainEqualizerBand5_8000Hz(INT8 bEQValue)
{
	HW_SSDCheckRegRange(REG_Ssd_eqchm5,bEQValue);
        HW_SSDSetReg(REG_Ssd_eqchm5, bEQValue);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief Get Main Equalizer Band1_0100HZ
* @param INT8 *pbRetEQValue : return value of 100 Hz main equalizer band
******************************************************************************/
DRV_Status_t DRV_AudGetMainEqualizerBand1_0100Hz(INT8 *pbRetEQValue)
{
    HW_SSDSignExpand(REG_Ssd_eqchm1,*pbRetEQValue);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief Get Main Equalizer Band2_0300HZ
* @param INT8 *pbRetEQValue : return value of 300 Hz main equalizer band
******************************************************************************/
DRV_Status_t DRV_AudGetMainEqualizerBand2_0300Hz(INT8 *pbRetEQValue)
{
    HW_SSDSignExpand(REG_Ssd_eqchm2,*pbRetEQValue);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief Get Main Equalizer Band3_1000Hz
* @param INT8 *pbRetEQValue : return value of 1000 Hz main equalizer band
******************************************************************************/
DRV_Status_t DRV_AudGetMainEqualizerBand3_1000Hz(INT8 *pbRetEQValue)
{
    HW_SSDSignExpand(REG_Ssd_eqchm3,*pbRetEQValue);
    return DRV_SUCCESS;

}

/******************************************************************************/
/**
* @brief Get Main Equalizer Band4_3000Hz
* @param INT8 *pbRetEQValue : return value of 3000 Hz main equalizer band
******************************************************************************/
DRV_Status_t DRV_AudGetMainEqualizerBand4_3000Hz(INT8 *pbRetEQValue)
{
    HW_SSDSignExpand(REG_Ssd_eqchm4,*pbRetEQValue);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief Get Main Equalizer Band5_8000Hz
* @param INT8 *pbRetEQValue : return value of 8000 Hz main equalizer band
******************************************************************************/
DRV_Status_t DRV_AudGetMainEqualizerBand5_8000Hz(INT8 *pbRetEQValue)
{
    HW_SSDSignExpand(REG_Ssd_eqchm5,*pbRetEQValue);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief Set Center Equalizer Band1_0100HZ
* @param INT8 bEQValue : -12(-12 dB) ~ 12(+12dB),(1 step is 1 dB)
******************************************************************************/
DRV_Status_t DRV_AudSetCenterEqualizerBand1_0100Hz(INT8 bEQValue)
{
	HW_SSDCheckRegRange(REG_Ssd_eqchc1,bEQValue);
    HW_SSDSetReg(REG_Ssd_eqchc1, bEQValue);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief Set Center Equalizer Band2_0300HZ
* @param INT8 bEQValue : -12(-12 dB) ~ 12(+12dB),(1 step is 1 dB)
******************************************************************************/
DRV_Status_t DRV_AudSetCenterEqualizerBand2_0300Hz(INT8 bEQValue)
{
	HW_SSDCheckRegRange(REG_Ssd_eqchc2,bEQValue);
    HW_SSDSetReg(REG_Ssd_eqchc2, bEQValue);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief Set Center Equalizer Band3_1000Hz
* @param INT8 bEQValue : -12(-12 dB) ~ 12(+12dB),(1 step is 1 dB)
******************************************************************************/
DRV_Status_t DRV_AudSetCenterEqualizerBand3_1000Hz(INT8 bEQValue)
{
	HW_SSDCheckRegRange(REG_Ssd_eqchc3,bEQValue);
    HW_SSDSetReg(REG_Ssd_eqchc3, bEQValue);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief Set Center Equalizer Band4_3000Hz
* @param INT8 bEQValue : -12(-12 dB) ~ 12(+12dB),(1 step is 1 dB)
******************************************************************************/
DRV_Status_t DRV_AudSetCenterEqualizerBand4_3000Hz(INT8 bEQValue)
{
	HW_SSDCheckRegRange(REG_Ssd_eqchc4,bEQValue);
    HW_SSDSetReg(REG_Ssd_eqchc4, bEQValue);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief Set Center Equalizer Band5_8000Hz
* @param INT8 bEQValue : -12(-12 dB) ~ 12(+12dB),(1 step is 1 dB)
******************************************************************************/
DRV_Status_t DRV_AudSetCenterEqualizerBand5_8000Hz(INT8 bEQValue)
{
	HW_SSDCheckRegRange(REG_Ssd_eqchc5,bEQValue);
    HW_SSDSetReg(REG_Ssd_eqchc5, bEQValue);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief Get Center Equalizer Band1_0100HZ
* @param INT8 *pbRetEQValue : return value of 100 Hz center equalizer band
******************************************************************************/
DRV_Status_t DRV_AudGetCenterEqualizerBand1_0100Hz(INT8 *pbRetEQValue)
{
    HW_SSDSignExpand(REG_Ssd_eqchc1,*pbRetEQValue);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief Get Center Equalizer Band2_0300HZ
* @param INT8 *pbRetEQValue : return value of 300 Hz center equalizer band
******************************************************************************/
DRV_Status_t DRV_AudGetCenterEqualizerBand2_0300Hz(INT8 *pbRetEQValue)
{
    HW_SSDSignExpand(REG_Ssd_eqchc2,*pbRetEQValue);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief Get Center Equalizer Band3_1000Hz
* @param INT8 *pbRetEQValue : return value of 1000 Hz center equalizer band
******************************************************************************/
DRV_Status_t DRV_AudGetCenterEqualizerBand3_1000Hz(INT8 *pbRetEQValue)
{
    HW_SSDSignExpand(REG_Ssd_eqchc3,*pbRetEQValue);
    return DRV_SUCCESS;

}

/******************************************************************************/
/**
* @brief Get Center Equalizer Band4_3000Hz
* @param INT8 *pbRetEQValue : return value of 3000 Hz center equalizer band
******************************************************************************/
DRV_Status_t DRV_AudGetCenterEqualizerBand4_3000Hz(INT8 *pbRetEQValue)
{
    HW_SSDSignExpand(REG_Ssd_eqchc4,*pbRetEQValue);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief Get Center Equalizer Band5_8000Hz
* @param INT8 *pbRetEQValue : return value of 8000 Hz center equalizer band
******************************************************************************/
DRV_Status_t DRV_AudGetCenterEqualizerBand5_8000Hz(INT8 *pbRetEQValue)
{
    HW_SSDSignExpand(REG_Ssd_eqchc5,*pbRetEQValue);
    return DRV_SUCCESS;
}
#endif
/******************************************************************************/
/**
* @brief    Enable/disable main and center equalizer bypass
* @param    fEn : Enable/Disable main and center equalizer bypass
* @return   DRV_SUCCESS if successful
******************************************************************************/
DRV_Status_t DRV_AudSetMainCenterEQBypass(Boolean fEn)
{
    if(fEn == DTV_TRUE)
    {
        HW_SSDSetReg(REG_Ssd_eqbypass, REG_SSD_EQBYPASS_VALUE_EQ_BYPASS_ON);
    }
    else
    {
        HW_SSDSetReg(REG_Ssd_eqbypass, REG_SSD_EQBYPASS_VALUE_EQ_BYPASS_OFF);
    }
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Get if main and center equalizer bypass
* @param    pfRetEn : return if main and center equalizer bypass
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudGetMainCenterEQBypass(Boolean *pfRetEn)
{
    if(pfRetEn == DTV_NULL)
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        *pfRetEn = HW_SSDGetReg(REG_Ssd_eqbypass);
        return DRV_SUCCESS;
    }
}

/******************************************************************************/
/**
* @brief    Set sound effect mode
* @param    eEffectMode : specify sound effect mode
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetEffectMode(DRV_AudEffectMode_t eEffectMode)
{
	HW_SSDCheckRegRange(REG_Ssd_sndmod,eEffectMode);
    HW_SSDSetReg(REG_Ssd_sndmod, eEffectMode);

    DRV_AUDIO_DBG("DRV_AudSetEffectMode eEffectMode=%d\n",eEffectMode);
    /*execute MODE Table*/
    HW_SSDSetReg(REG_Ssd_exemodtab, REG_SSD_EXEMODTAB_VALUE_CLEAR);
    HW_SSDSetReg(REG_Ssd_exemodtab, REG_SSD_EXEMODTAB_VALUE_EXECUTE);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set spatial effect
* @param    eSpatialEffect : specify spatial sound effect
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetSpatialEffect(DRV_AudSpatialEffect_t eSpatialEffect)
{
    if(eSpatialEffect == DRV_AUD_SPATIAL_EFFECT_AUTO_I_STEREO_I_MONO)
    {
        DRV_AUDIO_DBG("DRV_AudSetSpatialEffect eSpatialEffect=%d\n",eSpatialEffect);
        HW_SSDSetReg(REG_Ssd_isoundmode, REG_SSD_ISOUNDMODE_VALUE_AUTOMATIC_MODE);
    }
    else
    {
        DRV_AUDIO_DBG("DRV_AudSetSpatialEffect eSpatialEffect=%d\n",eSpatialEffect);
	HW_SSDCheckRegRange(REG_Ssd_somoctrl,eSpatialEffect);
        HW_SSDSetReg(REG_Ssd_isoundmode, REG_SSD_ISOUNDMODE_VALUE_MANUAL_MODE);
    HW_SSDSetReg(REG_Ssd_somoctrl, eSpatialEffect);
    }
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set spatializer selector
* @param    eSpatializerSelect : specify 3D sound/WOW sound type
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetSRSSpatialSelector(DRV_AudSRSSpatializer_t eSpatializerSelect)
{
    if(eSpatializerSelect == DRV_AUD_SPATSEL_3D_SOUND)
    {
        DRV_AUDIO_DBG("DRV_AudSetSRSSpatialSelector eSpatializerSelect=%d\n",eSpatializerSelect);
        HW_SSDSetReg(REG_Ssd_srs_spatsel, REG_SSD_SRS_SPATSEL_VALUE_3D_SOUND);
    }
    else if(eSpatializerSelect == DRV_AUD_SPATSEL_WOW)
    {
        DRV_AUDIO_DBG("DRV_AudSetSRSSpatialSelector eSpatializerSelect=%d\n",eSpatializerSelect);
        HW_SSDSetReg(REG_Ssd_srs_spatsel, REG_SSD_SRS_SPATSEL_VALUE_WOW);
    }
    else
    {
        return DRV_ERR_INVALID_PARAM;
    }
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set bass feature control
* @param    eBassFeature : select bass feature control type
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetBassFeatureControl(DRV_AudBassFeatureControl_t eBassFeature)
{
	HW_SSDCheckRegRange(REG_Ssd_bassfeaturectrl,eBassFeature);
    DRV_AUDIO_DBG("DRV_AudSetBassFeatureControl eBassFeature=%d\n",eBassFeature);
    HW_SSDSetReg(REG_Ssd_bassfeaturectrl, eBassFeature);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set subwoofer mode
* @param    eSubwooferMode : subwoofer mode selection
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetSubwooferMode(DRV_AudSubwooferMode_t eSubwooferMode)
{
    if(eSubwooferMode == DRV_AUD_NORMAL_SUBWOOFER_MODE)
    {
        DRV_AUDIO_DBG("DRV_AudSubwooferMode_t eSubwooferMode=%d\n",eSubwooferMode);
        HW_SSDSetReg(REG_Ssd_mainsubctrl, REG_SSD_MAINSUBCTRL_VALUE_NORMAL_SUBWOOFER_MODE);
    }
    else if(eSubwooferMode == DRV_AUD_ECONOMIC_SUBWOOFER_MODE)
    {
        DRV_AUDIO_DBG("DRV_AudSubwooferMode_t eSubwooferMode=%d\n",eSubwooferMode);
        HW_SSDSetReg(REG_Ssd_mainsubctrl, REG_SSD_MAINSUBCTRL_VALUE_ECONOMIC_SUBWOOFER_MODE);
    }
    else
    {
        return DRV_ERR_INVALID_PARAM;
    }
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set AVL(Auto Volume Levelling) configuration
* @param    eMode : AVL mode control
* @param    bReference_level : 0 (-6 dBFS)~ 15(-36 dBFS),AVL reference level
*                              2 = high threshold | (-10 dBFS), small reduction ("daytime mode")
*                              7 = medium threshold | (-20 dBFS), medium reduction ("evening mode")
*                              12 = low threshold | (-30 dBFS), strong reduction ("night mode")
* @param    fEnWeightFilter : enable/disable weight filter
* @param    fEnPeakModeWithClipManagement : enable/disable peak mode with clip management
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetAVLConfig(DRV_AudAVLMode_t eMode,
                                            INT8 bReference_level,
                                            Boolean fEnWeightFilter,
                                            Boolean fEnPeakModeWithClipManagement)
{
    /*Set AVL mode control*/
    HW_SSDSetReg(REG_Ssd_avlmod, eMode);

    /*Set AVL reference volume level*/
	HW_SSDCheckRegRange(REG_Ssd_avllev,bReference_level);
    HW_SSDSetReg(REG_Ssd_avllev, bReference_level);

    /*Set AVL weight filter enable/disable*/
    if(fEnWeightFilter == DTV_TRUE)
    {
        HW_SSDSetReg(REG_Ssd_avlweight, REG_SSD_AVLWEIGHT_VALUE_ON_RECOMMENDED);
    }
    else
    {
        HW_SSDSetReg(REG_Ssd_avlweight, REG_SSD_AVLWEIGHT_VALUE_OFF);
    }

    /*Set AVL peak mode with clip management enable/disable*/
    if(fEnWeightFilter == DTV_TRUE)
    {
        HW_SSDSetReg(REG_Ssd_avlpeakmode, REG_SSD_AVLPEAKMODE_VALUE_AVL_WITH_CLIP_MANAGEMENT);
    }
    else
    {
        HW_SSDSetReg(REG_Ssd_avlpeakmode, REG_SSD_AVLPEAKMODE_VALUE_AVL_STANDALONE);
    }
    return DRV_SUCCESS;
}
#if 0
/******************************************************************************/
/**
* @brief    Get AVL(Auto Volume Levelling) configuration
* @param    *peRetMode : AVL mode control
* @param    *pbRetReference_level : 0 (-6 dBFS)~ 15(-36 dBFS),AVL reference level
*                                   2 = high threshold | (-10 dBFS), small reduction ("daytime mode")
*                                   7 = medium threshold | (-20 dBFS), medium reduction ("evening mode")
*                                  12 = low threshold | (-30 dBFS), strong reduction ("night mode")
* @param    *pRetfEnWeightFilter : enable/disable weight filter
* @param    *pRetfEnPeakModeWithClipManagement : enable/disable peak mode with clip management
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudGetAVLConfig(DRV_AudAVLMode_t *peRetMode,
                                            SINT8 *pbRetReference_level,
                                            Boolean *pfRetEnWeightFilter,
                                            Boolean *pfRetEnPeakModeWithClipManagement)
{
	if((peRetMode == DTV_NULL)
		||(pbRetReference_level == DTV_NULL)
		||(pfRetEnWeightFilter == DTV_NULL)
		||(pfRetEnPeakModeWithClipManagement == DTV_NULL))
	{
        return DRV_ERR_FAILURE;
	}

    /*Set AVL mode control*/
    *peRetMode = HW_SSDGetReg(REG_Ssd_avlmod);

    /*Set AVL reference volume level*/
    *pbRetReference_level = HW_SSDGetReg(REG_Ssd_avllev);

    /*Set AVL weight filter enable/disable*/
    if(HW_SSDGetReg(REG_Ssd_avlweight) == REG_SSD_AVLWEIGHT_VALUE_ON_RECOMMENDED)
    {
		*pfRetEnWeightFilter = DTV_TRUE;
    }
    else
    {
		*pfRetEnWeightFilter = DTV_FALSE;
    }

    /*Set AVL peak mode with clip management enable/disable*/
    if(HW_SSDGetReg(REG_Ssd_avlpeakmode) == REG_SSD_AVLPEAKMODE_VALUE_AVL_WITH_CLIP_MANAGEMENT)
    {
		*pfRetEnPeakModeWithClipManagement = DTV_TRUE;
    }
    else
    {
		*pfRetEnPeakModeWithClipManagement = DTV_FALSE;
    }
    return DRV_SUCCESS;
}
#endif
/******************************************************************************/
/**
* @brief    Set main loudness configuration
* @param    fEnMainLoudness : DTV_TRUE : enable mainloudness process
*                             DTV_FALSE : disable mainloudness process
* @param    fEnExtraBass : DTV_TRUE : filter character is 1000 Hz,
*						   DTV_FALSE : filter character is 500 Hz
* @param    eNoneAttackVolumeLevel : main loudness none attack volume level
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetMainLoudnessConfig(Boolean fEnMainLoudness,
                                                    Boolean fEnExtraBass,
                                                    DRV_AudMainLoudnessVolumeLevel_t eNoneAttackVolumeLevel)
{
    UINT32 mapping_table[][2] =
        {
            {DRV_AUD_MAINLOUDNESS_VOLUME_MINUS_15DB,    REG_SSD_MAINLONA_VALUE_MINUS_15DB_VOLUME_9DB_VOLUME     },
            {DRV_AUD_MAINLOUDNESS_VOLUME_MINUS_12DB,    REG_SSD_MAINLONA_VALUE_MINUS_12DB_VOLUME_12DB_VOLUME    },
            {DRV_AUD_MAINLOUDNESS_VOLUME_MINUS_09DB,    REG_SSD_MAINLONA_VALUE_MINUS_9DB_VOLUME_15DB_VOLUME     },
            {DRV_AUD_MAINLOUDNESS_VOLUME_MINUS_06DB,    REG_SSD_MAINLONA_VALUE_MINUS_6DB_VOLUME_18DB_VOLUME     },
            {DRV_AUD_MAINLOUDNESS_VOLUME_MINUS_03DB,    REG_SSD_MAINLONA_VALUE_MINUS_3DB_VOLUME_21DB_VOLUME     },
            {DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_00DB,     REG_SSD_MAINLONA_VALUE_0DB_VOLUME_24DB_VOLUME           },
            {DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_03DB,     REG_SSD_MAINLONA_VALUE_3DB_VOLUME_27DB_VOLUME           },
            {DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_06DB,     REG_SSD_MAINLONA_VALUE_6DB_VOLUME_30DB_VOLUME           },
            {DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_09DB,     REG_SSD_MAINLONA_VALUE_MINUS_15DB_VOLUME_9DB_VOLUME     },
            {DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_12DB,     REG_SSD_MAINLONA_VALUE_MINUS_12DB_VOLUME_12DB_VOLUME    },
            {DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_15DB,     REG_SSD_MAINLONA_VALUE_MINUS_9DB_VOLUME_15DB_VOLUME     },
            {DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_18DB,     REG_SSD_MAINLONA_VALUE_MINUS_6DB_VOLUME_18DB_VOLUME     },
            {DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_21DB,     REG_SSD_MAINLONA_VALUE_MINUS_3DB_VOLUME_21DB_VOLUME     },
            {DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_24DB,     REG_SSD_MAINLONA_VALUE_0DB_VOLUME_24DB_VOLUME           },
            {DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_27DB,     REG_SSD_MAINLONA_VALUE_3DB_VOLUME_27DB_VOLUME           },
            {DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_30DB,     REG_SSD_MAINLONA_VALUE_6DB_VOLUME_30DB_VOLUME           },
        };

    /*Set main loudness volume level*/
    HW_SSDSetReg(REG_Ssd_mainlona, mapping_table[eNoneAttackVolumeLevel][1]);

    /*Set main loudness volume level*/
    if((eNoneAttackVolumeLevel >= DRV_AUD_MAINLOUDNESS_VOLUME_MINUS_15DB)
        &&(eNoneAttackVolumeLevel <= DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_06DB))
    {/*Set main loudness volume level extension to set1*/
        HW_SSDSetReg(REG_Ssd_mainlonaext, REG_SSD_MAINLONAEXT_VALUE_SET_1_MINUS_15DB_6DB);
    }
    else if((eNoneAttackVolumeLevel >= DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_09DB)
        &&(eNoneAttackVolumeLevel <= DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_30DB))
    {/*Set main loudness volume level extension to set2*/
        HW_SSDSetReg(REG_Ssd_mainlonaext, REG_SSD_MAINLONAEXT_VALUE_SET_2_9DB_30DB);
    }
    else
    {
        return DRV_ERR_INVALID_PARAM;
    }

    /*Set mainloudness enable/disable*/
    if(fEnMainLoudness == DTV_TRUE)
    {
        HW_SSDSetReg(REG_Ssd_mainloud, REG_SSD_MAINLOUD_VALUE_ON);
    }
    else
    {
        HW_SSDSetReg(REG_Ssd_mainloud, REG_SSD_MAINLOUD_VALUE_OFF);
    }

    /*Set mainloudness extra bass enable/disable*/
    if(fEnExtraBass == DTV_TRUE)
    {
        HW_SSDSetReg(REG_Ssd_mainloch, REG_SSD_MAINLOCH_VALUE_EXTRA_BASS_1000HZ);
	}
    else
	{
        HW_SSDSetReg(REG_Ssd_mainloch, REG_SSD_MAINLOCH_VALUE_STANDARD_500HZ);
    }
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set DLU(Delay Line Unit) configuration
* @param    fEn : DTV_TRUE : enable delay line unit process
*                 DTV_FALSE : disable delay line unit process
* @param    bDelay : 0 ~ 63(1 step is 4 ms)
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetDLUConfig(Boolean fEn,UINT8 bDelay)
{
#if 0
    HW_SetReg(REG_Ctrl_dlu_ini, REG_CTRL_DLU_INI_VALUE_NOT_FINISHED);

    /*Disable DRAM request*/
    HW_SetReg(REG_Ctrl_reqen_audio, REG_CTRL_REQEN_AUDIO_VALUE_DISABLE);
    /*Set DRAM start address*/
    HW_SetReg(REG_Ctrl_dlu_sta_adr_3, (CYGNUM_HAL_MEMRGN_LIPSYNC_START&0x7FFFFFF)>>24);
	HW_SetReg(REG_Ctrl_dlu_sta_adr_2, (CYGNUM_HAL_MEMRGN_LIPSYNC_START&0x7FFFFFF)>>16);
	HW_SetReg(REG_Ctrl_dlu_sta_adr_1, (CYGNUM_HAL_MEMRGN_LIPSYNC_START&0x7FFFFFF)>>8);
    HW_SetReg(REG_Ctrl_dlu_sta_adr_0, (CYGNUM_HAL_MEMRGN_LIPSYNC_START&0x7FFFFFF));
    /*Set DRAM end address*/
	HW_SetReg(REG_Ctrl_dlu_end_adr_3, ((CYGNUM_HAL_MEMRGN_LIPSYNC_START+CYGNUM_HAL_MEMRGN_LIPSYNC_K_SIZE*1024)&0x7FFFFFF)>>24);
	HW_SetReg(REG_Ctrl_dlu_end_adr_2, ((CYGNUM_HAL_MEMRGN_LIPSYNC_START+CYGNUM_HAL_MEMRGN_LIPSYNC_K_SIZE*1024)&0x7FFFFFF)>>16);
	HW_SetReg(REG_Ctrl_dlu_end_adr_1, ((CYGNUM_HAL_MEMRGN_LIPSYNC_START+CYGNUM_HAL_MEMRGN_LIPSYNC_K_SIZE*1024)&0x7FFFFFF)>>8);
    HW_SetReg(REG_Ctrl_dlu_end_adr_0, (CYGNUM_HAL_MEMRGN_LIPSYNC_START+CYGNUM_HAL_MEMRGN_LIPSYNC_K_SIZE*1024)&0x7FFFFFF);

	HW_SetReg(REG_Ctrl_dlu_ini, REG_CTRL_DLU_INI_VALUE_INITIAL_DONE);
    /*Enable DRAM request*/
    HW_SetReg(REG_Ctrl_reqen_audio, REG_CTRL_REQEN_AUDIO_VALUE_ENABLE);
    /*Set DLU delay*/
	if(bDelay > REG_SSD_DLU_DLU_DELAY_V_MAX)
	{
		return DRV_ERR_INVALID_PARAM;
	}
	else
	{
		HW_SetReg(reg_ssd_dlu_dlu_delay, bDelay);
	}

    /*Set Delay Line Unit enable/disable*/
    if(fEn == DTV_TRUE)
    {
        /*reset DLU*/
        regs0->sft_cfg5 |= (0x01 << 12);

        /*release reset DLU*/
	    regs0->sft_cfg5 &= ~(0x01 << 12);


        /*Un-initial*/
        HW_SetReg(reg_ssd_dlu_dlu_ini, REG_SSD_DLU_DLU_INI_V_NOT_FINISHED);

        /*enable DLU*/
        HW_SetReg(reg_ssd_dlu_dlu_en, REG_SSD_DLU_DLU_EN_V_ENABLED);
        HW_SSDSetReg(REG_Ssd_lipsync_bypass, REG_SSD_LipSync_Bypass_VALUE_USE_EXTERNAL_LIPSYNC);

        /*Initial done*/
        HW_SetReg(reg_ssd_dlu_dlu_ini, REG_SSD_DLU_DLU_INI_V_INITIAL_DONE);
    }
    else
    {
        HW_SetReg(reg_ssd_dlu_dlu_en, REG_SSD_DLU_DLU_EN_V_DISABLED);
        HW_SSDSetReg(REG_Ssd_lipsync_bypass, REG_SSD_LipSync_Bypass_VALUE_BYPASS);
    }
#endif
        return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set DLU source selection
* @param    fEn : DTV_TRUE : enable AUX1 ~ delay line unit process
*                 DTV_FALSE : disable delay line unit process
* @param    eSource : source selection for DLU
* @return   DRV_SUCCESS if successful
* @note     This funciton is used to config DLU source for process path except main process path
******************************************************************************/
DRV_Status_t DRV_AudSetDLUSource(Boolean fEn,DRV_AudDLUSource_t eSource)
{
#if 1
    /*Set Delay Line Unit enable/disable*/
    if(fEn == DTV_TRUE)
    {
        if(eSource==DRV_AUD_DLU_ADC_INPUT)
        {
            HW_SetReg(reg_ssd__2nd_dlu_adc_sel_reg, REG_SSD__2ND_DLU_ADC_SEL_REG_V_DLU_PATH);
        }
        else if(eSource==DRV_AUD_DLU_DIGITAL_INPUT)
        {
            HW_SetReg(reg_ssd__2nd_dlu_dig1_sel_reg, REG_SSD__2ND_DLU_DIG1_SEL_REG_V_DLU_PATH);
            HW_SetReg(reg_ssd__2nd_dlu_dig2_sel_reg, REG_SSD__2ND_DLU_DIG2_SEL_REG_V_DLU_PATH);
            HW_SetReg(reg_ssd__2nd_dlu_dig3_sel_reg, REG_SSD__2ND_DLU_DIG3_SEL_REG_V_DLU_PATH);
            HW_SetReg(reg_ssd__2nd_dlu_dig4_sel_reg, REG_SSD__2ND_DLU_DIG4_SEL_REG_V_DLU_PATH);
        }
    }
    else
    {
        if(eSource==DRV_AUD_DLU_ADC_INPUT)
        {
            HW_SetReg(reg_ssd__2nd_dlu_adc_sel_reg, REG_SSD__2ND_DLU_ADC_SEL_REG_V_ORIGINAL_ADC_INPUT);
        }
        else if(eSource==DRV_AUD_DLU_DIGITAL_INPUT)
        {
            HW_SetReg(reg_ssd__2nd_dlu_dig1_sel_reg, REG_SSD__2ND_DLU_DIG1_SEL_REG_V_ORIGINAL_DIGITAL_1_INPUT);
            HW_SetReg(reg_ssd__2nd_dlu_dig2_sel_reg, REG_SSD__2ND_DLU_DIG2_SEL_REG_V_ORIGINAL_DIGITAL_2_INPUT);
            HW_SetReg(reg_ssd__2nd_dlu_dig3_sel_reg, REG_SSD__2ND_DLU_DIG3_SEL_REG_V_ORIGINAL_DIGITAL_3_INPUT);
            HW_SetReg(reg_ssd__2nd_dlu_dig4_sel_reg, REG_SSD__2ND_DLU_DIG4_SEL_REG_V_ORIGINAL_DIGITAL_4_INPUT);
        }
    }
        return DRV_SUCCESS;
#else
    /*Set Delay Line Unit enable/disable*/
    if(fEn == DTV_TRUE)
    {
        if(eSource==DRV_AUD_DLU_ADC_INPUT)
        {
            HW_SetReg(reg_ssd_adc_sel, REG_SSD_ADC_SEL_DRAM);
        }
        else if(eSource==DRV_AUD_DLU_DIGITAL_INPUT)
        {
            HW_SetReg(reg_ssd_digital_sel, REG_SSD_DIGITAL_SEL_DRAM);
        }
    }
    else
    {
        if(eSource==DRV_AUD_DLU_ADC_INPUT)
        {
            HW_SetReg(reg_ssd_adc_sel, REG_SSD_ADC_SEL_AUDIO);
        }
        else if(eSource==DRV_AUD_DLU_DIGITAL_INPUT)
        {
            HW_SetReg(reg_ssd_digital_sel, REG_SSD_DIGITAL_SEL_DIGITAL);
        }
    }
        return DRV_SUCCESS;
#endif
}

/******************************************************************************/
/**
* @brief    Set clip management mode
* @param    eClipManagementMode
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetClipManagementMode(DRV_AudClipManagementMode_t eClipManagementMode)
{
	if(eClipManagementMode > DRV_AUD_CLIP_MANAGEMENT_DYNAMIC_VOLUME_MODE)
    {
		return DRV_ERR_INVALID_PARAM;
    }
    else
    {
        HW_SSDSetReg(REG_Ssd_clipmanage, eClipManagementMode);
    }
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set clip management configuration at DVM(Dynamic Volume Mode)
* @param    ePeakDetectSelection : clip management peakdetection selection
* @param    bRelease_range_upper_limit : 0 ~ 3,dynamic volume mode release range upper limit (clip management)
*                                        0 = -3.5 dBFS
*                                        1 = -6 dBFS
*                                        2 = -9 dBFS
*                                        3 = -12 dBFS
* @param    bRelease_range_lower_limit : 0 ~ 3,dynamic volume mode release range lower limit (clip management)
*                                        0 = -40 dBFS
*                                        1 = -50 dBFS
*                                        2 = -60 dBFS
*                                        3 = -inf
* @param    bAttack_speed : 0 ~ 1,dynamic volume mode attack speed
*                           0 = 4.8 dB/s
*                           1 = 9.6 dB/s
* @param    bRelease_speed : 0 ~ 3,dynamic volume mode release speed
*                            0 = 0.06 dB/s
*                            1 = 0.12 dB/s
*                            2 = 0.24 dB/s
*                            3 = 0.48 dB/s
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetClipManagementDVMConfig(DRV_AudDVMPeakDetectionSelection_t ePeakDetectSelection,
                                                        UINT8 bRelease_range_upper_limit,
                                                        UINT8 bRelease_range_lower_limit,
                                                        UINT8 bAttack_speed,
                                                        UINT8 bRelease_speed)
{
#if 1
    if((bRelease_range_upper_limit > REG_Ssd_dvm_rr_upper_MAX)
        ||(bRelease_range_lower_limit > REG_Ssd_dvm_rr_lower_MAX)
        ||(bAttack_speed > REG_Ssd_dvm_attspeed_MAX)
        ||(bRelease_speed > REG_Ssd_dvm_relspeed_MAX))
        return DRV_ERR_INVALID_PARAM;
#else
    HW_SSDCheckRegRange(REG_Ssd_dvm_rr_upper, bRelease_range_upper_limit);
    HW_SSDCheckRegRange(REG_Ssd_dvm_rr_lower, bRelease_range_lower_limit);
    HW_SSDCheckRegRange(REG_Ssd_dvm_attspeed, bAttack_speed);
    HW_SSDCheckRegRange(REG_Ssd_dvm_relspeed, bRelease_speed);
#endif

	if(ePeakDetectSelection == DRV_AUD_DVM_PEAK_DETECTION_SELECT_MAIN)
    {
		HW_SSDSetReg(REG_Ssd_dvm_peak_sel, REG_SSD_DVM_PEAK_SEL_VALUE_PEAK_DETECTION_MAIN_L_R);
    }
	else if(ePeakDetectSelection == DRV_AUD_DVM_PEAK_DETECTION_SELECT_MAIN_CENTER_SUBWOOFER)
    {
        HW_SSDSetReg(REG_Ssd_dvm_peak_sel, REG_SSD_DVM_PEAK_SEL_VALUE_PEAK_DETECTION_L_R_C_SUB);
    }
    else
    {
        return DRV_ERR_INVALID_PARAM;
    }

    HW_SSDSetReg(REG_Ssd_dvm_rr_upper, bRelease_range_upper_limit);
    HW_SSDSetReg(REG_Ssd_dvm_rr_lower, bRelease_range_lower_limit);
    HW_SSDSetReg(REG_Ssd_dvm_attspeed, bAttack_speed);
    HW_SSDSetReg(REG_Ssd_dvm_relspeed, bRelease_speed);

    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set bass management configuration
* @param    eMode : bass management mode
* @param    eFilterCharacteristic : 0 ~ 15,bass management filtercharacteristics (50 - 400hz corner frequency)
* @param    fEnSubwooferFilter : DTV_TRUE : enable bass management subwoofer filter control
*                                DTV_FALSE : disable bass management subwoofer filter control
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetBassManagementConfig(DRV_AudBassManagementMode_t eMode,
                                                    DRV_AudBassManagementFilterCharacteristic_t eFilterCharacteristic,
                                                    Boolean fEnSubwooferFilter)
{
    DRV_AUDIO_DBG("DRV_AudBassManagementMode_t eMode=%d\n",eMode);
//    DRV_AUDIO_DBG("DRV_AudBassManagementMode_t eFilter=%d\n",eFilter);

//	HW_SSDCheckRegRange(REG_Ssd_bamamo,eMode);
#if 1
    if(eFilterCharacteristic > REG_Ssd_bamafc_MAX)
        return DRV_ERR_INVALID_PARAM;
#else
    HW_SSDCheckRegRange(REG_Ssd_bamafc, bFilterCharacteristic);
#endif

    if(fEnSubwooferFilter == DTV_TRUE)
    {
        HW_SSDSetReg(REG_Ssd_bamasub, REG_SSD_BAMASUB_VALUE_SUBWOOFER_FILTER_ON);
    }
    else
    {
        HW_SSDSetReg(REG_Ssd_bamasub, REG_SSD_BAMASUB_VALUE_SUBWOOFER_FILTER_OFF);
    }

    HW_SSDSetReg(REG_Ssd_bamamo, eMode);
    HW_SSDSetReg(REG_Ssd_bamafc, eFilterCharacteristic);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set VDSII configuration
* @param    eEffectLevel : VDSII Effect Level: 0..100% (5 steps)
* @param    eBalanceControl : VDSII Balance Control
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetVDSIIConfig(DRV_AudVDSIIEffectLevel_t eEffectLevel,
                                            DRV_AudVDSIIBalanceControl_t eBalanceControl)
{
#if 1
    if((eEffectLevel > DRV_AUD_VDSII_EFFECT_LEVEL_100_PERCENT)
        ||(eBalanceControl > DRV_AUD_VDSII_BALANCE_RIGHT))
        return DRV_ERR_INVALID_PARAM;
#else
	HW_SSDCheckRegRange(REG_Ssd_vdsmixlev,bEffectLevel);
	HW_SSDCheckRegRange(REG_Ssd_vdsiibalance,bBalanceControl);
#endif

    HW_SSDSetReg(REG_Ssd_vdsmixlev, eEffectLevel);
    HW_SSDSetReg(REG_Ssd_vdsiibalance, eBalanceControl);

    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set SRS TruSurround XT configuration
* @param    eMode : TSXT K Mode selection
* @param    fEnDialogClarity : DTV_TRUE : enable dialog clarity function control
*                              DTV_FALSE : disable dialog clarity function control
* @param    fEnTruBass : DTV_TRUE : enable TruBass function control
*                        DTV_FALSE : disable TruBass function control
* @param    fEnHeadphoneControl : DTV_TRUE : enable headphone control
*                                 DTV_FALSE : disable headphone control
* @param    fEnCenterOutputModeControl : DTV_TRUE : enable center output mode control
*                                        DTV_FALSE : disable center output mode control
* @param    eSpeakerSize : 0 ~ 7,speaker size selection
* @param    eTruBassGain : 0 ~ 15,trubass gain control
* @param    eDialogClarityGain : 0 ~ 15,dialog clarity gain control
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetSRSTruSurroundXTConfig(DRV_AudSRSTrusurroundXT_K_Mode_t eMode,
                                                        Boolean fEnDialogClarity,
                                                        Boolean fEnTruBass,
                                                        Boolean fEnHeadphoneControl,
                                                        Boolean fEnCenterOutputModeControl,
                                                        DRV_AudSRSTrusurroundXT_SpeakerSize_t eSpeakerSize,
                                                        DRV_AudSRSTrusurroundXT_TruBassGain_t eTruBassGain,
                                                        DRV_AudSRSTrusurroundXT_DialogClarityGain_t eDialogClarityGain)
{
#if 1
    if((eSpeakerSize > DRV_AUD_SRSTSXT_SPEAKERSIZE_400HZ)
        ||(eTruBassGain > DRV_AUD_SRSTSXT_TRUBASS_100_PERCENT)
        ||(eDialogClarityGain > DRV_AUD_SRSTSXT_DIALOGCLARITY_100_PERCENT))
        return DRV_ERR_INVALID_PARAM;
#else
	HW_SSDCheckRegRange(REG_Ssd_tsxt_k_mode,bMode);
	HW_SSDCheckRegRange(REG_Ssd_tsxt_speakersize,bSpeakerSize);
	HW_SSDCheckRegRange(REG_Ssd_tsxt_trubass_gain,bTruBassGain);
	HW_SSDCheckRegRange(REG_Ssd_tsxt_dialogclarity_gain,bDialogClarityGain);
#endif

    if(fEnDialogClarity == DTV_TRUE)
    {
        HW_SSDSetReg(REG_Ssd_tsxt_dialogclarityenable, REG_SSD_TSXT_DialogClarityEnable_VALUE_ON);
    }
    else
    {
        HW_SSDSetReg(REG_Ssd_tsxt_dialogclarityenable, REG_SSD_TSXT_DialogClarityEnable_VALUE_OFF);
    }

    if(fEnTruBass==DTV_TRUE)
    {
        HW_SSDSetReg(REG_Ssd_tsxt_trubassenable, REG_SSD_TSXT_TruBassEnable_VALUE_ON);
    }
    else
    {
        HW_SSDSetReg(REG_Ssd_tsxt_trubassenable, REG_SSD_TSXT_TruBassEnable_VALUE_OFF);
    }

    if(fEnHeadphoneControl==DTV_TRUE)
    {
        HW_SSDSetReg(REG_Ssd_tsxt_headphone_modeenable, REG_SSD_TSXT_Headphone_ModeEnable_VALUE_ON);
    }
    else
    {

        HW_SSDSetReg(REG_Ssd_tsxt_headphone_modeenable, REG_SSD_TSXT_Headphone_ModeEnable_VALUE_OFF);
    }

    if(fEnCenterOutputModeControl==DTV_TRUE)
    {
        HW_SSDSetReg(REG_Ssd_tsxt_centerout_523modeenable, REG_SSD_TSXT_CenterOut_523ModeEnable_VALUE_ON);
    }
    else
    {
        HW_SSDSetReg(REG_Ssd_tsxt_centerout_523modeenable, REG_SSD_TSXT_CenterOut_523ModeEnable_VALUE_OFF);
    }

    HW_SSDSetReg(REG_Ssd_tsxt_k_mode,eMode);
    HW_SSDSetReg(REG_Ssd_tsxt_speakersize,eSpeakerSize);
    HW_SSDSetReg(REG_Ssd_tsxt_trubass_gain,eTruBassGain);
    HW_SSDSetReg(REG_Ssd_tsxt_dialogclarity_gain,eDialogClarityGain);

    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set ISound configuration
* @param    eIntensityEffect : I-Mono or I-Stereo intensity
* @param    eFadeTime : ISound fade time
* @param    eDetectorTime : ISound detector time
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetISoundConfig(DRV_AudISoundIntensityEffect_t eIntensityEffect,
                                            DRV_AudISoundFadeTime_t eFadeTime,
                                            DRV_AudISoundDetectorTime_t eDetectorTime)
{

    UINT32 mapping_table[][2] =
    {
            {DRV_AUD_ISOUND_FADETIME_01_1_SEC,   REG_SSD_ISOUNDFADETIME_VALUE_1_1SEC_136MSEC	},
            {DRV_AUD_ISOUND_FADETIME_02_2_SEC,   REG_SSD_ISOUNDFADETIME_VALUE_2_2SEC_272MSEC    },
            {DRV_AUD_ISOUND_FADETIME_03_3_SEC,   REG_SSD_ISOUNDFADETIME_VALUE_3_3SEC_410MSEC    },
            {DRV_AUD_ISOUND_FADETIME_04_5_SEC,   REG_SSD_ISOUNDFADETIME_VALUE_4_5SEC_546MSEC    },
            {DRV_AUD_ISOUND_FADETIME_06_8_SEC,   REG_SSD_ISOUNDFADETIME_VALUE_6_8SEC_820MSEC    },
            {DRV_AUD_ISOUND_FADETIME_09_0_SEC,   REG_SSD_ISOUNDFADETIME_VALUE_9_0SEC_1_1SEC    	},
            {DRV_AUD_ISOUND_FADETIME_14_5_SEC,   REG_SSD_ISOUNDFADETIME_VALUE_14_5SEC_1_8SEC    },
            {DRV_AUD_ISOUND_FADETIME_19_0_SEC,   REG_SSD_ISOUNDFADETIME_VALUE_19_0SEC_2_4SEC   	},
            {DRV_AUD_ISOUND_FADETIME_136_MSEC,   REG_SSD_ISOUNDFADETIME_VALUE_1_1SEC_136MSEC    },
            {DRV_AUD_ISOUND_FADETIME_272_MSEC,   REG_SSD_ISOUNDFADETIME_VALUE_2_2SEC_272MSEC    },
            {DRV_AUD_ISOUND_FADETIME_410_MSEC,   REG_SSD_ISOUNDFADETIME_VALUE_3_3SEC_410MSEC    },
            {DRV_AUD_ISOUND_FADETIME_546_MSEC,   REG_SSD_ISOUNDFADETIME_VALUE_4_5SEC_546MSEC    },
            {DRV_AUD_ISOUND_FADETIME_820_MSEC,   REG_SSD_ISOUNDFADETIME_VALUE_6_8SEC_820MSEC    },
//            {DRV_AUD_ISOUND_FADETIME_01_1_SEC,   REG_SSD_ISOUNDFADETIME_VALUE_9_0SEC_1_1SEC   },
            {DRV_AUD_ISOUND_FADETIME_01_8_SEC,   REG_SSD_ISOUNDFADETIME_VALUE_14_5SEC_1_8SEC    },
            {DRV_AUD_ISOUND_FADETIME_02_4SEC,   REG_SSD_ISOUNDFADETIME_VALUE_19_0SEC_2_4SEC   	},
    };

#if 1
    if((eIntensityEffect > DRV_AUD_ISOUND_INTENSITY_EFFECT_5)
        ||(eFadeTime > DRV_AUD_ISOUND_FADETIME_02_4SEC)
        ||(eDetectorTime > DRV_AUD_ISOUND_DETECTORTIME_14SEC))
        return DRV_ERR_INVALID_PARAM;
#else
	HW_SSDCheckRegRange(REG_Ssd_insoef,bEffect);
	HW_SSDCheckRegRange(REG_Ssd_isoundfadetime,bFadeTime);
	HW_SSDCheckRegRange(REG_Ssd_isounddettime,bDetectorTime);
#endif

    if((eFadeTime >= DRV_AUD_ISOUND_FADETIME_01_1_SEC)
        && (eFadeTime <= DRV_AUD_ISOUND_FADETIME_19_0_SEC))
    {/*Set I sound Fader table selector to table1*/
        HW_SSDSetReg(REG_Ssd_isoundfadetimesel, REG_SSD_ISOUNDFADETIMESEL_VALUE_TABLE_1_1_1SEC_19SEC);
    }
    else if((eFadeTime >= DRV_AUD_ISOUND_FADETIME_136_MSEC)
        && (eFadeTime <= DRV_AUD_ISOUND_FADETIME_02_4SEC))
    {/*Set I sound Fader table selector to table2*/
        HW_SSDSetReg(REG_Ssd_isoundfadetimesel, REG_SSD_ISOUNDFADETIMESEL_VALUE_TABLE_2_136MSEC_2_4SEC);
    }
    else
    {
        return DRV_ERR_INVALID_PARAM;
    }

    /*Set I sound fade time*/
    HW_SSDSetReg(REG_Ssd_isoundfadetime, mapping_table[eFadeTime][1]);
    HW_SSDSetReg(REG_Ssd_insoef,eIntensityEffect);
    HW_SSDSetReg(REG_Ssd_isounddettime,eDetectorTime);

    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set SRS 3D sound configuration
* @param    fEnBypass : DTV_TRUE : enable SRS 3D sound bypass
*                       DTV_FALSE : disable SRS 3D sound bypass
* @param    eCenter : SRS 3D sound center Selection
* @param    eSpace : SRS 3D sound space Selection
******************************************************************************/
DRV_Status_t DRV_AudSetSRS3DSoundConfig(Boolean fEnBypass,
                                                DRV_AudSRS3DCenter_t eCenter,
                                                DRV_AudSRS3DSpace_t eSpace)
{
#if 1
    if((eCenter > REG_Ssd_srs3dcenter_MAX)
        ||(eSpace > REG_Ssd_srs3dspace_MAX))
        return DRV_ERR_INVALID_PARAM;
#else
    HW_SSDCheckRegRange(REG_Ssd_srs3dcenter,bCenter);
    HW_SSDCheckRegRange(REG_Ssd_srs3dspace,bSpace);
#endif

    if(fEnBypass == DTV_TRUE)
{
        HW_SSDSetReg(REG_Ssd_srs3dbypass, REG_SSD_SRS3DBYPASS_VALUE_BYPASS_ACTIVE);
}
    else
{
        HW_SSDSetReg(REG_Ssd_srs3dbypass, REG_SSD_SRS3DBYPASS_VALUE_3D_SOUND_ACTIVE);
}

    HW_SSDSetReg(REG_Ssd_srs3dcenter,eCenter);
    HW_SSDSetReg(REG_Ssd_srs3dspace,eSpace);

    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set WOW configuration
* @param    eBrightness : SRS WOW brightness control
* @param    eSpeakerSize : SRS WOW speaker size selection
* @param    eBassLevel : SRS WOW bass level control
* @param    eStereoWidth : SRS WOW stereo width control
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetSRSWOWConfig(DRV_AudSRSWOWBrightness_t eBrightness,
                                            DRV_AudSRSWOWSpeakerSize_t eSpeakerSize,
                                            DRV_AudSRSWOWBassLevel_t eBassLevel,
                                            DRV_AudSRSWOWStereoWidth_t eStereoWidth)
{
#if 1
    if((eBrightness > REG_SSD_WOW_BRIGHTNESS_VALUE_HIGH)
        ||(eSpeakerSize > REG_SSD_WOW_SPKSIZE_VALUE_400HZ)
        ||(eBassLevel > REG_SSD_WOW_BASSLEVEL_VALUE_100)
        ||(eStereoWidth > REG_SSD_WOW_STEREOWIDTH_VALUE_100))
        return DRV_ERR_INVALID_PARAM;
#else
    HW_SSDCheckRegRange(REG_Ssd_wow_brightness,bBrightness);
    HW_SSDCheckRegRange(REG_Ssd_wow_spksize,bSpeakerSize);
    HW_SSDCheckRegRange(REG_Ssd_wow_basslevel,bBassLevel);
    HW_SSDCheckRegRange(REG_Ssd_wow_stereowidth,bStereoWidth);
#endif

    HW_SSDSetReg(REG_Ssd_wow_brightness,eBrightness);
    HW_SSDSetReg(REG_Ssd_wow_spksize,eSpeakerSize);
    HW_SSDSetReg(REG_Ssd_wow_basslevel,eBassLevel);
    HW_SSDSetReg(REG_Ssd_wow_stereowidth,eStereoWidth);

    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set 3D sound configuration
* @param    eBassBoost : BBE contour value at 100 Hz
* @param    eTrebleBoost : BBE process value at 10 kHz
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetBBEConfig(DRV_AudBBEBoost_t eBassBoost,
                                            DRV_AudBBEBoost_t eTrebleBoost)
{
#if 1
    if((eBassBoost > REG_Ssd_bbecontour_MAX)
        ||(eTrebleBoost > REG_Ssd_bbeprocess_MAX))
        return DRV_ERR_INVALID_PARAM;
#else
    HW_SSDCheckRegRange(REG_Ssd_bbecontour,bBassBoost);
    HW_SSDCheckRegRange(REG_Ssd_bbeprocess,bTrebleBoost);
#endif

    HW_SSDSetReg(REG_Ssd_bbecontour,eBassBoost);
    HW_SSDSetReg(REG_Ssd_bbeprocess,eTrebleBoost);

    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set ACC(Acoustical Compensation) configuration
* @param    eProcessPath : specify ACC process path
* @param    a0 : specify ACC parameter a0
* @param    a1 : specify ACC parameter a1
* @param    a2 : specify ACC parameter a2
* @param    b1 : specify ACC parameter b1
* @param    b2 : specify ACC parameter b2
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetACCConfig(DRV_AudProcessPath_t eProcessPath,
                                            INT32 a0,
                                            INT32 a1,
                                            INT32 a2,
                                            INT32 b1,
                                            INT32 b2)
{
	DRV_Status_t ret = DRV_SUCCESS;
	switch(eProcessPath)
	{
		case DRV_AUD_PROCESS_PATH_MAIN:
			HW_SSDSetBit(REG_Ssd_accadr_OFFSET,0xffffff,
				REG_Ssd_accadr_BIT_IDX,((a0<<REG_Ssd_acccoef_BIT_IDX)|0x01));
			HW_SSDSetBit(REG_Ssd_accadr_OFFSET,0xffffff,
				REG_Ssd_accadr_BIT_IDX,((a1<<REG_Ssd_acccoef_BIT_IDX)|0x02));
			HW_SSDSetBit(REG_Ssd_accadr_OFFSET,0xffffff,
				REG_Ssd_accadr_BIT_IDX,((a2<<REG_Ssd_acccoef_BIT_IDX)|0x05));
			HW_SSDSetBit(REG_Ssd_accadr_OFFSET,0xffffff,
				REG_Ssd_accadr_BIT_IDX,((b1<<REG_Ssd_acccoef_BIT_IDX)|0x03));
			HW_SSDSetBit(REG_Ssd_accadr_OFFSET,0xffffff,
				REG_Ssd_accadr_BIT_IDX,((b2<<REG_Ssd_acccoef_BIT_IDX)|0x04));
			break;

		case DRV_AUD_PROCESS_PATH_CENTER:
			HW_SSDSetBit(REG_Ssd_accadr_OFFSET,0xffffff,
				REG_Ssd_accadr_BIT_IDX,((a0<<REG_Ssd_acccoef_BIT_IDX)|0x08));
			HW_SSDSetBit(REG_Ssd_accadr_OFFSET,0xffffff,
				REG_Ssd_accadr_BIT_IDX,((a1<<REG_Ssd_acccoef_BIT_IDX)|0x07));
			HW_SSDSetBit(REG_Ssd_accadr_OFFSET,0xffffff,
				REG_Ssd_accadr_BIT_IDX,((a2<<REG_Ssd_acccoef_BIT_IDX)|0x06));
			HW_SSDSetBit(REG_Ssd_accadr_OFFSET,0xffffff,
				REG_Ssd_accadr_BIT_IDX,((b1<<REG_Ssd_acccoef_BIT_IDX)|0x09));
			HW_SSDSetBit(REG_Ssd_accadr_OFFSET,0xffffff,
				REG_Ssd_accadr_BIT_IDX,((b2<<REG_Ssd_acccoef_BIT_IDX)|0x0a));
			break;

		case DRV_AUD_PROCESS_PATH_SURROUND:
			HW_SSDSetBit(REG_Ssd_accadr_OFFSET,0xffffff,
				REG_Ssd_accadr_BIT_IDX,((a0<<REG_Ssd_acccoef_BIT_IDX)|0x0d));
			HW_SSDSetBit(REG_Ssd_accadr_OFFSET,0xffffff,
				REG_Ssd_accadr_BIT_IDX,((a1<<REG_Ssd_acccoef_BIT_IDX)|0x0c));
			HW_SSDSetBit(REG_Ssd_accadr_OFFSET,0xffffff,
				REG_Ssd_accadr_BIT_IDX,((a2<<REG_Ssd_acccoef_BIT_IDX)|0x0b));
			HW_SSDSetBit(REG_Ssd_accadr_OFFSET,0xffffff,
				REG_Ssd_accadr_BIT_IDX,((b1<<REG_Ssd_acccoef_BIT_IDX)|0x0e));
			HW_SSDSetBit(REG_Ssd_accadr_OFFSET,0xffffff,
				REG_Ssd_accadr_BIT_IDX,((b2<<REG_Ssd_acccoef_BIT_IDX)|0x0f));
			break;

		default:
			    ret = DRV_ERR_INVALID_PARAM;
			break;
	};
	return ret;
}

/******************************************************************************/
/**
* @brief    Set monitor configuration
* @param    eSource : specify monitor source
* @param    eDetectMode : specify monitor detect method
* @param    eMonitorMatrix : specify calulating matrix
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetMonitorConfig(DRV_AudMonitorSource_t eSource,
                                                DRV_AudMonitorMode_t eDetectMode,
                                                DRV_AudMonitorMatrix_t eMonitorMatrix)
{
    /*Set monitor config*/
    HW_SSDSetReg(REG_Ssd_mon_src, eSource);
    HW_SSDSetReg(REG_Ssd_mon_det, eDetectMode);
    HW_SSDSetReg(REG_Ssd_mon_mat, eMonitorMatrix);
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Get Monitor Level
* @param    pdRetMonitorLevel : monitor level report
* @return   DRV_SUCCESS if successful
*			DRV_ERR_FAILURE if pointer variable is DTV_NULL
******************************************************************************/
DRV_Status_t DRV_AudGetMonitorLevel(INT32 *pdRetMonitorLevel)
{
    if(pdRetMonitorLevel == DTV_NULL)
    {
        return DRV_ERR_FAILURE;
    }
    else
    {
        HW_SSDSignExpand(REG_Ssd_monlevel,*pdRetMonitorLevel);
        return DRV_SUCCESS;
    }
}

/******************************************************************************/
/**
* @brief    Set Beeper configuration
* @param    eProcessPath : specify main,center,surround,AUX1,AUX2,AUX3 process path
*                          to output beeper sound
* @param    wBeeperVolume :  -84(mute) ~ 0(0 dB),(1 step is 1 dB)
* @param    eBeeperFrequency : specify beeper frequency
* @param    eBeeperMode : specify beeper mode
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetBeeperConfig(DRV_AudProcessPath_t eProcessPath,
                                            INT16 wBeeperVolume,
                                            DRV_AudBeeperFrequency_t eBeeperFrequency,
                                            DRV_AudBeeperMode_t eBeeperMode)
{
    UINT32 mapping_table[][2] =
        {
            {DRV_AUD_BEEPERFREQUENCY_00030HZ,   REG_SSD_BEEPFREQ_VALUE_200_HZ_30HZ      },
            {DRV_AUD_BEEPERFREQUENCY_00060HZ,   REG_SSD_BEEPFREQ_VALUE_400_HZ_60HZ      },
            {DRV_AUD_BEEPERFREQUENCY_00090HZ,   REG_SSD_BEEPFREQ_VALUE_1000_HZ_90HZ     },
            {DRV_AUD_BEEPERFREQUENCY_00120HZ,   REG_SSD_BEEPFREQ_VALUE_2000_HZ_120HZ    },
            {DRV_AUD_BEEPERFREQUENCY_00150HZ,   REG_SSD_BEEPFREQ_VALUE_3000_HZ_150HZ    },
            {DRV_AUD_BEEPERFREQUENCY_00180HZ,   REG_SSD_BEEPFREQ_VALUE_5000_HZ_180HZ    },
            {DRV_AUD_BEEPERFREQUENCY_00250HZ,   REG_SSD_BEEPFREQ_VALUE_8000_HZ_250HZ    },
            {DRV_AUD_BEEPERFREQUENCY_00300HZ,   REG_SSD_BEEPFREQ_VALUE_12500_HZ_300HZ   },
            {DRV_AUD_BEEPERFREQUENCY_00200HZ,   REG_SSD_BEEPFREQ_VALUE_200_HZ_30HZ      },
            {DRV_AUD_BEEPERFREQUENCY_00400HZ,   REG_SSD_BEEPFREQ_VALUE_400_HZ_60HZ      },
            {DRV_AUD_BEEPERFREQUENCY_01000HZ,   REG_SSD_BEEPFREQ_VALUE_1000_HZ_90HZ     },
            {DRV_AUD_BEEPERFREQUENCY_02000HZ,   REG_SSD_BEEPFREQ_VALUE_2000_HZ_120HZ    },
            {DRV_AUD_BEEPERFREQUENCY_03000HZ,   REG_SSD_BEEPFREQ_VALUE_3000_HZ_150HZ    },
            {DRV_AUD_BEEPERFREQUENCY_05000HZ,   REG_SSD_BEEPFREQ_VALUE_5000_HZ_180HZ    },
            {DRV_AUD_BEEPERFREQUENCY_08000HZ,   REG_SSD_BEEPFREQ_VALUE_8000_HZ_250HZ    },
            {DRV_AUD_BEEPERFREQUENCY_12500HZ,   REG_SSD_BEEPFREQ_VALUE_12500_HZ_300HZ   },
        };

    /*Set process path*/
    switch(eProcessPath)
    {
        case DRV_AUD_PROCESS_PATH_MAIN:
            DRV_AudSetMainAUXPath(DRV_AUD_PROCESS_PATH_MAIN,
                                        DRV_AUD_MAIN_AUX_PATH_SOURCE_BEEPER);
            break;

        case DRV_AUD_PROCESS_PATH_AUX1:
            DRV_AudSetMainAUXPath(DRV_AUD_PROCESS_PATH_AUX1,
                                        DRV_AUD_MAIN_AUX_PATH_SOURCE_BEEPER);
            break;

        case DRV_AUD_PROCESS_PATH_AUX2:
            DRV_AudSetMainAUXPath(DRV_AUD_PROCESS_PATH_AUX2,
                                        DRV_AUD_MAIN_AUX_PATH_SOURCE_BEEPER);
            break;

        case DRV_AUD_PROCESS_PATH_AUX3:
            DRV_AudSetMainAUXPath(DRV_AUD_PROCESS_PATH_AUX3,
                                        DRV_AUD_MAIN_AUX_PATH_SOURCE_BEEPER);
            break;

        case DRV_AUD_PROCESS_PATH_CENTER:
            DRV_AudSetCenterSurroundPath(DRV_AUD_PROCESS_PATH_CENTER,
                                        DRV_AUD_CENTER_SURROUND_PATH_SOURCE_BEEPER);
            break;

        case DRV_AUD_PROCESS_PATH_SURROUND:
            DRV_AudSetCenterSurroundPath(DRV_AUD_PROCESS_PATH_SURROUND,
                                        DRV_AUD_CENTER_SURROUND_PATH_SOURCE_BEEPER);
            break;

        default :
            return DRV_ERR_INVALID_PARAM;
            break;
    }

    /*Set beeper volume*/
	HW_SSDCheckRegRange(REG_Ssd_beepvol,wBeeperVolume);
    HW_SSDSetReg(REG_Ssd_beepvol, wBeeperVolume);

    if((eBeeperFrequency >= DRV_AUD_BEEPERFREQUENCY_00030HZ)
        && (eBeeperFrequency <= DRV_AUD_BEEPERFREQUENCY_00300HZ))
    {/*Set beeper frequency extension to set2*/
        HW_SSDSetReg(REG_Ssd_beepfreext, REG_SSD_BEEPFREEXT_VALUE_BEEPER_FREQUENCY_SET2);
    }
    else if((eBeeperFrequency >= DRV_AUD_BEEPERFREQUENCY_00200HZ)
        && (eBeeperFrequency <= DRV_AUD_BEEPERFREQUENCY_12500HZ))
    {/*Set beeper frequency extension to set1*/
        HW_SSDSetReg(REG_Ssd_beepfreext, REG_SSD_BEEPFREEXT_VALUE_BEEPER_FREQUENCY_SET1);
    }
    else
    {
        return DRV_ERR_INVALID_PARAM;
    }

    /*Set beeper frequency*/
    HW_SSDSetReg(REG_Ssd_beepfreq, mapping_table[eBeeperFrequency][1]);

    /*Set beeper mode*/
    if(eBeeperMode == DRV_AUD_BEEPER_GRAPHIC_MODE)
    {
        HW_SSDSetReg(REG_Ssd_beepmodesel, REG_SSD_BEEPMODESEL_VALUE_GRAPHIC_MODE);
    }
    else if(eBeeperMode == DRV_AUD_BEEPER_PARAMETRIC_MODE)
    {
        HW_SSDSetReg(REG_Ssd_beepmodesel, REG_SSD_BEEPMODESEL_VALUE_PARAMETRIC_MODE);
    }
    else
    {
        return DRV_ERR_INVALID_PARAM;
    }
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set Silence Detection configuration
* @param    eChannel : only main and AUX1 process path have this function
* @param    fEnSilenceDetection : DTV_TRUE : enable auto silence detection
*			       	         	  DTV_FALSE : disable auto silence detection
* @param    eThreshold : config silence detect threshold
* @param    eDelay : config delay time for this function
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
*******************************************************************************/
DRV_Status_t DRV_AudSetSilenceDetectionConfig(DRV_AudSilenceDetectionChannel_t eChannel,
														Boolean fEnSilenceDetection,
                                                      	DRV_AudSilenceDetectionThreshold_t eThreshold,
                                                      	DRV_AudSilenceDetectionDelay_t eDelay)
{
    DRV_Status_t ret = DRV_SUCCESS;

    switch (eChannel)
    {
        case DRV_AUD_SILENCE_DETECTION_CHANNEL_MAIN:
            if (fEnSilenceDetection == DTV_TRUE)
            {
                HW_SSDSetReg(REG_Ssd_sild_en_main, REG_SSD_SILD_EN_MAIN_VALUE_ENABLED);
                HW_SSDSetReg(REG_Ssd_sild_thr_main, eThreshold);
                HW_SSDSetReg(REG_Ssd_sild_delay_main, eDelay);
            }
            else
            {
                HW_SSDSetReg(REG_Ssd_sild_en_main, REG_SSD_SILD_EN_MAIN_VALUE_DISABLED);
            }
            break;

        case DRV_AUD_SILENCE_DETECTION_CHANNEL_AUX1:
            if (fEnSilenceDetection == DTV_TRUE)
            {
                HW_SSDSetReg(REG_Ssd_sild_en_aux1, REG_SSD_SILD_EN_AUX1_VALUE_ENABLED);
                HW_SSDSetReg(REG_Ssd_sild_thr_aux1, eThreshold);
                HW_SSDSetReg(REG_Ssd_sild_delay_aux1, eDelay);
            }
            else
            {
                HW_SSDSetReg(REG_Ssd_sild_en_aux1, REG_SSD_SILD_EN_AUX1_VALUE_DISABLED);
            }
            break;

        default:
            ret = DRV_ERR_INVALID_PARAM;
            break;
    }
    return ret;
}
/******************************************************************************/
/**
* @brief    Set audio line-in ADC PGA gain
* @param    bPGA_gain : -25(mute) ~ 6(6 dB),(1 step is 1 dB)
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetLineInADCPGAGain(INT8 bPGA_gain)
{
	UINT32 /*cfg51=0,william need to update*/PGA_reg=0;

	if((bPGA_gain < DRV_AUD_PGA_MIN)||(bPGA_gain > DRV_AUD_PGA_MAX))
		return DRV_ERR_INVALID_PARAM;

	/*the PGA control is bit 16~20*/

	/*calculate the PGA register value*/
	PGA_reg = (31 - (bPGA_gain + 25));

	HWREG_W(sft_cfg_40_dtv, ((HWREG_R(sft_cfg_40_dtv)&0xffe0ffff)|PGA_reg));


    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Get audio line-in ADC PGA gain
* @param    pbPGA_gain : -25(mute) ~ 6(6 dB),(1 step is 1 dB)
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudGetLineInADCPGAGain(INT8 *pbPGA_gain)
{
	UINT32 PGA_reg=0;

	if(pbPGA_gain == DTV_NULL)
	{
    	return DRV_ERR_INVALID_PARAM;
	}
	else
	{
		/*the PGA control is bit 16~20*/
		PGA_reg = ((HWREG_R(sft_cfg_40_dtv))&0x001f0000)>>16;
		*pbPGA_gain = (31 - (INT8)(PGA_reg + 25));
//		DRV_AUDIO_DBG("DRV_AudGetPFAGain get cfg51=%x PGA_reg=%x PGA_gain=%d\n",//william need to update regs0->sft_cfg51,PGA_reg,*PGA_gain);
		return DRV_SUCCESS;
	}
}

/******************************************************************************/
/**
* @brief    Set audio SIF ADC PGA gain
* @param    bPGA_gain : -6(mute) ~ 25(25 dB),(1 step is 1 dB)
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetSIFADCPGAGain(INT8 bPGA_gain)
{
	UINT32 /*cfg52=0,william need to update*/PGA_reg=0;

	if((bPGA_gain < DRV_AUD_PGA_MIN)||(bPGA_gain > DRV_AUD_PGA_MAX))
		return DRV_ERR_INVALID_PARAM;

	/*calculate the PGA register value*/
	PGA_reg = (bPGA_gain + 6);
	HWREG_W(sft_cfg_41_dtv, ((HWREG_R(sft_cfg_41_dtv)&0xffffffe0)|PGA_reg));
	DRV_AUDIO_DBG("SIFADC Conf1=%x\n",(HWREG_R(sft_cfg_41_dtv)));

    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Get audio SIF ADC PGA gain
* @param    pbPGA_gain : -6(mute) ~ 25(25 dB),(1 step is 1 dB)
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudGetSIFADCPGAGain(INT8 *pbPGA_gain)
{
	UINT32 PGA_reg=0;

	if(pbPGA_gain == DTV_NULL)
	{
		return DRV_ERR_INVALID_PARAM;
	}
	else
	{
		/*the PGA control is bit 0~4*/
		PGA_reg = ((HWREG_R(sft_cfg_41_dtv))&0x0000001f);
		*pbPGA_gain = (INT8)(PGA_reg - 6);
//		DRV_AUDIO_DBG("DRV_AudGetPFAGain get cfg52=%x PGA_reg=%x PGA_gain=%d\n",//william need to update regs0->sft_cfg52,PGA_reg,*PGA_gain);
		return DRV_SUCCESS;
	}
}

/******************************************************************************/
/**
* @brief    Set audio SIF AGC
* @param    fEnAGC : DTV_TRUE : enable AGC
*				   DTV_FALSE : disable AGC
* @param    fHysteresisFastMode : DTV_TRUE : Hysteresis is fast mode
*								DTV_FALSE : Hysteresis is slow mode
* @param    bGain : SIF AGC gain, -6(mute) ~ 25(25 dB),(1 step is 1 dB)
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudSetSIFAGCConfig(Boolean fEnAGC,
                                            Boolean fHysteresisFastMode,
                                            INT8 bGain)
{
    UINT8 bOffset;

    bOffset = REG_SSD_AGC_CTL_V_MAX - DRV_AUD_SIF_AGC_MAX;

    if(fHysteresisFastMode == DTV_TRUE)
    {
        HW_SetReg(reg_ssd_agc_hysteresis, REG_SSD_AGC_HYSTERESIS_V_FAST_MODE);
    }
    else
    {
        HW_SetReg(reg_ssd_agc_hysteresis, REG_SSD_AGC_HYSTERESIS_V_SLOW_MODE);
    }

    if(fEnAGC == DTV_TRUE)
    {
        HW_SetReg(reg_ssd_agc_off, REG_SSD_AGC_OFF_V_AGC_NOT_OFF);
    }
    else
    {
        if((bGain < DRV_AUD_SIF_AGC_MIN)
            ||(bGain > DRV_AUD_SIF_AGC_MAX))
        {
            return DRV_ERR_INVALID_PARAM;
        }
        else
        {
            HW_SetReg(reg_ssd_agc_ctl, (bGain + bOffset));
        }
        HW_SetReg(reg_ssd_agc_off, REG_SSD_AGC_OFF_V_AGC_OFF);
    }
    return DRV_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Get audio SIF AGC gain
* @param    pbRetGain : SIF AGC status report, -6(mute) ~ 25(25 dB),(1 step is 1 dB)
* @return   DRV_SUCCESS if successful
*			DRV_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
DRV_Status_t DRV_AudGetSIFAGCGain(INT8 *pbRetGain)
{
    UINT8 bOffset;

    if(pbRetGain == DTV_NULL)
    {
        return DRV_ERR_INVALID_PARAM;
    }
    else
    {
        bOffset = REG_SSD_AGC_STATUS_V_MAX - DRV_AUD_SIF_AGC_MAX;

        *pbRetGain = (HW_GetReg(reg_ssd_agc_status) - bOffset);

        return DRV_SUCCESS;
    }
}

/*!
    @} end of addtogroup Audio */

