#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <errno.h>
// libunwind header
#include <libunwind.h>
#include <libunwind-arm.h>
#include <libunwind-ptrace.h>
// ptrace header
#include <sys/ptrace.h>
#include <asm/ptrace.h>
#include <sys/user.h>

// global variable
pid_t g_child;
pid_t g_mainPid;
struct user regs;
static unw_addr_space_t as;
static struct UPT_info *ui;

static void dump_regs(struct user const *regs, FILE *outfp)
{
    fprintf(outfp, "cpsr = 0x%08x, pc = 0x%08x\n", regs->regs.ARM_cpsr, regs->regs.ARM_pc);
    fprintf(outfp, "lr   = 0x%08x, sp = 0x%08x\n", regs->regs.ARM_lr, regs->regs.ARM_sp);
    fprintf(outfp, "ip   = 0x%08x, fp = 0x%08x\n", regs->regs.ARM_ip, regs->regs.ARM_fp);
    fprintf(outfp, "r0   = 0x%08x, r1 = 0x%08x\n", regs->regs.ARM_r0, regs->regs.ARM_r1);
    fprintf(outfp, "r2   = 0x%08x, r3 = 0x%08x\n", regs->regs.ARM_r2, regs->regs.ARM_r3);
    fprintf(outfp, "r4   = 0x%08x, r5 = 0x%08x\n", regs->regs.ARM_r4, regs->regs.ARM_r5);
    fprintf(outfp, "r6   = 0x%08x, r7 = 0x%08x\n", regs->regs.ARM_r6, regs->regs.ARM_r7);
    fprintf(outfp, "r8   = 0x%08x, r9 = 0x%08x\n", regs->regs.ARM_r8, regs->regs.ARM_r9);
    fprintf(outfp, "\n");
}

void do_backtrace(pid_t child) {

	ui = _UPT_create(child);
	if (!ui) {
		printf("_UPT_create failed");
	}

	as = unw_create_addr_space(&_UPT_accessors, 0);
	if (!as) {
		printf("unw_create_addr_space failed");
	}

	unw_cursor_t c;
	int rc = unw_init_remote(&c, as, ui);
	if (rc != 0) {
		if (rc == UNW_EINVAL) {
			printf("unw_init_remote: UNW_EINVAL");
		} else if (rc == UNW_EUNSPEC) {
			printf("unw_init_remote: UNW_EUNSPEC");
		} else if (rc == UNW_EBADREG) {
			printf("unw_init_remote: UNW_EBADREG");
		} else {
			printf("unw_init_remote: UNKNOWN");
		}
	}

	printf("\n### backtrace start ###\n");
	do {
		unw_word_t  offset, pc;
		char        fname[64];

		unw_get_reg(&c, UNW_REG_IP, &pc);
		fname[0] = '\0';
		(void) unw_get_proc_name(&c, fname, sizeof(fname), &offset);

		printf("%p : (%s+0x%x) [%p]\n", (void *)pc,
				fname,
				(int) offset,
				(void *) pc);
	} while (unw_step(&c) > 0);
	printf("### backtrace end ###\n\n");

	unw_destroy_addr_space(as);
	_UPT_destroy(ui);
}

static void signal_handler(int signo)
{
	printf("SIGINT trigger\n");
	ptrace(PTRACE_KILL, g_child,0,0);
	kill(g_mainPid, SIGKILL);
}

int main(int argc __attribute__((unused)), char **argv, char **envp) 
{
	pid_t new_child;

    if(argc == 1)
    {
        fprintf(stderr, "usage memtrace <bin>\n");
        exit(-1);
    }

    const char *path = argv[1];
    const char *name = strrchr(path, '/');
    if(name){
        name += 1;        
    }else{
        name = path;
    }

	signal(SIGINT, signal_handler);

	g_child = fork();
    long newpid = 0;

	if (!g_child) {

		ptrace(PTRACE_TRACEME, g_child,0,0);
		execve(path,
				argv, envp);

		return 0;

	} else {
		g_mainPid = getpid();
        printf("g_child: %d\n", g_child);
        printf("g_mainPid: %d\n", g_mainPid);
			
		int status,w;
	
		new_child = waitpid(-1, &status, __WALL);
		if (WIFSTOPPED(status)) {
			printf("### pid: %ld, stop signal: %d\n", new_child, WSTOPSIG(status));  
			ptrace(PTRACE_SETOPTIONS, new_child, NULL, PTRACE_O_TRACECLONE | PTRACE_O_TRACEVFORK | PTRACE_O_TRACEFORK | PTRACE_O_TRACEEXEC);
		}
		ptrace(PTRACE_CONT,new_child, NULL, NULL);

		while(1) {
			new_child = waitpid(-1, &status, __WALL);
			if (WIFSTOPPED(status)) {
            	memset(&regs, 0, sizeof(regs));
            	ptrace(PTRACE_GETREGS, new_child, NULL, &regs);
				printf("### pid: %ld, stop signal: %d\n", new_child, WSTOPSIG(status));  
                dump_regs(&regs, stdout);
				do_backtrace(new_child);
				ptrace(PTRACE_CONT,new_child, NULL, NULL);
			}
			if (status>>8 == (SIGTRAP | (PTRACE_EVENT_EXEC<<8))) {
				printf("### PTRACE_EVENT_EXEC %ld, \n", new_child);  
			}
			if (status>>8 == (SIGTRAP | (PTRACE_EVENT_CLONE<<8))) {
				printf("### PTRACE_EVENT_CLONE %ld\n", new_child);  
			}
			if (status>>8 == (SIGTRAP | (PTRACE_EVENT_VFORK<<8))) {
				printf("### PTRACE_EVENT_VLONE %ld\n", new_child);  
			}
			if (status>>8 == (SIGTRAP | (PTRACE_EVENT_VFORK_DONE<<8))) {
				printf("### PTRACE_EVENT_VFORK_DONE %ld\n", new_child);  
			}
			if (status >>8 == PTRACE_EVENT_FORK) {
				printf("### PTRACE_EVENT_FORK %ld\n", new_child);  
			}
			if(WIFEXITED(status)) {
				printf("### new_child %d exited\n", new_child);
				if(new_child == g_child)
					break;
			}
		}
	}

	return 0;
}

