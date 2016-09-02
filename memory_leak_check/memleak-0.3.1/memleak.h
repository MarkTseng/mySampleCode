/***************************************************************************
   memleak.h: Redirects calls to the functions defined in memleak.c
   Copyright (c) 2005, 2008 Stanislav Maslovski.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
   *************************************************************************
   
   Include this file into the module you want to debug for memory leaks.
   
   IMPORTANT:
   ==========
   If you do not want to catch ramdom segfaults make sure that this file gets
   included in _every_ module which uses malloc(), free() and friends.
   Only if you know that the blocks allocated in a particular module will never
   be freed or reallocated in any other module you may include this file only in
   that module. For additional information see memleak.c
*/

#ifndef H_MEMLEAK_H
#define H_MEMLEAK_H

/* this makes sure that stdlib.h gets included before our macros */
#include <stdlib.h>

extern char *dbg_file_name;
extern unsigned long dbg_line_number;

#define CHK_FREED 1
#define CHK_ALLOC 2
#define CHK_ALL (CHK_FREED | CHK_ALLOC)

extern void *dbg_malloc(size_t size);
extern void *dbg_realloc(void *ptr, size_t size);
extern void *dbg_calloc(size_t num, size_t size);
extern void dbg_free(void *ptr);

extern void dbg_init(int history_length);
extern int dbg_check_addr(char *msg, void *ptr, int opt);
extern void dbg_mem_stat(void);
extern void dbg_zero_stat(void);
extern void dbg_abort(char *msg);
extern void dbg_heap_dump(char *keyword);
extern void dbg_history_dump(char *keyword);
extern void dbg_catch_sigsegv(void);

#define FILE_LINE dbg_file_name = __FILE__, dbg_line_number = __LINE__
#define malloc(s) (FILE_LINE, dbg_malloc(s))
#define realloc(p, s) (FILE_LINE, dbg_realloc(p, s))
#define calloc(n, s) (FILE_LINE, dbg_calloc(n, s))
#define free(p) (FILE_LINE, dbg_free(p))

#define dbg_init(n) (FILE_LINE, dbg_init(n))
#define dbg_check_addr(m, p, o) (FILE_LINE, dbg_check_addr(m, p, o))
#define dbg_mem_stat() (FILE_LINE, dbg_mem_stat())
#define dbg_zero_stat() (FILE_LINE, dbg_zero_stat())
#define dbg_abort(m) (FILE_LINE, dbg_abort(m))
#define dbg_heap_dump(k) (FILE_LINE, dbg_heap_dump(k))
#define dbg_history_dump(k) (FILE_LINE, dbg_history_dump(k))
#define dbg_catch_sigsegv() (FILE_LINE, dbg_catch_sigsegv())

#ifdef WITH_DBG_STRDUP
/* this makes sure that string.h gets included before our macros */
#include <string.h>

extern char *dbg_strdup(const char *s);
extern char *dbg_strndup(const char *s, size_t n);

#define strdup(s) (FILE_LINE, dbg_strdup(s))
#define strndup(s, n) (FILE_LINE, dbg_strndup(s, n))

#endif

#endif
