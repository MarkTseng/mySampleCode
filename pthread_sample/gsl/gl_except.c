#include <signal.h>
#include <ucontext.h>
#include <unistd.h>
#include "types.h"
#include "gsl/gl_proc.h"
#include "gsl/gl_except.h"

static const char *__signame_str[] = {
	"Unknown"
	"SIGHUP",
	"SIGINT",
	"SIGQUIT",
	"SIGILL",
	"SIGTRAP",
	"SIGIOT",
	"SIGABRT",
	"SIGEMT",
	"SIGFPE",
	"SIGKILL",
	"SIGBUS",
	"SIGSEGV",
	"SIGSYS",
	"SIGPIPE",
	"SIGALRM",
	"SIGTERM",
	"SIGUSR1",
	"SIGUSR2",
	"SIGCHLD",
	"SIGCLD",
	"SIGPWR",
	"SIGWINCH",
	"SIGURG",
	"SIGIO",
	"SIGPOLL",
	"SIGSTOP",
	"SIGTSTP",
	"SIGCONT",
	"SIGTTIN",
	"SIGTTOU",
	"SIGVTALRM",
	"SIGPROF",
	"SIGXCPU",
	"SIGXFSZ"
};

const char *signame(int signo)
{
	if (signo < (sizeof(__signame_str) / sizeof(__signame_str[0]))) {
		return __signame_str[signo];
	}
	return __signame_str[0];
}

static void dump_mips_context(void *context)
{
	ucontext_t *uctx = (ucontext_t *)context;
	mcontext_t *mctx = &uctx->uc_mcontext;
	struct gsl_proc_maps *proc_map;
	struct gsl_proc_maps const *stack_map;

	diag_printf("     EPC = 0x%08x\n", (unsigned int)mctx->pc);
	diag_printf("      RA = 0x%08x\n", (unsigned int)mctx->gregs[31]);

	diag_printf("Register List:\n");
	diag_printf("\t-------------------------------------------\n");
	diag_printf("\tzero       at         v0         v1\n");
	diag_printf("\t0x%08X 0x%08X 0x%08X 0x%08X \n", (unsigned int)mctx->gregs[0], (unsigned int)mctx->gregs[1], (unsigned int)mctx->gregs[2], (unsigned int)mctx->gregs[3]);
	diag_printf("\ta0         a1         a2         a3\n");
	diag_printf("\t0x%08X 0x%08X 0x%08X 0x%08X \n", (unsigned int)mctx->gregs[4], (unsigned int)mctx->gregs[5], (unsigned int)mctx->gregs[6], (unsigned int)mctx->gregs[7]);
	diag_printf("\tt0         t1         t2         t3\n");
	diag_printf("\t0x%08X 0x%08X 0x%08X 0x%08X \n", (unsigned int)mctx->gregs[8], (unsigned int)mctx->gregs[9], (unsigned int)mctx->gregs[10], (unsigned int)mctx->gregs[11]);
	diag_printf("\tt4         t5         t6         t7\n");
	diag_printf("\t0x%08X 0x%08X 0x%08X 0x%08X \n", (unsigned int)mctx->gregs[12], (unsigned int)mctx->gregs[13], (unsigned int)mctx->gregs[14], (unsigned int)mctx->gregs[15]);
	diag_printf("\ts0         s1         s2         s3\n");
	diag_printf("\t0x%08X 0x%08X 0x%08X 0x%08X \n", (unsigned int)mctx->gregs[16], (unsigned int)mctx->gregs[17], (unsigned int)mctx->gregs[18], (unsigned int)mctx->gregs[19]);
	diag_printf("\ts4         s5         s6         s7\n");
	diag_printf("\t0x%08X 0x%08X 0x%08X 0x%08X \n", (unsigned int)mctx->gregs[20], (unsigned int)mctx->gregs[21], (unsigned int)mctx->gregs[22], (unsigned int)mctx->gregs[23]);
	diag_printf("\tt8         t9         k0         k1\n");
	diag_printf("\t0x%08X 0x%08X 0x%08X 0x%08X \n", (unsigned int)mctx->gregs[24], (unsigned int)mctx->gregs[25], (unsigned int)mctx->gregs[26], (unsigned int)mctx->gregs[27]);
	diag_printf("\tgp         sp         s8/fp      ra\n");
	diag_printf("\t0x%08X 0x%08X 0x%08X 0x%08X \n", (unsigned int)mctx->gregs[28], (unsigned int)mctx->gregs[29], (unsigned int)mctx->gregs[30], (unsigned int)mctx->gregs[31]);
	diag_printf("\t-------------------------------------------\n");
	diag_printf("\n\n");

	diag_printf("Memory maps (pid = %d):\n", getpid());
	GL_ProcMapsDumpMatches(getpid(), 1, (unsigned int)mctx->pc);

	/*
	 * Print statck contents without decoration because the program may exited
	 * when we parse memory maps.
	 */
	{
		int i;
		unsigned int *sp = (unsigned int *)(unsigned int)mctx->gregs[29];

		diag_printf("Part of stack content:\n");
		for (i = 0; i < 20; ++i) {
			diag_printf("    *0x%08x: 0x%08x\n", (unsigned int)sp, *sp);
			sp++;
		}
	}

	/*
	 * If we're lucky, we will have abuntant information about current system status.
	 */
	proc_map = GL_ProcMapsParsePid(getpid());
	if (!proc_map) {
		diag_printf("%s: cannot parse process memory maps.\n", __FUNCTION__);
		return;
	}
	diag_printf("Detailed stack backtrace:\n");
	stack_map = GL_ProcMapsFindAddr((struct gsl_proc_maps const *)proc_map, (unsigned int)mctx->gregs[29]);
	if (stack_map) {
		int i;
		unsigned int *sp = (unsigned int *)(unsigned int)mctx->gregs[29];

		diag_printf("Current stack pointer = 0x%08x, current stack top = 0x%08x\n",
			   (unsigned int)sp, stack_map->vm_end);
		for (i = 0; i < 100; ++i) {
			struct gsl_proc_maps const *m;

			diag_printf("    *0x%08x: 0x%08x", (unsigned int)sp, *sp);
			m = GL_ProcMapsFindAddr((struct gsl_proc_maps const *)proc_map, *sp);
			if (!m) {
				diag_printf("\n");
			} else {
				diag_printf("  (%s %s)\n", m->perm_str, m->name);
			}

			sp++;
			if ((unsigned int)sp >= stack_map->vm_end) {
				break;
			}
		}
	}
	diag_printf("\n\n");
}

static const char *gsl_siginfo_code_resolve(int signo, int code)
{
	switch (code) {
	case SI_USER:
		return "kill or raise";
	case SI_KERNEL:
		return "sent by kernel";
	case SI_QUEUE:
		return "sigqueue";
	case SI_TIMER:
		return "POSIX timer expired";
	case SI_MESGQ:
		return "POSIX message queue state changed";
	case SI_ASYNCIO:
		return "AIO completed";
	case SI_SIGIO:
		return "queued SIGIO";
	case SI_TKILL:
		return "tkill/tgkill";
	default:
		break;
	}

	switch (signo) {
	case SIGSEGV:
		switch (code) {
		case SEGV_MAPERR:
			return "address not mapped to object";
		case SEGV_ACCERR:
			return "invalid permissions for mapped object";
		}
		break;
	case SIGBUS:
		switch (code) {
		case BUS_ADRALN:
			return "invalid address alignment";
		case BUS_ADRERR:
			return "non-existent physical address";
		case BUS_OBJERR:
			return "object specific hardware error";
		}
		break;
	case SIGILL:
		switch (code) {
		case ILL_ILLOPC:
			return "illegal opcode";
		case ILL_ILLOPN:
			return "illegal operand";
		case ILL_ILLADR:
			return "illegal addressing mode";
		case ILL_ILLTRP:
			return "illegal trap";
		case ILL_PRVOPC:
			return "privileged opcode";
		case ILL_PRVREG:
			return "privileged register";
		case ILL_COPROC:
			return "coprocessor error";
		case ILL_BADSTK:
			return "internal stack error";
		}
		break;
	case SIGFPE:
		break;
	default:
		break;
	}
	return "Unknown error";
}

void gsl_siginfo_dump(int signo, siginfo_t *siginfo, void *arg)
{
	diag_printf("\n\nOops:\n");
	diag_printf("Signal num [%d], name = %s\n", signo, signame(signo));
	diag_printf("code = %d, cause: %s\n", siginfo->si_code, gsl_siginfo_code_resolve(signo, siginfo->si_code));
	switch (signo) {
	case SIGSEGV:
	case SIGBUS:
	case SIGILL:
	case SIGFPE:
		diag_printf(" BADADDR = 0x%08x (si_addr)\n", (unsigned int)siginfo->si_addr);
		break;
#if 0
	case SIGCHLD:
		diag_printf(" si_utime = %llu\n", (unsigned long long)siginfo->si_utime);
		diag_printf(" si_stime = %llu\n", (unsigned long long)siginfo->si_stime);
		diag_printf("si_status= 0x%08x\n", (unsigned int)siginfo->si_status);
		diag_printf("si_utime = 0x%08x\n", (unsigned int)siginfo->si_utime);
		diag_printf("si_stime = 0x%08x\n", (unsigned int)siginfo->si_stime);
		break;
	case SIGPOLL:
		diag_printf(" si_band = 0x%08x\n", (unsigned int)siginfo->si_band);
		diag_printf("   si_fd = 0x%08x\n", (unsigned int)siginfo->si_fd);
		break;
#endif
	default:
		break;
	}
#if 0
	diag_printf("  si_addr  = 0x%08X\n", (unsigned int)siginfo->si_addr);
	diag_printf("  si_signo = %d\n", siginfo->si_signo);
	diag_printf("  si_errno = %d\n", siginfo->si_errno);
	diag_printf("  si_code  = %d\n", siginfo->si_code);
	diag_printf("  si_pid   = %d\n", siginfo->si_pid);
	diag_printf("  si_utime = %lu\n", siginfo->si_utime);
	diag_printf("  si_fd    = %d\n", siginfo->si_fd);
#endif
	dump_mips_context(arg);

	exit(-signo);
}

GL_Status_t GL_ExceptInit(void)
{
	struct sigaction act;
#if 0
	static unsigned char bInit = 0;

	if (bInit) {
		return GL_FAILURE;
	}
	bInit = 1;
#endif
	/* Get default signal handler.  */
	sigaction(SIGSEGV, NULL, &act);

	act.sa_sigaction = gsl_siginfo_dump;
	act.sa_flags |= SA_SIGINFO;

	/* Set default signal handler.  */
	diag_printf("Register SIGSEGV handlers ....\n");
	sigaction(SIGSEGV, &act, NULL);
	diag_printf("Register SIGILL handlers ....\n");
	sigaction(SIGILL, &act, NULL);
	diag_printf("Register SIGFPE handlers ....\n");
	sigaction(SIGFPE, &act, NULL);
	diag_printf("Register SIGBUS handlers ....\n");
	sigaction(SIGBUS, &act, NULL);
	diag_printf("Register SIGTRAP handlers ....\n");
	sigaction(SIGTRAP, &act, NULL);


	return GL_SUCCESS;
}
