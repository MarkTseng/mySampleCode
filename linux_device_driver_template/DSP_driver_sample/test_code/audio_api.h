#ifndef __AUDIO_API__
#define __AUDIO_API__

#include "types.h"
#include "wave_fmt.h"
#include "plf_av_source.h"
#include "drv_ssd.h"
#include "tvfe_api.h"
#include "tvfe_audio.h"
#include "dsp_ioctl.h"

TVFE_Audio_Status_e TVFE_Audio_Set_Output_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_Output_Selection(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_Volume(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_Bass(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_Treble(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_Main_Equlizer_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Get_Main_Equlizer_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_Balance(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_AUX_Volume(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_Effect_Mode(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_AVL_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_Mute_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Initial(int Aud_fd);
TVFE_Audio_Status_e TVFE_Audio_Set_Level_Adjust_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_SIF_Over_Modulation_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_SIF_Detect_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_SPDIF_Output_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_AtvRamp(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Lip_Sync_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_PCM_Output_Phase_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_SUNSUR_Config(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_HEAAC_Output(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_Spdif_ChannelStatus(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_MasterMuteStatus(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Get_MasterMuteStatus(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_AC3TransCoding(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_Audio_Set_SourceType(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
TVFE_Audio_Status_e TVFE_AudioPowerControl(int Aud_fd, TVFE_Audio_CMD_st *TVFE_Config);
#endif
