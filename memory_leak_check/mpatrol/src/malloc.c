/*
 * mpatrol
 * A library for controlling and tracing dynamic memory allocations.
 * Copyright (C) 1997-2008 Graeme S. Roy <graemeroy@users.sourceforge.net>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


/*
 * Dynamic memory allocation functions and memory operation functions.
 * Overrides the standard library definitions of these functions if they
 * haven't already been overridden by the mpatrol.h header file.
 */


#include "inter.h"


#if MP_IDENT_SUPPORT
#ident "$Id$"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *malloc_id = "$Id$";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* Allocate an uninitialised memory block of a given size.
 */

#ifdef malloc
#undef malloc
#endif /* malloc */

MP_API
void *
malloc(size_t l)
{
    return __mp_alloc(l, 0, AT_MALLOC, NULL, NULL, 0, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void *
MP_ALTFUNCNAME(malloc)(size_t l)
{
    return __mp_alloc(l, 0, AT_MALLOC, NULL, NULL, 0, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Allocate a zero-initialised memory block to hold enough space for an
 * array of elements of a given size.
 */

#ifdef calloc
#undef calloc
#endif /* calloc */

MP_API
void *
calloc(size_t l, size_t n)
{
    return __mp_alloc(l * n, 0, AT_CALLOC, NULL, NULL, 0, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void *
MP_ALTFUNCNAME(calloc)(size_t l, size_t n)
{
    return __mp_alloc(l * n, 0, AT_CALLOC, NULL, NULL, 0, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Allocate an uninitialised memory block of a given size and alignment.
 */

#ifdef memalign
#undef memalign
#endif /* memalign */

MP_API
void *
memalign(size_t a, size_t l)
{
    return __mp_alloc(l, a, AT_MEMALIGN, NULL, NULL, 0, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void *
MP_ALTFUNCNAME(memalign)(size_t a, size_t l)
{
    return __mp_alloc(l, a, AT_MEMALIGN, NULL, NULL, 0, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Allocate an uninitialised memory block of a given size and aligned to
 * the system page size.
 */

#ifdef valloc
#undef valloc
#endif /* valloc */

MP_API
void *
valloc(size_t l)
{
    return __mp_alloc(l, 0, AT_VALLOC, NULL, NULL, 0, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void *
MP_ALTFUNCNAME(valloc)(size_t l)
{
    return __mp_alloc(l, 0, AT_VALLOC, NULL, NULL, 0, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Allocate an uninitialised number of pages from the system.
 */

#ifdef pvalloc
#undef pvalloc
#endif /* pvalloc */

MP_API
void *
pvalloc(size_t l)
{
    return __mp_alloc(l, 0, AT_PVALLOC, NULL, NULL, 0, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void *
MP_ALTFUNCNAME(pvalloc)(size_t l)
{
    return __mp_alloc(l, 0, AT_PVALLOC, NULL, NULL, 0, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Allocate a temporary uninitialised memory block of a given size.
 */

#ifdef alloca
#undef alloca
#endif /* alloca */

MP_API
void *
alloca(size_t l)
{
#if SYSTEM == SYSTEM_INTERIX
    /* alloca() has a non-standard calling convention on Interix.
     */
    register int eax asm("%eax");
    l = eax;
#endif /* SYSTEM */
    return __mp_alloc(l, 0, AT_ALLOCA, NULL, NULL, 0, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void *
MP_ALTFUNCNAME(alloca)(size_t l)
{
#if SYSTEM == SYSTEM_INTERIX
    /* _alloca() has a non-standard calling convention on Interix.
     */
    register int eax asm("%eax");
    l = eax;
#endif /* SYSTEM */
    return __mp_alloc(l, 0, AT_ALLOCA, NULL, NULL, 0, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Duplicate an existing string using memory from the heap.
 */

#ifdef strdup
#undef strdup
#endif /* strdup */

MP_API
char *
strdup(MP_CONST char *p)
{
    return __mp_strdup((char *) p, 0, AT_STRDUP, NULL, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
char *
MP_ALTFUNCNAME(strdup)(MP_CONST char *p)
{
    return __mp_strdup((char *) p, 0, AT_STRDUP, NULL, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Duplicate an existing string using memory from the heap, but set a limit
 * on the size of the memory allocated for the new string.
 */

#ifdef strndup
#undef strndup
#endif /* strndup */

MP_API
char *
strndup(MP_CONST char *p, size_t l)
{
    return __mp_strdup((char *) p, l, AT_STRNDUP, NULL, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
char *
MP_ALTFUNCNAME(strndup)(MP_CONST char *p, size_t l)
{
    return __mp_strdup((char *) p, l, AT_STRNDUP, NULL, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Duplicate an existing string using memory from the heap.
 */

#ifdef strsave
#undef strsave
#endif /* strsave */

MP_API
char *
strsave(MP_CONST char *p)
{
    return __mp_strdup((char *) p, 0, AT_STRSAVE, NULL, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
char *
MP_ALTFUNCNAME(strsave)(MP_CONST char *p)
{
    return __mp_strdup((char *) p, 0, AT_STRSAVE, NULL, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Duplicate an existing string using memory from the heap, but set a limit
 * on the size of the memory allocated for the new string.
 */

#ifdef strnsave
#undef strnsave
#endif /* strnsave */

MP_API
#if SYSTEM == SYSTEM_DGUX
char *
strnsave(MP_CONST char *p, int l)
#else /* SYSTEM */
char *
strnsave(MP_CONST char *p, size_t l)
#endif /* SYSTEM */
{
    return __mp_strdup((char *) p, l, AT_STRNSAVE, NULL, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
#if SYSTEM == SYSTEM_DGUX
char *
MP_ALTFUNCNAME(strnsave)(MP_CONST char *p, int l)
#else /* SYSTEM */
char *
MP_ALTFUNCNAME(strnsave)(MP_CONST char *p, size_t l)
#endif /* SYSTEM */
{
    return __mp_strdup((char *) p, l, AT_STRNSAVE, NULL, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Duplicate an existing string using temporary memory from the heap.
 */

#ifdef strdupa
#undef strdupa
#endif /* strdupa */

MP_API
char *
strdupa(MP_CONST char *p)
{
    return __mp_strdup((char *) p, 0, AT_STRDUPA, NULL, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
char *
MP_ALTFUNCNAME(strdupa)(MP_CONST char *p)
{
    return __mp_strdup((char *) p, 0, AT_STRDUPA, NULL, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Duplicate an existing string using temporary memory from the heap, but set a
 * limit on the size of the memory allocated for the new string.
 */

#ifdef strndupa
#undef strndupa
#endif /* strndupa */

MP_API
char *
strndupa(MP_CONST char *p, size_t l)
{
    return __mp_strdup((char *) p, l, AT_STRNDUPA, NULL, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
char *
MP_ALTFUNCNAME(strndupa)(MP_CONST char *p, size_t l)
{
    return __mp_strdup((char *) p, l, AT_STRNDUPA, NULL, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Resize an existing block of memory.
 */

#ifdef realloc
#undef realloc
#endif /* realloc */

MP_API
void *
realloc(void *p, size_t l)
{
    return __mp_realloc(p, l, 0, AT_REALLOC, NULL, NULL, 0, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void *
MP_ALTFUNCNAME(realloc)(void *p, size_t l)
{
    return __mp_realloc(p, l, 0, AT_REALLOC, NULL, NULL, 0, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Resize an existing block of memory but free the block if no memory is
 * available to resize it.
 */

#ifdef reallocf
#undef reallocf
#endif /* reallocf */

MP_API
void *
reallocf(void *p, size_t l)
{
    return __mp_realloc(p, l, 0, AT_REALLOCF, NULL, NULL, 0, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void *
MP_ALTFUNCNAME(reallocf)(void *p, size_t l)
{
    return __mp_realloc(p, l, 0, AT_REALLOCF, NULL, NULL, 0, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Resize an existing block of memory, usually a block allocated by calloc().
 */

#ifdef recalloc
#undef recalloc
#endif /* recalloc */

MP_API
void *
recalloc(void *p, size_t l, size_t n)
{
    return __mp_realloc(p, l * n, 0, AT_RECALLOC, NULL, NULL, 0, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void *
MP_ALTFUNCNAME(recalloc)(void *p, size_t l, size_t n)
{
    return __mp_realloc(p, l * n, 0, AT_RECALLOC, NULL, NULL, 0, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Resize an existing block of memory, but never relocate it.
 */

#ifdef expand
#undef expand
#endif /* expand */

MP_API
void *
expand(void *p, size_t l)
{
    return __mp_realloc(p, l, 0, AT_EXPAND, NULL, NULL, 0, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void *
MP_ALTFUNCNAME(expand)(void *p, size_t l)
{
    return __mp_realloc(p, l, 0, AT_EXPAND, NULL, NULL, 0, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Free an existing block of memory.
 */

#ifdef free
#undef free
#endif /* free */

MP_API
void
free(void *p)
{
    __mp_free(p, AT_FREE, NULL, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void
MP_ALTFUNCNAME(free)(void *p)
{
    __mp_free(p, AT_FREE, NULL, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Free an existing block of memory, usually a block allocated by calloc().
 */

#ifdef cfree
#undef cfree
#endif /* cfree */

MP_API
void
cfree(void *p, size_t l, size_t n)
{
    __mp_free(p, AT_CFREE, NULL, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void
MP_ALTFUNCNAME(cfree)(void *p, size_t l, size_t n)
{
    __mp_free(p, AT_CFREE, NULL, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Explicitly free an existing block of temporary memory.
 */

#ifdef dealloca
#undef dealloca
#endif /* dealloca */

MP_API
void
dealloca(void *p)
{
    __mp_free(p, AT_DEALLOCA, NULL, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void
MP_ALTFUNCNAME(dealloca)(void *p)
{
    __mp_free(p, AT_DEALLOCA, NULL, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Allocate an uninitialised memory block of a given size and abort on failure.
 */

#ifdef xmalloc
#undef xmalloc
#endif /* xmalloc */

MP_API
void *
xmalloc(size_t l)
{
    return __mp_alloc(l, 0, AT_XMALLOC, NULL, NULL, 0, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void *
MP_ALTFUNCNAME(xmalloc)(size_t l)
{
    return __mp_alloc(l, 0, AT_XMALLOC, NULL, NULL, 0, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Allocate a zero-initialised memory block to hold enough space for an
 * array of elements of a given size and abort on failure.
 */

#ifdef xcalloc
#undef xcalloc
#endif /* xcalloc */

MP_API
void *
xcalloc(size_t l, size_t n)
{
    return __mp_alloc(l * n, 0, AT_XCALLOC, NULL, NULL, 0, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void *
MP_ALTFUNCNAME(xcalloc)(size_t l, size_t n)
{
    return __mp_alloc(l * n, 0, AT_XCALLOC, NULL, NULL, 0, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Duplicate an existing string using memory from the heap and abort on failure.
 */

#ifdef xstrdup
#undef xstrdup
#endif /* xstrdup */

MP_API
char *
xstrdup(MP_CONST char *p)
{
    return __mp_strdup((char *) p, 0, AT_XSTRDUP, NULL, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
char *
MP_ALTFUNCNAME(xstrdup)(MP_CONST char *p)
{
    return __mp_strdup((char *) p, 0, AT_XSTRDUP, NULL, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Resize an existing block of memory and abort on failure.
 */

#ifdef xrealloc
#undef xrealloc
#endif /* xrealloc */

MP_API
void *
xrealloc(void *p, size_t l)
{
    return __mp_realloc(p, l, 0, AT_XREALLOC, NULL, NULL, 0, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void *
MP_ALTFUNCNAME(xrealloc)(void *p, size_t l)
{
    return __mp_realloc(p, l, 0, AT_XREALLOC, NULL, NULL, 0, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Free an existing block of memory, usually a block allocated by xmalloc().
 */

#ifdef xfree
#undef xfree
#endif /* xfree */

MP_API
void
xfree(void *p)
{
    __mp_free(p, AT_XFREE, NULL, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void
MP_ALTFUNCNAME(xfree)(void *p)
{
    __mp_free(p, AT_XFREE, NULL, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Set a block of memory to a specific byte.
 */

#ifdef memset
#undef memset
#endif /* memset */

MP_API
void *
memset(void *p, int c, size_t l)
{
    return __mp_setmem(p, l, (unsigned char) c, AT_MEMSET, NULL, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void *
MP_ALTFUNCNAME(memset)(void *p, int c, size_t l)
{
    return __mp_setmem(p, l, (unsigned char) c, AT_MEMSET, NULL, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Set a block of memory to the zero byte.
 */

#ifdef bzero
#undef bzero
#endif /* bzero */

MP_API
void
bzero(void *p, size_t l)
{
    __mp_setmem(p, l, 0, AT_BZERO, NULL, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void
MP_ALTFUNCNAME(bzero)(void *p, size_t l)
{
    __mp_setmem(p, l, 0, AT_BZERO, NULL, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Copy a non-overlapping block of memory from one address to another,
 * stopping when a specific character is found.
 */

#ifdef memccpy
#undef memccpy
#endif /* memccpy */

MP_API
void *
memccpy(void *q, MP_CONST void *p, int c, size_t l)
{
    return __mp_copymem((void *) p, q, l, (unsigned char) c, AT_MEMCCPY, NULL,
                        NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void *
MP_ALTFUNCNAME(memccpy)(void *q, MP_CONST void *p, int c, size_t l)
{
    return __mp_copymem((void *) p, q, l, (unsigned char) c, AT_MEMCCPY, NULL,
                        NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Copy a non-overlapping block of memory from one address to another.
 */

#ifdef memcpy
#undef memcpy
#endif /* memcpy */

MP_API
void *
memcpy(void *q, MP_CONST void *p, size_t l)
{
    return __mp_copymem((void *) p, q, l, 0, AT_MEMCPY, NULL, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void *
MP_ALTFUNCNAME(memcpy)(void *q, MP_CONST void *p, size_t l)
{
    return __mp_copymem((void *) p, q, l, 0, AT_MEMCPY, NULL, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Copy a possibly-overlapping block of memory from one address to another.
 */

#ifdef memmove
#undef memmove
#endif /* memmove */

MP_API
void *
memmove(void *q, MP_CONST void *p, size_t l)
{
    return __mp_copymem((void *) p, q, l, 0, AT_MEMMOVE, NULL, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void *
MP_ALTFUNCNAME(memmove)(void *q, MP_CONST void *p, size_t l)
{
    return __mp_copymem((void *) p, q, l, 0, AT_MEMMOVE, NULL, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Copy a possibly-overlapping block of memory from one address to another.
 */

#ifdef bcopy
#undef bcopy
#endif /* bcopy */

MP_API
void
bcopy(MP_CONST void *p, void *q, size_t l)
{
    __mp_copymem((void *) p, q, l, 0, AT_BCOPY, NULL, NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void
MP_ALTFUNCNAME(bcopy)(MP_CONST void *p, void *q, size_t l)
{
    __mp_copymem((void *) p, q, l, 0, AT_BCOPY, NULL, NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Look for the first occurrence of a character in a block of memory.
 */

#ifdef memchr
#undef memchr
#endif /* memchr */

MP_API
void *
memchr(MP_CONST void *p, int c, size_t l)
{
    return __mp_locatemem((void *) p, l, NULL, (size_t) c, AT_MEMCHR, NULL,
                          NULL, 0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void *
MP_ALTFUNCNAME(memchr)(MP_CONST void *p, int c, size_t l)
{
    return __mp_locatemem((void *) p, l, NULL, (size_t) c, AT_MEMCHR, NULL,
                          NULL, 0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Attempt to locate the position of one block of memory in another block.
 */

#ifdef memmem
#undef memmem
#endif /* memmem */

MP_API
void *
memmem(MP_CONST void *p, size_t l, MP_CONST void *q, size_t m)
{
    return __mp_locatemem((void *) p, l, (void *) q, m, AT_MEMMEM, NULL, NULL,
                          0, 1);
}


#if MP_ALTFUNCNAMES
MP_API
void *
MP_ALTFUNCNAME(memmem)(MP_CONST void *p, size_t l, MP_CONST void *q, size_t m)
{
    return __mp_locatemem((void *) p, l, (void *) q, m, AT_MEMMEM, NULL, NULL,
                          0, 1);
}
#endif /* MP_ALTFUNCNAMES */


/* Compare two blocks of memory.
 */

#ifdef memcmp
#undef memcmp
#endif /* memcmp */

MP_API
int
memcmp(MP_CONST void *p, MP_CONST void *q, size_t l)
{
    return __mp_comparemem((void *) p, (void *) q, l, AT_MEMCMP, NULL, NULL, 0,
                           1);
}


#if MP_ALTFUNCNAMES
MP_API
int
MP_ALTFUNCNAME(memcmp)(MP_CONST void *p, MP_CONST void *q, size_t l)
{
    return __mp_comparemem((void *) p, (void *) q, l, AT_MEMCMP, NULL, NULL, 0,
                           1);
}
#endif /* MP_ALTFUNCNAMES */


/* Compare two blocks of memory.
 */

#ifdef bcmp
#undef bcmp
#endif /* bcmp */

MP_API
int
bcmp(MP_CONST void *p, MP_CONST void *q, size_t l)
{
    return __mp_comparemem((void *) p, (void *) q, l, AT_BCMP, NULL, NULL, 0,
                           1);
}


#if MP_ALTFUNCNAMES
MP_API
int
MP_ALTFUNCNAME(bcmp)(MP_CONST void *p, MP_CONST void *q, size_t l)
{
    return __mp_comparemem((void *) p, (void *) q, l, AT_BCMP, NULL, NULL, 0,
                           1);
}
#endif /* MP_ALTFUNCNAMES */


#ifdef __cplusplus
}
#endif /* __cplusplus */
