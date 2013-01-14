/*
 * Amlogic osd
 * frame buffer driver
 *
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
 * Author:  Tim Yao <timyao@amlogic.com>
 *
 */

#ifndef OSD_MAIN_H
#define OSD_MAIN_H
#include  <linux/list.h>
#include  <linux/vout/vout_notify.h>
#include  <linux/fb.h>




static struct fb_var_screeninfo mydef_var[] = {
{
#if defined(CONFIG_FB_OSD1_DEFAULT_WIDTH)
	.xres            = CONFIG_FB_OSD1_DEFAULT_WIDTH,
#else
	.xres            = 1200,
#endif
#if defined(CONFIG_FB_OSD1_DEFAULT_HEIGHT)
	.yres            = CONFIG_FB_OSD1_DEFAULT_HEIGHT,
#else
	.yres            = 690,
#endif
#if defined(CONFIG_FB_OSD1_DEFAULT_WIDTH_VIRTUAL)
	.xres_virtual    = CONFIG_FB_OSD1_DEFAULT_WIDTH_VIRTUAL,
#else
	.xres_virtual    = 1200,
#endif
#if defined(CONFIG_FB_OSD1_DEFAULT_HEIGHT_VIRTUAL)
	.yres_virtual    = CONFIG_FB_OSD1_DEFAULT_HEIGHT_VIRTUAL,
#else
	.yres_virtual    = 1380,
#endif
	.xoffset         = 0,
	.yoffset         = 0,
#if defined(CONFIG_FB_OSD1_DEFAULT_BITS_PER_PIXEL)
	.bits_per_pixel  = CONFIG_FB_OSD1_DEFAULT_BITS_PER_PIXEL,
#else
	.bits_per_pixel = 16,
#endif
	.grayscale       = 0,
	.red             = {0, 0, 0},
	.green           = {0, 0, 0},
	.blue            = {0, 0, 0},
	.transp          = {0, 0, 0},
	.nonstd          = 0,
	.activate        = FB_ACTIVATE_NOW,
	.height          = -1,
	.width           = -1,
	.accel_flags     = 0,
	.pixclock        = 0,
	.left_margin     = 0,
	.right_margin    = 0,
	.upper_margin    = 0,
	.lower_margin    = 0,
	.hsync_len       = 0,
	.vsync_len       = 0,
	.sync            = 0,
	.vmode           = FB_VMODE_NONINTERLACED,
	.rotate          = 0,
	
}

#ifdef  CONFIG_FB_OSD2_ENABLE
,
{
#if defined(CONFIG_FB_OSD2_DEFAULT_WIDTH)
	.xres            = CONFIG_FB_OSD2_DEFAULT_WIDTH,
#else
	.xres            = 32,
#endif
#if defined(CONFIG_FB_OSD2_DEFAULT_HEIGHT)
	.yres            = CONFIG_FB_OSD2_DEFAULT_HEIGHT,
#else
	.yres            = 32,
#endif
#if defined(CONFIG_FB_OSD2_DEFAULT_WIDTH_VIRTUAL)
	.xres_virtual    = CONFIG_FB_OSD2_DEFAULT_WIDTH_VIRTUAL,
#else
	.xres_virtual    = 32,
#endif
#if defined(CONFIG_FB_OSD2_DEFAULT_HEIGHT_VIRTUAL)
	.yres_virtual    = CONFIG_FB_OSD2_DEFAULT_HEIGHT_VIRTUAL,
#else
	.yres_virtual    = 32,
#endif
	.xoffset         = 0,
	.yoffset         = 0,
#if defined(CONFIG_FB_OSD2_DEFAULT_BITS_PER_PIXEL)
	.bits_per_pixel  = CONFIG_FB_OSD2_DEFAULT_BITS_PER_PIXEL,
#else
	.bits_per_pixel = 32,
#endif
	.grayscale       = 0,
	.red             = {0, 0, 0},  //leave as it is ,set by system.
	.green           = {0, 0, 0},
	.blue            = {0, 0, 0},
	.transp          = {0, 0, 0},
	.nonstd          = 0,
	.activate        = FB_ACTIVATE_NOW,
	.height          = -1,
	.width           = -1,
	.accel_flags     = 0,
	.pixclock        = 0,
	.left_margin     = 0,
	.right_margin    = 0,
	.upper_margin    = 0,
	.lower_margin    = 0,
	.hsync_len       = 0,
	.vsync_len       = 0,
	.sync            = 0,
	.vmode           = FB_VMODE_NONINTERLACED,
	.rotate          = 0,
}
#endif 
};


static struct fb_fix_screeninfo mydef_fix = {
	.id		    = "OSD FB",
	.xpanstep 	= 1,
	.ypanstep 	= 1,
	.type		= FB_TYPE_PACKED_PIXELS,
	.visual		= FB_VISUAL_TRUECOLOR,
	.accel		= FB_ACCEL_NONE,
};

typedef  struct {
	int x ;
	int y ;
	int w ;
	int h ;
}disp_rect_t;
#define DRIVER_NAME "osdfb"
#define MODULE_NAME "osdfb"
#define  FBIOPUT_OSD_SRCCOLORKEY	0x46fb
#define  FBIOPUT_OSD_SRCKEY_ENABLE	0x46fa
#define  FBIOPUT_OSD_SET_GBL_ALPHA	0x4500
#define  FBIOGET_OSD_GET_GBL_ALPHA	0x4501
#define  FBIOPUT_OSD_2X_SCALE		0x4502	
#define  FBIOPUT_OSD_ENABLE_3D_MODE	0x4503
#define  FBIOPUT_OSD_FREE_SCALE_ENABLE	0x4504
#define  FBIOPUT_OSD_FREE_SCALE_WIDTH	0x4505
#define  FBIOPUT_OSD_FREE_SCALE_HEIGHT	0x4506
#define  FBIOPUT_OSD_ORDER  		0x4507
#define  FBIOGET_OSD_ORDER  		0x4508
#define  FBIOGET_OSD_SCALE_AXIS		0x4509
#define  FBIOPUT_OSD_SCALE_AXIS		0x450a
#define  FBIOGET_OSD_BLOCK_WINDOWS	0x450b
#define  FBIOPUT_OSD_BLOCK_WINDOWS	0x450c
#define  FBIOGET_OSD_BLOCK_MODE		0x450d
#define  FBIOPUT_OSD_BLOCK_MODE		0x450e
#define  FBIOGET_OSD_FREE_SCALE_AXIS 0x450f
#define  FBIOPUT_OSD_FREE_SCALE_AXIS 0x4510








#endif /* OSD_MAIN_H */
