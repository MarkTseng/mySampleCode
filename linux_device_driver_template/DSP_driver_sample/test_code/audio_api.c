/*
 * Sunplus Audio HAL API
 * Author : Mark Tseng
 * DOCUMENT: Audio HAL API with TVFE interface 
 * $Id: audio_api.c,v 0.1 2011/11/28 Mark Tseng Exp $
 */

static char const rcsid[] = "$Id: audio_api, v0.0.0.1 " __DATE__ " " __TIME__ " Mark Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <linux/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>		/* ioctl */
#include <signal.h>

#include "audio_api.h"

#define	 DSP_DEVNAME	"/dev/dsp"
#define	 DSP_MAJOR_NR	42
#define	 DSP_MINOR_NR	0

int bRunProcess = 1;
int DSPfd;

int DSPDEV_OPEN()
{
	int ret = 0;

	DSPfd = open(DSP_DEVNAME,O_RDWR);
	if(DSPfd < 0)
	{
		/* auto create dsp device node */
		ret = mknod(DSP_DEVNAME,S_IFCHR,makedev(DSP_MAJOR_NR,DSP_MINOR_NR));
		if(ret)
		{
			perror("mknod fail\n");
			exit(1);	
		}
		DSPfd = open(DSP_DEVNAME,O_RDWR);
	}
	return ret;
}


int DSPDEV_CLOSE()
{
	int ret = 0;
	ret = close(DSPfd);
	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_Output_Config()
Parameters:
            pOutput_Config->Output_Port
            pOutput_Config->Process_Path
            pOutput_Config->If_Channel_Swap
Returns:TVFE_Audio_Status_e
Description: This function is used to set output path by input parameters
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Output_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;
	
	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_Output_Config,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_Output_Config\n");
	}	

    return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_Output_Selection()
Parameters:
            pOutput_Selection->Process_Path
            pOutput_Selection->Selection
Returns:TVFE_Audio_Status_e
Description: This function is used to set input source of aux1 aux2
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Output_Selection(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_Output_Selection,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_Output_Selection\n");
	}	

    return ret;

}

/********************************************************************
Prototype: TVFE_Audio_Set_Volume()
Parameters:
            pTable:volume table address
            Size:the size of the volume table
            Index:the index of the volume table
            OffsetStep:the offset by the source
Returns:TVFE_Audio_Status_e
Description: This function is used to set master volume
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Volume(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_Volume,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_Volume\n");
	}	

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_Bass()
Parameters:
            pTable:bass table address
            Size:the size of the bass table
            Index:the index of the bass table
Returns:TVFE_Audio_Status_e
Description: This function is used to set bass
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Bass(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_Bass,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_Bass\n");
	}	

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_Treble()
Parameters:
            pTable:treble table address
            Size:the size of the treble table
            Index:the index of the treble table
Returns:TVFE_Audio_Status_e
Description: This function is used to set treble
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Treble(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_Treble,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_Treble\n");
	}	

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_Main_Equlizer_Config()
Parameters:
            pTable:equlizer table address
            Size:the size of the equlizer table
            Index:the index of the equlizer table
Returns:TVFE_Audio_Status_e
Description: This function is used to set equlizer
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Main_Equlizer_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_Main_Equlizer_Config,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_Main_Equlizer_Config\n");
	}	


	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Get_Main_Equlizer_Config()
Parameters:
            pTable:equlizer table address

Returns:TVFE_Audio_Status_e
Description: This function is used to get equlizer config
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Get_Main_Equlizer_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Get_Main_Equlizer_Config,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Get_Main_Equlizer_Config\n");
	}	

    return ret;

}
/********************************************************************
Prototype: TVFE_Audio_Set_Balance()
Parameters:
            pTable:balance table address
            Size:the size of the balance table
            LeftIndex:the left index of the balance table
            RightIndex:the right index of the balance table
Returns:TVFE_Audio_Status_e
Description: This function is used to set balance
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Balance(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_Balance,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_Balance\n");
	}	

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_AUX_Volume()
Parameters:
            pAUX_Volume_Config->Process_Path
            pAUX_Volume_Config->Volume
Returns:TVFE_Audio_Status_e
Description: This function is used to set aux1 volume and aux2 volume
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_AUX_Volume(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_AUX_Volume,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_AUX_Volume\n");
	}	

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_Effect_Mode()
Parameters:
            Effect_Mode:TVFE_Audio_Effect_e
Returns:TVFE_Audio_Status_e
Description: This function is used to set sound effect
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Effect_Mode(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_Effect_Mode,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_Effect_Mode\n");
	}	

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_AVL_Config()
Parameters:
            pAVL_Config->Mode
            pAVL_Config->Enable_Weight_Filter
            pAVL_Config->Enable_Peak_Mode_With_ClipManagement
            pAVL_Config->Enable_AVL_Func
Returns:TVFE_Audio_Status_e
Description: This function is used to set AVL
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_AVL_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_AVL_Config,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_AVL_Config\n");
	}	

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_Mute_Config()
Parameters:
            pMute_Config->pExternal_Mute_Control
            pMute_Config->Process_Path
            pMute_Config->Enable
            pMute_Config->Mute_Delay
Returns:TVFE_Audio_Status_e
Description: This function is used to set mute main, aux1 and aux2
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Mute_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_Mute_Config,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_Mute_Config\n");
	}	

	return ret;
}


/********************************************************************
Prototype: TVFE_Audio_Initial()
Parameters:void
Returns:TVFE_Audio_Status_e
Description: This function is used to initiate the audio module
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Initial(int Aud_fd)
{
	TVFE_Audio_Status_e  ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Initial,NULL);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Initial\n");
	}	

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_Level_Adjust_Config()
Parameters:
            pLevel_Adjust_Config->DEC_Adjust_Level
            pLevel_Adjust_Config->Mono_Adjust_Level
            pLevel_Adjust_Config->NICAM_Adjust_Level
            pLevel_Adjust_Config->SAP_Adjust_Level
Returns:TVFE_Audio_Status_e
Description: This function is used to set the level adjustment for DEC, Mono, NICAM, SAP
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Level_Adjust_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_Level_Adjust_Config,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_Level_Adjust_Config\n");
	}	

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_SIF_Over_Modulation_Config()
Parameters:
            pOver_Modulation_Config->Enable_Adapter
            pOver_Modulation_Config->Filter_BandWidth
            pOver_Modulation_Config->Level_Thershold
            pOver_Modulation_Config->Attenuation
Returns:TVFE_Audio_Status_e
Description: This function is used to set the over modulation of SIF path
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_SIF_Over_Modulation_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_SIF_Over_Modulation_Config,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_SIF_Over_Modulation_Config\n");
	}	

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_SIF_Detect_Config()
Parameters:
            pDetect_Config->Carrier1_Magnitude_Threshold
            pDetect_Config->MPX_Magnitude_Threshold_Low
            pDetect_Config->MPX_Magnitude_Threshold_Up
            pDetect_Config->SAP_Magnitude_Threshold_Low
            pDetect_Config->SAP_Magnitude_Threshold_Up
            pDetect_Config->Enable_Sub_Carrier_Detect
            pDetect_Config->EIAJ_Sub_Carrier_Magnitude_Threshold_Low
            pDetect_Config->EIAJ_Sub_Carrier_Magnitude_Threshold_Up
Returns:TVFE_Audio_Status_e
Description: This function is used to set detect level
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_SIF_Detect_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_SIF_Detect_Config,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_SIF_Detect_Config\n");
	}	

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_SPDIF_Output_Config()
Parameters:
           SPDIF_Output_Config
                0:TVFE_AUDIO_SPDIF_OFF
                1:TVFE_AUDIO_SPDIF_PCM
                2:TVFE_AUDIO_SPDIF_BITSTREAM
                3:TVFE_AUDIO_SPDIF_REENC
           SPDIF_SmpRate
Returns:TVFE_Audio_Status_e
Description: This function is used to set SPDIF output
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_SPDIF_Output_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_SPDIF_Output_Config,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_SPDIF_Output_Config\n");
	}	

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_AtvRamp()
Parameters:
            ramp_flag
            0:ramp off
            1:ramp on
Returns:TVFE_Audio_Status_e
Description: This function is used to on/off SIF Ramp output
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_AtvRamp(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_AtvRamp,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_AtvRamp\n");
	}	

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Lip_Sync_Config()
Parameters:
            Lip_Sync_Config.Enable
            1:enable
            0:disable
            Lip_Sync_Config.Delay
            0~252ms
Returns:TVFE_Audio_Status_e
Description: This function is used to set lip sync
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Lip_Sync_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Lip_Sync_Config,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Lip_Sync_Config\n");
	}	

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_PCM_Output_Phase_Config()
Parameters:
            Pcm_Output_Phase.main_phase
            0:TVFE_AUDIO_OUTPUT_PHASE_0
            1:TVFE_AUDIO_OUTPUT_PHASE_180
            Pcm_Output_Phase.aux1_phase
            0:TVFE_AUDIO_OUTPUT_PHASE_0
            1:TVFE_AUDIO_OUTPUT_PHASE_180
            Pcm_Output_Phase.aux2_phase
            0:TVFE_AUDIO_OUTPUT_PHASE_0
            1:TVFE_AUDIO_OUTPUT_PHASE_180
Returns:TVFE_Audio_Status_e
Description: This function is used to set the PCM phase of main aux1 aux2
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_PCM_Output_Phase_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_PCM_Output_Phase_Config,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_PCM_Output_Phase_Config\n");
	}	

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_SUNSUR_Config()
Parameters:
            SUNSUR_Param
Returns:TVFE_Audio_Status_e
Description: This function is used to set the gain of ambiance, reverb, direct in
             sunplus surround
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_SUNSUR_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_SUNSUR_Config,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_SUNSUR_Config\n");
	}	

	return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_HEAAC_Output()
Parameters:
            OnOFF_flag: on/off
Returns:TVFE_Audio_Status_e
Description: This function is used to enable/disable the output of digital stream in HEAAC format
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_HEAAC_Output(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_HEAAC_Output,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_HEAAC_Output\n");
	}	

    return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_Spdif_ChannelStatus()
Parameters:
     CGMS_type:
*	          |_bit2_    bit15
*	0 0       |   1        x   unlimited copies allowed
*	0 1       |   -        -    not used
*	1 0       |   0        0   one copy allowed
*	1 1       |   0        1   no copy allowed
*
*	bit_res
*	0: SPDIF/PCM output 16-bit
*	1: SPDIF/PCM output 24-bit
*
*	disc_type: byte1 category code
*	0: set CD
*	1: set DVD

Returns:TVFE_Audio_Status_e
Description: Set the SCMS signal of the SPDIF output and Copy Right flag
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_Spdif_ChannelStatus(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_Spdif_ChannelStatus,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_Spdif_ChannelStatus\n");
	}	

    return ret;
}



TVFE_Audio_Status_e TVFE_Audio_Set_MasterMuteStatus(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

    return ret;
}
/********************************************************************
Prototype: TVFE_Audio_Get_MasterMuteStatus()
Parameters:

Returns: gMasterMuteStatus status, 0:unmute, 1:mute
Description: This function report main path mute status
Restrictions:
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Get_MasterMuteStatus(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Get_MasterMuteStatus,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Get_MasterMuteStatus\n");
	}	

    return ret;
}

/********************************************************************
Prototype: TVFE_Audio_Set_AC3TransCoding()
Parameters:
Returns: TVFE_Audio_Status_e
Description:init encoder mode to avoid download encoder failed when mainprogram state changed.
Restrictions:call this function after TVFE_Audio_Initial()
********************************************************************/
TVFE_Audio_Status_e TVFE_Audio_Set_AC3TransCoding(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
	TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_AC3TransCoding,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_AC3TransCoding\n");
	}	

    return ret;
}


TVFE_Audio_Status_e TVFE_Audio_Set_SourceType(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_SourceType,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_SourceType\n");
	}	

    return ret;
}

TVFE_Audio_Status_e TVFE_AudioPowerControl(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_AudioPowerControl,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_AudioPowerControl\n");
	}	

    return ret;
}
#if 0
TVFE_Audio_Status_e TVFE_Audio_Set_SIF_Source(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_SIF_Source,NULL);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_SIF_Source\n");
	}	

    return ret;
}
#else
TVFE_Audio_Status_e TVFE_Audio_Set_Sound_Source(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_Sound_Source,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_Sound_Source\n");
	}	

    return ret;
}
#endif
TVFE_Audio_Status_e TVFE_AudioSetSIFConnect(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_SIF_Connect,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_AudioSetSIFConnect\n");
	}	

    return ret;
}

TVFE_Audio_Status_e TVFE_AudioGetFrontEndParam(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Get_FrontEndParam,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_AudioGetFrontEndParam\n");
	}	

    return ret;
}

TVFE_Audio_Status_e TVFE_Audio_Set_SIFHandler(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_Audio_Set_SIFHandler,NULL);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_Audio_Set_SIFHandler\n");
	}	

    return ret;
}

TVFE_Audio_Status_e TVFE_SetTunerValueType(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_SetTunerValueType,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_SetTunerValueType\n");
	}	

    return ret;
}

TVFE_Audio_Status_e TVFE_SIF_SetCurrentMode(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_SIF_SetCurrentMode,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_SIF_SetCurrentMode\n");
	}	

    return ret;
}

TVFE_Audio_Status_e TVFE_SetSifAvailableMode(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_SetSifAvailableMode,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_SetSifAvailableMode\n");
	}	

    return ret;
}

TVFE_Audio_Status_e TVFE_GetTunerValueType(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_GetTunerValueType,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_GetTunerValueType\n");
	}	

    return ret;
}

TVFE_Audio_Status_e TVFE_GetSIFWorkingMode(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_GetSIFWorkingMode,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_GetSIFWorkingMode\n");
	}	

    return ret;
}

TVFE_Audio_Status_e TVFE_GetSifAvailableMode(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config)
{
    TVFE_Audio_Status_e ret = TVFE_AUDIO_SUCCESS;

	ret = ioctl(Aud_fd,DSP_TVFE_GetSifAvailableMode,TVFE_Config);
	if(ret != TVFE_AUDIO_SUCCESS)
	{
		perror("TVFE_GetSifAvailableMode\n");
	}	

    return ret;
}






// TVFE operation HAL interface
TVFE_Operation_st AUDIO_Operation = {
	 .Audio_Set_Output_Config			= TVFE_Audio_Set_Output_Config,
	 .Audio_Set_Output_Selection		= TVFE_Audio_Set_Output_Selection,
	 .Audio_Set_Volume					= TVFE_Audio_Set_Volume,
	 .Audio_Set_Bass					= TVFE_Audio_Set_Bass,
	 .Audio_Set_Treble					= TVFE_Audio_Set_Treble,
	 .Audio_Set_Main_Equlizer_Config	= TVFE_Audio_Set_Main_Equlizer_Config,
	 .Audio_Get_Main_Equlizer_Config	= TVFE_Audio_Get_Main_Equlizer_Config,
	 .Audio_Set_Balance					= TVFE_Audio_Set_Balance,
	 .Audio_Set_AUX_Volume				= TVFE_Audio_Set_AUX_Volume,
	 .Audio_Set_Effect_Mode				= TVFE_Audio_Set_Effect_Mode,
	 .Audio_Set_AVL_Config				= TVFE_Audio_Set_AVL_Config,
	 .Audio_Set_Mute_Config				= TVFE_Audio_Set_Mute_Config,
	 .Audio_Initial						= TVFE_Audio_Initial,
	 .Audio_Set_Level_Adjust_Config		= TVFE_Audio_Set_Level_Adjust_Config,
	 .Audio_Set_SIF_Over_Modulation_Config	= TVFE_Audio_Set_SIF_Over_Modulation_Config,
	 .Audio_Set_SIF_Detect_Config		= TVFE_Audio_Set_SIF_Detect_Config,
	 .Audio_SPDIF_Output_Config			= TVFE_Audio_SPDIF_Output_Config,
	 .Audio_Set_AtvRamp					= TVFE_Audio_Set_AtvRamp,
	 .Audio_Lip_Sync_Config				= TVFE_Audio_Lip_Sync_Config,
	 .Audio_PCM_Output_Phase_Config		= TVFE_Audio_PCM_Output_Phase_Config,
	 .Audio_Set_SUNSUR_Config			= TVFE_Audio_Set_SUNSUR_Config,
	 .Audio_Set_HEAAC_Output			= TVFE_Audio_Set_HEAAC_Output,
	 .Audio_Set_Spdif_ChannelStatus		= TVFE_Audio_Set_Spdif_ChannelStatus,
	 .Audio_Set_MasterMuteStatus		= TVFE_Audio_Set_MasterMuteStatus,
	 .Audio_Get_MasterMuteStatus		= TVFE_Audio_Get_MasterMuteStatus,
	 .Audio_Set_AC3TransCoding			= TVFE_Audio_Set_AC3TransCoding,
	 .Audio_Set_SourceType				= TVFE_Audio_Set_SourceType,
};


/*************
 * Customer.c 
 *
 *************/
#define DTV_FALSE	0
#define DTV_TRUE	/*lint -save -e506 */ (Boolean)(!DTV_FALSE)

#define APP_TRUE    1
#define APP_FALSE   0

#define APP_PCB_AUDIO_VOLUME_TABLE_SIZE     101
#define APP_PCB_AUDIO_BASS_TABLE_SIZE       101
#define APP_PCB_AUDIO_TREBLE_TABLE_SIZE     101
#define APP_PCB_AUDIO_EQ_TABLE_SIZE         101
#define APP_PCB_AUDIO_BALANCE_TABLE_SIZE    101

typedef unsigned char       APP_BOOL;
#define APP_TRUE    1
#define APP_FALSE   0

typedef enum _APP_PCB_Source_t
{
    APP_PCB_SOURCE_TUNER0 = 0,
    APP_PCB_SOURCE_CVBS0,
    APP_PCB_SOURCE_YPBPR0,
    APP_PCB_SOURCE_VGA0,
    APP_PCB_SOURCE_HDMI0,
    APP_PCB_SOURCE_MULTIMEDIA,
    APP_PCB_SOURCE_MAX,
} APP_PCB_Source_t;

typedef enum _APP_AudioSoundMode_t
{
    APP_AUDIO_MODE_STANDARD = 0,
    APP_AUDIO_MODE_MOVIE,
    APP_AUDIO_MODE_MUSIC,
    APP_AUDIO_MODE_SPEECH,
    APP_AUDIO_MODE_FAVORITE,
    APP_AUDIO_MODE_MAX,
} APP_AudioSoundMode_t;

typedef enum _APP_PCB_Audio_OutputTarget_t
{
    APP_PCB_AUDIO_OUTPUT_START = 0,
    APP_PCB_AUDIO_OUTPUT_VIA_SPEAKER = APP_PCB_AUDIO_OUTPUT_START,
    APP_PCB_AUDIO_OUTPUT_VIA_RCA_JACK,
    APP_PCB_AUDIO_OUTPUT_VIA_RCA_JACK_1,
    APP_PCB_AUDIO_OUTPUT_VIA_HEADPHONE,
    APP_PCB_AUDIO_OUTPUT_MAX,
} APP_PCB_Audio_OutputTarget_t;

/*
 ** Max platform config.
 ** Do not edit these enums and they are coworked with kconfig.
 */
typedef enum
{
    SOURCE_ATV = 0,
    SOURCE_DTV,
    SOURCE_RADIO,
    SOURCE_SCART,
    SOURCE_SCART1,
    SOURCE_AV,
    SOURCE_AV1,
    SOURCE_AV2,
    SOURCE_SVIDEO,
    SOURCE_SVIDEO1,
    SOURCE_SVIDEO2,
    SOURCE_YPBPR,
    SOURCE_YPBPR1,
    SOURCE_YPBPR2,
    SOURCE_HDMI,
    SOURCE_HDMI1,
    SOURCE_HDMI2,
    SOURCE_PC,
    SOURCE_MEDIA,
    SOURCE_KARAOKE,
    SOURCE_NET,
    SOURCE_DVD,
    SOURCE_MAX,
} eSource_Type_t;

typedef enum {
     AL_EQ_BAND_100HZ,
     AL_EQ_BAND_300HZ,
     AL_EQ_BAND_1000HZ,
     AL_EQ_BAND_3000HZ,
     AL_EQ_BAND_8000HZ,
     AL_EQ_BAND_MAX,
} AL_EQBand_t;

typedef enum _APP_Audio_SPDIF_Output_Config_e
{
    APP_AUDIO_SPDIF_OFF	= 0,
    APP_AUDIO_SPDIF_PCM,
    APP_AUDIO_SPDIF_BITSTREAM,
    APP_AUDIO_SPDIF_REENC,
    APP_AUDIO_SPDIF_IECRX,
} APP_Audio_SPDIF_Output_Config_e;

typedef enum _APP_Audio_SPDIF_Output_SmpRate_Config_e
{
    APP_AUDIO_SPDIF_Output_SmpRate_48K = 0,
    APP_AUDIO_SPDIF_Output_SmpRate_32K,
    APP_AUDIO_SPDIF_Output_SmpRate_44K,
    APP_AUDIO_SPDIF_Output_SmpRate_96K,
    APP_AUDIO_SPDIF_Output_SmpRate_192K,
} APP_Audio_SPDIF_Output_SmpRate_Config_e;

typedef enum _APP_Audio_Mode_e
{
    APP_AUDIO_MODE_NO_SIGNALE	= 0,
    APP_AUDIO_MODE_SIGNAL_DETECT,
    APP_AUDIO_MODE_CH_SEARCH,
    APP_AUDIO_MODE_CH_CHANGE,
    APP_AUDIO_MODE_STATEMAX,
} APP_Audio_Mode_e;

typedef struct _APP_PCB_AudioOutputConfig_t
{
	APP_PCB_Audio_OutputTarget_t Target;
	TVFE_Audio_Output_Config_t OutputConfig;
	TVFE_Audio_Output_Selection_e OutputSelection;
} APP_PCB_AudioOutputConfig_t;

// Note: For EEPROM
typedef struct _APP_AudioSoundModeSetting_t
{
    UINT8 Bass;
    UINT8 Treble;
} APP_AudioSoundModeSetting_t;

//Audio related tables
APP_AudioSoundModeSetting_t g_AudioSoundModeSetting[APP_AUDIO_MODE_MAX - 1] =
{
    // Bass,    Treble
    {  50,      50},    // APP_AUDIO_MODE_STANDARD
    {  100,    100},    // APP_AUDIO_MODE_MOVIE
    { 100,       0},    // APP_AUDIO_MODE_MUSIC
    {   0,     100},    // APP_AUDIO_MODE_SPEECH
};

TVFE_Audio_Equalizer_Config_t g_AudioSoundModeEqualizerSetting[APP_AUDIO_MODE_MAX] =
{
    // Enable_Bypass,   Band1_0100HZ,   Band2_0300HZ,   Band3_1000HZ,   Band4_3000HZ    Band5_8000HZ
    {APP_FALSE,          0,              0,              0,              0,              0},  // APP_AUDIO_MODE_STANDARD
    {APP_FALSE,          8,              8,              4,              -2,             3},  // APP_AUDIO_MODE_MOVIE
    {APP_FALSE,          0,              5,              0,              4,              7},  // APP_AUDIO_MODE_MUSIC
    {APP_FALSE,          0,              5,              7,              3,              3},  // APP_AUDIO_MODE_SPEECH
    {APP_FALSE,          0,              0,              0,              0,              0},  // APP_AUDIO_MODE_FAVORITE
};

TVFE_Audio_Equalizer_Config_t g_AudioSoundModeEqualizerSetting_Surround_ON[APP_AUDIO_MODE_MAX] =
{
    // Enable_Bypass,   Band1_0100HZ,   Band2_0300HZ,   Band3_1000HZ,   Band4_3000HZ    Band5_8000HZ
    {APP_FALSE,          -12,            -12,            0,              -2,             3},  // APP_AUDIO_MODE_STANDARD
    {APP_FALSE,          -12,            -12,            4,              -2,             3},  // APP_AUDIO_MODE_MOVIE
    {APP_FALSE,          -12,            -12,            0,              4,              7},  // APP_AUDIO_MODE_MUSIC
    {APP_FALSE,          -12,            -12,            7,              3,              3},  // APP_AUDIO_MODE_SPEECH
    {APP_FALSE,          -12,            -12,            0,              -2,             3},  // APP_AUDIO_MODE_FAVORITE
};

TVFE_Audio_Equalizer_Config_t* EQTablePointer = g_AudioSoundModeEqualizerSetting;

static const INT8 _APP_PCB_AudioVolumeOffset[] =
{
    24,//SOURCE_ATV
    -51,//SOURCE_DTV
    -51,//SOURCE_RADIO
    -6,//SOURCE_SCART
    -6,//SOURCE_SCART1
    -6,//SOURCE_AV
    -6,//SOURCE_SVIDEO
    -6,//SOURCE_SVIDEO1
    -6,//SOURCE_YPBPR
    -6,//SOURCE_YPBPR1
    -51,//SOURCE_HDMI
    -51,//SOURCE_HDMI1
    -51,//SOURCE_HDMI2
    -6,//SOURCE_PC
    -16,//SOURCE_MEDIA
    #ifdef NET_SUPPORT
	-51,//SOURCE_NET
	#endif
};

#if 0
static const INT8 _APP_PCB_AudioAuxiliaryVolumeOffset[APP_PCB_SOURCE_MAX] =
{
    2,//42, //APP_FULL_SOURCE_TUNER0
    2,//65, //APP_FULL_SOURCE_CVBS0
    2,//63, //APP_FULL_SOURCE_YPBPR0
    2,//63, //APP_FULL_SOURCE_VGA0
    2,//21, //APP_FULL_SOURCE_HDMI0
    20,//34, //APP_FULL_SOURCE_MULTIMEDIA
};
#endif
static const INT32 _APP_PCB_AudioAuxiliaryVolumeOffset[APP_PCB_SOURCE_MAX] =
{
    16,//42, //APP_FULL_SOURCE_TUNER0
    16,//65, //APP_FULL_SOURCE_CVBS0
    16,//63, //APP_FULL_SOURCE_YPBPR0
    16,//63, //APP_FULL_SOURCE_VGA0
    16,//21, //APP_FULL_SOURCE_HDMI0
    160,//34, //APP_FULL_SOURCE_MULTIMEDIA
};

static const INT32 _APP_PCB_AudioAuxiliaryOffset[2][PLF_INPUT_PIN_TYPE_MAX] =
{//NULL,TUNER,CVBS,SVIDEO,YPBPR,RGB,HDMI,DVI,SCART_AV,SCART_AV,SCART_AV_RGB,DTV,STORAGE,CCIR656,VIF,SIF,AUDIO_ADC,I2S,SPDIF
  {   0,   23,  23,    23,   23, 23,   0,  0,      23,      23,          23,  0,      0,      0,  0, 23,        0,  0,    0}, // follow current source
  {   0,   23,  23,    23,   23, 23,  23, 23,      23,      23,          23,  0,     23,     23, 23, 23,       23, 23,   23} // follow TV
};

static const APP_PCB_AudioOutputConfig_t _APP_PCB_AudioOutputConfigTable[] =
{
    {APP_PCB_AUDIO_OUTPUT_VIA_SPEAKER,	{TVFE_AUDIO_OUTPUT_DAC0,	TVFE_AUDIO_MAIN_PATH,			TVFE_FALSE},	TVFE_AUDIO_OUTPUT_CURRENT_SOURCE	},
    {APP_PCB_AUDIO_OUTPUT_VIA_RCA_JACK,	{TVFE_AUDIO_OUTPUT_DAC2,  TVFE_AUDIO_AUXILIARY_1_PATH,	TVFE_FALSE},	TVFE_AUDIO_OUTPUT_CURRENT_SOURCE	},
    {APP_PCB_AUDIO_OUTPUT_VIA_RCA_JACK_1,	{TVFE_AUDIO_OUTPUT_DAC2,  TVFE_AUDIO_AUXILIARY_1_PATH,	TVFE_FALSE},	TVFE_AUDIO_OUTPUT_ANALOG_TV	},

};

static INT16 _APP_PCB_AudioVolumeTable[APP_PCB_AUDIO_VOLUME_TABLE_SIZE] =
{
    //1 step 0.125 dB ,min = -672(-84 dB,mute),max = 192(+ 24 dB)
	-672,  -499,  -419,  -367,  -338,  -321,  -302,  -289,  -273,  -259,
	-243,  -230,  -211,  -198,  -182,  -163,  -148,  -138,  -129,  -120,
	-112,  -105,   -99,   -93,   -87,   -81,   -75,   -70,   -65,   -60,
	 -55,   -51,   -47,   -43,   -39,   -35,   -32,   -29,   -26,   -23,
	 -20,   -18,   -16,   -14,   -12,   -10,    -8,    -6,    -4,    -2,
	   0,     2,     4,     6,     8,    10,    12,    13,    14,    15,
	  16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
	  26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
	  36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
	  46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
	  56,
};

static const INT8 _APP_PCB_AudioBassTable[APP_PCB_AUDIO_BASS_TABLE_SIZE]=
{
    //1 step 1db, min = -8(-8 dB), max = 8(+8 dB)
	-8,	-8,	-8,	-8,	-8,	-8,	-7,	-7,	-7,	-7,
	-7, -7, -6, -6, -6, -6, -6, -6, -5, -5,
	-5,	-5,	-5,	-5,	-4,	-4,	-4,	-4,	-4,	-4,
	-3,	-3,	-3,	-3,	-3,	-3,	-2,	-2,	-2,	-2,
	-2,	-2,	-1,	-1,	-1,	-1,	-1,	-1,	-0,	-0,
	 0,	 0,	 0,	 0,	 1,	 1,	 1,	 1,	 1,	 1,
	 2,	 2,	 2,	 2,	 2,	 3,	 3,	 3,	 3,	 3,
	 3,	 4,	 4,  4,	 4,	 4,	 4,	 5,	 5,	 5,
	 5,	 5,	 5,	 6,	 6,	 6,	 6,	 6,	 6,	 7,
	 7,	 7,  7,  7,  7,  8,  8,  8,  8,  8,
	 8,

};

static const INT8 _APP_PCB_AudioTrebleTable[APP_PCB_AUDIO_TREBLE_TABLE_SIZE]=
{
    //1 step 1db, min = -8(-8 dB), max = 8(+8 dB)
	-8,	-8,	-8,	-8,	-8,	-8,	-7,	-7,	-7,	-7,
	-7, -7, -6, -6, -6, -6, -6, -6, -5, -5,
	-5,	-5,	-5,	-5,	-4,	-4,	-4,	-4,	-4,	-4,
	-3,	-3,	-3,	-3,	-3,	-3,	-2,	-2,	-2,	-2,
	-2,	-2,	-1,	-1,	-1,	-1,	-1,	-1,	-0,	-0,
	 0,	 0,	 0,	 0,	 1,	 1,	 1,	 1,	 1,	 1,
	 2,	 2,	 2,	 2,	 2,	 3,	 3,	 3,	 3,	 3,
	 3,	 4,	 4,  4,	 4,	 4,	 4,	 5,	 5,	 5,
	 5,	 5,	 5,	 6,	 6,	 6,	 6,	 6,	 6,	 7,
	 7,	 7,  7,  7,  7,  8,  8,  8,  8,  8,
	 8,
};

static const INT8 _APP_PCB_AudioEQTable[APP_PCB_AUDIO_EQ_TABLE_SIZE]=
{
    //1 step 1db, min = -12(-12 dB), max = 12(+12 dB)
     -12,    -12,    -12,   -12,    -11,     -11,    -11,    -11,    -10,    -10,
     -10,    -10,    -9,     -9,     -9,     -9,     -8,     -8,     -8,     -8,
     -7,     -7,     -7,     -7,     -6,     -6,     -6,     -6,     -5,     -5,
     -5,     -5,     -4,     -4,     -4,     -4,     -3,     -3,     -3,     -3,
     -2,     -2,     -2,     -2,     -1,     -1,     -1,     -1,      0,      0,
      0,      0,      1,      1,      1,      1,      2,      2,      2,      2,
      3,      3,      3,      3,      4,      4,      4,      4,      5,      5,
      5,      5,      6,      6,      6,      6,      7,      7,      7,      7,
      8,      8,      8,      8,      9,      9,      9,      9,     10,     10,
     10,     10,     11,     11,     11,     11,     12,     12,     12,     12,
     12,

};

static const INT8 _APP_PCB_AudioBalanceTable[APP_PCB_AUDIO_BALANCE_TABLE_SIZE]=
{
    //1 step 1db, min = -84(-84 dB, mute), max = 24(+24 dB)
	  0,	  0,	  0,	  0,	  0,	  0,	  0,	  0,	  0,	  0,
	  0,	  0,	  0,	  0,	  0,	  0,	  0,	  0,	  0,	  0,
	  0,	  0,	  0,	  0,	  0,	  0,	  0,	  0,	  0,	  0,
	  0,	  0,	  0,	  0,	  0,	  0,	  0,	  0,	  0,	  0,
	  0,	  0,	  0,	  0,	  0,	  0,	  0,	  0,	  0,	  0,
	  0,	  0,	 -2,	 -2,	 -3,	 -3,	 -4,	 -4,	 -5,	 -5,
	 -6,	 -6,	 -7,	 -7,	 -8,	 -8,	 -9,	 -9,	-10,	-10,
	-12,	-12,	-14,	-14,	-16,	-16,	-18,	-18,	-20,	-20,
	-22,	-22,	-24,	-24,	-26,	-26,	-28,	-28,	-30,	-30,
	-32,	-32,	-36,	-36,	-42,	-42,	-48,	-48,	-60,	-60,
	-84,
};

#define APP_PCB_AUDIO_CARRIER1_MAGNITUDE_THRESHOLD_DEFAULT				(-8)	//range : -16 ~ 15
#define APP_PCB_AUDIO_MPX_MAGNITUDE_THRESHOLD_LOW_DEFAULT				(9)   	//range : 0 ~ 15
#define APP_PCB_AUDIO_MPX_MAGNITUDE_THRESHOLD_UP_DEFAULT				(3)   	//range : 0 ~ 15
#define APP_PCB_AUDIO_SAP_MAGNITUDE_THRESHOLD_LOW_DEFAULT				(15)   	//range : 0 ~ 15
#define APP_PCB_AUDIO_SAP_MAGNITUDE_THRESHOLD_UP_DEFAULT				(12)   	//range : 0 ~ 15
#define APP_PCB_AUDIO_ENABLE_SUB_CARRIER_DETECT_DEFAULT					(DTV_FALSE)   //DTV_TRUE : enable EIAJ sub carrier detect, DTV_FALSE : disable EIAJ sub carrier detect
#define APP_PCB_AUDIO_EIAJ_SUB_CARRIER_MAGNITUDE_THRESHOLD_LOW_DEFAULT	(12)   	//range : 0 ~ 15
#define APP_PCB_AUDIO_EIAJ_SUB_CARRIER_MAGNITUDE_THRESHOLD_UP_DEFAULT	(8)	   	//range : 0 ~ 15

TVFE_Audio_SIF_Detect_Config_t _APP_PCB_Audio_SIF_Detect_Config =
{
	.Carrier1_Magnitude_Threshold = APP_PCB_AUDIO_CARRIER1_MAGNITUDE_THRESHOLD_DEFAULT,
	.MPX_Magnitude_Threshold_Low = APP_PCB_AUDIO_MPX_MAGNITUDE_THRESHOLD_LOW_DEFAULT,
	.MPX_Magnitude_Threshold_Up = APP_PCB_AUDIO_MPX_MAGNITUDE_THRESHOLD_UP_DEFAULT,
	.SAP_Magnitude_Threshold_Low = APP_PCB_AUDIO_SAP_MAGNITUDE_THRESHOLD_LOW_DEFAULT,
	.SAP_Magnitude_Threshold_Up = APP_PCB_AUDIO_SAP_MAGNITUDE_THRESHOLD_UP_DEFAULT,
	.Enable_Sub_Carrier_Detect = APP_PCB_AUDIO_ENABLE_SUB_CARRIER_DETECT_DEFAULT,
	.EIAJ_Sub_Carrier_Magnitude_Threshold_Low = APP_PCB_AUDIO_EIAJ_SUB_CARRIER_MAGNITUDE_THRESHOLD_LOW_DEFAULT,
	.EIAJ_Sub_Carrier_Magnitude_Threshold_Up = APP_PCB_AUDIO_EIAJ_SUB_CARRIER_MAGNITUDE_THRESHOLD_UP_DEFAULT,
};



/***** APP_XXX function *****/

/********************************************************************
Prototype: APP_PCB_GetAudioOutputConfig()
Parameters:

Returns:
Description: Get output path via speak, RCA_Jack and headphone
Restrictions:
********************************************************************/
const APP_PCB_AudioOutputConfig_t* APP_PCB_GetAudioOutputConfig(APP_PCB_Audio_OutputTarget_t target)
{
	UINT8 Index = 0;

	for (Index = 0; Index < (sizeof(_APP_PCB_AudioOutputConfigTable) / sizeof(APP_PCB_AudioOutputConfig_t)); Index++)
	{
		if (_APP_PCB_AudioOutputConfigTable[Index].Target == target)
		{
			break;
		}
	}

	return &(_APP_PCB_AudioOutputConfigTable[Index]);
}

/********************************************************************
Prototype: _APP_PCB_SetAudioOutputConfig()
Parameters:

Returns:
Description: Configure the output path via speak, RCA_Jack and headphone
Restrictions:
********************************************************************/
void _APP_PCB_SetAudioOutputConfig(void)
{
    const APP_PCB_AudioOutputConfig_t* pAudioOutputConfig;
    APP_PCB_Audio_OutputTarget_t index = 0;
	TVFE_Audio_CMD_st TVFE_Config;
	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));

    for (index = APP_PCB_AUDIO_OUTPUT_START; index < APP_PCB_AUDIO_OUTPUT_MAX; index++)
    {
        pAudioOutputConfig = APP_PCB_GetAudioOutputConfig(index);
		TVFE_Config.Set_Output_Config.Output_Config = pAudioOutputConfig->OutputConfig;
        TVFE_Audio_Set_Output_Config(DSPfd,&TVFE_Config);
    }

}

/********************************************************************
Prototype: APP_PCB_Audio_Initial()
Parameters: Param: no used

Returns: APP_TRUE;APP_FALSE
Description: 1. initiate the audio module
                 2. Configure the output path
                 3. Configure sif detect setting
Restrictions:
********************************************************************/
APP_BOOL APP_PCB_Audio_Initial(UINT32 Param)
{
	APP_BOOL IsSuccess = APP_TRUE;
	TVFE_Audio_CMD_st TVFE_Config;

	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));

	//APP_PCB_AUDIO_DBG("APP_PCB_Audio_Initial Param = %d\n",Param);

    /* Audio Initial */

	// DSP driver will initial this function
    //TVFE_Audio_Initial();

	/*configure audio path*/
	_APP_PCB_SetAudioOutputConfig();

    #if 0
    TVFE_Audio_Level_Adjust_t Audio_Level_Param;
    TVFE_Audio_SIF_Over_Modulation_Config_t pOver_Modulation_Config;

    Audio_Level_Param.DEC_Adjust_Level		= 0;
    Audio_Level_Param.Mono_Adjust_Level		= 0;
    Audio_Level_Param.NICAM_Adjust_Level	= 0;
    Audio_Level_Param.SAP_Adjust_Level		= 0;
    Audio_Level_Param.ADC_Adjust_Level		= 0;
    Audio_Level_Param.Digital_Adjust_Level	= 0;
    TVFE_Audio_Set_Level_Adjust_Config(&Audio_Level_Param);

    pOver_Modulation_Config.Attenuation     = 0;
    pOver_Modulation_Config.Enable_Adapter  = APP_FALSE;
    pOver_Modulation_Config.Filter_BandWidth = 0;
    pOver_Modulation_Config.Level_Thershold = 0;
    TVFE_Audio_Set_SIF_Over_Modulation_Config( &pOver_Modulation_Config);
    #endif
	
	TVFE_Config.Set_SIF_Detect_Config.Detect_Config = _APP_PCB_Audio_SIF_Detect_Config;
	TVFE_Audio_Set_SIF_Detect_Config(DSPfd,&TVFE_Config);

//	APP_Func_Audio_Initial();

	return IsSuccess;
}


/********************************************************************
Prototype: APP_PCB_Audio_Set_Level_Adjust()
Parameters: Param: no used

Returns: APP_TRUE;APP_FALSE
Description: Set the offset of aux1 according to the source index
Restrictions:
********************************************************************/
APP_BOOL APP_PCB_Audio_Set_Level_Adjust(UINT32 Param)
{
	APP_BOOL IsSuccess = APP_TRUE;
	//TVFE_Audio_AUX_Volume_t AUX_Volume_Config;
	TVFE_Audio_CMD_st TVFE_Config;

	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));

	//AUX_Volume_Config.Process_Path = TVFE_AUDIO_AUXILIARY_1_PATH;
	//AUX_Volume_Config.Volume = _APP_PCB_AudioAuxiliaryVolumeOffset[Param];

	TVFE_Config.Set_AUX_Volume.AUX_Volume_Config.Process_Path = TVFE_AUDIO_AUXILIARY_1_PATH;
	TVFE_Config.Set_AUX_Volume.AUX_Volume_Config.Volume = _APP_PCB_AudioAuxiliaryVolumeOffset[Param];

    TVFE_Audio_Set_AUX_Volume(DSPfd,&TVFE_Config);

	return IsSuccess;
}



/********************************************************************
Prototype: APP_PCB_Audio_Set_Volume()
Parameters:
                 Param0: Index of volume table(0~100)
                 Param1: Source selection

Returns: APP_TRUE;APP_FALSE
Description: Set master volume
Restrictions:
********************************************************************/
APP_BOOL APP_PCB_Audio_Set_Volume(UINT32 Param0, UINT32 Param1)
{
	APP_BOOL IsSuccess = APP_TRUE;
	TVFE_Audio_CMD_st TVFE_Config;

	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));


	memcpy(TVFE_Config.Set_Volume.Table, _APP_PCB_AudioVolumeTable,sizeof(_APP_PCB_AudioVolumeTable));
	TVFE_Config.Set_Volume.Size = APP_PCB_AUDIO_VOLUME_TABLE_SIZE;
	TVFE_Config.Set_Volume.Index = Param0;
	TVFE_Config.Set_Volume.OffsetStep = _APP_PCB_AudioVolumeOffset[Param1];


    //TVFE_Audio_Set_Volume(APP_Audio_GetVolumeTable(), APP_PCB_AUDIO_VOLUME_TABLE_SIZE, Param0, _APP_PCB_AudioVolumeOffset[Param1]);
    TVFE_Audio_Set_Volume(DSPfd, &TVFE_Config);
	return IsSuccess;
}


TVFE_Audio_Equalizer_Config_t APP_Audio_GetAudioSoundModeEqualizerSetting(APP_AudioSoundMode_t Mode)
{
	return EQTablePointer[Mode];
}

APP_AudioSoundModeSetting_t APP_Audio_GetAudioSoundModeSetting(APP_AudioSoundMode_t Mode)
{
    return g_AudioSoundModeSetting[(UINT8)Mode];
}


/********************************************************************
Prototype: APP_PCB_Audio_Set_Audio_Mode()
Parameters:
                 Param0: Audio mode(0:standard, 1:movie, 2:music and 3:speech)
                 Param1: Bass value(0~100)
                 Param2: Treble value(0~100)

Returns: APP_TRUE;APP_FALSE
Description: set audio sound including bass, treble and EQ in each audio mode
Restrictions:
********************************************************************/
APP_BOOL APP_PCB_Audio_Set_Audio_Mode(UINT32 Param0, UINT32 Param1, UINT32 Param2)
{
	APP_BOOL IsSuccess = APP_TRUE;
    APP_AudioSoundModeSetting_t SoundSetting;
    TVFE_Audio_Equalizer_Config_t EqualizerSetting;
	TVFE_Audio_CMD_st TVFE_Bass_Config;
	TVFE_Audio_CMD_st TVFE_Treble_Config;
	TVFE_Audio_CMD_st TVFE_Equalizer_Config;

	memset(&TVFE_Bass_Config,0,sizeof(TVFE_Audio_CMD_st));
	memset(&TVFE_Treble_Config,0,sizeof(TVFE_Audio_CMD_st));
	memset(&TVFE_Equalizer_Config,0,sizeof(TVFE_Audio_CMD_st));

//    APP_DataBuffer_Get_CurrentSource(&OSD_Source);

    if (Param0 < APP_AUDIO_MODE_MAX - 1)
    {
        // Standard / Movie / Music / Speech
        SoundSetting = APP_Audio_GetAudioSoundModeSetting(Param0);
    }
    else
    {
        // Favorite (APP_AUDIO_MODE_MAX - 1)
		SoundSetting.Bass = Param1;
		SoundSetting.Treble = Param2;
    }

	memcpy(TVFE_Bass_Config.Set_Bass.Table,  _APP_PCB_AudioBassTable, sizeof(_APP_PCB_AudioBassTable));
	TVFE_Bass_Config.Set_Bass.Size = APP_PCB_AUDIO_BASS_TABLE_SIZE;
	TVFE_Bass_Config.Set_Bass.Index = SoundSetting.Bass;
	//TVFE_Audio_Set_Bass(_APP_PCB_AudioBassTable,APP_PCB_AUDIO_BASS_TABLE_SIZE,SoundSetting.Bass);
	TVFE_Audio_Set_Bass(DSPfd,&TVFE_Bass_Config);

	memcpy(TVFE_Treble_Config.Set_Treble.Table, _APP_PCB_AudioTrebleTable, sizeof(_APP_PCB_AudioTrebleTable));
	TVFE_Treble_Config.Set_Treble.Size = APP_PCB_AUDIO_TREBLE_TABLE_SIZE;
	TVFE_Treble_Config.Set_Treble.Index = SoundSetting.Treble;
	//TVFE_Audio_Set_Treble(_APP_PCB_AudioTrebleTable,APP_PCB_AUDIO_TREBLE_TABLE_SIZE,SoundSetting.Treble);
	TVFE_Audio_Set_Treble(DSPfd,&TVFE_Treble_Config);

    // Set Equalizer
    EqualizerSetting = APP_Audio_GetAudioSoundModeEqualizerSetting((APP_AudioSoundMode_t)Param0);

	TVFE_Equalizer_Config.Set_Main_Equlizer_Config.Table = EqualizerSetting;
	TVFE_Equalizer_Config.Set_Main_Equlizer_Config.Size = 1; 
	TVFE_Equalizer_Config.Set_Main_Equlizer_Config.Index = 0;
    //TVFE_Audio_Set_Main_Equlizer_Config(&EqualizerSetting, 1, 0);
	TVFE_Audio_Set_Main_Equlizer_Config(DSPfd,&TVFE_Equalizer_Config);

	return IsSuccess;
}



/********************************************************************
Prototype: APP_PCB_Audio_Set_Bass()
Parameters:
                 Param:bass table index(0~100)

Returns: APP_TRUE;APP_FALSE
Description: set bass by the bass table index
Restrictions:
********************************************************************/
APP_BOOL APP_PCB_Audio_Set_Bass(UINT32 Param)
{
	APP_BOOL IsSuccess = APP_TRUE;
	TVFE_Audio_CMD_st TVFE_Bass_Config;

	memset(&TVFE_Bass_Config,0,sizeof(TVFE_Audio_CMD_st));

	memcpy(TVFE_Bass_Config.Set_Bass.Table,  _APP_PCB_AudioBassTable, sizeof(_APP_PCB_AudioBassTable));
	TVFE_Bass_Config.Set_Bass.Size = APP_PCB_AUDIO_BASS_TABLE_SIZE;
	TVFE_Bass_Config.Set_Bass.Index = Param;
	//TVFE_Audio_Set_Bass(_APP_PCB_AudioBassTable,APP_PCB_AUDIO_BASS_TABLE_SIZE,Param);
	TVFE_Audio_Set_Bass(DSPfd,&TVFE_Bass_Config);

	return IsSuccess;
}



/********************************************************************
Prototype: APP_PCB_Audio_Set_Treble()
Parameters:
                 Param:treble table index(0~100)

Returns: APP_TRUE;APP_FALSE
Description: set treble by the treble table index
Restrictions:
********************************************************************/
APP_BOOL APP_PCB_Audio_Set_Treble(UINT32 Param)
{
	APP_BOOL IsSuccess = APP_TRUE;
	TVFE_Audio_CMD_st TVFE_Treble_Config;

	memset(&TVFE_Treble_Config,0,sizeof(TVFE_Audio_CMD_st));

	memcpy(TVFE_Treble_Config.Set_Treble.Table, _APP_PCB_AudioTrebleTable, sizeof(_APP_PCB_AudioTrebleTable));
	TVFE_Treble_Config.Set_Treble.Size = APP_PCB_AUDIO_TREBLE_TABLE_SIZE;
	TVFE_Treble_Config.Set_Treble.Index = Param;
	//TVFE_Audio_Set_Treble(_APP_PCB_AudioTrebleTable,APP_PCB_AUDIO_TREBLE_TABLE_SIZE,Param);
	TVFE_Audio_Set_Treble(DSPfd,&TVFE_Treble_Config);

	return IsSuccess;
}


/********************************************************************
Prototype: APP_PCB_Audio_Set_EQ()
Parameters:
                 Param:EQ table index(0~100)

Returns: APP_TRUE;APP_FALSE
Description: set bass by the EQ table index
Restrictions:
********************************************************************/
APP_BOOL APP_PCB_Audio_Set_MainEqulizer(UINT32 Param0,UINT32 Param1)
{
     APP_BOOL IsSuccess = APP_TRUE;
     TVFE_Audio_Equalizer_Config_t EqualizerSetting;
	 TVFE_Audio_CMD_st TVFE_Equalizer_Config;

	 memset(&TVFE_Equalizer_Config,0,sizeof(TVFE_Audio_CMD_st));


     TVFE_Audio_Get_Main_Equlizer_Config(DSPfd,&TVFE_Equalizer_Config);

     switch(Param0)
     {
          case AL_EQ_BAND_100HZ:
               EqualizerSetting.Band1_0100HZ = _APP_PCB_AudioEQTable[Param1];
               break;

          case AL_EQ_BAND_300HZ:
               EqualizerSetting.Band2_0300HZ = _APP_PCB_AudioEQTable[Param1];
               break;

          case AL_EQ_BAND_1000HZ:
               EqualizerSetting.Band3_1000HZ = _APP_PCB_AudioEQTable[Param1];
               break;

          case AL_EQ_BAND_3000HZ:
               EqualizerSetting.Band4_3000HZ = _APP_PCB_AudioEQTable[Param1];
               break;

          case AL_EQ_BAND_8000HZ:
               EqualizerSetting.Band5_8000HZ = _APP_PCB_AudioEQTable[Param1];
               break;

          default:
                    break;
     }
	 TVFE_Equalizer_Config.Set_Main_Equlizer_Config.Table = EqualizerSetting;
	 TVFE_Equalizer_Config.Set_Main_Equlizer_Config.Size = APP_PCB_AUDIO_EQ_TABLE_SIZE; 
	 TVFE_Equalizer_Config.Set_Main_Equlizer_Config.Index = 0;
     //TVFE_Audio_Set_Main_Equlizer_Config(&EqualizerSetting, APP_PCB_AUDIO_EQ_TABLE_SIZE, 0);
	 TVFE_Audio_Set_Main_Equlizer_Config(DSPfd,&TVFE_Equalizer_Config);

    return IsSuccess;
}



/********************************************************************
Prototype: APP_PCB_Audio_Set_Balance()
Parameters:
                 Param:balance table index(0~100)

Returns: APP_TRUE;APP_FALSE
Description: set balance by the balance table index
Restrictions:
********************************************************************/
APP_BOOL APP_PCB_Audio_Set_Balance(UINT32 Param)
{
	APP_BOOL IsSuccess = APP_TRUE;
	TVFE_Audio_CMD_st TVFE_Balance_Config;

	memset(&TVFE_Balance_Config,0,sizeof(TVFE_Audio_CMD_st));

	memcpy(TVFE_Balance_Config.Set_Balance.Table,_APP_PCB_AudioBalanceTable, sizeof(_APP_PCB_AudioBalanceTable));
	TVFE_Balance_Config.Set_Balance.Size = APP_PCB_AUDIO_BALANCE_TABLE_SIZE;
	TVFE_Balance_Config.Set_Balance.LeftIndex = Param;
	TVFE_Balance_Config.Set_Balance.RightIndex = (APP_PCB_AUDIO_BALANCE_TABLE_SIZE - Param -1);

	//TVFE_Audio_Set_Balance(_APP_PCB_AudioBalanceTable,APP_PCB_AUDIO_BALANCE_TABLE_SIZE,Param,(APP_PCB_AUDIO_BALANCE_TABLE_SIZE - Param -1));
	TVFE_Audio_Set_Balance(DSPfd, &TVFE_Balance_Config);

	return IsSuccess;
}


/********************************************************************
Prototype: APP_PCB_Audio_Set_Surround()
Parameters:
                 Param:
                    0:disable surround
                    1:enable surround

Returns: APP_TRUE;APP_FALSE
Description: enable/disable surround
Restrictions:
********************************************************************/
APP_BOOL APP_PCB_Audio_Set_Surround(UINT32 Param)
{
	APP_BOOL IsSuccess = APP_TRUE;
	TVFE_Audio_CMD_st TVFE_Config;

	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));

    if (Param == APP_TRUE)
    {
		TVFE_Config.Set_Effect_Mode.Effect_Mode = TVFE_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_HD_PASSIVE_MATRIX;
        //TVFE_Audio_Set_Effect_Mode(TVFE_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_HD_PASSIVE_MATRIX);
        TVFE_Audio_Set_Effect_Mode(DSPfd, &TVFE_Config);
    }
    else
    {
		TVFE_Config.Set_Effect_Mode.Effect_Mode = TVFE_AUDIO_EFFECT_MODE_NORMAL;
        //TVFE_Audio_Set_Effect_Mode(TVFE_AUDIO_EFFECT_MODE_NORMAL);
        TVFE_Audio_Set_Effect_Mode(DSPfd, &TVFE_Config);
    }

	return IsSuccess;
}

 

/********************************************************************
Prototype: APP_PCB_Audio_Set_Perfect_Sound()
Parameters:
                    Param:
                    0:disable AVL
                    1:enable AVL

Returns: APP_TRUE;APP_FALSE
Description: enable/disable AVL function setting:
                AVL mode: 2s
                AVL level: -20dB
                Weight filer: off
                Peak mode with clip management: off
Restrictions:
********************************************************************/
APP_BOOL APP_PCB_Audio_Set_Perfect_Sound(UINT32 Param)
{
	APP_BOOL IsSuccess = APP_TRUE;
	TVFE_Audio_AVL_Config_t AVLConfig;
	TVFE_Audio_CMD_st TVFE_Config;

	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));
	//APP_PCB_AUDIO_DBG("APP_PCB_Audio_Set_Perfect_Sound Param = %d\n",Param);

    if (Param == TVFE_TRUE)
    {
        AVLConfig.Mode = TVFE_AUDIO_AVL_MODE_02S;
	AVLConfig.Enable_AVL_Func = TVFE_TRUE;
    }
    else
    {
        AVLConfig.Mode = TVFE_AUDIO_AVL_MODE_OFF;
	AVLConfig.Enable_AVL_Func = TVFE_FALSE;
    }
    AVLConfig.Level = TVFE_AUDIO_AVL_REFERENCE_LEVEL_MINUS_20_DB;
    AVLConfig.Enable_Weight_Filter = TVFE_FALSE;
    AVLConfig.Enable_Peak_Mode_With_ClipManagement = TVFE_FALSE;

	TVFE_Config.Set_AVL_Config.AVL_Config = AVLConfig;
    //TVFE_Audio_Set_AVL_Config(&AVLConfig);
    TVFE_Audio_Set_AVL_Config(DSPfd, &TVFE_Config);

	return IsSuccess;
}


/********************************************************************
Prototype: APP_PCB_Audio_Set_Lip_Sync()
Parameters:
                Param0: func enable(0:Disable; 1:Enable)
                Param1: delay time(ms)

Returns: APP_TRUE;APP_FALSE
Description: enable/disable lip sync function and the delay time
Restrictions:
********************************************************************/
APP_BOOL APP_PCB_Audio_Set_Lip_Sync(UINT32 Param0, UINT32 Param1)
{
	APP_BOOL IsSuccess = APP_TRUE;
	TVFE_Audio_Lip_Sync_Config_t Lip_Sync_Config;
	TVFE_Audio_CMD_st TVFE_Config;

	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));
	
    if(Param0==0)
    {
	    Lip_Sync_Config.Enable = TVFE_FALSE;
    }
    else
    {
	    Lip_Sync_Config.Enable = TVFE_TRUE;
    }
	Lip_Sync_Config.Delay = (UINT16)Param1;
	TVFE_Config.Lip_Sync_Config.Lip_Sync_Config = Lip_Sync_Config;
    //TVFE_Audio_Lip_Sync_Config(Lip_Sync_Config);
    TVFE_Audio_Lip_Sync_Config(DSPfd, &TVFE_Config);
	return IsSuccess;

}


/********************************************************************
Prototype: APP_PCB_Audio_Set_Output_Selection()
Parameters:
                    eSrcVideoType

Returns: APP_TRUE;APP_FALSE
Description: Set the audio output via speaker according to video type
Restrictions:
********************************************************************/
APP_BOOL APP_PCB_Audio_Set_Output_Selection(PLF_AVType_e eSrcVideoType)
{
	APP_BOOL IsSuccess = APP_TRUE;
    const APP_PCB_AudioOutputConfig_t* pAudioOutputConfig;
    //TVFE_Audio_Output_Selection_t OutputSelection;
	TVFE_Audio_CMD_st TVFE_Config;

	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));

    pAudioOutputConfig = APP_PCB_GetAudioOutputConfig(APP_PCB_AUDIO_OUTPUT_VIA_SPEAKER);
    //OutputSelection.Output_Port = pAudioOutputConfig->OutputConfig.Output_Port;
    //OutputSelection.Process_Path = pAudioOutputConfig->OutputConfig.Process_Path;
    //OutputSelection.Selection = pAudioOutputConfig->OutputSelection;

	TVFE_Config.Set_Output_Selection.Output_Selection.Process_Path = pAudioOutputConfig->OutputConfig.Process_Path;
	TVFE_Config.Set_Output_Selection.Output_Selection.Selection = pAudioOutputConfig->OutputSelection;
	TVFE_Config.Set_Output_Selection.eSrcVideoType = eSrcVideoType;
    //TVFE_Audio_Set_Output_Selection(&OutputSelection,eSrcVideoType);
	TVFE_Audio_Set_Output_Selection(DSPfd, &TVFE_Config);	

    pAudioOutputConfig = APP_PCB_GetAudioOutputConfig(APP_PCB_AUDIO_OUTPUT_VIA_RCA_JACK);
    //OutputSelection.Output_Port = pAudioOutputConfig->OutputConfig.Output_Port;
    //OutputSelection.Process_Path = pAudioOutputConfig->OutputConfig.Process_Path;
    //OutputSelection.Selection = pAudioOutputConfig->OutputSelection;

	TVFE_Config.Set_Output_Selection.Output_Selection.Process_Path = pAudioOutputConfig->OutputConfig.Process_Path;
	TVFE_Config.Set_Output_Selection.Output_Selection.Selection = pAudioOutputConfig->OutputSelection;
	TVFE_Config.Set_Output_Selection.eSrcVideoType = eSrcVideoType;
    //TVFE_Audio_Set_Output_Selection(&OutputSelection,eSrcVideoType);
	TVFE_Audio_Set_Output_Selection(DSPfd, &TVFE_Config);	

    #if 0
    pAudioOutputConfig = APP_PCB_GetAudioOutputConfig(APP_PCB_AUDIO_OUTPUT_VIA_HEADPHONE);
    //OutputSelection.Output_Port = pAudioOutputConfig->OutputConfig.Output_Port;
    OutputSelection.Process_Path = pAudioOutputConfig->OutputConfig.Process_Path;
    OutputSelection.Selection = pAudioOutputConfig->OutputSelection;
    TVFE_Audio_Set_Output_Selection(&OutputSelection,eSrcVideoType);
    #endif
	return IsSuccess;
}

/********************************************************************
Prototype: APP_PCB_Audio_Set_SCART_Output()
Parameters:
                    Param0:
                    0:SCART output is current source
                    1:SCART output is analog TV
                    Source_Type: eSrcVideoType

Returns: APP_TRUE;APP_FALSE
Description: set the audio output via SCART out by the source type and scart type
Restrictions:
********************************************************************/
APP_BOOL APP_PCB_Audio_Set_SCART_Output(UINT32 Param0,PLF_AVType_e eSrcVideoType)
{
	APP_BOOL IsSuccess = APP_TRUE;
	const APP_PCB_AudioOutputConfig_t* pAudioOutputConfig;
	//TVFE_Audio_Output_Selection_t OutputSelection;
	//TVFE_Audio_AUX_Volume_t AUX_Volume_Config;

	TVFE_Audio_CMD_st TVFE_Config;

	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));

	if(Param0==0)
	{
		pAudioOutputConfig = APP_PCB_GetAudioOutputConfig(APP_PCB_AUDIO_OUTPUT_VIA_RCA_JACK);
		//OutputSelection.Process_Path = pAudioOutputConfig->OutputConfig.Process_Path;
		//OutputSelection.Selection = pAudioOutputConfig->OutputSelection;
		//TVFE_Audio_Set_Output_Selection(&OutputSelection,eSrcVideoType);
		
		TVFE_Config.Set_Output_Selection.Output_Selection.Process_Path = pAudioOutputConfig->OutputConfig.Process_Path;
		TVFE_Config.Set_Output_Selection.Output_Selection.Selection = pAudioOutputConfig->OutputSelection;
		TVFE_Config.Set_Output_Selection.eSrcVideoType = eSrcVideoType;
		TVFE_Audio_Set_Output_Selection(DSPfd, &TVFE_Config);	

		//AUX_Volume_Config.Volume = _APP_PCB_AudioAuxiliaryOffset[Param0][eSrcVideoType];
		//AUX_Volume_Config.Process_Path = pAudioOutputConfig->OutputConfig.Process_Path;
		//TVFE_Audio_Set_AUX_Volume(&AUX_Volume_Config);
		
		TVFE_Config.Set_AUX_Volume.AUX_Volume_Config.Volume = _APP_PCB_AudioAuxiliaryOffset[Param0][eSrcVideoType];
		TVFE_Config.Set_AUX_Volume.AUX_Volume_Config.Process_Path = pAudioOutputConfig->OutputConfig.Process_Path;
		TVFE_Audio_Set_AUX_Volume(DSPfd,&TVFE_Config);
		
	}
	else if(Param0==1)
	{
		pAudioOutputConfig = APP_PCB_GetAudioOutputConfig(APP_PCB_AUDIO_OUTPUT_VIA_RCA_JACK_1);
		//OutputSelection.Process_Path = pAudioOutputConfig->OutputConfig.Process_Path;
		//OutputSelection.Selection = pAudioOutputConfig->OutputSelection;
		//TVFE_Audio_Set_Output_Selection(&OutputSelection,eSrcVideoType);
		TVFE_Config.Set_Output_Selection.Output_Selection.Process_Path = pAudioOutputConfig->OutputConfig.Process_Path;
		TVFE_Config.Set_Output_Selection.Output_Selection.Selection = pAudioOutputConfig->OutputSelection;
		TVFE_Config.Set_Output_Selection.eSrcVideoType = eSrcVideoType;
		TVFE_Audio_Set_Output_Selection(DSPfd, &TVFE_Config);	

		//AUX_Volume_Config.Volume = _APP_PCB_AudioAuxiliaryOffset[Param0][eSrcVideoType];
		//AUX_Volume_Config.Process_Path = pAudioOutputConfig->OutputConfig.Process_Path;
		//TVFE_Audio_Set_AUX_Volume(&AUX_Volume_Config);
		
		TVFE_Config.Set_AUX_Volume.AUX_Volume_Config.Volume = _APP_PCB_AudioAuxiliaryOffset[Param0][eSrcVideoType];
		TVFE_Config.Set_AUX_Volume.AUX_Volume_Config.Process_Path = pAudioOutputConfig->OutputConfig.Process_Path;
		TVFE_Audio_Set_AUX_Volume(DSPfd,&TVFE_Config);

	}

	return IsSuccess;
}

/********************************************************************
Prototype: APP_PCB_Audio_Set_SPDIF_Output()
Parameters:
                    SPDIF_Output_Config
                        0: off
                        1: PCM
                        2: bitstream

                    SPDIF_SmpRate
                        0: 48K
                        1: 32K
                        2: 44K
                        3: 96K
                        4: 192K

Returns: APP_TRUE;APP_FALSE
Description: set the output format and sampling rate of SPDIF
Restrictions:
********************************************************************/
APP_BOOL APP_PCB_Audio_Set_SPDIF_Output(APP_Audio_SPDIF_Output_Config_e SPDIF_Output_Config,APP_Audio_SPDIF_Output_SmpRate_Config_e SPDIF_SmpRate)
{
	APP_BOOL IsSuccess = APP_TRUE;
	TVFE_Audio_CMD_st TVFE_Config;

	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));

    switch(SPDIF_Output_Config)
    {
        case APP_AUDIO_SPDIF_OFF:
            switch(SPDIF_SmpRate)
            {
                case APP_AUDIO_SPDIF_Output_SmpRate_48K:
					TVFE_Config.SPDIF_Output_Config.SPDIF_Output_Config = TVFE_AUDIO_SPDIF_OFF;
					TVFE_Config.SPDIF_Output_Config.SPDIF_SmpRate = TVFE_AUDIO_SPDIF_Output_SmpRate_48K;
                    //TVFE_Audio_SPDIF_Output_Config(TVFE_AUDIO_SPDIF_OFF,TVFE_AUDIO_SPDIF_Output_SmpRate_48K);
                    TVFE_Audio_SPDIF_Output_Config(DSPfd, &TVFE_Config);
                    break;
                case APP_AUDIO_SPDIF_Output_SmpRate_32K:
					TVFE_Config.SPDIF_Output_Config.SPDIF_Output_Config = TVFE_AUDIO_SPDIF_OFF;
					TVFE_Config.SPDIF_Output_Config.SPDIF_SmpRate = TVFE_AUDIO_SPDIF_Output_SmpRate_32K;
                    //TVFE_Audio_SPDIF_Output_Config(TVFE_AUDIO_SPDIF_OFF,TVFE_AUDIO_SPDIF_Output_SmpRate_32K);
                    TVFE_Audio_SPDIF_Output_Config(DSPfd, &TVFE_Config);
                    break;
                case APP_AUDIO_SPDIF_Output_SmpRate_44K:
					TVFE_Config.SPDIF_Output_Config.SPDIF_Output_Config = TVFE_AUDIO_SPDIF_OFF;
					TVFE_Config.SPDIF_Output_Config.SPDIF_SmpRate = TVFE_AUDIO_SPDIF_Output_SmpRate_44K;
                    //TVFE_Audio_SPDIF_Output_Config(TVFE_AUDIO_SPDIF_OFF,TVFE_AUDIO_SPDIF_Output_SmpRate_44K);
                    TVFE_Audio_SPDIF_Output_Config(DSPfd, &TVFE_Config);
                    break;
                case APP_AUDIO_SPDIF_Output_SmpRate_96K:
					TVFE_Config.SPDIF_Output_Config.SPDIF_Output_Config = TVFE_AUDIO_SPDIF_OFF;
					TVFE_Config.SPDIF_Output_Config.SPDIF_SmpRate = TVFE_AUDIO_SPDIF_Output_SmpRate_96K;
                    //TVFE_Audio_SPDIF_Output_Config(TVFE_AUDIO_SPDIF_OFF,TVFE_AUDIO_SPDIF_Output_SmpRate_96K);
                    TVFE_Audio_SPDIF_Output_Config(DSPfd, &TVFE_Config);
                    break;
                case APP_AUDIO_SPDIF_Output_SmpRate_192K:
					TVFE_Config.SPDIF_Output_Config.SPDIF_Output_Config = TVFE_AUDIO_SPDIF_OFF;
					TVFE_Config.SPDIF_Output_Config.SPDIF_SmpRate = TVFE_AUDIO_SPDIF_Output_SmpRate_192K;
                    //TVFE_Audio_SPDIF_Output_Config(TVFE_AUDIO_SPDIF_OFF,TVFE_AUDIO_SPDIF_Output_SmpRate_192K);
                    TVFE_Audio_SPDIF_Output_Config(DSPfd, &TVFE_Config);
                    break;
                default:
                    return APP_FALSE;
            }
            break;
        case APP_AUDIO_SPDIF_PCM:
            switch(SPDIF_SmpRate)
            {
                case APP_AUDIO_SPDIF_Output_SmpRate_48K:
					TVFE_Config.SPDIF_Output_Config.SPDIF_Output_Config = TVFE_AUDIO_SPDIF_PCM;
					TVFE_Config.SPDIF_Output_Config.SPDIF_SmpRate = TVFE_AUDIO_SPDIF_Output_SmpRate_48K;
                    //TVFE_Audio_SPDIF_Output_Config(TVFE_AUDIO_SPDIF_PCM,TVFE_AUDIO_SPDIF_Output_SmpRate_48K);
                    TVFE_Audio_SPDIF_Output_Config(DSPfd, &TVFE_Config);
                    break;
                case APP_AUDIO_SPDIF_Output_SmpRate_32K:
					TVFE_Config.SPDIF_Output_Config.SPDIF_Output_Config = TVFE_AUDIO_SPDIF_PCM;
					TVFE_Config.SPDIF_Output_Config.SPDIF_SmpRate = TVFE_AUDIO_SPDIF_Output_SmpRate_32K;
                    //TVFE_Audio_SPDIF_Output_Config(TVFE_AUDIO_SPDIF_PCM,TVFE_AUDIO_SPDIF_Output_SmpRate_32K);
                    TVFE_Audio_SPDIF_Output_Config(DSPfd, &TVFE_Config);
                    break;
                case APP_AUDIO_SPDIF_Output_SmpRate_44K:
					TVFE_Config.SPDIF_Output_Config.SPDIF_Output_Config = TVFE_AUDIO_SPDIF_PCM;
					TVFE_Config.SPDIF_Output_Config.SPDIF_SmpRate = TVFE_AUDIO_SPDIF_Output_SmpRate_44K;
                    //TVFE_Audio_SPDIF_Output_Config(TVFE_AUDIO_SPDIF_PCM,TVFE_AUDIO_SPDIF_Output_SmpRate_44K);
                    TVFE_Audio_SPDIF_Output_Config(DSPfd, &TVFE_Config);
                    break;
                case APP_AUDIO_SPDIF_Output_SmpRate_96K:
					TVFE_Config.SPDIF_Output_Config.SPDIF_Output_Config = TVFE_AUDIO_SPDIF_PCM;
					TVFE_Config.SPDIF_Output_Config.SPDIF_SmpRate = TVFE_AUDIO_SPDIF_Output_SmpRate_96K;
                    //TVFE_Audio_SPDIF_Output_Config(TVFE_AUDIO_SPDIF_PCM,TVFE_AUDIO_SPDIF_Output_SmpRate_96K);
                    TVFE_Audio_SPDIF_Output_Config(DSPfd, &TVFE_Config);
                    break;
                case APP_AUDIO_SPDIF_Output_SmpRate_192K:
					TVFE_Config.SPDIF_Output_Config.SPDIF_Output_Config = TVFE_AUDIO_SPDIF_PCM;
					TVFE_Config.SPDIF_Output_Config.SPDIF_SmpRate = TVFE_AUDIO_SPDIF_Output_SmpRate_192K;
                    //TVFE_Audio_SPDIF_Output_Config(TVFE_AUDIO_SPDIF_PCM,TVFE_AUDIO_SPDIF_Output_SmpRate_192K);
                    TVFE_Audio_SPDIF_Output_Config(DSPfd, &TVFE_Config);
                    break;
                default:
                    return APP_FALSE;
            }
            break;
        case APP_AUDIO_SPDIF_BITSTREAM:
            switch(SPDIF_SmpRate)
            {
                case APP_AUDIO_SPDIF_Output_SmpRate_48K:
					TVFE_Config.SPDIF_Output_Config.SPDIF_Output_Config = TVFE_AUDIO_SPDIF_BITSTREAM;
					TVFE_Config.SPDIF_Output_Config.SPDIF_SmpRate = TVFE_AUDIO_SPDIF_Output_SmpRate_48K;
                    //TVFE_Audio_SPDIF_Output_Config(TVFE_AUDIO_SPDIF_BITSTREAM,TVFE_AUDIO_SPDIF_Output_SmpRate_48K);
                    TVFE_Audio_SPDIF_Output_Config(DSPfd, &TVFE_Config);
                    break;
                case APP_AUDIO_SPDIF_Output_SmpRate_32K:
					TVFE_Config.SPDIF_Output_Config.SPDIF_Output_Config = TVFE_AUDIO_SPDIF_BITSTREAM;
					TVFE_Config.SPDIF_Output_Config.SPDIF_SmpRate = TVFE_AUDIO_SPDIF_Output_SmpRate_32K;
                    //TVFE_Audio_SPDIF_Output_Config(TVFE_AUDIO_SPDIF_BITSTREAM,TVFE_AUDIO_SPDIF_Output_SmpRate_32K);
                    TVFE_Audio_SPDIF_Output_Config(DSPfd, &TVFE_Config);
                    break;
                case APP_AUDIO_SPDIF_Output_SmpRate_44K:
					TVFE_Config.SPDIF_Output_Config.SPDIF_Output_Config = TVFE_AUDIO_SPDIF_BITSTREAM;
					TVFE_Config.SPDIF_Output_Config.SPDIF_SmpRate = TVFE_AUDIO_SPDIF_Output_SmpRate_44K;
                    //TVFE_Audio_SPDIF_Output_Config(TVFE_AUDIO_SPDIF_BITSTREAM,TVFE_AUDIO_SPDIF_Output_SmpRate_44K);
                    TVFE_Audio_SPDIF_Output_Config(DSPfd, &TVFE_Config);
                    break;
                case APP_AUDIO_SPDIF_Output_SmpRate_96K:
					TVFE_Config.SPDIF_Output_Config.SPDIF_Output_Config = TVFE_AUDIO_SPDIF_BITSTREAM;
					TVFE_Config.SPDIF_Output_Config.SPDIF_SmpRate = TVFE_AUDIO_SPDIF_Output_SmpRate_96K;
                    //TVFE_Audio_SPDIF_Output_Config(TVFE_AUDIO_SPDIF_BITSTREAM,TVFE_AUDIO_SPDIF_Output_SmpRate_96K);
                    TVFE_Audio_SPDIF_Output_Config(DSPfd, &TVFE_Config);
                    break;
                case APP_AUDIO_SPDIF_Output_SmpRate_192K:
					TVFE_Config.SPDIF_Output_Config.SPDIF_Output_Config = TVFE_AUDIO_SPDIF_BITSTREAM;
					TVFE_Config.SPDIF_Output_Config.SPDIF_SmpRate = TVFE_AUDIO_SPDIF_Output_SmpRate_192K;
                    //TVFE_Audio_SPDIF_Output_Config(TVFE_AUDIO_SPDIF_BITSTREAM,TVFE_AUDIO_SPDIF_Output_SmpRate_192K);
                    TVFE_Audio_SPDIF_Output_Config(DSPfd, &TVFE_Config);
                    break;
                default:
                    return APP_FALSE;
            }
            break;
        case APP_AUDIO_SPDIF_IECRX:
			TVFE_Config.SPDIF_Output_Config.SPDIF_Output_Config = TVFE_AUDIO_SPDIF_IECRX;
			TVFE_Config.SPDIF_Output_Config.SPDIF_SmpRate = TVFE_AUDIO_SPDIF_Output_SmpRate_48K;
			//TVFE_Audio_SPDIF_Output_Config(TVFE_AUDIO_SPDIF_IECRX,TVFE_AUDIO_SPDIF_Output_SmpRate_48K);
            TVFE_Audio_SPDIF_Output_Config(DSPfd, &TVFE_Config);
            break;
        default:
           return APP_FALSE;
    }

	return IsSuccess;
}


/********************************************************************
Prototype: APP_PCB_Audio_Set_Mute_Speaker()
Parameters:
                    Param:
                        0:unmute; 1:mute

Returns: APP_TRUE;APP_FALSE
Description: set Speaker mute/unmute
Restrictions:
********************************************************************/
APP_BOOL APP_PCB_Audio_Set_Mute_Speaker(UINT32 Param)
{
	APP_BOOL IsSuccess = APP_TRUE;
    TVFE_Audio_Mute_t AudioMuteConfig;
	TVFE_Audio_CMD_st TVFE_Config;

	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));

	//APP_PCB_AUDIO_DBG("APP_PCB_Audio_Set_Mute_Speaker Param = %d\n",Param);

	AudioMuteConfig.Mute_Delay = 0;
	AudioMuteConfig.Enable = (APP_BOOL)Param;
	AudioMuteConfig.Process_Path = _APP_PCB_AudioOutputConfigTable[APP_PCB_AUDIO_OUTPUT_VIA_SPEAKER].OutputConfig.Process_Path;
	AudioMuteConfig.pExternal_Mute_Control = NULL;
	TVFE_Config.Set_Mute_Config.Mute_Config = AudioMuteConfig;
	//TVFE_Audio_Set_Mute_Config(&AudioMuteConfig);
	TVFE_Audio_Set_Mute_Config(DSPfd,&TVFE_Config);
	return IsSuccess;
}


/********************************************************************
Prototype: APP_PCB_Audio_Set_Mute_RCA_Jack()
Parameters:
                    Param:
                        0:unmute; 1:mute

Returns: APP_TRUE;APP_FALSE
Description: set RCA mute/unmute
Restrictions:
********************************************************************/
APP_BOOL APP_PCB_Audio_Set_Mute_RCA_Jack(UINT32 Param)
{
	APP_BOOL IsSuccess = APP_TRUE;
    TVFE_Audio_Mute_t AudioMuteConfig;
	TVFE_Audio_CMD_st TVFE_Config;

	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));

	//APP_PCB_AUDIO_DBG("APP_PCB_Audio_Set_Mute_RCA_Jack Param = %d\n",Param);

	AudioMuteConfig.Mute_Delay = 0;
	AudioMuteConfig.Enable = (APP_BOOL)Param;
	AudioMuteConfig.Process_Path = _APP_PCB_AudioOutputConfigTable[APP_PCB_AUDIO_OUTPUT_VIA_RCA_JACK].OutputConfig.Process_Path;
	AudioMuteConfig.pExternal_Mute_Control = NULL;
	TVFE_Config.Set_Mute_Config.Mute_Config = AudioMuteConfig;
	//TVFE_Audio_Set_Mute_Config(&AudioMuteConfig);
	TVFE_Audio_Set_Mute_Config(DSPfd,&TVFE_Config);
	return IsSuccess;
}


/********************************************************************
Prototype: APP_PCB_Audio_Set_Mute_All()
Parameters:
                    Param:
                        0:unmute; 1:mute

Returns: APP_TRUE;APP_FALSE
Description: set Speaker and RCA mute/unmute
Restrictions:
********************************************************************/
APP_BOOL APP_PCB_Audio_Set_Mute_All(UINT32 Param)
{
	APP_BOOL IsSuccess = APP_TRUE;
	//APP_PCB_AUDIO_DBG("APP_PCB_Audio_Set_Mute_All Param = %d\n",Param);
	APP_PCB_Audio_Set_Mute_Speaker(Param);
	APP_PCB_Audio_Set_Mute_RCA_Jack(Param);
	return IsSuccess;
}


/********************************************************************
Prototype: APP_PCB_Audio_Set_Mute()
Parameters:
                    IsMute: 0:APP_FALSE;1:APP_TRUE
                    fRefVideoInSatus:
                    0:APP_FALSE;1:APP_TRUE
                    IsMode:APP_Audio_Mode_e
                    SourceType:eSource_Type_t

Returns: APP_TRUE;APP_FALSE
Description: set mute by the customer's spec
Restrictions:
********************************************************************/
APP_BOOL APP_PCB_Audio_Set_Mute(APP_BOOL IsMute, APP_BOOL fRefVideoInSatus, APP_Audio_Mode_e IsMode, PLF_AVType_e SourceType)
{
	APP_BOOL IsSuccess = APP_TRUE;

    if (fRefVideoInSatus)
    {
    	switch (IsMode)
		{
			case APP_AUDIO_MODE_NO_SIGNALE:
				if (IsMute)
				{
					if(PLF_INPUT_PIN_TYPE_ATV_TUNER == SourceType)
					{
					printf("jason APP_AUDIO_MODE_NO_SIGNALE AUDIF_Set_AtvRamp(1)\n");
						//TVFE_Audio_Set_AtvRamp(1);
					}
                    APP_PCB_Audio_Set_Mute_Speaker(APP_TRUE);
                    APP_PCB_Audio_Set_Mute_RCA_Jack(APP_FALSE);
				}
				else
				{
					if(PLF_INPUT_PIN_TYPE_STORAGE == SourceType)
					{
						APP_PCB_Audio_Set_Mute_Speaker(APP_FALSE);
					}
					else
					{
						APP_PCB_Audio_Set_Mute_Speaker(APP_TRUE);
					}
                    APP_PCB_Audio_Set_Mute_RCA_Jack(APP_FALSE);
				}
				break;

			case APP_AUDIO_MODE_SIGNAL_DETECT:
				if (IsMute)
				{
					if(PLF_INPUT_PIN_TYPE_ATV_TUNER == SourceType)
					{
						printf("jason APP_AUDIO_MODE_SIGNAL_DETECT AUDIF_Set_AtvRamp(1)\n");
						//TVFE_Audio_Set_AtvRamp(1);
					}
					APP_PCB_Audio_Set_Mute_Speaker(APP_TRUE);
                    APP_PCB_Audio_Set_Mute_RCA_Jack(APP_FALSE);
				}
				else
				{
					if(PLF_INPUT_PIN_TYPE_ATV_TUNER == SourceType)
					{
						printf("jason APP_AUDIO_MODE_SIGNAL_DETECT AUDIF_Set_AtvRamp(0)\n");
						//TVFE_Audio_Set_AtvRamp(0);
					}
					APP_PCB_Audio_Set_Mute_Speaker(APP_FALSE);
                    APP_PCB_Audio_Set_Mute_RCA_Jack(APP_FALSE);
				}
				break;

			case APP_AUDIO_MODE_CH_SEARCH:
				if(PLF_INPUT_PIN_TYPE_ATV_TUNER == SourceType)
				{
					//TVFE_Audio_Set_AtvRamp(1);
				}
				APP_PCB_Audio_Set_Mute_Speaker(APP_TRUE);
				APP_PCB_Audio_Set_Mute_RCA_Jack(APP_TRUE);
				break;

			case APP_AUDIO_MODE_CH_CHANGE:
				if(PLF_INPUT_PIN_TYPE_ATV_TUNER == SourceType)
				{
					//TVFE_Audio_Set_AtvRamp(1);
				}
				APP_PCB_Audio_Set_Mute_Speaker(APP_TRUE);
				APP_PCB_Audio_Set_Mute_RCA_Jack(APP_TRUE);
				break;

			default:
				break;
		}

    }
    else
    {
        if (IsMute)
        {
    		if(PLF_INPUT_PIN_TYPE_ATV_TUNER == SourceType)
    		{
    			//TVFE_Audio_Set_AtvRamp(1);
    		}
        	//APP_PCB_Audio_Set_Mute_All(APP_TRUE);
        	APP_PCB_Audio_Set_Mute_Speaker(APP_TRUE);
            APP_PCB_Audio_Set_Mute_RCA_Jack(APP_FALSE);
        }
        else
        {
    		if(PLF_INPUT_PIN_TYPE_ATV_TUNER == SourceType)
    		{
    			//TVFE_Audio_Set_AtvRamp(0);
    		}
        	APP_PCB_Audio_Set_Mute_All(APP_FALSE);
        }
    }
	//GL_TaskSleep(80);
	usleep(80000);

	return IsSuccess;
}

/********************************************************************
Prototype: APP_PCB_Audio_Set_DAC_Power()
Parameters:
                Param0:
                0:all DAC(include DAC0, DAC1, DAC2);
                1:DAC0; 2:DAC1; 3:DAC2;
                4:DAC VRT; 5:DAC Vcom
                Param1:
                0:on; 1:off

Returns: APP_TRUE;APP_FALSE
Description: set DAC power on/off
Restrictions:
********************************************************************/
APP_BOOL APP_PCB_Audio_Set_DAC_Power(UINT32 Param0, UINT32 Param1)
{
	APP_BOOL IsSuccess = APP_TRUE;
	TVFE_Audio_CMD_st TVFE_Config;

	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));

    if(Param0==0)
    {
        if(Param1==0)
        {	
			TVFE_Config.PowerControl.ePowerControl = TVFE_AUDIO_DAC_POWER_ON;
			//TVFE_AudioPowerControl(TVFE_AUDIO_DAC_POWER_ON);
            TVFE_AudioPowerControl(DSPfd, &TVFE_Config);
        }
        else
        {
			TVFE_Config.PowerControl.ePowerControl = TVFE_AUDIO_DAC_POWER_OFF;
            //TVFE_AudioPowerControl(TVFE_AUDIO_DAC_POWER_OFF);
            TVFE_AudioPowerControl(DSPfd, &TVFE_Config);
        }
    }
    else if(Param0==1)
    {
        if(Param1==0)
        {
			TVFE_Config.PowerControl.ePowerControl = TVFE_AUDIO_DAC0_POWER_ON;
            //TVFE_AudioPowerControl(TVFE_AUDIO_DAC0_POWER_ON);
            TVFE_AudioPowerControl(DSPfd, &TVFE_Config);
        }
        else
        {
			TVFE_Config.PowerControl.ePowerControl = TVFE_AUDIO_DAC0_POWER_OFF;
            //TVFE_AudioPowerControl(TVFE_AUDIO_DAC0_POWER_OFF);
            TVFE_AudioPowerControl(DSPfd, &TVFE_Config);
        }
    }
    else if(Param0==2)
    {
        if(Param1==0)
        {
			TVFE_Config.PowerControl.ePowerControl = TVFE_AUDIO_DAC1_POWER_ON;
            //TVFE_AudioPowerControl(TVFE_AUDIO_DAC1_POWER_ON);
            TVFE_AudioPowerControl(DSPfd, &TVFE_Config);
        }
        else
        {
			TVFE_Config.PowerControl.ePowerControl = TVFE_AUDIO_DAC1_POWER_OFF;
            //TVFE_AudioPowerControl(TVFE_AUDIO_DAC1_POWER_OFF);
            TVFE_AudioPowerControl(DSPfd, &TVFE_Config);
        }
    }
    else if(Param0==3)
    {
        if(Param1==0)
        {
			TVFE_Config.PowerControl.ePowerControl = TVFE_AUDIO_DAC2_POWER_ON;
            //TVFE_AudioPowerControl(TVFE_AUDIO_DAC2_POWER_ON);
            TVFE_AudioPowerControl(DSPfd, &TVFE_Config);
        }
        else
        {
			TVFE_Config.PowerControl.ePowerControl = TVFE_AUDIO_DAC2_POWER_OFF;
            //TVFE_AudioPowerControl(TVFE_AUDIO_DAC2_POWER_OFF);
            TVFE_AudioPowerControl(DSPfd, &TVFE_Config);
        }
    }
    else if(Param0==4)
    {
        if(Param1==0)
        {
			TVFE_Config.PowerControl.ePowerControl = TVFE_AUDIO_DAC_VRT_ON;
            //TVFE_AudioPowerControl(TVFE_AUDIO_DAC_VRT_ON);
            TVFE_AudioPowerControl(DSPfd, &TVFE_Config);
        }
        else
        {
			TVFE_Config.PowerControl.ePowerControl = TVFE_AUDIO_DAC_VRT_OFF;
            //TVFE_AudioPowerControl(TVFE_AUDIO_DAC_VRT_OFF);
            TVFE_AudioPowerControl(DSPfd, &TVFE_Config);
        }
    }
    else if(Param0==5)
    {
        if(Param1==0)
        {
			TVFE_Config.PowerControl.ePowerControl = TVFE_AUDIO_DAC_VCOM_ON;
            //TVFE_AudioPowerControl(TVFE_AUDIO_DAC_VCOM_ON);
            TVFE_AudioPowerControl(DSPfd, &TVFE_Config);
        }
        else
        {
			TVFE_Config.PowerControl.ePowerControl = TVFE_AUDIO_DAC_VCOM_OFF;
            //TVFE_AudioPowerControl(TVFE_AUDIO_DAC_VCOM_OFF);
            TVFE_AudioPowerControl(DSPfd, &TVFE_Config);
        }
    }
	return IsSuccess;
}

APP_BOOL APP_PCB_Set_SourceType(eSource_Type_t Param)
{
    APP_BOOL IsSuccess = APP_TRUE;
	TVFE_Audio_CMD_st TVFE_Config;

	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));

	TVFE_Config.Set_SourceType.Param = Param;
   	//TVFE_Audio_Set_SourceType(Param);
   	TVFE_Audio_Set_SourceType(DSPfd, &TVFE_Config);
	
    return IsSuccess;
}

#if 0
/********************************************************************
Prototype: APP_Audio_Set_SIF_Source(UINT32 Param0)
Parameters:
                Param0:

Returns: APP_TRUE;APP_FALSE
Description: 
Restrictions:
********************************************************************/
APP_BOOL APP_Audio_Set_SIF_Source(UINT32 Param0)
{
	APP_BOOL IsSuccess = APP_TRUE;
	TVFE_Audio_CMD_st TVFE_Config;

	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));

    //TVFE_Audio_Initial(DSPfd);
     TVFE_Audio_Set_SIF_Source(DSPfd, &TVFE_Config);
	return IsSuccess;
}
#else

/********************************************************************
Prototype: APP_Audio_Set_Sound_Source(PLF_AVSourcePinConfig_st *pCurrSrc)
Parameters:
                Param0:

Returns: APP_TRUE;APP_FALSE
Description: 
Restrictions:
********************************************************************/
APP_BOOL APP_Audio_Set_Sound_Source(PLF_AVSourcePinConfig_st *pCurrSrc)
{
	APP_BOOL IsSuccess = APP_TRUE;
	TVFE_Audio_CMD_st TVFE_Config;

	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));
	TVFE_Config.Set_PLF_AVSourcePinConfig.PLF_AVSourcePinConfig.eType= pCurrSrc->eType;
	TVFE_Config.Set_PLF_AVSourcePinConfig.PLF_AVSourcePinConfig.eAudioPri= pCurrSrc->eAudioPri;
     	TVFE_Audio_Set_Sound_Source(DSPfd, &TVFE_Config);
	return IsSuccess;
}
#endif
APP_BOOL APP_Set_SIF_Connect(TVFE_AudioSIFStandardMask_e eSIFStandardMask)
{
	APP_BOOL IsSuccess = APP_TRUE;
	TVFE_Audio_CMD_st TVFE_Config;

	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));
	TVFE_Config.SIFStandardMask.eSIFStandardMask = eSIFStandardMask;
	TVFE_AudioSetSIFConnect(DSPfd, &TVFE_Config);
	return IsSuccess;
}


APP_BOOL APP_Get_SIF_FrontEndParam(TVFE_AudioFrontEndParam_t *pFrontEndParam)
{
	APP_BOOL IsSuccess = APP_TRUE;
	TVFE_Audio_CMD_st TVFE_Config;
	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));

	TVFE_Config.Get_FrontEndParam.FrontEndParam = *(pFrontEndParam);
	TVFE_AudioGetFrontEndParam(DSPfd, &TVFE_Config);

 	*(pFrontEndParam) = TVFE_Config.Get_FrontEndParam.FrontEndParam; 

	printf("eMethod=%x\n",pFrontEndParam->eMethod);
	printf("eRetStandard=%x\n",pFrontEndParam->eRetStandard);
	printf("eRetMode=%x\n",pFrontEndParam->eRetMode);
	return IsSuccess;
}

APP_BOOL APP_Audio_Set_SIFHandler(UINT32 Param0)
{
	APP_BOOL IsSuccess = APP_TRUE;
	TVFE_Audio_CMD_st TVFE_Config;
	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));
   	TVFE_Audio_Set_SIFHandler(DSPfd, &TVFE_Config);
	return IsSuccess;
}

APP_BOOL APP_Audio_SetTunerValueType(TVFE_SoundStd_e eSoundStdType)
{
	APP_BOOL IsSuccess = APP_TRUE;
	TVFE_Audio_CMD_st TVFE_Config;

	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));
	TVFE_Config.TunerSoundStd.eTunerSoundStd = eSoundStdType;
	TVFE_SetTunerValueType(DSPfd, &TVFE_Config);
	return IsSuccess;
}

APP_BOOL APP_Audio_SIF_SetCurrentMode(TVFE_AudioSIFMode_e eSifMode)
{
	APP_BOOL IsSuccess = APP_TRUE;
	TVFE_Audio_CMD_st TVFE_Config;

	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));
	TVFE_Config.SIFMode.eSIFMode= eSifMode;
	TVFE_SIF_SetCurrentMode(DSPfd, &TVFE_Config);
	return IsSuccess;
}

APP_BOOL APP_Audio_SetSifAvailableMode(TVFE_AudioSIFAvailableMode_e eSIFAvailableMode)
{
	APP_BOOL IsSuccess = APP_TRUE;
	TVFE_Audio_CMD_st TVFE_Config;

	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));
	TVFE_Config.SIFAvailableMode.eSIFAvailableMode= eSIFAvailableMode;
	TVFE_SetSifAvailableMode(DSPfd, &TVFE_Config);
	return IsSuccess;
}

APP_BOOL APP_Audio_GetTunerValueType(TVFE_SoundStd_e *peSoundStdType)
{
	APP_BOOL IsSuccess = APP_TRUE;
	TVFE_Audio_CMD_st TVFE_Config;

	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));
	TVFE_GetTunerValueType(DSPfd, &TVFE_Config);	
	*(peSoundStdType)=TVFE_Config.TunerSoundStd.eTunerSoundStd;
	printf("GetTunerValueType=%x\n",*(peSoundStdType));
	return IsSuccess;
}

APP_BOOL APP_Audio_GetSIFWorkingMode(TVFE_AudioSIFMode_e *peCurrentMode)
{
	APP_BOOL IsSuccess = APP_TRUE;
	TVFE_Audio_CMD_st TVFE_Config;
	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));
	TVFE_GetSIFWorkingMode(DSPfd, &TVFE_Config);
 	*(peCurrentMode) = TVFE_Config.SIFMode.eSIFMode;
	printf("GetSIFWorkingMode=%x\n",*(peCurrentMode));
	return IsSuccess;
}

APP_BOOL APP_Audio_GetSifAvailableMode(TVFE_AudioSIFAvailableMode_e *peSifAvaiMode)
{
	APP_BOOL IsSuccess = APP_TRUE;
	TVFE_Audio_CMD_st TVFE_Config;
	memset(&TVFE_Config,0,sizeof(TVFE_Audio_CMD_st));
	TVFE_GetSifAvailableMode(DSPfd, &TVFE_Config);
 	*(peSifAvaiMode) = TVFE_Config.SIFAvailableMode.eSIFAvailableMode;
	printf("GetSifAvailableMode=%x\n",*(peSifAvaiMode));
	return IsSuccess;
}


/***** End of APP_XXX function *****/

/***************
 * End of customer.c
 *
 * ************/


/************************

APP_PCB_Audio API TEST Code Here

************************/
void SIF_Auto_Test(void)
{
	TVFE_AudioFrontEndParam_t FrontEndParam;

	//Leon Test
	//while(count<5)
	#if 0
	APP_Audio_Set_SIF_Source(0);
	#else
	PLF_AVSourcePinConfig_st  pCurrSrc;
	pCurrSrc.eType=PLF_INPUT_PIN_TYPE_ATV_TUNER;

	pCurrSrc.eType=PLF_INPUT_PIN_TYPE_CVBS;
	pCurrSrc.eAudioPri=AUDIF_ADC_SOURCE_ADC0;
	APP_Audio_Set_Sound_Source(&pCurrSrc);
	#endif
	memset(&FrontEndParam,0,sizeof(TVFE_AudioFrontEndParam_t));		
	#if 0  // tuner detection
	FrontEndParam.eMethod=TVFE_AUDIO_REPORT_DIRECT;
	APP_Set_SIF_Connect(atoi(argv[1]));		
	usleep(200000);
	usleep(200000);
	usleep(200000);
	usleep(200000);
	usleep(200000);
	usleep(200000);
	usleep(200000);
	usleep(200000);
	usleep(200000);
	usleep(200000);
	usleep(200000);
	usleep(200000);
	usleep(200000);
	usleep(200000);
	usleep(200000);
	usleep(200000);
	usleep(200000);
	APP_Get_SIF_FrontEndParam(&FrontEndParam);
	#else
	TVFE_SoundStd_e eSoundStdType;
	TVFE_AudioSIFMode_e eCurrentMode;
	TVFE_AudioSIFAvailableMode_e eSifAvaiMode;
	TVFE_AudioSIFStandard_t SifStandrad;
	TVFE_AudioSIFAvailableMode_e eSifAvailableMode;
	TVFE_AudioSIFMode_e g_u8SifNextMode = TVFE_AUDIO_SIF_MODE_MAX;
    	TVFE_AudioSIFMode_e ePreferSifMode = TVFE_AUDIO_SIF_MODE_MAX;

	APP_Audio_SetTunerValueType(TVFE_SOUND_STANDARD_AUTO);
	int i=10;
	
	while(1)
	{
		#if 0		
		while(i)
		{
			APP_Audio_Set_SIFHandler(0);
			usleep(200000);
			usleep(200000);
			usleep(200000);
			usleep(200000);
			usleep(200000);
			usleep(200000);
			usleep(200000);
			usleep(200000);
			usleep(200000);
			usleep(200000);
			usleep(200000);
			usleep(200000);
			usleep(200000);
			usleep(200000);
			usleep(200000);
			usleep(200000);
			usleep(200000);
			
			FrontEndParam.eMethod=TVFE_AUDIO_REPORT_INDIRECT;
			APP_Get_SIF_FrontEndParam(&FrontEndParam);
			SifStandrad = FrontEndParam.eRetStandard;
			
			printf("xxx SifStandrad=%x\n",SifStandrad);
			i--;
		}
		#else
		APP_Audio_Set_SIFHandler(0);
		usleep(3400000);
		FrontEndParam.eMethod=TVFE_AUDIO_REPORT_INDIRECT;
		APP_Get_SIF_FrontEndParam(&FrontEndParam);
		SifStandrad = FrontEndParam.eRetStandard;			
		#endif
		
		APP_Audio_GetTunerValueType(&eSoundStdType);


		printf("xxx SifStandrad=%x\n",SifStandrad);
		printf("xxx eSoundStdType=%x\n",eSoundStdType);

		if(SifStandrad != TVFE_AUDIO_SIF_STANDARD_BUSY)
		{
			if((eSoundStdType == TVFE_SOUND_STANDARD_L)
				||(eSoundStdType == TVFE_SOUND_STANDARD_L1))
			{
				if(SifStandrad!=TVFE_AUDIO_SIF_STANDARD_L_NICAM)
				{
					SifStandrad=TVFE_AUDIO_SIF_STANDARD_FAIL;
				}
			}
			else if(eSoundStdType == TVFE_SOUND_STANDARD_BG)
			{
				if((SifStandrad!=TVFE_AUDIO_SIF_STANDARD_BG)&&(SifStandrad!=TVFE_AUDIO_SIF_STANDARD_BG_A2)
					&&(SifStandrad!=TVFE_AUDIO_SIF_STANDARD_BG_NICAM))
				{
					SifStandrad=TVFE_AUDIO_SIF_STANDARD_FAIL;
				}
			}
			else if(eSoundStdType == TVFE_SOUND_STANDARD_DK)
			{
				if((SifStandrad!=TVFE_AUDIO_SIF_STANDARD_DK)&&(SifStandrad!=TVFE_AUDIO_SIF_STANDARD_DK_A2_1)
					&&(SifStandrad!=TVFE_AUDIO_SIF_STANDARD_DK_A2_2)&&(SifStandrad!=TVFE_AUDIO_SIF_STANDARD_DK_A2_3)
					&&(SifStandrad!=TVFE_AUDIO_SIF_STANDARD_DK_NICAM))
				{
					SifStandrad=TVFE_AUDIO_SIF_STANDARD_FAIL;
				}
			}
			else if(eSoundStdType == TVFE_SOUND_STANDARD_I)
			{
				if(SifStandrad!=TVFE_AUDIO_SIF_STANDARD_I_NICAM)
				{
					SifStandrad=TVFE_AUDIO_SIF_STANDARD_FAIL;
				}

			}
			else
			{
				if((SifStandrad!=TVFE_AUDIO_SIF_STANDARD_BG)&&(SifStandrad!=TVFE_AUDIO_SIF_STANDARD_DK)
					&&(SifStandrad!=TVFE_AUDIO_SIF_STANDARD_BG_A2)&&(SifStandrad!=TVFE_AUDIO_SIF_STANDARD_BG_NICAM)
					&&(SifStandrad!=TVFE_AUDIO_SIF_STANDARD_DK_A2_1)&&(SifStandrad!=TVFE_AUDIO_SIF_STANDARD_DK_A2_2)
					&&(SifStandrad!=TVFE_AUDIO_SIF_STANDARD_DK_A2_3)&&(SifStandrad!=TVFE_AUDIO_SIF_STANDARD_DK_NICAM)
					&&(SifStandrad!=TVFE_AUDIO_SIF_STANDARD_I_NICAM))
				{
					SifStandrad=TVFE_AUDIO_SIF_STANDARD_FAIL;
				}
			}
			#if 0
			if(SifStandrad!=TVFE_AUDIO_SIF_STANDARD_FAIL)
			{
				if((SifStandrad==TVFE_AUDIO_SIF_STANDARD_BG)||(SifStandrad==TVFE_AUDIO_SIF_STANDARD_BG_A2)
					||(SifStandrad==TVFE_AUDIO_SIF_STANDARD_BG_NICAM))
				{
					eSoundStdType=TVFE_SOUND_STANDARD_BG;				
				}
				else if((SifStandrad==TVFE_AUDIO_SIF_STANDARD_DK)||(SifStandrad==TVFE_AUDIO_SIF_STANDARD_DK_A2_1)
					||(SifStandrad==TVFE_AUDIO_SIF_STANDARD_DK_A2_2)||(SifStandrad==TVFE_AUDIO_SIF_STANDARD_DK_A2_3)
					||(SifStandrad==TVFE_AUDIO_SIF_STANDARD_DK_NICAM))
				{
					eSoundStdType=TVFE_SOUND_STANDARD_DK;				
				}
				else if(SifStandrad==TVFE_AUDIO_SIF_STANDARD_I_NICAM)
				{
					eSoundStdType=TVFE_SOUND_STANDARD_I;				
				}
				
				APP_Audio_SetTunerValueType(eSoundStdType);				
			}
			#endif
		}

		if(SifStandrad == TVFE_AUDIO_SIF_STANDARD_FAIL)
		{

			if((eSoundStdType == TVFE_SOUND_STANDARD_L)
				||(eSoundStdType == TVFE_SOUND_STANDARD_L1))
			{
				APP_Set_SIF_Connect(TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_L);
			}
			else
			{
				if(eSoundStdType == TVFE_SOUND_STANDARD_BG)
				{
					APP_Set_SIF_Connect( \
						TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_BG);
				}
				else if(eSoundStdType == TVFE_SOUND_STANDARD_DK)
				{
					APP_Set_SIF_Connect( \
						TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_DK);
				}
				else if(eSoundStdType == TVFE_SOUND_STANDARD_I)
				{
					APP_Set_SIF_Connect( \
						TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_I);
				}
				else
				{
					APP_Set_SIF_Connect( \
						TVFE_AUDIO_SIF_STANDARD_ASD_MODE_MASK_BGDKI);
				}
			}
		}
		else if(SifStandrad != TVFE_AUDIO_SIF_STANDARD_BUSY)
		{
			#if 0
			if(fChangeLevelAdjust == DTV_FALSE)
			{
				switch(SifStandrad)
				{
					case TVFE_AUDIO_SIF_STANDARD_I_NICAM:
						LevelAdjust_config.bDECAdjustLevel		= 1;
						LevelAdjust_config.bMonoAdjustLevel		= 2;
						LevelAdjust_config.bNICAMAdjustLevel	= 2;
						LevelAdjust_config.bSAPAdjustLevel		= 0;
						LevelAdjust_config.bADCAdjustLevel		= -1;
						LevelAdjust_config.bDigitalAdjustLevel	= -3;
						TVFE_AudioSetLevelAdjust(&LevelAdjust_config);
						break;

					case TVFE_AUDIO_SIF_STANDARD_L_NICAM:
						LevelAdjust_config.bDECAdjustLevel		= 1;
						LevelAdjust_config.bMonoAdjustLevel		= 0;
						LevelAdjust_config.bNICAMAdjustLevel	= 2;
						LevelAdjust_config.bSAPAdjustLevel		= 0;
						LevelAdjust_config.bADCAdjustLevel		= -1;
						LevelAdjust_config.bDigitalAdjustLevel	= -3;
						TVFE_AudioSetLevelAdjust(&LevelAdjust_config);
						break;

					default :
						LevelAdjust_config.bDECAdjustLevel		= 2;
						LevelAdjust_config.bMonoAdjustLevel		= 2;
						LevelAdjust_config.bNICAMAdjustLevel	= 1;
						LevelAdjust_config.bSAPAdjustLevel		= 0;
						LevelAdjust_config.bADCAdjustLevel		= -1;
						LevelAdjust_config.bDigitalAdjustLevel	= -3;
						break;
				}
				fChangeLevelAdjust = DTV_TRUE;
			}
			#endif
			/* Decide the available sound modes for the current channel */
			FrontEndParam.eMethod=TVFE_AUDIO_REPORT_INDIRECT;
			APP_Get_SIF_FrontEndParam(&FrontEndParam);
			eSifAvailableMode = FrontEndParam.eRetMode;
			
			APP_Audio_GetSifAvailableMode(&eSifAvaiMode);
			
		printf("222 eSifAvailableMode=%x\n",eSifAvailableMode);
		printf("222 eSifAvaiMode=%x\n",eSifAvaiMode);

			if (eSifAvailableMode != eSifAvaiMode)
			{
				printf("Detect sound changed!!\n");

	            		/* get user's setting from database */
	            		//ePreferSifMode = PMA_GetChanPropEx(PLAYBACK_ATV_GetCurrentProgIndex(), ACHN_ATTRIB_MULTI_AUDIO_GROUP, ACHN_ATTRIB_MULTI_AUDIO_MASK);
				if(eSifAvailableMode==TVFE_AUDIO_SIF_AVAIL_STEREO)
				{
					ePreferSifMode=TVFE_AUDIO_SIF_MODE_STEREO;
				}
				else if (eSifAvailableMode==TVFE_AUDIO_SIF_AVAIL_DUAL)
				{
					ePreferSifMode=TVFE_AUDIO_SIF_MODE_DUAL_II;
				}
				else if (eSifAvailableMode==TVFE_AUDIO_SIF_AVAIL_NICAM_MONO)
				{
					ePreferSifMode=TVFE_AUDIO_SIF_MODE_NICAM_MONO;
				}
				else if (eSifAvailableMode==TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO)
				{
					ePreferSifMode=TVFE_AUDIO_SIF_MODE_ANALOG_MONO;
				}
				
				printf("010 ePreferSifMode=%x\n",ePreferSifMode);
				
	            		/* judge */
	            	     switch (ePreferSifMode)
		            {
		                case TVFE_AUDIO_SIF_MODE_STEREO:
		                    if ((eSifAvailableMode&TVFE_AUDIO_SIF_AVAIL_STEREO) != 0)
		                    {
		                        g_u8SifNextMode = ePreferSifMode;
		                    }
		                    break;
		                case TVFE_AUDIO_SIF_MODE_DUAL_I:
		                case TVFE_AUDIO_SIF_MODE_DUAL_II:
		                case TVFE_AUDIO_SIF_MODE_DUAL_I_II:
		                    if ((eSifAvailableMode&TVFE_AUDIO_SIF_AVAIL_DUAL) != 0)
		                    {
		                        g_u8SifNextMode = ePreferSifMode;
		                    }
		                    break;
		                case TVFE_AUDIO_SIF_MODE_NICAM_MONO:
		                    if ((eSifAvailableMode&TVFE_AUDIO_SIF_AVAIL_NICAM_MONO) != 0)
		                    {
		                        g_u8SifNextMode = ePreferSifMode;
		                    }
		                    break;
		                case TVFE_AUDIO_SIF_MODE_ANALOG_MONO:
		                    if ((eSifAvailableMode&TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO) != 0)
		                    {
		                        g_u8SifNextMode = ePreferSifMode;
		                    }
		                    break;
		                default:
		                    g_u8SifNextMode = TVFE_AUDIO_SIF_MODE_MAX;
		                    break;
		            }
						 
				printf("020 ePreferSifMode=%x\n",ePreferSifMode);
				printf("021 g_u8SifNextMode=%x\n",g_u8SifNextMode);

				if (g_u8SifNextMode == TVFE_AUDIO_SIF_MODE_MAX)
				{
					if ((eSifAvailableMode & TVFE_AUDIO_SIF_AVAIL_STEREO) != 0)
					{
	 					g_u8SifNextMode = TVFE_AUDIO_SIF_MODE_STEREO;
					}
					else if ((eSifAvailableMode & TVFE_AUDIO_SIF_AVAIL_DUAL) != 0)
					{
						g_u8SifNextMode = TVFE_AUDIO_SIF_MODE_DUAL_I;
					}
					else if ((eSifAvailableMode & TVFE_AUDIO_SIF_AVAIL_NICAM_MONO) != 0)
					{
						g_u8SifNextMode = TVFE_AUDIO_SIF_MODE_NICAM_MONO;
					}
					else if ((eSifAvailableMode & TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO) != 0)
					{
						g_u8SifNextMode = TVFE_AUDIO_SIF_MODE_ANALOG_MONO;
					}
				}
				printf("030 ePreferSifMode=%x\n",ePreferSifMode);
				printf("031 g_u8SifNextMode=%x\n",g_u8SifNextMode);
				
			}
			
			APP_Audio_GetSIFWorkingMode(&eCurrentMode);
			if(eCurrentMode != g_u8SifNextMode)
			{
				if(g_u8SifNextMode != TVFE_AUDIO_SIF_MODE_MAX)
				{
					APP_Audio_SIF_SetCurrentMode(g_u8SifNextMode);
				}
			}
			APP_Audio_SetSifAvailableMode(eSifAvaiMode);
			
		}
		
		
		//APP_Audio_SIF_SetCurrentMode(TVFE_AUDIO_SIF_MODE_ANALOG_MONO);
		//APP_Audio_SetSifAvailableMode(TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO);

		//printf("eSifAvaiMode=%x\n",eSifAvaiMode);

		
	}
	#endif


	
	#if 0
	APP_Audio_SIF_SetCurrentMode(TVFE_AUDIO_SIF_MODE_ANALOG_MONO);
	APP_Audio_GetTunerValueType(&eSoundStdType);
	APP_Audio_GetSIFWorkingMode(&eCurrentMode);
	APP_Audio_SetSifAvailableMode(TVFE_AUDIO_SIF_AVAIL_ANALOG_MONO);
	APP_Audio_GetSifAvailableMode(&eSifAvaiMode);
	#endif	
}


int main()
{
	int ret = 0;

	DSPDEV_OPEN();

	// Audio_Initial demo
	APP_PCB_Audio_Initial(0);

	//APP_PCB_Source API demo
	APP_PCB_Audio_Set_Level_Adjust(APP_PCB_SOURCE_TUNER0);
	//APP_PCB_Audio_Set_Level_Adjust(APP_PCB_SOURCE_CVBS0);
	//APP_PCB_Audio_Set_Level_Adjust(APP_PCB_SOURCE_CVBS0);
	//APP_PCB_Audio_Set_Level_Adjust(APP_PCB_SOURCE_YPBPR0);
	//APP_PCB_Audio_Set_Level_Adjust(APP_PCB_SOURCE_VGA0);
	//APP_PCB_Audio_Set_Level_Adjust(APP_PCB_SOURCE_HDMI0);
	//APP_PCB_Audio_Set_Level_Adjust(APP_PCB_SOURCE_MULTIMEDIA);

	//APP_PCB_Audio_Set_Volume API Demo
	APP_PCB_Audio_Set_Volume(100,SOURCE_ATV);
	//APP_PCB_Audio_Set_Volume(10,SOURCE_AV);
	//APP_PCB_Audio_Set_Volume(20,SOURCE_ATV);
	
	//APP_PCB_Audio_Set_Audio_Mode API Demo
	APP_PCB_Audio_Set_Audio_Mode(0, 20, 30);	
	//APP_PCB_Audio_Set_Audio_Mode(1, 50, 60);	

	//APP_PCB_Audio_Set_Bass API demo
	APP_PCB_Audio_Set_Bass(0);
	APP_PCB_Audio_Set_Bass(100);

	//APP_PCB_Audio_Set_Treble API demo
	APP_PCB_Audio_Set_Treble(0);
	APP_PCB_Audio_Set_Treble(100);
	
	//APP_PCB_Audio_Set_MainEqulizer API demo
	APP_PCB_Audio_Set_MainEqulizer(AL_EQ_BAND_100HZ,0);
	//APP_PCB_Audio_Set_MainEqulizer(AL_EQ_BAND_100HZ,100);
	//APP_PCB_Audio_Set_MainEqulizer(AL_EQ_BAND_8000HZ,0);
	//APP_PCB_Audio_Set_MainEqulizer(AL_EQ_BAND_8000HZ,100);
	 
	//APP_PCB_Audio_Set_Balance API demo
	APP_PCB_Audio_Set_Balance(0);
	//APP_PCB_Audio_Set_Balance(100);


	//APP_PCB_Audio_Set_Surround API demo
	APP_PCB_Audio_Set_Surround(0);
	//APP_PCB_Audio_Set_Surround(1);


	//APP_PCB_Audio_Set_Perfect_Sound API demo
	APP_PCB_Audio_Set_Perfect_Sound(0);
	//APP_PCB_Audio_Set_Perfect_Sound(1);

	//APP_PCB_Audio_Set_Lip_Sync API demo
	APP_PCB_Audio_Set_Lip_Sync(TVFE_TRUE, 20);
	//APP_PCB_Audio_Set_Lip_Sync(TVFE_FALSE, 10);


	//APP_PCB_Audio_Set_Output_Selection API demo
	APP_PCB_Audio_Set_Output_Selection(PLF_INPUT_PIN_TYPE_ATV_TUNER);
	//APP_PCB_Audio_Set_Output_Selection(PLF_INPUT_PIN_TYPE_DTV);

	
	//APP_PCB_Audio_Set_SPDIF_Output API demo
	//APP_PCB_Audio_Set_SPDIF_Output(APP_AUDIO_SPDIF_PCM,APP_AUDIO_SPDIF_Output_SmpRate_44K);
	//APP_PCB_Audio_Set_SPDIF_Output(APP_AUDIO_SPDIF_BITSTREAM,APP_AUDIO_SPDIF_Output_SmpRate_96K);
	APP_PCB_Audio_Set_SPDIF_Output(APP_AUDIO_SPDIF_OFF,APP_AUDIO_SPDIF_Output_SmpRate_48K);
	//APP_PCB_Audio_Set_SPDIF_Output(APP_AUDIO_SPDIF_IECRX,APP_AUDIO_SPDIF_Output_SmpRate_192K);


	//APP_PCB_Audio_Set_Mute_Speaker API demo
	//APP_PCB_Audio_Set_Mute_Speaker(0);
	//APP_PCB_Audio_Set_Mute_Speaker(1);

	//APP_PCB_Audio_Set_Mute_RCA_Jack API demo
	//APP_PCB_Audio_Set_Mute_RCA_Jack(0);
	//APP_PCB_Audio_Set_Mute_RCA_Jack(1);

	//APP_PCB_Audio_Set_Mute_All API demo
	//APP_PCB_Audio_Set_Mute_All(0);
	//APP_PCB_Audio_Set_Mute_All(1);
	

	//APP_PCB_Audio_Set_Mute API demo
	//APP_PCB_Audio_Set_Mute(0, 1, APP_AUDIO_MODE_NO_SIGNALE, APP_PCB_SOURCE_TUNER0);

	//APP_PCB_Audio_Set_DAC_Power API demo
	//APP_PCB_Audio_Set_DAC_Power(0,0);
	//APP_PCB_Audio_Set_DAC_Power(4,1);


	//APP_PCB_Set_SourceType api demo
	//APP_PCB_Set_SourceType(SOURCE_AV);
	//APP_PCB_Set_SourceType(SOURCE_ATV);

	SIF_Auto_Test();

	DSPDEV_CLOSE();
	return ret;
}

