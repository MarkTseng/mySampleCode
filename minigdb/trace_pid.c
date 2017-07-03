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

#include <sys/ptrace.h>
#include <asm/ptrace.h>
#include <sys/user.h>

// global variable
pid_t g_child;
pid_t g_mainPid;
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
				printf("### pid: %ld, stop signal: %d\n", new_child, WSTOPSIG(status));  
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

