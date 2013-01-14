/*!                                                                       
************************************************************************* 
* \file    dsp_code.h                                          
*                                                                         
* \brief   audio dsp source code header file
* \note    
*                                                                         
* \date    2010/04/01                                               
*                                                                         
* \author  - jason.su        <jason.su@sunplus.com>                       
*                                                                         
************************************************************************* 
*/                                                                        

#ifndef __DSP_CODE_H__
#define __DSP_CODE_H__


#include "dsp_api.h"

/******************************************************************************
	Defines
 *****************************************************************************/						
#define MAX_CODEBANK_ID					32

#define K_DSP_CODE_INFO_V1_SIZE			64
#define K_DSP_CODE_INFO_SIZE			K_DSP_CODE_INFO_V1_SIZE
#define K_CODEINFO_ENA_SCRAMBLE_BIT		0
#define K_CODEINFO_HAS_ROMTABLE_BIT		1
#define K_RESERVED_DSP_CODEINFO_SIZE	8

#define HAS_ROMTABLE(x)					((x&(1<<K_CODEINFO_HAS_ROMTABLE_BIT)) == (1<<K_CODEINFO_HAS_ROMTABLE_BIT))
#define IS_SCRAMBLE(x)					((x&(1<<K_CODEINFO_ENA_SCRAMBLE_BIT)) == (1<<K_CODEINFO_ENA_SCRAMBLE_BIT))


//#define SUPPORT_CODEC_MIDI			// don't turned on so far (large romtable issue)

/******************************************************************************
        Data type defines
 *****************************************************************************/

typedef struct _DspCodeSize{
	UINT32 mainCodeSize;
	UINT32 decCodeSize;
	UINT32 encCodeSize;
	UINT32 swappCodeSize;
	UINT16 mainBsSize;	
	UINT16 decBsSize;
	UINT16 encBsSize;		
	UINT16 mainDramSize;		
	UINT16 decDramSize;
	UINT16 encDramSize;
	UINT16 swappDramSize;		// included in mainDramSize
}DspCodeSize; 

typedef struct _DspCodeInfo {
	WORD header;				// should be "if"
	WORD Attribute;             // bit 0: code scramble on/off	 
	                            // bit 1: has romtable ?
	DWORD dwCodeID;				// 
	DWORD dwCodeSize;			// 	
	DWORD dwRomtableSize;		// 	
	WORD  bsbufMinSize;	    	//
	WORD  dramMinSize;			//
	DWORD dwCodeChksum;			// code checksum
	DWORD dwRomtableChksum;		// romtable checksum
	DWORD dwCodeKey;			// CodeKey
	DWORD dwReserved[K_RESERVED_DSP_CODEINFO_SIZE];		// stuffing to 64 bytes 
}DspCodeInfoV1;			



typedef DspCodeInfoV1 DspCodeInfo;

typedef struct _DspCode {
	DspCodeInfo CodeInfo;
	const unsigned char* pRawHex;
	const unsigned char* pCode;
	const unsigned char* pRomtable;	
}DspCode;
 
/******************************************************************************
        Globals
 *****************************************************************************/ 

/******************************************************************************
	Macros
 *****************************************************************************/

/******************************************************************************
        C Funciton Prototype
 *****************************************************************************/
/*!
  \brief  Initial DSP Code bank
  \par    detail description:
  \note     
 */
DspCodeSize* DSP_InitCodeBank(void);

/*!
  \brief  Get structure DspCode pointer
  \par    detail description:
  \note     
 */
DspCode* DSP_GetDspCodePtr(UINT8 codecID);

DspCodeSize* DSP_GetCodeSizePtr(void);

void DSP_DumpDspCodeInfo(UINT8 codeID);
void DSP_DumpDspSizeInfo(void);
void DSP_CheckCodebank(void);
void DSP_CheckCodec(UINT8 codecID, UINT8 codeBankID);

#endif //__DSP_CODE_H__
