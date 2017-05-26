#ifndef  SYMBOLS_H
#ifndef _GNU_SOURCE
#error You must define _GNU_SOURCE!
#endif
#define  SYMBOLS_H
#include <stdlib.h>

typedef enum {
    LOCAL_SYMBOL  = 1,
    GLOBAL_SYMBOL = 2,
    WEAK_SYMBOL   = 3,
} symbol_bind;

typedef enum {
    FUNC_SYMBOL   = 4,
    OBJECT_SYMBOL = 5,
    COMMON_SYMBOL = 6,
    THREAD_SYMBOL = 7,
} symbol_type;

int symbols(int (*callback)(const char *libpath, const char *libname, const char *objname,
                            const void *addr, const size_t size,
                            const symbol_bind binding, const symbol_type type,
                            void *custom),
            void *custom);

#endif /* SYMBOLS_H */

