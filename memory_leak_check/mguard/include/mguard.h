#ifndef __MGUARD__
#define __MGUARD__


#undef BIT_FLAG
#define BIT_FLAG(x)     (1 << (x))
#undef BIT_SET
#define BIT_SET(v,f)        (v) |= (f)
#undef BIT_CLEAR
#define BIT_CLEAR(v,f)      (v) &= ~(f)
#undef BIT_IS_SET
#define BIT_IS_SET(v,f)     ((v) & (f))

#define MGUARD_VER "1.02"
#define MAX_CALL_STACK_LEN 10

#define OPT_DBG_TRACK_FORKED_CHILD      (1) //(1<<0)
#define OPT_DBG_NO_TRACK_DLOPEN         (2) //(1<<1)
#define OPT_DBG_NO_TRACK_MMAP           (4) //(1<<2)
#define OPT_DBG_NO_TRACK_MEMALIGN       (8) //(1<<2)
#define OPT_DBG_RELAY                   (16)//(1<<4)
#endif
