#ifndef __DSP_API__
#define __DSP_API__
#include "types.h"
#include "wave_fmt.h"
#include "plf_av_source.h"
#include "drv_ssd.h"
#include "tvfe_api.h"
#include "tvfe_audio.h"
#include "dsp_ioctl.h"


int DSPDEV_OPEN();
int DSPDEV_CLOSE(int DSPfd);
int DSPDEV_SetAudioWavefmt(int DSPfd, int AudioCodecType);
int DSPDEV_GetAudioWavefmt(int DSPfd,WAVFORMATMACRO *wfmt);
int DSPDEV_CleanAudioBuf(int DSPfd);
int DSPDEV_GetBSFreeSize(int DSPfd);

#endif
