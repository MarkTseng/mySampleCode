//?!
#include "gsl.h"

#if 0   //?! machao@20101105 #ifdef CYGPKG_KERNEL_INSTRUMENT
#define INSTRUMENT_BUFFER_SIZE	4096
#define INSTRUMENT_MAX_RECORD	(INSTRUMENT_BUFFER_SIZE / 16)

extern int instrument_switch;
/* defined in ecos and it points to the next input slot */
extern struct Instrument_Record *instrument_buffer_pointer;

#if defined(CYGDBG_KERNEL_INSTRUMENT_FLAGS) && defined(CYGDBG_KERNEL_INSTRUMENT_SCHED)
/*
	TODO: use CYGDBG_KERNEL_INSTRUMENT_SCHED to know if we are using
	context-switch-only version or full version
 */
void GL_OS_Monitor_UnmaskAll()
{
	cyg_instrument_enable(CYG_INSTRUMENT_CLASS_SCHED, 0);
	cyg_instrument_enable(CYG_INSTRUMENT_CLASS_THREAD, 0);
	cyg_instrument_enable(CYG_INSTRUMENT_CLASS_INTR, 0);
	cyg_instrument_enable(CYG_INSTRUMENT_CLASS_MUTEX, 0);
	cyg_instrument_enable(CYG_INSTRUMENT_CLASS_CONDVAR, 0);
	cyg_instrument_enable(CYG_INSTRUMENT_CLASS_BINSEM, 0);
	cyg_instrument_enable(CYG_INSTRUMENT_CLASS_CNTSEM, 0);
	cyg_instrument_enable(CYG_INSTRUMENT_CLASS_CLOCK, 0);
	cyg_instrument_enable(CYG_INSTRUMENT_CLASS_ALARM, 0);
	cyg_instrument_enable(CYG_INSTRUMENT_CLASS_MBOXT, 0);
	cyg_instrument_enable(CYG_INSTRUMENT_CLASS_SMP, 0);
	cyg_instrument_enable(CYG_INSTRUMENT_CLASS_MLQ, 0);
}

void GL_OS_Monitor_MaskAll()
{
	cyg_instrument_disable(CYG_INSTRUMENT_CLASS_SCHED, 0);
	cyg_instrument_disable(CYG_INSTRUMENT_CLASS_THREAD, 0);
	cyg_instrument_disable(CYG_INSTRUMENT_CLASS_INTR, 0);
	cyg_instrument_disable(CYG_INSTRUMENT_CLASS_MUTEX, 0);
	cyg_instrument_disable(CYG_INSTRUMENT_CLASS_CONDVAR, 0);
	cyg_instrument_disable(CYG_INSTRUMENT_CLASS_BINSEM, 0);
	cyg_instrument_disable(CYG_INSTRUMENT_CLASS_CNTSEM, 0);
	cyg_instrument_disable(CYG_INSTRUMENT_CLASS_CLOCK, 0);
	cyg_instrument_disable(CYG_INSTRUMENT_CLASS_ALARM, 0);
	cyg_instrument_disable(CYG_INSTRUMENT_CLASS_MBOXT, 0);
	cyg_instrument_disable(CYG_INSTRUMENT_CLASS_SMP, 0);
	cyg_instrument_disable(CYG_INSTRUMENT_CLASS_MLQ, 0);
}
#else
/* When it is selective, we usually enable THREAD_SWITCH event only */
void GL_OS_Monitor_UnmaskAll()
{
	cyg_instrument_enable(CYG_INSTRUMENT_CLASS_THREAD, CYG_INSTRUMENT_EVENT_THREAD_SWITCH);
}

void GL_OS_Monitor_MaskAll()
{
	cyg_instrument_disable(CYG_INSTRUMENT_CLASS_THREAD, CYG_INSTRUMENT_EVENT_THREAD_SWITCH);
}
#endif // CYGDBG_KERNEL_INSTRUMENT_FLAGS & CYG_INSTRUMENT_CLASS_SCHED

void GL_OS_Monitor_Enable()
{
	instrument_switch = 1;
}

void GL_OS_Monitor_Disable()
{
	instrument_switch = 0;
}

struct Instrument_Record {
	CYG_WORD16 type;			// record type
	CYG_WORD16 thread;			// current thread id
	CYG_WORD timestamp;			// 32 bit timestamp
	CYG_WORD arg1;				// first arg
	CYG_WORD arg2;				// second arg
};

/* Used by instrument functions in ecos */
struct Instrument_Record instrument_buffer[INSTRUMENT_MAX_RECORD];
cyg_uint32 instrument_buffer_size = INSTRUMENT_MAX_RECORD;

void GL_OS_Monitor_Print_Records(unsigned int num)
{
	unsigned int last, i;
	/*
	 * We should stop recording instrument events. Otherwise, new events
	 * may come and overwrite old records while printing.
	 */
	INSTRUMENT_DISABLE();

	if (num == 0 || num > INSTRUMENT_MAX_RECORD) {
		num = INSTRUMENT_MAX_RECORD;
	}
	/*
	 * There is no pointer pointing to the last record,
	 * so it needs to find last record first.
	 */
	for (last = 0; last < INSTRUMENT_MAX_RECORD; ++last) {
		if (&(instrument_buffer[last]) == instrument_buffer_pointer) {
			break;
		}
	}
	/*
	 * instrument_buffer_pointer points to the next unfilled record, so
	 * we subtract "last" from one to find last record
	 */
	last = (last - 1) % INSTRUMENT_MAX_RECORD;

	diag_printf("%4s%12s%20s%10s%12s%12s\n", "Idx", "Time", "Type", "Thread ID", "Arg1", "Arg2");
	diag_printf("==== =========== =================== ========= =========== ===========\n");
	for (i = 0; i < num; ++i) {
		diag_printf("%4d%12d%20s%10d  0x%08X  0x%08X\n",
					last,
					instrument_buffer[last].timestamp,
					cyg_instrument_msg(instrument_buffer[last].type),
					instrument_buffer[last].thread,
					instrument_buffer[last].arg1,
					instrument_buffer[last].arg2);
		if (last == 0) {
			last = INSTRUMENT_MAX_RECORD - 1;
		} else {
			--last;
		}
	}
	INSTRUMENT_ENABLE();
}
#else
void GL_OS_Monitor_UnmaskAll()
{
	/* do nothing */
}

void GL_OS_Monitor_MaskAll()
{
	/* do nothing */
}

void GL_OS_Monitor_Enable()
{
	/* do nothing */
}

void GL_OS_Monitor_Disable()
{
	/* do nothing */
}

void GL_OS_Monitor_Print_Records(unsigned int num)
{
	/* do nothing */
}
#endif

