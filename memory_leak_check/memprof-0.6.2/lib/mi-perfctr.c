/* -*- mode: C; c-file-style: "linux" -*- */

/* MemProf -- memory profiler and leak detector
 * Copyright 1999, 2000, 2001, Red Hat, Inc.
 * Copyright 2002, Kristian Rietveld
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
/*====*/

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ucontext.h>
#include <unistd.h>

#include "perfctr.h"

#include "intercept.h"
#include "mi-perfctr.h"
#include "memintercept.h"
#include "memintercept-utils.h"
#include "stack-frame.h"

static int perfctr_fd = -1;

#undef ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define MI_DEBUG(arg) mi_debug arg
#else /* !ENABLE_DEBUG */
#define MI_DEBUG(arg) (void)0
#endif /* ENABLE_DEBUG */

/* External declaration */
/* XXX hmm.. */
#define SIGHANDLER_FRAMES 2

static void
sigprof_handler (int unused, siginfo_t *si, ucontext_t *ucontext)
{
    int saved_errno = errno;
    struct sigcontext *ctx = (struct sigcontext *)&ucontext->uc_mcontext;
    MIInfo info;

    info.alloc.operation = MI_TIME;
    info.alloc.old_ptr = NULL;
    info.alloc.new_ptr = NULL;
    info.alloc.size = 1;
    
    mi_call_with_signal_backtrace ((void *)ctx->EIPRIP, (void *)ctx->EBPRBP,
					(void *)ctx->ESPRSP, mi_write_stack, &info);

    if (ioctl (perfctr_fd, VPERFCTR_IRESUME) < 0)
	    mi_perror ("Error restarting handler interrupt");

    errno = saved_errno;
}

static MIBool
init_sighandler (void)
{
    struct sigaction sa;
    sa.sa_handler = (void (*)(int))sigprof_handler;
    sigemptyset (&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    if (sigaction (SIGPROF, &sa, NULL) != 0) {
	    mi_perror ("Error setting up signal handler\n");
	    return MI_FALSE;
    }

    return MI_TRUE;
}

MIBool
mi_perfctr_start (int interval)
{
	struct perfctr_info info;
	struct vperfctr_control control;
	
	MI_DEBUG (("Turning on performance monitoring timer support\n"));

	if ((perfctr_fd = open ("/proc/self/perfctr", O_RDONLY)) < 0) {
		mi_perror ("Error opening /proc/self/perfctr");
		goto bail;
	}

	if (ioctl (perfctr_fd, PERFCTR_INFO, &info) < 0) {
		mi_perror ("Error getting perfctr information");
		goto bail;
	}

	if (!(info.cpu_features & PERFCTR_FEATURE_PCINT)) {
		mi_debug ("Performance monitoring interrupts not found\n");
		goto bail;
	}

	if (info.cpu_type != PERFCTR_X86_INTEL_PII &&
	    info.cpu_type != PERFCTR_X86_INTEL_PIII) {
		mi_debug ("Only support PII and PIII performance monitoring counters, found %d\n", info.cpu_type);
		goto bail;
	}

	memset(&control, 0, sizeof control);
	
	control.si_signo = SIGPROF;
	control.cpu_control.nractrs = 0;
	control.cpu_control.nrictrs = 1;
	control.cpu_control.tsc_on = 1;
	control.cpu_control.pmc_map[0] = 0;
	/* CPU_CLK_UNHALTED, USR, ENable, INT */
	control.cpu_control.evntsel[0] = 0x79 | (1 << 16) | (1 << 22) | (1 << 20);
	control.cpu_control.ireset[0] = - (int) (0.5 + (info.cpu_khz / 1000.) * interval);

	if (ioctl (perfctr_fd, VPERFCTR_CONTROL, &control) < 0) {
		mi_perror ("Error setting up performance monitoring counters");
		goto bail;
	}

	return init_sighandler ();
	
 bail:
	if (perfctr_fd != -1)
		close (perfctr_fd);

	return MI_FALSE;
}

void
mi_perfctr_stop (void)
{
}
