#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <bfd.h>
#include <dis-asm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <assert.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <errno.h>
#include <readline/readline.h>
#include "uthash.h"

typedef struct{
    char *sym;         /*symbol*/
    long addr;         /*symbol address*/
    long opc;          /*wrod at symbol*/
    UT_hash_handle hh; /*uthash handle*/
}SymbolTable, Symbol;

/*debugger commands*/
typedef enum {CONTINUE, BREAK, KILL, NEXT, REGISTER}mdb_cmd;

/*reads command from user*/
mdb_cmd next_cmd(char **argv)
{
    mdb_cmd cmd= -1;

    char *cmdstr;
    if(!(cmdstr = readline("(mdb) "))) return cmd;

    if(!strcmp(cmdstr, "continue")){
        cmd = CONTINUE;
    }else if(!strcmp(cmdstr, "kill")){   
        cmd = KILL;
    }else if(!strcmp(cmdstr, "next")){
        cmd = NEXT;
    }else if(!strcmp(cmdstr, "registers")){
        cmd = REGISTER;
    }else{
        const char *op = strtok(cmdstr, " ");
        const char *arg = strtok(NULL, " ");
        if( op && arg && !strcmp(op, "break"))
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
    struct disassemble_info info;
    init_disassemble_info (&info, stdout, (fprintf_ftype)fprintf);
    info.mach = bfd_mach_x86_64;
    info.endian = BFD_ENDIAN_LITTLE;
    info.buffer = buf;
    info.buffer_length = size;
    print_insn_i386(0, &info);
    printf("\n");    
}

int main(int argc, char **argv)
{
    if(argc == 1)
    {
        fprintf(stderr, "usage mdb <bin>");
        exit(-1);
    }

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

    /* load symbol table*/
    long size;
    long nsym;
    asymbol **asymtab;

    bfd_init();
    bfd *abfd = bfd_openr(path, NULL);
 
    bfd_check_format(abfd, bfd_object);
    size = bfd_get_symtab_upper_bound(abfd);
    asymtab = malloc(size);
    nsym = bfd_canonicalize_symtab(abfd, asymtab); /*reads symtab*/
    
    /*create symbol table hash*/
    long i;
    SymbolTable *symtab=NULL, *symbol;
    for(i = 0; i < nsym; i++)
    {
        symbol = malloc(sizeof(Symbol));
        symbol->sym  = bfd_asymbol_name(asymtab[i]);
        symbol->addr = bfd_asymbol_value(asymtab[i]);
        printf("sym: %s, addr: %#x \n", symbol->sym, symbol->addr);
        HASH_ADD_KEYPTR(hh, symtab, symbol->sym, strlen(symbol->sym), symbol);
    }

    char *arg;
    int status;
    long opcode[2];
    long old_rip;
    SymbolTable *brptab=NULL, *brp;
    struct user_regs_struct regs;

    wait(&status);

    while(1)
    {       
        if(WIFEXITED(status) || (WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL))
        {
            printf("process %d terminated\n", pid);
            exit(0);
        }

        if(WIFSTOPPED(status))
        {           
            long rip =  ptrace(PTRACE_PEEKUSER, pid, 8 * RIP, NULL)-1;
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

            HASH_FIND_INT(brptab, &rip, brp);
            if(brp){
                HASH_DEL(brptab, brp);
                /*restore instruction(s)*/
                ptrace(PTRACE_POKETEXT, pid, brp->addr, brp->opc);
                /*decrement eip*/ 
                ptrace(PTRACE_POKEUSER, pid, 8 * RIP, rip);
                printf("process %d stopped at 0x%lx<%s>, address:%x, opc:%x \n", pid, rip, brp->sym, brp->addr, brp->opc);
            }else{
                printf("process %d stopped at 0x%lx\n", pid, rip);
            }
        }
        switch(next_cmd(&arg))
        {
            case BREAK: /*set break point*/
            {
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
                    printf("breakpoint at %lx<%s>, op:%#x \n", brp->addr, brp->sym, brp->opc);
                }
                else
                {
                    printf("symbol not found <%s>\n", arg);
                }                  
                free(arg);
                break;
            }

            case NEXT: /*next instruction*/
            {
                /*read instruction pointer*/
                long rip =  ptrace(PTRACE_PEEKUSER, pid, 8 * RIP, NULL);
                
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

                /*next instruction*/
                ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
                wait(&status);
                break;
            }

            case REGISTER:/*dump registers*/
            {
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
}

