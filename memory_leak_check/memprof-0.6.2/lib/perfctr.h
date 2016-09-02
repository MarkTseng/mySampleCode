/* Extracts from the perfctr sources of the parts of the
 * linux/perfctr.h and asm-i386/perfctr.h that we need.
 * 
 * Trying to depend on the user-space library for perfctr
 * looks like it will be more trouble than its worth,
 * and doesn't really isolate us from the hard parts.
 *
 * Copyright (C) 1999-2002  Mikael Pettersson
 */

struct perfctr_sum_ctrs {
	unsigned long long tsc;
	unsigned long long pmc[18];
};

struct perfctr_low_ctrs {
	unsigned int tsc;
	unsigned int pmc[18];
};

struct perfctr_cpu_control {
	unsigned int tsc_on;
	unsigned int nractrs;		/* # of a-mode counters */
	unsigned int nrictrs;		/* # of i-mode counters */
	unsigned int pmc_map[18];
	unsigned int evntsel[18];	/* one per counter, even on P5 */
	unsigned int evntsel_aux[18];	/* e.g. P4 ESCR contents */
	struct {
		unsigned int pebs_enable;	/* for replay tagging */
		unsigned int pebs_matrix_vert;	/* for replay tagging */
	} p4;
	int ireset[18];			/* <= 0, for i-mode counters */
};

struct perfctr_cpu_state {
	unsigned int cstatus;
	union {
		unsigned int p5_cesr;
		unsigned int id;	/* cache owner id */
	} k1;
	struct perfctr_sum_ctrs sum;
	struct perfctr_low_ctrs start;
	struct perfctr_cpu_control control;
	struct {
		unsigned int p4_escr_map[18];
		const void *isuspend_cpu;
	} k2;
};

struct perfctr_info {
	char version[32];
	unsigned char nrcpus;
	unsigned char cpu_type;
	unsigned char cpu_features;
	unsigned long cpu_khz;
};

/* cpu_type values */
#define PERFCTR_X86_GENERIC	0	/* any x86 with rdtsc */
#define PERFCTR_X86_INTEL_P5	1	/* no rdpmc */
#define PERFCTR_X86_INTEL_P5MMX	2
#define PERFCTR_X86_INTEL_P6	3
#define PERFCTR_X86_INTEL_PII	4
#define PERFCTR_X86_INTEL_PIII	5
#define PERFCTR_X86_CYRIX_MII	6
#define PERFCTR_X86_WINCHIP_C6	7	/* no rdtsc */
#define PERFCTR_X86_WINCHIP_2	8	/* no rdtsc */
#define PERFCTR_X86_AMD_K7	9
#define PERFCTR_X86_VIA_C3	10	/* no pmc0 */
#define PERFCTR_X86_INTEL_P4	11	/* model 0 and 1 */
#define PERFCTR_X86_INTEL_P4M2	12	/* model 2 and above */

/* cpu_features flag bits */
#define PERFCTR_FEATURE_RDPMC	0x01
#define PERFCTR_FEATURE_RDTSC	0x02
#define PERFCTR_FEATURE_PCINT	0x04

/* user's view of mmap:ed virtual perfctr */
struct vperfctr_state {
	unsigned int magic;
	int si_signo;
	struct perfctr_cpu_state cpu_state;
};

/* `struct vperfctr_state' binary layout version number */
#define VPERFCTR_STATE_MAGIC	0x0201	/* 2.1 */
#define VPERFCTR_MAGIC	((VPERFCTR_STATE_MAGIC<<16)|PERFCTR_CPU_STATE_MAGIC)

/* parameter in VPERFCTR_CONTROL command */
struct vperfctr_control {
	int si_signo;
	struct perfctr_cpu_control cpu_control;
};

#include <linux/ioctl.h>
#define _PERFCTR_IOCTL	0xD0	/* 'P'+128, currently unassigned */

#define PERFCTR_INFO		_IOR(_PERFCTR_IOCTL,0,struct perfctr_info)

#define VPERFCTR_SAMPLE		 _IO(_PERFCTR_IOCTL,1)
#define VPERFCTR_UNLINK		 _IO(_PERFCTR_IOCTL,2)
#define VPERFCTR_CONTROL	_IOW(_PERFCTR_IOCTL,3,struct vperfctr_control)
#define VPERFCTR_STOP		 _IO(_PERFCTR_IOCTL,4)
#define VPERFCTR_IRESUME	 _IO(_PERFCTR_IOCTL,5)
