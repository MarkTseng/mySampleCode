#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "stdlib.h"
#include "gsl.h"
//#include "gl_common.h"
//#include "config_sys.h"
//#include "priority.h"
//#include <platform/regmap.h>

//#include <cyg/fileio/fileio.h>
//#include <cyg/compress/zlib.h>
//#include <cyg/kernel/kapi.h>
//#include <cyg/hal/hal_if.h>

#ifdef CYGDBG_KERNEL_TIMER_INTERRUPT_VARIATION
extern cyg_uint8 cyg_timer_variation_enable;

static INT16 *timer_var_record_start = NULL;
static INT16 *timer_var_record_end;
static INT16 *timer_var_record_pointer;
static GL_UINT32 timer_var_record_prev_val;

void cyg_timer_variation_callback(GL_UINT32 cur_tick)
{
	if (cur_tick > timer_var_record_prev_val) {
		*timer_var_record_pointer = (INT16)((cur_tick - timer_var_record_prev_val) - hal_sysclock);
	} else {
		*timer_var_record_pointer = (INT16)((((GL_UINT32)~0 - timer_var_record_prev_val) + cur_tick) - hal_sysclock);
	}
	timer_var_record_prev_val = cur_tick;

	if (timer_var_record_pointer == timer_var_record_end) {
		timer_var_record_pointer = timer_var_record_start;
	} else {
		++timer_var_record_pointer;
	}
}
#endif

GL_Status_t GL_TimerVariation_Enable(void)
{
#ifdef CYGDBG_KERNEL_TIMER_INTERRUPT_VARIATION
	if (timer_var_record_start == NULL) {
		gsl_printf("Error: Buffer has not been set. Try \"tvarb <num>\"\n");
		return GL_FAILURE;
	}
	asm volatile (
		"mfc0	%0, $9;"
		: "=r"(timer_var_record_prev_val)
	);
	cyg_timer_variation_enable = 1;
	return GL_SUCCESS;
#else
	gsl_printf("Error: eCos library doesn't support this.\n");
	return GL_FAILURE;
#endif
}

GL_Status_t GL_TimerVariation_Disable(void)
{
#ifdef CYGDBG_KERNEL_TIMER_INTERRUPT_VARIATION
	cyg_timer_variation_enable = 0;
	return GL_SUCCESS;
#else
	gsl_printf("Error: eCos library doesn't support this.\n");
	return GL_FAILURE;
#endif
}

GL_Status_t GL_TimerVariation_SetBuffer(GL_UINT32 record)
{
#ifdef CYGDBG_KERNEL_TIMER_INTERRUPT_VARIATION
	if (timer_var_record_start != NULL) {
		gsl_printf("Error: Buffer has been set.\n");
		return GL_FAILURE;
	}
	timer_var_record_start = GL_MemAlloc(sizeof(UINT16) * record);
	if (timer_var_record_start == NULL) {
		gsl_printf("Error: Memory is insufficient.\n");
		return GL_FAILURE;
	}
	timer_var_record_pointer = timer_var_record_start;
	timer_var_record_end = &timer_var_record_start[record];
	return GL_SUCCESS;
#else
	gsl_printf("Error: eCos library doesn't support this.\n");
	return GL_FAILURE;
#endif
}

GL_Status_t GL_TimerVariation_ClearBuffer(void)
{
#ifdef CYGDBG_KERNEL_TIMER_INTERRUPT_VARIATION
	if (timer_var_record_start == NULL) {
		return GL_FAILURE;
	}
	if (cyg_timer_variation_enable == 1) {
		gsl_printf("Error: Please stop measuring first. Try \"tvare\".\n");
		return GL_FAILURE;
	}
	GL_MemFree(timer_var_record_start);
	timer_var_record_start = NULL;
	return GL_SUCCESS;
#else
	gsl_printf("Error: eCos library doesn't support this.\n");
	return GL_FAILURE;
#endif
}

void GL_TimerVariation_ShowResult(void)
{
#ifdef CYGDBG_KERNEL_TIMER_INTERRUPT_VARIATION
	INT16 *p = timer_var_record_start;
	INT32 sum = 0;
	INT32 max = -(~0);
	INT32 min = ~0;
	GL_UINT32 intr_status;

	if (timer_var_record_start == NULL) {
		gsl_printf("Error: Timer Variation has not started. Try \"tvars\"\n");
		return;
	}
	if (cyg_timer_variation_enable == 1) {
		gsl_printf("Error: Please stop measuring first. Try \"tvare\"\n");
		return;
	}

	HAL_DISABLE_INTERRUPTS(intr_status);
	do {
		sum += *p;
		if (*p > max) {
			max = *p;
		}
		if (*p < min) {
			min = *p;
		}
		if (p == timer_var_record_end) {
			break;
		}
		++p;
	} while (1);
	diag_printf("Result:\n");
	diag_printf("Variation: max = %d, min = %d\n", max, min);
	diag_printf("Sum of variation = %d, hal_sysclock = %d\n", sum, hal_sysclock);
	diag_printf("Timer Interrupt Variation is: %d.%04d%%\n",
				(sum * 100 / (INT32)(hal_sysclock)),
				GL_GET_FRACTION(sum, hal_sysclock, 10000));
	HAL_RESTORE_INTERRUPTS(intr_status);
#else
	gsl_printf("Error: eCos library doesn't support this.\n");
#endif
}

