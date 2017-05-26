//#define _GNU_SOURCE
#include <dlfcn.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <execinfo.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <errno.h>
#include <signal.h>

#include <pthread.h>
#include <time.h>

#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include "mguard.h"



#define MERR  printf("[MGUARD] ERROR!! [%s]%d\n",__FILE__,__LINE__)
static int SID=1;
//
// This LD_PRELOAD library instruments malloc, calloc, realloc, memalign,
// valloc, posix_memalign, and free by outputting all calls to stderr.
//
// Unfortunately, it's not quite as straightforward as it sounds, as fprintf
// and dlsym both use heap-based memory allocation. During initialization, we
// use a dummy implementation of malloc that uses a small buffer. Once dlsym
// loading is done, then we switch to our real implementation, which, unless
// a recursive mutex is already held, first outputs the call arguments, makes
// the call, and then outputs the return value. If the recursive mutex is
// already held, then the call was due to some call made while outputting
// arguments, so we just forward the call along to the real call.
//
// Parsing this output can be done by the corresponding utility Python script.
//
// Big thanks to http://stackoverflow.com/a/10008252/379568
//
// -JT Olds <jt@spacemonkey.com>
//

// gcc -shared -fPIC -o malloc_instrument.so malloc_instrument.c -lpthread -ldl



static void* (*real_malloc)(size_t size);
static void* (*real_calloc)(size_t nmemb, size_t size);
static void* (*real_realloc)(void *ptr, size_t size);
static void* (*real_memalign)(size_t boundary, size_t size);
static void* (*real_valloc)(size_t size);
static int   (*real_posix_memalign)(void** memptr, size_t alignment, size_t size);
static void* (*real_mmap)(void *start, size_t length, int prot, int flags, int fd, off_t offset);
static void* (*real_mremap)(void *start, size_t length, size_t new_length, int flag,...);
static int   (*real_munmap)(void *addr, size_t length);
static void* (*real_dlopen)(const char *filename, int flag);
static int   (*real_dlclose)(void* handle);

static void  (*real_free)(void *ptr);
static void* (*temp_malloc)(size_t size);
static void* (*temp_calloc)(size_t nmemb, size_t size);
static void* (*temp_realloc)(void *ptr, size_t size);
static void* (*temp_memalign)(size_t blocksize, size_t bytes);
static void* (*temp_valloc)(size_t size);
static int   (*temp_posix_memalign)(void** memptr, size_t alignment, size_t size);
static void  (*temp_free)(void *ptr);
static void* (*temp_mmap)(void *start, size_t length, int prot, int flags, int fd, off_t offset);
static void* (*temp_mremap)(void *start, size_t length, size_t new_length, int flag,...);
static int   (*temp_munmap)(void *addr, size_t length);
static void* (*temp_dlopen)(const char *filename, int flag);
static int   (*temp_dlclose)(void* handle);

extern void env_load_option(void);
extern void dmalloc_message(const char *format, ...);
extern void dmalloc_message_concat(const char *format, ...);

#define uthash_malloc(x) umalloc(x)
#define uthash_free(x,y) ufree(x,y)
#include "utlist.h"
#include "uthash.h"

#define SIGNAL1     SIGUSR1
#define SIGNAL2     SIGUSR2
#define SIGNAL3     SIGTERM




#define INIT_RECORD(R)      R.backtrace_id=0;\
                            R.rv=-1;\
                            R.size=0;\
                            R.ptr=NULL;




unsigned long uthash_memory_usage=0;
void* umalloc(size_t size)
{
    uthash_memory_usage+=size;
   return (real_malloc)(size);
}


void ufree(void *ptr,size_t size)
{
    uthash_memory_usage-=size;
    (real_free)(ptr);
}


int get_SID()
{
    //I don't think we need the __sync op here, but anyway...
    __sync_fetch_and_add(&SID,1);
    return SID;
}

typedef enum {
        MALLOC_CALL=1,
        CALLOC_CALL,
        REALLOC_CALL,
        VALLOC_CALL,

        MEMALIGN_CALL,
        POSIX_MEMALIGN_CALL,

        MMAP_CALL,
        MREMAP_CALL,
        MUNMAP_CALL,

        DLOPEN_CALL,
        DLCLOSE_CALL,

        FREE_CALL,
        TOTAL_CALL_TYPE_SIZE
}mcall_type ;

typedef struct {
    mcall_type mtype;
    size_t size;
    void* ptr;
    int rv;
    size_t backtrace_id;


    union {


        struct {
            size_t ncount;
            size_t nsize;
        } calloc_call;

        struct {
            void* in_ptr;
        } realloc_call;

        struct {
            size_t boundary;
        } memalign_call;

        struct {
            void** memptr;
            size_t alignment;
        } posix_memalign_call;

        struct{
        	void *start;
        	int prot;
        	int flag;
        	int fd;
        	off_t offset;
        } mmap_call;

        struct {
            void *in_ptr;
        	size_t in_size;
        	int flag;
		} mremap_call;


		struct {
			const char *filename;
			int flag;
		} dlopen_call;


  };


} mcall_record;


struct backtrace_struct {
	size_t id;            /* we'll use this field as the key */
	pid_t pid;
	pid_t tid;
	mcall_type mtype;
    void *stack[MAX_CALL_STACK_LEN];
    unsigned int cnt;
    unsigned int tracked;
    UT_hash_handle hh; /* makes this structure hashable */
};

struct mcall_struct{
    size_t id;
    int address_cnt;
    mcall_record record;
    UT_hash_handle hh; /* makes this structure hashable */
};

struct hash_id_struct {
    size_t id;            /* we'll use this field as the key */
    UT_hash_handle hh; /* makes this structure hashable */
};

__thread unsigned int entered = 0;
static int call_count_list[TOTAL_CALL_TYPE_SIZE]={0};

static struct backtrace_struct *btrace=NULL;
static struct mcall_struct *mcall_table=NULL;

static pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;
static int initialized = 0;

static int forked=0;

long mguard_opt_debug=(OPT_DBG_NO_TRACK_DLOPEN|OPT_DBG_NO_TRACK_MEMALIGN);
size_t mguard_opt_rpid=0;

static pthread_t report_thread=0;
static int generating_report=0;
static pthread_rwlock_t btrace_table_lock;
static pthread_rwlock_t mcall_table_lock;

extern void mguard_open_log(int sid,char *timestamp_str);
extern void mguard_close_log(void);
extern void env_setup_logpath(void);
void do_unwind_backtrace(void);

pid_t ourgetpid() {
  return syscall(SYS_getpid);
}

pid_t ourgettid() {
  return syscall(SYS_gettid);
}

static void prepare_fork()
{
//	printf("!!!!0[%d]\n",ourgettid());
	forked=1;
}

static void parent_fork()
{
//   printf("!!!!1[%d]\n",ourgettid());
   forked=0;
}

static void child_fork()
{
//	printf("!!!!2[%d]\n",ourgettid());
    forked=1;

}

static char tmpbuf[1024];
static unsigned long tmppos = 0;
static unsigned long tmpallocs = 0;

static void* dummy_malloc(size_t size) {
    if (tmppos + size >= sizeof(tmpbuf)) exit(1);
    void *retptr = tmpbuf + tmppos;
    tmppos += size;
    ++tmpallocs;
    return retptr;
}

static void* dummy_calloc(size_t nmemb, size_t size) {
    void *ptr = dummy_malloc(nmemb * size);
    unsigned int i = 0;
    for (; i < nmemb * size; ++i)
        ((char*)ptr )[i] = '\0';
    return ptr;
}


static void report_meminfo()
{
    FILE *fp;
    int nread=0;
    size_t len = 1024;
    char *buf=NULL;
    char* buffer = (char*)malloc( 1024 );
    char file[]="/proc/meminfo";
    char content[16]="";
    fp = fopen(file, "rb");
    if (fp == NULL)
    {
        printf("error to open: %s\n", file);
        exit(EXIT_FAILURE);
    }
    while((nread = getline(&buffer, &len, fp)) != -1) {
        if((buf=strstr(buffer, "MemTotal"))!=NULL)
        {
            buffer[strlen(buffer)-1]=0;
            sscanf(buffer, "%s%s", content, content);
            int memtotal_kb=(int)(strtof(content, NULL));
            dmalloc_message("MemTotal:\t%dkB",memtotal_kb);
        }
        if((buf=strstr(buffer, "MemFree"))!=NULL)
        {
            buffer[strlen(buffer)-1]=0;
            sscanf(buffer, "%s%s", content, content);
            int memfree_kb=(int)(strtof(content, NULL));
            dmalloc_message("MemFree:\t%dkB",memfree_kb);
        }

        if((buf=strstr(buffer, "MemAvailable"))!=NULL)
        {
            buffer[strlen(buffer)-1]=0;
            sscanf(buffer, "%s%s", content, content);
            int memava_kb=(int)(strtof(content, NULL));
            dmalloc_message("MemAvailable:\t%dkB\n",memava_kb);

            break;
        }
        memset((void *)buffer,0,sizeof(buffer));
    }
    dmalloc_message("MemMGUARD:\t%dkB\n",(uthash_memory_usage/1024));
    dmalloc_message("\n");
    fclose(fp);
    free( (void*)buffer );
}


static void dummy_free(void *ptr) {
	ptr=ptr;
}

static void *report_job(void *arg)
{
    int signal=*((int*)arg);
    struct mcall_struct *mtable=NULL;
    struct mcall_struct *rmst;
    struct mcall_struct *rtmp;
    int sid=get_SID();
    time_t now;
    char    datetime[100];
    time(&now);
    strftime(datetime, 100, "%Y%m%d%H%M%S", localtime(&now));


    //not to bother with mguard itself
    entered=1;

    printf("[MGUARD] Start report generation thread!!\n");

    mguard_open_log(sid,datetime);

    dmalloc_message("Process: %s[%d]",program_invocation_short_name,ourgetpid());
    //clone the mcall_table
    if (pthread_rwlock_rdlock(&mcall_table_lock) != 0) MERR;
    HASH_ITER(hh, mcall_table, rmst, rtmp) {

        struct mcall_struct *mst=(struct mcall_struct *)uthash_malloc(sizeof(struct mcall_struct));
        memcpy(&mst->record,&rmst->record,sizeof(mcall_record));
        mst->id=rmst->id;
        mst->address_cnt=rmst->address_cnt;
        HASH_ADD(hh,mtable,id,sizeof(size_t),mst);
    }
    pthread_rwlock_unlock(&mcall_table_lock);
    if(SIGNAL1==signal)
    {
        int i=0;


        report_meminfo();

        printf("\n[MGUARD]  Report_%d start @%s\n",sid,datetime);
        dmalloc_message("[MGUARD]Report_%d\n========@%s\n",sid,datetime);
        dmalloc_message(":[TYPE]:[MEM_PTR]:[TOTAL_SIZE]:[MCALL_TYPE]:[ADDRESS_COUNT]:[BACKTRACE_ID]\n");
        HASH_ITER(hh, mtable, rmst, rtmp) {
          if(rmst->record.mtype==FREE_CALL)
          {
              dmalloc_message(":F:0x%08X:%d:%d:%d:0x%08X",rmst->id,rmst->record.size,(int)rmst->record.mtype,rmst->address_cnt,rmst->record.backtrace_id);
          }
          else
          {
              dmalloc_message(":M:0x%08X:%d:%d:%d:0x%08X",rmst->id,rmst->record.size,(int)rmst->record.mtype,rmst->address_cnt,rmst->record.backtrace_id);
          }
          i++;
        //          free(s->str);
        }
        dmalloc_message("========[%d]@%s\n\n",i,datetime);
        printf("[MGUARD]  Report_%d end @%s\n",sid,datetime);


        {
            struct backtrace_struct *rbt;
            struct hash_id_struct *ubt_table=NULL;
            struct hash_id_struct *ubt;
            struct hash_id_struct *tubt;
                //
            int i=0,j=0;

            printf("[MGUARD]  BTList_%d start @%s\n",sid,datetime);

            dmalloc_message("[MGUARD]BTList_%d\n++++++++@%s\n",sid,datetime);
            dmalloc_message(":[TYPE]:[BACKTRACE_ID]:[MCALL_TYPE]:[XCOUNT]:[PID]:[TID]:[BACKTRACE]\n");
            HASH_ITER(hh, mtable, rmst, rtmp) {
                size_t bid=rmst->record.backtrace_id;
                HASH_FIND(hh,btrace,&bid,sizeof(size_t),rbt);
                if(rbt!=NULL)
                {
                    //if used, do not gernerate the report
                    HASH_FIND(hh,ubt_table,&bid,sizeof(size_t),ubt);
                    if(ubt==NULL)
                    {
                        dmalloc_message_concat(":X:0x%08X:%d:%d:%d:%d:",rbt->id,rbt->mtype,rbt->cnt,rbt->pid,rbt->tid);
                        i++;
                        for(j=0;j<MAX_CALL_STACK_LEN;j++)
                        {
                            Dl_info info;
                            if(NULL==rbt->stack[j])break;
                            dladdr(rbt->stack[j], &info);
                            if(info.dli_sname != NULL)
                            {
                                dmalloc_message_concat("%s+0x%X/",info.dli_sname,((size_t)rbt->stack[j]-(size_t)info.dli_saddr));
                            }
                            else
                            {
                                dmalloc_message_concat("0x%08X/",(int)rbt->stack[j]);
                            }


                        }
                        dmalloc_message_concat("\n");
                        ubt=(struct hash_id_struct *)uthash_malloc(sizeof(struct hash_id_struct));
                        ubt->id=bid;
                        HASH_ADD(hh,ubt_table,id,sizeof(size_t),ubt);

                    }
                }
                                //          free(s->str);
            }


            HASH_ITER(hh, ubt_table, ubt, tubt) {
                HASH_DELETE(hh,ubt_table,ubt);
                uthash_free(ubt,sizeof(struct hash_id_struct));
                ubt=NULL;
            }
            ubt_table=NULL;
            dmalloc_message("++++++++[%d/%d]@%s\n",i,HASH_COUNT(mtable),datetime);
            printf("[MGUARD]  BTList_%d end @%s\n",sid,datetime);
        }
    }

//BEACH:
    //free the tmp table

    HASH_ITER(hh, mtable, rmst, rtmp) {
        HASH_DELETE(hh,mtable,rmst);
        uthash_free(rmst,sizeof(struct mcall_struct));
        rmst=NULL;
    }
    sync();
    mguard_close_log();
    printf("[MGUARD] End report generation thread!!\n");
    __sync_fetch_and_sub(&generating_report,1);

    return NULL;
}


static  void  signal_SIGNAL_handler(const int sig)
{
    int rc;
    pthread_attr_t attr;
    if(__sync_fetch_and_add(&generating_report,1))
    {
        __sync_fetch_and_sub(&generating_report,1);
        return;
    }

    pthread_attr_init (&attr);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
    rc=pthread_create(&report_thread,&attr,report_job,(void *)&sig);
    if(rc!=0){
        printf("[MGUARD] report job thread create ERROR!! %d\n",rc);
    }
    pthread_attr_destroy (&attr);

}


static void mguard_config(void)
{

    env_setup_logpath();
    env_load_option();
    (void)signal(SIGNAL1, signal_SIGNAL_handler);
    (void)signal(SIGNAL2, signal_SIGNAL_handler);
}

bool is_pointer_valid(void *p) {
	int ret=0;
    /* get the page size */
    size_t page_size = sysconf(_SC_PAGESIZE);
    /* find the address of the page that contains p */
    void *base = (void *)((((size_t)p) / page_size) * page_size);
    /* call msync, if it returns non-zero, return false */
    ret = msync(base, page_size, MS_ASYNC) != -1;
    return ret ? ret : errno != ENOMEM;
}

extern char env_opt_rname[];
extern size_t env_opt_rpid;
extern int env_opt_remove_zero_called_btrace;
extern int env_opt_track;
static int guarding_flag=1;
void set_guarding_flag(int flag)
{
    guarding_flag=flag;
}
static int should_be_guard(mcall_record *record)
{
    if(0==guarding_flag && strlen(env_opt_rname)>0 && (0==strncmp(program_invocation_short_name,env_opt_rname,strlen(env_opt_rname))) )
    {
        printf("[MGUARD] Process: %s tracked, pid=%d\n",program_invocation_short_name,ourgetpid());
        guarding_flag=1;
    }

//    if(env_opt_rpid>0 && ourgetpid()!=((pid_t)env_opt_rpid))
//    {
//       return 0;
//    }

    switch(record->mtype) {
        case MMAP_CALL:
        case MREMAP_CALL:
        case MUNMAP_CALL:
            if(mguard_opt_debug & OPT_DBG_NO_TRACK_MMAP)return 0;
            break;
        case DLOPEN_CALL:
        case DLCLOSE_CALL:
            if(mguard_opt_debug & OPT_DBG_NO_TRACK_DLOPEN)return 0;
            break;
        case POSIX_MEMALIGN_CALL:
        case MEMALIGN_CALL:
             if(mguard_opt_debug & OPT_DBG_NO_TRACK_MEMALIGN)return 0;
            break;
        default:
            ;

    }
    return guarding_flag;
}

//static size_t _Znwj_addr=0;
//static size_t _Znaj_addr=0;

static void pre_guard(mcall_record *record) {


    unw_cursor_t cursor;
    unw_context_t uc;
    unw_word_t pc;
    unw_word_t offset=0;
    char        fname[64];
    int ret;
    int i=0;

    struct backtrace_struct _bt;
    offset=offset;
    fname[0]=fname[0];

    unw_getcontext(&uc);

    if((ret=unw_init_local(&cursor, &uc))!=0)
    {
        printf("unw_init_local error, %d\n",ret);
        return;
    }

    _bt.tid=ourgettid();
    _bt.pid=ourgetpid();
    _bt.id=_bt.pid;
    _bt.tracked=0;
    _bt.mtype=record->mtype;
    if(env_opt_track)_bt.tracked=1;

    if( (unw_step(&cursor) <= 0))
    {
        return;
    }
    if( (unw_step(&cursor) <= 0))
    {
        return;
    }
    if( (unw_step(&cursor) <= 0) || unw_get_reg(&cursor, UNW_REG_IP, &pc) !=0 )
    {
        return;
    }


#if 0
    if(_Znwj_addr==0)
    {
        fname[0] = '\0';

        unw_get_proc_name(&cursor, fname, sizeof(fname), &offset);
        if((fname[0]=='_' && fname[1]== 'Z' &&fname[2]=='n' && fname[3]=='w'))
        {
            _Znwj_addr=pc;
            //        i++;
            //        printf ("===> %p : (%s+0x%x) [%d]\n", pc, fname, offset,record->malloc_call.size);
        }
    }
    if(pc!=_Znwj_addr)
    {
        return;
    }
    _bt.id+=(size_t)pc;
    _bt.stack[i]=(void*)pc;
    _bt.cursor[i]=&cursor;
    i++;

    if( (unw_step(&cursor) <= 0) || unw_get_reg(&cursor, UNW_REG_IP, &pc) !=0 )
    {
        return;
    }
    if(_Znaj_addr==0)
    {
       fname[0] = '\0';

       unw_get_proc_name(&cursor, fname, sizeof(fname), &offset);
       if((fname[0]=='_' && fname[1]== 'Z' &&fname[2]=='n' && fname[3]=='a'))
       {
           _Znaj_addr=pc;
           //        i++;
           //        printf ("===> %p : (%s+0x%x) [%d]\n", pc, fname, offset,record->malloc_call.size);
       }
    }
    if(pc!=_Znaj_addr)
    {
       return;
    }


#endif
    _bt.id+=(size_t)pc;
    _bt.stack[i]=(void*)pc;
    i++;

    while (1) {
        if( (unw_step(&cursor) <= 0) || (i >= MAX_CALL_STACK_LEN))
        {
            break;
        }

        if((ret=unw_get_reg(&cursor, UNW_REG_IP, &pc))!=0)
        {
            break;
        }
        if(pc==0)
        {
            break;
        }

        _bt.id+=(size_t)pc;
        _bt.stack[i]=(void*)pc;
        i++;

    }


    if(i!=0)
    {
        //printf("\n");
        struct backtrace_struct *tbt=NULL;
        if (pthread_rwlock_wrlock(&btrace_table_lock) != 0) MERR;
        HASH_FIND(hh,btrace,&_bt.id,sizeof(size_t),tbt);
        if(tbt==NULL)
        {
#if 0 // customized track filter for SZ JOVI OOM
            if(FREE_CALL!=record->mtype)
            {
                _bt.tracked=0;
            }
//            else
//            {
//                struct mcall_struct *sst=NULL;
//                size_t sid=(size_t)record->ptr;
//
//                HASH_FIND(hh,mcall_table,&sid,sizeof(size_t),sst);
//
//                if(sst==NULL) _bt.tracked=0;
//            }

            if(_bt.tracked==0)
            {
                int h=0;
                unw_cursor_t cursor2;
                unw_context_t uc2;
                unw_getcontext(&uc2);
                if(unw_init_local(&cursor2, &uc2)==0)
                {
                    for(h=0;h<5;h++){ if( (unw_step(&cursor2) <= 0))break;}
                    if(5==h)
                    {
                        memset(fname,0,sizeof(fname));

                        unw_get_proc_name(&cursor2, fname, sizeof(fname), &offset);
                        if(   0==strncmp(fname,"_ZN12CSndLossList6insertERKiS1_",sizeof("_ZN12CSndLossList6insertERKiS1_"))
                              || 0==strncmp(fname,"_ZN10CRcvBufferC2ERKiP10CUnitQueue",sizeof("_ZN10CRcvBufferC2ERKiP10CUnitQueue"))
                              || 0==strncmp(fname,"_ZN10CACKWindowC2ERKi",sizeof("_ZN10CACKWindowC2ERKi"))
                              || 0==strncmp(fname,"_ZN10CSndBuffer8increaseEv",sizeof("_ZN10CSndBuffer8increaseEv"))
                           )
                        {

                            _bt.tracked=1;
                            printf("captured!!\n");
                        }

                    }
                 }
            }
#endif

           if (0==env_opt_track || _bt.tracked >0)
           {
//               printf("btrace added!!\n");
               tbt=(struct backtrace_struct*)uthash_malloc(sizeof(struct backtrace_struct));
               memcpy(tbt,&_bt,sizeof(struct backtrace_struct));
               HASH_ADD(hh,btrace,id,sizeof(size_t),tbt);
               tbt->cnt=1;
           }

        }
        else
        {
            tbt->cnt++;
        }


        pthread_rwlock_unlock(&btrace_table_lock);
    }
    record->backtrace_id=_bt.id;

    if(env_opt_track > 0 && 0==_bt.tracked)
    {
        record->backtrace_id=0;
    }

}



void post_guard(mcall_record* record)
{
    size_t mid;
    struct mcall_struct *mst;
    switch(record->mtype) {

        case MALLOC_CALL:
        case CALLOC_CALL:
        case REALLOC_CALL:
        {
            goto ADD_MTABLE_RECORD;
        }
        break;


        case FREE_CALL:
        {
            mid=(size_t)record->ptr;
            if (pthread_rwlock_wrlock(&mcall_table_lock) != 0) MERR;
            HASH_FIND(hh,mcall_table,&mid,sizeof(size_t),mst);

            if(mst==NULL)
            {
               // printf("!!!! FREE ERROR 0x%08X !!!!\n",(unsigned int)ptr);
            }
            else
            {

                if(env_opt_remove_zero_called_btrace>0 && mst->record.backtrace_id!=0)
                {

                    struct backtrace_struct *tbt=NULL;
                    HASH_FIND(hh,btrace,&(mst->record.backtrace_id),sizeof(size_t),tbt);
                    if(tbt!=NULL)
                    {
                        if(0==(--tbt->cnt) && (env_opt_track == 0 || 0==tbt->tracked))
                        {
                            HASH_DELETE(hh,btrace,tbt);
                            uthash_free(tbt,sizeof(struct backtrace_struct));
                            tbt=NULL;
                        }
                    }
                }

                if(env_opt_track>0 && record->backtrace_id!=0)
                {
                    struct mcall_struct *fst;
                    size_t fid;
                    fid=mid+1;
                    HASH_FIND(hh,mcall_table,&fid,sizeof(size_t),fst);
                    if(fst!=NULL)
                    {
                        fst->address_cnt+=1;
                        fst->record.size+=record->size;
                    }
                    else
                    {
                        fst=(struct mcall_struct*)uthash_malloc(sizeof(struct mcall_struct));
                        memcpy(&fst->record,record,sizeof(mcall_record));
                        fst->id=fid;
                        fst->address_cnt=1;
                        HASH_ADD(hh,mcall_table,id,sizeof(size_t),fst);
//                        printf("free added!!\n");
                    }

                }
                else
                {

                    HASH_DELETE(hh,mcall_table,mst);
                    uthash_free(mst,sizeof(struct mcall_struct));
                    mst=NULL;
//                    printf("freed!!\n");
                }

            }
            pthread_rwlock_unlock(&mcall_table_lock);

        }
        return;

        default:
        return;
    }


ADD_MTABLE_RECORD:
    if(record->backtrace_id!=0 && record->ptr!=NULL){

           //printf("\n");

           mid=(size_t)record->ptr;
           if (pthread_rwlock_wrlock(&mcall_table_lock) != 0) MERR;
           HASH_FIND(hh,mcall_table,&mid,sizeof(size_t),mst);
           if(mst!=NULL)
           {

               if(record->mtype==MREMAP_CALL || record->mtype==REALLOC_CALL )
               {
                   memcpy(&mst->record,record,sizeof(mcall_record));
               }
               else
               {

                   if(env_opt_track)
                   {
                       mst->address_cnt+=1;
                       mst->record.size+=record->size;

                   }
                   else
                   {
                       printf("!!!!! [MGUARD] NOT NULL 0x%08X,%d %d !!!!\n",mst->id,mst->record.mtype,record->mtype);
                   }

               }

           }
           else
           {
               mst=(struct mcall_struct*)uthash_malloc(sizeof(struct mcall_struct));
               memcpy(&mst->record,record,sizeof(mcall_record));
               mst->id=mid;
               mst->address_cnt=1;
               HASH_ADD(hh,mcall_table,id,sizeof(size_t),mst);
//               printf("added!!\n");
               //
           }
           pthread_rwlock_unlock(&mcall_table_lock);
    }

}



static void  hookfns()
{

    if(initialized)return;

    pthread_mutex_lock(&init_mutex);
    if (!initialized)
    {


        real_malloc         = dummy_malloc;
        real_calloc         = dummy_calloc;
        real_realloc        = NULL;
        real_free           = dummy_free;
        real_memalign       = NULL;
        real_valloc         = NULL;
        real_posix_memalign = NULL;

        if (pthread_rwlock_init(&btrace_table_lock,NULL) != 0)
        {
            MERR;
            entered=0x10;
        }
        if (pthread_rwlock_init(&mcall_table_lock,NULL) != 0)
        {
            MERR;
            entered=0x20;
        }


        mguard_config();


        pthread_atfork(&prepare_fork, &parent_fork, &child_fork);

        temp_malloc         = (void* (*)(size_t))dlsym(RTLD_NEXT, "malloc");
        temp_calloc         = (void* (*)(size_t, size_t))dlsym(RTLD_NEXT, "calloc");
        temp_realloc        = (void* (*)(void*, size_t))dlsym(RTLD_NEXT, "realloc");
        temp_free           = (void (*)(void*))dlsym(RTLD_NEXT, "free");

        temp_memalign       = (void* (*)(size_t, size_t))dlsym(RTLD_NEXT, "memalign");
        temp_valloc         = (void* (*)(size_t))dlsym(RTLD_NEXT, "valloc");
        temp_posix_memalign = (int (*)(void**, size_t, size_t))dlsym(RTLD_NEXT, "posix_memalign");

        temp_mmap           = (void* (*)(void*, size_t, int, int, int, off_t))dlsym(RTLD_NEXT, "mmap");
        temp_mremap         = (void* (*)(void*, size_t, size_t, int, ...))dlsym(RTLD_NEXT, "mremap");
        temp_munmap         = (int (*)(void*, size_t))dlsym(RTLD_NEXT, "munmap");

        temp_dlopen         = (void* (*)(const char*, int))dlsym(RTLD_NEXT, "dlopen");
        temp_dlclose        = (int (*)(void*))dlsym(RTLD_NEXT, "dlclose");

        if (!temp_malloc || !temp_calloc || !temp_realloc || !temp_memalign ||
            !temp_valloc || !temp_posix_memalign || !temp_free || !temp_mmap || !temp_mremap || !temp_munmap || !temp_dlopen || !temp_dlclose )
        {
            fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
            exit(1);
        }

        real_malloc         = temp_malloc;
        real_calloc         = temp_calloc;
        real_realloc        = temp_realloc;
        real_free           = temp_free;
        real_memalign       = temp_memalign;
        real_valloc         = temp_valloc;
        real_posix_memalign = temp_posix_memalign;

        real_mmap           = temp_mmap;
        real_mremap         = temp_mremap;
        real_munmap         = temp_munmap;
        real_dlopen         = temp_dlopen;
        real_dlclose        = temp_dlclose;
        initialized=1;
    }
    pthread_mutex_unlock(&init_mutex);
}


static int call_begin()
{
  return entered++;
}

static void call_end()
{
  --entered;
}


void do_unwind_backtrace()
{
    unw_cursor_t    cursor;
    unw_context_t   context;

    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

    while (unw_step(&cursor) > 0) {
        unw_word_t  offset, pc;
        char        fname[64];

        unw_get_reg(&cursor, UNW_REG_IP, &pc);

        fname[0] = '\0';
        (void) unw_get_proc_name(&cursor, fname, sizeof(fname), &offset);

        printf ("%p : (%s+0x%x) [%p]\n", (void *)pc, fname, offset, (void *)pc);
    }
    printf("---------------------------------------------------------\n");
}

static void do_mcall(mcall_record *record,va_list args) {

    int internal=call_begin();
    if((!forked) && (!internal))
    {
        hookfns();
        call_count_list[record->mtype]+=1;

        if(should_be_guard(record))
        {
	        pre_guard(record);
        }
    }


    switch(record->mtype) {

        case MALLOC_CALL:
        {
        	record->ptr = real_malloc(record->size);
        }
        break;

        case CALLOC_CALL:
        {
        	record->ptr = real_calloc(record->calloc_call.ncount, record->calloc_call.nsize);
        }
        break;

        case REALLOC_CALL:
        {
        	record->ptr = real_realloc(record->realloc_call.in_ptr, record->size);
        }
        break;

        case MEMALIGN_CALL:
        {
        	record->ptr = real_memalign(record->memalign_call.boundary, record->size);
        }
        break;

        case VALLOC_CALL:
        {
        	record->ptr = real_valloc(record->size);
        }
        break;

        case POSIX_MEMALIGN_CALL:
        {
        	record->rv = real_posix_memalign(record->posix_memalign_call.memptr, record->posix_memalign_call.alignment, record->size);
        }
        break;

        case FREE_CALL:
		{
			real_free(record->ptr);
		}
		break;

        case MMAP_CALL:
        {
        	record->ptr=real_mmap(record->mmap_call.start,record->size,record->mmap_call.prot,record->mmap_call.flag,record->mmap_call.fd,record->mmap_call.offset);
        }
        break;

        case MREMAP_CALL:
		{

			record->ptr=real_mremap(record->mremap_call.in_ptr,record->mremap_call.in_size,record->size,record->mremap_call.flag,args);
		}
		break;

        case MUNMAP_CALL:
		{
			record->rv=real_munmap(record->ptr,record->size);
		}
		break;

        case DLOPEN_CALL:
		{
			record->ptr=real_dlopen(record->dlopen_call.filename,record->dlopen_call.flag);
//			printf("ptr=0x%08X\n",record->ptr);
		}
        break;

        case DLCLOSE_CALL:
		{
//		    printf("ptr=0x%08X\n",record->ptr);
			record->rv=real_dlclose(record->ptr);
		}
		break;
        default:
            break;


    };

    if( (!forked) && (!internal) && should_be_guard(record) )
    {
   	    post_guard(record);
    }

    call_end();

}
static va_list nonused_va;

void* malloc(size_t size)
{
    mcall_record record;
    INIT_RECORD(record);
    record.mtype = MALLOC_CALL;
    record.size = size;

    do_mcall(&record,nonused_va);

    return record.ptr;
}

void* calloc(size_t ncount, size_t nsize)
{
    mcall_record record;
    INIT_RECORD(record);
    record.mtype = CALLOC_CALL;
    record.calloc_call.ncount = ncount;
    record.calloc_call.nsize = nsize;
    record.size=ncount*nsize;

    do_mcall(&record,nonused_va);

    return record.ptr;
}

void* realloc(void *ptr, size_t size)
{
    mcall_record record;
    INIT_RECORD(record);
    record.mtype = REALLOC_CALL;
    record.realloc_call.in_ptr = ptr;
    record.size = size;

    do_mcall(&record,nonused_va);

    return record.ptr;
}

void free(void *ptr)
{
    mcall_record record;
    INIT_RECORD(record);
    record.mtype = FREE_CALL;
    record.ptr = ptr;

	do_mcall(&record,nonused_va);
}

void* dlopen(const char* filename, int flag)
{
    mcall_record record;
    INIT_RECORD(record);
    record.mtype = DLOPEN_CALL;
    record.dlopen_call.filename=filename;
    record.dlopen_call.flag = flag;

    do_mcall(&record,nonused_va);

    return record.ptr;
}

int dlclose(void* handle)
{
    mcall_record record;
    INIT_RECORD(record);
    record.mtype = DLCLOSE_CALL;
    record.ptr = handle;

    do_mcall(&record,nonused_va);

    return record.rv;
}

void* memalign(size_t boundary, size_t size)
{
    mcall_record record;
    INIT_RECORD(record);
    record.mtype = MEMALIGN_CALL;
    record.memalign_call.boundary = boundary;
    record.size = size;

	do_mcall(&record,nonused_va);

    return record.ptr;
}

int posix_memalign(void** memptr, size_t alignment, size_t size)
{
    mcall_record record;
    INIT_RECORD(record);
    record.mtype = POSIX_MEMALIGN_CALL;
    record.posix_memalign_call.memptr = memptr;
    record.posix_memalign_call.alignment = alignment;
    record.size = size;

	do_mcall(&record,nonused_va);

    return record.rv;
}

void* valloc(size_t size)
{
    mcall_record record;
    INIT_RECORD(record);
    record.mtype = VALLOC_CALL;
    record.size = size;

	do_mcall(&record,nonused_va);

    return record.ptr;
}

void* mmap(void *start, size_t size, int prot, int flag, int fd, off_t offset)
{
    mcall_record record;
    INIT_RECORD(record);
    record.mtype = MMAP_CALL;
    record.mmap_call.start = start;
    record.size=size;
    record.mmap_call.prot = prot;
    record.mmap_call.flag = flag;
    record.mmap_call.fd = fd;
    record.mmap_call.offset = offset;

    do_mcall(&record,nonused_va);

    return record.ptr;

}

//DO HAVE PROBLEMS on mapping
void* mremap(void *in_ptr, size_t in_size, size_t size, int flag,...)
{
    va_list args;
    mcall_record record;
    record.mtype = MREMAP_CALL;
    record.mremap_call.in_ptr = in_ptr;
    record.size = size;
    record.mremap_call.in_size = in_size;
    record.mremap_call.flag = flag;

    va_start(args, flag);
    do_mcall(&record,args);
    va_end(args);

    return record.ptr;
}

int munmap(void *ptr, size_t size)
{
	mcall_record record;
	INIT_RECORD(record);
	record.mtype = MUNMAP_CALL;
	record.size = size;
	record.ptr = ptr;

	do_mcall(&record,nonused_va);

	return record.rv;
}
