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
 * Author:	Tim Yao <timyao@amlogic.com>
 *
 */

#ifndef OSD_DEV_H
#define OSD_DEV_H

#include "osd.h"
#include <linux/vout/vinfo.h>
#include <linux/logo/logo.h>

#define  OSD_COUNT 	2 /* we have two osd layer on hardware*/

#define KEYCOLOR_FLAG_TARGET  1
#define KEYCOLOR_FLAG_ONHOLD  2
#define KEYCOLOR_FLAG_CURRENT 4

typedef struct myfb_dev {
    struct mutex lock;

    struct fb_info *fb_info;
    struct platform_device *dev;

	u32 fb_mem_paddr;
	void __iomem *fb_mem_vaddr;
	u32 fb_len;
	const color_bit_define_t  *color;
   	 vmode_t vmode;
    	
    	struct osd_ctl_s osd_ctl;
	u32  order;	
	u32  scale;	
	u32  enable_3d;
	u32  preblend_enable;
	u32  enable_key_flag;
	u32  color_key;	
} myfb_dev_t;
typedef  struct list_head   list_head_t ;

typedef   struct{
	list_head_t  list;
	struct myfb_dev *fbdev;
}fb_list_t,*pfb_list_t;

typedef  struct {
	int start ;
	int end ;
	int fix_line_length;
}osd_addr_t ;



#define fbdev_lock(dev) mutex_lock(&dev->lock);
#define fbdev_unlock(dev) mutex_unlock(&dev->lock);
extern u32 osddev_get_osd_order(u32 index);
extern void osddev_change_osd_order(u32 index,u32 order);
extern void osddev_free_scale_enable(u32 index ,u32 enable);
extern void osddev_free_scale_width(u32 index ,u32 width);
extern void osddev_free_scale_height(u32 index ,u32 height);
extern void osddev_get_free_scale_axis(u32 index, s32 *x0, s32 *y0, s32 *x1, s32 *y1);
extern void osddev_set_free_scale_axis(u32 index, s32 x0, s32 y0, s32 x1, s32 y1);
extern void osddev_get_scale_axis(u32 index, s32 *x0, s32 *y0, s32 *x1, s32 *y1);
extern void osddev_set_scale_axis(u32 index, s32 x0, s32 y0, s32 x1, s32 y1);
extern void osddev_get_block_windows(u32 index, u32 *windows);
extern void osddev_set_block_windows(u32 index, u32 *windows);
extern void osddev_get_block_mode(u32 index, u32 *mode);
extern void osddev_set_block_mode(u32 index, u32 mode);
extern int osddev_select_mode(struct myfb_dev *fbdev);
extern void osddev_enable_3d_mode(u32 index ,u32 enable);
extern void osddev_set_2x_scale(u32 index,u16 h_scale_enable,u16 v_scale_enable);
extern void osddev_set(struct myfb_dev *fbdev);
extern void osddev_update_disp_axis(struct myfb_dev *fbdev,int  mode_change) ;
extern int osddev_setcolreg(unsigned regno, u16 red, u16 green, u16 blue,
        u16 transp, struct myfb_dev *fbdev);
extern void osddev_init(void) ;        
extern void osddev_enable(int enable,int index);

extern void osddev_pan_display(struct fb_var_screeninfo *var,struct fb_info *fbi);

#if defined (CONFIG_FB_OSD2_CURSOR)
extern void osddev_cursor(struct myfb_dev *fbdev, s16 x, s16 y, s16 xstart, s16 ystart, u32 osd_w, u32 osd_h);
#endif

extern  void  osddev_set_colorkey(u32 index,u32 bpp,u32 colorkey );
extern  void  osddev_srckey_enable(u32  index,u8 enable);
extern void  osddev_set_gbl_alpha(u32 index,u32 gbl_alpha) ;
extern u32  osddev_get_gbl_alpha(u32  index);
extern  void  osddev_suspend(void);
extern  void  osddev_resume(void);
#endif /* OSDFBDEV_H */

