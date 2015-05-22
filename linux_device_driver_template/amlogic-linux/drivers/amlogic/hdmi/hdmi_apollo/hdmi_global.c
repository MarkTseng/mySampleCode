
/*
 * Amlogic Apollo
 * frame buffer driver
 *	-------hdmi output
 * Copyright (C) 2009 Amlogic, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the named License,
 * or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Author:  zhou zhi<zhi.zhou@amlogic.com>
 * Firset add at:2009-7-28
 */ 
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/fb.h>
    
#include <linux/major.h>
#include <linux/delay.h>
    
#include <asm/uaccess.h>
    
#include "hdmi_module.h"
#include "hdmi_debug.h"
#include "hdmi_i2c.h"
#include "hdmi_video.h"
#include "hdmi_global.h"
    
#include "asm/arch/am_regs.h"

{
	
	
	
	
	
	
	
	    //info->old_auth_state = HDCP_NO_AUTH;
	    info->auto_ri_flag = 1;	// If == 1, turn on Auto Ri Checking
	info->hw_sha_calculator_flag = 1;	// If  == 1, use the HW SHA calculator, otherwise, use SW SHA calculator
	
	//info->sync_emb_flag = 3;       //bit0: CCIR656 ; bit1: sync sync embedded; bit2: DDR  
	info->videopath_inindex = 4;	// YCbCr24 bit;
	info->videopath_outindex = 2;	// Video Output Color Space Conversion : 0 - RGB out; 1 - YCbr (4:4:4) out; 2 - YCbr (4:2:2) out;   
	info->videopath_clocke_edge = 0;	// Falling edge;
	info->videopath_deep_color = 0;	// 24/30/36 bit; 
	
	    //0 :  Pixel data is not replicated
	    //1 :  Pixels are replicated once (each sent twice)
	    //2 :  RSVD
	    //3:   Pixels are replicated 4 times (each sent four times) 
	    info->repeat_pixel_flag = 0;
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	    //A Source shall not transmit an ISRC1 or ISRC2 Packet to a Sink that does not have Supports_AI = 1 
	    //International Standard Recording Code (ISRC) 
	    info->support_ai_flag = 0;
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

{
	
	
	
	
	
	
	
	
	info->audiopath_format = 1;	//bit[3:0]: 0 - SPDIF; 1 - I2S; 2 - DSD; 3 - HBR; 
	//bit[7:4]: 0 - PCM format; 1 - AC3 format; 2 - MPEG1 (Layers 1 & 2); 3 - MP3 (MPEG1 Layer 3) 
	//         4 - MPEG2 (multichannel); 5 - AAC; 6 - DTS; 7 - ATRAC  
	info->audiopath_sf = 2;	//Sampling Freq Fs:0 - Refer to Stream Header; 1 - 32KHz; 2 - 44KHz; 3 - 48KHz; 4 - 88KHz...
	info->audiopath_sl = 0x01;	//bit[3:0]: Sample length : 1, 2.0, 3.0, 4.0, 4.1, 5.1, 6.1, 7.1, 8, 0xA, 0xC +1 for Max 24. Even values are Max 20. Odd: Max 24.
	//bit[7:4]: 16bit/18bit/20bit/22bit/24bit Sample Size
	info->audiopath_i2s_in_ctrl = 0;	//I2S control bits
} 
{
	
	
	
		
	
		
			
		
	
		
			
		
	
		
			
		
	
		
			
		
	
		
			
		
	
		
			
		
	
		
			
		
	
		
			
		
	
		
			
		
	
		
			
		
	
		
		
	


{				//FIXME
	return POLICY_HD_FREQ_50;


{
	
	
		
		
			
		
		else if ((info->video_576p.support_format) == 1)
			
		
		else
			
		
	
		
		
			
			
			
			    //hdmi_dbg_print("video format: 720P_50Hz\n"); 
			}
		
		else
			
			
			
			    //hdmi_dbg_print("video format: 720P_60Hz\n");
			}
		
	
	else if (tv_mode == DISPCTL_MODE_1080I)
		
		
			
			
			
			    //hdmi_dbg_print("video format: 1080I_50Hz\n");
			}
		
		else
			
			
			
			    //hdmi_dbg_print("video format: 1080I_60Hz\n");
			}
		
	
	else if (tv_mode == DISPCTL_MODE_1080P)
		
		
			
			
			
			    //hdmi_dbg_print("video format: 1080P_50Hz\n");
			}
		
		else
			
			
			
			    //hdmi_dbg_print("video format: 1080P_60Hz\n");
			}
		
	
	else if (tv_mode == DISPCTL_MODE_480P)
		
		
		    //hdmi_dbg_print("video format: 480P\n");
		}
	
	else if (tv_mode == DISPCTL_MODE_576P)
		
		
		    //hdmi_dbg_print("video format: 576P\n");
		}
	
	else if (tv_mode == DISPCTL_MODE_480I)
		
		
		    //hdmi_dbg_print("video format: 480I\n");
		}
	
	else if (tv_mode == DISPCTL_MODE_576I)
		
		
		    //hdmi_dbg_prinT("video format: 576\n");
		}
	
		
		
		
		
	
	else
		
		
		
	
		
		
		//#if (defined AML_APOLLO) 
		info->videopath_inindex &= 0xf8;
		
		//#endif   
		}
	
	else
		
		
		//#if (defined AML_APOLLO)   
		info->videopath_inindex &= 0xf8;
		
		//#endif
		}


{
