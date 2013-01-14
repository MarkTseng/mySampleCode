/*!                                                                       
************************************************************************* 
* \file    dsp_msg.h                                          
*                                                                         
* \brief   audio dsp command protocol handling header file
* \note    
*                                                                         
* \date    2009/08/06                                       
*                                                                         
* \author  - jason.su        <jason.su@sunplus.com>                       
*                                                                         
************************************************************************* 
*/                                                                        
 

#ifndef __DSP_CMD_H__
#define __DSP_CMD_H__

/******************************************************************************
	defines
 *****************************************************************************/	

typedef enum _RISC_CMD_GROUP_ID
{
	RISC_CMD0x00_FLOW 		     = 0x00,
	RISC_CMD0x01_FIFO_CTRL       = 0x01,
	RISC_CMD0x02_BSBUF  	     = 0x02,	
	RISC_CMD0x03_RAMP            = 0x03,
	RISC_CMD0x04_READ  		     = 0x04,
	RISC_CMD0x05_WRITE 	         = 0x05,	
	RISC_CMD0x06_IEC_MODE 	     = 0x06,
	RISC_CMD0x07_OUTPUT_MODE     = 0x07,
	RISC_CMD0x08_AUD_GAIN        = 0x08,
	RISC_CMD0x09_MIC_CTRL        = 0x09,
	RISC_CMD0x0A_AC3_OUTPUT      = 0x0A,
	RISC_CMD0x10_HW_APP          = 0x10,
	RISC_CMD0x11_SW_APP	 	     = 0x11,
	RISC_CMD0x12_EFFECT_AUDIO    = 0x12,
	RISC_CMD0x13_CDRIP_MODE      = 0x13,
	RISC_CMD0x14_MIXER_GAIN      = 0x14,
	RISC_CMD0x15_EMU             = 0x15,		
	RISC_CMD0x16_SRS             = 0x16,		
	RISC_CMD0x17_DEMOD_AGC_CTRL  = 0x17,			
	RISC_CMD0x18_PCM_INPUT_REINIT= 0x18,				
	RISC_CMD_MAX,
}RISC_CMD_GROUP_ID;

typedef enum _RISC_CMD_CODE
{
	// RISC_CMD0x00_FLOW:0x00
	CODE0x00_MAIN_FLOW 	 = 0x00,
	CODE0x01_DEC1_FLOW 	 = 0x01,
	CODE0x02_DEC2_FLOW 	 = 0x02,
	CODE0x03_ENC_FLOW 	 = 0x03,
	CODE0x04_AUTO_START  = 0x04,
                             
	// RISC_CMD0x01     
	CODE0x00_FIFO_ENABLE = 0x00,
	CODE0x01_FIFO_RESET	 = 0x01,	
		                     
	// RISC_CMD0x02_BSBUF:0x02	 	
	CODE0x00_BSBUF_FLUSH	= 0x00,
	CODE0x01_BSBUF_ENDING   = 0x01,	
		 	
	                         
	// RISC_CMD0x03_RAMP:0x03	 
	CODE0x00_RAMP_UP      = 0x00,
	CODE0x01_RAMP_DOWN    = 0x01,
	CODE0x02_A0_RAMP_UP   = 0x02,
	CODE0x03_A0_RAMP_DOWN = 0x03,	
	CODE0x04_A1_RAMP_UP   = 0x04,
	CODE0x05_A1_RAMP_DOWN = 0x05,	
	CODE0x06_A2_RAMP_UP   = 0x06,
	CODE0x07_A2_RAMP_DOWN = 0x07,		
		                     
	// RISC_CMD0x04_READ:0x04	 
	CODE0x00_READ_SHORT   = 0x00,
	CODE0x01_READ_LONG    = 0x01,
	CODE0x02_DUMP_DATA    = 0x02,
	CODE0x03_DUMP_RISC    = 0x03,
	CODE0x04_DUMP_DSP     = 0x04,
	CODE0x05_DUMP_MAIN    = 0x05,
	CODE0x06_DUMP_MIXER   = 0x06,
	CODE0x07_DUMP_SWAPP   = 0x07,
	CODE0x08_DUMP_DEC1    = 0x08,
	CODE0x09_DUMP_DEC2    = 0x09,
	CODE0x0a_DUMP_ENC     = 0x0a,	
	CODE0x0b_DUMP_APP     = 0x0b,	
	CODE0x0c_DUMP_DBG     = 0x0c,	
	CODE0x0d_DUMP_DRAMBASE= 0x0d,		
	CODE0x0e_DUMP_UTIL    = 0x0e,	
	CODE0x0f_DUMP_STACK   = 0x0f,		
	CODE0x10_DUMP_ISR     = 0x10,		
	CODE0x11_DUMP_SPDIFIN = 0x11,	
	CODE0x12_DUMP_EFFECT  = 0x12,		
	CODE0x13_READ_REG     = 0x13,	
	CODE0x14_READ_INT_REG = 0x14,	
	CODE0x15_DUMP_ROM_DDP       = 0x15,	
	CODE0x16_DUMP_ROM_SRC       = 0x16,	
	CODE0x17_DUMP_ROM_PL2X      = 0x17,	
	CODE0x18_DUMP_ROM_DTSHD     = 0x18,	
	CODE0x19_DUMP_ROM_TRUEHD    = 0x19,	
	CODE0x1a_DUMP_ROM_WATERMARK = 0x1a,	
	CODE0x1b_DUMP_ROM_DTSLBR    = 0x1b,	
	CODE0x1c_DUMP_ROM_DTSENC    = 0x1c,	
	CODE0x1d_DUMP_ROM_WMAPRO    = 0x1d,	
	CODE0x1e_DUMP_NXP_RISC      = 0x1e,	
	CODE0x1f_DUMP_NXP_DSP       = 0x1f,		
	
	// RISC_CMD0x05_WRITE:0x05
	CODE0x00_WRITE_SHORT = 0x00,     
	CODE0x01_WRITE_LONG  = 0x01, 
	CODE0x02_SYNC_RISC_PARAM = 0x02,   
	CODE0x03_WRITE_RISC_PARAM = 0x03,  
	CODE0x04_SYNC_RISC_PARAM1 = 0x04,   

	// RISC_CMD0x06_IEC_MODE
	CODE0x00_SPDIF_MODE  = 0x00,
	CODE0x01_HDMI_MODE   = 0x01,

	// RISC_CMD0x07_OUTPUT_MODE
	CODE0x00_DOWNMIX_MODE = 0x00,
	CODE0x01_2CH_MODE     = 0x01,

	// RISC_CMD0x08_AUD_GAIN
	CODE0x00_MASTER_GAIN  = 0x00,
	CODE0x01_A4_LM_GAIN   = 0x01,
	CODE0x02_A4_RM_GAIN   = 0x02,
	CODE0x03_A0_LF_GAIN   = 0x03,
	CODE0x04_A0_RF_GAIN   = 0x04,
	CODE0x05_A1_LS_GAIN   = 0x05,
	CODE0x06_A1_RS_GAIN   = 0x06,
	CODE0x07_A2_C_GAIN    = 0x07,
	CODE0x08_A2_LFE_GAIN  = 0x08,
	CODE0x09_A3_LB_GAIN   = 0x09,
	CODE0x0A_A3_RB_GAIN   = 0x0A,

	// RISC_CMD0x09_MIC_CTRL
	CODE0x00_MIC_SWITCH      = 0x00,
	CODE0x01_MIC0_GAIN       = 0x01,
	CODE0x02_MIC1_GAIN       = 0x02,
	CODE0x03_MIC_ECHO        = 0x03,
	CODE0x04_VOCAL_SCORE     = 0x04,
	CODE0x05_KARA_SND_EFFECT = 0x05,	

	// RISC_CMD0x0A_AC3_OUTPUT
	CODE0x00_RF_LINE_MODE    = 0x00,
	CODE0x01_KARA_MODE       = 0x01,
	CODE0x02_KARA_MELODY_CH  = 0x02,
	CODE0x03_DIAL_NORM       = 0x03,
	CODE0x04_DUAL_MONO       = 0x04,	
	
	
	// RISC_CMD0x11_SW_APP:0x11
	CODE0x00_PL2_CMD 	 = 0x00,
	CODE0x01_NEO6_CMD 	 = 0x01,
	CODE0x02_GM5_CMD 	 = 0x02,
	CODE0x03_SWAPP_INIT_CMD  = 0x03,
	
	// RISC_CMD0x14_MIXER_GAIN:0x14
    CODEC0X00_DEC1_GAIN   = 0X00,
    CODEC0X01_DEC2_GAIN   = 0X01,
    CODEC0X02_MIXER_MODE  = 0X02,
	
	//RISC_CMD0x12_EFFECT_AUDIO
	CODE0x00_EFFECT_ADDRESS    = 0x00,
	CODE0x01_EFFECT_SAMPLE_CNT = 0x01,
	CODE0x02_EFFECT_MIXGAIN_A0 = 0x02,
	CODE0x03_EFFECT_MIXGAIN_A1 = 0x03,
	CODE0x04_EFFECT_MIXGAIN_A2 = 0x04,
	CODE0x05_EFFECT_MIXGAIN_A3 = 0x05,
	CODE0x06_EFFECT_MIXGAIN_A4 = 0x06,
	CODE0x07_EFFECT_CHECK_DONE = 0x07,

    //RISC_CMD0x18_PCM_INPUT_REINIT
	CODE0x00_SPDIF_INPUT        = 0x00,
	CODE0x01_SIF_INPUT          = 0x01,
	CODE0x02_AUX_INPUT = 0x02,    

	
}RISC_CMD_CODE;

typedef enum _RISC_CMD_FLOW_PARAMS
{
	RISC_CMD0_INIT   = 0x0000,
    RISC_CMD0_READY  = 0x0001,
    RISC_CMD0_STOP   = 0x0002,
    RISC_CMD0_IDLE   = 0x0002,
    RISC_CMD0_PLAY   = 0x0003,
    RISC_CMD0_PAUSE  = 0x0004,
    RISC_CMD0_FF_2X  = 0x0005,
    RISC_CMD0_FF_4X  = 0x0006,
    RISC_CMD0_FF_8X  = 0x0007,
    RISC_CMD0_FF_16X = 0x0008,
    RISC_CMD0_FF_20X = 0x0009,
    RISC_CMD0_FF_32X = 0x000a,
}RISC_CMD_FLOW_PARAMS;
typedef enum _RISC_CMD_TYPE
{		
	//DSP_WriteData
	DSP_WRITE_SHORT 	= 0,
	DSP_WRITE_LONG 	 	= 1,
	//DSP_ReadData
	DSP_READ_SHORT 		= 0,
	DSP_READ_LONG 		= 1,	
	DSP_READ_REG     	= 2, 
	DSP_READ_INT_REG    = 3,		
	//DSP_DumpData
	DSP_DUMP_DATA       = 0,
	DSP_DUMP_RISC_PARAM,  
	DSP_DUMP_DSP_PARAM,   
	DSP_DUMP_MAIN_DATA,   
	DSP_DUMP_MIXER_DATA,  
	DSP_DUMP_SWAPP_DATA,
	DSP_DUMP_DEC1_PARAM,  
	DSP_DUMP_DEC2_PARAM,  
	DSP_DUMP_ENC_PARAM,   
	DSP_DUMP_APP_DATA,    
	DSP_DUMP_DBG_DATA,    
	DSP_DUMP_DRAMBASE,    
	DSP_DUMP_UTIL_DATA,   
	DSP_DUMP_STACK,       
	DSP_DUMP_ISR_GLOBALS, 
	DSP_DUMP_SPDIFIN_DATA,
	DSP_DUMP_EFFECT_PARAM,
	DSP_DUMP_ROM_DDP,
	DSP_DUMP_ROM_SRC,
	DSP_DUMP_ROM_PL2X,
	DSP_DUMP_ROM_DTSHD,
	DSP_DUMP_ROM_TRUEHD,
	DSP_DUMP_ROM_WATERMARK,
	DSP_DUMP_ROM_DTSLBR,
	DSP_DUMP_ROM_DTSENC,
	DSP_DUMP_ROM_WMAPRO,
	DSP_DUMP_NXP_RISC_PARAM,
	DSP_DUMP_NXP_DSP_PARAM,
}RISC_CMD_TYPE;


/******************************************************************************
	typedef
 *****************************************************************************/						


/******************************************************************************
        Globals
 *****************************************************************************/



/******************************************************************************
        C Funciton Prototype
 *****************************************************************************/
/*!
  \brief  send command to dsp system
  \par    detail description:\n 
          dsp command protocol: (added later)

  \par    send command procedure:
          1. check if previous command serviced?\n
          2. write parameter port\n
          3. write command port\n
          4. check if previous INT0 serviced?\n
          5. issue interrupt\n
          6. wait dsp acknowledged by checking valid bit of command protocol
  \param  CmdID  [IN]command group id (8 bit)
  \param  CmdParam [IN]command code parameter (8 bit)
  \param  OptParam [IN]optional parameter (lo-word command)
  \param  PortParam [IN]parameter port (full 32 bit)
  \return DSP_ERROR_NONE: succeed and system is running\n
          others: failed  
  \note   
 */
UINT32 DSP_SendCmd(UINT8 CmdID, UINT8 CmdParam, UINT16 OptParam, UINT32 PortParam);

/*!
  \brief  send command to dsp system with no wait
  \par    detail description:\n 
          same as DSP_SendCmd(), but no wait for dsp acknowledge\n
  \par    send command procedure:
          1. check if previous command serviced?\n
          2. write parameter port\n
          3. write command port\n
          4. check if previous INT0 serviced?\n
          5. issue interrupt\n
  \param  CmdID  [IN]command group id (8 bit)
  \param  CmdParam [IN]command code parameter (8 bit)
  \param  OptParam [IN]optional parameter (lo-word command)
  \param  PortParam [IN]parameter port (full 32 bit)
  \return DSP_ERROR_NONE: succeed and system is running\n
          others: failed  
  \note   
 */
UINT32 DSP_SendCmdNoWait(UINT8 CmdID, UINT8 CmdParam, UINT16 OptParam, UINT32 PortParam);

/*!
  \brief  set this flag to force reset dsp in audio MainRoutine if sendcmd timeout
  \par    detail description:\n
  \param  value [IN] 
  			1: would reset dsp\n
  			0: clean flag
  \return none
 */
void DSP_Set_SendCmdTimeout_Flag( UINT16 value);

/*!
  \brief  Get this flag to check if need to force reset dsp or not
  \par    void
  \return flag_status
 */
UINT16 DSP_Get_SendCmdTimeout_Flag(void);

/*!
  \brief  write data to internal dsp memory\n
  \par    detail description:\n 
          internal dsp memory range: 0x0000~0x7FFF if QAE377S
          internal dsp memory range: 0x19000~0x20FFF if QAE377A
  \param  dspAddr[IN]internal dsp memory address
  \param  type   [IN]CODE0x00_WRITE_SHORT : write 16 bit word
                     CODE0x00_WRITE_LONG  : write 32 bit double word
  \param  nData  [IN]data to be sned
  \return DSP_ERROR_NONE: succeed and system is running\n
          others: failed  
  \note   
 */
UINT32 DSP_WriteData(UINT32 type,UINT32 dspAddr, UINT32 nData);

/*!
  \brief  read internal dsp memory
  \par    detail description:\n   		  
          internal dsp memory range: 0x0000~0x7FFF if QAE377S
          internal dsp memory range: 0x19000~0x20FFF if QAE377A
  \param  dspAddr[IN]internal dsp memory address
  \param  type   [IN]DSP_READ_SHORT : read a 16-bit word
                     DSP_READ_LONG  : read a 32-bit double word
                     DSP_REG        : read a register
                     DSP_INT_REG    : read a internal register
                     
  \return desired data
  \note   
 */
UINT32 DSP_ReadData(UINT32 type, UINT32 dspAddr);
/*!
  \brief  dump internal dsp memory
  \par    detail description:\n   		  
          internal dsp memory range: 0x0000~0x7FFF if QAE377S
          internal dsp memory range: 0x19000~0x20FFF if QAE377A
  \param  dspAddr[IN]internal dsp memory address
  \param  size   [IN]dump size
  \param  type   [IN]DSP_DUMP_DATA      
                     DSP_DUMP_RISC_PARAM  
                     DSP_DUMP_DSP_PARAM
                     DSP_DUMP_MAIN_DATA
                     DSP_DUMP_MIXER_DATA
                     DSP_DUMP_SWAPP_DATA
                     DSP_DUMP_DEC1_PARAM
                     DSP_DUMP_DEC2_PARAM
                     DSP_DUMP_ENC_PARAM
                     DSP_DUMP_APP_DATA
                     DSP_DUMP_DBG_DATA
                     DSP_DUMP_DRAMBASE
                     DSP_DUMP_UTIL_DATA
                     DSP_DUMP_STACK
                     DSP_DUMP_ISR_GLOBALS
                     DSP_DUMP_SPDIFIN_DATA
                     DSP_DUMP_EFFECT_PARAM

  \return desired data
  \note   
 */
void DSP_DumpData(UINT32 type, UINT32 dspAddr, UINT32 size);
#endif //__DSP_CMD_H__ 
