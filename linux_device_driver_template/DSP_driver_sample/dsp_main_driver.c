/*
 *	Sunplus DSP /SSD driver on 2011/11/02
 *	Product : SPV9200_CPU-D demo board 
 *	Author  : Mark Tseng mark.tseng@sunplus.com
 */

#include <linux/module.h>

#if defined (CONFIG_SMP)
#define __SMP__
#endif

#if defined(CONFIG_MODVERSIONS)
#define MODVERSIONS
#include <linux/modversions.h>
#endif

#include <linux/kernel.h>
#include <linux/sched.h>	/* tasklet workqueue */
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>   /* printk() */
#include <linux/slab.h>     /* kmalloc() */
#include <linux/fs.h>       /* everything... */
#include <linux/errno.h>    /* error codes */
#include <linux/types.h>    /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>    /* O_ACCMODE */
#include <linux/aio.h>
#include <linux/ioctl.h>	/* ioctl */
#include <linux/cdev.h>		/* char device API */
#include <linux/device.h>	/* dev_name(),dev_get_drvdata,dev_set_drvdata  */
#include <linux/list.h>		/* link list*/
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/seq_file.h>
#include <linux/vmalloc.h>
#include <linux/platform_device.h> /* platform device structure*/
#include <linux/ioport.h> /* resource structure */
#include <linux/kthread.h> /* kthread_run, kthread_stop */

#include <net/sock.h>		/* sock struct */
#include <net/netlink.h>	/* netlink protocol */
#include <linux/skbuff.h>	/* socket buffer */


#include <asm/mach-spv9200/regmap_9200.h> /* SPV9200 CPU Register */
#include <asm/mach-spv9200/spv9200_memlayout.h> /* SPV9200 memory layout */
#include <asm/mach-spv9200/sizes.h> /* SZ_XX for resource structure */
#include <asm/cacheflush.h>
#include <asm/uaccess.h>
#include <asm/page.h>
#include <asm/io.h>	/* ioremap,ioremap_nocache, ioread32/iowrite32, request_mem_region, release_mem_region */

#include <asm/mach-spv9200/types.h>
#include <asm/mach-spv9200/ic-spv9200/hw_public.h>

// dsp header file
#include "include/aud_global.h"
#include "include/auddrv.h"
#include "include/aud_hw.h"
#include "include/wave_fmt.h"

// ssd header file
#include "drv_ssd_resource.h"
#include "drv_ssd_access.h"
#include "drv_ssd_config.h"
#include "drv_ssd.h"
#include "hw_ssd_def.h"

// tvfe headr file
#include "include/tvfe_api.h"
#include "include/tvfe_audio.h"

// dsp mon header file
#include "include/dsp_mon.h"

#include "dsp_main_driver.h"

DSP_DEV *pDSPDev;

/* Playback BS ptr */
BYTE *audBSbuf_start = NULL;
static INT32 audBSbuf_free,audBSbuf_size;
static u32 dsp_off_bytes, risc_off_bytes;

/* memory map point for auddrv DSP DRAM size */
extern unsigned long __iomem *gAUDBufAddr; 

/* TVFE */
extern bool gMasterMuteStatus;
extern TVFE_Audio_Aux_Path_t gAudioAuxPath;
extern TVFE_Audio_Dac_Mapping_t gAudioDacMapping;


static int dsp_mmap(struct file *file, struct vm_area_struct *vma)
{
	unsigned long size;

    DSP_DBGMSG("mmap: vma->vm_start     = %#lx\n", vma->vm_start);
    DSP_DBGMSG("mmap: vma->vm_pgoff     = %#lx\n", vma->vm_pgoff);
    DSP_DBGMSG("mmap: vma->vm_end       = %#lx\n", vma->vm_end);
    DSP_DBGMSG("mmap: size              = %#lx\n", vma->vm_end - vma->vm_start);
	DSP_DBGMSG("mmap: SDRAMB virtual    = %#lx\n", pDSPDev->phys_addr >> PAGE_SHIFT);

	/* no such device */
	if (!pDSPDev->virtual_mem_base)
		return -ENXIO;
	
	/* size must be a multiple of PAGE_SIZE */
	size = vma->vm_end - vma->vm_start;

    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

    vma->vm_flags |= VM_RESERVED | VM_IO;

	// mmap : convert SDRAMB phys addr to virt addr in 3rd argument
    if (io_remap_pfn_range(vma, vma->vm_start, pDSPDev->phys_addr >> PAGE_SHIFT,
                        vma->vm_end - vma->vm_start, vma->vm_page_prot)) {
        DSP_DBGMSG("set_noncached: failed remap_pfn_range\n");
        return -EAGAIN;
    }

	DSP_DBGMSG("region mmapped\n");
	return 0;
}

/* DSP ioctl command */
static long dsp_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	DSP_IOCTL_CMD dspCMD;
	Aud_EqualizerConfig_t EQ_config;
	AUDIF_DacMapping_t ChannelMap;
	Aud_AudioAVLConfig_t AVL_config;
	UINT8 ui8_arg_1;
	UINT16 ui16_arg_1;
	TVFE_Audio_CMD_st TVFE_CMD;
	WAVFORMATMACRO wfmt;

	/* don't even decode wrong cmds: better returning  ENOTTY than EFAULT */
	if (_IOC_TYPE(cmd) != DSP_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > DSP_IOC_MAXNR ) return -ENOTTY;

	/* ioctl cmd parser */
	switch (cmd) {

/* AUDID_XXX_XXX function */
		case DSP_AUDIF_Set_MasterVolume: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_MasterVolume: %d\n",dspCMD.argument_1);
			ret = AUDIF_Set_MasterVolume(dspCMD.argument_1);
			return 0;
		}

		case DSP_AUDIF_Set_MainChannelOutput: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_MainChannelOutput: %d\n",dspCMD.argument_1);
			ret = AUDIF_Set_MainChannelOutput(dspCMD.argument_1);
			return ret;
		}

		case DSP_AUDIF_Set_AuxChannelOutput: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_AuxChannelOutput: %d, %d\n",dspCMD.argument_1,dspCMD.argument_2);
			ret = AUDIF_Set_AuxChannelOutput(dspCMD.argument_1,dspCMD.argument_2);
			return ret;
		}


		case DSP_AUDIF_Set_AdcSourceSelect: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("AUDIF_Set_AdcSourceSelect: %d\n",dspCMD.argument_1);
			ret = AUDIF_Set_AdcSourceSelect(dspCMD.argument_1);
			return ret;
		}

		case DSP_AUDIF_Set_Bass: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_Bass: %d\n",dspCMD.argument_1);
			ret = AUDIF_Set_Bass(dspCMD.argument_1);
			return ret;
		}

		case DSP_AUDIF_Set_Treble: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_Treble: %d\n",dspCMD.argument_1);
			ret = AUDIF_Set_Treble(dspCMD.argument_1);
			return ret;
		}

		case DSP_AUDIF_Set_MainEqulizer: {
			memset(&EQ_config,0,sizeof(Aud_EqualizerConfig_t));
		   	if(copy_from_user(&EQ_config, (Aud_EqualizerConfig_t *)arg, sizeof(Aud_EqualizerConfig_t)))
		 	  return -EFAULT;
			
			DSP_DBGMSG("DSP_AUDIF_Set_MainEqulizer: %d\n",EQ_config.fEnBypass);
			DSP_DBGMSG("DSP_AUDIF_Set_MainEqulizer: %d\n",EQ_config.bMainEQ_Band1_0100HZ);
			DSP_DBGMSG("DSP_AUDIF_Set_MainEqulizer: %d\n",EQ_config.bMainEQ_Band2_0300HZ);
			DSP_DBGMSG("DSP_AUDIF_Set_MainEqulizer: %d\n",EQ_config.bMainEQ_Band3_1000HZ);
			DSP_DBGMSG("DSP_AUDIF_Set_MainEqulizer: %d\n",EQ_config.bMainEQ_Band4_3000HZ);
			DSP_DBGMSG("DSP_AUDIF_Set_MainEqulizer: %d\n",EQ_config.bMainEQ_Band5_8000HZ);
			ret = AUDIF_Set_MainEqulizer(&EQ_config);
			return ret;
		}

		case DSP_SET_MUTE: {
			// argument_1 : bMute
			// argument_2 : PIN_NUMBER
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;
			DSP_DBGMSG("DSP_SET_MUTE: %d\n",dspCMD.argument_1);
			if(dspCMD.argument_1)
			{
				AUDHW_Enable_Mute(dspCMD.argument_2);
			} else {
				AUDHW_Disable_Mute(dspCMD.argument_2);
			}
			return 0;
		}

		case DSP_AUDIF_Set_Balance: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_Balance: %d, %d\n",dspCMD.argument_1,dspCMD.argument_2);
			ret = AUDIF_Set_Balance(dspCMD.argument_1,dspCMD.argument_2);
			return ret;
		}

		case DSP_AUDIF_Set_ChannelMap: {
			memset(&ChannelMap,0,sizeof(AUDIF_DacMapping_t));
		   	if(copy_from_user(&ChannelMap, (AUDIF_DacMapping_t *)arg, sizeof(AUDIF_DacMapping_t)))
		 	  return -EFAULT;
			
			DSP_DBGMSG("DSP_AUDIF_Set_ChannelMap: %d\n",ChannelMap.DacMapping_Data0_L);
			DSP_DBGMSG("DSP_AUDIF_Set_ChannelMap: %d\n",ChannelMap.DacMapping_Data0_R);
			DSP_DBGMSG("DSP_AUDIF_Set_ChannelMap: %d\n",ChannelMap.DacMapping_Data1_L);
			DSP_DBGMSG("DSP_AUDIF_Set_ChannelMap: %d\n",ChannelMap.DacMapping_Data1_R);
			DSP_DBGMSG("DSP_AUDIF_Set_ChannelMap: %d\n",ChannelMap.DacMapping_Data2_L);
			DSP_DBGMSG("DSP_AUDIF_Set_ChannelMap: %d\n",ChannelMap.DacMapping_Data2_R);
			ret = AUDIF_Set_ChannelMap(&ChannelMap);
			return ret;
		}
		
		case DSP_AUDIF_Set_Aux1Volume: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_Aux1Volume: %d\n",dspCMD.argument_1);
			ret = AUDIF_Set_Aux1Volume(dspCMD.argument_1);
			return ret;
		}

		case DSP_AUDIF_Set_Aux2Volume: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_Aux2Volume: %d\n",dspCMD.argument_1);
			ret = AUDIF_Set_Aux2Volume(dspCMD.argument_1);
			return ret;
		}

		case DSP_AUDIF_Set_EffectMode: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_EffectMode: %d\n",dspCMD.argument_1);
			ret = AUDIF_Set_EffectMode(dspCMD.argument_1);
			return ret;
		}

		case DSP_AUDIF_Set_SUNSUR_OnOff: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_SUNSUR_OnOff: %d\n",dspCMD.argument_1);
			ret = AUDIF_Set_SUNSUR_OnOff(dspCMD.argument_1);
			return ret;
		}

		case DSP_AUDIF_Set_AVL: {
			memset(&AVL_config,0,sizeof(Aud_AudioAVLConfig_t));
		   	if(copy_from_user(&AVL_config, (Aud_AudioAVLConfig_t *)arg, sizeof(Aud_AudioAVLConfig_t)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_AVL: %d\n",AVL_config.eMode);
			DSP_DBGMSG("DSP_AUDIF_Set_AVL: %d\n",AVL_config.bReference_level);
			DSP_DBGMSG("DSP_AUDIF_Set_AVL: %d\n",AVL_config.fEnWeightFilter);
			DSP_DBGMSG("DSP_AUDIF_Set_AVL: %d\n",AVL_config.fEnPeakModeWithClipManagement);
			DSP_DBGMSG("DSP_AUDIF_Set_AVL: %d\n",AVL_config.Enable_AVL_Func);
			ret = AUDIF_Set_AVL(&AVL_config);
			return ret;
		}
		
		case DSP_AUDIF_Set_MasterMuteStatus: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_MasterMuteStatus: %d\n",dspCMD.argument_1);
			ret = AUDIF_Set_MasterMuteStatus(dspCMD.argument_1);
			return ret;
		}

		case DSP_AUDIF_RampUp_PCM_Out: {
			DSP_DBGMSG("DSP_AUDIF_RampUp_PCM_Out\n");
			AUDIF_RampUp_PCM_Out();
			return 0;
		}

		case DSP_AUDIF_Set_Aux1MuteStatus: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_Aux1MuteStatus: %d\n",dspCMD.argument_1);
			ret = AUDIF_Set_Aux1MuteStatus(dspCMD.argument_1);
			return ret;
		}

		case DSP_AUDIF_Set_Aux2MuteStatus: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_Aux2MuteStatus: %d\n",dspCMD.argument_1);
			ret = AUDIF_Set_Aux2MuteStatus(dspCMD.argument_1);
			return ret;
		}

		case DSP_AUDIF_Set_SPDIF: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_SPDIF: %d\n",dspCMD.argument_1);
			ret = AUDIF_Set_SPDIF(dspCMD.argument_1);
			return ret;
		}
		
		/* NO need to set sample rate again */
		case DSP_AUDIF_Set_SPDIF_SmpRate: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;
			/* FS_XXK as an argument */
			DSP_DBGMSG("DSP_AUDIF_Set_SPDIF_SmpRate: %d\n",dspCMD.argument_1);
			AUDIF_Set_SPDIF_SmpRate(dspCMD.argument_1);
			return 0;
		}

		case DSP_AUDIF_Set_AtvRamp: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_AtvRamp: %d\n",dspCMD.argument_1);
			ret = AUDIF_Set_AtvRamp(dspCMD.argument_1);
			return ret;
		}

		case DSP_AUDIF_Set_LipSync_DelayTime: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_LipSync_DelayTime: %d\n",dspCMD.argument_1);
			ret = AUDIF_Set_LipSync_DelayTime(dspCMD.argument_1);
			return ret;
		}

		case DSP_AUDIF_Set_PcmOutputPhase: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_PcmOutputPhase: %d\n",dspCMD.argument_1);
			DSP_DBGMSG("DSP_AUDIF_Set_PcmOutputPhase: %d\n",dspCMD.argument_2);
			DSP_DBGMSG("DSP_AUDIF_Set_PcmOutputPhase: %d\n",dspCMD.argument_3);
			ret = AUDIF_Set_PcmOutputPhase(dspCMD.argument_1,
										   dspCMD.argument_2,
										   dspCMD.argument_3);
			return ret;
		}

		case DSP_AUDIF_Set_SUNSUR_Param: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_SUNSUR_Param: %d\n",dspCMD.argument_1);
			ret = AUDIF_Set_SUNSUR_Param((Aud_SUNSUR_Param_t *)dspCMD.argument_1);
			return ret;
		}

		case DSP_AUDIF_Set_Decoder_OPT: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_Decoder_OPT: %d\n",dspCMD.argument_1);
			DSP_DBGMSG("DSP_AUDIF_Set_Decoder_OPT: %d\n",dspCMD.argument_2);
			ret = AUDIF_Set_Decoder_OPT(dspCMD.argument_1,
										   dspCMD.argument_2);
			return ret;
		}

		case DSP_AUDIF_Set_Spdif_ChannelStatus: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_Spdif_ChannelStatus: %d\n",dspCMD.argument_1);
			DSP_DBGMSG("DSP_AUDIF_Set_Spdif_ChannelStatus: %d\n",dspCMD.argument_2);
			DSP_DBGMSG("DSP_AUDIF_Set_Spdif_ChannelStatus: %d\n",dspCMD.argument_3);
			AUDIF_Set_Spdif_ChannelStatus(dspCMD.argument_1,
								   		dspCMD.argument_2,
								   		dspCMD.argument_3);
			return 0;
		}


		case DSP_AUDIF_Enable_AC3TransCoding: {
			DSP_DBGMSG("DSP_ENABLE_AC3_TRANSCODING\n");
			ret = AUDIF_Enable_AC3TransCoding();
			return ret;
		}

		case DSP_AUDIF_Set_SourceType: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_SourceType: %d\n",dspCMD.argument_1);
			ret = AUDIF_Set_SourceType(dspCMD.argument_1);
			return ret;
		}
		
		case DSP_AUDIF_TestTone_mode: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_TestTone_mode: %d\n",dspCMD.argument_1);
			ret = AUDIF_TestTone_mode(dspCMD.argument_1);
			return ret;
		}


		case DSP_AUDIF_UnInit_SpdifIn: {
			DSP_DBGMSG("DSP_AUDIF_UnInit_SpdifIn\n");
			AUDIF_UnInit_SpdifIn();
			return 0;
		}

		case DSP_AUDIF_Set_SampleRate: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Set_SampleRate: %d\n",dspCMD.argument_1);
			AUDIF_Set_SampleRate(dspCMD.argument_1);
			return 0;
		}

		case DSP_AUDIF_Set_FIFOEnable: {
			DSP_DBGMSG("DSP_AUDIF_Set_FIFOEnable\n");
			ret = AUDIF_Set_FIFOEnable();
			return ret;
		}

		case DSP_AUDIF_Set_FIFOReset: {
			DSP_DBGMSG("DSP_AUDIF_Set_FIFOReset\n");
			ret = AUDIF_Set_FIFOReset();
			return ret;
		}


/* DSP Get command set */
		case DSP_AUDIF_Get_MainEqulizer: {
			memset(&EQ_config,0,sizeof(Aud_EqualizerConfig_t));
		   	ret = AUDIF_Get_MainEqulizer(&EQ_config);

		   	if(copy_to_user((Aud_EqualizerConfig_t *)arg, &EQ_config, sizeof(Aud_EqualizerConfig_t)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Get_MainEqulizer\n");
			return ret;
		}

		case DSP_AUDIF_Get_MasterMuteStatus: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
			ret = AUDIF_Get_MasterMuteStatus(&ui8_arg_1);
			dspCMD.argument_1 = ui8_arg_1;
		   	if(copy_to_user((DSP_IOCTL_CMD *)arg, &dspCMD, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Get_MasterMuteStatus: %d\n",dspCMD.argument_1);
			return ret;
		}

		case DSP_AUDIF_Get_Aux1MuteStatus: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
			ret = AUDIF_Get_Aux1MuteStatus(&ui8_arg_1);
			dspCMD.argument_1 = ui8_arg_1;

		   	if(copy_to_user((DSP_IOCTL_CMD *)arg, &dspCMD, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Get_Aux1MuteStatus: %d\n",dspCMD.argument_1);
			return ret;
		}

		case DSP_AUDIF_Get_Aux2MuteStatus: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
			ret = AUDIF_Get_Aux2MuteStatus(&ui8_arg_1);
			dspCMD.argument_1 = ui8_arg_1;

		   	if(copy_to_user((DSP_IOCTL_CMD *)arg, &dspCMD, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Get_Aux2MuteStatus: %d\n",dspCMD.argument_1);
			return ret;
		}

		case DSP_AUDIF_Get_DdpFmt: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
			ret = AUDIF_Get_DdpFmt(&ui16_arg_1);
			dspCMD.argument_1 = ui16_arg_1;

		   	if(copy_to_user((DSP_IOCTL_CMD *)arg, &dspCMD, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Get_DdpFmt: %d\n",dspCMD.argument_1);
			return ret;
		}

/* DSP ioctl command for playback */
		case DSP_AUDIF_Demute: {
			ret = AUDIF_Demute();
			DSP_DBGMSG("DSP_AUDIF_Demute\n");
			return ret;
		}

//BYTE* AUDIF_Get_audBSbuf_start(UINT8 BankID)
		case DSP_AUDIF_Get_audBSbuf_start: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
			if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
				return -EFAULT;

			audBSbuf_start = AUDIF_Get_audBSbuf_start(dspCMD.argument_1);
			DSP_DBGMSG("DSP_AUDIF_Get_audBSbuf_start:0x%p\n",audBSbuf_start);
		
			return ret;
		}

//AUDIF_Get_audBSbuf_free
		case DSP_AUDIF_Get_audBSbuf_free: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
			if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
				return -EFAULT;

			audBSbuf_free = AUDIF_Get_audBSbuf_free(dspCMD.argument_1);

			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
			dspCMD.argument_1 = audBSbuf_free;
		   	if(copy_to_user((DSP_IOCTL_CMD *)arg, &dspCMD, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Get_audBSbuf_free:0x%02x\n",audBSbuf_free);
			return ret;
		}

//AUDIF_Get_audBSbuf_size
		case DSP_AUDIF_Get_audBSbuf_size: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
			if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
				return -EFAULT;

			audBSbuf_size = AUDIF_Get_audBSbuf_size(dspCMD.argument_1);
	
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
			dspCMD.argument_1 = audBSbuf_size;
		   	if(copy_to_user((DSP_IOCTL_CMD *)arg, &dspCMD, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_AUDIF_Get_audBSbuf_size:0x%02x\n",audBSbuf_size);
			return ret;
		}

		case DSP_AUDIF_Set_Stop: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
			if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
				return -EFAULT;

			ret = AUDIF_Set_Stop(dspCMD.argument_1);
			DSP_DBGMSG("DSP_AUDIF_Set_Stop\n");
			return ret;
		}


		case DSP_AUDIF_Set_Play: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
			if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
				return -EFAULT;

			ret = AUDIF_Set_Play(dspCMD.argument_1);
			DSP_DBGMSG("DSP_AUDIF_Set_Play\n");
			return ret;
		}

//AUDIF_Set_HDMI(0x0); 
		case DSP_AUDIF_Set_HDMI: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
			if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
				return -EFAULT;

			ret = AUDIF_Set_HDMI(dspCMD.argument_1);
			DSP_DBGMSG("DSP_AUDIF_Set_HDMI\n");
			return ret;
		}

//AUDIF_Set_Pause
		case DSP_AUDIF_Set_Pause: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
			if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
				return -EFAULT;

			ret = AUDIF_Set_Pause(dspCMD.argument_1);
			DSP_DBGMSG("DSP_AUDIF_Set_Pause\n");
			return ret;
		}


//dsp_off_bytes = AUDIF_Get_BarrierPtr(0x1);
		case DSP_AUDIF_Get_BarrierPtr: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
			if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
				return -EFAULT;

			dsp_off_bytes = AUDIF_Get_BarrierPtr(dspCMD.argument_1);

			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
			dspCMD.argument_1 = dsp_off_bytes;
		   	if(copy_to_user((DSP_IOCTL_CMD *)arg, &dspCMD, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_DSP_AUDIF_Get_BarrierPtr:%d\n",dsp_off_bytes);
			return ret;
		}


//risc_off_bytes = AUDIF_Get_DumpPtr(0x1);
		case DSP_AUDIF_Get_DumpPtr: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
			if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
				return -EFAULT;

			risc_off_bytes = AUDIF_Get_DumpPtr(dspCMD.argument_1);
			DSP_DBGMSG("DSP_AUDIF_Get_DumpPtr:%d\n",risc_off_bytes);
			return ret;
		}

//AUDIF_Set_DumpPtr( UINT8 BankID ,UINT32 val)
		case DSP_AUDIF_Set_DumpPtr: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
			if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
				return -EFAULT;
			risc_off_bytes += dspCMD.argument_2;	
			AUDIF_Set_DumpPtr(dspCMD.argument_1,risc_off_bytes);
			DSP_DBGMSG("AUDIF_Set_DumpPtr\n");
			return ret;
		}

//AUDIF_Update_Dec1_SampleRate
		case DSP_AUDIF_Update_Dec1_SampleRate: {
			AUDIF_Update_Dec1_SampleRate();
			DSP_DBGMSG("AUDIF_Update_Dec1_SampleRate\n");
			return ret;
		}

//AUDIF_Clean_Audio_Buf(UINT8 BankID)
		case DSP_AUDIF_Clean_Audio_Buf: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
			if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
				return -EFAULT;

			ret = AUDIF_Clean_Audio_Buf(dspCMD.argument_1);
			DSP_DBGMSG("AUDIF_Clean_Audio_Buf\n");
			return ret;
		}

//AUDIF_Init_Wave_Audio
		case DSP_AUDIF_Init_Wave_Audio: {
			memset(&wfmt,0,sizeof(WAVFORMATMACRO));
			if(copy_from_user(&wfmt, (WAVFORMATMACRO *)arg, sizeof(WAVFORMATMACRO)))
				return -EFAULT;
			
			ret = AUDIF_Init_Wave_Audio(&wfmt);
			DSP_DBGMSG("AUDIF_Init_Wave_Audio\n");
			return ret;
		}





/* SSD ioctl command */
		case SSD_DRV_AudSetSIFADCPGAGain: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("SSD_DRV_AudSetSIFADCPGAGain: %d\n",dspCMD.argument_1);
			ret = DRV_AudSetSIFADCPGAGain(dspCMD.argument_1);
			return ret;
		}

		case SSD_DRV_AudEnableSSDROMReg: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("SSD_AudEnableSSDROMReg: %d\n",dspCMD.argument_1);
			ret = DRV_AudEnableSSDROMReg(dspCMD.argument_1);
			return ret;
		}

		case SSD_DRV_AudSetBTSCDBXMode: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("SSD_DRV_AudSetBTSCDBXMode: %d\n",dspCMD.argument_1);
			ret = DRV_AudSetBTSCDBXMode(dspCMD.argument_1);
			return ret;
		}

		case SSD_DRV_AudSetSRCPath: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("SSD_DRV_AudSetSRCPath: %d\n",dspCMD.argument_1);
			DSP_DBGMSG("SSD_DRV_AudSetSRCPath: %d\n",dspCMD.argument_2);
			ret = DRV_AudSetSRCPath(dspCMD.argument_1, dspCMD.argument_2);
			return ret;
		}

		case SSD_DRV_AudSetDigitalMatrix: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("SSD_DRV_AudSetDigitalMatrix: %d\n",dspCMD.argument_1);
			DSP_DBGMSG("SSD_DRV_AudSetDigitalMatrix: %d\n",dspCMD.argument_2);
			ret = DRV_AudSetDigitalMatrix(dspCMD.argument_1, dspCMD.argument_2);
			return ret;
		}

		case SSD_DRV_AudSetPathAdjustLevel: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("SSD_DRV_AudSetPathAdjustLevel: %d\n",dspCMD.argument_1);
			DSP_DBGMSG("SSD_DRV_AudSetPathAdjustLevel: %d\n",dspCMD.argument_2);
			DSP_DBGMSG("SSD_DRV_AudSetPathAdjustLevel: %d\n",dspCMD.argument_3);
			DSP_DBGMSG("SSD_DRV_AudSetPathAdjustLevel: %d\n",dspCMD.argument_4);
			DSP_DBGMSG("SSD_DRV_AudSetPathAdjustLevel: %d\n",dspCMD.argument_5);
			DSP_DBGMSG("SSD_DRV_AudSetPathAdjustLevel: %d\n",dspCMD.argument_6);
			ret = DRV_AudSetPathAdjustLevel(dspCMD.argument_1, 
											dspCMD.argument_2,
											dspCMD.argument_3,
											dspCMD.argument_4,
											dspCMD.argument_5,
											dspCMD.argument_6);
			return ret;
		}

		case SSD_DRV_AudSetOverModulationAdapterConfig: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("SSD_DRV_AudSetOverModulationAdapterConfig: %d\n",dspCMD.argument_1);
			DSP_DBGMSG("SSD_DRV_AudSetOverModulationAdapterConfig: %d\n",dspCMD.argument_2);
			DSP_DBGMSG("SSD_DRV_AudSetOverModulationAdapterConfig: %d\n",dspCMD.argument_3);
			DSP_DBGMSG("SSD_DRV_AudSetOverModulationAdapterConfig: %d\n",dspCMD.argument_4);
			ret = DRV_AudSetOverModulationAdapterConfig(dspCMD.argument_1, 
														dspCMD.argument_2,
														dspCMD.argument_3,
														dspCMD.argument_4);
			return ret;
		}

		case SSD_DRV_AudSetMagnitudeDetectConfig: {
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		   	if(copy_from_user(&dspCMD, (DSP_IOCTL_CMD *)arg, sizeof(DSP_IOCTL_CMD)))
		 	  return -EFAULT;

			DSP_DBGMSG("SSD_DRV_AudSetMagnitudeDetectConfig: %d\n",dspCMD.argument_1);
			DSP_DBGMSG("SSD_DRV_AudSetMagnitudeDetectConfig: %d\n",dspCMD.argument_2);
			DSP_DBGMSG("SSD_DRV_AudSetMagnitudeDetectConfig: %d\n",dspCMD.argument_3);
			DSP_DBGMSG("SSD_DRV_AudSetMagnitudeDetectConfig: %d\n",dspCMD.argument_4);
			DSP_DBGMSG("SSD_DRV_AudSetMagnitudeDetectConfig: %d\n",dspCMD.argument_5);
			DSP_DBGMSG("SSD_DRV_AudSetMagnitudeDetectConfig: %d\n",dspCMD.argument_6);
			DSP_DBGMSG("SSD_DRV_AudSetMagnitudeDetectConfig: %d\n",dspCMD.argument_7);
			DSP_DBGMSG("SSD_DRV_AudSetMagnitudeDetectConfig: %d\n",dspCMD.argument_8);
			ret = DRV_AudSetMagnitudeDetectConfig(dspCMD.argument_1, 
												  dspCMD.argument_2,
												  dspCMD.argument_3,
												  dspCMD.argument_4,
												  dspCMD.argument_5,
												  dspCMD.argument_6,
												  dspCMD.argument_7,
												  dspCMD.argument_8);
			return ret;
		}

// TVFE ioctl

		case DSP_TVFE_Audio_Set_Output_Config: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_Audio_Set_Output_Config\n");
			ret = TVFE_Audio_Set_Output_Config(&TVFE_CMD.Set_Output_Config.Output_Config);
			return ret;
		}

		case DSP_TVFE_Audio_Set_Output_Selection: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_Audio_Set_Output_Selection\n");
			ret = TVFE_Audio_Set_Output_Selection(&TVFE_CMD.Set_Output_Selection.Output_Selection,
												  TVFE_CMD.Set_Output_Selection.eSrcVideoType);
			return ret;
		}

		case DSP_TVFE_Audio_Set_Effect_Mode: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;
			DSP_DBGMSG("DSP_TVFE_Audio_Set_Effect_Mode: %d\n",TVFE_CMD.Set_Effect_Mode.Effect_Mode);
			ret = TVFE_Audio_Set_Effect_Mode(TVFE_CMD.Set_Effect_Mode.Effect_Mode);
			return ret;
		}

		case DSP_TVFE_Audio_Set_Volume: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_Audio_Set_Volume\n");
			ret = TVFE_Audio_Set_Volume(TVFE_CMD.Set_Volume.Table,
										TVFE_CMD.Set_Volume.Size,
										TVFE_CMD.Set_Volume.Index,
										TVFE_CMD.Set_Volume.OffsetStep);
			return ret;
		}

		case DSP_TVFE_Audio_Set_Bass: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_Audio_Set_Bass\n");
			ret = TVFE_Audio_Set_Bass(TVFE_CMD.Set_Bass.Table,
									  TVFE_CMD.Set_Bass.Size,
									  TVFE_CMD.Set_Bass.Index);

			return ret;
		}

		case DSP_TVFE_Audio_Set_Treble: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_Audio_Set_Treble\n");
			ret = TVFE_Audio_Set_Treble(TVFE_CMD.Set_Treble.Table,
										TVFE_CMD.Set_Treble.Size,
										TVFE_CMD.Set_Treble.Index);

			return ret;
		}

		case DSP_TVFE_Audio_Set_Main_Equlizer_Config: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_Audio_Set_Main_Equlizer_Config\n");
			ret = TVFE_Audio_Set_Main_Equlizer_Config(&TVFE_CMD.Set_Main_Equlizer_Config.Table,
													  TVFE_CMD.Set_Main_Equlizer_Config.Size,
													  TVFE_CMD.Set_Main_Equlizer_Config.Index);

			return ret;
		}

		case DSP_TVFE_Audio_Get_Main_Equlizer_Config: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
			ret = TVFE_Audio_Get_Main_Equlizer_Config(&TVFE_CMD.Get_Main_Equlizer_Config.EQ);
			
			DSP_DBGMSG("DSP_TVFE_Audio_Get_Main_Equlizer_Config\n");
			if(copy_to_user((TVFE_Audio_CMD_st *)arg,&TVFE_CMD, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			return ret;
		}

		case DSP_TVFE_Audio_Set_Balance: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_Audio_Set_Balance\n");
			ret = TVFE_Audio_Set_Balance(TVFE_CMD.Set_Balance.Table,
										  TVFE_CMD.Set_Balance.Size,
										  TVFE_CMD.Set_Balance.LeftIndex,
										  TVFE_CMD.Set_Balance.RightIndex);

			return ret;
		}

		case DSP_TVFE_Audio_Set_AUX_Volume: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_Audio_Set_AUX_Volume\n");
			ret = TVFE_Audio_Set_AUX_Volume(&TVFE_CMD.Set_AUX_Volume.AUX_Volume_Config);

			return ret;
		}

		case DSP_TVFE_Audio_Set_AVL_Config: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_Audio_Set_AVL_Config\n");
			ret = TVFE_Audio_Set_AVL_Config(&TVFE_CMD.Set_AVL_Config.AVL_Config);

			return ret;
		}

		case DSP_TVFE_Audio_Set_Mute_Config: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_Audio_Set_Mute_Config\n");
			ret = TVFE_Audio_Set_Mute_Config(&TVFE_CMD.Set_Mute_Config.Mute_Config);

			return ret;
		}

		case DSP_TVFE_Audio_Set_Level_Adjust_Config: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_Audio_Set_Level_Adjust_Config\n");
			ret = TVFE_Audio_Set_Level_Adjust_Config(&TVFE_CMD.Set_Level_Adjust_Config.Level_Adjust_Config);

			return ret;
		}

		case DSP_TVFE_Audio_Set_SIF_Over_Modulation_Config: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_Audio_Set_SIF_Over_Modulation_Config\n");
			ret = TVFE_Audio_Set_SIF_Over_Modulation_Config(&TVFE_CMD.Set_SIF_Over_Modulation_Config.Over_Modulation_Config);

			return ret;
		}

		case DSP_TVFE_Audio_Set_SIF_Detect_Config: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_Audio_Set_SIF_Detect_Config\n");
			ret = TVFE_Audio_Set_SIF_Detect_Config(&TVFE_CMD.Set_SIF_Detect_Config.Detect_Config);

			return ret;
		}

		case DSP_TVFE_Audio_SPDIF_Output_Config: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_Audio_SPDIF_Output_Config\n");
			ret = TVFE_Audio_SPDIF_Output_Config(TVFE_CMD.SPDIF_Output_Config.SPDIF_Output_Config,
					                             TVFE_CMD.SPDIF_Output_Config.SPDIF_SmpRate);

			return ret;
		}

		case DSP_TVFE_Audio_Set_AtvRamp: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_Audio_Set_AtvRamp\n");
			ret = TVFE_Audio_Set_AtvRamp(TVFE_CMD.Set_AtvRamp.ramp_flag);

			return ret;
		}

		case DSP_TVFE_Audio_Lip_Sync_Config: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_Audio_Lip_Sync_Config\n");
			ret = TVFE_Audio_Lip_Sync_Config(TVFE_CMD.Lip_Sync_Config.Lip_Sync_Config);

			return ret;
		}

		case DSP_TVFE_Audio_PCM_Output_Phase_Config: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_Audio_PCM_Output_Phase_Config\n");
			ret = TVFE_Audio_PCM_Output_Phase_Config(TVFE_CMD.PCM_Output_Phase_Config.Pcm_Output_Phase);

			return ret;
		}

		case DSP_TVFE_Audio_Set_SUNSUR_Config: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_Audio_Set_SUNSUR_Config\n");
			ret = TVFE_Audio_Set_SUNSUR_Config(TVFE_CMD.Set_SUNSUR_Config.SUNSUR_Param);

			return ret;
		}

		case DSP_TVFE_Audio_Set_HEAAC_Output: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_Audio_Set_HEAAC_Output\n");
			ret = TVFE_Audio_Set_HEAAC_Output(TVFE_CMD.Set_HEAAC_Output.OnOFF_flag);

			return ret;
		}

		case DSP_TVFE_Audio_Set_Spdif_ChannelStatus: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_Audio_Set_Spdif_ChannelStatus\n");
			ret = TVFE_Audio_Set_Spdif_ChannelStatus(TVFE_CMD.Set_Spdif_ChannelStatus.CGMS_type,
													TVFE_CMD.Set_Spdif_ChannelStatus.bit_res,
													TVFE_CMD.Set_Spdif_ChannelStatus.disc_type);

			return ret;
		}

		case DSP_TVFE_Audio_Set_MasterMuteStatus: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_Audio_Set_MasterMuteStatus\n");
			ret = TVFE_Audio_Set_MasterMuteStatus(TVFE_CMD.Set_MasterMuteStatus.mute);

			return ret;
		}

		case DSP_TVFE_Audio_Get_MasterMuteStatus: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));

			DSP_DBGMSG("DSP_TVFE_Audio_Get_MasterMuteStatus\n");
			ret = TVFE_Audio_Get_MasterMuteStatus(&TVFE_CMD.Get_MasterMuteStatus.mute);

			if(copy_to_user((TVFE_Audio_CMD_st *)arg,&TVFE_CMD, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			return ret;
		}

		case DSP_TVFE_Audio_Set_SourceType: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_Audio_Set_SourceType\n");
			ret = TVFE_Audio_Set_SourceType(TVFE_CMD.Set_SourceType.Param);

			return ret;
		}

		case DSP_TVFE_Audio_Set_AC3TransCoding: {
			DSP_DBGMSG("DSP_TVFE_Audio_Set_AC3TransCoding\n");
			ret = TVFE_Audio_Set_AC3TransCoding();
			return ret;
		}

		case DSP_TVFE_Audio_Initial: {
			DSP_DBGMSG("DSP_TVFE_Audio_Initial\n");
			ret = TVFE_Audio_Initial();
			return ret;
		}


		case DSP_TVFE_AudioPowerControl: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;

			DSP_DBGMSG("DSP_TVFE_AudioPowerControl\n");
			ret = TVFE_AudioPowerControl(TVFE_CMD.PowerControl.ePowerControl);

			return ret;
		}

		case DSP_TVFE_Audio_Set_SIF_Source: {
			DSP_DBGMSG("DSP_TVFE_Audio_Set_SIF_Source\n");
			ret = TVFE_Audio_Set_SIF_Source();
			return ret;
		}
		
		case DSP_TVFE_Audio_Set_Sound_Source: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;
			DSP_DBGMSG("DSP_TVFE_Audio_Set_Sound_Source\n");
			ret = TVFE_Audio_Set_Sound_Source(TVFE_CMD.Set_PLF_AVSourcePinConfig.PLF_AVSourcePinConfig);
			return ret;
		}
		
		case DSP_TVFE_Audio_Set_SIF_Connect: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;			
			DSP_DBGMSG("DSP_TVFE_Audio_Set_SIF_Connect\n");
			ret = TVFE_AudioSetSIFConnect(TVFE_CMD.SIFStandardMask.eSIFStandardMask);
			return ret;
		}
		case DSP_TVFE_Audio_Get_FrontEndParam: {

			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;			
			ret = TVFE_AudioGetFrontEndParam(&TVFE_CMD.Get_FrontEndParam.FrontEndParam);
			
			if(copy_to_user((TVFE_Audio_CMD_st *)arg,&TVFE_CMD, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;	
			
			DSP_DBGMSG("DSP_TVFE_Audio_Get_FrontEndParam: %d\n",TVFE_CMD.Get_FrontEndParam.FrontEndParam.eMethod);
			return ret;

		}

		case DSP_TVFE_Audio_Set_SIFHandler: {
			DSP_DBGMSG("DSP_TVFE_Audio_Set_SIFHandler\n");
			ret = TVFE_Audio_Set_SIFHandler();
			return ret;
		}

		case DSP_TVFE_SetTunerValueType: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;			
			DSP_DBGMSG("DSP_TVFE_SetTunerValueType\n");
			ret = TVFE_SetTunerValueType(TVFE_CMD.TunerSoundStd.eTunerSoundStd);
			return ret;
		}		

		case DSP_TVFE_SIF_SetCurrentMode: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;			
			DSP_DBGMSG("DSP_TVFE_SIF_SetCurrentMode\n");
			ret = TVFE_SIF_SetCurrentMode(TVFE_CMD.SIFMode.eSIFMode);
			return ret;
		}	


		case DSP_TVFE_GetTunerValueType: {

			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		
			ret = TVFE_GetTunerValueType(&TVFE_CMD.TunerSoundStd.eTunerSoundStd);
			
			if(copy_to_user((TVFE_Audio_CMD_st *)arg,&TVFE_CMD, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;	
			
			DSP_DBGMSG("DSP_TVFE_GetTunerValueType: %d\n",TVFE_CMD.TunerSoundStd.eTunerSoundStd);
			return ret;

		}
		
		case DSP_TVFE_GetSIFWorkingMode: {

			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		
			ret = TVFE_GetSIFWorkingMode(&TVFE_CMD.SIFMode.eSIFMode);
			
			if(copy_to_user((TVFE_Audio_CMD_st *)arg,&TVFE_CMD, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;	

			DSP_DBGMSG("DSP_TVFE_GetSIFWorkingMode: %d\n",TVFE_CMD.SIFMode.eSIFMode);
			return ret;

		}

		case DSP_TVFE_SetSifAvailableMode: {
			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
		   	if(copy_from_user(&TVFE_CMD, (TVFE_Audio_CMD_st *)arg, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;			
			DSP_DBGMSG("DSP_TVFE_SetSifAvailableMode\n");
			ret = TVFE_SetSifAvailableMode(TVFE_CMD.SIFAvailableMode.eSIFAvailableMode);
			return ret;
		}	
		

		case DSP_TVFE_GetSifAvailableMode: {

			memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));		
			ret = TVFE_GetSifAvailableMode(&TVFE_CMD.SIFAvailableMode.eSIFAvailableMode);			
			if(copy_to_user((TVFE_Audio_CMD_st *)arg,&TVFE_CMD, sizeof(TVFE_Audio_CMD_st)))
		 	  return -EFAULT;	

			DSP_DBGMSG("DSP_TVFE_GetSifAvailableMode: %d\n",TVFE_CMD.SIFAvailableMode.eSIFAvailableMode);
			return ret;

		}

	}

	return -ENOTTY;
}

static ssize_t dsp_read(struct file *file, char __user *buf, size_t count,
			  loff_t *f_pos)
{
	DSP_DEV *idev = pDSPDev;
	
	*f_pos += count;
	return count;
}


static void DSP_Codec_Status()
{
	UINT16 val = DSP_GetRiscParam0(RISC_MAIN_MODE);

	DSP_DBGMSG("curr run mode = 0x%04x\n", val);

	val |=  (1<<MODE_DEC1_ENABLE_BIT) ;
	DSP_SetRiscParam0(RISC_MAIN_MODE, val);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
}


static ssize_t dsp_write(struct file *file, const char __user *buf, size_t count,
			   loff_t *f_pos)
{
	DSP_DEV *idev = pDSPDev;
	int i,ret;
	u8 *tmp;
	
	tmp = (char *) kmalloc(count, GFP_KERNEL);
	if (!tmp)
		return -ENOMEM;

	/* get user data */
	if (copy_from_user(tmp, buf, count)) {
		kfree(tmp);
		return -EFAULT;
	}

#if 0
	printk(KERN_ALERT "tmp memory:\n");
	for(i=0;i<10;i++)
		printk(KERN_ALERT "count %d:%d ",i,*(tmp +i));
#endif

	*f_pos += count;
	DSP_DBGMSG("dsp_write: %p, count:0x%x, f_pos:0x%llx,offset:0x%llx\n",tmp,count,file->f_pos,*f_pos);

	DSP_BsBufFeed(BSBUF_ID_DEC1, tmp, count);

	// dsp mon for debug
	//DSP_PC_Status();
	//DSP_Codec_Status();

	kfree(tmp);
	return count;
}

static int dsp_open(struct inode *inode, struct file *file)
{
	DSP_DBGMSG("fun:%s\n",__func__);
	return 0;
}

static int dsp_release(struct inode *inode, struct file *file)
{
	DSP_DBGMSG("fun:%s\n",__func__);
	return 0;
}

static const struct file_operations dsp_fops = {
	.owner			= THIS_MODULE,
	.read			= dsp_read,		/* read dev system call */
	.write			= dsp_write,	/* write dev system call */
	.unlocked_ioctl = dsp_ioctl,	/* ioctl system call */
	.mmap			= dsp_mmap,     /* mmap system call */
	.open			= dsp_open,		/* open dev system call */
	.release		= dsp_release,  /* close dev system call */
};

/* DSP Hardware Initial */
int spv9200_dsp_hw_initial(void)
{
	int ret = 0;
	
	TVFE_Audio_Initial();	

	// for I2S FIFI enable
	AUDIF_Set_FIFOEnable();

	return ret;
}

// netlink reply message 
static void dsp_mon_reply(int pid, int uid, int gid, int seq, void *payload)
{
	struct sk_buff	*skb;
	struct nlmsghdr	*nlh;
	int size=strlen(payload)+1;
	int		len = NLMSG_SPACE(size);
	void		*data;
	int ret;

	skb = alloc_skb(len, GFP_ATOMIC);
	if (!skb)
		return;
	nlh= NLMSG_PUT(skb, pid, seq, 0, size);
	nlh->nlmsg_flags = 0;
	data=NLMSG_DATA(nlh);
	memcpy(data, payload, size);
	NETLINK_CB(skb).pid = 0;         /* from kernel */
	NETLINK_CB(skb).dst_group = 0;  /* unicast */
	ret=netlink_unicast(pDSPDev->dsp_netlink_sock, skb, pid, MSG_DONTWAIT);

	if (ret <0)
	{
		printk("send failed\n");
		return;
	}
	return;

	nlmsg_failure:			/* Used by NLMSG_PUT */
	if (skb)
		kfree_skb(skb);
}

/* netlink receive messages */
static void dsp_mon_receive(struct sk_buff  *skb)
{
	u_int	uid, pid, gid, seq, sid;
	void			*command;
	struct nlmsghdr *nlh;

	nlh = (struct nlmsghdr *)skb->data;
	pid  = NETLINK_CREDS(skb)->pid;
	gid  = NETLINK_CREDS(skb)->gid;
	uid  = NETLINK_CREDS(skb)->uid;
	sid  = NETLINK_CB(skb).sid;
	seq  = nlh->nlmsg_seq;
	command = NLMSG_DATA(nlh);
	//DSP_DBGMSG("mon command(%d):%s\n",strlen(command),(char *)command);
	printk("mon command(%d):%s\n",strlen(command),(char *)command);
	dsp_monCmd((char *)command);
	//dsp_mon_reply(pid,uid,gid,seq,reply_message);

	return 0;
}

/*  platform drvier probe */
static int spv9200_dsp_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct resource *mem;

	/* register device with kernel */
	ret = register_chrdev(DSP_MAJOR, DSP_DEVICE_NAME, &dsp_fops);
	if (ret) {
		DSP_DBGMSG("can't register device with kernel\n");
		return ret;
	}
	
	pDSPDev = (DSP_DEV *) kmalloc(sizeof(DSP_DEV), GFP_KERNEL);


	DSP_DBGMSG("module loaded\n");	
	
	/* remap the i/o region */
    mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!mem) {
        dev_err(&pdev->dev, "no mem resource?\n");
        ret = -ENODEV;
        goto err_release_data;
    }

	pDSPDev->virtual_size = (mem->end - mem->start) + 1;
	pDSPDev->phys_addr = mem->start;

	DSP_DBGMSG("mem->start: 0x%llx, mem->end:0x%llx\n",(unsigned long long)mem->start,(unsigned long long)mem->end);
	DSP_DBGMSG("DRAMB: 0x%lx, size:0x%lx\n",(unsigned long)pDSPDev->phys_addr,(unsigned long)pDSPDev->virtual_size);

	pDSPDev->virtual_mem_base = ioremap_nocache(mem->start, pDSPDev->virtual_size);
	if(pDSPDev->virtual_mem_base == NULL)
	{
		goto err_release_region;
	}

	DSP_DBGMSG("ioremap setup: 0x%lx extending 0x%lx bytes\n", (unsigned long)pDSPDev->virtual_mem_base, (unsigned long)pDSPDev->virtual_size);

	memset_io(pDSPDev->virtual_mem_base, 0, pDSPDev->virtual_size);

#if 0 /* test memory map */
	/* write */
	int i;
	for(i=0;i<100;i++)
	{
		writeb(i,pDSPDev->virtual_mem_base+i);
		DSP_DBGMSG("virtual mem write : 0x%lx, value: 0x%02x\n", (unsigned long)pDSPDev->virtual_mem_base+i, i);		
	}
	
	/* read */
	for(i=0;i<100;i++)
	{
		DSP_DBGMSG("virtual mem read : 0x%lx, value: 0x%02x\n", (unsigned long)pDSPDev->virtual_mem_base+i, (int)readb(pDSPDev->virtual_mem_base+i));		
	}
#endif

	/* DSP init*/
	gAUDBufAddr = pDSPDev->virtual_mem_base;

	spv9200_dsp_hw_initial();

	// for EVB board playback test
	// AUDIF_Init_Audio();

	/* DSP test tone */
	//AUDIF_TestTone_mode(0);
	//msleep(2000);

	/* phone out disable mute on demo board */
	AUDHW_Disable_Mute(GPIO_AMUTE_PIN);

	/* test code here */
	//printk(KERN_ALERT "gAudioAuxPath.Aux1Path = %x\n",gAudioAuxPath.Aux1Path);

	/* kthread_task for Audio_MainRoutine */
#if 1
	pDSPDev->dsp_kthread_task = kthread_run(Audio_MainRoutine, NULL, "audio_kthread");
	if (IS_ERR(pDSPDev->dsp_kthread_task )) {
		DSP_DBGMSG("Audio_MainRoutine: can't start kthread.\n");
		goto err_release_region;
	}		
#endif

	/* create netlink sock for mon command */
	pDSPDev->dsp_netlink_sock = netlink_kernel_create(&init_net, NETLINK_DSPMON, 0,dsp_mon_receive, NULL, THIS_MODULE);

	return ret;


err_release_region:
    release_mem_region(pDSPDev->phys_addr, pDSPDev->virtual_size);
err_release_data:
    kfree(pDSPDev);


	return ret;
}

/*  platform drvier remove */
static int spv9200_dsp_remove(struct platform_device *pdev)
{

	struct resource *spv9200_dsp_res;

	DSP_DBGMSG("fun:%s\n",__func__);

	/* release netlink sock */
	netlink_kernel_release(pDSPDev->dsp_netlink_sock);

#if 1
	/* stop Audio_MainRoutine kthread */
	kthread_stop(pDSPDev->dsp_kthread_task);
#endif

	/* phone out disable mute */
	AUDHW_Enable_Mute(GPIO_AMUTE_PIN);

	spv9200_dsp_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	DSP_DBGMSG("spv9200_dsp_res->start: 0x%llx, spv9200_dsp_res->end:0x%llx\n",(unsigned long long)spv9200_dsp_res->start,(unsigned long long)spv9200_dsp_res->end);

	/* delete the devices */
	DSP_DBGMSG("buffer at 0x%lx will be removed\n", (unsigned long)pDSPDev->virtual_mem_base);
	iounmap(pDSPDev->virtual_mem_base);

	unregister_chrdev(DSP_MAJOR, DSP_DEVICE_NAME);
	
	return 0;
}


/*  platform driver - suspend */
static int spv9200_dsp_suspend(struct platform_device *pdev, pm_message_t state)
{
	return 0;
}

/*  platform driver - resume */
static int spv9200_dsp_resume(struct platform_device *pdev)
{
	return 0;
}


static struct platform_driver spv9200_dsp_driver = {
	.probe      = spv9200_dsp_probe,
	.remove     = spv9200_dsp_remove,
	.driver     = {
		.name   = "spv9200-dsp",  // it has to the same as platform device name
		.owner  = THIS_MODULE,
	},
	.suspend	= spv9200_dsp_suspend,
	.resume		= spv9200_dsp_resume,
};


static int __init dsp_init(void)
{
    int ret;

	/* Register DSP platform driver */
	ret = platform_driver_register(&spv9200_dsp_driver);
	DSP_DBGMSG("platform_driver_register result = %d\n",ret);

	DSP_DBGMSG("DSP initial up\n");

    return ret;
	
}

static void __exit dsp_cleanup(void)
{
	
	DSP_DBGMSG("unloaded\n");

	platform_driver_unregister(&spv9200_dsp_driver);
	kfree(pDSPDev);
	return;
}
	
module_init(dsp_init);
module_exit(dsp_cleanup);
MODULE_AUTHOR("Mark Tseng mark.tseng@sunplus.com");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("SPV9200 DSP / SSD driver with TVFE API v1.0.0.0");


