#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "stdlib.h"
#include <gsl.h>
//#include "gl_common.h"
//#include "config_sys.h"
//#include "priority.h"
//#include <platform/regmap.h>

//#include <cyg/fileio/fileio.h>
//#include <cyg/compress/zlib.h>
//#include <cyg/kernel/kapi.h>
//#include <cyg/hal/hal_if.h>

/* TODO:
 * The expected time unit of return value for gsl is milli-second.
 * The time unit of times() is decided by USER_TICK in Linux kernel,
 * and it is ten-milli-second currently.
 *
 * We can sysconf(_SC_CLK_TCK) to get user clock hz.
 */
#define GSL_USER_TICK 1

GL_UINT64 GL_GetRtc64(void)
{
#if 0
	return cyg_current_time();
#else   // hz = sysconf(_SC_CLK_TCK);
    clock_t tick = (clock_t)times(NULL);
    return tick * GSL_USER_TICK;
#endif
}

GL_UINT32 GL_GetRtc32(void)
{
#if 0
	return (GL_UINT32)cyg_current_time();
#else // hz = sysconf(_SC_CLK_TCK);
    clock_t tick = (clock_t)times(NULL);
    return tick * GSL_USER_TICK;
#endif
}

GL_UINT32 GL_GetCpuCnt(void)
{
	GL_UINT32 clk;
	asm volatile (
		"rdhwr	%0, $2\n\t"	/* mfc0 %0, $9 */
		: "=r"(clk));
	return clk;
}

GL_UINT32 GL_GetCpuCntFreq(void)
{
	return GL_GetSysBusFreq() * GL_GetCpuFreqMult() / 2;
}

GL_UINT32 GL_GetUsrRtc(void)
{
	return (HWREG_R(rtc_23_16) << 16) | HWREG_R(rtc_15_0);
}

void GL_SetUsrRtc(GL_UINT32 rtc)
{
	HWREG_W(rtc_23_16, rtc << 16);
	HWREG_W(rtc_15_0 , rtc & 0xFFFF);
}

void GL_InitUsrRtc(GL_UINT32 divisor)
{
	HWREG_W(rtc_divisor, divisor);
	HWREG_W(rtc_23_16, 0);
	HWREG_W(rtc_15_0 , 0);
}

