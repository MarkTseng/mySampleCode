/*!                                                                       
************************************************************************* 
* \file    dsp_msg.h                                          
*                                                                         
* \brief   aaudio dsp message handling header file
* \note    
*                                                                         
* \date    2009/08/06                                       
*                                                                         
* \author  - jason.su        <jason.su@sunplus.com>                       
*                                                                         
************************************************************************* 
*/                                                                        
 

#ifndef __DSP_MSG_H__
#define __DSP_MSG_H__


/******************************************************************************
	defines
 *****************************************************************************/	
#define MAX_DSPMON_NUM 							10

#define MSG_TYPE_SHORT                         0x0      // 0x00000000 (default)
#define MSG_TYPE_LONG                          0x1      // 0x10000000 
#define MSG_TYPE_DRAM                          0x2
                                                              
#define MSG_ID_MAIN                            0x0      // 0x00000000 (default) 
#define MSG_ID_DEC1                            0x1      // 0x01000000
#define MSG_ID_DEC2                            0x2      // 0x02000000
#define MSG_ID_ENC                             0x3      // 0x03000000    
#define MSG_ID_DBG                             0x4      // 0x04000000
#define MSG_ID_PRF                             0x5      // 0x05000000
#define MSG_ID_CHKSUM                          0x6      // 0x06000000
#define MSG_ID_DMA                             0x7      // 0x07000000  
#define MSG_ID_ISR                             0x8      // 0x08000000  
#define MSG_ID_APP                             0x9      // 0x09000000  
#define MSG_ID_SWAPP                           0xA      // 0x0A000000  
#define MSG_ID_MIXER                           0xB      // 0x0B000000  
#define MSG_ID_FPGA                            0xF      // 0x0F000000  

/******************************************************************************
	typedef
 *****************************************************************************/						
typedef struct _ProfileHistoryInfo_t
{
	UINT16 MonID;	
	UINT32 frameCounts;	
//	UINT32 sampleRate;
	UINT32 time_us;
	UINT32 mips_max;
	UINT32 mips_min;
	UINT32 mips_dma_max;
	UINT32 mips_dma_min;
	UINT32 mips_cache_max;
	UINT32 mips_cache_min;
	UINT32 mips_max_frame;
	UINT32 mips_min_frame;	
	UINT32 mips_dma_max_frame;
	UINT32 mips_dma_min_frame;
	UINT32 mips_cache_max_frame;
	UINT32 mips_cache_min_frame;	
	UINT32 allCycles;
//	float allSamples;
	UINT32 allCacheCycles;
	UINT32 allDmaCycles;
	UINT32 allDMABandWidthCnt;
	UINT32 allCacheBWCount;
	UINT32 dma_max;
	UINT32 dma_min;
	UINT32 chksum;


	UINT32 allCacheBurstCnt;
	UINT32 allCacheSingleCnt;
	
}ProfileHistoryInfo_t;


#define K_MAX_GOLDEN_LEN				(512*1024)		// 512k frames
typedef struct _DspChksumInfo_t
{		
	char    filename[512];
	UINT32  bChksumExist;
	UINT32  chksumTbl[K_MAX_GOLDEN_LEN];
   	UINT32  nChksumTblSize;
}DspChksumInfo_t;


//
// should use __attribute__ ((packed)) to make compact data alignment (gcc syntax)
//
typedef struct _ProfilerInfo
{
	unsigned short MonID          		__attribute__ ((packed));   // Monitor profiling ID
	unsigned int   dwFrameCount  		__attribute__ ((packed));   // Frame Count 
	unsigned short SampleRateID      	__attribute__ ((packed));	// Sample Rate ID
	unsigned short BlockSize         	__attribute__ ((packed));	// Block Size (in samples)
	unsigned int   dwChecksum        	__attribute__ ((packed));	// checksum
	unsigned int   dwDspCycleCnt	  	__attribute__ ((packed));	// DSP_CYCLE 			        
	unsigned int   dwDspDMACycleCnt  	__attribute__ ((packed));	// DSP_DMA_CYCLE             
	unsigned int   dwDspCacheCycleCnt	__attribute__ ((packed));	// DSP_CACHE_ACC_CYCLE      
	unsigned int   dwDspDMABWCnt       	__attribute__ ((packed));	// DSP_DMA_CNT           
//	unsigned int   dwDspCacheMissedCnt 	__attribute__ ((packed));	// DSP_CACHE_NSEQ_CNT        
	unsigned int   dwDspCacheBurstCnt 	__attribute__ ((packed));	// REG_DSP_CACHE_BURST_CNT        
	unsigned int   dwDspCacheSingleCnt 	__attribute__ ((packed));	// REG_DSP_CACHE_SINGLE_CNT 
	unsigned int   dwDspDMACycleMax    	__attribute__ ((packed));	// DSP_MAX_DMA_CYCLE
	unsigned int   dwDspDMACycleMin    	__attribute__ ((packed));	// DSP_MIN_DMA_CYCLE  
}ProfilerInfo_t;


typedef struct _FifoMonInfo_t
{
    UINT8   state;        //0: no initialize, 1: disable,    2: counting
    UINT8   isRead;       //0: do nothing,    1: print cnt
    UINT8   isDramA;      //0: use DRAM B,    1: use DRAM A
   // UINT32  audMaxCyc;
   // UINT32  audTotalCmdCnt;
   // UINT32  audTotalCmdCyc;
   // UINT16  mbarCnt1;
   // UINT16  mbarCnt2;
   // UINT16  mbarCnt3;
}FifoMonInfo_t;


/******************************************************************************
        Globals
 *****************************************************************************/
extern UINT16 gDspMsgIDFilter;   		// message ID filter
extern UINT16 gDspMsgSubIDFilter;		// message subID filter

/******************************************************************************
        C Funciton Prototype
 *****************************************************************************/

/*!
  \brief  Get dsp messages from message protocol
  \par    detail description:\n
          dsp message protocol: (added later)
  \param  dspMsg  [IN]received dsp message were put
  \return DSP_ERROR_NO_MESSAGE: if no message
          others: get dsp messages
  \note   
  \todo   code refactor required
 */
UINT32 DSP_GetMessage(UINT32* dspMsg);

/*!
  \brief  Get dsp messages flag 
  \par    detail description:\n
          Get valid bit (MSB) of message port (port10).\n
          If valid bit == 0, new dsp message were received.\n
  \par    message protocol handling APIs:        
          DSP_GetMessageFlag() is used to check if new message arrived\n
          DSP_GetNewMessage() is used to get the new message\n
          DSP_SetMessageFlag() is used to set valid bit (MSB) of message port
  \return 1, if valid bit flag set \n
          0, else
  \note   
 */
UINT32 DSP_GetMessageFlag(void);

/*!
  \brief  Set dsp messages flag 
  \par    detail description:\n
          Set valid bit (MSB) of message port (port10).\n
  \par    message protocol handling APIs:
          DSP_GetMessageFlag() is used to check if new message arrived\n
          DSP_GetNewMessage() is used to get the new message\n
          DSP_SetMessageFlag() is used to set valid bit (MSB) of message port 
  \see    DSP_GetMessage()
  \note   
 */
void DSP_SetMessageFlag(void);

/*!
  \brief  Get dsp messages 
  \par    detail description:\n
          Get dsp message from message port for short message (PORT10),\n
          or from optional message port for long message (PORT11).
  \par    message protocol handling APIs:
          DSP_GetMessageFlag() is used to check if new message arrived\n
          DSP_GetNewMessage() is used to get the new message\n
          DSP_SetMessageFlag() is used to set valid bit (MSB) of message port 
  \see    DSP_GetMessage()
  \note   
 */
UINT32 DSP_GetNewMessage(void);


/*!
  \brief  Converse Hex string to integer in decimal
  \par    input:	 token[]  : input string
  \return conversion result
  \note   Converse Hex string to integer in decimal
*/
INT32 StrToInt(char token[]);

/*!
  \brief  Converse Hex string to integer in decimal
  \par    token[]  : input string
          start_i  : indicate which number to start conversion
  \return sum      : conversion result
  \note   Converse Hex string to integer in decimal
*/
INT32 HexToInt_i(char token[],int start_i);


void DspMonPrintHistory(UINT16 monID);

ProfileHistoryInfo_t* DspMonGetPrfHistoryInfo(UINT16 monID);

//void dspShowAutoBatchMsg(UINT32 dspMsgCmd, UINT32 dspMsgParam);

void DspMonInitPrfHistory(UINT16 monID);
void DSP_ShowPrfResult(void);

INT32 DSP_InitGoldChksumTbl(UINT8 codecBankID, char* pFilename);

void DSP_FifoMonitor(UINT8 cmd, UINT32 param);

#endif //__DSP_MSG_H__ 
