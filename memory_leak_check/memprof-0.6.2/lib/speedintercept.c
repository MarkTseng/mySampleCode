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

#define _GNU_SOURCE

#undef ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define MI_DEBUG(arg) mi_debug arg
#else /* !ENABLE_DEBUG */
#define MI_DEBUG(arg) (void)0
#endif /* ENABLE_DEBUG */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <linux/rtc.h>

#include "intercept.h"
#include "memintercept.h"
#include "memintercept-utils.h"
#include "mi-perfctr.h"
#include "stack-frame.h"

static void init_sighandler (int signum);

static int timer_fd = -1;
static int profile_interval = 10000; /* In usecs */

typedef enum {
	SPEED_PROF_RTC,
	SPEED_PROF_PERFCTR,
	SPEED_PROF_ITIMER
} SpeedProfType;

static SpeedProfType profile_type;

static MIBool
start_rtc_timer (void)
{
	int flags;
	int irq_rate;
    
	MI_DEBUG (("Turning on RTC timer\n"));

	if ((timer_fd = open ("/dev/rtc", O_RDONLY)) < 0) {
		mi_perror ("Error opening /dev/rtc");
		return MI_FALSE;
	}

	flags = fcntl (timer_fd, F_GETFL);
	if (fcntl (timer_fd, F_SETFL, flags | FASYNC) < 0) {
		mi_perror ("Error setting FASYNC");
		goto bail;
	}
	if (fcntl (timer_fd, F_SETOWN, getpid ()) < 0) {
		mi_perror ("Error setting owner for SIGIO");
		goto bail;
	}
	if (fcntl (timer_fd, F_SETSIG, SIGPROF) < 0) {
		mi_perror ("Error setting signal for /dev/rtc");
		goto bail;
	}

	irq_rate = 1;
	while (1000000 / irq_rate > profile_interval &&
	       irq_rate < 8192)
		irq_rate *= 2;
	
	if (ioctl (timer_fd, RTC_IRQP_SET, irq_rate) < 0) {
		mi_perror ("Error setting interrupt rate");
		goto bail;
	}
	if (ioctl (timer_fd, RTC_PIE_ON) < 0) {
		mi_perror ("Error turning on interrupts");
		goto bail;
	}

	init_sighandler (SIGPROF);
	return MI_TRUE;

 bail:
	close (timer_fd);
	return MI_FALSE;
}

static void
stop_rtc_timer (void)
{
	if (timer_fd !=  -1) {
		if (ioctl (timer_fd, RTC_PIE_OFF) < 0)
			mi_perror ("Error turning off interrupts");

		if (close (timer_fd) < 0)
			mi_perror ("Error closing /dev/rtc");
	}
}

static int profile_itimer_type;

static MIBool
reset_itimer_timer (void)
{
	struct itimerval it, tem;
	it.it_interval.tv_usec = 0;
	it.it_interval.tv_sec = 0;
	it.it_value.tv_usec = profile_interval % 1000000;
	it.it_value.tv_sec = profile_interval / 1000000;
	if (setitimer (profile_itimer_type, &it, &tem) != 0) {
		mi_perror ("Error setting up itimer");
		return MI_FALSE;
	} else {
		return MI_TRUE;
	}
}

static MIBool
start_itimer_timer (int timer_type)
{
	profile_itimer_type = timer_type;
	if (!reset_itimer_timer ())
		return MI_FALSE;

	init_sighandler (timer_type == ITIMER_PROF ? SIGPROF : SIGALRM);

	return MI_TRUE;
}

/* XXX hmm.. */
#define SIGHANDLER_FRAMES 2

static void
#if defined (__linux__)
sigprof_handler (int unused, struct sigcontext ctx)
#else
sigprof_handler (int unused)
#endif
{
	int saved_errno = errno;
	MIInfo info;

	info.alloc.operation = MI_TIME;
	info.alloc.old_ptr = NULL;
	info.alloc.new_ptr = NULL;
	info.alloc.size = 1;
    
#if defined (__linux__)
	mi_call_with_signal_backtrace ((void *)ctx.EIPRIP, (void *)ctx.EBPRBP,
					(void *)ctx.ESPRSP, mi_write_stack, &info);
#else
	mi_call_with_backtrace (SIGHANDLER_FRAMES, saved_pc, mi_write_stack, &info);
#endif

	if (profile_type == SPEED_PROF_ITIMER)
		reset_itimer_timer ();
	errno = saved_errno;
}

static void
init_sighandler (int signum)
{
	struct sigaction sa;
	sa.sa_handler = (void (*)(int))sigprof_handler;
	sigemptyset (&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction (signum, &sa, 0) != 0)
		mi_perror ("Error setting up signal handler");
}

void
mi_init (void)
{
}

void
mi_start (void)
{
	const char *type_string;
	const char *interval_string;
	char *end;
	int success = 1;
	
	interval_string = getenv ("_MEMPROF_INTERVAL");
	if (!interval_string)
		interval_string = "10000";

	profile_interval = strtol (interval_string, &end, 10);
	if (*interval_string == '\0' || *end != '\0' || profile_interval <= 0) {
		mi_debug ("Invalid interval %s\n", interval_string);
		profile_interval = 10000;
	}
	
	type_string = getenv ("_MEMPROF_SPEED_TYPE");
	if (!type_string)
		type_string = "cycles";

	if (strcmp (type_string, "time") == 0) {
		if (start_rtc_timer ())
			profile_type = SPEED_PROF_RTC;
 		else if (start_itimer_timer (ITIMER_REAL))
			profile_type = SPEED_PROF_ITIMER;
		else
			success = 0;

	} else if (strcmp (type_string, "cycles") == 0) {
		if (mi_perfctr_start (profile_interval))
			profile_type = SPEED_PROF_PERFCTR;
		else if (start_itimer_timer (ITIMER_PROF))
			profile_type = SPEED_PROF_ITIMER;
		else
			success = 0;
	} else {
		mi_debug ("Unknown value for _MEMPROF_SPEED_TYPE: %s\n");
		_exit (1);
	}

	if (!success) {
		mi_debug ("Couldn't start timer\n");
		_exit (1);
	}
}

void
mi_stop (void)
{
	switch (profile_type) {
	case SPEED_PROF_RTC:
		stop_rtc_timer ();
		break;
	case SPEED_PROF_PERFCTR:
		mi_perfctr_stop ();
		break;
	case SPEED_PROF_ITIMER:
		break;
	}
}
