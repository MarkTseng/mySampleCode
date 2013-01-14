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

#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/fb.h>
#include <mach/am_regs.h>

#include <linux/osd/osd.h>
#include <linux/osd/osd_dev.h>
#include <linux/osd/osd_hw.h>
/* to-do: TV output mode should be configured by
 * sysfs attribute
 */

void osddev_set(struct myfb_dev *fbdev)
{
	
    fbdev_lock(fbdev);

    //memset((char*) fbdev->fb_mem,0x0,fbdev->fb_len);

    osd_setup(&fbdev->osd_ctl,
               fbdev->fb_info->var.xoffset,
               fbdev->fb_info->var.yoffset,
               fbdev->fb_info->var.xres,
               fbdev->fb_info->var.yres,
               fbdev->fb_info->var.xres_virtual,
               fbdev->fb_info->var.yres_virtual,
               fbdev->osd_ctl.disp_start_x,
               fbdev->osd_ctl.disp_start_y,
               fbdev->osd_ctl.disp_end_x,
               fbdev->osd_ctl.disp_end_y,
               fbdev->fb_mem_paddr,
               fbdev->color,
               fbdev->fb_info->node);	



    fbdev_unlock(fbdev);
    	
    return;
}
void osddev_update_disp_axis(struct myfb_dev *fbdev,int  mode_change)
{
	osddev_update_disp_axis_hw(	fbdev->osd_ctl.disp_start_x,
								fbdev->osd_ctl.disp_end_x,
               						fbdev->osd_ctl.disp_start_y,
               						fbdev->osd_ctl.disp_end_y,
               						fbdev->fb_info->var.xoffset,
               						fbdev->fb_info->var.yoffset,
               						mode_change,
               						fbdev->fb_info->node);
}
int osddev_setcolreg(unsigned regno, u16 red, u16 green, u16 blue,
        u16 transp, struct myfb_dev *fbdev)
{
    struct fb_info *info = fbdev->fb_info;

    if ((fbdev->color->color_index== COLOR_INDEX_02_PAL4) ||
        (fbdev->color->color_index == COLOR_INDEX_04_PAL16) ||
        (fbdev->color->color_index == COLOR_INDEX_08_PAL256)) {

        fbdev_lock(fbdev);

        osd_setpal_hw(regno, red, green, blue, transp,fbdev->fb_info->node);

        fbdev_unlock(fbdev);
    }

	if (info->fix.visual == FB_VISUAL_TRUECOLOR) {
		u32 v, r, g, b, a;

		if (regno >= 16)
			return 1;
	 
        r = red    >> (16 - info->var.red.length);
        g = green  >> (16 - info->var.green.length);
        b = blue   >> (16 - info->var.blue.length);
        a = transp >> (16 - info->var.transp.length);

		v = (r << info->var.red.offset)   |
		    (g << info->var.green.offset) |
		    (b << info->var.blue.offset)  |
		    (a << info->var.transp.offset);

  		((u32*)(info->pseudo_palette))[regno] = v;
	}

    return 0;
}
void osddev_init(void)
{
	osd_init_hw(0);
}
u32	osddev_get_osd_order(u32 index)
{
	return osd_get_osd_order_hw(index);
}
void osddev_change_osd_order(u32 index,u32 order)
{
	osd_change_osd_order_hw(index,order);
}
void osddev_free_scale_enable(u32 index ,u32 enable)
{
	//at present we only support osd1 & osd2 have the same random scale mode.
	osd_free_scale_enable_hw(index,enable);
	
}
void osddev_free_scale_width(u32 index ,u32 width)
{
	//at present we only support osd1 & osd2 have the same random scale mode.
	osd_free_scale_width_hw(index,width);
	
}
void osddev_free_scale_height(u32 index ,u32 height)
{
	//at present we only support osd1 & osd2 have the same random scale mode.
	osd_free_scale_height_hw(index,height);
	
}

void osddev_get_free_scale_axis(u32 index, s32 *x0, s32 *y0, s32 *x1, s32 *y1)
{
	//at present we only support osd1 & osd2 have the same random scale mode.
	osd_get_free_scale_axis_hw(index, x0, y0, x1, y1);

}

void osddev_set_free_scale_axis(u32 index, s32 x0, s32 y0, s32 x1, s32 y1)
{
	//at present we only support osd1 & osd2 have the same random scale mode.
	osd_set_free_scale_axis_hw(index, x0, y0, x1, y1);
}

void osddev_enable_3d_mode(u32 index ,u32 enable)
{
	osd_enable_3d_mode_hw(index,enable);
}

void osddev_set_scale_axis(u32 index, s32 x0, s32 y0, s32 x1, s32 y1)
{
	osd_set_scale_axis_hw(index, x0, y0, x1, y1);
}

void osddev_get_scale_axis(u32 index, s32 *x0, s32 *y0, s32 *x1, s32 *y1)
{
	osd_get_scale_axis_hw(index, x0, y0, x1, y1);
}

void osddev_set_2x_scale(u32 index,u16 h_scale_enable,u16 v_scale_enable)
{
	osd_set_2x_scale_hw(index,h_scale_enable,v_scale_enable);
}

void osddev_set_block_windows(u32 index, u32 *block_windows)
{
	osd_set_block_windows_hw(index, block_windows);
}

void osddev_get_block_windows(u32 index, u32 *block_windows)
{
	osd_get_block_windows_hw(index, block_windows);
}

void osddev_set_block_mode(u32 index, u32 mode)
{
	osd_set_block_mode_hw(index, mode);
}

void osddev_get_block_mode(u32 index, u32 *mode)
{
	osd_get_block_mode_hw(index, mode);
}

void osddev_enable(int enable,int  index)
{
    osd_enable_hw(enable,index);
}

void osddev_pan_display(struct fb_var_screeninfo *var,struct fb_info *fbi)
{
    osd_pan_display_hw(var->xoffset, var->yoffset,fbi->node);
}

#if defined(CONFIG_FB_OSD2_CURSOR)
void osddev_cursor(struct myfb_dev *fbdev, s16 x, s16 y, s16 xstart, s16 ystart, u32 osd_w, u32 osd_h)
{
    fbdev_lock(fbdev);
    osd_cursor_hw(x, y, xstart, ystart, osd_w, osd_h, fbdev->fb_info->node);
    fbdev_unlock(fbdev);
}
#endif

void  osddev_set_colorkey(u32 index,u32 bpp,u32 colorkey )
{
	osd_set_colorkey_hw( index, bpp, colorkey );
}
void  osddev_srckey_enable(u32  index,u8 enable)
{
	osd_srckey_enable_hw(index,enable);
}
void  osddev_set_gbl_alpha(u32 index,u32 gbl_alpha)
{
	osd_set_gbl_alpha_hw(index,gbl_alpha);
}
u32  osddev_get_gbl_alpha(u32  index)
{
	return osd_get_gbl_alpha_hw(index);
}
void  osddev_suspend(void)
{
	osd_suspend_hw();
}
void osddev_resume(void)
{
	osd_resume_hw();
}
