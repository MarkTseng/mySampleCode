/*
 * AMLOGIC Audio/Video streaming port driver.
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
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/platform_device.h>
#include <linux/amports/ptsserv.h>
#include <linux/amports/amstream.h>
#include <linux/amports/canvas.h>
#include <linux/amports/vframe.h>
#include <linux/amports/vframe_provider.h>
#include <linux/amports/vframe_receiver.h>
#include <mach/am_regs.h>

#ifdef CONFIG_AM_VDEC_MJPEG_LOG
#define AMLOG
#define LOG_LEVEL_VAR       amlog_level_vmjpeg
#define LOG_MASK_VAR        amlog_mask_vmjpeg
#define LOG_LEVEL_ERROR     0
#define LOG_LEVEL_INFO      1
#define LOG_LEVEL_DESC  "0:ERROR, 1:INFO"
#endif
#include <linux/amlog.h>
MODULE_AMLOG(LOG_LEVEL_ERROR, 0, LOG_LEVEL_DESC, LOG_DEFAULT_MASK_DESC);

#include "amvdec.h"
#include "vmjpeg_mc.h"

#define DRIVER_NAME "amvdec_mjpeg"
#define MODULE_NAME "amvdec_mjpeg"

/* protocol register usage
    AV_SCRATCH_0 - AV_SCRATCH_1 : initial display buffer fifo
    AV_SCRATCH_2 - AV_SCRATCH_3 : decoder settings
    AV_SCRATCH_4 - AV_SCRATCH_7 : display buffer spec
    AV_SCRATCH_8 - AV_SCRATCH_9 : amrisc/host display buffer management
    AV_SCRATCH_a                : time stamp
*/

#define MREG_DECODE_PARAM   AV_SCRATCH_2 /* bit 0-3: pico_addr_mode */
                                         /* bit 15-4: reference height */
#define MREG_TO_AMRISC      AV_SCRATCH_8
#define MREG_FROM_AMRISC    AV_SCRATCH_9
#define MREG_FRAME_OFFSET   AV_SCRATCH_A

#define PICINFO_BUF_IDX_MASK        0x0007
#define PICINFO_AVI1                0x0080
#define PICINFO_INTERLACE           0x0020
#define PICINFO_INTERLACE_AVI1_BOT  0x0010
#define PICINFO_INTERLACE_FIRST     0x0010

#define VF_POOL_SIZE        12
#define PUT_INTERVAL        HZ/100

#define INCPTR(p) ptr_atomic_wrap_inc(&p)

#define STAT_TIMER_INIT     0x01
#define STAT_MC_LOAD        0x02
#define STAT_ISR_REG        0x04
#define STAT_VF_HOOK        0x08
#define STAT_TIMER_ARM      0x10
#define STAT_VDEC_RUN       0x20

static struct dec_sysinfo vmjpeg_amstream_dec_info;

static vframe_t *vmjpeg_vf_peek(void*);
static vframe_t *vmjpeg_vf_get(void*);
static void vmjpeg_vf_put(vframe_t *, void*);
static int  vmjpeg_vf_states(vframe_states_t *states, void*);
static int vmjpeg_event_cb(int type, void *data, void *private_data);

static void vmjpeg_prot_init(void);
static void vmjpeg_local_init(void);

static const char vmjpeg_dec_id[] = "vmjpeg-dev";

#define PROVIDER_NAME   "decoder.mjpeg"
static const struct vframe_operations_s vmjpeg_vf_provider = {
    .peek = vmjpeg_vf_peek,
    .get  = vmjpeg_vf_get,
    .put  = vmjpeg_vf_put,
    .event_cb =  vmjpeg_event_cb,
    .vf_states = vmjpeg_vf_states,
};
static struct vframe_provider_s vmjpeg_vf_prov;

static struct vframe_s vfpool[VF_POOL_SIZE];
static u32 vfpool_idx[VF_POOL_SIZE];
static s32 vfbuf_use[4];
static s32 fill_ptr, get_ptr, putting_ptr, put_ptr;
static u32 frame_width, frame_height, frame_dur;
static struct timer_list recycle_timer;
static u32 stat;
static u32 buf_start, buf_size;
static spinlock_t lock = SPIN_LOCK_UNLOCKED;

static inline u32 index2canvas0(u32 index)
{
    const u32 canvas_tab[4] = {
        0x020100, 0x050403, 0x080706, 0x0b0a09
    };

    return canvas_tab[index];
}

static inline u32 index2canvas1(u32 index)
{
    const u32 canvas_tab[4] = {
        0x0e0d0c, 0x11100f, 0x141312, 0x171615
    };

    return canvas_tab[index];
}

static inline void ptr_atomic_wrap_inc(u32 *ptr)
{
    u32 i = *ptr;

    i++;

    if (i >= VF_POOL_SIZE) {
        i = 0;
    }

    *ptr = i;
}

static void set_frame_info(vframe_t *vf)
{
    vf->width  = frame_width;
    vf->height = frame_height;
    vf->duration = frame_dur;
    vf->ratio_control = 0;
    vf->duration_pulldown = 0;
}

static irqreturn_t vmjpeg_isr(int irq, void *dev_id)
{
    u32 reg, offset, pts, pts_valid = 0;
    vframe_t *vf;

    WRITE_MPEG_REG(ASSIST_MBOX1_CLR_REG, 1);

    reg = READ_MPEG_REG(MREG_FROM_AMRISC);

    if (reg & PICINFO_BUF_IDX_MASK) {
        offset = READ_MPEG_REG(MREG_FRAME_OFFSET);

        if (pts_lookup_offset(PTS_TYPE_VIDEO, offset, &pts, 0) == 0) {
            pts_valid = 1;
        }

        if ((reg & PICINFO_INTERLACE) == 0) {
            u32 index = ((reg & PICINFO_BUF_IDX_MASK) - 1) & 3;

            vfpool_idx[fill_ptr] = index;
            vf = &vfpool[fill_ptr];

            set_frame_info(vf);

            vf->type = VIDTYPE_PROGRESSIVE | VIDTYPE_VIU_FIELD;
            vf->canvas0Addr = vf->canvas1Addr = index2canvas0(index);
            vf->pts = (pts_valid) ? pts : 0;

            vfbuf_use[index]++;

            INCPTR(fill_ptr);
		vf_notify_receiver(PROVIDER_NAME,VFRAME_EVENT_PROVIDER_VFRAME_READY,NULL);               

        } else {
            u32 index = ((reg & PICINFO_BUF_IDX_MASK) - 1) & 3;

            vfpool_idx[fill_ptr] = index;
            vf = &vfpool[fill_ptr];

            set_frame_info(vf);

#if 0
            if (reg & PICINFO_AVI1) {
                /* AVI1 format */
                if (reg & PICINFO_INTERLACE_AVI1_BOT) {
                    vf->type = VIDTYPE_INTERLACE_BOTTOM | VIDTYPE_INTERLACE_FIRST;
                } else {
                    vf->type = VIDTYPE_INTERLACE_TOP;
                }
            } else {
                if (reg & PICINFO_INTERLACE_FIRST) {
                    vf->type = VIDTYPE_INTERLACE_TOP | VIDTYPE_INTERLACE_FIRST;
                } else {
                    vf->type = VIDTYPE_INTERLACE_BOTTOM;
                }
            }

            vf->type |= VIDTYPE_VIU_FIELD;
            vf->duration >>= 1;
            vf->canvas0Addr = vf->canvas1Addr = index2canvas0(index);

            if ((vf->type & VIDTYPE_INTERLACE_FIRST) && (pts_valid)) {
                vf->pts = pts;
            } else {
                vf->pts = 0;
            }

            vfbuf_use[index]++;

            INCPTR(fill_ptr);
#else
            /* send whole frame by weaving top & bottom field */
            vf->type = VIDTYPE_PROGRESSIVE;
            vf->canvas0Addr = index2canvas0(index);
            vf->canvas1Addr = index2canvas1(index);

            if (pts_valid) {
                vf->pts = pts;
            } else {
                vf->pts = 0;
            }

            vfbuf_use[index]++;

            INCPTR(fill_ptr);
		vf_notify_receiver(PROVIDER_NAME,VFRAME_EVENT_PROVIDER_VFRAME_READY,NULL);            
#endif
        }

        WRITE_MPEG_REG(MREG_FROM_AMRISC, 0);
    }

    return IRQ_HANDLED;
}

static vframe_t *vmjpeg_vf_peek(void* op_arg)
{
    if (get_ptr == fill_ptr) {
        return NULL;
    }

    return &vfpool[get_ptr];
}

static vframe_t *vmjpeg_vf_get(void* op_arg)
{
    vframe_t *vf;

    if (get_ptr == fill_ptr) {
        return NULL;
    }

    vf = &vfpool[get_ptr];

    INCPTR(get_ptr);

    return vf;
}

static void vmjpeg_vf_put(vframe_t *vf, void* op_arg)
{
    INCPTR(putting_ptr);
}

static int vmjpeg_event_cb(int type, void *data, void *private_data)
{
    if(type & VFRAME_EVENT_RECEIVER_RESET){
        unsigned long flags;
        amvdec_stop();
#ifndef CONFIG_POST_PROCESS_MANAGER
        vf_light_unreg_provider(&vmjpeg_vf_prov);
#endif
        spin_lock_irqsave(&lock, flags);
        vmjpeg_local_init();
        vmjpeg_prot_init();
        spin_unlock_irqrestore(&lock, flags); 
#ifndef CONFIG_POST_PROCESS_MANAGER
        vf_reg_provider(&vmjpeg_vf_prov);
#endif              
        amvdec_start();
    }
    return 0;        
}

static int  vmjpeg_vf_states(vframe_states_t *states, void* op_arg)
{
    int i;
    unsigned long flags;
    spin_lock_irqsave(&lock, flags);

    states->vf_pool_size = VF_POOL_SIZE;
    i = put_ptr - fill_ptr;
    if (i < 0) i += VF_POOL_SIZE;
    states->buf_free_num = i;
    
    i = putting_ptr - put_ptr;
    if (i < 0) i += VF_POOL_SIZE;
    states->buf_recycle_num = i;
    
    i = fill_ptr - get_ptr;
    if (i < 0) i += VF_POOL_SIZE;
    states->buf_avail_num = i;

    spin_unlock_irqrestore(&lock, flags);
    return 0;
}

static void vmjpeg_put_timer_func(unsigned long arg)
{
    struct timer_list *timer = (struct timer_list *)arg;

    while ((putting_ptr != put_ptr) && (READ_MPEG_REG(MREG_TO_AMRISC) == 0)) {
        u32 index = vfpool_idx[put_ptr];

        if (--vfbuf_use[index] == 0) {
            WRITE_MPEG_REG(MREG_TO_AMRISC, index + 1);
        }

        INCPTR(put_ptr);
    }

    timer->expires = jiffies + PUT_INTERVAL;

    add_timer(timer);
}

int vmjpeg_dec_status(struct vdec_status *vstatus)
{
    vstatus->width = frame_width;
    vstatus->height = frame_height;
    if (0 != frame_dur) {
        vstatus->fps = 96000 / frame_dur;
    } else {
        vstatus->fps = 96000;
    }
    vstatus->error_count = 0;
    vstatus->status = stat;

    return 0;
}

/****************************************/
static void vmjpeg_canvas_init(void)
{
    int i;
    u32 canvas_width, canvas_height;
    u32 decbuf_size, decbuf_y_size, decbuf_uv_size;

    if (buf_size <= 0x00400000) {
        /* SD only */
        canvas_width   = 768;
        canvas_height  = 576;
        decbuf_y_size  = 0x80000;
        decbuf_uv_size = 0x20000;
        decbuf_size    = 0x100000;
    } else {
        /* HD & SD */
        canvas_width   = 1920;
        canvas_height  = 1088;
        decbuf_y_size  = 0x200000;
        decbuf_uv_size = 0x80000;
        decbuf_size    = 0x300000;
    }

    for (i = 0; i < 4; i++) {
        canvas_config(3 * i + 0,
                      buf_start + i * decbuf_size,
                      canvas_width, canvas_height,
                      CANVAS_ADDR_NOWRAP, CANVAS_BLKMODE_LINEAR);
        canvas_config(3 * i + 1,
                      buf_start + i * decbuf_size + decbuf_y_size,
                      canvas_width / 2, canvas_height / 2,
                      CANVAS_ADDR_NOWRAP, CANVAS_BLKMODE_LINEAR);
        canvas_config(3 * i + 2,
                      buf_start + i * decbuf_size + decbuf_y_size + decbuf_uv_size,
                      canvas_width / 2, canvas_height / 2,
                      CANVAS_ADDR_NOWRAP, CANVAS_BLKMODE_LINEAR);
        canvas_config(3 * i + 0 + 12,
                      buf_start + i * decbuf_size + decbuf_size / 2,
                      canvas_width, canvas_height,
                      CANVAS_ADDR_NOWRAP, CANVAS_BLKMODE_LINEAR);
        canvas_config(3 * i + 1 + 12,
                      buf_start + i * decbuf_size + decbuf_y_size + decbuf_uv_size / 2,
                      canvas_width / 2, canvas_height / 2,
                      CANVAS_ADDR_NOWRAP, CANVAS_BLKMODE_LINEAR);
        canvas_config(3 * i + 2 + 12,
                      buf_start + i * decbuf_size + decbuf_y_size + decbuf_uv_size + decbuf_uv_size / 2,
                      canvas_width / 2, canvas_height / 2,
                      CANVAS_ADDR_NOWRAP, CANVAS_BLKMODE_LINEAR);
    }
}

static void init_scaler(void)
{
    /* 4 point triangle */
    const unsigned filt_coef[] = {
        0x20402000, 0x20402000, 0x1f3f2101, 0x1f3f2101,
        0x1e3e2202, 0x1e3e2202, 0x1d3d2303, 0x1d3d2303,
        0x1c3c2404, 0x1c3c2404, 0x1b3b2505, 0x1b3b2505,
        0x1a3a2606, 0x1a3a2606, 0x19392707, 0x19392707,
        0x18382808, 0x18382808, 0x17372909, 0x17372909,
        0x16362a0a, 0x16362a0a, 0x15352b0b, 0x15352b0b,
        0x14342c0c, 0x14342c0c, 0x13332d0d, 0x13332d0d,
        0x12322e0e, 0x12322e0e, 0x11312f0f, 0x11312f0f,
        0x10303010
    };
    int i;

    /* pscale enable, PSCALE cbus bmem enable */
    WRITE_MPEG_REG(PSCALE_CTRL, 0xc000);

    /* write filter coefs */
    WRITE_MPEG_REG(PSCALE_BMEM_ADDR, 0);
    for (i = 0; i < 33; i++) {
        WRITE_MPEG_REG(PSCALE_BMEM_DAT, 0);
        WRITE_MPEG_REG(PSCALE_BMEM_DAT, filt_coef[i]);
    }

    /* Y horizontal initial info */
    WRITE_MPEG_REG(PSCALE_BMEM_ADDR, 37 * 2);
    /* [35]: buf repeat pix0,
     * [34:29] => buf receive num,
     * [28:16] => buf blk x,
     * [15:0] => buf phase
     */
    WRITE_MPEG_REG(PSCALE_BMEM_DAT, 0x0008);
    WRITE_MPEG_REG(PSCALE_BMEM_DAT, 0x60000000);

    /* C horizontal initial info */
    WRITE_MPEG_REG(PSCALE_BMEM_ADDR, 41 * 2);
    WRITE_MPEG_REG(PSCALE_BMEM_DAT, 0x0008);
    WRITE_MPEG_REG(PSCALE_BMEM_DAT, 0x60000000);

    /* Y vertical initial info */
    WRITE_MPEG_REG(PSCALE_BMEM_ADDR, 39 * 2);
    WRITE_MPEG_REG(PSCALE_BMEM_DAT, 0x0008);
    WRITE_MPEG_REG(PSCALE_BMEM_DAT, 0x60000000);

    /* C vertical initial info */
    WRITE_MPEG_REG(PSCALE_BMEM_ADDR, 43 * 2);
    WRITE_MPEG_REG(PSCALE_BMEM_DAT, 0x0008);
    WRITE_MPEG_REG(PSCALE_BMEM_DAT, 0x60000000);

    /* Y horizontal phase step */
    WRITE_MPEG_REG(PSCALE_BMEM_ADDR, 36 * 2 + 1);
    /* [19:0] => Y horizontal phase step */
    WRITE_MPEG_REG(PSCALE_BMEM_DAT, 0x10000);
    /* C horizontal phase step */
    WRITE_MPEG_REG(PSCALE_BMEM_ADDR, 40 * 2 + 1);
    /* [19:0] => C horizontal phase step */
    WRITE_MPEG_REG(PSCALE_BMEM_DAT, 0x10000);

    /* Y vertical phase step */
    WRITE_MPEG_REG(PSCALE_BMEM_ADDR, 38 * 2 + 1);
    /* [19:0] => Y vertical phase step */
    WRITE_MPEG_REG(PSCALE_BMEM_DAT, 0x10000);
    /* C vertical phase step */
    WRITE_MPEG_REG(PSCALE_BMEM_ADDR, 42 * 2 + 1);
    /* [19:0] => C horizontal phase step */
    WRITE_MPEG_REG(PSCALE_BMEM_DAT, 0x10000);

    /* reset pscaler */
    WRITE_MPEG_REG(RESET2_REGISTER, RESET_PSCALE);
    READ_MPEG_REG(RESET2_REGISTER);
    READ_MPEG_REG(RESET2_REGISTER);
    READ_MPEG_REG(RESET2_REGISTER);

    WRITE_MPEG_REG(PSCALE_RST, 0x7);
    WRITE_MPEG_REG(PSCALE_RST, 0x0);
}

static void vmjpeg_prot_init(void)
{
    WRITE_MPEG_REG(RESET0_REGISTER, RESET_IQIDCT | RESET_MC);

    vmjpeg_canvas_init();

    WRITE_MPEG_REG(AV_SCRATCH_0, 12);
    WRITE_MPEG_REG(AV_SCRATCH_1, 0x031a);
    WRITE_MPEG_REG(AV_SCRATCH_4, 0x020100);
    WRITE_MPEG_REG(AV_SCRATCH_5, 0x050403);
    WRITE_MPEG_REG(AV_SCRATCH_6, 0x080706);
    WRITE_MPEG_REG(AV_SCRATCH_7, 0x0b0a09);

    init_scaler();

    /* clear buffer IN/OUT registers */
    WRITE_MPEG_REG(MREG_TO_AMRISC, 0);
    WRITE_MPEG_REG(MREG_FROM_AMRISC, 0);

    WRITE_MPEG_REG(MCPU_INTR_MSK, 0xffff);
    WRITE_MPEG_REG(MREG_DECODE_PARAM, (frame_height << 4) | 0x8000);

    /* clear mailbox interrupt */
    WRITE_MPEG_REG(ASSIST_MBOX1_CLR_REG, 1);
    /* enable mailbox interrupt */
    WRITE_MPEG_REG(ASSIST_MBOX1_MASK, 1);
    /* set interrupt mapping for vld */
    WRITE_MPEG_REG(ASSIST_AMR1_INT8, 8);
}

static void vmjpeg_local_init(void)
{
    int i;

    fill_ptr = get_ptr = put_ptr = putting_ptr = 0;

    frame_width = vmjpeg_amstream_dec_info.width;
    frame_height = vmjpeg_amstream_dec_info.height;
    frame_dur = vmjpeg_amstream_dec_info.rate;

    amlog_level(LOG_LEVEL_INFO, "mjpegdec: w(%d), h(%d), dur(%d)\n", frame_width, frame_height, frame_dur);

    for (i = 0; i < 4; i++) {
        vfbuf_use[i] = 0;
    }
}

static s32 vmjpeg_init(void)
{
    int r;

    init_timer(&recycle_timer);

    stat |= STAT_TIMER_INIT;

    amvdec_enable();

    vmjpeg_local_init();

    if (amvdec_loadmc(vmjpeg_mc) < 0) {
        amvdec_disable();

        return -EBUSY;
    }

    stat |= STAT_MC_LOAD;

    /* enable AMRISC side protocol */
    vmjpeg_prot_init();

    r = request_irq(INT_MAILBOX_1A, vmjpeg_isr,
                    IRQF_SHARED, "vmjpeg-irq", (void *)vmjpeg_dec_id);

    if (r) {
        amvdec_disable();

        amlog_level(LOG_LEVEL_ERROR, "vmjpeg irq register error.\n");
        return -ENOENT;
    }

    stat |= STAT_ISR_REG;

 #ifdef CONFIG_POST_PROCESS_MANAGER
    vf_provider_init(&vmjpeg_vf_prov, PROVIDER_NAME, &vmjpeg_vf_provider, NULL);
    vf_reg_provider(&vmjpeg_vf_prov);
    vf_notify_receiver(PROVIDER_NAME,VFRAME_EVENT_PROVIDER_START,NULL);               
 #else 
    vf_provider_init(&vmjpeg_vf_prov, PROVIDER_NAME, &vmjpeg_vf_provider, NULL);
    vf_reg_provider(&vmjpeg_vf_prov);
 #endif 

    stat |= STAT_VF_HOOK;

    recycle_timer.data = (ulong)&recycle_timer;
    recycle_timer.function = vmjpeg_put_timer_func;
    recycle_timer.expires = jiffies + PUT_INTERVAL;

    add_timer(&recycle_timer);

    stat |= STAT_TIMER_ARM;

    amvdec_start();

    stat |= STAT_VDEC_RUN;

    set_vdec_func(&vmjpeg_dec_status);

    return 0;
}

static int amvdec_mjpeg_probe(struct platform_device *pdev)
{
    struct resource *mem;

    amlog_level(LOG_LEVEL_INFO, "amvdec_mjpeg probe start.\n");

    if (!(mem = platform_get_resource(pdev, IORESOURCE_MEM, 0))) {
        amlog_level(LOG_LEVEL_ERROR, "amvdec_mjpeg memory resource undefined.\n");
        return -EFAULT;
    }

    buf_start = mem->start;
    buf_size  = mem->end - mem->start + 1;

    memcpy(&vmjpeg_amstream_dec_info, (void *)mem[1].start, sizeof(vmjpeg_amstream_dec_info));

    if (vmjpeg_init() < 0) {
        amlog_level(LOG_LEVEL_ERROR, "amvdec_mjpeg init failed.\n");

        return -ENODEV;
    }

    amlog_level(LOG_LEVEL_INFO, "amvdec_mjpeg probe end.\n");

    return 0;
}

static int amvdec_mjpeg_remove(struct platform_device *pdev)
{
    if (stat & STAT_VDEC_RUN) {
        amvdec_stop();
        stat &= ~STAT_VDEC_RUN;
    }

    if (stat & STAT_ISR_REG) {
        free_irq(INT_MAILBOX_1A, (void *)vmjpeg_dec_id);
        stat &= ~STAT_ISR_REG;
    }

    if (stat & STAT_TIMER_ARM) {
        del_timer_sync(&recycle_timer);
        stat &= ~STAT_TIMER_ARM;
    }

    if (stat & STAT_VF_HOOK) {
        vf_unreg_provider(&vmjpeg_vf_prov);
        stat &= ~STAT_VF_HOOK;
    }

    amvdec_disable();

    amlog_level(LOG_LEVEL_INFO, "amvdec_mjpeg remove.\n");

    return 0;
}

/****************************************/

static struct platform_driver amvdec_mjpeg_driver = {
    .probe      = amvdec_mjpeg_probe,
    .remove     = amvdec_mjpeg_remove,
#ifdef CONFIG_PM
    .suspend    = amvdec_suspend,
    .resume     = amvdec_resume,
#endif
    .driver     = {
        .name   = DRIVER_NAME,
    }
};
static struct codec_profile_t amvdec_mjpeg_profile = {
	.name = "mjpeg",
	.profile = ""
};
static int __init amvdec_mjpeg_driver_init_module(void)
{
    amlog_level(LOG_LEVEL_INFO, "amvdec_mjpeg module init\n");

    if (platform_driver_register(&amvdec_mjpeg_driver)) {
        amlog_level(LOG_LEVEL_ERROR, "failed to register amvdec_mjpeg driver\n");
        return -ENODEV;
    }
	vcodec_profile_register(&amvdec_mjpeg_profile);
    return 0;
}

static void __exit amvdec_mjpeg_driver_remove_module(void)
{
    amlog_level(LOG_LEVEL_INFO, "amvdec_mjpeg module remove.\n");

    platform_driver_unregister(&amvdec_mjpeg_driver);
}

/****************************************/

module_param(stat, uint, 0664);
MODULE_PARM_DESC(stat, "\n amvdec_mjpeg stat \n");

module_init(amvdec_mjpeg_driver_init_module);
module_exit(amvdec_mjpeg_driver_remove_module);

MODULE_DESCRIPTION("AMLOGIC MJMPEG Video Decoder Driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tim Yao <timyao@amlogic.com>");
