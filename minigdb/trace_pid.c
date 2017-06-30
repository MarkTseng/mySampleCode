#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <errno.h>

#include <sys/ptrace.h>
#include <asm/ptrace.h>
#include <sys/user.h>
#if 0
void * trace_process(void * pid){
    pid_t child = atoi((char *) pid);
    long orig_eax, eax;
    int status;
    int callmade = FALSE;
    long opt = PTRACE_O_TRACEFORK;
    long newpid;

    long trace = ptrace(PTRACE_ATTACH,child,NULL,NULL);
    ptrace(PTRACE_SETOPTIONS,child,NULL,opt);
    if(trace == FALSE)
        printf("Attached to %d\n",child);

    while(TRUE) {
        child = waitpid(-1, &status, __WALL);

        if (status >> 16 == PTRACE_EVENT_FORK) {
            ptrace(PTRACE_GETEVENTMSG, child, NULL, (long) &newpid);
            ptrace(PTRACE_SYSCALL, newpid, NULL, NULL);       

            printf("Attached to offspring %ld\n", newpid);  
        }
        else{
            if(WIFEXITED(status))
                printf("Child %d exited\n", child);
        }

        ptrace(PTRACE_SYSCALL,child, NULL, NULL);
    }  
}
#endif
int main(int argc __attribute__((unused)), char **argv, char **envp) {

	pid_t child;
	child = fork();
    long newpid = 0;

	if (!child) {

		ptrace(PTRACE_TRACEME, child,0,0);
		execve("/ext/bin/mixer",
				argv, envp);

		return 0;

	} else {
        printf("child: %d\n", child);
		int status,w;
	
		child = waitpid(-1, &status, __WALL);
		if (WIFSTOPPED(status)) {
			printf("### pid: %ld, stop signal: %d\n", child, WSTOPSIG(status));  
			ptrace(PTRACE_SETOPTIONS, child, NULL, PTRACE_O_TRACECLONE | PTRACE_O_TRACEVFORK | PTRACE_O_TRACEFORK | PTRACE_O_TRACEEXEC);
		}
		ptrace(PTRACE_CONT,child, NULL, NULL);

		while(1) {
			child = waitpid(-1, &status, __WALL);
			if (WIFSTOPPED(status)) {
				printf("### pid: %ld, stop signal: %d\n", child, WSTOPSIG(status));  
				ptrace(PTRACE_CONT,child, NULL, NULL);
			}
			if (status>>8 == (SIGTRAP | (PTRACE_EVENT_EXEC<<8))) {
				printf("### PTRACE_EVENT_EXEC %ld, \n", child);  
			}
			if (status>>8 == (SIGTRAP | (PTRACE_EVENT_CLONE<<8))) {
				printf("### PTRACE_EVENT_CLONE %ld\n", child);  
			}
			if (status>>8 == (SIGTRAP | (PTRACE_EVENT_VFORK<<8))) {
				printf("### PTRACE_EVENT_VLONE %ld\n", child);  
			}
			if (status>>8 == (SIGTRAP | (PTRACE_EVENT_VFORK_DONE<<8))) {
				printf("### PTRACE_EVENT_VFORK_DONE %ld\n", child);  
			}
			if (status >>8 == PTRACE_EVENT_FORK) {
				printf("### PTRACE_EVENT_FORK %ld\n", child);  
			}
			if(WIFEXITED(status)) {
				printf("### Child %d exited\n", child);
			}
		}  
	}

	return 0;
}

