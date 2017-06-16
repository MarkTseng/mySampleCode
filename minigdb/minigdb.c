/*
 * minigdb, detects memory leak of running process.
 *
 * Author: Wu Bingzheng
 *   Date: 2016-5
 */

#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <readline/readline.h>

#include "breakpoint.h"
#include "ptrace_utils.h"
#include "memblock.h"
#include "symtab.h"
#include "debug_line.h"
#include "proc_info.h"
#include "addr_maps.h"
#include "minigdb.h"

uintptr_t g_current_entry;
pid_t g_current_thread;
int opt_backtrace_limit = BACKTRACE_MAX;
const char *opt_debug_info_file;

static pid_t g_target_pid;
static int g_signo = 0;


static void signal_handler(int signo)
{
	g_signo = signo;
	kill(g_target_pid, SIGSTOP);
}

/*debugger commands*/
typedef enum {CONTINUE, BREAK, KILL, NEXT, REGISTER}mdb_cmd;

/*reads command from user*/
mdb_cmd next_cmd(char **argv)
{
    mdb_cmd cmd= -1;

    char *cmdstr;
    if(!(cmdstr = readline("(mdb) "))) return cmd;

    if(!strcmp(cmdstr, "c")){
        cmd = CONTINUE;
    }else if(!strcmp(cmdstr, "kill")){   
        cmd = KILL;
    }else if(!strcmp(cmdstr, "n")){
        cmd = NEXT;
    }else if(!strcmp(cmdstr, "regs")){
        cmd = REGISTER;
    }else{
        const char *op = strtok(cmdstr, " ");
        const char *arg = strtok(NULL, " ");
        if( op && arg && !strcmp(op, "b"))
        {
            cmd = BREAK;
            *argv = strdup(arg);
        }
    }
    free(cmdstr);
    return cmd;
}

/*disassemble instruction(s) from buf*/
void disassemble(void *buf, unsigned long size)
{
#if 0
    struct disassemble_info info;
    init_disassemble_info (&info, stdout, (fprintf_ftype)fprintf);
    info.mach = bfd_mach_x86_64;
    info.endian = BFD_ENDIAN_LITTLE;
    info.buffer = buf;
    info.buffer_length = size;
    print_insn_i386(0, &info);
#endif
    printf("disassemble \n");    
}

int main(int argc, char * const *argv)
{
	time_t memory_expire = 10;
	int memblock_limit = 1000;
	int callstack_limit = 1000;
    const char *path = argv[1];
    const char *name = strrchr(path, '/');
    if(name){
        name += 1;        
    }else{
        name = path;
    }

    pid_t pid;

    switch(pid = fork())
    {
        case -1: /*error*/
        {
            perror("fork()");
            exit(-1);
        }   

        case 0:/*child process*/
        {
            ptrace(PTRACE_TRACEME, NULL, NULL);     /*allow child process to be traced*/
            execl(path, name, NULL);                /*child will be stopped here*/
            perror("execl()");
            exit(-1);
        }
        
        /*parent continues execution*/
    }

    g_target_pid = pid;
    char *arg;
    int status;
    long opcode[2];
    long old_rip;
	uintptr_t return_address = 0, return_code = 0;
	struct breakpoint_s *bp = NULL;
	uintptr_t arg1 = 0, arg2 = 0;

    /* load symbol table*/
	addr_maps_build(g_target_pid);
	ptr_maps_build(g_target_pid);
	symtab_build(g_target_pid);
	debug_line_build(g_target_pid);

    wait(&status);

    while(1)
    {       
#if 0
		int signum = WSTOPSIG(status);
		if (signum == SIGSTOP) {

		}
#endif
        if(WIFEXITED(status) || (WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL))
        {
            printf("process %d terminated\n", pid);
			/* clean up, entry and return point */
			breakpoint_cleanup(g_target_pid);
			if (return_address != 0) {
				ptrace_set_data(g_target_pid, return_address, return_code);
			}

			callstack_report();
            exit(0);

        }

		if(WIFSTOPPED(status))
        {           

#if 0
            long rip =  ptrace(PTRACE_PEEKUSER, pid, 8 * RIP, NULL)-1;
            HASH_FIND_INT(brptab, &rip, brp);
            if(brp){
                HASH_DEL(brptab, brp);
                /*restore instruction(s)*/
                ptrace(PTRACE_POKETEXT, pid, brp->addr, brp->opc);
                /*decrement eip*/ 
                ptrace(PTRACE_POKEUSER, pid, 8 * RIP, rip);
                printf("process %d stopped at 0x%lx<%s>\n", pid, rip, brp->sym);
            }else{
                printf("process %d stopped at 0x%lx\n", pid, rip);
            }
#endif
			/* get registers */
			registers_info_t regs;
			ptrace_get_regs(pid, &regs);

			/* unwind the RIP back by 1 to let the CPU execute the
			 * original instruction that was there. */
			REG_RIP(regs) -= 1;
			//ptrace_set_regs(pid, &regs);

			uintptr_t rip = REG_RIP(regs);

			if ((bp = breakpoint_by_entry(rip)) != NULL) {
				/* -- at function entry */

                struct user_regs_struct regs;
				/* recover entry code */
				ptrace_set_data(pid, bp->entry_address, bp->entry_code);
                /*decrement eip*/ 
                ptrace(PTRACE_POKEUSER, pid, 8 * RIP, rip);

				/* save arguments */
				arg1 = REG_RDI(regs);
				arg2 = REG_RSI(regs);
                printf("process %d stopped at 0x%lx<%s>, address:%#x, entry:%#x \n", pid, rip, bp->name, bp->entry_address, bp->entry_code);
				
				/* clear entry */
				breakpoint_clear_by_entry(rip);
			}else{
				printf("process %d stopped at 0x%lx\n", pid, rip);
			}
        }

        switch(next_cmd(&arg))
        {
            case BREAK: /*set break point*/
            {
#if 0
                /*look up the symbol in the symbol table*/
                HASH_FIND_STR(symtab, arg, symbol);
                if(symbol)
                {
                    /*insert new break point*/
                    brp = malloc(sizeof(Symbol));
                    brp->sym  = symbol->sym;
                    brp->addr = symbol->addr;
                    /*save instruction at eip*/
                    brp->opc = ptrace(PTRACE_PEEKTEXT, pid, symbol->addr, NULL);
                    /*insert break point*/                    
                    ptrace(PTRACE_POKETEXT, pid, symbol->addr, 0xcc);
                    /*add break point to hash*/
                    HASH_ADD_INT(brptab, addr, brp);
                    printf("breakpoint at %lx<%s>\n", brp->addr, brp->sym);
                }
                else
                {
                    printf("symbol not found <%s>\n", arg);
                }                  
                free(arg);
#endif
				breakpoint_init(g_target_pid);
                break;
            }

            case NEXT: /*next instruction*/
            {
                /*read instruction pointer*/
                long rip =  ptrace(PTRACE_PEEKUSER, pid, 8 * RIP, NULL);
#if 0 
                if(old_rip)
                {
                    /*calculate instruction size*/
                    long oplen = rip - old_rip;
                    if(oplen > 0 && oplen < 16) 
                        disassemble(&opcode, oplen);
                }
                /*read two words at eip*/
                opcode[0] = ptrace(PTRACE_PEEKDATA, pid, rip, NULL);
                opcode[1] = ptrace(PTRACE_PEEKDATA, pid, rip+sizeof(long), NULL);
                old_rip = rip;
#endif
                /*next instruction*/
                ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
                wait(&status);
                break;
            }

            case REGISTER:/*dump registers*/
            {
                struct user_regs_struct regs;
                ptrace(PTRACE_GETREGS, pid, NULL, &regs);
                printf("rax 0x%lx\n", regs.rax);
                printf("rbx 0x%lx\n", regs.rbx);
                printf("rcx 0x%lx\n", regs.rcx);
                printf("rdx 0x%lx\n", regs.rdx);
                printf("rsi 0x%lx\n", regs.rsi);
                printf("rdi 0x%lx\n", regs.rdi);
                printf("rbp 0x%lx\n", regs.rbp);
                printf("rsp 0x%lx\n", regs.rsp);
                printf("rip 0x%lx\n", regs.rip);
                break;
            }
            case CONTINUE:/*continue execution*/
                ptrace(PTRACE_CONT, pid, NULL, NULL);
                wait(&status);
                break;

            case KILL: /*kill process*/
                kill(pid, SIGKILL);
                wait(&status);
                break;

            default:/*error*/
                fprintf(stderr, "invalid or unknown command\n");
        }
    }


	/* clean up, entry and return point */
	breakpoint_cleanup(g_target_pid);
	if (return_address != 0) {
		ptrace_set_data(g_target_pid, return_address, return_code);
	}
	
	callstack_report();
	return 0;
}
