/*
 * ksymoops v2.0 -- A simple filter to resolve symbols in Linux Oops-logs
 *
 * Copyright (C) 1995 Greg McGary <gkm@magilla.cichlid.com> (version 1.x)
 * Copyright (C) 1996 Alessandro Rubini <rubini@ipvvis.unipc.it> (upgrades)
 */

/*
 * New features:
 *       strips syslog prefixes
 *       decodes registers and stack
 *       uses %esp to show pointers to local vars
 *       uses symbols in modules from /proc/ksyms
 */ 

/* 
 * Missing features:
 *       check /proc/ksyms against the map for mismatches
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <linux/module.h>


#define MAX_NAME MOD_MAX_NAME /* quite big, indeed ... */
struct ksym {
  unsigned long address, extent;
  struct ksym *next;
  char name[MAX_NAME];
  char id[MAX_NAME];
};

struct ksym *gsymlist; /* global symlist */

/*----------------------------------------------------------------------*/
/* read a text line into a symbol */
struct  ksym *linesym(char *line)
{
    struct ksym *newsym=malloc(sizeof(struct ksym));
    char s[MAX_NAME];
    int i;
    
    i=sscanf(line,"%lx %s %s",&newsym->address,newsym->name,s);
    switch(i) {
      case 2:
        newsym->id[0]='\0'; /* no id available, but allright */
        break;
      case 3:
        if (strlen(newsym->name)==1) {        /* a line in the system map */
            strcpy(newsym->id,newsym->name);  /* use segment as id */
            newsym->id[0]=toupper(newsym->id[0]);
            strcpy(newsym->name,s);           /* this was the name */
            break;
        }
        if (s[0]=='[') { /* a module from "ksyms" */
            s[strlen(s)-1]='\0'; /* trim the bracket */
            strcpy(newsym->id,s+1);
            break;
        }

      default:         /* unknown format */
        free(newsym);
        return NULL;
    }
    newsym->extent = newsym->extent = 0;
    newsym->next = NULL;
    return newsym;
}

/*----------------------------------------------------------------------*/
/*
 * insert a symbol in the list.
 * "trial" is an hint used to speed things  -- it's tenfold faster 
 */
struct ksym *insert(struct ksym *symlist, struct ksym *newsym,
                   struct ksym *trial)
{
    if (!symlist) return newsym;
    if (!trial || trial->address > newsym->address) {
        if (newsym->address < symlist->address) {
            newsym->next = symlist;
            return newsym;
        }
        trial = symlist;
        /* disregard hint */
    }
    for ( ; trial->next; trial=trial->next)
        if (trial->next->address > newsym->address)
            break;

    newsym->next = trial->next;
    trial->next = newsym;

    return symlist;
}


/*----------------------------------------------------------------------*/
/* read a file into a symlist */
struct ksym *filesym(struct ksym *symlist, FILE *f, const char *fname)
{
    char s[80];
    int lineno = 0;
    struct ksym *last = NULL, *sym;

    if (!f) {
        f=fopen(fname,"r");
        if (!f) {
            perror(fname);
            return symlist;
        }
    }
    while (fgets(s,80,f)) {
        lineno++;
        sym = linesym(s);
        if (!sym) {
            fprintf(stderr,"%s:%i: Unknown format for data line\n",
                    fname,lineno);
            continue;
        }
        symlist = insert(symlist,sym,last);
        last = sym;
    }
    return symlist;
}

/*----------------------------------------------------------------------*/
/* write extents and remove duplicate items */
struct ksym *fixlist(struct ksym *symlist)
{ 
    struct ksym *ptr, *next;
    
    for (ptr=symlist; ptr && ptr->next; ptr = ptr->next) {
        while (ptr->address == ptr->next->address) { /* remove duplicates */
            next=ptr->next;
            if (ptr->id[0] && next->id[0]) /* both valid... hmmm */
                break;
            if (next->id[0])
                strcpy(ptr->id,next->id);
            ptr->next=next->next;
            free(next);
        }
        if (!strcmp(ptr->id,ptr->next->id))
            ptr->extent = ptr->next->address - ptr->address;
        else
            ptr->extent = 0;
    }
    ptr->extent=0;
 
    return symlist;
}
/*----------------------------------------------------------------------*/
#ifdef DEBUG
int dumplist(struct ksym *symlist)
{
    for (; symlist; symlist=symlist->next) {
       printf("%08lx %-30s (e %08lx, id \"%s\")\n",
              symlist->address,
              symlist->name, symlist->extent, symlist->id);
       fflush(stdout);
    }
    return 0;
}    
#endif /* DEBUG */                       

/*----------------------------------------------------------------------*/
/* print an address in symbolic form */
char *decode(unsigned int add, unsigned int esp)
{
    static char res[64];
    static struct ksym *symlist=NULL; /* used to remember last time */

    if (!symlist || symlist->address > add) symlist=gsymlist;
    for (;symlist && symlist->next && symlist->next->address <= add;
         symlist=symlist->next)
        /* nothing */;
    if (add < symlist->address || !symlist->extent) { /* out of regions */
        if (abs(add-esp) < 2000) {
            sprintf(res,"<%%esp+%x>",add-esp);
            return res;
        } else
            return "";
    }
    sprintf(res,"<%s+%lx/%lx>",symlist->name,add-symlist->address,
            symlist->extent);
    return res;
}

/*----------------------------------------------------------------------*/
int disass(char *data, unsigned int eip, unsigned int esp)
{
    /*
     * This is a hack to avoid using gcc.  We create an object file by
     * concatenating objfile_head, the twenty bytes of code, and
     * objfile_tail. 
     */
    static unsigned char objfile_head[] = {
	0x07, 0x01, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    static unsigned char objfile_tail[] = {
	0x00, 0x90, 0x90, 0x90,
	0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00,
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x25, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x00, 0x00,
	'g',  'c',  'c',  '2',  '_',  'c',  'o',  'm',  
	'p',  'i',  'l',  'e',  'd',  '.',  '\0', '_',  
	'E',  'I',  'P',  '\0', '\0', '\0', '\0', '\0',
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
	'\0', '\0', '\0', '\0', '\0', '\0'
    };

    char * objdump = "objdump -d /tmp/oops_decode.o";
    char * objfile = "/tmp/oops_decode.o";
    char buf[128], *bptr;
    char *s;
    FILE *f;
    long l;

    if (!(f=fopen(objfile,"w"))) {
        perror(objfile); return -1;
    }
    fwrite(objfile_head, 1, sizeof(objfile_head), f);
    fwrite(data, 1, 20, f);
    fwrite(objfile_tail, 1, sizeof(objfile_tail), f);
    fclose(f);

    if (!(f=popen(objdump,"r"))) {
        perror("objdump"); return -1;
    }

    while (fgets(buf,128,f)) {
        if (strncmp(&buf[9], "<_EIP", 5))
            continue;
	if (strstr(buf, " is out of bounds"))
	    break;
        printf("Code: ");

        /* try to print eip in decoded form */
	l = strtol(buf, &bptr, 16);
        s=decode(eip+l,esp);
        if (s[0])
            printf("%s",s); /* the decoded address */
        else
            printf("%08lx",eip+l);
        while (*bptr++ != '>')
            /* skip */ ;
        
        /* print the rest of line, without newline */
        bptr[strlen(bptr)-1]='\0';
        printf("%s",bptr);
        
        /* and now decode any address it embedded */
        while ( (bptr=strstr(bptr, "0x")) ) {
            l=strtol(bptr, &bptr, 0);
            s=decode(l, esp);
            if (s[0])
                printf(" %s", s);
        }
        putchar('\n');
    }
    pclose(f);
    unlink(objfile);
           
    return 0;
}

/*----------------------------------------------------------------------*/
/*
 * All is there: now use it
 */
int main(int argc, char **argv)
{
    char s[512];
    char *subs=NULL, *savedsubs;
    int status=0, offset;
    unsigned int j, k, esp, eip;
    unsigned int i[16];        /* 8 suffice by now, but who knows... */
    char names[16][8], c[20];  /* [8][8] suffice, but who knows */
    char *ptr;
    char *prgname=argv[0];
    struct ksym *symlist=NULL;
    char defaultmap[]="/usr/src/linux/System.map";
    char *mapname=defaultmap;


    if (argc>2) {
        fprintf(stderr,"%s: Usage: \"%s [mapfile_name] < oops-log\"\n",
                prgname, prgname);
        exit(1);
    }
    if (argc==2)
        mapname=argv[1];
    fprintf(stderr,"%s: Using %s as map\n", prgname, mapname);

    symlist = filesym(symlist,NULL,mapname);
    symlist = filesym(symlist,NULL,"/proc/ksyms");

    symlist=fixlist(symlist);
    /* dumplist(symlist); */
    gsymlist=symlist;
    /*
     * Ok, the symbol table is there, now get the message
     */
    if (isatty(0)) {
        fprintf(stderr,"%s: please paste the oops on my stdin\n",prgname);
    }

    while (fgets(s,512,stdin)) {
        if (!subs) { /* nothing yet */
            savedsubs = subs = strstr(s,"EIP: ");
        }
        switch(status) {
        case 0: /* not found */
            if (!subs) continue;
            status++;
            offset=0;
            esp=0;
            j=sscanf(subs,"%s %x:[<%x>]",s,&i[0],&i[1]);
            if (j!=3) {
                fprintf(stderr,"Wrong \"EIP\" line\n");
                continue;
            }
            printf("\n%-7s %04x:%08x %s\n",s,i[0],i[1],decode(i[1],0));
            eip=i[1]; /* keep to disass */
            break;
            
        case 1: /* before the stack */
            j=sscanf(subs,"%s %x %s %x %s %x %s %x",names[0+offset],i+offset,
                     names[1+offset],i+1+offset,names[2+offset],i+2+offset,
                     names[3+offset],i+3+offset);
            if (j==8 && strlen(names[0])==4) { /* registers */
                /*
                 * The problem with registers is that I need "esp"
                 * first, on order to refer registers to the stack.
                 * So, I save them, and decode later on
                 */
                if ( (ptr=strstr(subs,"esp:")) ) {
                    sscanf(ptr,"%*s %x",&esp);
                }
                if (esp) { /* decode only when they can be ref'd to stack */
                    for (j=0; j<4+offset; j++)
                        printf("%s %08x %s\n",names[j],i[j],decode(i[j],esp));
                    offset=0;
                } else {
                    offset+=4; /* next time, write after these ones */
                }
                continue;
            }
            else if (strncmp(subs,"Stack: ",7)) {
                printf("%s",subs);
                continue;
            } else {
                status++;
                offset=0;
                subs+=6; /* skip the string, and fall through */
            }

        case 2: /* stack and trace*/
            if (strncmp(subs,"Call Trace",4) != 0) {
                k=sscanf(subs,"%x %x %x %x %x %x %x %x",i,i+1,i+2,i+3,
                         i+4,i+5,i+6,i+7);
                if (k<=0) {
                    fprintf(stderr,"Bad stack line (%i items)\n",j);
                } else {
                    for (j=0; j<k; j++)
                        printf("esp+%02x: %08x %s\n",(offset+j)*4,i[j],
                               decode(i[j],esp));
                }
                if (!offset) subs = savedsubs; /* restore */
                offset+=8;
                continue;
            }
            /* call trace */
            status++;
            offset=0;
            subs+=12; /* skip the string, and fall through */

        case 3: /* the trace */
            if (strncmp(subs,"Code",4)) {
                k=sscanf(subs," [<%x>] [<%x>] [<%x>] [<%x>] "
                         "[<%x>] [<%x>] [<%x>] [<%x>]",i,i+1,i+2,i+3,
                         i+4,i+5,i+6,i+7);
                if (k==0) {
                    fprintf(stderr,"Bad trace line %s(no add found)\n",subs);
                } else {
                    for (j=0; j<k; j++)
                        printf("Trace: %08x %s\n", i[j], decode(i[j],esp));
                }
                subs = savedsubs; /* restore */
                continue;
            }
            /* the code */
            ptr = subs+5; /* skip "Code:" */
            for (j=0; j<20; j++) {
                long l=strtol(ptr,&ptr,16);
                c[j]=(char)l;
            }
   
            disass(c, eip, esp);
            esp=0; status=0; subs=NULL; /* ready for another oops */
        }
        
                
    }
    return 0;
}




