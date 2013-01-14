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
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/platform_device.h>
#include <linux/amports/amstream.h>
#include <linux/amports/ptsserv.h>
#include <linux/amports/canvas.h>
#include <linux/amports/vframe.h>
#include <linux/amports/vframe_provider.h>
#include <linux/amports/vframe_receiver.h>
#include <linux/workqueue.h>
#include <linux/dma-mapping.h>
#include <asm/atomic.h>

#include <mach/am_regs.h>

#include "amvdec.h"
#include "vh264mvc_mc.h"

#define DRIVER_NAME "amvdec_h264mvc"
#define MODULE_NAME "amvdec_h264mvc"

#define HANDLE_h264mvc_IRQ

#define DEBUG_PTS
#define DEBUG_SKIP

#define PUT_INTERVAL        (HZ/100)

#define STAT_TIMER_INIT     0x01
#define STAT_MC_LOAD        0x02
#define STAT_ISR_REG        0x04
#define STAT_VF_HOOK        0x08
#define STAT_TIMER_ARM      0x10
#define STAT_VDEC_RUN       0x20

#define DROPPING_THREAD_HOLD    4
#define DROPPING_FIRST_WAIT     6

static vframe_t *vh264mvc_vf_peek(void*);
static vframe_t *vh264mvc_vf_get(void*);
static void vh264mvc_vf_put(vframe_t *, void*);
static int vh264mvc_event_cb(int type, void *data, void *private_data);

static void vh264mvc_prot_init(void);
static void vh264mvc_local_init(void);
static void vh264mvc_put_timer_func(unsigned long arg);

static const char vh264mvc_dec_id[] = "vh264mvc-dev";

#define PROVIDER_NAME   "decoder.h264mvc"

static const struct vframe_operations_s vh264mvc_vf_provider = {
    .peek = vh264mvc_vf_peek,
    .get = vh264mvc_vf_get,
    .put = vh264mvc_vf_put,
    .event_cb = vh264mvc_event_cb,
    .vf_states=NULL,
};
static struct vframe_provider_s vh264mvc_vf_prov;

static u32 frame_width, frame_height, frame_dur;
static struct timer_list recycle_timer;
static u32 stat;
static u32 pts_outside = 0;
static u32 sync_outside = 0;
static u32 vh264mvc_ratio;
static u32 h264mvc_ar;
static u32 no_dropping_cnt;

#ifdef DEBUG_SKIP
static unsigned long view_total, view_dropped;
#endif

#ifdef DEBUG_PTS
static unsigned long pts_missed, pts_hit;
#endif

static atomic_t vh264mvc_active = ATOMIC_INIT(0);
static struct work_struct error_wd_work;

static struct dec_sysinfo vh264mvc_amstream_dec_info;
extern u32 trickmode_i;

static spinlock_t lock = SPIN_LOCK_UNLOCKED;

static int vh264mvc_stop(void);
static s32 vh264mvc_init(void);

/***************************
*   new
***************************/

// bit[3:0] command :
//           0 - command finished
//               (DATA0 - {level_idc_mmco, max_reference_frame_num, width, height}
//           1 - alloc view_0 display_buffer and reference_data_area
//           2 - alloc view_1 display_buffer and reference_data_area
#define MAILBOX_COMMAND         AV_SCRATCH_0
#define MAILBOX_DATA_0          AV_SCRATCH_1
#define MAILBOX_DATA_1          AV_SCRATCH_2
#define MAILBOX_DATA_2          AV_SCRATCH_3
#define CANVAS_START            AV_SCRATCH_6
#define BUFFER_RECYCLE          AV_SCRATCH_7
#define DROP_CONTROL            AV_SCRATCH_8
#define PICTURE_COUNT           AV_SCRATCH_9
#define DECODE_STATUS           AV_SCRATCH_A
#define SPS_STATUS              AV_SCRATCH_B
#define PPS_STATUS              AV_SCRATCH_C
#define SIM_RESERV_D            AV_SCRATCH_D
#define WORKSPACE_START         AV_SCRATCH_E
#define SIM_RESERV_F            AV_SCRATCH_F
#define DECODE_ERROR_CNT        AV_SCRATCH_G
#define CURRENT_UCODE           AV_SCRATCH_H
#define CURRENT_SPS_PPS         AV_SCRATCH_I // bit[15:9]-SPS, bit[8:0]-PPS
#define DECODE_SKIP_PICTURE     AV_SCRATCH_J
#define SIM_RESERV_K            AV_SCRATCH_K
#define SIM_RESERV_L            AV_SCRATCH_L
#define REF_START_VIEW_0        AV_SCRATCH_M
#define REF_START_VIEW_1        AV_SCRATCH_N

/********************************************
 *  Mailbox command
 ********************************************/
#define CMD_FINISHED               0
#define CMD_ALLOC_VIEW_0           1
#define CMD_ALLOC_VIEW_1           2 
#define CMD_FRAME_DISPLAY          3 

#define CANVAS_INDEX_START         120

unsigned  DECODE_BUFFER_START=0x00200000;
unsigned DECODE_BUFFER_END=0x05000000;

#define DECODE_BUFFER_NUM_MAX    16
#define DISPLAY_BUFFER_NUM         4

static unsigned int ANC_CANVAS_ADDR;
static unsigned int index;
static unsigned int dpb_start_addr[3]; 
static unsigned int ref_start_addr[2]; 
static unsigned int max_dec_frame_buffering[2];
static unsigned int total_dec_frame_buffering[2];
static unsigned int level_idc, max_reference_frame_num, mb_width, mb_height;
static unsigned int dpb_size, ref_size;

static int display_buff_id;
static int display_view_id;
static int display_POC;
static int stream_offset;

#define video_domain_addr(adr) (adr&0x7fffffff)
static unsigned work_space_adr;
static unsigned work_space_size = 0xa0000;

typedef struct {
    unsigned int y_addr;
    unsigned int u_addr;
    unsigned int v_addr;

    int y_canvas_index;
    int u_canvas_index;
    int v_canvas_index;
} buffer_spec_t;
static buffer_spec_t buffer_spec0[DECODE_BUFFER_NUM_MAX+DISPLAY_BUFFER_NUM];
static buffer_spec_t buffer_spec1[DECODE_BUFFER_NUM_MAX+DISPLAY_BUFFER_NUM];


/* 
    dbg_mode:
    bit 0: 1, print debug information
    bit 4: 1, recycle buffer without displaying; 
    bit 5: 1, buffer single frame step , set dbg_cmd to 1 to step
    
*/
static int dbg_mode = 0; 
static int dbg_cmd = 0;
static int view_mode = 3; /* 0, left; 1 ,right ; 2, left<->right 3, right<->left */
static int drop_rate = 2;
/**/

#define MVC_BUF_NUM     (DECODE_BUFFER_NUM_MAX+DISPLAY_BUFFER_NUM)
static struct list_head free_list_head = LIST_HEAD_INIT(free_list_head); 
static struct list_head ready_list_head = LIST_HEAD_INIT(ready_list_head); 
static struct list_head recycle_list_head = LIST_HEAD_INIT(recycle_list_head); 
typedef struct mvc_buf_s{
    struct list_head list;
    vframe_t vframe;
    int display_POC;
    int view0_buff_id; 
    int view1_buff_id;
    int view0_drop;
    int view1_drop;
    int stream_offset;
    unsigned pts;
}mvc_buf_t;
static mvc_buf_t mvc_buf_local[MVC_BUF_NUM];

#define spec2canvas(x)  \
    (((x)->v_canvas_index << 16) | \
     ((x)->u_canvas_index << 8)  | \
     ((x)->y_canvas_index << 0))

#define to_mvcbuf(vf)	\
	container_of(vf, struct mvc_buf_s, vframe)

static int vf_buf_init_flag = 0;

static void recycle_vframe(void);

static void init_vf_buf(void)
{
    int i;
    mvc_buf_t* mvc_buf;
    for(i = 0; i< MVC_BUF_NUM; i++){
        mvc_buf = &mvc_buf_local[i];
        memset(mvc_buf, 0, sizeof(mvc_buf_t));
        mvc_buf->view0_buff_id = -1;
        mvc_buf->view1_buff_id = -1;
        mvc_buf->display_POC = -1;
        list_add_tail(&(mvc_buf->list), &free_list_head);
    }
    vf_buf_init_flag = 1;
}    

static void uninit_vf_buf(void)
{
    mvc_buf_t *p = NULL, *ptmp;
    vf_buf_init_flag = 0;
    list_for_each_entry_safe(p, ptmp, &free_list_head, list) {
        list_del(&p->list);
    }
    list_for_each_entry_safe(p, ptmp, &ready_list_head, list) {
        list_del(&p->list);
    }
    list_for_each_entry_safe(p, ptmp, &recycle_list_head, list) {
        list_del(&p->list);
    }
}    

static void set_frame_info(vframe_t *vf)
{
    unsigned int ar = 0;

    vf->width = frame_width;
    vf->height = frame_height;
    vf->duration = frame_dur;
    vf->duration_pulldown = 0;

    if (vh264mvc_ratio == 0) {
        vf->ratio_control |= (0x90 << DISP_RATIO_ASPECT_RATIO_BIT); // always stretch to 16:9
    } else {
        //h264mvc_ar = ((float)frame_height/frame_width)*customer_ratio;
        ar = min(h264mvc_ar, (u32)DISP_RATIO_ASPECT_RATIO_MAX);

        vf->ratio_control = (ar << DISP_RATIO_ASPECT_RATIO_BIT);
    }

    return;
}

static vframe_t *vh264mvc_vf_peek(void* op_arg)
{
    int ready_cnt = 0;
    vframe_t *vf, *vf_next;
    mvc_buf_t *p = NULL, *ptmp;
    if(vf_buf_init_flag == 0)
        return NULL;
    if((dbg_mode&0x30)==0x20){
        if(dbg_cmd!=1){
            return NULL;    
        }    
    }
        
    ready_cnt = 0;
    list_for_each_entry_safe(p, ptmp, &ready_list_head, list) {
       if((p->view0_buff_id >= 0 && p->view1_buff_id >= 0) &&
          ((p->view0_drop == 0) && (p->view1_drop) == 0))
          ready_cnt++;
    }

    if((no_dropping_cnt >= DROPPING_FIRST_WAIT) && (ready_cnt < DROPPING_THREAD_HOLD)) {
        WRITE_MPEG_REG(DROP_CONTROL, (1 << 31) | (drop_rate));
    } else {
        WRITE_MPEG_REG(DROP_CONTROL, 0);
    }

    if (ready_cnt == 0)
        return NULL;

    /* consolidate ready queue for dropped frames */
    list_for_each_entry_safe(p, ptmp, &ready_list_head, list) {
        if((p->view0_buff_id >= 0 && p->view1_buff_id >= 0)){
            if (((p->view0_drop) || (p->view1_drop)) &&
                (&ptmp->list != &ready_list_head)) {
                vf = &(p->vframe);
                vf_next = &(ptmp->vframe);
                if (vf_next->pts == 0) {
                    if (p->vframe.pts == 0) {
                        vf_next->duration += frame_dur;
                    } else {
                        vf_next->pts = p->vframe.pts + p->vframe.duration;
                    }
                }
                list_move_tail(&p->list, &recycle_list_head);
            }
        }
    }
    
    vf = NULL;

    list_for_each_entry_safe(p, ptmp, &ready_list_head, list) {
        if((p->view0_buff_id >= 0 && p->view1_buff_id >= 0)){
            /* if there is only a single dropped frame in the ready queue, return NULL */
            if ((p->view0_drop) || (p->view1_drop))
                break;

            vf = &(p->vframe);

            if(view_mode==0 || view_mode==1){
                vf->type = VIDTYPE_PROGRESSIVE | VIDTYPE_VIU_FIELD;
                vf->canvas0Addr = vf->canvas1Addr = (view_mode==0)?spec2canvas(&buffer_spec0[p->view0_buff_id]):
                        spec2canvas(&buffer_spec1[p->view1_buff_id]);
            }
            else{
                vf->type = VIDTYPE_PROGRESSIVE | VIDTYPE_MVC;
                if(view_mode==2){
                    vf->canvas0Addr = spec2canvas(&buffer_spec1[p->view1_buff_id]);
                    vf->canvas1Addr = spec2canvas(&buffer_spec0[p->view0_buff_id]);
                }
                else{
                    vf->canvas0Addr = spec2canvas(&buffer_spec0[p->view0_buff_id]);
                    vf->canvas1Addr = spec2canvas(&buffer_spec1[p->view1_buff_id]);
                }
            }
            break;
        }
    }
    if(vf){
        if(frame_width==0){
            frame_width = vh264mvc_amstream_dec_info.width;
        }
        if(frame_height==0){
            frame_height = vh264mvc_amstream_dec_info.height;
        }    

        vf->width = frame_width;
        vf->height = frame_height;
    }

    return vf;
}

static vframe_t *vh264mvc_vf_get(void* op_arg)
{
    mvc_buf_t * mvc_buf;
    vframe_t* vf = vh264mvc_vf_peek(op_arg);
    if(vf){
        mvc_buf = to_mvcbuf(vf);
        list_del(&(mvc_buf->list));

        /* use newest frame_width, frame_height */ 
        if((dbg_mode&0x30)==0x20){
            if(dbg_cmd==1){
                dbg_cmd = 0;
            }    
        }
        if(dbg_mode&0x1)
            printk("vh264mvc_vf_get %d %d %d poc %d view0_buf %d view1_buf %d\n", 
                    vf->width, vf->height, vf->duration, mvc_buf->display_POC, mvc_buf->view0_buff_id, mvc_buf->view1_buff_id);
        
        if (vf->pts)
            pts_hit++;
        else
            pts_missed++;
            
        if (no_dropping_cnt < DROPPING_FIRST_WAIT)
            no_dropping_cnt++;
    }
    return vf;
}

static void vh264mvc_vf_put(vframe_t *vf, void* op_arg)
{
    mvc_buf_t * mvc_buf;
    if(vf_buf_init_flag == 0){
        return;
    }
    mvc_buf = to_mvcbuf(vf);
    list_add_tail(&mvc_buf->list, &recycle_list_head);
    recycle_vframe();
}

static int vh264mvc_event_cb(int type, void *data, void *private_data)
{
    if(type & VFRAME_EVENT_RECEIVER_RESET){
        unsigned long flags;
        amvdec_stop();
#ifndef CONFIG_POST_PROCESS_MANAGER
        vf_light_unreg_provider(&vh264mvc_vf_prov);
#endif
        spin_lock_irqsave(&lock, flags);
        vh264mvc_local_init();
        vh264mvc_prot_init();
        spin_unlock_irqrestore(&lock, flags); 
#ifndef CONFIG_POST_PROCESS_MANAGER
        vf_reg_provider(&vh264mvc_vf_prov);
#endif              
        amvdec_start();
    }
    return 0;        
}

/**/
long init_canvas( int start_addr, long dpb_size, int dpb_number, int mb_width, int mb_height, buffer_spec_t* buffer_spec) { 

  int dpb_addr, addr;
  int i;
  int mb_total;

  //cav_con canvas;

    dpb_addr = start_addr;

    mb_total = mb_width * mb_height;

    for ( i = 0 ; i < dpb_number ; i++ )
    {
        WRITE_MPEG_REG(ANC_CANVAS_ADDR, index | ((index+1)<<8) | ((index+2)<<16));
        ANC_CANVAS_ADDR++;

        addr = dpb_addr;
        buffer_spec[i].y_addr = addr;
        buffer_spec[i].y_canvas_index = index;
        canvas_config(index,
                  addr,
                  mb_width << 4,
                  mb_height << 4,
                  CANVAS_ADDR_NOWRAP,
                  CANVAS_BLKMODE_32X32);
        
        addr += mb_total << 8;
        index++;
        buffer_spec[i].u_addr = addr;
        buffer_spec[i].u_canvas_index = index;
        canvas_config(index,
                  addr,
                  mb_width << 3,
                  mb_height << 3,
                  CANVAS_ADDR_NOWRAP,
                  CANVAS_BLKMODE_32X32);

        addr += mb_total << 6;
        index++;
        buffer_spec[i].v_addr = addr;
        buffer_spec[i].v_canvas_index = index;
        canvas_config(index,
                  addr,
                  mb_width << 3,
                  mb_height << 3,
                  CANVAS_ADDR_NOWRAP,
                  CANVAS_BLKMODE_32X32);


        addr += mb_total << 6;
        index++;

        dpb_addr = dpb_addr + dpb_size;
        if(dpb_addr >= DECODE_BUFFER_END) return -1;
    }

    return dpb_addr;
}


static int get_max_dec_frame_buf_size(int level_idc, int max_reference_frame_num, int mb_width, int mb_height)
{
  int pic_size = mb_width * mb_height * 384;

  int size = 0;

  switch (level_idc)
  {
  case 9:
    size = 152064;
    break;
  case 10:
    size = 152064;
    break;
  case 11:
    size = 345600;
    break;
  case 12:
    size = 912384;
    break;
  case 13:
    size = 912384;
    break;
  case 20:
    size = 912384;
    break;
  case 21:
    size = 1824768;
    break;
  case 22:
    size = 3110400;
    break;
  case 30:
    size = 3110400;
    break;
  case 31:
    size = 6912000;
    break;
  case 32:
    size = 7864320;
    break;
  case 40:
    size = 12582912;
    break;
  case 41:
    size = 12582912;
    break;
  case 42:
    size = 13369344;
    break;
  case 50:
    size = 42393600;
    break;
  case 51:
    size = 70778880;
    break;
  default:
    break;
  }

  size /= pic_size;
  size = size + 1; // For MVC need onr more buffer   
  if(max_reference_frame_num > size) size = max_reference_frame_num;
  if(size > DECODE_BUFFER_NUM_MAX) size = DECODE_BUFFER_NUM_MAX;

  return size;
}

#ifdef DEBUG_PTS
static void dump_ready_list(void)
{
    mvc_buf_t *p = NULL, *ptmp;

    list_for_each_entry_safe(p, ptmp, &ready_list_head, list) {
        printk("view0_buff_id = %d, view1_buff_id = %d, stream_offset = %d, POC = %d\n",
                p->view0_buff_id,
                p->view1_buff_id,
                p->stream_offset,
                p->display_POC);
    }    
}
#endif

#ifdef HANDLE_h264mvc_IRQ
static irqreturn_t vh264mvc_isr(int irq, void *dev_id)
#else
static void vh264mvc_isr(void)
#endif
{
        int drop_status;
        int ret = READ_MPEG_REG(MAILBOX_COMMAND);
        //printk("vh264mvc_isr, cmd =%x\n", ret);
        switch(ret & 0xff) {
          case CMD_ALLOC_VIEW_0:
            if(dbg_mode&0x1)
                printk("Start H264 display buffer allocation for view 0\n");
            if((dpb_start_addr[0] != -1) | (dpb_start_addr[1] != -1)){
              dpb_start_addr[0] = -1; 
              dpb_start_addr[1] = -1; 
            }
            dpb_start_addr[0] = DECODE_BUFFER_START; 
            ret = READ_MPEG_REG(MAILBOX_DATA_0);
            level_idc = (ret >> 24) & 0xff;
            max_reference_frame_num = (ret >> 16) & 0xff;
            mb_width = (ret >> 8) & 0xff;
            mb_height = (ret >> 0) & 0xff;
            max_dec_frame_buffering[0] = get_max_dec_frame_buf_size(level_idc, max_reference_frame_num, mb_width, mb_height);

            total_dec_frame_buffering[0] = max_dec_frame_buffering[0] + DISPLAY_BUFFER_NUM;

            mb_width = (mb_width+3) & 0xfffffffc;
            mb_height = (mb_height+3) & 0xfffffffc;

            dpb_size = mb_width * mb_height * 384;
            ref_size = mb_width * mb_height * 96;

            if(dbg_mode&0x1){
                printk("dpb_size: 0x%x\n", dpb_size);
                printk("ref_size: 0x%x\n", ref_size);
                printk("total_dec_frame_buffering[0] : 0x%x\n", total_dec_frame_buffering[0]);
                printk("max_reference_frame_num: 0x%x\n", max_reference_frame_num);
            }
            ref_start_addr[0] = dpb_start_addr[0] + 
                                (dpb_size * total_dec_frame_buffering[0]); 
            dpb_start_addr[1] = ref_start_addr[0] +
                                (ref_size * (max_reference_frame_num+1));

            if(dbg_mode&0x1){
                printk("dpb_start_addr[0]: 0x%x\n", dpb_start_addr[0]);
                printk("ref_start_addr[0]: 0x%x\n", ref_start_addr[0]);
                printk("dpb_start_addr[1]: 0x%x\n", dpb_start_addr[1]);
            }
            if(dpb_start_addr[1] >= DECODE_BUFFER_END)
            {
               printk(" No enough memory for alloc view 0\n");
               goto exit;
            }

            index = CANVAS_INDEX_START;
            ANC_CANVAS_ADDR = ANC0_CANVAS_ADDR;

            ret = init_canvas(dpb_start_addr[0], dpb_size, total_dec_frame_buffering[0], mb_width, mb_height, buffer_spec0); 

            if(ret == -1) {
               printk(" Un-expected memory alloc problem\n");
               goto exit;
            }

            WRITE_MPEG_REG(REF_START_VIEW_0, video_domain_addr(ref_start_addr[0]));
            WRITE_MPEG_REG(MAILBOX_DATA_0, 
                (max_dec_frame_buffering[0] << 8) |
                (total_dec_frame_buffering[0] << 0)
              );
            WRITE_MPEG_REG(MAILBOX_DATA_1, ref_size); 
            WRITE_MPEG_REG(MAILBOX_COMMAND, CMD_FINISHED);

            if(dbg_mode&0x1){
                printk("End H264 display buffer allocation for view 0\n");
            }
            if(frame_width == 0){
                frame_width = mb_width<<4;    
            }
            if(frame_height == 0){
                frame_height = mb_height<<4;  
                if(frame_height == 1088) frame_height = 1080;
            }
            break;
          case CMD_ALLOC_VIEW_1:
            if(dbg_mode&0x1){
                printk("Start H264 display buffer allocation for view 1\n");
            }
            if((dpb_start_addr[0] == -1) | (dpb_start_addr[1] == -1)){
              printk("Error : allocation view 1 before view 0 !!!\n");
              break;
            }
            ret = READ_MPEG_REG(MAILBOX_DATA_0);
            level_idc = (ret >> 24) & 0xff;
            max_reference_frame_num = (ret >> 16) & 0xff;
            mb_width = (ret >> 8) & 0xff;
            mb_height = (ret >> 0) & 0xff;
            max_dec_frame_buffering[1] = get_max_dec_frame_buf_size(level_idc, max_reference_frame_num, mb_width, mb_height);
            if(max_dec_frame_buffering[1] != max_dec_frame_buffering[0]){
              printk(" Warning : view0/1 max_dec_frame_buffering different : 0x%x/0x%x, Use View0\n", 
                max_dec_frame_buffering[0], max_dec_frame_buffering[1]);
              max_dec_frame_buffering[1] = max_dec_frame_buffering[0]; 
            }

            total_dec_frame_buffering[1] = max_dec_frame_buffering[1] + DISPLAY_BUFFER_NUM;

            mb_width = (mb_width+3) & 0xfffffffc;
            mb_height = (mb_height+3) & 0xfffffffc;

            dpb_size = mb_width * mb_height * 384;
            ref_size = mb_width * mb_height * 96;

            if(dbg_mode&0x1){
                printk("dpb_size: 0x%x\n", dpb_size);
                printk("ref_size: 0x%x\n", ref_size);
                printk("total_dec_frame_buffering[1] : 0x%x\n", total_dec_frame_buffering[1]);
                printk("max_reference_frame_num: 0x%x\n", max_reference_frame_num);
            }
            ref_start_addr[1] = dpb_start_addr[1] + 
                                (dpb_size * total_dec_frame_buffering[1]); 
            dpb_start_addr[2] = ref_start_addr[1] +
                                (ref_size * (max_reference_frame_num+1));

            if(dbg_mode&0x1){
                printk("dpb_start_addr[1]: 0x%x\n", dpb_start_addr[1]);
                printk("ref_start_addr[1]: 0x%x\n", ref_start_addr[1]);
                printk("dpb_start_addr[2]: 0x%x\n", dpb_start_addr[2]);
            }
            if(dpb_start_addr[2] >= DECODE_BUFFER_END)
            {
               printk(" No enough memory for alloc view 1\n");
               goto exit;
            }

            index = CANVAS_INDEX_START + total_dec_frame_buffering[0] * 3;
            ANC_CANVAS_ADDR = ANC0_CANVAS_ADDR + total_dec_frame_buffering[0];

            ret = init_canvas(dpb_start_addr[1], dpb_size, total_dec_frame_buffering[1], mb_width, mb_height, buffer_spec1); 

            if(ret == -1) {
               printk(" Un-expected memory alloc problem\n");
               goto exit;
            }

            WRITE_MPEG_REG(REF_START_VIEW_1, video_domain_addr(ref_start_addr[1]));
            WRITE_MPEG_REG(MAILBOX_DATA_0, 
                (max_dec_frame_buffering[1] << 8) |
                (total_dec_frame_buffering[1] << 0)
              );
            WRITE_MPEG_REG(MAILBOX_DATA_1, ref_size); 
            WRITE_MPEG_REG(MAILBOX_COMMAND, CMD_FINISHED);

            if(dbg_mode&0x1){
                printk("End H264 display buffer allocation for view 1\n");
            }
            if(frame_width == 0){
                frame_width = mb_width<<4;    
            }
            if(frame_height == 0){
                frame_height = mb_height<<4;    
                if(frame_height == 1088) frame_height = 1080;
            }
            break;
          case CMD_FRAME_DISPLAY:
            ret = READ_MPEG_REG(MAILBOX_DATA_0);
            display_buff_id = (ret >> 0) & 0x3f;
            display_view_id = (ret >> 6) & 0x3;
            drop_status = (ret >> 8) & 0x1;
            display_POC = READ_MPEG_REG(MAILBOX_DATA_1);
            stream_offset = READ_MPEG_REG(MAILBOX_DATA_2);
//if (display_view_id == 0)
//printk("view_id=%d,buff_id=%d,offset=%d, display_POC = %d,ret=0x%x\n", display_view_id, display_buff_id, stream_offset, display_POC, ret);
            WRITE_MPEG_REG(MAILBOX_COMMAND, CMD_FINISHED);

#ifdef DEBUG_SKIP
            view_total++;
            if (drop_status)
                view_dropped++;
#endif

            if(dbg_mode&0x1){
                printk(" H264 display frame ready -------------- View : %x, Buffer : %x\n", display_view_id, display_buff_id);
                printk(" H264 display frame POC -------------- Buffer : %x, POC : %x\n", display_buff_id, display_POC);
                printk("H264 display frame ready\n");
            }
            if(dbg_mode&0x10){
                if((dbg_mode&0x20)==0){
                    while(READ_MPEG_REG(BUFFER_RECYCLE) !=0) {}
                    WRITE_MPEG_REG(BUFFER_RECYCLE, (display_view_id<<8)|(display_buff_id+1));
                    display_buff_id = -1;
                    display_view_id = -1;
                    display_POC = -1;
                }
            }
            else{
                mvc_buf_t *p = NULL, *ptmp;
                unsigned char in_list_flag = 0;
                ulong fiq_flag;
                raw_local_save_flags(fiq_flag);
                local_fiq_disable();
                list_for_each_entry_safe(p, ptmp, &ready_list_head, list) {
                    if(p->display_POC == display_POC){
                        if(display_view_id == 0){
                            if(p->view0_buff_id>=0){
#ifdef DEBUG_PTS
                                dump_ready_list();
#endif
                                printk("H264MVC Warning, view0_buff_id is %d, not -1\n", p->view0_buff_id);    
                            }
                            p->view0_buff_id = display_buff_id;
                            p->view0_drop    = drop_status;
                            p->stream_offset = stream_offset;
                        }
                        else{
                            if(p->view1_buff_id>=0){
#ifdef DEBUG_PTS
                                dump_ready_list();
#endif
                                printk("H264MVC Warning, view1_buff_id is %d, not -1\n", p->view1_buff_id);    
                            }
                            p->view1_drop    = drop_status;
                            p->view1_buff_id = display_buff_id;
                        }
                        in_list_flag = 1;
                        break;
                    }
                }
                if(in_list_flag == 0){
                    if(list_empty(&free_list_head)){
                        printk("H264MVC Error, free_list empty\n");
                    }
                    else{
                        p = list_first_entry(&free_list_head, struct mvc_buf_s, list);
                        p->display_POC = display_POC;
                        if(display_view_id == 0){
                            p->view0_buff_id = display_buff_id;
                            p->view0_drop    = drop_status;
                            p->stream_offset = stream_offset;
                        }
                        else{
                            p->view1_buff_id = display_buff_id;
                            p->view1_drop    = drop_status;
                        }

#if 1
                        if (pts_lookup_offset(PTS_TYPE_VIDEO, p->stream_offset, &p->vframe.pts, 0) != 0) {
                            p->vframe.pts = 0;
                        }
#else
                        p->vframe.pts = 0;
#endif
                        set_frame_info(&p->vframe);
                        in_list_flag = 1;
                        list_del(&(p->list));
                        list_add_tail(&(p->list), &ready_list_head);
                         vf_notify_receiver(PROVIDER_NAME,VFRAME_EVENT_PROVIDER_VFRAME_READY,NULL);
                    }
                }
                raw_local_irq_restore(fiq_flag);

                if(in_list_flag){
                    display_buff_id = -1;
                    display_view_id = -1;
                    display_POC = -1;
                    stream_offset = -1;
                }
            }
            break;
          default : break;
        }
exit:        
#ifdef HANDLE_h264mvc_IRQ
    return IRQ_HANDLED;
#else
    return;
#endif
}

static void recycle_vframe(void)
{
    int cnt = 0;
    mvc_buf_t *p = NULL, *ptmp;

    list_for_each_entry_safe(p, ptmp, &recycle_list_head, list) {
        cnt++;
    }

    if((!list_empty(&recycle_list_head))&&
        (READ_MPEG_REG(BUFFER_RECYCLE) ==0)){
        mvc_buf_t* mvc_buf;
        ulong fiq_flag;
        raw_local_save_flags(fiq_flag);
        local_fiq_disable();
        
        mvc_buf = list_first_entry(&recycle_list_head, struct mvc_buf_s, list);
        if(mvc_buf->view0_buff_id >= 0){
            WRITE_MPEG_REG(BUFFER_RECYCLE, (0<<8)|(mvc_buf->view0_buff_id+1));
            mvc_buf->view0_buff_id = -1;
        }
        else if(mvc_buf->view1_buff_id >= 0){
            WRITE_MPEG_REG(BUFFER_RECYCLE, (1<<8)|(mvc_buf->view1_buff_id+1));
            mvc_buf->view1_buff_id = -1;
        }
        if((mvc_buf->view0_buff_id < 0)&&
            (mvc_buf->view1_buff_id < 0)){
            mvc_buf->display_POC = -1;
            mvc_buf->view0_drop = 0;
            mvc_buf->view1_drop = 0;
            list_del(&mvc_buf->list);        
            list_add_tail(&mvc_buf->list, &free_list_head);
        }
        raw_local_irq_restore(fiq_flag);
    }
}
    
static void vh264mvc_put_timer_func(unsigned long arg)
{
    struct timer_list *timer = (struct timer_list *)arg;

    if((dbg_mode&0x30)==0x30){
        if(display_buff_id >=0 ){
            if(dbg_cmd==1){
                while(READ_MPEG_REG(BUFFER_RECYCLE) !=0) {}
                WRITE_MPEG_REG(BUFFER_RECYCLE, (display_view_id<<8)|(display_buff_id+1));
                display_buff_id = -1;
                display_view_id = -1;
                display_POC = -1;
                dbg_cmd = 0;
            }
        }
    }

    recycle_vframe();   

#ifndef HANDLE_h264mvc_IRQ
    if(display_buff_id<0){
        vh264mvc_isr();
    }
#endif

    timer->expires = jiffies + PUT_INTERVAL;

    add_timer(timer);
}

int vh264mvc_dec_status(struct vdec_status *vstatus)
{
    vstatus->width = frame_width;
    vstatus->height = frame_height;
    if (frame_dur != 0) {
        vstatus->fps = 96000 / frame_dur;
    } else {
        vstatus->fps = -1;
    }
    vstatus->error_count = READ_MPEG_REG(AV_SCRATCH_D);
    vstatus->status = stat;
    return 0;
}

int vh264mvc_set_trickmode(unsigned long trickmode)
{
    if (trickmode == TRICKMODE_I) {
        WRITE_MPEG_REG(AV_SCRATCH_F, (READ_MPEG_REG(AV_SCRATCH_F) & 0xfffffffc) | 2);
        trickmode_i = 1;
    } else if (trickmode == TRICKMODE_NONE) {
        WRITE_MPEG_REG(AV_SCRATCH_F, READ_MPEG_REG(AV_SCRATCH_F) & 0xfffffffc);
        trickmode_i = 0;
    }

    return 0;
}

static void H264_DECODE_INIT(void)
{
  int i;
  i = READ_MPEG_REG(DECODE_SKIP_PICTURE);
#if 1
    WRITE_MPEG_REG(RESET0_REGISTER, RESET_IQIDCT | RESET_MC | RESET_VLD_PART);
    READ_MPEG_REG(RESET0_REGISTER);
    WRITE_MPEG_REG(RESET0_REGISTER, RESET_IQIDCT | RESET_MC | RESET_VLD_PART);
    WRITE_MPEG_REG(RESET2_REGISTER, RESET_PIC_DC | RESET_DBLK);
#else
  WRITE_MPEG_REG(
      RESET0_REGISTER,
      (
        (1<<15) |  // vififo_reset_n
        (1<<14) |  // vld_part_reset_n
        (1<<13) |  // mdec_reset_n
        (1<<3)  |  // mc_reset_n
        (1<<2)  |  // iqidct_reset_n
        (1<<1)     // vld_reset_n
      )
    );
    READ_MPEG_REG(RESET0_REGISTER);
  WRITE_MPEG_REG(
      RESET0_REGISTER,
      (
        (1<<15) |  // vififo_reset_n
        (1<<14) |  // vld_part_reset_n
        (1<<13) |  // mdec_reset_n
        (1<<3)  |  // mc_reset_n
        (1<<2)  |  // iqidct_reset_n
        (1<<1)     // vld_reset_n
      )
    );
  WRITE_MPEG_REG(
      RESET2_REGISTER,
      (
        (1<<4)  |  // psc_reset_n
        (1<<3)  |  // pic_dc_reset_n
        (1<<2)     // dblk_reset_n
      )
    );
#endif
  // Wait for some time for RESET
  READ_MPEG_REG(DECODE_SKIP_PICTURE);
  READ_MPEG_REG(DECODE_SKIP_PICTURE);

  WRITE_MPEG_REG(DECODE_SKIP_PICTURE, i);

  // fill_weight_pred
  WRITE_MPEG_REG(MC_MPORT_CTRL, 0x0300);
  for(i=0; i<192; i++) WRITE_MPEG_REG(MC_MPORT_DAT, 0x100);
  WRITE_MPEG_REG(MC_MPORT_CTRL, 0);

  WRITE_MPEG_REG(MB_WIDTH, 0xff); // invalid mb_width

  // set slice start to 0x000000 or 0x000001 for check more_rbsp_data
  WRITE_MPEG_REG(SLICE_START_BYTE_01, 0x00000000);
  WRITE_MPEG_REG(SLICE_START_BYTE_23, 0x01010000);

  WRITE_MPEG_REG(MPEG1_2_REG, 1); // set to mpeg2 to enable mismatch logic
  WRITE_MPEG_REG(VLD_ERROR_MASK, 0x1011); // disable COEF_GT_64 , error_m4_table and voff_rw_err

  // Config MCPU Amrisc interrupt
  WRITE_MPEG_REG(ASSIST_AMR1_INT0, 0x1 ); // viu_vsync_int
  WRITE_MPEG_REG(ASSIST_AMR1_INT1, 0x5 ); // mbox_isr
  WRITE_MPEG_REG(ASSIST_AMR1_INT2, 0x8 ); // vld_isr
  WRITE_MPEG_REG(ASSIST_AMR1_INT3, 0x15); // vififo_empty
  WRITE_MPEG_REG(ASSIST_AMR1_INT4, 0xd ); // rv_ai_mb_finished_int
  WRITE_MPEG_REG(ASSIST_AMR1_INT7, 0x14); // dcac_dma_done

  // Config MCPU Amrisc interrupt
  WRITE_MPEG_REG(ASSIST_AMR1_INT5, 0x9 ); // MCPU interrupt
  WRITE_MPEG_REG(ASSIST_AMR1_INT6, 0x17); // CCPU interrupt

  WRITE_MPEG_REG(CPC_P, 0xc00); // CCPU Code will start from 0xc00
  WRITE_MPEG_REG(CINT_VEC_BASE, (0xc20>>5));
#if 0
    WRITE_MPEG_REG(POWER_CTL_VLD, READ_MPEG_REG(POWER_CTL_VLD) | (0 << 10) | (1 << 9) | (1 << 6));
#else    
  WRITE_MPEG_REG(
      POWER_CTL_VLD,
      (
        (1<<10) | // disable cabac_step_2
        (1<<9)  | // viff_drop_flag_en
        (1<<6)    // h264_000003_en
      )
    );
#endif
  WRITE_MPEG_REG(M4_CONTROL_REG, (1<<13));  // H264_DECODE_INFO - h264_en

  WRITE_MPEG_REG(CANVAS_START , CANVAS_INDEX_START);
#if 1
  WRITE_MPEG_REG(WORKSPACE_START, video_domain_addr(work_space_adr)); // Start Address of Workspace (UCODE, temp_data...) 
#else
  WRITE_MPEG_REG(WORKSPACE_START, 0x05000000); // Start Address of Workspace (UCODE, temp_data...) 
#endif  
  WRITE_MPEG_REG(SPS_STATUS, 0); // Clear all sequence parameter set available
  WRITE_MPEG_REG(PPS_STATUS, 0); // Clear all picture parameter set available
  WRITE_MPEG_REG(CURRENT_UCODE, 0xff); // Set current microcode to NULL
  WRITE_MPEG_REG(CURRENT_SPS_PPS, 0xffff); // Set current SPS/PPS to NULL
  WRITE_MPEG_REG(DECODE_STATUS, 1); // Set decode status to DECODE_START_HEADER 


    
}    

#if 0
static void INIT_VIFF(void)
{
       // Cleanup
       WRITE_MPEG_REG(VLD_MEM_VIFIFO_CONTROL, 0x0000);
       // VB_START = 0x05200000
       WRITE_MPEG_REG(VLD_MEM_VIFIFO_START_PTR, 0x05200000);
       // VB_END = 0x02000000
       WRITE_MPEG_REG(VLD_MEM_VIFIFO_END_PTR, 0x1ffffff8);
       // VB_RD_START = 0x05200000
       WRITE_MPEG_REG(VLD_MEM_VIFIFO_CURR_PTR, 0x05200000);
       // set manual mode
       WRITE_MPEG_REG(VLD_MEM_VIFIFO_BUF_CNTL, 0x0002);
       // SET MANUAL WR PTR = 0x02000000
       WRITE_MPEG_REG(VLD_MEM_VIFIFO_WP, 0x1ffffff8);
       // filling based on level
       WRITE_MPEG_REG(VLD_MEM_VIFIFO_CONTROL, 0x0400);
       // set cntl_init
       WRITE_MPEG_REG(VLD_MEM_VIFIFO_CONTROL, 0x0401);
       // clear cntl_init
       WRITE_MPEG_REG(VLD_MEM_VIFIFO_CONTROL, 0x0400);
       // set Init
       WRITE_MPEG_REG(VLD_MEM_VIFIFO_BUF_CNTL, 0x0003);
       // clear Init
       WRITE_MPEG_REG(VLD_MEM_VIFIFO_BUF_CNTL, 0x0002);
       // set cntl_fill_en and cntl_empty_en
       WRITE_MPEG_REG(VLD_MEM_VIFIFO_CONTROL, 0x00110406); // When VB < 0x80 or vififo < 8dw, interrupt
}
#endif

static void vh264mvc_prot_init(void)
{
    while (READ_MPEG_REG(DCAC_DMA_CTRL) & 0x8000) {
        ;
    }
    while (READ_MPEG_REG(LMEM_DMA_CTRL) & 0x8000) {
        ;    // reg address is 0x350
    }
    /* clear mailbox interrupt */
    WRITE_MPEG_REG(ASSIST_MBOX1_CLR_REG, 1);

    /* enable mailbox interrupt */
    WRITE_MPEG_REG(ASSIST_MBOX1_MASK, 1);

    /* disable PSCALE for hardware sharing */
    WRITE_MPEG_REG(PSCALE_CTRL, 0);
    
    H264_DECODE_INIT();

    WRITE_MPEG_REG(RESET0_REGISTER, 0x80); // RESET MCPU

    //INIT_VIFF();

    WRITE_MPEG_REG(MAILBOX_COMMAND, 0);
    WRITE_MPEG_REG(BUFFER_RECYCLE, 0);
    WRITE_MPEG_REG(DROP_CONTROL, 0);
}

static void vh264mvc_local_init(void)
{
    display_buff_id = -1;
    display_view_id = -1;
    display_POC = -1;
    no_dropping_cnt = 0;

#ifdef DEBUG_PTS
    pts_missed = 0;
    pts_hit = 0;
#endif

#ifdef DEBUG_SKIP
    view_total = 0;
    view_dropped = 0;
#endif

    //vh264mvc_ratio = vh264mvc_amstream_dec_info.ratio;
    vh264mvc_ratio = 0x100;

    //frame_width = vh264mvc_amstream_dec_info.width;
    //frame_height = vh264mvc_amstream_dec_info.height;
    frame_dur = vh264mvc_amstream_dec_info.rate;
    if(frame_dur == 0){
        frame_dur = 96000/24;
    }
    
    pts_outside = ((u32)vh264mvc_amstream_dec_info.param) & 0x01;
    sync_outside = ((u32)vh264mvc_amstream_dec_info.param & 0x02) >> 1;

    /**/
    dpb_start_addr[0] = -1; 
    dpb_start_addr[1] = -1; 
    max_dec_frame_buffering[0] = -1; 
    max_dec_frame_buffering[1] = -1; 

    init_vf_buf();
    return;
}

static s32 vh264mvc_init(void)
{
    void __iomem *p = ioremap_nocache(work_space_adr, work_space_size);

    if (!p) {
        printk("\nvh264mvc_init: Cannot remap ucode swapping memory\n");
        return -ENOMEM;
    }

    printk("\nvh264mvc_init\n");
    init_timer(&recycle_timer);

    stat |= STAT_TIMER_INIT;

    vh264mvc_local_init();

    amvdec_enable();

    if (amvdec_loadmc(vh264mvc_mc) < 0) {
        amvdec_disable();
        return -EBUSY;
    }

    memcpy(p,
           vh264mvc_header_mc, sizeof(vh264mvc_header_mc));

    memcpy((void *)((ulong)p + 0x1000),
           vh264mvc_mmco_mc, sizeof(vh264mvc_mmco_mc));

    memcpy((void *)((ulong)p + 0x3000),
           vh264mvc_slice_mc, sizeof(vh264mvc_slice_mc));

    iounmap(p);

    stat |= STAT_MC_LOAD;

    /* enable AMRISC side protocol */
    vh264mvc_prot_init();

#ifdef HANDLE_h264mvc_IRQ
    if (request_irq(INT_MAILBOX_1A, vh264mvc_isr,
                    IRQF_SHARED, "vh264mvc-irq", (void *)vh264mvc_dec_id)) {
        printk("vh264mvc irq register error.\n");
        amvdec_disable();
        return -ENOENT;
    }
#endif

    stat |= STAT_ISR_REG;

    vf_provider_init(&vh264mvc_vf_prov, PROVIDER_NAME, &vh264mvc_vf_provider, NULL);
    vf_reg_provider(&vh264mvc_vf_prov);
    vf_notify_receiver(PROVIDER_NAME,VFRAME_EVENT_PROVIDER_START,NULL);

    stat |= STAT_VF_HOOK;

    recycle_timer.data = (ulong) & recycle_timer;
    recycle_timer.function = vh264mvc_put_timer_func;
    recycle_timer.expires = jiffies + PUT_INTERVAL;

    add_timer(&recycle_timer);

    stat |= STAT_TIMER_ARM;

    amvdec_start();

    stat |= STAT_VDEC_RUN;

    set_vdec_func(&vh264mvc_dec_status);
    //set_trickmode_func(&vh264mvc_set_trickmode);

    return 0;
}

static int vh264mvc_stop(void)
{
    if (stat & STAT_VDEC_RUN) {
        amvdec_stop();
        stat &= ~STAT_VDEC_RUN;
    }

    if (stat & STAT_ISR_REG) {
        WRITE_MPEG_REG(ASSIST_MBOX1_MASK, 0);
#ifdef HANDLE_h264mvc_IRQ
        free_irq(INT_MAILBOX_1A,
                 (void *)vh264mvc_dec_id);
#endif                 
        stat &= ~STAT_ISR_REG;
    }

    if (stat & STAT_TIMER_ARM) {
        del_timer_sync(&recycle_timer);
        stat &= ~STAT_TIMER_ARM;
    }

    if (stat & STAT_VF_HOOK) {
        ulong flags;
        spin_lock_irqsave(&lock, flags);
        spin_unlock_irqrestore(&lock, flags);
        vf_unreg_provider(&vh264mvc_vf_prov);
        stat &= ~STAT_VF_HOOK;
    }

    amvdec_disable();

    uninit_vf_buf();
    return 0;
}

static void error_do_work(struct work_struct *work)
{
    if (atomic_read(&vh264mvc_active)) {
        vh264mvc_stop();
        vh264mvc_init();
    }
}

static int amvdec_h264mvc_probe(struct platform_device *pdev)
{
    struct resource *mem;
    int buf_size;

    printk("amvdec_h264mvc probe start.\n");

    if (!(mem = platform_get_resource(pdev, IORESOURCE_MEM, 0))) {
        printk("\namvdec_h264mvc memory resource undefined.\n");
        return -EFAULT;
    }

    buf_size = mem->end - mem->start + 1;
    //buf_offset = mem->start - DEF_BUF_START_ADDR;
    work_space_adr = mem->start;
    DECODE_BUFFER_START = work_space_adr + work_space_size;
    DECODE_BUFFER_END = mem->start + buf_size;

    printk("work_space_adr %x, DECODE_BUFFER_START %x, DECODE_BUFFER_END %x\n", work_space_adr, DECODE_BUFFER_START, DECODE_BUFFER_END);
    memcpy(&vh264mvc_amstream_dec_info, (void *)mem[1].start, sizeof(vh264mvc_amstream_dec_info));

    if (vh264mvc_init() < 0) {
        printk("\namvdec_h264mvc init failed.\n");

        return -ENODEV;
    }

    INIT_WORK(&error_wd_work, error_do_work);

    atomic_set(&vh264mvc_active, 1);

    printk("amvdec_h264mvc probe end.\n");

    return 0;
}

static int amvdec_h264mvc_remove(struct platform_device *pdev)
{
    printk("amvdec_h264mvc_remove\n");
    vh264mvc_stop();

    atomic_set(&vh264mvc_active, 0);

#ifdef DEBUG_PTS
    printk("pts missed %ld, pts hit %ld, pts_outside %d, duration %d, sync_outside %d\n",
           pts_missed, pts_hit, pts_outside, frame_dur, sync_outside);
#endif

#ifdef DEBUG_SKIP
    printk("view_total = %ld, dropped %ld\n", view_total, view_dropped);
#endif

    return 0;
}

/****************************************/

static struct platform_driver amvdec_h264mvc_driver = {
    .probe   = amvdec_h264mvc_probe,
    .remove  = amvdec_h264mvc_remove,
#ifdef CONFIG_PM
    .suspend = amvdec_suspend,
    .resume  = amvdec_resume,
#endif
    .driver  = {
        .name = DRIVER_NAME,
    }
};

static int __init amvdec_h264mvc_driver_init_module(void)
{
    printk("amvdec_h264mvc module init\n");

    if (platform_driver_register(&amvdec_h264mvc_driver)) {
        printk("failed to register amvdec_h264mvc driver\n");
        return -ENODEV;
    }

    return 0;
}

static void __exit amvdec_h264mvc_driver_remove_module(void)
{
    printk("amvdec_h264mvc module remove.\n");

    platform_driver_unregister(&amvdec_h264mvc_driver);
}

/****************************************/

module_param(stat, uint, 0664);
MODULE_PARM_DESC(stat, "\n amvdec_h264mvc stat \n");

module_param(dbg_mode, uint, 0664);
MODULE_PARM_DESC(dbg_mode, "\n amvdec_h264mvc dbg mode \n");

module_param(view_mode, uint, 0664);
MODULE_PARM_DESC(view_mode, "\n amvdec_h264mvc view mode \n");

module_param(dbg_cmd, uint, 0664);
MODULE_PARM_DESC(dbg_mode, "\n amvdec_h264mvc cmd mode \n");

module_param(drop_rate, uint, 0664);
MODULE_PARM_DESC(dbg_mode, "\n amvdec_h264mvc drop rate \n");

module_init(amvdec_h264mvc_driver_init_module);
module_exit(amvdec_h264mvc_driver_remove_module);

MODULE_DESCRIPTION("AMLOGIC h264mvc Video Decoder Driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chen Zhang <chen.zhang@amlogic.com>");
