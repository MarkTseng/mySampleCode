/******************************************************************************/
/*!
* @file    drv_audio_ssd_access.h
*
* @brief   This file specifies the APIs provided to control audio input device.
*
* @note     Copyright (c) 2007 Sunplus Technology Co., Ltd. \n
*          All rights reserved.
*
********************************************************************************/

#ifndef __DRV_AUDIO_SSD_ACCESS_H__
#define __DRV_AUDIO_SSD_ACCESS_H__
/*******************************************************************************
* Header include
******************************************************************************/

/*******************************************************************************
* configuration
******************************************************************************/
#define HW_DTL_DEBUG_MODE_ENABLE    1
#define HW_DTL_DEBUG_MODE_DISABLE   0
#define HW_DTL_DEBUG_MODE           HW_DTL_DEBUG_MODE_DISABLE
//#define HW_DTL_NO_ACTION

/*******************************************************************************
* constant
******************************************************************************/
/**
 * @brief  maximum number of retries for DTL interface read/write.
 */
#define HW_DTL_MAX_RETRY_VALUE                    127

/*******************************************************************************
* Structure
******************************************************************************/

/*******************************************************************************
* Variable
******************************************************************************/

/*******************************************************************************
* Marco
******************************************************************************/
/******************************************************************************/
/**
* @brief Get SSD register value
******************************************************************************/
#define HW_SSDGetReg(reg)               HW_SSDGetBit(reg##_OFFSET, reg##_BIT_MASK, reg##_BIT_IDX)

/******************************************************************************/
/**
* @brief Set SSD register value
******************************************************************************/
#define HW_SSDSetReg(reg, value)        HW_SSDSetBit(reg##_OFFSET, reg##_BIT_MASK, reg##_BIT_IDX, value)

/******************************************************************************/
/**
* @brief Check SSD register value
******************************************************************************/
#define HW_SSDCheckRegRange(reg, value)\
{\
	if((value < reg##_MIN)\
        ||(value > reg##_MAX))\
    {\
        return DRV_ERR_INVALID_PARAM;\
    }\
}\

/******************************************************************************/
/**
* @brief tranlate register unsign value to sign value
******************************************************************************/
#define HW_SSDSignExpand(reg, ret)\
{\
    ret = HW_SSDGetBit(reg##_OFFSET, reg##_BIT_MASK, reg##_BIT_IDX);\
	if (((ret) & ((reg##_SIGNED_BIT_MASK) >> (reg##_BIT_IDX))) != 0)\
    {\
        ret |= ~((reg##_BIT_MASK) >> (reg##_BIT_IDX));\
    }\
    else\
    {\
        ret &= ((reg##_BIT_MASK) >> (reg##_BIT_IDX));\
    }\
}\

/*******************************************************************************
* Program
******************************************************************************/
/**
 * @brief  SSD DTL initial
 */
void HW_DTL_Init(void);

/**
 * @brief  SSD DTL un-initial
 */
void HW_DTL_Uninit(void);

/**
 * @brief  This function read data from SSD registers.
 * @param  dDTLAddr SSD register address
 * @param  pdDTLData point to SSD register data
 */
void HW_DTL_Read(UINT32 dDTLAddr, UINT32 *pdDTLData);

/**
 * @brief  This function write data from SSD registers.
 * @param  dDTLAddr SSD register address
 * @param  dDTLData SSD register data
 */
void HW_DTL_Write(UINT32 dDTLAddr, UINT32 dDTLData);

/******************************************************************************/
/**
* @brief  This function read data from SSD with double word operation.
* @param  dSSDAddr         SSD DTL Address
* @return SSD data
******************************************************************************/
/* Need to check if return value is needed */
UINT32 HW_SSDGetDWord(UINT32 dSSDAddr);

/******************************************************************************/
/**
* @brief  This function read data from SSD with double word operation.
* @param  dSSDAddr         SSD DTL Address
* @return SSD data
******************************************************************************/
/* Need to check if return value is needed */
void HW_SSDSetDWord(UINT32 dSSDAddr, UINT32 dSSDData);

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
                    UINT8 bSSDDataBitIndex);

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
                  UINT32 dSSDData);

#endif /* __DRV_AUDIO_SSD_ACCESS_H__ */

