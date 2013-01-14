/******************************************************************************/
/*! @addtogroup Audio
*  @{
******************************************************************************/
/******************************************************************************/
/*!
* @file    drv_audio_resource.h
*
* @brief   This file specifies the APIs provided to control audio input device.
*
* @note     Copyright (c) 2007 Sunplus Technology Co., Ltd. \n
*          All rights reserved.
*
********************************************************************************/
#ifndef _DRV_AUDIO_RESOURCE_H_
#define _DRV_AUDIO_RESOURCE_H_

/*******************************************************************************
* Header include
******************************************************************************/
//#include "types.h"
#include "drv_common.h"
//#include "gsl.h"
#include <asm/mach-spv9200/types.h>

/*******************************************************************************
* constant
******************************************************************************/

/*******************************************************************************
* enumeration
******************************************************************************/

/*******************************************************************************
* Structure
******************************************************************************/

/*******************************************************************************
* Variable
******************************************************************************/

/*******************************************************************************
* Marco
******************************************************************************/

/*bridge OS relative data type or function */
//#define OS_MUTEX			GL_Mutex_t
#define OS_TASK_ARGUMENT	cyg_addrword_t

//#define OS_MUTEX_INIT(pParam)   	cyg_mutex_init(pParam)
//#define OS_MUTEX_LOCK(pParam)   	cyg_mutex_lock(pParam)
//#define OS_MUTEX_UNLOCK(pParam) 	cyg_mutex_unlock(pParam)
//#define OS_MUTEX_DESTROY(pParam)	cyg_mutex_destroy(pParam)

#define OS_TASK_CREATE(Name,pArg,pTaskArg,dPriority,dStackSize,fTaskRun,pTaskId)\
		DTV_GL_TaskCreate(Name, pArg, pTaskArg, dPriority, dStackSize, fTaskRun, pTaskId)
#define OS_TASK_SLEEP(Param)    	GL_TaskSleep(Param)
#define OS_TASK_SEL_DELETE()		GL_TaskSelfDelete()

/*******************************************************************************
* Program
******************************************************************************/

#endif /* _DRV_AUDIO_RESOURCE_H_ */

/*!
    @} end of addtogroup Audio */

