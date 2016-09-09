#ifndef MP_DIAG_H
#define MP_DIAG_H


/*
 * mpatrol
 * A library for controlling and tracing dynamic memory allocations.
 * Copyright (C) 1997-2002 Graeme S. Roy <graeme.roy@analog.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307, USA.
 */


/*
 * Diagnostics.  All mpatrol library diagnostics have to pass through this
 * interface, which shields the rest of the library from the details of where
 * the diagnostics actually go.
 */


/*
 * $Id: diag.h,v 1.47 2002/01/08 20:13:59 graeme Exp $
 */


#include "config.h"
#include "info.h"


#define FLG_EDIT     1 /* edit any source files associated with diagnostics */
#define FLG_LIST     2 /* list any source files associated with diagnostics */
#define FLG_HTML     4 /* format the log file in HTML rather than text */
#define FLG_HTMLNEXT 8 /* format the next log file in HTML rather than text */

#define FLG_COUNTS   1 /* display counts in leak table */
#define FLG_BOTTOM   2 /* display bottom entries in leak table */


/* The different types of warnings and errors reported by the mpatrol library.
 * This must be kept up to date with the definition of __mp_errortype in
 * mpatrol.h.
 */

typedef enum errortype
{
    ET_NONE,   /* no error has occurred */
    ET_ALLOVF, /* allocation %1 has a corrupted overflow buffer at %2 */
    ET_ALLZER, /* attempt to create an allocation of size 0 */
    ET_BADALN, /* alignment %1 is not a power of two */
    ET_FRDCOR, /* freed allocation %1 has memory corruption at %2 */
    ET_FRDOPN, /* attempt to perform operation on freed memory */
    ET_FRDOVF, /* freed allocation %1 has a corrupted overflow buffer at %2 */
    ET_FRECOR, /* free memory corruption at %1 */
    ET_FREMRK, /* attempt to free a marked allocation */
    ET_FRENUL, /* attempt to free a NULL pointer */
    ET_FREOPN, /* attempt to perform operation on free memory */
    ET_ILLMEM, /* illegal memory access at address %1 */
    ET_INCOMP, /* %1 was allocated with %2 */
    ET_MAXALN, /* alignment %1 is greater than the system page size */
    ET_MISMAT, /* %1 does not match allocation of %2 */
    ET_NOTALL, /* %1 has not been allocated */
    ET_NULOPN, /* attempt to perform operation on a NULL pointer */
    ET_OUTMEM, /* out of memory */
    ET_PRVFRD, /* %1 was freed with %2 */
    ET_RNGOVF, /* range [%1,%2] overflows [%3,%4] */
    ET_RNGOVL, /* range [%1,%2] overlaps [%3,%4] */
    ET_RSZNUL, /* attempt to resize a NULL pointer */
    ET_RSZZER, /* attempt to resize an allocation to size 0 */
    ET_STROVF, /* string %1 overflows [%2,%3] */
    ET_ZERALN, /* alignment 0 is invalid */
    ET_MAX
}
errortype;


/* The structure containing details about each error reported by the mpatrol
 * library.
 */

typedef struct errorinfo
{
    char *code;   /* error abbreviation code */
    char *string; /* error information string */
    char *format; /* error format */
}
errorinfo;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


MP_EXPORT errorinfo __mp_errordetails[ET_MAX + 1];
MP_EXPORT char *__mp_functionnames[AT_MAX + 1];
MP_EXPORT char *__mp_lognames[LT_MAX + 1];
MP_EXPORT unsigned long __mp_diagflags;


MP_EXPORT char *__mp_logfile(meminfo *, char *);
MP_EXPORT char *__mp_proffile(meminfo *, char *);
MP_EXPORT char *__mp_tracefile(meminfo *, char *);
MP_EXPORT int __mp_openlogfile(char *);
MP_EXPORT int __mp_closelogfile(void);
MP_EXPORT void __mp_diag(char *, ...);
MP_EXPORT void __mp_diagtag(char *);
MP_EXPORT void __mp_warn(errortype, alloctype, char *, unsigned long, char *,
                         ...);
MP_EXPORT void __mp_error(errortype, alloctype, char *, unsigned long, char *,
                          ...);
MP_EXPORT int __mp_editfile(char *, unsigned long, int);
MP_EXPORT int __mp_readalloc(char *, unsigned long, void *, size_t);
MP_EXPORT int __mp_writealloc(char *, unsigned long, void *, size_t);
MP_EXPORT long __mp_cmpalloc(char *, unsigned long, void *, size_t);
MP_EXPORT int __mp_remalloc(char *, unsigned long);
MP_EXPORT void __mp_printmemory(void *, size_t);
MP_EXPORT void __mp_printsize(size_t);
MP_EXPORT void __mp_printtype(infonode *);
MP_EXPORT void __mp_printloc(infonode *);
MP_EXPORT void __mp_printtypeinfo(infonode *, size_t);
MP_EXPORT void __mp_printsymbol(symhead *, void *);
MP_EXPORT void __mp_printsymbols(symhead *);
MP_EXPORT void __mp_printaddrs(symhead *, addrnode *);
MP_EXPORT void __mp_printstack(symhead *, stackinfo *);
MP_EXPORT void __mp_printalloc(symhead *, allocnode *);
MP_EXPORT void __mp_log(infohead *, loginfo *);
MP_EXPORT void __mp_printallocs(infohead *, int);
MP_EXPORT void __mp_printfreed(infohead *);
MP_EXPORT void __mp_printfree(infohead *);
MP_EXPORT void __mp_printleaktab(infohead *, size_t, int, unsigned char);
MP_EXPORT void __mp_printmap(infohead *);
MP_EXPORT void __mp_printversion(void);
MP_EXPORT void __mp_printsummary(infohead *);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MP_DIAG_H */
