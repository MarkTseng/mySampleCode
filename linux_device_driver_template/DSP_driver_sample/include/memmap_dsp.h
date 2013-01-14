/******************************************************************************
 * Filename    : memmap_dsp.h
 * 
 * Description : DSP memory related settings
 * Language    : C Language 
 *           
 * Revision History: 
 * ------------------
 * [080915]hcsu Initial Draft
 *
 * -----------------------------------------------------------------------
 * Copyright (C) 2007-2009 Sunplus Technology Co., Ltd. 
 * All rights reserved. The information contained herein is the exclusive 
 * property of SUNPLUS TECHNOLOGY CO., LTD. and shall not be distributed, 
 * reproduced, or disclosed in whole or in part without prior written 
 * permission of SUNPLUS TECHNOLOGY CO., LTD.   
 *****************************************************************************/
 
#ifndef __memmap_dsp_h__
#define __memmap_dsp_h__

/******************************************************************************
    Bitstream buffers
 ******************************************************************************/
//
// Default Settings (also maximum available bitstream buffer settings)
//
#define K_BS_BUF_SIZE_DEFAULT						700
#define K_BS_BUF_SIZE_ENCODER                       150

//
// Individual Codec settings (if defined ENABLE_DSP_RUNTIME_MEMMAP) 
//
#define K_BS_BUF_SIZE_PCM      						20
#define K_BS_BUF_SIZE_DDP      						16
#define K_BS_BUF_SIZE_TRUEHD   						120
#define K_BS_BUF_SIZE_DTSLBR   						10
#define K_BS_BUF_SIZE_DTSHD    						148
#define K_BS_BUF_SIZE_MPEG     						10
#define K_BS_BUF_SIZE_AAC      						10
#define K_BS_BUF_SIZE_WMA      						11
#define K_BS_BUF_SIZE_RA       						20
#define K_BS_BUF_SIZE_OGG      						10
#define K_BS_BUF_SIZE_EMU      						0
#define K_BS_BUF_SIZE_WMAPRO   						20
#define K_BS_BUF_SIZE_FLAC   						216
#define K_BS_BUF_SIZE_AMR   						10
#define K_BS_BUF_SIZE_MIDI   						10
#define K_BS_BUF_SIZE_MPEG_ENC   					10
#define K_BS_BUF_SIZE_AC3_ENC   					5
#define K_BS_BUF_SIZE_DTS_ENC   					4
#define K_BS_BUF_SIZE_APE                           32
#define K_BS_BUF_SIZE_AVS                           29



#endif // __memmap_dsp_h__

