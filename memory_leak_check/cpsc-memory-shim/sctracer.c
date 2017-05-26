// Jesse Kennedy
// CPSC 322 Project 1
// Fall 2015

#define SIZE 1024
#define NUMCALLS 326  // Number of system calls in Linux

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/wait.h>

FILE *fout;
char args[SIZE];
int syscall[NUMCALLS];

int main(int argc, char **argv) {
  pid_t child = fork();

  if (child == 0) {
    strcpy(args, argv[1]);

    for (int i = 2; i < (argc - 1); i++) {
      strcat(args, " ");
      strcat(args, argv[i]);
    }

    char *arguments[SIZE];
    char *tmp = strtok(args, " ");
    char **ptr = arguments;

    while (tmp) {
      *(ptr++) = tmp;
      tmp = strtok(NULL, " ");
    }

    ptrace(PTRACE_TRACEME);
    child = getpid();
    kill(child, SIGSTOP);
    execvp(arguments[0], arguments);

  } else {
    int stat, count;

    waitpid(child, &stat, 0);
    ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD);

    while (1) {
      do {
        ptrace(PTRACE_SYSCALL, child, 0, 0);
        waitpid(child, &stat, 0);

        if (WIFEXITED(stat)) {
          fout = fopen(argv[argc - 1], "w");

          for (int i = 0; i < NUMCALLS; i++) {
            if (syscall[i])
              fprintf(fout, "%d\t%d\n", i, syscall[i]);
          }

          fclose(fout);
          return 1;
        }
      } while (!(WIFSTOPPED(stat) && WSTOPSIG(stat) & 0x80));

      count = ptrace(PTRACE_PEEKUSER, child, sizeof(long) * ORIG_RAX, 0);
      syscall[count]++;

      ptrace(PTRACE_SYSCALL, child, 0, 0);
      waitpid(child, &stat, 0);
    }

  }
}
