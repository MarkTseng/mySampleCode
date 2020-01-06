#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <endian.h>

#if 0
#include <libunwind.h>
#include <libunwind-arm.h>
#include <libunwind-ptrace.h>
#endif

#include <sys/ptrace.h>
#include <asm/ptrace.h>
#include <sys/user.h>

typedef struct app_args_t {
    pid_t       pid;
    char const *out;
    char const *exec;
} app_args_t;

typedef struct exec_args_t {
    char *path;
    char **args;
} exec_args_t;

static void show_usage();
static int parse_args(app_args_t *args, int argc, char **argv);
static char const *sigstr(int sig);
static void dump_siginfo(siginfo_t const *info, FILE *outfp);
static void dump_regs(struct user const *regs, FILE *outfp);
static void build_exec_args(char const *cmd, exec_args_t *args);
#if 0
static void backtrace(unw_addr_space_t *as, pid_t pid, FILE *outfp);
#endif
int main(int argc, char **argv)
{
    app_args_t args;
    siginfo_t sif;
    int status = 0;
    int failed = 0;
    int stopsig = 0;
    int i;
    FILE *outfp = NULL;
    int ret = 0;
    struct user regs;
#if 0
    unw_addr_space_t unw_as;
#endif

    parse_args(&args, argc, argv);

    if (!args.pid && (NULL == args.exec)) {
        show_usage();
        return 0;
    }

    if (!strcmp(args.out, "-")) {
        outfp = stdout;
    } else {
        outfp = fopen(args.out, "w");
        if (NULL == outfp) {
            ret = errno;
            fprintf(stderr, "failed to open output file (%s).\n", strerror(errno));
            goto __cleanup;
        }
    }

    if (args.exec) {
        exec_args_t exec_args = { NULL, NULL };
        build_exec_args(args.exec, &exec_args);
        pid_t pid = fork();
        if (0 == pid) {
            if (0 != ptrace(PTRACE_TRACEME, 0, NULL, NULL)) {
                fprintf(stderr, "failed to start trace (%s).\n", strerror(errno));
                exit(1);
            }
			usleep(100);
            execv(exec_args.path, exec_args.args);
            exit(2);
        } else {
            args.pid = pid;
        }
    } else {
        if (0 != ptrace(PTRACE_ATTACH, args.pid, NULL, NULL)) {
            ret = errno;
            fprintf(stderr, "ERROR: failed to attach the process (pid=%d) (%s)\n", args.pid, strerror(errno));
            goto __cleanup;
        }
    }
#if 0
    unw_as = unw_create_addr_space(&_UPT_accessors, 0);
    if (!unw_as) {
        fprintf(stderr, "ERROR: unw_create_addr_space failed.\n");
        goto __detach;
    }
#endif

#if 1
		if (-1 == wait(&status)) {
			ret = errno;
			fprintf(stderr, "WARNING: failed to wait events (%s).\n", strerror(errno));
			goto __detach;
		}
#endif
		if (0 != ptrace(PTRACE_CONT, args.pid, NULL, stopsig)) {
			ret = errno;
			fprintf(stderr, "ERROR: failed to continue the process (%s).\n", strerror(errno));
			failed = 1;
		}
		if (0 != ptrace(PTRACE_SETOPTIONS, args.pid, NULL, PTRACE_O_TRACECLONE | PTRACE_O_TRACEVFORK | PTRACE_O_TRACEFORK)) {
			fprintf(stderr, "failed toPTRACE_SETOPTIONS (%s).\n", strerror(errno));
			exit(1);
		}


    for (;;) {
        if (-1 == wait(&status)) {
            fprintf(stderr, "WARNING: failed to wait events (%s).\n", strerror(errno));
            continue;
        }
        fprintf(outfp, "INFO: status = %d\n", status);
        if (WIFEXITED(status)) {
            fprintf(outfp, "INFO: process exited (ret=%d)\n", WEXITSTATUS(status));
            break;
        }
        if (WIFSTOPPED(status)) {
			printf("STOP\n");
        }
#if 0
        stopsig = WSTOPSIG(status);
        fprintf(outfp, "INFO: stop signal = %s(%d)\n", sigstr(stopsig), stopsig);
        if (stopsig != SIGSTOP) {
            if (0 != ptrace(PTRACE_GETSIGINFO, args.pid, NULL, &sif)) {
                ret = errno;
                fprintf(stderr, "ERROR: failed to get signal information (%s).\n", strerror(errno));
                failed = 1;
                break;
            }
            dump_siginfo(&sif, outfp);
        }
        if (stopsig == SIGSEGV || stopsig == SIGILL) {
            char cat_cmd[48];
            memset(&regs, 0, sizeof(regs));
            if (0 != ptrace(PTRACE_GETREGS, args.pid, NULL, &regs)) {
                fprintf(stderr, "WARNING: failed to get registers information (%s).\n", strerror(errno));
            } else {
                dump_regs(&regs, outfp);
            }
#if 0
            backtrace(&unw_as, args.pid, outfp);
#endif
            snprintf(cat_cmd, sizeof(cat_cmd), "cat /proc/%d/maps", args.pid);
            system(cat_cmd);
        }
#endif
    }

__detach:
#if 0
    unw_destroy_addr_space (unw_as);
#endif
    if (args.exec) {
        ptrace(PTRACE_KILL, args.pid, NULL, NULL);
        waitpid(args.pid, NULL, 0);
    } else {
        ptrace(PTRACE_DETACH, args.pid, NULL, NULL);
    }

__cleanup:
    if (outfp && (stdout != outfp)) {
        fclose(outfp);
    }

    return ret;
}

static void show_usage()
{
    printf("Usage: debug (-p pid | -c cmd) [-o out]\n");
}

static int parse_args(app_args_t *args, int argc, char **argv)
{
    args->pid = 0;
    args->out = "-";
    args->exec = NULL;

    int opt;
    while (-1 != (opt = getopt(argc, argv, "p:c:o:"))) {
        switch (opt) {
        case 'p':
            args->pid = atoi(optarg);
            break;
        case 'c':
            args->exec = optarg;
            break;
        case 'o':
            args->out = optarg;
            break;
        }
    }
    return 0;
}

static char const *sigstr(int sig)
{
    static char signum[4];
#define CASESTR(x) case x: return #x;
    switch (sig) {
    CASESTR(SIGINT);
    CASESTR(SIGHUP);
    CASESTR(SIGILL);
    CASESTR(SIGTRAP);
    CASESTR(SIGABRT);
    CASESTR(SIGSEGV);
    CASESTR(SIGTERM);
    CASESTR(SIGCHLD);
    default: snprintf (signum, sizeof(signum), "%d", sig); return signum;
    }
}

static void dump_siginfo(siginfo_t const *info, FILE *outfp)
{
    fprintf(outfp, "SIGNAL: %s(%d), ERRNO: %d, CODE: %d\n",
        sigstr(info->si_signo), info->si_signo, info->si_errno, info->si_code);

    switch (info->si_signo) {
    case SIGCHLD:
        fprintf(outfp, "pid      = %d\n", info->si_pid);
        fprintf(outfp, "uid      = %d\n", info->si_uid);
        fprintf(outfp, "status   = %x\n", info->si_status);
        fprintf(outfp, "utime    = %u\n", info->si_utime);
        fprintf(outfp, "stime    = %u\n", info->si_stime);
        break;
    case SIGILL:
    case SIGFPE:
    case SIGSEGV:
    case SIGBUS:
        fprintf(outfp, "addr     = %p\n", info->si_addr);
#ifdef __ARCH_SI_TRAPNO
        fprintf(outfp, "trapno   = %d\n", info->si_trapno);
#endif
        break;
    }
}

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

static void build_exec_args(char const *cmd, exec_args_t *args)
{
    char const delim[] = " \t";
    char *token;
    char *cur_ptr;
    char *cmd_dup = strdup(cmd);
    int err = 0;
    int i;

    args->path = NULL;
    args->args = NULL;

    if (NULL == cmd_dup) {
        goto __cleanup;
    }

    token = strtok_r(cmd_dup, delim, &cur_ptr);
    if (!token) {
        goto __cleanup;
    }

    args->path = strdup(token);
    if (!args->path) {
        err = errno;
        goto __cleanup;
    }

    args->args = malloc(sizeof(char *) * 66);
    if (!args->args) {
        err = errno;
        goto __cleanup;
    }

    memset(args->args, 0, sizeof(char *) * 66);
    args->args[0] = strdup(token);
    if (!args->args[0]) {
        err = errno;
        goto __cleanup;
    }

    for (i = 1; i < 65; ++i) {
        token = strtok_r(NULL, delim, &cur_ptr);
        if (!token) {
            break;
        }
        args->args[i] = strdup(token);
        if (!args->args[i]) {
            err = errno;
            break;
        }
    }

    args->args[65] = NULL;

__cleanup:
    if (cmd_dup) {
        free(cmd_dup);
    }

    if (err) {
        if (args->path) {
            free(args->path);
        }
        if (args->args) {
            for (i = 0; args->args[i]; ++i) {
                free(args->args[i]);
            }
            free(args->args);
        }
        args->path = NULL;
        args->args = NULL;
    }
}
#if 0
static void backtrace(unw_addr_space_t *as, pid_t pid, FILE *outfp)
{
    unw_word_t ip, sp, start_ip = 0, off;
    int n = 0;
    unw_proc_info_t pi;
    char buf[512];
    size_t len;
    unw_cursor_t unw_c;
    int ret;
    void *ui;

    ui = _UPT_create(pid);

    fprintf(outfp, "\n---- [ backtrace ] ----------\n");

    ret = unw_init_remote(&unw_c, *as, ui);
    if (0 > ret) {
        fprintf(stderr, "unw_init_remote failed (ret=%d).\n", ret);
        goto __cleanup;
    }

    do {
        if ((ret = unw_get_reg(&unw_c, UNW_REG_IP, &ip)) < 0 ||
            (ret = unw_get_reg(&unw_c, UNW_REG_SP, &sp)) < 0) {
            fprintf(stderr, "unw_get_reg failed (ret=%d).\n", ret);
            break;
        }

        if (0 == n) {
            start_ip = ip;
        }

        buf[0] = '\0';
        if ((ret = unw_get_proc_name(&unw_c, buf, sizeof(buf), &off)) == 0) {
            fprintf(outfp, "%p <%s + 0x%x>\n", (void*)ip, buf, off);
        } else {
            //fprintf(stderr, "unw_get_proc_name failed (ret = %d).\n", ret);
            break;
        }

        if ((ret = unw_get_proc_info(&unw_c, &pi)) < 0) {
            //fprintf(stderr, "unw_get_proc_info failed (ret=%d).\n", ret);
            break;
        }

        ret = unw_step(&unw_c);
        if (ret < 0) {
            unw_get_reg(&unw_c, UNW_REG_IP, &ip);
        }

        if (++n > 64) {
            break;
        }
    } while (ret > 0);

__cleanup:
    _UPT_destroy(ui);
    fprintf(outfp ,"-----------------------------\n\n");
}
#endif
