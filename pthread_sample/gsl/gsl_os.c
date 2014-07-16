#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mcheck.h>
#include <gsl.h>
#include "gl_common.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <pthread.h>
#include <gsl/lambo86_hwop.h>

// by hanjun added
#define GSLOS_INITDMA	0
#define GSLOS_INITINTERRUPT	0
#define GSLOS_MEMORYPART	0
#define STACK_PROTECTOR   // by hanjun
#define GSLONLYUSING	0
#define GSLCURRENTCANNOTFUNC 0
//#define CYGDBG_KERNEL_THREAD_USAGE  // by hanjun

typedef struct gl_thread_info
{
	GL_TASK_T	handle;
	GL_UINT16	id;
	GL_INT32 	state;
	GL_INT8		*name;
	GL_UINT32	set_pri;
	GL_UINT32 	cur_pri;
	GL_UINT32	stack_base;
	GL_UINT32	stack_size;
	GL_UINT32	stack_used;
}gl_thread_info_t;

#if GSLONLYUSING
static GL_INT8 gl_thread_get_ext_info(GL_TASK_T thread);
static GL_INT32 gl_thread_reset_usage( GL_TASK_T thread );
static GL_INT32 gl_thread_get_usage( GL_TASK_T thread );
static GL_INT32 gl_thread_get_next(GL_TASK_T *thread, GL_UINT16 *id);
static GL_INT32 gl_thread_get_info(GL_TASK_T thread,GL_UINT16 id, GL_THREAD_INFO_T *info );
static GL_INT32 gl_thread_get_stack_base(GL_TASK_T self, void *stackaddr);
static GL_INT32 gl_thread_get_stack_size(GL_TASK_T self);
static pid_t gl_thread_get_id( void );
static GL_TASK_T gl_pthread_self( void );
#endif

static pthread_mutex_t gSchedulerLockMutex = PTHREAD_MUTEX_INITIALIZER;

GL_INT32 gl_uart_channel = GL_UART_CHANNEL;
const GL_INT8 *gModuleName[ID_END + 1] = {
	"ECOS",
	"M_KERNEL",
	"FILESYSTEM",
	"DRAM_B",
	"PLATFORM",
	"DRIVERS",
	"MW",
	"GUI_ENG",
	"APP_INFRA",
	"APP",
	"UNICODE",
	"GAME",
	"UNKNOWN"
};
/*
 * To know if system is already initialized.
 * We use it to check if LoadModule is executed after system initialized.
 */
static unsigned char bSysInit = 0;


inline void enter_critical_section(int spin)
{
	GL_SchedulerLock();
	//while(HWREG_R(tset[spin])==0);
	return;
}

inline void leave_critical_section(int spin)
{
	//HWREG_W(tset[spin], 0);
	GL_SchedulerUnLock();
	return;
}

void GL_DelayUs(GL_INT32 us)
{
	UINT32 val1, val2;
	UINT32 diff;
	INT32 usticks;
	INT32 ticks;

    // Calculate the number of counter register ticks per microsecond.
	usticks = ((GL_GetSysBusFreq() * GL_GetCpuFreqMult()) / 1000000) / 2;

    /* Make sure that the value is not zero. This will only happen if the
     * CPU is running at < 2MHz.
	 */
	if (usticks == 0) {
		usticks = 1;
	}

	while( us > 0 ) {
		int us1 = us;
        /* Wait in bursts of less than 10000us to avoid any overflow
         * problems in the multiply.
		 */
        if (us1 > 10000) {
			us1 = 10000;
		}
		us -= us1;
		ticks = us1 * usticks;
		val1 = GL_GetCpuCnt();
		while (ticks > 0) {
			do {
				val2 = GL_GetCpuCnt();
			} while (val1 == val2);
			asm volatile (
				"move	$8, %1;"
				"move	$9, %2;"
				"subu	%0, $9, $8;"
				: "=r"(diff)
				: "r"(val1), "r"(val2)
				: "$8", "$9");
			if ((GL_UINT32)ticks < diff) {
				ticks = 0;
				break;
			}
			ticks -= diff;
			val1 = val2;
		}
	}
}

GL_Status_t GL_GetFileSize(GL_INT32 fd, GL_UINT32 *pFileSize)
{
	struct stat stStatFile = { 0 };

	if ((fstat(fd, &stStatFile)) < 0) {
		// fail to get size
		GL_TraceInfo("Get wrong size\n");
		return GL_FAILURE;
	}
	if (pFileSize) {
		*pFileSize = (GL_UINT32)stStatFile.st_size;
	}
	return GL_SUCCESS;
}

GL_Status_t GL_GetFileSize64(GL_INT32 fd, GL_UINT64 *pFileSize)
{
	struct stat stStatFile = { 0 };

	if ((fstat(fd, &stStatFile)) < 0) {
		// fail to get size
		GL_TraceInfo("Get wrong size\n");
		return GL_FAILURE;
	}
	if (pFileSize) {
		*pFileSize = (GL_UINT64)stStatFile.st_size;
	}
	return GL_SUCCESS;
}

//extern GL_Queue_t Kill_Thread_QueueId;
GL_Queue_t Kill_Thread_QueueId;  // by hanjun
extern struct gl_intr_t gl_intrpool[];
GL_Status_t GL_Init(void)
{
#ifdef MEMORY_TRACE
	{
		int ret;
		ret = setenv("MALLOC_TRACE", MEMORY_TRACE_LOG_FILE, 1);
		if (0 != ret) {
			printf("Cannot set MALLOC_TRACE=%s\n", MEMORY_TRACE_LOG_FILE);
		}
		mtrace();
	}
#endif
#ifdef MEMORY_WATCH
	mwInit();
	mwStatistics(2);
#endif

	if (lambo86_hw_open() != 0) {
		return GL_FAILURE;
	}

	GL_InitMem();

	extern struct gl_intr_t gl_intrpool[];
	GL_IntrInit(gl_intrpool);

	GL_ExceptInit();

	GL_DmaInit();

	return GL_SUCCESS;
}

GL_Status_t GL_InitMem(void)
{
    GL_Status_t ret = 0;

#if (GL_BGET_TEST_ON_PC == 0)
	lambo86_hw_init();
#endif
    ret = bget_meminit();
	if (0 != ret) {
		diag_printf("%s: Error: bget init failed.\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

void GL_Uninit(void)
{
	gsl_printf("GSL uninit ...\n");

	GL_DmaUninit();

	GL_IntrUninit(gl_intrpool);

	GL_TaskUninit();

	bget_memdeinit();

	lambo86_hw_release();

#ifdef MEMORY_TRACE
	printf("Terminate mtrace ...\n");
	muntrace();
#endif
#ifdef MEMORY_WATCH
	printf("Terminate memwatch ...\n");
	mwAbort();
#endif
}

#if 0
GL_Status_t GL_Init(void)
{
#if GSLOS_INITDMA
	extern void sample_dramb(void);
	sample_dramb();
#endif
	//extern GL_Task_t gl_kill_thread_task;
	GL_Task_t gl_kill_thread_task; // by hanjun
	GL_Status_t ret = GL_FAILURE;

	GL_CHECK_BLOCK_IN_INTR();

//	GL_PthreadInit();  //in posix/gl_pthread.c
//	gl_SchedulerLockmetex_init();

#if GSLOS_INITINTERRUPT
	extern void GL_ExceptInit(void);
	GL_ExceptInit();
#endif
	ret = GL_QueueCreate("self delete", 4, 10, 0, &Kill_Thread_QueueId);
	if (ret != GL_SUCCESS) {
		GL_TraceInfo(" self delete fail \n");
		return ret;
	}
//	gl_kill_thread_task = GL_TaskPoolGetHandle("GSL_Kill_Thread");
//	ret = GL_TaskPoolActivate(gl_kill_thread_task);
	if (ret != GL_SUCCESS) {
		GL_TraceInfo(" GSL Kill Thread fail \n");
		return ret;
	}
#ifdef CYGDBG_KERNEL_THREAD_USAGE
	thread_stat_id = GL_TaskPoolGetHandle("Thread_Usage");
	GL_TaskPoolActivate(thread_stat_id);
#endif
#ifdef BUFFERED_UART
	buffered_uart_init();
#endif
#if GSLOS_INITDMA
	GL_DmaInit();
#endif
#if GSLOS_INITINTERRUPT
	unmap_addr_check_init();
#endif
#if GSLOS_INITDMA
	dram_monitor_init();
#endif
	return ret;
}
#endif
GL_Status_t GL_InitEnd(void)
{
	bSysInit = 1;
	return GL_SUCCESS;
}

unsigned char GL_IsSysInit(void)
{
	return bSysInit;
}
static GL_UINT32 crc_table[256] = {
	0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B,
	0x1A864DB2, 0x1E475005, 0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61,
	0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD, 0x4C11DB70, 0x48D0C6C7,
	0x4593E01E, 0x4152FDA9, 0x5F15ADAC, 0x5BD4B01B, 0x569796C2, 0x52568B75,
	0x6A1936C8, 0x6ED82B7F, 0x639B0DA6, 0x675A1011, 0x791D4014, 0x7DDC5DA3,
	0x709F7B7A, 0x745E66CD, 0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039,
	0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5, 0xBE2B5B58, 0xBAEA46EF,
	0xB7A96036, 0xB3687D81, 0xAD2F2D84, 0xA9EE3033, 0xA4AD16EA, 0xA06C0B5D,
	0xD4326D90, 0xD0F37027, 0xDDB056FE, 0xD9714B49, 0xC7361B4C, 0xC3F706FB,
	0xCEB42022, 0xCA753D95, 0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1,
	0xE13EF6F4, 0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D, 0x34867077, 0x30476DC0,
	0x3D044B19, 0x39C556AE, 0x278206AB, 0x23431B1C, 0x2E003DC5, 0x2AC12072,
	0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16, 0x018AEB13, 0x054BF6A4,
	0x0808D07D, 0x0CC9CDCA, 0x7897AB07, 0x7C56B6B0, 0x71159069, 0x75D48DDE,
	0x6B93DDDB, 0x6F52C06C, 0x6211E6B5, 0x66D0FB02, 0x5E9F46BF, 0x5A5E5B08,
	0x571D7DD1, 0x53DC6066, 0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA,
	0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E, 0xBFA1B04B, 0xBB60ADFC,
	0xB6238B25, 0xB2E29692, 0x8AAD2B2F, 0x8E6C3698, 0x832F1041, 0x87EE0DF6,
	0x99A95DF3, 0x9D684044, 0x902B669D, 0x94EA7B2A, 0xE0B41DE7, 0xE4750050,
	0xE9362689, 0xEDF73B3E, 0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2,
	0xC6BCF05F, 0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686, 0xD5B88683, 0xD1799B34,
	0xDC3ABDED, 0xD8FBA05A, 0x690CE0EE, 0x6DCDFD59, 0x608EDB80, 0x644FC637,
	0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB, 0x4F040D56, 0x4BC510E1,
	0x46863638, 0x42472B8F, 0x5C007B8A, 0x58C1663D, 0x558240E4, 0x51435D53,
	0x251D3B9E, 0x21DC2629, 0x2C9F00F0, 0x285E1D47, 0x36194D42, 0x32D850F5,
	0x3F9B762C, 0x3B5A6B9B, 0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF,
	0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623, 0xF12F560E, 0xF5EE4BB9,
	0xF8AD6D60, 0xFC6C70D7, 0xE22B20D2, 0xE6EA3D65, 0xEBA91BBC, 0xEF68060B,
	0xD727BBB6, 0xD3E6A601, 0xDEA580D8, 0xDA649D6F, 0xC423CD6A, 0xC0E2D0DD,
	0xCDA1F604, 0xC960EBB3, 0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7,
	0xAE3AFBA2, 0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B, 0x9B3660C6, 0x9FF77D71,
	0x92B45BA8, 0x9675461F, 0x8832161A, 0x8CF30BAD, 0x81B02D74, 0x857130C3,
	0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640, 0x4E8EE645, 0x4A4FFBF2,
	0x470CDD2B, 0x43CDC09C, 0x7B827D21, 0x7F436096, 0x7200464F, 0x76C15BF8,
	0x68860BFD, 0x6C47164A, 0x61043093, 0x65C52D24, 0x119B4BE9, 0x155A565E,
	0x18197087, 0x1CD86D30, 0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC,
	0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088, 0x2497D08D, 0x2056CD3A,
	0x2D15EBE3, 0x29D4F654, 0xC5A92679, 0xC1683BCE, 0xCC2B1D17, 0xC8EA00A0,
	0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB, 0xDBEE767C, 0xE3A1CBC1, 0xE760D676,
	0xEA23F0AF, 0xEEE2ED18, 0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4,
	0x89B8FD09, 0x8D79E0BE, 0x803AC667, 0x84FBDBD0, 0x9ABC8BD5, 0x9E7D9662,
	0x933EB0BB, 0x97FFAD0C, 0xAFB010B1, 0xAB710D06, 0xA6322BDF, 0xA2F33668,
	0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4
};

GL_UINT32 GL_CRC32(GL_UINT8 *pBuffer, GL_UINT32 dSize)
{
	register GL_UINT32 i;
	GL_UINT32 crc = 0xFFFFFFFF;

	for (i = 0; i < dSize; i++) {
		crc = (crc << 8) ^ crc_table[((crc >> 24) ^ *pBuffer++) & 0xFF];
	}

	return crc;
}
GL_UINT32 GL_CRC32_Continus(GL_UINT8 *pBuffer, GL_UINT32 dSize,GL_UINT32 crc)
{
	register GL_UINT32 i;

	for (i = 0; i < dSize; i++) {
		crc = (crc << 8) ^ crc_table[((crc >> 24) ^ *pBuffer++) & 0xFF];
	}

	return crc;
}
void gsl_copy_name(GL_INT8 *dest, GL_INT8 *src)
{
	if (src && dest) {
		memset(dest, 0, GSL_NAME_LEN + 1);
		strncpy(dest, src, GSL_NAME_LEN);
	}
}
GL_INT32 GL_SchedulerLock( void )
{
	GL_CHECK_BLOCK_IN_INTR();
	return pthread_mutex_lock(&gSchedulerLockMutex);
}

GL_INT32 GL_SchedulerUnLock( void )
{
	GL_CHECK_BLOCK_IN_INTR();
	return pthread_mutex_unlock(&gSchedulerLockMutex);
}
#if GSLCURRENTCANNOTFUNC
#if GSLOS_MEMORYPART  //by hanjun memory remove
static void gl_Show_MemInfo(struct mallinfo *minfo)
{
	GL_TraceInfo("Total size    : %8d bytes\n",minfo->arena);
	GL_TraceInfo("Used          : %8d bytes\n", minfo->uordblks);
	GL_TraceInfo("Max malloc    : %8d bytes\n", minfo->history_maxmalloc);
	GL_TraceInfo("Num Small Blk : %8d\n", minfo->smblks);
	GL_TraceInfo("Free Small Blk: %8d bytes\n", minfo->fsmblks);
	GL_TraceInfo("Max Frag Num  : %8d\n", minfo->history_maxfragnum);
	GL_TraceInfo("Max Frag Size : %8d bytes\n", minfo->history_maxfragsize);
	GL_TraceInfo("Max Free Blk  : %8d bytes\n", minfo->maxfree);
	GL_TraceInfo("\n");
}

void GL_Show_MemInfo(void)
{
	struct mallinfo minfo = mallinfo();

	GL_TraceInfo("[Memory Allocation Info]\n");
	gl_Show_MemInfo(&minfo);
}

void GL_Show_MemInfoA(void)
{
	struct mallinfo minfo = mallinfoA();

	GL_TraceInfo("[Memory Allocation Info -- DRAM A]\n");
	gl_Show_MemInfo(&minfo);
}

void GL_Show_MemInfoB(void)
{
	struct mallinfo minfo = mallinfoB();

	GL_TraceInfo("[Memory Allocation Info -- DRAM B]\n");
	gl_Show_MemInfo(&minfo);
}
#endif

void GL_Show_TaskInfo(void)
{
	GL_TASK_T thread = 0;
	GL_UINT16 id = 0;
	GL_THREAD_INFO_T info;
	GL_INT32 percent, j = 0, max = 0, i = 0;

	while (gl_thread_get_next(&thread, &id)) {
		gl_thread_get_info(thread, id, &info);
		if (info.name == NULL) {
			info.name = "No Name";
		}
		//GL_TraceInfo("%3d ID:%3d, Hndl:0x%X Priority:%d, State:", i, info.id, thread, info.cur_pri);
		switch (info.state) {
		case 0:
			GL_TraceInfo("RUNNING");
			break;
		case 1:
			GL_TraceInfo("SLEEPING");
			break;
		case 2:
			GL_TraceInfo("COUNTSLEEP");
			break;
		case 3:
			GL_TraceInfo("SLEEPSET");
			break;
		case 4:
			GL_TraceInfo("SUSPENDED");
			break;
		case 8:
			GL_TraceInfo("CREATING");
			break;
		case 16:
			GL_TraceInfo("EXITED");
			break;
		default:
			GL_TraceInfo("Unknown (0x%X)", info.state);
			break;
		}
		GL_TraceInfo(", Name:%s\n", info.name);
		percent = (info.stack_used * 100) / info.stack_size;
		if (max < percent) {
			j = i;
			max = percent;
		}
		GL_TraceInfo("    Stack Base:0x%08X, Size:%d bytes, MaxUsed:%d bytes (%d%%)\n\n",
					info.stack_base, info.stack_size, info.stack_used, percent);
		i++;
	}
	GL_TraceInfo("========================================================================\n");
	GL_TraceInfo("The Max stack usage percentage: %d%%, Index:%2d\n", max, j);
}
#endif


#ifdef CYGDBG_KERNEL_THREAD_USAGE
#define THREAD_USAGE_DEFAULT_PERIOD	4000	// milli-seconds
GL_Task_t thread_stat_id = NULL;
static GL_UINT8 thread_stat_isprint = GL_FALSE;
static GL_UINT16 thread_stat_period = THREAD_USAGE_DEFAULT_PERIOD;

#if GSLOS_INITINTERRUPT
/* defined in ecos ".hxx" header files */
extern void cyg_intrrupt_reset_usage(void);
extern GL_UINT32 cyg_isr_usage;
extern GL_UINT32 cyg_dsr_usage;
#endif
#if 0//GSLONLYUSING
static const GL_INT8 *gl_GetBlockName(GL_UINT32 handle, GL_UINT8 type)
{
	switch (type) {
	case GL_BLOCK_QUEUE:
		return ((gsl_queue_t *)handle)->name;
	case GL_BLOCK_SEMA:
		return ((gsl_semaphore_t *)handle)->name;
	case GL_BLOCK_MUTEX:
		return ((gsl_mutex_t *)handle)->name;
	case GL_BLOCK_FLAG:
		return GL_SUCCESS;
		//return ((gsl_flag_t *)handle)->name;
	case GL_BLOCK_COND:
		return GL_SUCCESS;
		//return ((gsl_cond_t *)handle)->name;
	case GL_BLOCK_MEMPOOL_VAR:
		return GL_SUCCESS;
		//return ((gsl_mempool_var_t *)handle)->name;
	case GL_BLOCK_MEMPOOL_FIX:
		return GL_SUCCESS;
		//return ((gsl_mempool_fix_t *)handle)->name;
	default:			// Others
		return NULL;
	}
}
#endif
#if 0
#if GSLCURRENTCANNOTFUNC
GL_UINT8 GL_ThreadStatToggle(void)
{
	thread_stat_isprint ^= 1;
	return thread_stat_isprint;
}

void GL_ThreadStatSetRefreshTime(GL_UINT32 sec)
{
	/*
	 * The maximum value will be equal to:
	 *
	 * Max Period = 2 * (Max(WORD32) / CPU) seconds
	 *
	 * For example: CPU = 168 MHz, Max Period = 51 seconds
	 * But it is better not to use this long period.
	 */
	if (sec == 0) {
		sec = 4;
	} else if (sec > 30) {
		sec = 30;
	}
	thread_stat_period = sec * 1000;
}

void GL_Show_ThreadStat(GL_INT32 detail)
{
	GL_THREAD_INFO_T info;
	GL_TASK_T thread;
	GL_UINT16 id;
	GL_UINT64 usage;
	GL_UINT64 isr_usage, dsr_usage;
	GL_UINT64 cpu_usage = 0;
	GL_UINT32 self_usage = 0;
	GL_UINT32 intr_state;
	GL_UINT8 b_type;
	gsl_thread_block_info *blk_info;
	gsl_thread_ext_info *ext_info;
	const GL_INT8 *b_info_name;
	GL_UINT32 dNumThread = 0;
	GL_TASK_T thread_stat_handle;

	/* Comment this because exception handler may need this function. */
	// GL_CHECK_BLOCK_IN_ISR();

	GL_TraceInfo("P: Priority, S: State, B: Block Type\n");
	GL_TraceInfo("State: 0=RUNNING, 1=SLEEP, 2=COUNTSLEEP, 3=SLEEPSET\n");
	GL_TraceInfo("       4=SUSPEND, 8=CREATING, 16=EXITED, Other=Unknown\n");
	GL_TraceInfo("Block Type: N=None, M=Mutex, S=Semaphore, F=Flag, Q=Queue\n");
	GL_TraceInfo("            C=Condition Var, V=MempoolVar, X=MempoolFix\n");
	GL_TraceInfo("\n");
	GL_TraceInfo("%5s %6s %10s %2s %2s %2s %24s %29s %24s\n",
				"TID",
				"%CPU",
				"Handle",
				"P",
				"S",
				"B",
				"B_Info",
				"[Stk  Base   Size   Max( % )]",
				"Name");
	GL_TraceInfo("--------------------------------------------------------"
				"--------------------------------------------------------\n");

	if (NULL == thread_stat_id) {
		thread_stat_handle = (GL_TASK_T)NULL;
	} else {
		thread_stat_handle = ((gsl_thread_t *)thread_stat_id)->thread_handle;
	}

	/* To get precise isr/dsr usage value */
#if GSLOS_INITINTERRUPT
	HAL_DISABLE_INTERRUPTS(intr_state);
	isr_usage = cyg_isr_usage;
	dsr_usage = cyg_dsr_usage;
	cyg_intrrupt_reset_usage();
	HAL_RESTORE_INTERRUPTS(intr_state);
#endif
	/*
	 * 1. To prevent thread deletion whiling dumping thread info.
	 * 2. To assure no preemption happens, or the usage will be imprecise,
	 * especially when this function called by low priority threads.
	 */
	gl_scheduler_lock();

	/* get totoal usage first */
	thread = id = 0;
	while (gl_thread_get_next(&thread, &id)) {
		usage = gl_thread_get_usage(thread);
		/* To get precise self usage value */
		if (thread == thread_stat_handle) {
			self_usage = usage;
			gl_thread_reset_usage(thread);
		}
		cpu_usage += usage;
		++dNumThread;
	}

	cpu_usage = (cpu_usage + isr_usage + dsr_usage) / 100;
	if (0 == cpu_usage) {
		cpu_usage = 1;
	}

	thread = id = 0;
	while (gl_thread_get_next(&thread, &id)) {
		if (thread == thread_stat_handle) {
			usage = self_usage;
		} else {
			usage = gl_thread_get_usage(thread);
			gl_thread_reset_usage(thread);
		}
		gl_thread_get_info(thread, id, &info);
		if (info.name == NULL) {
			info.name = "No Name";
		}
		// th = (gsl_thread_t *)(info.stack_base - sizeof(gsl_thread_t));
		ext_info = (gsl_thread_ext_info *)gl_thread_get_ext_info(thread);
		b_type = GL_BLOCK_NONE;
		b_info_name = " ";
		blk_info = NULL;
		if (NULL != ext_info) {
			blk_info = ext_info->block_info;
			if (NULL != blk_info) {
				b_type = blk_info->type;
				b_info_name = gl_GetBlockName(blk_info->handle, b_type);
				blk_info = blk_info->next;
			}
		}
#if 0
		GL_TraceInfo("%5d %2lld.%03lld 0x%08X %2d %2d  %c %24s 0x%8x %6d %6d(%3d) %24s\n",
					id,
					usage / cpu_usage,
					GL_GET_FRACTION(usage, cpu_usage, 1000),
					thread,
					info.cur_pri,
					info.state,
					b_type,
					b_info_name,
					info.stack_base,
					info.stack_size,
					info.stack_used,
					(info.stack_used * 100) / info.stack_size,
					info.name);
#endif
		if (1 == detail) {
			if (NULL != blk_info) {
				while (NULL != blk_info) {
					const char *name = gl_GetBlockName(blk_info->handle, blk_info->type);
					GL_TraceInfo("%31s%c %24s 0x%08x\n", " ", blk_info->type, name, blk_info->handle);
					blk_info = blk_info->next;
				}
			}
		}
	}

	/* The scheduler will not affect the below actions so unlock it. */
	gl_scheduler_unlock();

//	GL_TraceInfo("  ISR %2lld.%03lld\n", isr_usage / cpu_usage, GL_GET_FRACTION(isr_usage, cpu_usage, 1000));
//	GL_TraceInfo("  DSR %2lld.%03lld\n", dsr_usage / cpu_usage, GL_GET_FRACTION(dsr_usage, cpu_usage, 1000));
	GL_TraceInfo("\n");
	GL_TraceInfo("Number of threads: %d\n", dNumThread);
	GL_TraceInfo("\n");
}

void GL_ThreadStat(void *data)
{
	GL_TASK_T thread;
	GL_UINT16 id;

	while (1) {
		GL_TaskSleep(thread_stat_period);
		if (!thread_stat_isprint) {
			/* reset */
			thread = id = 0;
			while (gl_thread_get_next(&thread, &id)) {
				gl_thread_reset_usage(thread);
			}
#if GSLOS_INITINTERRUPT
			cyg_intrrupt_reset_usage();
#endif
			continue;
		}
		GL_Show_ThreadStat(0);
	}
}
#endif
#else
GL_UINT8 GL_ThreadStatToggle(void)
{
	return GL_FALSE;
}

void GL_ThreadStatSetRefreshTime(GL_UINT32 sec)
{
	GL_TraceInfo("Thread Usage isn't supported in this eCos lib!\n");
}

void GL_Show_ThreadStat(GL_INT32 detail)
{
	GL_TraceInfo("Thread Usage isn't supported in this eCos lib!\n");
}
#endif
#endif


//by hanjun suggest removing to C file of hardware
#if GSLCURRENTCANNOTFUNC
GL_UINT32 gl_uart_init(void)
{
	/*
	 * when there's GDB, don't use attach uart interrupt
	 */
#if !defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)
	HWREG_1(uart0_isc, 1 << 5);
#if (!defined(IC_SIMPLE_TEST_ASIC) && !defined(IC_SIMPLE_TEST_FPGA) && (GL_UART_CHANNEL == 0)) && defined(WORKAROUND_UART0_RXFIFO)
	GL_IntrAttach(intr_uart0_hnd);
	GL_IntrEnable(intr_uart0_hnd);
#endif
	return 0;
#else
	return -1;
#endif
}
#endif

#if GSLONLYUSING
#ifdef STACK_PROTECTOR
void __wrap___stack_chk_fail(void)
{
	GL_TraceInfo("\n");
	GL_TraceInfo("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	GL_TraceInfo("~               stack smashing detected !!!            ~\n");
	GL_TraceInfo("~      The contents of stack are smashed somehow.      ~\n");
	GL_TraceInfo("~                 Please check your code.              ~\n");
	GL_TraceInfo("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	GL_TraceInfo("Error occurs in 0x%08x \n",(GL_UINT32)__builtin_return_address(0) - 8);
	GL_TraceInfo("\n");

	// asm volatile ("break");  // by hanjun

	while (1) {
		continue;
        }
}
#endif

/* Callback function in timer DSR */
void GL_TimeTick(void)
{
#if defined(SERVO_SUPPORT)
	extern void ServoIsr(void);
	ServoIsr();
#endif
}
/*
 * System Service
 */
int GL_Uncompress(GL_INT8 *dest, GL_UINT32 *destLen, GL_INT8 *source, GL_UINT32 sourceLen )
{
	return uncompress(dest, destLen, source, sourceLen);   //by hanjun
}
GL_UINT32 GL_TaskGetCurStackUsage(int bAutoPrint)
{
#if GL_DEBUG
	GL_UINT32 u32StackUsage = 0;
	GL_TASK_T self = gl_pthread_self();   ///hanjun
	GL_INT8 *p;
	gl_thread_get_stack_base(self, &p);
	GL_INT32 size = gl_thread_get_stack_size(self);

	u32StackUsage = (GL_UINT32)p + size - (GL_UINT32)&u32StackUsage;

	if (bAutoPrint) {
		pid_t id = gl_thread_get_id();
		GL_THREAD_INFO_T info;
		gl_thread_get_info(self, id, &info);

		GL_TraceInfo("Thread[%s] stack usage = %d, total = %d\n", info.name, (GL_INT32)u32StackUsage, info.stack_size);
	}

	return u32StackUsage;
#else
	GL_TraceInfo("Warning: %s called but GL_DEBUG defined as (%d) !\n", __FUNCTION__, GL_DEBUG);
	return 0;
#endif
}
#if GL_DEBUG
GL_Status_t GL_QueueDebug(GL_Queue_t queueId, iGslQueueDebugCallback pfCallbk)
{
	MSG_ITEM *urgent_list = NULL;
	MSG_ITEM *normal_list = NULL;
	gsl_queue_t *p_queue = (gsl_queue_t *)queueId;

	if (!queueId) {
		return GL_FAILURE;
	}

	if (!pfCallbk) {
		return GL_FAILURE;
	}

	pthread_mutex_lock(&p_queue->mt);
	urgent_list = p_queue->urgent_list;
	normal_list = p_queue->used_last_item;
	if ((urgent_list == NULL) && (normal_list == NULL)) {
		pfCallbk(0, NULL);
		goto QUEUE_DEBUG_EXIT;
	}

	while (urgent_list) {
		if (pfCallbk(1, urgent_list->data) == 0) {
			break;
		}

		urgent_list = urgent_list->p_next;
	}

	while (normal_list) {
		if (pfCallbk(0, normal_list->data) == 0) {
			break;
		}

		normal_list = normal_list->p_next;
	}

QUEUE_DEBUG_EXIT:
	pthread_mutex_unlock(&p_queue->mt);

	return GL_SUCCESS;
}
#endif
#ifdef DIAG_PRINTF_DISABLE
int __wrap_diag_printf(const char *fmt, ...)
{
	return 0;
}

int __wrap_diag_printf_nt(const char *fmt, ...)
{
	return 0;
}
#endif
static GL_INT8 gl_thread_get_ext_info(GL_TASK_T thread)
{
	GL_INT8 p;

	//call cyg function
	return p;
}
static GL_INT32 gl_thread_reset_usage( GL_TASK_T thread )
{
	//call cyg function
	return GL_SUCCESS;
}
static GL_INT32 gl_thread_get_usage( GL_TASK_T thread )
{
	//call cyg function
	return GL_SUCCESS;
}
static GL_INT32 gl_thread_get_next(GL_TASK_T *thread, GL_UINT16 *id)
{
	//call cyg function
	return GL_SUCCESS;
}
static GL_INT32 gl_thread_get_info(GL_TASK_T thread,GL_UINT16 id, GL_THREAD_INFO_T *info )
{
	//call cyg function
	GL_INT32 err;
	pthread_attr_t attr;
	struct sched_param sched_t;


	if(thread == (GL_TASK_T)NULL)
	{
		return GL_FALSE;
	}
	err = pthread_attr_init(&attr);
	if(err != 0)
	{
		return (err);
	}
	info->handle = thread;
	info->id = gl_thread_get_id();
	info->name = NULL;
	info->set_pri = 0;
	err = pthread_attr_getschedparam( &attr, &sched_t);
	if(err != 0)
	{
		return GL_FALSE;
	}
	info->cur_pri = sched_t.__sched_priority;
	if( GL_FALSE == gl_thread_get_stack_base(thread, &info->stack_base))
	{
		return GL_FALSE;
	}
	err = gl_thread_get_stack_size(thread);
	if(GL_FALSE == err)
	{
		return GL_FALSE;
	}
	else
	{
		info->stack_size = err;
	}
	info->stack_used = 0;
	err = pthread_attr_getdetachstate( &attr, &(info->state));
	if( err != 0)
	{
		return GL_FALSE;
	}
	pthread_attr_destroy(&attr);

	return GL_TRUE;
}
static pid_t gl_thread_get_id( void )
{
	//call cyg function
	return syscall(SYS_gettid);
	//return GL_FALSE;
}
static GL_INT32 gl_thread_get_stack_base(GL_TASK_T self, void *stackaddr)
{
	//call cyg function

	pthread_attr_t attr;
//	void *stackaddr;
	size_t stacksize;
	GL_INT32 err;

	if(self == (GL_TASK_T)NULL)
	{
		return GL_FALSE;
	}

	err = pthread_attr_init(&attr);
	if(err != 0)
	{
		return (err);
	}
	err = pthread_attr_getstack( &attr, &stackaddr, &stacksize);
	if(err != 0)
	{
		printf("hanjun debug:pthread_attr_getstack failed!!\r\n");
		return (err);
	}
	printf("hanjun debug: pthread_attr_getstack get stack addr is %x\r\n", (GL_INT32)stacksize);
	pthread_attr_destroy(&attr);

	return GL_TRUE;
}
static GL_INT32 gl_thread_get_stack_size(GL_TASK_T self)
{
	//call cyg function

	pthread_attr_t attr;
	size_t stacksize;
	GL_INT32 err;

	if(self == (GL_TASK_T)NULL)
	{
		return GL_FALSE;
	}

	err = pthread_attr_init(&attr);
	if(err != 0)
	{
		return (err);
	}
	err = pthread_attr_getstacksize( &attr, &stacksize);
	if(err != 0)
	{
		return (err);
	}
	pthread_attr_destroy(&attr);

	return stacksize;
}
static GL_TASK_T gl_pthread_self( void )
{
	return pthread_self();
}
#endif

void GL_SysMemInfo(struct lambo86_mem_info *info)
{
	if (NULL == info) {
		return;
	}
	lambo86_hw_get_meminfo(info);
}

void *GL_Addr2Cache(void *addr)
{
	return lambo_memaddrswitch_ex(addr, LAMBO_ADDR_TYPE_CACHE);
}

void *GL_Addr2Uncache(void *addr)
{
	return lambo_memaddrswitch_ex(addr, LAMBO_ADDR_TYPE_UNCACHE);
}

void *GL_Addr2Phys(void *addr)
{
	return lambo_memaddrswitch_ex(addr, LAMBO_ADDR_TYPE_PHYSICAL);
}

void *GL_Phys2MemA(void *addr)
{
	unsigned int p = (unsigned int)addr;

	if (p >= 0x10000000) {
		return NULL;
	}
	return (void *)(p & ~0xF0000000);
}

void *GL_Phys2MemB(void *addr)
{
	unsigned int p = (unsigned int)addr;

	if ((p < 0x10000000) || (p >= 0x20000000)) {
		return NULL;
	}
	return (void *)(p & ~0xF0000000);
}

int GL_AddrIsCached(void *addr)
{
	return lambo86_memaddr_check(addr, LAMBO_ADDR_TYPE_CACHE);
}

int GL_AddrIsMemA(void *addr)
{
	return CHECK_IF_ADDR_IN_DRAM_A((unsigned int)addr);
}

int GL_AddrIsMemB(void *addr)
{
	return CHECK_IF_ADDR_IN_DRAM_B((unsigned int)addr);
}

int GL_AddrIsValid(void *addr)
{
	return CHECK_IF_ADDR_IN_RANGE((unsigned int)addr);
}

void *GL_CACHE_TO_UNCACHE(void* addr)
{
	return lambo_memaddrswitch(addr, LAMBO_ADDR_TYPE_CACHE, LAMBO_ADDR_TYPE_UNCACHE);
}

void *GL_UNCACHE_TO_CACHE(void* addr)
{
	return lambo_memaddrswitch(addr, LAMBO_ADDR_TYPE_UNCACHE, LAMBO_ADDR_TYPE_CACHE);
}

void *GL_CACHE_TO_PHYSICAL(void* addr)
{
	return lambo_memaddrswitch(addr, LAMBO_ADDR_TYPE_CACHE, LAMBO_ADDR_TYPE_PHYSICAL);
}

void *GL_UNCACHE_TO_PHYSICAL(void* addr)
{
	return lambo_memaddrswitch(addr, LAMBO_ADDR_TYPE_UNCACHE, LAMBO_ADDR_TYPE_PHYSICAL);
}

void *GL_PHYSICAL_TO_CACHE(void* addr)
{
	return lambo_memaddrswitch(addr, LAMBO_ADDR_TYPE_PHYSICAL, LAMBO_ADDR_TYPE_CACHE);
}

void *GL_PHYSICAL_TO_UNCACHE(void* addr)
{
	return lambo_memaddrswitch(addr, LAMBO_ADDR_TYPE_PHYSICAL, LAMBO_ADDR_TYPE_UNCACHE);
}
