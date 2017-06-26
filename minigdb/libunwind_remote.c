#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>
#include <sys/ptrace.h>
#include <libunwind.h>
#include <libunwind-x86_64.h>
#include <libunwind-ptrace.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>


#define panic(X) fprintf(stderr, #X "\n");

static unw_addr_space_t as;
static struct UPT_info *ui;

void do_backtrace(pid_t child) {

	ui = _UPT_create(child);
	if (!ui) {
		panic("_UPT_create failed");
	}

	//ptrace(PTRACE_ATTACH, child, 0, 0);
	struct timespec t = { .tv_sec = 0, t.tv_nsec = 1000000 };
	//nanosleep(&t, NULL);

	unw_cursor_t c;
	int rc = unw_init_remote(&c, as, ui);
	if (rc != 0) {
		if (rc == UNW_EINVAL) {
			panic("unw_init_remote: UNW_EINVAL");
		} else if (rc == UNW_EUNSPEC) {
			panic("unw_init_remote: UNW_EUNSPEC");
		} else if (rc == UNW_EBADREG) {
			panic("unw_init_remote: UNW_EBADREG");
		} else {
			panic("unw_init_remote: UNKNOWN");
		}
	}

	do {
		unw_word_t  offset, pc;
		char        fname[64];

		unw_get_reg(&c, UNW_REG_IP, &pc);
		fname[0] = '\0';
		(void) unw_get_proc_name(&c, fname, sizeof(fname), &offset);

		printf("\n%p : (%s+0x%x) [%p]\n", (void *)pc,
				fname,
				(int) offset,
				(void *) pc);
	} while (unw_step(&c) > 0);


	//ptrace(PTRACE_DETACH, child, 0, 0);

	_UPT_destroy(ui);
}


int main(int argc __attribute__((unused)), char **argv, char **envp) {

	as = unw_create_addr_space(&_UPT_accessors, 0);
	if (!as) {
		panic("unw_create_addr_space failed");
	}

	pid_t child;
	child = fork();

	if (!child) {

		ptrace(PTRACE_TRACEME, child,0,0);
		execve("./target",
				argv, envp);

		return 0;

	} else {

		struct timespec t = { .tv_sec = 1, .tv_nsec = 0 };
		nanosleep(&t, NULL);

		int status,w;
		while(1)
		{
			do {
				w = waitpid(child, &status, WUNTRACED | WCONTINUED);
				if (w == -1) {
					perror("waitpid");
					exit(EXIT_FAILURE);
				}

				if (WIFEXITED(status)) {
					printf("exited, status=%d\n", WEXITSTATUS(status));
				} else if (WIFSIGNALED(status)) {
					printf("killed by signal %d\n", WTERMSIG(status));
				} else if (WIFSTOPPED(status)) {

					do_backtrace(child);
					ptrace(PTRACE_SINGLESTEP, child, 0,0);
					printf("stopped by signal %d\n", WSTOPSIG(status));
				} else if (WIFCONTINUED(status)) {
					printf("continued\n");
				}
				ptrace(PTRACE_SYSCALL, child, 0,0);
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
			exit(EXIT_SUCCESS);
		}
	}


	return 0;
}

