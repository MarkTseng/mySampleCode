/******************************************************************************
 * Filename    : dsp_cmd.c
 * 
 * Description : audio dsp command protocol handling implementation file
 * Language    : C Language 
 *           
 * Revision History: 
 * ------------------
 * [090804]hcsu Initial Draft
 *
 * -----------------------------------------------------------------------
 * Copyright (C) 2007-2009 Sunplus Technology Co., Ltd. 
 * All rights reserved. The information contained herein is the exclusive 
 * property of SUNPLUS TECHNOLOGY CO., LTD. and shall not be distributed, 
 * reproduced, or disclosed in whole or in part without prior written 
 * permission of SUNPLUS TECHNOLOGY CO., LTD.   
 *****************************************************************************/

/******************************************************************************
	Include files
 *****************************************************************************/ 
#include "include/setting.h"
//#include "stypes.h"

#include "include/aud_global.h"
#include "include/dsp_api.h"
#include <linux/time.h>
#include <asm/mach-spv9200/regmap_9200.h>

/******************************************************************************
	Local Defines
 *****************************************************************************/ 
#define ENABLE_INTERRUPT        			

#define K_INVALID_DSP_ADDRESS		(0xFFFFFFFF)


/******************************************************************************
    global&static variables
 *****************************************************************************/
//GL_Mutex_t gDspCmdMutex = NULL;

// a flag record if force to reset dsp or not
UINT16 gDSP_Sendcmd_Timeout=0;

static UINT32 DSP_GetValidDSPAddress(UINT32 address)
{

	// QAE377S - dsp address: 0x00000 ~ 0x07FFF
	//           rom address: 0x20000 ~ 0x27FFF (don't care)
	// QAE377A - dsp address: 0x19000 ~ 0x20FFF
	//           rom address: 0x39000 ~ 0x40FFF	(don't care)
	
	int address_offset = (int) address;  		

#ifdef DSP_NEW_MEMORY_LAYOUT
	address_offset = (int) address - 0x19000;
#endif
	
    // Check if exceed valid IDM range
    if (address_offset >= DSP_MAX_IDM_ADDRESS)
    {
		return K_INVALID_DSP_ADDRESS;
    }    	

	return address_offset;
}


/******************************************************************************
	function implementations
 *****************************************************************************/
UINT32 DSP_SendCmd(UINT8 CmdID, UINT8 CmdParam, UINT16 OptParam, UINT32 PortParam)
{   
	DSPCMD_MUTEX_LOCK();

    UINT32 CommandPort = CmdID<<24 | CmdParam<<16 | OptParam;

	DSP_PRINT("DSP_SendCmd::CmdID:0x%02x, CmdParam:0x%02x, OptParam:0x%04x, PortParam:0x%08x\n",CmdID,CmdParam,OptParam,PortParam);	
	
#ifdef ENABLE_INTERRUPT
	if (GET_DSP_INT0())
	{
		DSP_PRINT("DSP_SendCmd::e:wait previous interrupt done!\n");
		DSPCMD_MUTEX_UNLOCK();
		return DSP_ERROR_CMD_WAIT;
	}
#endif	

    // Check if Cmd Port is valid
    if (0 == GET_CMD_VALID_BIT()) 
    {
        DSP_PRINT("DSP_SendCmd::e:wait previous command done!\n");
		DSPCMD_MUTEX_UNLOCK();
        return DSP_ERROR_CMD_WAIT;
    }

    // Write Port Parameter
    SET_CMD_PARAM_PORT(PortParam);

    // Write Command 
    SET_CMD_CODE_PORT(CommandPort); 

	DSP_PRINT("DSP_SendCmd::cmd:0x%08x, param:0x%08x\n",CommandPort,PortParam);	

    // Issue Interrupt to DSP?
#ifdef ENABLE_INTERRUPT 
    SET_DSP_INT0();	
#endif

#ifdef AUD_Enable_TimeoutProtect
	//UINT64 timer_rtc = GL_GetRtc64();
	struct timeval timer_rtc, timer_stamp;
	do_gettimeofday(&timer_rtc);

	UINT16 timeout_flag = 0;
#endif

    // Wait Cmd Received
    while (0x80000000 != GET_CMD_VALID_BIT())
    {
		#ifdef AUD_Enable_TimeoutProtect
			do_gettimeofday(&timer_stamp);
	    	if( (timer_stamp.tv_sec - timer_rtc.tv_sec) >= 1 ) // 1s
			{
				timeout_flag=1;
				diag_printf("\n== DSP_SendCmd TimeOut==\n");			
				break;
			}
		#endif
		
		#ifdef ENABLE_DSP_LOCK_API    
			DSP_Waitlock(1);
		#else
			DSP_Waitloop(1);
		#endif
    }

#ifdef AUD_Enable_TimeoutProtect
	if( timeout_flag==1 )
	{
		DSP_Set_SendCmdTimeout_Flag(1);
		DSPCMD_MUTEX_UNLOCK();		
		return DSP_ERROR_CMD_WAIT;
	}
#endif

	DSPCMD_MUTEX_UNLOCK();

    return DSP_ERROR_NONE;          
}

void DSP_Set_SendCmdTimeout_Flag( UINT16 value)
{	
	// if 1, we will force to reset dsp in audio MainRoutine
	gDSP_Sendcmd_Timeout = value;
}

UINT16 DSP_Get_SendCmdTimeout_Flag(void)
{	
	return gDSP_Sendcmd_Timeout;
}

UINT32 DSP_SendCmdNoWait(UINT8 CmdID, UINT8 CmdParam, UINT16 OptParam, UINT32 PortParam)
{   
    UINT32 CommandPort = CmdID<<24 | CmdParam<<16 | OptParam;
	
#ifdef ENABLE_INTERRUPT
	if (GET_DSP_INT0())
	{
		DSP_PRINT("DSP_SendCmd::e:wait previous interrupt done!\n");
		return DSP_ERROR_CMD_WAIT;
	}
#endif	
	
    // Check if Cmd Port is valid
    if (0 == GET_CMD_VALID_BIT()) 
    {
        DSP_PRINT("DSP_SendCmdNoWait::e:wait previous command done!\n");
        return DSP_ERROR_CMD_WAIT;
    }

    // Write Port Parameter
    SET_CMD_PARAM_PORT(PortParam);

    // Write Command 
    SET_CMD_CODE_PORT(CommandPort); 
/*
 	DSP_PRINT("DSP_SendCmdNoWait::cmd:0x%08x, param:0x%08x\n",CommandPort,PortParam);
*/
    // Issue Interrupt to DSP?
#ifdef ENABLE_INTERRUPT 
    SET_DSP_INT0();	
#endif	

    return DSP_ERROR_NONE;          
}

void DSP_DumpData(UINT32 type, UINT32 dspAddr, UINT32 size)
{
	UINT32 addr_offset = 0;
	UINT32 dspMsg = 0;

	switch(type)
	{
	case DSP_DUMP_DATA:
		addr_offset = DSP_GetValidDSPAddress(dspAddr);				
		if (K_INVALID_DSP_ADDRESS == addr_offset) 
		{
			DSP_PRINT("DSP_DumpData::e:error address:0x%x\n",dspAddr);
			return;
		}
		if (DSP_MAX_IDM_ADDRESS < size)
		{
			DSP_PRINT("DSP_DumpData::e:error size:%d\n",size);
			return;			
		}			
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x02_DUMP_DATA, size, addr_offset);		
		break;	
	case DSP_DUMP_RISC_PARAM:
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x03_DUMP_RISC,0x0,0x0);
		break;
	case DSP_DUMP_DSP_PARAM:   
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x04_DUMP_DSP,0x0,0x0);				
		break;		
	case DSP_DUMP_MAIN_DATA:   
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x05_DUMP_MAIN,0x0,0x0);
		break;		
	case DSP_DUMP_MIXER_DATA:  
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x06_DUMP_MIXER,0x0,0x0);
		break;		
	case DSP_DUMP_SWAPP_DATA:
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x07_DUMP_SWAPP,0x0,0x0);		
		break;		
	case DSP_DUMP_DEC1_PARAM:  
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x08_DUMP_DEC1,0x0,0x0);				
		break;		
	case DSP_DUMP_DEC2_PARAM:  
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x09_DUMP_DEC2,0x0,0x0);	
		break;		
	case DSP_DUMP_ENC_PARAM:   
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x0a_DUMP_ENC,0x0,0x0);	
		break;		
	case DSP_DUMP_APP_DATA: 
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x0b_DUMP_APP,0x0,0x0);	
		break;		
	case DSP_DUMP_DBG_DATA:  
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x0c_DUMP_DBG,0x0,0x0);	
		break;		
	case DSP_DUMP_DRAMBASE:  
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x0d_DUMP_DRAMBASE,0x0,0x0);	
		break;		
	case DSP_DUMP_UTIL_DATA: 
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x0e_DUMP_UTIL,0x0,0x0);	
		break;
	case DSP_DUMP_STACK:
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x0f_DUMP_STACK,0x0,0x0);	
		break;		
	case DSP_DUMP_ISR_GLOBALS: 
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x10_DUMP_ISR,0x0,0x0);			
		break;		
	case DSP_DUMP_SPDIFIN_DATA:
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x11_DUMP_SPDIFIN,0x0,0x0);			
		break;		
	case DSP_DUMP_EFFECT_PARAM:	
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x12_DUMP_EFFECT,0x0,0x0);			
		break;	
    //
    // Romtable
    // 
    case DSP_DUMP_ROM_DDP:
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x15_DUMP_ROM_DDP,0x0,0x0);			
		break;						
	case DSP_DUMP_ROM_SRC:
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x16_DUMP_ROM_SRC,0x0,0x0);			
		break;						
	case DSP_DUMP_ROM_PL2X:	
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x17_DUMP_ROM_PL2X,0x0,0x0);			
		break;						
	case DSP_DUMP_ROM_DTSHD:		
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x18_DUMP_ROM_DTSHD,0x0,0x0);			
		break;				
	case DSP_DUMP_ROM_TRUEHD:
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x19_DUMP_ROM_TRUEHD,0x0,0x0);			
		break;						
	case DSP_DUMP_ROM_WATERMARK:
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x1a_DUMP_ROM_WATERMARK,0x0,0x0);			
		break;						
	case DSP_DUMP_ROM_DTSLBR:
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x1b_DUMP_ROM_DTSLBR,0x0,0x0);			
		break;						
	case DSP_DUMP_ROM_DTSENC:		
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x1c_DUMP_ROM_DTSENC,0x0,0x0);			
		break;						
    case DSP_DUMP_ROM_WMAPRO:		
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x1d_DUMP_ROM_WMAPRO,0x0,0x0);			
		break;	

    //
    // 9200 new data dection
    //
    case DSP_DUMP_NXP_RISC_PARAM:
        DSP_SendCmd(RISC_CMD0x04_READ,CODE0x1e_DUMP_NXP_RISC,0x0,0x0);			
        break;
    case DSP_DUMP_NXP_DSP_PARAM:
        DSP_SendCmd(RISC_CMD0x04_READ,CODE0x1f_DUMP_NXP_DSP,0x0,0x0);			
        break;
        
	}

	// wait for a dram message
	int i;
	for (i=0; i<100; i++)
	{
		if (DSP_ERROR_NONE == DSP_GetMessage(&dspMsg)) break;
#ifndef ENABLE_DSP_LOCK_API  		
		//GL_TaskSleep(1);	
		msleep(1);
#endif
	}
	
}

UINT32 DSP_ReadData(UINT32 type, UINT32 dspAddr)
{
	UINT32 dspValue = 0;	
	UINT32 addr_offset = 0;

	switch(type)
	{
	case DSP_READ_SHORT:
		addr_offset = DSP_GetValidDSPAddress(dspAddr);
		if (K_INVALID_DSP_ADDRESS == addr_offset) return 0;		
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x00_READ_SHORT,0,addr_offset);		
		break;
	case DSP_READ_LONG:
		addr_offset = DSP_GetValidDSPAddress(dspAddr);		
		if (K_INVALID_DSP_ADDRESS == addr_offset) return 0;
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x01_READ_LONG,0,addr_offset);
		break;
	case DSP_READ_REG:
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x13_READ_REG,0,dspAddr);		
		break;
	case DSP_READ_INT_REG:
		DSP_SendCmd(RISC_CMD0x04_READ,CODE0x14_READ_INT_REG,0,dspAddr);				
		break;	
	}

	dspValue = GET_CMD_PARAM_PORT();
	if (DSP_READ_SHORT == type)
	{
		dspValue &=0xFFFF;
	}    

    return dspValue;
    
}

UINT32 DSP_WriteData( UINT32 type, UINT32 dspAddr,UINT32 nData)
{
	UINT32 addr_offset = DSP_GetValidDSPAddress(dspAddr);	
	if (K_INVALID_DSP_ADDRESS == addr_offset)
	{
		DSP_PRINT("DSP_WriteData::e:invalid address:0x%x, offset:0x%x\n",dspAddr,addr_offset);
		return  DSP_ERROR_CMD_FAIL;		
	}
	
	switch(type)
	{
	case DSP_WRITE_SHORT:
		if (DSP_ERROR_NONE != DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x00_WRITE_SHORT,addr_offset,nData))
			return DSP_ERROR_CMD_FAIL;		
		else
			return DSP_ERROR_NONE;	  
	case DSP_WRITE_LONG:
		if (DSP_ERROR_NONE != DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x01_WRITE_LONG,addr_offset,nData))
			return DSP_ERROR_CMD_FAIL;		
		else
			return DSP_ERROR_NONE;	  
	}	
	return DSP_ERROR_CMD_FAIL;

} 
