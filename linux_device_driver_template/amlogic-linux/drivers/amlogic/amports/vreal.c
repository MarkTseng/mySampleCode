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
 * Author:  Chen Zhang <chen.zhang@amlogic.com>
 *
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/amports/amstream.h>
#include <linux/amports/ptsserv.h>
#include <linux/amports/canvas.h>
#include <linux/amports/vframe.h>
#include <linux/amports/vframe_provider.h>
#include <linux/amports/vframe_receiver.h>
#include <mach/am_regs.h>
#include <asm/uaccess.h>

#include "amvdec.h"
#include "vreal_mc.h"

#include "streambuf.h"
#include "streambuf_reg.h"

#include "rmparser.h"
#include "vreal.h"

#define DRIVER_NAME "amvdec_real"
#define MODULE_NAME "amvdec_real"

#define HANDLE_REAL_IRQ

#define RM_DEF_BUFFER_ADDR        0x81000000
/* protocol registers */
#define STATUS_AMRISC   AV_SCRATCH_4
#define PARSER_ERROR_WRONG_PACKAGE_SIZE 0x80
#define PARSER_ERROR_WRONG_HEAD_VER     0x40
#define DECODER_ERROR_VLC_DECODE_TBL    0x20

#define RV_PIC_INFO     AV_SCRATCH_5
#define VPTS_TR          AV_SCRATCH_6
#define VDTS             AV_SCRATCH_7
#define FROM_AMRISC     AV_SCRATCH_8
#define TO_AMRISC       AV_SCRATCH_9
#define SKIP_B_AMRISC   AV_SCRATCH_A
#define MDEC_WIDTH      HARM_ASB_MB2
#define MDEC_HEIGHT     HASB_ARM_MB0

#define PARC_FORBIDDEN              0
#define PARC_SQUARE                 1
#define PARC_CIF                    2
#define PARC_10_11                  3
#define PARC_16_11                  4
#define PARC_40_33                  5
#define PARC_RESERVED               6
/* values between 6 and 14 are reserved */
#define PARC_EXTENDED              15

#define VF_POOL_SIZE        12
#define VF_BUF_NUM          4
#define PUT_INTERVAL        HZ/100

#define INCPTR(p) ptr_atomic_wrap_inc(&p)

#define STAT_TIMER_INIT     0x01
#define STAT_MC_LOAD        0x02
#define STAT_ISR_REG        0x04
#define STAT_VF_HOOK        0x08
#define STAT_TIMER_ARM      0x10
#define STAT_VDEC_RUN       0x20

#define REAL_RECYCLE_Q_BITS 3
#define REAL_RECYCLE_Q_SIZE (1<<(REAL_RECYCLE_Q_BITS))
#define REAL_RECYCLE_Q_MASK ((REAL_RECYCLE_Q_SIZE)-1)

static vframe_t *vreal_vf_peek(void*);
static vframe_t *vreal_vf_get(void*);
static void vreal_vf_put(vframe_t *, void*);
static int  vreal_vf_states(vframe_states_t *states, void*);
static int vreal_event_cb(int type, void *data, void *private_data);

static void vreal_prot_init(void);
static void vreal_local_init(void);

static const char vreal_dec_id[] = "vreal-dev";

#define PROVIDER_NAME   "decoder.real"
static const struct vframe_operations_s vreal_vf_provider = {
    .peek = vreal_vf_peek,
    .get = vreal_vf_get,
    .put = vreal_vf_put,
    .event_cb = vreal_event_cb,
    .vf_states = vreal_vf_states,
};
static struct vframe_provider_s vreal_vf_prov;

static struct vframe_s vfpool[VF_POOL_SIZE];
static u32 vfpool_idx[VF_POOL_SIZE];
static s32 vfbuf_use[4];
static s32 fill_ptr, get_ptr, putting_ptr, put_ptr;
static u32 frame_width, frame_height, frame_dur, frame_prog;
static struct timer_list recycle_timer;
static u32 stat;
static u32 buf_start, buf_size, buf_offset;
static u32 vreal_ratio;
u32 vreal_format;
static u32 wait_key_frame;
static u32 last_tr;
static u32 frame_count;
static u32 current_vdts;
static u32 hold;
static u32 decoder_state;
static u32 real_err_count;

static u32 real_recycle_q[REAL_RECYCLE_Q_SIZE];
static u32 real_recycle_rd;
static u32 real_recycle_wr;

static spinlock_t lock = SPIN_LOCK_UNLOCKED;

static unsigned short pic_sz_tbl[12] __attribute__((aligned(32)));
static dma_addr_t pic_sz_tbl_map;
static const unsigned char RPR_size[9] = {0, 1, 1, 2, 2, 3, 3, 3, 3};

static struct dec_sysinfo vreal_amstream_dec_info;

static unsigned char aspect_ratio_table[16] = {
    PARC_FORBIDDEN,
    PARC_SQUARE,
    PARC_CIF,
    PARC_10_11,
    PARC_16_11,
    PARC_40_33,
    PARC_RESERVED, PARC_RESERVED, PARC_RESERVED, PARC_RESERVED,
    PARC_RESERVED, PARC_RESERVED, PARC_RESERVED, PARC_RESERVED,
    PARC_RESERVED, PARC_EXTENDED
};

static inline u32 index2canvas(u32 index)
{
    const u32 canvas_tab[4] = {
        0x020100, 0x050403, 0x080706, 0x0b0a09
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

static void set_aspect_ratio(vframe_t *vf, unsigned pixel_ratio)
{
    int ar = 0;

    if (vreal_ratio == 0) {
        vf->ratio_control |= (0x90 << DISP_RATIO_ASPECT_RATIO_BIT); // always stretch to 16:9
    } else {
        switch (aspect_ratio_table[pixel_ratio]) {
        case 0:
            ar = vreal_amstream_dec_info.height * vreal_ratio / vreal_amstream_dec_info.width;
            break;
        case 1:
        case 0xff:
            ar = vreal_ratio * vf->height / vf->width;
            break;
        case 2:
            ar = (vreal_ratio * vf->height * 12) / (vf->width * 11);
            break;
        case 3:
            ar = (vreal_ratio * vf->height * 11) / (vf->width * 10);
            break;
        case 4:
            ar = (vreal_ratio * vf->height * 11) / (vf->width * 16);
            break;
        case 5:
            ar = (vreal_ratio * vf->height * 33) / (vf->width * 40);
            break;
        default:
            ar = vreal_ratio * vf->height / vf->width;
            break;
        }
    }

    ar = min(ar, DISP_RATIO_ASPECT_RATIO_MAX);

    vf->ratio_control |= (ar << DISP_RATIO_ASPECT_RATIO_BIT);
}

#ifdef HANDLE_REAL_IRQ
static irqreturn_t vreal_isr(int irq, void *dev_id)
#else
static void vreal_isr(void)
#endif
{
    u32 from;
    vframe_t *vf;
    u32 buffer_index;
    unsigned int status;
    unsigned int vdts;
    unsigned int info;
    unsigned int tr;
    unsigned int pictype;

    if (decoder_state == 0) {
#ifdef HANDLE_REAL_IRQ
        return IRQ_HANDLED;
#else
        return;
#endif
    }

    status = READ_MPEG_REG(STATUS_AMRISC);
    if (status & (PARSER_ERROR_WRONG_PACKAGE_SIZE | PARSER_ERROR_WRONG_HEAD_VER | DECODER_ERROR_VLC_DECODE_TBL)) {
        // decoder or parser error
        real_err_count++;
        //printk("real decoder or parser error, status 0x%x\n", status);
    }

    from = READ_MPEG_REG(FROM_AMRISC);
    if ((hold == 0) && from) {
        tr = READ_MPEG_REG(VPTS_TR);
        pictype = (tr >> 13) & 3;
        tr = (tr & 0x1fff) * 96;
        vf = &vfpool[fill_ptr];
        vdts = READ_MPEG_REG(VDTS);
        if (last_tr == -1) { // ignore tr for first time
            vf->duration = frame_dur;
        } else {
            if (tr > last_tr) {
                vf->duration = tr - last_tr;
            } else {
                vf->duration = (96 << 13) + tr - last_tr;
            }

            if (vf->duration > 10 * frame_dur) {
                // not a reasonable duration, should not happen
                vf->duration = frame_dur;
            }
#if 0
            else {
                if (check_frame_duration == 0) {
                    frame_dur = vf->duration;
                    check_frame_duration = 1;
                }
            }
#endif
        }

        last_tr = tr;
        buffer_index = from & 0x03;

        if (0 == pictype) { // I
            current_vdts = vdts * 90 + 1;
            vf->pts = current_vdts;
            if (wait_key_frame) {
                wait_key_frame = 0;
            }
        } else {
            if (wait_key_frame) {
                while (READ_MPEG_REG(TO_AMRISC)) {}
                WRITE_MPEG_REG(TO_AMRISC, ~(1 << buffer_index));
                //INCPTR(put_ptr);
                WRITE_MPEG_REG(FROM_AMRISC, 0);
#ifdef HANDLE_REAL_IRQ
                return IRQ_HANDLED;
#else
                return;
#endif
            } else {
                current_vdts += vf->duration - (vf->duration >> 4);
                vf->pts = current_vdts;
            }
        }

        //printk("pts %d, picture type %d\n", vf->pts, pictype);

        info = READ_MPEG_REG(RV_PIC_INFO);
        vf->width = info >> 16;
        vf->height = (info >> 4) & 0xfff;
        vf->bufWidth = 1920;
        vf->ratio_control = 0;
        set_aspect_ratio(vf, info & 0x0f);
        vf->duration_pulldown = 0;
        vf->type = VIDTYPE_PROGRESSIVE | VIDTYPE_VIU_FIELD;
        vf->canvas0Addr = vf->canvas1Addr = index2canvas(buffer_index);

        vfpool_idx[fill_ptr] = buffer_index;

        vfbuf_use[buffer_index]++;

        INCPTR(fill_ptr);

        frame_count++;
        vf_notify_receiver(PROVIDER_NAME,VFRAME_EVENT_PROVIDER_VFRAME_READY,NULL);
        WRITE_MPEG_REG(FROM_AMRISC, 0);
    }

    WRITE_MPEG_REG(ASSIST_MBOX1_CLR_REG, 1);

#ifdef HANDLE_REAL_IRQ
    return IRQ_HANDLED;
#else
    return;
#endif
}

static vframe_t *vreal_vf_peek(void* op_arg)
{
    if (get_ptr == fill_ptr) {
        return NULL;
    }

    return &vfpool[get_ptr];
}

static vframe_t *vreal_vf_get(void* op_arg)
{
    vframe_t *vf;

    if (get_ptr == fill_ptr) {
        return NULL;
    }

    vf = &vfpool[get_ptr];

    INCPTR(get_ptr);

    return vf;
}

static void vreal_vf_put(vframe_t *vf, void* op_arg)
{
    INCPTR(putting_ptr);
}

static int vreal_event_cb(int type, void *data, void *private_data)
{
    if(type & VFRAME_EVENT_RECEIVER_RESET){
        unsigned long flags;
        amvdec_stop();
#ifndef CONFIG_POST_PROCESS_MANAGER
        vf_light_unreg_provider(&vreal_vf_prov);
#endif
        spin_lock_irqsave(&lock, flags);
        vreal_local_init();
        vreal_prot_init();
        spin_unlock_irqrestore(&lock, flags); 
#ifndef CONFIG_POST_PROCESS_MANAGER
        vf_reg_provider(&vreal_vf_prov);
#endif              
        amvdec_start();
    }
    return 0;        
}

static int  vreal_vf_states(vframe_states_t *states, void* op_arg)
{
    unsigned long flags;
    int i;
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

static void vreal_put_timer_func(unsigned long arg)
{
    struct timer_list *timer = (struct timer_list *)arg;
    //unsigned int status;

#ifndef HANDLE_REAL_IRQ
    vreal_isr();
#endif

    if (putting_ptr != put_ptr) {
        u32 index = vfpool_idx[put_ptr];

        if (--vfbuf_use[index] == 0) {
#if 0
            //WRITE_MPEG_REG(VIDEO_PTS, realdec.buffer_timestamp[disbuf->buffer_index]);
            /* this frame is not used, need return this buffer back to decoder side */
            /* todo: fix this polling, something on amrisc side */
            while (READ_MPEG_REG(TO_AMRISC)) {
                status = READ_MPEG_REG(STATUS_AMRISC);
                if (status & (PARSER_ERROR_WRONG_PACKAGE_SIZE | PARSER_ERROR_WRONG_HEAD_VER | DECODER_ERROR_VLC_DECODE_TBL)) {
                    break;
                }
            }
            WRITE_MPEG_REG(TO_AMRISC, ~(1 << index));
#endif
            real_recycle_q[real_recycle_wr++] = ~(1 << index);
            real_recycle_wr &= REAL_RECYCLE_Q_MASK;
        }

        INCPTR(put_ptr);
    }

    if ((real_recycle_rd != real_recycle_wr) && !READ_MPEG_REG(TO_AMRISC)) {
        WRITE_MPEG_REG(TO_AMRISC, real_recycle_q[real_recycle_rd++]);
        real_recycle_rd &= REAL_RECYCLE_Q_MASK;
    }

    timer->expires = jiffies + PUT_INTERVAL;

    add_timer(timer);
}

int vreal_dec_status(struct vdec_status *vstatus)
{
    vstatus->width = vreal_amstream_dec_info.width;
    vstatus->height = vreal_amstream_dec_info.height;
    if (0 != vreal_amstream_dec_info.rate) {
        vstatus->fps = 96000 / vreal_amstream_dec_info.rate;
    } else {
        vstatus->fps = 96000;
    }
    vstatus->error_count = real_err_count;
    vstatus->status = (READ_MPEG_REG(STATUS_AMRISC) << 16) | stat;
    return 0;
}


/****************************************/
static void vreal_canvas_init(void)
{
    int i;
    u32 canvas_width, canvas_height;
    u32 decbuf_size, decbuf_y_size, decbuf_uv_size;
    u32 disp_addr = 0xffffffff;

    if (buf_size <= 0x00400000) {
        /* SD only */
        canvas_width = 768;
        canvas_height = 576;
        decbuf_y_size = 0x80000;
        decbuf_uv_size = 0x20000;
        decbuf_size = 0x100000;
    } else {
        /* HD & SD */
        canvas_width = 1920;
        canvas_height = 1088;
        decbuf_y_size = 0x200000;
        decbuf_uv_size = 0x80000;
        decbuf_size = 0x300000;
    }

    if (READ_MPEG_REG(VPP_MISC) & VPP_VD1_POSTBLEND) {
        canvas_t cur_canvas;

        canvas_read((READ_MPEG_REG(VD1_IF0_CANVAS0) & 0xff), &cur_canvas);
        disp_addr = (cur_canvas.addr + 7) >> 3;
    }

    for (i = 0; i < 4; i++) {
        if (((buf_start + i * decbuf_size + 7) >> 3) == disp_addr) {
            canvas_config(3 * i + 0,
                          buf_start + 4 * decbuf_size,
                          canvas_width, canvas_height,
                          CANVAS_ADDR_NOWRAP, CANVAS_BLKMODE_32X32);
            canvas_config(3 * i + 1,
                          buf_start + 4 * decbuf_size + decbuf_y_size,
                          canvas_width / 2, canvas_height / 2,
                          CANVAS_ADDR_NOWRAP, CANVAS_BLKMODE_32X32);
            canvas_config(3 * i + 2,
                          buf_start + 4 * decbuf_size + decbuf_y_size + decbuf_uv_size,
                          canvas_width / 2, canvas_height / 2,
                          CANVAS_ADDR_NOWRAP, CANVAS_BLKMODE_32X32);
        } else {
            canvas_config(3 * i + 0,
                          buf_start + i * decbuf_size,
                          canvas_width, canvas_height,
                          CANVAS_ADDR_NOWRAP, CANVAS_BLKMODE_32X32);
            canvas_config(3 * i + 1,
                          buf_start + i * decbuf_size + decbuf_y_size,
                          canvas_width / 2, canvas_height / 2,
                          CANVAS_ADDR_NOWRAP, CANVAS_BLKMODE_32X32);
            canvas_config(3 * i + 2,
                          buf_start + i * decbuf_size + decbuf_y_size + decbuf_uv_size,
                          canvas_width / 2, canvas_height / 2,
                          CANVAS_ADDR_NOWRAP, CANVAS_BLKMODE_32X32);
        }
    }
}

static void vreal_prot_init(void)
{
    WRITE_MPEG_REG(RESET0_REGISTER, RESET_IQIDCT | RESET_MC);

    vreal_canvas_init();

    /* index v << 16 | u << 8 | y */
    WRITE_MPEG_REG(AV_SCRATCH_0, 0x020100);
    WRITE_MPEG_REG(AV_SCRATCH_1, 0x050403);
    WRITE_MPEG_REG(AV_SCRATCH_2, 0x080706);
    WRITE_MPEG_REG(AV_SCRATCH_3, 0x0b0a09);

    /* notify ucode the buffer offset */
    WRITE_MPEG_REG(AV_SCRATCH_F, buf_offset >> 12);

    WRITE_MPEG_REG(RESET2_REGISTER, RESET_PIC_DC | RESET_DBLK);

    /* disable PSCALE for hardware sharing */
    WRITE_MPEG_REG(PSCALE_CTRL, 0);

    WRITE_MPEG_REG(FROM_AMRISC, 0);
    WRITE_MPEG_REG(TO_AMRISC, 0);
    WRITE_MPEG_REG(STATUS_AMRISC, 0);

    WRITE_MPEG_REG(RV_PIC_INFO, 0);
    WRITE_MPEG_REG(VPTS_TR, 0);
    WRITE_MPEG_REG(VDTS, 0);
    WRITE_MPEG_REG(SKIP_B_AMRISC, 0);

    WRITE_MPEG_REG(MDEC_WIDTH, (frame_width + 15) & 0xfff0);
    WRITE_MPEG_REG(MDEC_HEIGHT, (frame_height + 15) & 0xfff0);

    /* clear mailbox interrupt */
    WRITE_MPEG_REG(ASSIST_MBOX1_CLR_REG, 1);

    /* enable mailbox interrupt */
    WRITE_MPEG_REG(ASSIST_MBOX1_MASK, 1);
}

static void vreal_local_init(void)
{
    int i;

    //vreal_ratio = vreal_amstream_dec_info.ratio;
    vreal_ratio = 0x100;

    fill_ptr = get_ptr = put_ptr = putting_ptr = 0;

    frame_prog = 0;

    frame_width = vreal_amstream_dec_info.width;
    frame_height = vreal_amstream_dec_info.height;
    frame_dur = vreal_amstream_dec_info.rate;

    for (i = 0; i < 4; i++) {
        vfbuf_use[i] = 0;
    }

    decoder_state = 1;
    hold = 0;
    last_tr = -1;
    wait_key_frame = 1;
    frame_count = 0;
    current_vdts = 0;
    real_err_count = 0;

    real_recycle_rd = 0;
    real_recycle_wr = 0;

    pic_sz_tbl_map = 0;
}

static void load_block_data(unsigned int dest, unsigned int count)
{
    unsigned short *pdest = (unsigned short *)dest;
    unsigned short src_tbl[12];
    unsigned int i;

    src_tbl[0] = RPR_size[vreal_amstream_dec_info.extra + 1];
    copy_from_user((void *)&src_tbl[1], vreal_amstream_dec_info.param, 2 << src_tbl[0]);

#if 0
    for (i = 0; i < 12; i++) {
        printk("src_tbl[%d]: 0x%x\n", i, src_tbl[i]);
    }
#endif

    for (i = 0; i < count / 4; i++) {
        pdest[i * 4] = src_tbl[i * 4 + 3];
        pdest[i * 4 + 1] = src_tbl[i * 4 + 2];
        pdest[i * 4 + 2] = src_tbl[i * 4 + 1];
        pdest[i * 4 + 3] = src_tbl[i * 4];
    }

    pic_sz_tbl_map = dma_map_single(NULL, &pic_sz_tbl,
                                    sizeof(pic_sz_tbl), DMA_TO_DEVICE);

    return;
}

s32 vreal_init(void)
{
    int r;

    printk("vreal_init\n");
    init_timer(&recycle_timer);

    stat |= STAT_TIMER_INIT;

    amvdec_enable();

    vreal_local_init();

    r = rmparser_init();
    if (r) {
        amvdec_disable();

        printk("rm parser init failed\n");
        return r;
    }

    if (vreal_amstream_dec_info.format == VIDEO_DEC_FORMAT_REAL_8) {
        load_block_data((unsigned int)pic_sz_tbl, 12);

        //  TODO: need to load the table into lmem
        WRITE_MPEG_REG(LMEM_DMA_ADR, (unsigned)pic_sz_tbl_map);
        WRITE_MPEG_REG(LMEM_DMA_COUNT, 10);
        WRITE_MPEG_REG(LMEM_DMA_CTRL, 0xc178 | (3 << 11));
        while (READ_MPEG_REG(LMEM_DMA_CTRL) & 0x8000) {};

        printk("load VIDEO_DEC_FORMAT_REAL_8\n");
        if (amvdec_loadmc(vreal_mc_8) < 0) {
            amvdec_disable();

            printk("failed\n");
            return -EBUSY;
        }
    } else if (vreal_amstream_dec_info.format == VIDEO_DEC_FORMAT_REAL_9) {
        printk("load VIDEO_DEC_FORMAT_REAL_9\n");
        if (amvdec_loadmc(vreal_mc_9) < 0) {
            amvdec_disable();

            printk("failed\n");
            return -EBUSY;
        }
    } else {
        printk("unsurpported real format\n");
    }

    stat |= STAT_MC_LOAD;

    /* enable AMRISC side protocol */
    vreal_prot_init();

#ifdef HANDLE_REAL_IRQ
    if (request_irq(INT_MAILBOX_1A, vreal_isr,
                    IRQF_SHARED, "vreal-irq", (void *)vreal_dec_id)) {
        amvdec_disable();

        printk("vreal irq register error.\n");
        return -ENOENT;
    }
#endif

    stat |= STAT_ISR_REG;
 #ifdef CONFIG_POST_PROCESS_MANAGER
    vf_provider_init(&vreal_vf_prov, PROVIDER_NAME, &vreal_vf_provider, NULL);
    vf_reg_provider(&vreal_vf_prov);
    vf_notify_receiver(PROVIDER_NAME,VFRAME_EVENT_PROVIDER_START,NULL);
 #else 
    vf_provider_init(&vreal_vf_prov, PROVIDER_NAME, &vreal_vf_provider, NULL);
    vf_reg_provider(&vreal_vf_prov);
 #endif 

    stat |= STAT_VF_HOOK;

    recycle_timer.data = (ulong) & recycle_timer;
    recycle_timer.function = vreal_put_timer_func;
    recycle_timer.expires = jiffies + PUT_INTERVAL;

    add_timer(&recycle_timer);

    stat |= STAT_TIMER_ARM;

    amvdec_start();

    stat |= STAT_VDEC_RUN;

    set_vdec_func(&vreal_dec_status);

    printk("vreal init finished\n");

    return 0;
}

static int amvdec_real_probe(struct platform_device *pdev)
{
    struct resource *mem;

    if (!(mem = platform_get_resource(pdev, IORESOURCE_MEM, 0))) {
        printk("amvdec_real memory resource undefined.\n");
        return -EFAULT;
    }

    buf_start = mem->start;
    buf_size = mem->end - mem->start + 1;
    buf_offset = buf_start - RM_DEF_BUFFER_ADDR;

    memcpy(&vreal_amstream_dec_info, (void *)mem[1].start, sizeof(vreal_amstream_dec_info));

    if (vreal_init() < 0) {
        printk("amvdec_real init failed.\n");

        return -ENODEV;
    }

    return 0;
}

static int amvdec_real_remove(struct platform_device *pdev)
{
    if (stat & STAT_VDEC_RUN) {
        amvdec_stop();
        stat &= ~STAT_VDEC_RUN;
    }

    if (stat & STAT_ISR_REG) {
        free_irq(INT_MAILBOX_1A, (void *)vreal_dec_id);
        stat &= ~STAT_ISR_REG;
    }

    if (stat & STAT_TIMER_ARM) {
        del_timer_sync(&recycle_timer);
        stat &= ~STAT_TIMER_ARM;
    }

    if (stat & STAT_VF_HOOK) {
        ulong flags;
        spin_lock_irqsave(&lock, flags);
        fill_ptr = get_ptr = put_ptr = putting_ptr = 0;
        spin_unlock_irqrestore(&lock, flags);
        vf_unreg_provider(&vreal_vf_prov);
        stat &= ~STAT_VF_HOOK;
    }

    if (pic_sz_tbl_map != 0) {
        dma_unmap_single(NULL, pic_sz_tbl_map, sizeof(pic_sz_tbl), DMA_TO_DEVICE);
    }

    amvdec_disable();

    printk("frame duration %d, frames %d\n", frame_dur, frame_count);
    return 0;
}

/****************************************/

static struct platform_driver amvdec_real_driver = {
    .probe  = amvdec_real_probe,
    .remove = amvdec_real_remove,
#ifdef CONFIG_PM
    .suspend = amvdec_suspend,
    .resume  = amvdec_resume,
#endif
    .driver = {
        .name = DRIVER_NAME,
    }
};

static struct codec_profile_t amvdec_real_profile = {
	.name = "real",
	.profile = "rmvb,"
};


static int __init amvdec_real_driver_init_module(void)
{
    printk("amvdec_real module init\n");

    if (platform_driver_register(&amvdec_real_driver)) {
        printk("failed to register amvdec_real driver\n");
        return -ENODEV;
    }
	vcodec_profile_register(&amvdec_real_profile);
    return 0;
}

static void __exit amvdec_real_driver_remove_module(void)
{
    printk("amvdec_real module remove.\n");

    platform_driver_unregister(&amvdec_real_driver);
}

/****************************************/

module_param(stat, uint, 0664);
MODULE_PARM_DESC(stat, "\n amvdec_real stat \n");

module_init(amvdec_real_driver_init_module);
module_exit(amvdec_real_driver_remove_module);

MODULE_DESCRIPTION("AMLOGIC REAL Video Decoder Driver");
MODULE_LICENSE("GPL");
