/******************************************************************************/
/*!
* @file    drv_audio_ssd_access.c
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
#include <asm/mach-spv9200/ic-spv9200/hw_public.h>
//#include <asm/mach-spv9200/ic-spv9200/hw_access.h>
//#include <asm/mach-spv9200/ic-spv9200/hw_bank6_0_ssd_def.h>
#include "drv_ssd_resource.h"
#include "hw_ssd_def.h"
#include "drv_ssd_access.h"

/*******************************************************************************
* Constant
******************************************************************************/
/**
 * @brief  specify the High Latency for the DTL bus
 */
#define HW_DTL_HIGH_LATENCY_OFFSETESS_RANGE_START   0x00000
#define HW_DTL_HIGH_LATENCY_OFFSETESS_RANGE_END     0x01FFF

/**
 * @brief  specify non-BAM register address
 */
#define HW_DTL_BAM_REGISTER_OFFSETESS               REG_Ssd_bam_en_OFFSET

/**
 * @brief  specify non-BAM register address
 */
#define HW_DTL_WRITE_TRIGGER                       0x02
#define HW_DTL_READ_TRIGGER                        0x01

/**
 * @brief  use fixed delay time for stable DTL output if any error from hardware
 */
#define HW_DTL_NOT_USE_RFT                         1

/**
 * @brief  debug mode for check register value for SSD
 */
#if(HW_DTL_DEBUG_MODE == HW_DTL_DEBUG_MODE_ENABLE)
#define HW_DTL_DEBF(fmt, arg...)                   diag_printf("\n@DTL "fmt, ##arg)
#else
#define HW_DTL_DEBF(fmt, arg...)
#endif



/*******************************************************************************
* Structure
******************************************************************************/

/*******************************************************************************
* Variable
******************************************************************************/
//static GL_Mutex_t  gDtlMutex;
static Boolean      gfDtlInitFlag   =DTV_FALSE;

/*******************************************************************************
* Internal used
******************************************************************************/
#if (HW_DTL_NOT_USE_RFT==1)
/**
 * @brief   delay ms (for 333)
 * @param   sMillisecond
 * @return  None
 */
 void HW_DTL_Delay_Ms(UINT32 sMillisecond)
{
       UINT32   i, j;
       for(i=0; i<sMillisecond; i++)
       {
           for(j=0; j<0x3800; j++)
               ;
       }
}
#endif

/**
 * @brief  This function Checks if a High Latency request is done.
 * @param  address Valid DTL target address
 * @return Returns TRUE when a high latency request is submitted.
 */
Boolean HW_DTL_IsHighLatencyRequest(UINT32 dAddress)
{
    Boolean high_latency_request    =DTV_FALSE;
    if(dAddress<=HW_DTL_HIGH_LATENCY_OFFSETESS_RANGE_END)
    {
        high_latency_request=DTV_TRUE;
    }
    return (high_latency_request);
}

/**
 * @brief  This function enables the BAM bit.
 * @param  fEn enable/disable the BAM bit
 */
void HW_DTL_EnableBAMMode(Boolean fEn)
{
    HW_SetReg(reg_ssd_dtl_dtl_sta_adr_2, (UINT8)(REG_Ssd_bam_en_OFFSET>>16));
    HW_SetReg(reg_ssd_dtl_dtl_sta_adr_1, (UINT8)(REG_Ssd_bam_en_OFFSET>>8));
    HW_SetReg(reg_ssd_dtl_dtl_sta_adr_0, (UINT8)REG_Ssd_bam_en_OFFSET);

    if(fEn)
    {
        HW_SetReg(reg_ssd_dtl_dtl_wdat_3, (UINT8)(REG_Ssd_bam_en_BIT_MASK>>24));
        HW_SetReg(reg_ssd_dtl_dtl_wdat_2, (UINT8)(REG_Ssd_bam_en_BIT_MASK>>16));
        HW_SetReg(reg_ssd_dtl_dtl_wdat_1, (UINT8)(REG_Ssd_bam_en_BIT_MASK>>8));
        HW_SetReg(reg_ssd_dtl_dtl_wdat_0, (UINT8)REG_Ssd_bam_en_BIT_MASK);
    }
    else
    {
        HW_SetReg(reg_ssd_dtl_dtl_wdat_3, 0x00);
        HW_SetReg(reg_ssd_dtl_dtl_wdat_2, 0x00);
        HW_SetReg(reg_ssd_dtl_dtl_wdat_1, 0x00);
        HW_SetReg(reg_ssd_dtl_dtl_wdat_0, 0x00);
    }
#if 0//gjshen modify@20090518
    HW_SetReg(REG_Ctrl_b610, HW_DTL_WRITE_TRIGGER);
#else
		HW_SetReg(reg_ssd_dtl_dtl_write, REG_SSD_DTL_DTL_WRITE_V_MAX);
#endif
}

/**
 * @brief  This function waits for the RFT flag (or a timeout occurs).
 * @return Returns TRUE a timeout occurred.
 */
Boolean HW_DTL_WaitForRFT(void)
{
    UINT8   cmd_done;
    UINT8   RFT_flag;
    Boolean cmd_error;
    UINT8   timeout =HW_DTL_MAX_RETRY_VALUE;
    do
    {
        HW_SetReg(reg_ssd_dtl_dtl_sta_adr_2,
                  (UINT8)(REG_Ssd_int_status_tnsact_grant_OFFSET>>16));
        HW_SetReg(reg_ssd_dtl_dtl_sta_adr_1,
                  (UINT8)(REG_Ssd_int_status_tnsact_grant_OFFSET>>8));
        HW_SetReg(reg_ssd_dtl_dtl_sta_adr_0,
                  (UINT8)REG_Ssd_int_status_tnsact_grant_OFFSET);
#if 0//gjshen modify@20090518
        HW_SetReg(reg_ssd_dtl_b610, HW_DTL_READ_TRIGGER);
#else
	HW_SetReg(reg_ssd_dtl_dtl_read, REG_SSD_DTL_DTL_READ_V_MAX);
#endif

        RFT_flag=(HW_GetReg(reg_ssd_dtl_dtl_rdat_3)&((UINT8)
                                                  (REG_Ssd_int_status_tnsact_grant_BIT_MASK>>
                                                   24)));
        if(RFT_flag)
        {
            cmd_done=0;
        }
        else
        {
            cmd_done=1;
        }
    }
    while((cmd_done)&&(timeout--));
    if(timeout)
    {
        cmd_error=DTV_FALSE;
    }
    else
    {
        cmd_error=DTV_TRUE;
    }
    return (cmd_error);
}

/**
 * @brief  This function clears the RFT flag.
 */
void HW_DTL_ClearRFT(void)
{
    HW_SetReg(reg_ssd_dtl_dtl_sta_adr_2,
              (UINT8)(REG_Ssd_int_clear_status_tnsact_grant_OFFSET>>16));
    HW_SetReg(reg_ssd_dtl_dtl_sta_adr_1,
              (UINT8)(REG_Ssd_int_clear_status_tnsact_grant_OFFSET>>8));
    HW_SetReg(reg_ssd_dtl_dtl_sta_adr_0,
              (UINT8)REG_Ssd_int_clear_status_tnsact_grant_OFFSET);

    HW_SetReg(reg_ssd_dtl_dtl_wdat_3,
              (UINT8)(REG_Ssd_int_clear_status_tnsact_grant_BIT_MASK>>24));
    HW_SetReg(reg_ssd_dtl_dtl_wdat_2,
              (UINT8)(REG_Ssd_int_clear_status_tnsact_grant_BIT_MASK>>16));
    HW_SetReg(reg_ssd_dtl_dtl_wdat_1,
              (UINT8)(REG_Ssd_int_clear_status_tnsact_grant_BIT_MASK>>8));
    HW_SetReg(reg_ssd_dtl_dtl_wdat_0,
              (UINT8)REG_Ssd_int_clear_status_tnsact_grant_BIT_MASK);
#if 0//gjshen modify@20090518
    HW_SetReg(REG_Ctrl_b610, HW_DTL_WRITE_TRIGGER);
#else
	HW_SetReg(reg_ssd_dtl_dtl_write, REG_SSD_DTL_DTL_WRITE_V_MAX);
#endif
}

/*******************************************************************************
* Program
******************************************************************************/
/**
 * @brief  SSD DTL initial
 */
void HW_DTL_Init(void)
{
    if(gfDtlInitFlag==DTV_FALSE)
    {
        //GL_MutexCreate("gDtlMutex",&gDtlMutex);
        gfDtlInitFlag=DTV_TRUE;
    }
}

/**
 * @brief  SSD DTL un-initial
 */
void HW_DTL_Uninit(void)
{
    if(gfDtlInitFlag==DTV_TRUE)
    {
        //GL_MutexDelete(gDtlMutex);
        gfDtlInitFlag=DTV_FALSE;
    }
}

/**
 * @brief  This function read data from SSD registers.
 * @param  dDTLAddr SSD register address
 * @param  pdDTLData point to SSD register data
 */
void HW_DTL_Read(UINT32 dDTLAddr, UINT32 *pdDTLData)
{
#ifdef HW_DTL_NO_ACTION
	return;
#endif
    HW_DTL_DEBF("DTL_Read Addr<= %x\n", dDTLAddr);
    //GL_MutexLock(gDtlMutex);

#if (HW_DTL_NOT_USE_RFT==1)

    /* write DTL address*/
    HW_SetReg(reg_ssd_dtl_dtl_sta_adr_2, (UINT8)(dDTLAddr>>16));
    HW_SetReg(reg_ssd_dtl_dtl_sta_adr_1, (UINT8)(dDTLAddr>>8));
    HW_SetReg(reg_ssd_dtl_dtl_sta_adr_0, dDTLAddr);

    /* enable DTL_READ */
#if 0//gjshen modify@20090518
    HW_SetReg(REG_Ctrl_b610, HW_DTL_READ_TRIGGER);
#else
    HW_SetReg(reg_ssd_dtl_dtl_read, REG_SSD_DTL_DTL_READ_V_MAX);
#endif
    HW_DTL_Delay_Ms(1);

    /*Read DTL Data*/
    *pdDTLData=(HW_GetReg(reg_ssd_dtl_dtl_rdat_3)<<24)|\
    (HW_GetReg(reg_ssd_dtl_dtl_rdat_2)<<16)|\
    (HW_GetReg(reg_ssd_dtl_dtl_rdat_1)<<8)|\
    HW_GetReg(reg_ssd_dtl_dtl_rdat_0);
    HW_DTL_Delay_Ms(1);

#else
    UINT8   cmd_done;
    UINT8   timeout =HW_DTL_MAX_RETRY_VALUE;

    if(HW_DTL_IsHighLatencyRequest(dDTLAddr))
    {
        HW_DTL_EnableBAMMode(DTV_TRUE);
        timeout=HW_DTL_MAX_RETRY_VALUE;
        do
        {
            HW_DTL_ClearRFT();

            /* write DTL address*/
            HW_SetReg(reg_ssd_dtl_dtl_sta_adr_2, (UINT8)(dDTLAddr>>16));
            HW_SetReg(reg_ssd_dtl_dtl_sta_adr_1, (UINT8)(dDTLAddr>>8));
            HW_SetReg(reg_ssd_dtl_dtl_sta_adr_0, dDTLAddr);

            /* enable DTL_READ */
#if 0//gjshen modify@20090518
            HW_SetReg(REG_Ctrl_b610, HW_DTL_READ_TRIGGER);
#else
			HW_SetReg(reg_ssd_dtl_dtl_read, REG_SSD_DTL_DTL_READ_V_MAX);
#endif
            cmd_done=HW_DTL_WaitForRFT();

            /* write DTL address*/
            HW_SetReg(reg_ssd_dtl_dtl_sta_adr_2, (UINT8)(dDTLAddr>>16));
            HW_SetReg(reg_ssd_dtl_dtl_sta_adr_1, (UINT8)(dDTLAddr>>8));
            HW_SetReg(reg_ssd_dtl_dtl_sta_adr_0, dDTLAddr);

            /* enable DTL_READ */
#if 0//gjshen modify@20090518
            HW_SetReg(REG_Ctrl_b610, HW_DTL_READ_TRIGGER);
#else
			HW_SetReg(reg_ssd_dtl_dtl_read, REG_SSD_DTL_DTL_READ_V_MAX);
#endif

            /*Read DTL Data*/
            *pdDTLData=(HW_GetReg(reg_ssd_dtl_dtl_rdat_3)<<24)|\
            (HW_GetReg(reg_ssd_dtl_dtl_rdat_2)<<16)|\
            (HW_GetReg(reg_ssd_dtl_dtl_rdat_1)<<8)|\
            HW_GetReg(reg_ssd_dtl_dtl_rdat_0);
        }
        while((cmd_done)&&(timeout--));
    }
    else
    {
        if(dDTLAddr!=HW_DTL_BAM_REGISTER_OFFSETESS)
        {
            HW_DTL_EnableBAMMode(DTV_FALSE);
        }
        /* write DTL address*/
        HW_SetReg(reg_ssd_dtl_dtl_sta_adr_2, (UINT8)(dDTLAddr>>16));
        HW_SetReg(reg_ssd_dtl_dtl_sta_adr_1, (UINT8)(dDTLAddr>>8));
        HW_SetReg(reg_ssd_dtl_dtl_sta_adr_0, dDTLAddr);

        /* enable DTL_READ */
#if 0//gjshen modify@20090518
        HW_SetReg(REG_Ctrl_b610, HW_DTL_READ_TRIGGER);
#else
			HW_SetReg(reg_ssd_dtl_dtl_read, REG_SSD_DTL_DTL_READ_V_MAX);
#endif

        /*Read DTL Data*/
        *pdDTLData=(HW_GetReg(reg_ssd_dtl_dtl_rdat_3)<<24)|\
        (HW_GetReg(reg_ssd_dtl_dtl_rdat_2)<<16)|\
        (HW_GetReg(reg_ssd_dtl_dtl_rdat_1)<<8)|\
        HW_GetReg(reg_ssd_dtl_dtl_rdat_0);
    }

#endif

    //GL_MutexUnlock(gDtlMutex);
    HW_DTL_DEBF("DTL_Read Data<= %x\n", *pdDTLData);
}

/**
 * @brief  This function write data from SSD registers.
 * @param  dDTLAddr SSD register address
 * @param  dDTLData SSD register data
 */
void HW_DTL_Write(UINT32 dDTLAddr, UINT32 dDTLData)
{
#ifdef HW_DTL_NO_ACTION
	return;
#endif

    HW_DTL_DEBF("DTL_Write-> Addr %x\n", dDTLAddr);
    HW_DTL_DEBF("DTL_Write-> Data %x\n", dDTLData);
    //GL_MutexLock(gDtlMutex);

#if (HW_DTL_NOT_USE_RFT==1)

    /* write DTL address*/
    HW_SetReg(reg_ssd_dtl_dtl_sta_adr_2, (UINT8)(dDTLAddr>>16));
    HW_SetReg(reg_ssd_dtl_dtl_sta_adr_1, (UINT8)(dDTLAddr>>8));
    HW_SetReg(reg_ssd_dtl_dtl_sta_adr_0, dDTLAddr);

    /* write DTL data */
    HW_SetReg(reg_ssd_dtl_dtl_wdat_3, (UINT8)(dDTLData>>24));
    HW_SetReg(reg_ssd_dtl_dtl_wdat_2, (UINT8)(dDTLData>>16));
    HW_SetReg(reg_ssd_dtl_dtl_wdat_1, (UINT8)(dDTLData>>8));
    HW_SetReg(reg_ssd_dtl_dtl_wdat_0, (UINT8)dDTLData);

    /* enable DTL_WRITE */
#if 0//gjshen modify@20090518
    HW_SetReg(REG_Ctrl_b610, HW_DTL_WRITE_TRIGGER);
#else
	HW_SetReg(reg_ssd_dtl_dtl_write, REG_SSD_DTL_DTL_WRITE_V_MAX);
#endif
    HW_DTL_Delay_Ms(1);

#else

    UINT8   cmd_done;
    UINT8   timeout =HW_DTL_MAX_RETRY_VALUE;

    if(HW_DTL_IsHighLatencyRequest(dDTLAddr))
    {
        HW_DTL_EnableBAMMode(DTV_TRUE);
        timeout=HW_DTL_MAX_RETRY_VALUE;
        do
        {
            HW_DTL_ClearRFT();

            /* write DTL address*/
            HW_SetReg(reg_ssd_dtl_dtl_sta_adr_2, (UINT8)(dDTLAddr>>16));
            HW_SetReg(reg_ssd_dtl_dtl_sta_adr_1, (UINT8)(dDTLAddr>>8));
            HW_SetReg(reg_ssd_dtl_dtl_sta_adr_0, dDTLAddr);

            /* write DTL data */
            HW_SetReg(reg_ssd_dtl_dtl_wdat_3, (UINT8)(dDTLData>>24));
            HW_SetReg(reg_ssd_dtl_dtl_wdat_2, (UINT8)(dDTLData>>16));
            HW_SetReg(reg_ssd_dtl_dtl_wdat_1, (UINT8)(dDTLData>>8));
            HW_SetReg(reg_ssd_dtl_dtl_wdat_0, (UINT8)dDTLData);

            /* enable DTL_WRITE */
#if 0//gjshen modify@20090518
            HW_SetReg(REG_Ctrl_b610, HW_DTL_WRITE_TRIGGER);
#else
			HW_SetReg(reg_ssd_dtl_dtl_write, REG_SSD_DTL_DTL_WRITE_V_MAX);
#endif
            cmd_done=HW_DTL_WaitForRFT();
        }
        while((cmd_done)&&(timeout--));
    }
    else
    {
        if(dDTLAddr!=HW_DTL_BAM_REGISTER_OFFSETESS)
        {
            HW_DTL_EnableBAMMode(DTV_FALSE);
        }
        /* write DTL address*/
        HW_SetReg(reg_ssd_dtl_dtl_sta_adr_2, (UINT8)(dDTLAddr>>16));
        HW_SetReg(reg_ssd_dtl_dtl_sta_adr_1, (UINT8)(dDTLAddr>>8));
        HW_SetReg(reg_ssd_dtl_dtl_sta_adr_0, dDTLAddr);

        /* write DTL data */
        HW_SetReg(reg_ssd_dtl_dtl_wdat_3, (UINT8)(dDTLData>>24));
        HW_SetReg(reg_ssd_dtl_dtl_wdat_2, (UINT8)(dDTLData>>16));
        HW_SetReg(reg_ssd_dtl_dtl_wdat_1, (UINT8)(dDTLData>>8));
        HW_SetReg(reg_ssd_dtl_dtl_wdat_0, (UINT8)dDTLData);

        /* enable DTL_WRITE */
#if 0//gjshen modify@20090518
        HW_SetReg(REG_Ctrl_b610, HW_DTL_WRITE_TRIGGER);
#else
			HW_SetReg(reg_ssd_dtl_dtl_write, REG_SSD_DTL_DTL_WRITE_V_MAX);
#endif
    }

#endif

    //GL_MutexUnlock(gDtlMutex);
}

/******************************************************************************/
/**
* @brief  This function read data from SSD with double word operation.
* @param  dSSDAddr         SSD DTL Address
* @return SSD data
******************************************************************************/
/* Need to check if return value is needed */
UINT32 HW_SSDGetDWord(UINT32 dSSDAddr)
{
    UINT32  dSSDData;
    HW_DTL_Read(dSSDAddr, &dSSDData);
    return dSSDData;
}

/******************************************************************************/
/**
* @brief  This function read data from SSD with double word operation.
* @param  dSSDAddr         SSD DTL Address
* @return SSD data
******************************************************************************/
/* Need to check if return value is needed */
void HW_SSDSetDWord(UINT32 dSSDAddr, UINT32 dSSDData)
{
    HW_DTL_Write(dSSDAddr, dSSDData);
}

/******************************************************************************/
/**
* @brief  This function read data from SSD with bit operation.
* @param  dSSDAddr         SSD DTL Address
* @param  dSSDDataBitMask  SSD data mask
* @param  bSSDDataBitIndex lowest bit index of SSD data (0..31)
* @return SSD data
******************************************************************************/
/* Need to check if return value is needed */
UINT32 HW_SSDGetBit(UINT32 dSSDAddr,
                    UINT32 dSSDDataBitMask,
                    UINT8 bSSDDataBitIndex)
{
    UINT32  dSSDData = 0;

    HW_DTL_Read(dSSDAddr, &dSSDData);
    dSSDData=(dSSDData&dSSDDataBitMask)>>bSSDDataBitIndex;
    return dSSDData;
}

/******************************************************************************/
/**
* @brief  This function write data from SSD with bit operation.
* @param  dSSDAddr         SSD DTL Address
* @param  dSSDDataBitMask  SSD data mask
* @param  bSSDDataBitIndex lowest bit index of SSD data (0..31)
* @param  dSSDData         SSD data
******************************************************************************/
void HW_SSDSetBit(UINT32 dSSDAddr,
                  UINT32 dSSDDataBitMask,
                  UINT8 bSSDDataBitIndex,
                  UINT32 dSSDData)
{
    UINT32  u32Tmp;

    HW_DTL_Read(dSSDAddr, &u32Tmp);

    u32Tmp&=(~dSSDDataBitMask);

    dSSDData=(dSSDData<<bSSDDataBitIndex)&dSSDDataBitMask;
    HW_DTL_Write(dSSDAddr, (u32Tmp|dSSDData));
}

