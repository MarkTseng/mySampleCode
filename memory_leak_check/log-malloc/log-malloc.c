/*
 * Building: gcc -DWITH_PTHREADS -nostartfiles --shared -fPIC -g -ldl -o log-malloc.so log-malloc.c
 *        or gcc -nostartfiles --shared -fPIC -g -ldl -o log-malloc.so log-malloc.c
 * Usage: LD_PRELOAD=./log-malloc.so command args ...
 * Homepage: http://www.brokestream.com/wordpress/category/projects/log-malloc/
 * Version : 2007-06-01
 *
 * Ivan Tikhonov, http://www.brokestream.com, kefeer@netangels.ru
 *
 * Changes:
 *   2007-06-01: pthread safety patch for Dan Carpenter
 *
 * Notes:
 *   If you want redirect output to file run:
 *   LD_PRELOAD=./log-malloc.so command args ... 200>filename
 * 
 *   You could use addr2line tool from GNU binutils to convert 0x addresses
 *   into source code line numbers.
 *
 */


/* Copyright (C) 2007 Ivan Tikhonov

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Ivan Tikhonov, kefeer@brokestream.com

*/


#include <stdio.h>
#include <unistd.h>

#define __USE_GNU
#include <dlfcn.h>

static void *(*real_malloc) (size_t size) = 0;
static void (*real_free) (void *ptr) = 0;
static void *(*real_realloc) (void *ptr, size_t size) = 0;
static void *(*real_calloc) (size_t nmemb, size_t size) = 0;
static FILE *memlog = 0;

#ifdef WITH_PTHREADS
#include <pthread.h>
static pthread_mutex_t loglock;
#  define LOCK (pthread_mutex_lock(&loglock));
#  define UNLOCK (pthread_mutex_unlock(&loglock));
#else
#  define LOCK ;
#  define UNLOCK ;
#endif

static void init_me()
{
    if(memlog) return;
    memlog = fdopen(200, "w");
    if(!memlog) { memlog = stderr; }
#ifdef WITH_PTHREADS
    pthread_mutex_init(&loglock,0);
#endif
    real_malloc = dlsym(RTLD_NEXT, "malloc");
    real_calloc = dlsym(RTLD_NEXT, "calloc");
    real_free   = dlsym(RTLD_NEXT, "free");
    real_realloc = dlsym(RTLD_NEXT, "realloc");
}

#define GETSP() ({ register unsigned int *stack_ptr asm ("esp"); stack_ptr; })
#define GETBP() ({ register unsigned int *stack_ptr asm ("ebp"); stack_ptr; })

void
__attribute__ ((constructor))
_init (void)
{
  init_me();
}

static void print_stack() {
    unsigned int *bp = GETBP();
    while(bp) {
      Dl_info i;
      if(dladdr(*(bp+1), &i)) {
	if(i.dli_sname) {
	  fprintf(memlog, "  %s:%s:0x%08x", basename(i.dli_fname), i.dli_sname, *(bp+1));
	} else {
	  fprintf(memlog, "  %s::0x%08x", i.dli_fname, *(bp+1));
	}
      } else {
	fprintf(memlog, "  <>:0x%08x", *(bp+1));
      }
      bp = *bp;
    }
}


void *malloc(size_t size)
{
    void *p;
    if(!real_malloc) {
	if(!real_malloc) return NULL;
    }
    p = real_malloc(size);

    if(memlog) {
      LOCK;
      fprintf(memlog, "malloc %u 0x%08x ", size, p);
      print_stack();
      fprintf(memlog, "\n");
      UNLOCK;
    }
    return p;
}

void *calloc(size_t nmemb, size_t size)
{
    void *p;
    if(!real_calloc) {
	if(!real_calloc) return NULL;
    	real_calloc = dlsym(RTLD_NEXT, "calloc");
	return NULL;
    }
    p = real_calloc(nmemb, size);
    if(memlog) {
      LOCK;
      fprintf(memlog, "calloc %u %u 0x%08x ", nmemb, size, p);
      print_stack();
      fprintf(memlog, "\n");
      UNLOCK;
    }
    return p;
}

void free(void *ptr)
{
    if(!real_free) {
	if(!real_free) return;
    }
    real_free(ptr);
    if(memlog) {
      LOCK;
      fprintf(memlog, "free 0x%08x ", ptr);
      print_stack();
      fprintf(memlog, "\n");
      UNLOCK;
    }
}

void *realloc(void *ptr, size_t size)
{
    void *p;
    if(!real_realloc) {
	if(!real_realloc) return NULL;
    }
    p = real_realloc(ptr, size);
    if(memlog) {
      LOCK;
      fprintf(memlog, "realloc 0x%08x %u 0x%08x ", ptr, size, p);
      print_stack();
      fprintf(memlog, "\n");
      UNLOCK;
    }
    return p;
}

