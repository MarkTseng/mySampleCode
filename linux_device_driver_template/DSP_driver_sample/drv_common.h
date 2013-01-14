/** @addtogroup common
 * @{
 */
/******************************************************************************/
/**
 *
 * @file    drv_common.h
 *
 * @brief   This file specifies general used constant and data type.
 *
 * @note    Copyright (c) 2006 Sunplus Technology Co., Ltd. \n
 *          All rights reserved.
 *
 * @author
 *
 ******************************************************************************/
#ifndef __DRV_COMMON_H__
#define __DRV_COMMON_H__



/*******************************************************************************
 * macros
 ******************************************************************************/
/*******************************************************************************
 * type definitions
 ******************************************************************************/
/**
 *	\brief General Driver return status
 */

typedef enum
{
	/* common status values */
	DRV_SUCCESS,				/*!< successful outcome					*/
	DRV_ERR_FAILURE,			/*!< operation failed					*/
	DRV_ERR_INVALID_HANDLE,		/*!< invalid handle						*/
	DRV_ERR_INVALID_ID,			/*!< invalid identifier					*/
	DRV_ERR_INVALID_PARAM,		/*!< invalid parameter					*/
	DRV_ERR_INVALID_OP,			/*!< requested operation is invalid		*/
	DRV_ERR_MEMORY_ALLOC,		/*!< problem allocating memory			*/
	DRV_ERR_MEMORY_SIZE,		/*!< problem with the size of memory	*/
									/* < supplied							*/
	DRV_ERR_RESOURCE_UNAVAILABLE,
	DRV_ERR_TIMEOUT,			/*!< timeout							*/
	DRV_WARN_NO_ACTION,			/* < the function completed successfully,*/
	                                /* < but no action was taken            */
	DRV_WARN_PARAM_CLIPPED,		/*!< the function has completed			*/
									/*!< successfully, though a parameter was	*/
									/*!< clipped to within a valid range.		*/
	DRV_WARN_BUFFER_EMPTY,
	DRV_WARN_BUFFER_FULL,
	DRV_WARN_UNINITED,			/*!< driver has not been initialized yet */
	DRV_WARN_INITED,			/*!< driver has been initialized already */
	DRV_ERR_MODE_MISMATCH,		/*!< deinterlacer off*/
	DRV_ERR_MAX					/*!< Max error number*/
} DRV_Status_t;


#define DRV_TIMEOUT_FOREVER 0	/*!< infinite waiting */


#ifdef DRV_DEBUG
extern DRV_Status_t drv_ret;
extern char* drv_ret_str[];
#define drv_ret_check() if (drv_ret!=DRV_SUCCESS) diag_printf("DRV Error: %s, %s,%d\n",drv_ret_str[drv_ret],__FILE__,__LINE__)	/*!< return check */
#else
#define drv_ret_check() {}	/*!< return check */
#endif
/* @} end of addtogroup common*/
#endif


