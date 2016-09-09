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
 * Symbol tables.  The implementation of reading symbols from executable files
 * may also interface with access libraries which support other file formats.
 * This module may also have to liaise with the dynamic linker (or equivalent)
 * in order to obtain symbols from shared objects.  A very useful book on the
 * various different object file and library formats that exist is Linkers &
 * Loaders, First Edition by John R. Levine (Morgan Kaufmann Publishers, 2000,
 * ISBN 1-558-60496-0).
 */


#include "symbol.h"
#include "diag.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if FORMAT == FORMAT_AOUT || FORMAT == FORMAT_COFF || \
    FORMAT == FORMAT_XCOFF || FORMAT == FORMAT_ELF32 || \
    FORMAT == FORMAT_ELF64 || FORMAT == FORMAT_BFD
#include <fcntl.h>
#include <unistd.h>
#if FORMAT == FORMAT_AOUT
#include <a.out.h>
#elif FORMAT == FORMAT_COFF || FORMAT == FORMAT_XCOFF
#include <a.out.h>
#if SYSTEM == SYSTEM_LYNXOS
#include <coff.h>
#ifndef ISCOFF
#define ISCOFF(m) ((m) == COFFMAGICNUM)
#endif /* ISCOFF */
#ifndef ISFCN
#define ISFCN(t) (((t) & 0x30) == (DT_FCN << 4))
#endif /* ISFCN */
#ifndef n_name
#define n_name _n._n_name
#endif /* n_name */
#else /* SYSTEM */
#if SYSTEM == SYSTEM_AIX
#ifdef FREAD
#undef FREAD
#endif /* FREAD */
#ifdef FWRITE
#undef FWRITE
#endif /* FWRITE */
#ifndef ISCOFF
#define ISCOFF(m) (((m) == U800TOCMAGIC) || ((m) == U802TOCMAGIC))
#endif /* ISCOFF */
#endif /* SYSTEM */
#include <ldfcn.h>
#endif /* SYSTEM */
#elif FORMAT == FORMAT_ELF32 || FORMAT == FORMAT_ELF64
#include <libelf.h>
#elif FORMAT == FORMAT_BFD
#include <bfd.h>
#endif /* FORMAT */
#endif /* FORMAT */
#if DYNLINK == DYNLINK_AIX
/* The shared libraries that an AIX executable has loaded can be obtained via
 * the loadquery() function.
 */
#include <sys/types.h>
#include <sys/ldr.h>
#elif DYNLINK == DYNLINK_BSD
/* The BSD a.out dynamic linker is based on the original SunOS implementation
 * and is the precursor to the SVR4 ELF dynamic linker.  Most BSD systems now
 * use ELF so this is for SunOS and the BSD systems that still use a.out.
 */
#include <sys/types.h>
#include <link.h>
#elif DYNLINK == DYNLINK_HPUX
/* The HP/UX dynamic linker support routines are available for use even by
 * statically linked programs.
 */
#include <dl.h>
#elif DYNLINK == DYNLINK_IRIX
/* IRIX doesn't have a conventional SVR4 dynamic linker and so does not have the
 * same interface for accessing information about any required shared objects at
 * run-time.
 */
#include <obj.h>
#include <obj_list.h>
#elif DYNLINK == DYNLINK_OSF
/* The shared libraries that an OSF executable has loaded can be obtained via
 * the ldr_inq_module() and ldr_inq_region() functions.
 */
#include <loader.h>
#elif DYNLINK == DYNLINK_SVR4
/* Despite the fact that Linux is now ELF-based, libelf seems to be missing from
 * many recent distributions and so we must use the GNU BFD library to read the
 * symbols from the object files and libraries.  However, we still need the ELF
 * definitions for reading the internal structures of the dynamic linker.
 */
#include <elf.h>
#elif DYNLINK == DYNLINK_WINDOWS
/* We use the imagehlp library on Windows platforms to obtain information about
 * the symbols loaded from third-party and system DLLs.  We can also use it to
 * obtain information about any symbols contained in the executable file and
 * program database if COFF or CodeView debugging information is being used.
 */
#include <windows.h>
#include <imagehlp.h>
#endif /* DYNLINK */


#if MP_IDENT_SUPPORT
#ident "$Id: symbol.c,v 1.62 2002/01/08 20:13:59 graeme Exp $"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *symbol_id = "$Id: symbol.c,v 1.62 2002/01/08 20:13:59 graeme Exp $";
#endif /* MP_IDENT_SUPPORT */


#if DYNLINK == DYNLINK_SVR4
/* These definitions are not always defined in ELF header files on all
 * systems so we define them here as they are documented in most
 * System V ABI documents.
 */

#ifndef DT_NULL
#define DT_NULL  0
#define DT_DEBUG 21

#if ENVIRON == ENVIRON_64
typedef struct Elf64_Dyn
{
    Elf64_Xword d_tag;     /* tag indicating type of data stored */
    union
    {
        Elf64_Xword d_val; /* data is a value */
        Elf64_Addr d_ptr;  /* data is a pointer */
    }
    d_un;
}
Elf64_Dyn;
#else /* ENVIRON */
typedef struct Elf32_Dyn
{
    Elf32_Sword d_tag;    /* tag indicating type of data stored */
    union
    {
        Elf32_Word d_val; /* data is a value */
        Elf32_Addr d_ptr; /* data is a pointer */
    }
    d_un;
}
Elf32_Dyn;
#endif /* ENVIRON */
#endif /* DT_NULL */
#endif /* DYNLINK */


#if DYNLINK == DYNLINK_IRIX
/* This structure represents an N32 ABI shared object as opposed to an O32 ABI
 * shared object, and is defined on IRIX 6.0 and above platforms as
 * Elf32_Obj_Info.  In order for us to compile on earlier IRIX platforms we
 * define this structure here, but under a different name so as to avoid a
 * naming clash on IRIX 6.0 and later.
 */

typedef struct objectinfo
{
    Elf32_Word magic;  /* N32 ABI shared object */
    Elf32_Word size;   /* size of this structure */
    Elf32_Addr next;   /* next shared object */
    Elf32_Addr prev;   /* previous shared object */
    Elf32_Addr ehdr;   /* address of object file header */
    Elf32_Addr ohdr;   /* original address of object file header */
    Elf32_Addr name;   /* filename of shared object */
    Elf32_Word length; /* length of filename of shared object */
}
objectinfo;
#elif DYNLINK == DYNLINK_SVR4
/* This is a structure that is internal to the dynamic linker on ELF systems,
 * and so it is not always guaranteed to be the same.  We try to rely on this
 * definition here for portability's sake as it is not publicly declared in
 * all ELF header files.
 */

typedef struct dynamiclink
{
    size_t base;              /* virtual address of shared object */
    char *name;               /* filename of shared object */
#if ENVIRON == ENVIRON_64
    Elf64_Dyn *dyn;           /* dynamic linking information */
#else /* ENVIRON */
    Elf32_Dyn *dyn;           /* dynamic linking information */
#endif /* ENVIRON */
    struct dynamiclink *next; /* pointer to next shared object */
}
dynamiclink;
#elif DYNLINK == DYNLINK_WINDOWS
/* This structure is used to pass information to the callback function
 * called by SymEnumerateSymbols().
 */

typedef struct syminfo
{
    symhead *syms; /* pointer to symbol table */
    char *file;    /* filename of module */
}
syminfo;


/* This structure is used to pass information to the callback function
 * called by SymEnumerateModules().
 */

typedef struct modinfo
{
    symhead *syms; /* pointer to symbol table */
    size_t index;  /* index of module */
    char libs;     /* only read DLLs */
}
modinfo;
#endif /* DYNLINK */


#if FORMAT == FORMAT_BFD
/* This structure is used to maintain a list of access library handles for
 * the purposes of mapping return addresses to line numbers.
 */

typedef struct objectfile
{
    struct objectfile *next;  /* pointer to next object file */
    bfd *file;                /* access library handle */
    asymbol **symbols;        /* pointer to symbols */
    size_t base;              /* virtual address of object file */
}
objectfile;


/* This structure is used when searching for source locations corresponding
 * to virtual addresses by looking at debugging information embedded in an
 * object file.
 */

typedef struct sourcepos
{
    bfd_vma addr;       /* virtual address to search for */
    asymbol **symbols;  /* pointer to symbols */
    size_t base;        /* virtual address of object file */
    char *func;         /* function name */
    char *file;         /* file name */
    unsigned int line;  /* line number */
    char found;         /* found flag */
}
sourcepos;
#endif /* FORMAT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#if DYNLINK == DYNLINK_BSD
/* The declaration of the _DYNAMIC symbol, which allows us direct access to the
 * dynamic linker's internal data structures.  This is set up in crt0 and will
 * be NULL in an object that does not need dynamic linking.
 */
#if SYSTEM == SYSTEM_SUNOS
extern struct link_dynamic _DYNAMIC;
#else /* SYSTEM */
extern struct _dynamic _DYNAMIC;
#endif /* SYSTEM */
#elif DYNLINK == DYNLINK_IRIX
/* The __rld_obj_head symbol is always defined in IRIX and points to the first
 * entry in a list of shared object files that are required by the program.  For
 * statically linked programs this will either be NULL or will only contain the
 * entry for the program itself.
 */
extern struct obj_list *__rld_obj_head;
#elif DYNLINK == DYNLINK_SVR4
/* The declaration of the _DYNAMIC symbol, which allows us direct access to the
 * dynamic linker's internal data structures.  We make it have weak visibility
 * so that it is always defined, even in the statically linked case.  It is
 * declared as a function because some compilers only allow weak function
 * symbols.  Another alternative would be to declare it as a common symbol, but
 * that wouldn't work under C++ and many libc shared objects have it defined as
 * a text symbol.
 */

#pragma weak _DYNAMIC
void _DYNAMIC(void);
#endif /* DYNLINK */


/* Initialise the fields of a symhead so that the symbol table becomes empty.
 */

MP_GLOBAL
void
__mp_newsymbols(symhead *y, heaphead *h, void *i)
{
    struct { char x; symnode y; } z;
    long n;

    y->heap = h;
    /* Determine the minimum alignment for a symbol node on this system
     * and force the alignment to be a power of two.  This information
     * is used when initialising the slot table.
     */
    n = (char *) &z.y - &z.x;
    __mp_newstrtab(&y->strings, h);
    __mp_newslots(&y->table, sizeof(symnode), __mp_poweroftwo(n));
    __mp_newtree(&y->itree);
    __mp_newtree(&y->dtree);
    y->size = 0;
    y->hhead = y->htail = NULL;
    y->inits = i;
    y->prot = MA_NOACCESS;
    y->protrecur = 0;
    y->lineinfo = 0;
}


/* Close all access library handles.
 */

MP_GLOBAL
void
__mp_closesymbols(symhead *y)
{
#if FORMAT == FORMAT_BFD
    objectfile *n, *p;
#endif /* FORMAT */

#if FORMAT == FORMAT_BFD
    for (n = (objectfile *) y->hhead; n != NULL; n = p)
    {
        p = n->next;
        bfd_close(n->file);
        free(n->symbols);
        free(n);
    }
#endif /* FORMAT */
    y->hhead = y->htail = NULL;
}


/* Forget all data currently in the symbol table.
 */

MP_GLOBAL
void
__mp_deletesymbols(symhead *y)
{
    /* We don't need to explicitly free any memory as this is dealt with
     * at a lower level by the heap manager.
     */
    y->heap = NULL;
    __mp_deletestrtab(&y->strings);
    y->table.free = NULL;
    y->table.size = 0;
    __mp_newtree(&y->itree);
    __mp_newtree(&y->dtree);
    y->size = 0;
    y->inits = NULL;
    y->prot = MA_NOACCESS;
    y->protrecur = 0;
}


/* Allocate a new symbol node.
 */

static
symnode *
getsymnode(symhead *y)
{
    symnode *n;
    heapnode *p;

    /* If we have no more symbol node slots left then we must allocate
     * some more memory for them.  An extra MP_ALLOCFACTOR pages of memory
     * should suffice.
     */
    if ((n = (symnode *) __mp_getslot(&y->table)) == NULL)
    {
        if ((p = __mp_heapalloc(y->heap, y->heap->memory.page * MP_ALLOCFACTOR,
              y->table.entalign, 1)) == NULL)
            return NULL;
        __mp_initslots(&y->table, p->block, p->size);
        n = (symnode *) __mp_getslot(&y->table);
        __mp_treeinsert(&y->itree, &n->index.node, (unsigned long) p->block);
        n->index.block = p->block;
        n->index.size = p->size;
        y->size += p->size;
        n = (symnode *) __mp_getslot(&y->table);
    }
    return n;
}


#if FORMAT == FORMAT_AOUT || FORMAT == FORMAT_COFF || \
    FORMAT == FORMAT_XCOFF || FORMAT == FORMAT_ELF32 || \
    FORMAT == FORMAT_ELF64 || FORMAT == FORMAT_BFD
/* Decide whether to store a symbol by looking at its name.
 */

static
int
addsymname(char **s)
{
    /* We don't bother storing a symbol which has no name or whose name
     * contains a '$', '@' or a '.', although GNU C++ destructors begin
     * with `_._'.  However, in XCOFF the symbol name is likely to be the
     * name of a CSECT beginning with a '.' and not the original name of
     * the function, so we skip the first character.  In addition, the
     * HP/UX $START$ symbol contains dollar characters but we don't want
     * to bother allowing any other symbols containing dollars.
     */
    if ((*s != NULL) && (**s != '\0') &&
#if ((SYSTEM == SYSTEM_AIX || SYSTEM == SYSTEM_LYNXOS) && \
     (ARCH == ARCH_POWER || ARCH == ARCH_POWERPC)) || FORMAT == FORMAT_XCOFF
        (*(*s)++ == '.') && (strcmp(*s, "text") != 0) &&
#endif /* SYSTEM && ARCH && FORMAT */
        ((strncmp(*s, "_._", 3) == 0) ||
#if SYSTEM == SYSTEM_HPUX
         (strcmp(*s, "$START$") == 0) ||
#endif /* SYSTEM */
         !strpbrk(*s, "$@.")))
        return 1;
    return 0;
}
#endif /* FORMAT */


#if FORMAT == FORMAT_AOUT
/* Allocate a new symbol node for a given a.out symbol.
 */

static
int
addsymbol(symhead *y, struct nlist *p, char *f, char *s, size_t b)
{
    symnode *n;
    char *r;
    size_t a;

    a = b + p->n_value;
    /* We don't allocate a symbol node for symbols which have a virtual
     * address of zero.
     */
    if (addsymname(&s) && (a > 0))
    {
        if ((n = getsymnode(y)) == NULL)
            return 0;
        if ((r = __mp_addstring(&y->strings, s)) == NULL)
        {
            __mp_freeslot(&y->table, n);
            return 0;
        }
        __mp_treeinsert(&y->dtree, &n->data.node, a);
        n->data.file = f;
        n->data.name = r;
        n->data.addr = (void *) a;
        /* The a.out file format doesn't record symbol sizes, so we have
         * to rely on the assumption that a function symbol ends at the
         * beginning of the next function symbol.  This will be calculated
         * in __mp_fixsymbols().
         */
        n->data.size = 0;
        n->data.index = 0;
        n->data.offset = 0;
        /* The linkage information is required for when we look up a symbol.
         */
        n->data.flags = p->n_type;
#if MP_INITFUNC_SUPPORT
        /* Check to see if this function should be called when the mpatrol
         * library is initialised or terminated.
         */
        if ((strncmp(r, "__mp_init_", 10) == 0) && (r[10] != '\0'))
            __mp_atinit((infohead *) y->inits, (void (*)(void)) a);
        else if ((strncmp(r, "__mp_fini_", 10) == 0) && (r[10] != '\0'))
            __mp_atfini((infohead *) y->inits, (void (*)(void)) a);
#endif /* MP_INITFUNC_SUPPORT */
    }
    return 1;
}
#elif FORMAT == FORMAT_COFF || FORMAT == FORMAT_XCOFF
/* Allocate a new symbol node for a given COFF or XCOFF symbol.
 */

static
int
addsymbol(symhead *y, SYMENT *p, char *f, char *s, size_t b)
{
    AUXENT *e;
    symnode *n;
    char *r;
    size_t a;

    a = b + p->n_value;
    /* We don't allocate a symbol node for symbols which have a virtual
     * address of zero and we only remember symbols that are declared
     * statically or externally visible.
     */
    if (addsymname(&s) && (a > 0) &&
#if FORMAT == FORMAT_XCOFF
        (ISFCN(p->n_type) || (p->n_sclass == C_EXT)))
#else /* FORMAT */
        ((p->n_sclass == C_STAT) || (p->n_sclass == C_EXT)))
#endif /* FORMAT */
    {
        if ((n = getsymnode(y)) == NULL)
            return 0;
        if ((r = __mp_addstring(&y->strings, s)) == NULL)
        {
            __mp_freeslot(&y->table, n);
            return 0;
        }
        __mp_treeinsert(&y->dtree, &n->data.node, a);
        n->data.file = f;
        n->data.name = r;
        n->data.addr = (void *) a;
        /* Attempt to figure out the size of the symbol.
         */
        if (p->n_numaux > 0)
        {
            e = (AUXENT *) ((char *) p + SYMESZ);
            if (ISFCN(p->n_type))
                n->data.size = e->x_sym.x_misc.x_fsize;
            else
                n->data.size = e->x_sym.x_misc.x_lnsz.x_size;
        }
        else
            n->data.size = 0;
        n->data.index = 0;
        n->data.offset = 0;
        /* The linkage information is required for when we look up a symbol.
         */
        n->data.flags = p->n_sclass;
#if MP_INITFUNC_SUPPORT
        /* Check to see if this function should be called when the mpatrol
         * library is initialised or terminated.
         */
        if ((strncmp(r, "__mp_init_", 10) == 0) && (r[10] != '\0'))
            __mp_atinit((infohead *) y->inits, (void (*)(void)) a);
        else if ((strncmp(r, "__mp_fini_", 10) == 0) && (r[10] != '\0'))
            __mp_atfini((infohead *) y->inits, (void (*)(void)) a);
#endif /* MP_INITFUNC_SUPPORT */
    }
    return 1;
}
#elif FORMAT == FORMAT_ELF32
/* Allocate a new symbol node for a given ELF32 symbol.
 */

static
int
addsymbol(symhead *y, Elf32_Sym *p, char *f, char *s, size_t b)
{
    symnode *n;
    char *r;
    size_t a;
    unsigned char t;

    a = b + p->st_value;
    /* We don't allocate a symbol node for symbols which have a virtual
     * address of zero or are of object type.
     */
    if (addsymname(&s) && (a > 0) &&
        (((t = ELF32_ST_TYPE(p->st_info)) == STT_NOTYPE) || (t == STT_FUNC)))
    {
        if ((n = getsymnode(y)) == NULL)
            return 0;
        if ((r = __mp_addstring(&y->strings, s)) == NULL)
        {
            __mp_freeslot(&y->table, n);
            return 0;
        }
        __mp_treeinsert(&y->dtree, &n->data.node, a);
        n->data.file = f;
        n->data.name = r;
        n->data.addr = (void *) a;
        n->data.size = p->st_size;
        n->data.index = 0;
        n->data.offset = 0;
        /* The linkage information is required for when we look up a symbol.
         */
        n->data.flags = ELF32_ST_BIND(p->st_info);
#if MP_INITFUNC_SUPPORT
        /* Check to see if this function should be called when the mpatrol
         * library is initialised or terminated.
         */
        if ((strncmp(r, "__mp_init_", 10) == 0) && (r[10] != '\0'))
            __mp_atinit((infohead *) y->inits, (void (*)(void)) a);
        else if ((strncmp(r, "__mp_fini_", 10) == 0) && (r[10] != '\0'))
            __mp_atfini((infohead *) y->inits, (void (*)(void)) a);
#endif /* MP_INITFUNC_SUPPORT */
    }
    return 1;
}
#elif FORMAT == FORMAT_ELF64
/* Allocate a new symbol node for a given ELF64 symbol.
 */

static
int
addsymbol(symhead *y, Elf64_Sym *p, char *f, char *s, size_t b)
{
    symnode *n;
    char *r;
    size_t a;
    unsigned char t;

    a = b + p->st_value;
    /* We don't allocate a symbol node for symbols which have a virtual
     * address of zero or are of object type.
     */
    if (addsymname(&s) && (a > 0) &&
        (((t = ELF64_ST_TYPE(p->st_info)) == STT_NOTYPE) || (t == STT_FUNC)))
    {
        if ((n = getsymnode(y)) == NULL)
            return 0;
        if ((r = __mp_addstring(&y->strings, s)) == NULL)
        {
            __mp_freeslot(&y->table, n);
            return 0;
        }
        __mp_treeinsert(&y->dtree, &n->data.node, a);
        n->data.file = f;
        n->data.name = r;
        n->data.addr = (void *) a;
        n->data.size = p->st_size;
        n->data.index = 0;
        n->data.offset = 0;
        /* The linkage information is required for when we look up a symbol.
         */
        n->data.flags = ELF64_ST_BIND(p->st_info);
#if MP_INITFUNC_SUPPORT
        /* Check to see if this function should be called when the mpatrol
         * library is initialised or terminated.
         */
        if ((strncmp(r, "__mp_init_", 10) == 0) && (r[10] != '\0'))
            __mp_atinit((infohead *) y->inits, (void (*)(void)) a);
        else if ((strncmp(r, "__mp_fini_", 10) == 0) && (r[10] != '\0'))
            __mp_atfini((infohead *) y->inits, (void (*)(void)) a);
#endif /* MP_INITFUNC_SUPPORT */
    }
    return 1;
}
#elif FORMAT == FORMAT_BFD
/* Allocate a new symbol node for a given BFD symbol.
 */

static
int
addsymbol(symhead *y, asymbol *p, char *f, char *s, size_t b)
{
    symnode *n;
    char *r;
    size_t a;

    a = b + (size_t) p->value;
    /* We don't allocate a symbol node for symbols which have a virtual
     * address of zero and we only remember symbols that are declared
     * statically, externally or weakly visible.
     */
    if (addsymname(&s) && (a > 0) &&
        (p->flags & (BSF_LOCAL | BSF_GLOBAL | BSF_WEAK)))
    {
        if ((n = getsymnode(y)) == NULL)
            return 0;
        if ((r = __mp_addstring(&y->strings, s)) == NULL)
        {
            __mp_freeslot(&y->table, n);
            return 0;
        }
        __mp_treeinsert(&y->dtree, &n->data.node, a);
        n->data.file = f;
        n->data.name = r;
        n->data.addr = (void *) a;
        /* The BFD library doesn't seem to support reading symbol sizes,
         * so we have to rely on the assumption that a function symbol
         * ends at the beginning of the next function symbol.  This will
         * be calculated in __mp_fixsymbols().
         */
        n->data.size = 0;
        n->data.index = 0;
        n->data.offset = 0;
        /* The linkage information is required for when we look up a symbol.
         */
        n->data.flags = p->flags;
#if MP_INITFUNC_SUPPORT
        /* Check to see if this function should be called when the mpatrol
         * library is initialised or terminated.
         */
        if ((strncmp(r, "__mp_init_", 10) == 0) && (r[10] != '\0'))
            __mp_atinit((infohead *) y->inits, (void (*)(void)) a);
        else if ((strncmp(r, "__mp_fini_", 10) == 0) && (r[10] != '\0'))
            __mp_atfini((infohead *) y->inits, (void (*)(void)) a);
#endif /* MP_INITFUNC_SUPPORT */
    }
    return 1;
}
#endif /* FORMAT */


#if DYNLINK == DYNLINK_WINDOWS
/* The callback function called to allocate a new symbol node for each
 * symbol located in a module by the imagehlp library.
 */

static
int __stdcall
addsym(char *s, unsigned long a, unsigned long l, void *p)
{
    syminfo *i;
    symhead *y;
    symnode *n;
    char *r;

    i = (syminfo *) p;
    y = i->syms;
    /* We don't bother storing a symbol which has no name or which has a
     * virtual address of zero.  Unfortunately, the imagehlp library does
     * not provide information about the types of symbols so we will just
     * have to add any symbols representing objects here as well.
     */
    if ((s != NULL) && (*s != '\0') && (a > 0))
    {
        if ((n = getsymnode(y)) == NULL)
            return 0;
        if ((r = __mp_addstring(&y->strings, s)) == NULL)
        {
            __mp_freeslot(&y->table, n);
            return 0;
        }
        __mp_treeinsert(&y->dtree, &n->data.node, a);
        n->data.file = i->file;
        n->data.name = r;
        n->data.addr = (void *) a;
        n->data.size = l;
        n->data.index = 0;
        n->data.offset = 0;
        /* Unfortunately, the imagehlp library does not provide information
         * about the visibility of symbols.
         */
        n->data.flags = 0;
    }
    return 1;
}
#endif /* DYNLINK */


#if FORMAT == FORMAT_AOUT
/* Allocate a set of symbol nodes for an a.out executable file.
 */

static
int
addsymbols(symhead *y, char *e, char *l, char *f, size_t b, size_t a)
{
    struct exec *o;
    struct nlist *p;
    char *c, *m;
    size_t i, n;

    /* Check that we have a valid a.out executable file.
     */
    if (b < sizeof(struct exec))
    {
        c = "not an object file";
        if (l != NULL)
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s [%s]: %s\n", l, f, c);
        else
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", f, c);
        return 1;
    }
    o = (struct exec *) e;
    if (N_BADMAG(*o))
    {
        c = "not an executable file";
        if (l != NULL)
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s [%s]: %s\n", l, f, c);
        else
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", f, c);
        return 1;
    }
    /* Look for the symbol table.
     */
    i = N_SYMOFF(*o);
    n = o->a_syms;
    if ((i == 0) || (n == 0) || (b < i + n))
    {
        c = "missing symbol table";
        if (l != NULL)
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s [%s]: %s\n", l, f, c);
        else
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", f, c);
        return 1;
    }
    p = (struct nlist *) (e + i);
    b -= i + n;
    n /= sizeof(struct nlist);
    /* Look for the string table.
     */
    m = e + N_STROFF(*o);
    if (b < 4)
        i = 0;
    else
        i = *((size_t *) m);
    if ((i == 0) || (b < i))
    {
        c = "missing string table";
        if (l != NULL)
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s [%s]: %s\n", l, f, c);
        else
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", f, c);
        return 1;
    }
    if (l != NULL)
        f = l;
    /* Cycle through every symbol contained in the executable file.
     */
    for (i = 0; i < n; i++, p++)
        /* We only need to bother looking at text symbols.
         */
        if ((((p->n_type & N_TYPE) == N_TEXT) && !(p->n_type & N_STAB)) &&
            !addsymbol(y, p, f, m + p->n_un.n_strx, a))
            return 0;
    return 1;
}
#elif FORMAT == FORMAT_COFF || FORMAT == FORMAT_XCOFF
/* Allocate a set of symbol nodes for a COFF or XCOFF executable file.
 */

static
int
addsymbols(symhead *y, char *e, char *l, char *f, size_t b, size_t a)
{
    char n[SYMNMLEN + 1];
    FILHDR *o;
    AOUTHDR *v;
    SCNHDR *h;
    SYMENT *p;
    char *c, *m, *s;
    size_t i, t;

    /* Check that we have a valid COFF or XCOFF executable file.
     */
    if (b < FILHSZ)
    {
        c = "not an object file";
        if (l != NULL)
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s [%s]: %s\n", l, f, c);
        else
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", f, c);
        return 1;
    }
    o = (FILHDR *) e;
    b -= FILHSZ;
    if (!ISCOFF(o->f_magic) || (o->f_opthdr == 0) || (b < o->f_opthdr))
    {
        c = "not an executable file";
        if (l != NULL)
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s [%s]: %s\n", l, f, c);
        else
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", f, c);
        return 1;
    }
    /* COFF and XCOFF dynamic linkers don't record the original start
     * address of the text section so we must adjust the base address here
     * if necessary.
     */
    if ((a != 0) && (o->f_opthdr >= sizeof(AOUTHDR)))
    {
        v = (AOUTHDR *) (e + FILHSZ);
        if (a > v->text_start)
            a -= (v->text_start & ~0xFFFF);
    }
    b -= o->f_opthdr;
    if ((o->f_nscns == 0) || (b < o->f_nscns * SCNHSZ))
    {
        c = "missing section table";
        if (l != NULL)
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s [%s]: %s\n", l, f, c);
        else
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", f, c);
        return 1;
    }
    /* Look for the section index of the text section.  This is
     * usually 1, but we should really make sure.
     */
    h = (SCNHDR *) (e + FILHSZ + o->f_opthdr);
    b += FILHSZ + o->f_opthdr - o->f_symptr;
    for (i = t = 0; i < o->f_nscns; i++)
        if ((h[i].s_flags & STYP_TEXT) ||
            (strncmp(h[i].s_name, ".text", 8) == 0))
        {
            t = i + 1;
            break;
        }
    if (t == 0)
        t = 1;
    /* Look for the symbol table.
     */
    i = o->f_nsyms * SYMESZ;
    if ((o->f_symptr == 0) || (o->f_nsyms == 0) || (b < i))
    {
        c = "missing symbol table";
        if (l != NULL)
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s [%s]: %s\n", l, f, c);
        else
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", f, c);
        return 1;
    }
    p = (SYMENT *) (e + o->f_symptr);
    b -= i;
    /* Look for the string table.
     */
    m = (char *) p + i;
    if (b < 4)
        i = 0;
    else
        i = *((size_t *) m);
    if ((i == 0) || (b < i))
    {
        c = "missing string table";
        if (l != NULL)
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s [%s]: %s\n", l, f, c);
        else
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", f, c);
        return 1;
    }
    if (l != NULL)
        f = l;
    /* Cycle through every symbol contained in the executable file.
     */
    for (i = 0; i < o->f_nsyms; i += p->n_numaux + 1,
         p = (SYMENT *) ((char *) p + (p->n_numaux + 1) * SYMESZ))
        /* We only need to bother looking at text symbols.
         */
        if (p->n_scnum == t)
        {
            if (p->n_zeroes)
            {
                /* Symbol name is stored in structure.
                 */
                strncpy(n, p->n_name, SYMNMLEN);
                n[SYMNMLEN] = '\0';
                s = n;
            }
            else
                /* Symbol name is stored in string table.
                 */
                s = m + p->n_offset;
            if (!addsymbol(y, p, f, s, a))
                return 0;
        }
    return 1;
}
#elif FORMAT == FORMAT_ELF32
/* Allocate a set of symbol nodes for an ELF32 object file.
 */

static
int
addsymbols(symhead *y, Elf *e, char *a, char *f, size_t b)
{
    Elf_Scn *s;
    Elf32_Shdr *h;
    Elf32_Sym *p;
    Elf_Data *d;
    char *m;
    size_t i, l, n, t;

    /* Look for the symbol table.
     */
    for (s = elf_nextscn(e, NULL), d = NULL; s != NULL; s = elf_nextscn(e, s))
        if ((h = elf32_getshdr(s)) && (h->sh_type == SHT_SYMTAB) &&
            (d = elf_getdata(s, NULL)))
            break;
    if (d == NULL)
        /* If we couldn't find the symbol table then it is likely that the file
         * has been stripped.  However, if the file was dynamically linked then
         * we may be able to obtain some symbols from its dynamic symbol table.
         */
        for (s = elf_nextscn(e, NULL), d = NULL; s != NULL;
             s = elf_nextscn(e, s))
            if ((h = elf32_getshdr(s)) && (h->sh_type == SHT_DYNSYM) &&
                (d = elf_getdata(s, NULL)))
                break;
    if ((d == NULL) || (d->d_buf == NULL) || (d->d_size == 0))
    {
        m = "missing symbol table";
        if (a != NULL)
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s [%s]: %s\n", a, f, m);
        else
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", f, m);
        return 1;
    }
    t = h->sh_link;
    /* Look for the string table.
     */
    if (((s = elf_getscn(e, t)) == NULL) || ((h = elf32_getshdr(s)) == NULL) ||
        (h->sh_type != SHT_STRTAB))
    {
        m = "missing string table";
        if (a != NULL)
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s [%s]: %s\n", a, f, m);
        else
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", f, m);
        return 1;
    }
    if (a != NULL)
        f = a;
    p = (Elf32_Sym *) d->d_buf + 1;
    l = d->d_size / sizeof(Elf32_Sym);
    /* Cycle through every symbol contained in the object file.
     */
    for (i = 1; i < l; i++, p++)
        /* We don't need to bother looking at undefined, absolute or common
         * symbols, and we only need to store non-data symbols.  However, on
         * IRIX the section index for a symbol in a linked object file does
         * not always refer to the ELF section it was defined in so we skip
         * the last check.
         */
#if SYSTEM == SYSTEM_IRIX
        if (((n = p->st_shndx) != SHN_UNDEF) && (n != SHN_ABS) &&
            (n != SHN_COMMON))
#else /* SYSTEM */
        if (((n = p->st_shndx) != SHN_UNDEF) && (n != SHN_ABS) &&
            (n != SHN_COMMON) && (s = elf_getscn(e, n)) &&
            (h = elf32_getshdr(s)) && (h->sh_flags & SHF_EXECINSTR))
#endif /* SYSTEM */
            if (!addsymbol(y, p, f, elf_strptr(e, t, p->st_name), b))
                return 0;
    return 1;
}
#elif FORMAT == FORMAT_ELF64
/* Allocate a set of symbol nodes for an ELF64 object file.
 */

static
int
addsymbols(symhead *y, Elf *e, char *a, char *f, size_t b)
{
    Elf_Scn *s;
    Elf64_Shdr *h;
    Elf64_Sym *p;
    Elf_Data *d;
    char *m;
    size_t i, l, n, t;

    /* Look for the symbol table.
     */
    for (s = elf_nextscn(e, NULL), d = NULL; s != NULL; s = elf_nextscn(e, s))
        if ((h = elf64_getshdr(s)) && (h->sh_type == SHT_SYMTAB) &&
            (d = elf_getdata(s, NULL)))
            break;
    if (d == NULL)
        /* If we couldn't find the symbol table then it is likely that the file
         * has been stripped.  However, if the file was dynamically linked then
         * we may be able to obtain some symbols from its dynamic symbol table.
         */
        for (s = elf_nextscn(e, NULL), d = NULL; s != NULL;
             s = elf_nextscn(e, s))
            if ((h = elf64_getshdr(s)) && (h->sh_type == SHT_DYNSYM) &&
                (d = elf_getdata(s, NULL)))
                break;
    if ((d == NULL) || (d->d_buf == NULL) || (d->d_size == 0))
    {
        m = "missing symbol table";
        if (a != NULL)
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s [%s]: %s\n", a, f, m);
        else
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", f, m);
        return 1;
    }
    t = h->sh_link;
    /* Look for the string table.
     */
    if (((s = elf_getscn(e, t)) == NULL) || ((h = elf64_getshdr(s)) == NULL) ||
        (h->sh_type != SHT_STRTAB))
    {
        m = "missing string table";
        if (a != NULL)
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s [%s]: %s\n", a, f, m);
        else
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", f, m);
        return 1;
    }
    if (a != NULL)
        f = a;
    p = (Elf64_Sym *) d->d_buf + 1;
    l = d->d_size / sizeof(Elf64_Sym);
    /* Cycle through every symbol contained in the object file.
     */
    for (i = 1; i < l; i++, p++)
        /* We don't need to bother looking at undefined, absolute or common
         * symbols, and we only need to store non-data symbols.  However, on
         * IRIX the section index for a symbol in a linked object file does
         * not always refer to the ELF section it was defined in so we skip
         * the last check.
         */
#if SYSTEM == SYSTEM_IRIX
        if (((n = p->st_shndx) != SHN_UNDEF) && (n != SHN_ABS) &&
            (n != SHN_COMMON))
#else /* SYSTEM */
        if (((n = p->st_shndx) != SHN_UNDEF) && (n != SHN_ABS) &&
            (n != SHN_COMMON) && (s = elf_getscn(e, n)) &&
            (h = elf64_getshdr(s)) && (h->sh_flags & SHF_EXECINSTR))
#endif /* SYSTEM */
            if (!addsymbol(y, p, f, elf_strptr(e, t, p->st_name), b))
                return 0;
    return 1;
}
#elif FORMAT == FORMAT_BFD
/* Allocate a set of symbol nodes for a BFD object file.
 */

static
int
addsymbols(symhead *y, bfd *h, char *a, char *f, size_t b)
{
    asymbol **p;
    char *m;
    long i, n;
    int d, r;

    /* Look for the symbol table.
     */
    d = 0;
    if ((n = bfd_get_symtab_upper_bound(h)) < 0)
    {
        m = (char *) bfd_errmsg(bfd_get_error());
        if (a != NULL)
            __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s [%s]: %s\n", a, f, m);
        else
            __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", f, m);
        return 0;
    }
    if (n == 0)
    {
        /* If we couldn't find the symbol table then it is likely that the file
         * has been stripped.  However, if the file was dynamically linked then
         * we may be able to obtain some symbols from its dynamic symbol table.
         */
        if ((n = bfd_get_dynamic_symtab_upper_bound(h)) < 0)
        {
            m = (char *) bfd_errmsg(bfd_get_error());
            if (a != NULL)
                __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s [%s]: %s\n", a, f, m);
            else
                __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", f, m);
            return 0;
        }
        if (n == 0)
        {
            m = "missing symbol table";
            if (a != NULL)
                __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s [%s]: %s\n", a, f, m);
            else
                __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", f, m);
            return 1;
        }
        d = 1;
    }
    /* It's actually safe to call malloc() here since the library checks
     * for recursive behaviour.
     */
    if ((p = (asymbol **) malloc(n)) == NULL)
    {
        m = "no memory for symbols";
        if (a != NULL)
            __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s [%s]: %s\n", a, f, m);
        else
            __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", f, m);
        return 0;
    }
    r = 1;
    if (((d == 0) && ((n = bfd_canonicalize_symtab(h, p)) < 0)) ||
        ((d == 1) && ((n = bfd_canonicalize_dynamic_symtab(h, p)) < 0)))
    {
        m = (char *) bfd_errmsg(bfd_get_error());
        if (a != NULL)
            __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s [%s]: %s\n", a, f, m);
        else
            __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", f, m);
        r = 0;
    }
    else
    {
        /* Cycle through every symbol contained in the object file.
         */
        if (a != NULL)
            f = a;
        for (i = 0; i < n; i++)
            /* We don't need to bother looking at undefined, absolute or common
             * symbols, and we only need to store non-data symbols.
             */
            if (!bfd_is_und_section(p[i]->section) &&
                !bfd_is_abs_section(p[i]->section) &&
                !bfd_is_com_section(p[i]->section) &&
                (p[i]->section->flags & SEC_CODE))
                if (!addsymbol(y, p[i], f, (char *) p[i]->name,
                    (size_t) p[i]->section->vma + b))
                {
                    r = 0;
                    break;
                }
    }
    /* If we are making use of line number information in the object files then
     * we store the symbol table along with the access library handle; otherwise
     * we can free the symbol table now.
     */
    if (y->lineinfo && (r == 1))
        ((objectfile *) y->htail)->symbols = p;
    else
        free(p);
    return r;
}
#endif /* FORMAT */


#if DYNLINK == DYNLINK_WINDOWS
/* The callback function called to allocate a set of symbol nodes for each
 * module located by the imagehlp library.
 */

static
int __stdcall
addsyms(char *f, unsigned long b, void *p)
{
    modinfo *i;
    symhead *y;
    char *t;
    syminfo s;
    IMAGEHLP_MODULE m;
    size_t l;
    int r;

    r = 1;
    i = (modinfo *) p;
    y = i->syms;
    l = y->dtree.size;
    /* The executable file is the first module, so we only want to examine it
     * if we are not looking for external symbols.  The DLLs are the subsequent
     * modules, so we only want to examine them if we are looking for external
     * symbols.
     */
    if ((!i->libs && !i->index) || (i->libs && i->index))
    {
        /* Attempt to determine the full path of the current module.
         */
        m.SizeOfStruct = sizeof(IMAGEHLP_MODULE);
        if (SymGetModuleInfo(GetCurrentProcess(), b, &m))
            f = m.LoadedImageName;
        if ((t = __mp_addstring(&y->strings, f)) == NULL)
            r = 0;
        else
        {
            /* Cycle through every symbol contained in the module.
             */
            s.syms = y;
            s.file = t;
            r = SymEnumerateSymbols(GetCurrentProcess(), b, addsym, &s);
        }
        if (i->libs)
        {
            if (r == 1)
            {
                l = y->dtree.size - l;
                __mp_diag("read %lu symbol%s from ", l, (l == 1) ? "" : "s");
            }
            else
                __mp_diag("problem reading symbols from ");
            if (__mp_diagflags & FLG_HTML)
                __mp_diagtag("<TT>");
            __mp_diag("%s", f);
            if (__mp_diagflags & FLG_HTML)
            {
                __mp_diagtag("</TT>\n");
                __mp_diagtag("<BR>");
            }
            __mp_diag("\n");
        }
    }
    i->index++;
    return r;
}
#endif /* DYNLINK */


/* Read a file and add all relevant symbols contained within it to the
 * symbol table.
 */

MP_GLOBAL
int
__mp_addsymbols(symhead *y, char *s, char *v, size_t b)
{
#if FORMAT == FORMAT_AOUT || FORMAT == FORMAT_COFF || \
    FORMAT == FORMAT_XCOFF || FORMAT == FORMAT_ELF32 || \
    FORMAT == FORMAT_ELF64 || FORMAT == FORMAT_BFD
#if FORMAT == FORMAT_AOUT || (SYSTEM == SYSTEM_LYNXOS && \
     (FORMAT == FORMAT_COFF || FORMAT == FORMAT_XCOFF))
    char *m;
    off_t o;
    int f;
#elif FORMAT == FORMAT_COFF || FORMAT == FORMAT_XCOFF
    LDFILE *f;
    char *c, *m, *n;
    ARCHDR a;
    size_t o;
#elif FORMAT == FORMAT_ELF32 || FORMAT == FORMAT_ELF64
    Elf *a, *e;
    Elf_Arhdr *h;
    int f;
#elif FORMAT == FORMAT_BFD
    objectfile *p, *q;
    bfd *a, *g, *h;
#endif /* FORMAT */
    char *t;
#elif FORMAT == FORMAT_PE
    modinfo m;
#endif /* FORMAT */
    size_t l;
    int r;

    r = 1;
    l = y->dtree.size;
#if FORMAT == FORMAT_AOUT || (SYSTEM == SYSTEM_LYNXOS && \
     (FORMAT == FORMAT_COFF || FORMAT == FORMAT_XCOFF))
    /* This is a very simple, yet portable, way to read symbols from a.out
     * executable files, or from COFF or XCOFF executable files when libld
     * is not available.
     */
    if ((f = open(s, O_RDONLY)) == -1)
    {
        __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: cannot open file\n", s);
        r = 0;
    }
    else
    {
        /* Find out the size of the file by doing a seek to the end of the file
         * and then a seek back to the start of the file.  Unfortunately, on
         * some systems off_t is a structure, so the code below may not work
         * for them.
         */
        if (((o = lseek(f, 0, SEEK_END)) == (off_t) -1) ||
            (lseek(f, 0, SEEK_SET) == (off_t) -1))
        {
            __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: cannot seek file\n", s);
            r = 0;
        }
        else if ((m = (char *) malloc((size_t) o)) == NULL)
        {
            /* It's actually safe to call malloc() here since the library
             * checks for recursive behaviour.
             */
            __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: no memory for symbols\n",
                       s);
            r = 0;
        }
        else
        {
            if (read(f, m, (size_t) o) != (size_t) o)
            {
                __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: cannot read file\n",
                           s);
                r = 0;
            }
            else if ((t = __mp_addstring(&y->strings, s)) == NULL)
                r = 0;
            else
                r = addsymbols(y, m, NULL, t, (size_t) o, b);
            free(m);
        }
        close(f);
    }
#elif FORMAT == FORMAT_COFF || FORMAT == FORMAT_XCOFF
    /* We use the libld COFF and XCOFF access library in order to simplify
     * the reading of symbols.  If this library is not available on a certain
     * platform then use the above code for reading a.out files and comment
     * this code out.  However, a better solution would be to use the GNU
     * BFD library if it is available.
     */
    t = NULL;
    if ((f = ldopen(s, NULL)) == NULL)
    {
        __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: cannot open file\n", s);
        r = 0;
    }
    while (r == 1)
    {
        if (ldahread(f, &a))
        {
            n = a.ar_name;
            o = a.ar_size;
            if (((v != NULL) && (strcmp(n, v) != 0)) ||
                (FSEEK(f, 0, BEGINNING) == -1))
            {
                if (ldclose(f))
                    break;
                if ((f = ldopen(s, f)) == NULL)
                    r = 0;
                continue;
            }
        }
        else
            n = NULL;
        /* Find out the size of the file by doing a seek to the end of the file
         * and then a seek back to the start of the file, but only if this is
         * not an archive member.
         */
        if ((n == NULL) && ((FSEEK(f, 0, END) == -1) ||
             ((o = FTELL(f)) == (size_t) -1) || (FSEEK(f, 0, BEGINNING) == -1)))
        {
            __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: cannot seek file\n", s);
            r = 0;
        }
        else if ((m = (char *) malloc(o)) == NULL)
        {
            /* It's actually safe to call malloc() here since the library
             * checks for recursive behaviour.
             */
            c = "no memory for symbols";
            if (n != NULL)
                __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s [%s]: %s\n", s, n, c);
            else
                __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", s, c);
            r = 0;
        }
        else
        {
            if (FREAD(m, sizeof(char), o, f) != o)
            {
                c = "cannot read file";
                if (n != NULL)
                    __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s [%s]: %s\n", s, n,
                               c);
                else
                    __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", s, c);
                r = 0;
            }
            else if ((t == NULL) &&
                     ((t = __mp_addstring(&y->strings, s)) == NULL))
                r = 0;
            else if (n != NULL)
                r = addsymbols(y, m, t, n, o, b);
            else
                r = addsymbols(y, m, NULL, t, o, b);
            free(m);
        }
        if (ldclose(f))
            break;
        if ((r == 1) && ((f = ldopen(s, f)) == NULL))
            r = 0;
    }
#elif FORMAT == FORMAT_ELF32 || FORMAT == FORMAT_ELF64
    /* We use the libelf ELF access library in order to simplify the reading
     * of symbols.  At the moment, this solution is better than using the
     * GNU BFD library as it currently has no support for symbol sizes or
     * liaising with the dynamic linker.
     */
    if (elf_version(EV_CURRENT) == EV_NONE)
    {
        __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: wrong version of libelf\n", s);
        r = 0;
    }
    else if ((f = open(s, O_RDONLY)) == -1)
    {
        __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: cannot open file\n", s);
        r = 0;
    }
    else
    {
        if ((e = elf_begin(f, ELF_C_READ, NULL)) == NULL)
        {
            __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", s, elf_errmsg(-1));
            r = 0;
        }
        else if ((t = __mp_addstring(&y->strings, s)) == NULL)
            r = 0;
        else
        {
            if (elf_kind(e) == ELF_K_AR)
                /* Normally we wouldn't ever need to read symbols from an
                 * archive library, but this is just provided for
                 * completeness.
                 */
                while ((r == 1) && (a = elf_begin(f, ELF_C_READ, e)))
                {
                    if ((h = elf_getarhdr(a)) == NULL)
                    {
                        __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", s,
                                   elf_errmsg(-1));
                        r = 0;
                    }
                    else if ((*h->ar_name != '/') && ((v == NULL) ||
                              (strcmp(h->ar_name, v) == 0)))
                        r = addsymbols(y, a, t, h->ar_name, b);
                    if (r == 1)
                        elf_next(a);
                    elf_end(a);
                }
            else
                r = addsymbols(y, e, NULL, t, b);
            elf_end(e);
        }
        close(f);
    }
#elif FORMAT == FORMAT_BFD
    /* Using the GNU BFD library allows us to read weird and wonderful file
     * formats that would otherwise be hard to support.  This is probably a
     * better choice to use than the in-built a.out, COFF and XCOFF
     * implementations but currently has no support for symbol sizes, so the
     * ELF access library is still worth using for ELF file formats, but the
     * BFD library comes with support for debugging information.  So take
     * your pick!
     */
    t = NULL;
    bfd_init();
    if ((h = bfd_openr(s, NULL)) == NULL)
    {
        __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", s,
                   bfd_errmsg(bfd_get_error()));
        r = 0;
    }
    else
    {
        /* Normally we wouldn't ever need to read symbols from an archive
         * library, but this is just provided for completeness, and for AIX
         * where shared libraries can be embedded within archive libraries.
         */
        if (bfd_check_format(h, bfd_archive))
        {
            a = h;
            h = bfd_openr_next_archived_file(a, NULL);
        }
        else
            a = NULL;
        while (h != NULL)
        {
            p = NULL;
            if ((a != NULL) && (v != NULL) && (strcmp(h->filename, v) != 0))
            {
                g = h;
                h = bfd_openr_next_archived_file(a, g);
                bfd_close(g);
                continue;
            }
            if (!bfd_check_format(h, bfd_object))
            {
                if (a != NULL)
                    __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s [%s]: %s\n", s,
                               h->filename, bfd_errmsg(bfd_get_error()));
                else
                    __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: %s\n", s,
                               bfd_errmsg(bfd_get_error()));
                r = 0;
            }
            else if (y->lineinfo &&
                     ((p = (objectfile *) malloc(sizeof(objectfile))) == NULL))
                r = 0;
            else if ((t == NULL) &&
                     ((t = __mp_addstring(&y->strings, s)) == NULL))
                r = 0;
            else
            {
                if (y->lineinfo)
                {
                    if (y->hhead == NULL)
                        y->hhead = p;
                    else
                    {
                        q = (objectfile *) y->htail;
                        q->next = p;
                    }
                    y->htail = p;
                    p->next = NULL;
                    p->file = h;
                    p->symbols = NULL;
                    p->base = b;
                }
                if (a != NULL)
                    r = addsymbols(y, h, t, (char *) h->filename, b);
                else
                    r = addsymbols(y, h, NULL, t, b);
                if (y->lineinfo && (r == 0))
                    if (y->hhead == p)
                        y->hhead = y->htail = NULL;
                    else
                    {
                        y->htail = q;
                        q->next = NULL;
                    }
            }
            g = h;
            if ((a != NULL) && (r == 1))
                h = bfd_openr_next_archived_file(a, g);
            else
                h = NULL;
            if (!y->lineinfo || (r == 0))
            {
                if (p != NULL)
                    free(p);
                bfd_close(g);
            }
        }
        if (a != NULL)
            bfd_close(a);
    }
#elif FORMAT == FORMAT_PE
    /* We only want to obtain the symbols from the executable file using the
     * imagehlp library if we are not using another object file access library,
     * such as GNU BFD.
     */
    m.syms = y;
    m.index = 0;
    m.libs = 0;
    r = SymEnumerateModules(GetCurrentProcess(), addsyms, &m);
#endif /* FORMAT */
    if (r == 1)
    {
        l = y->dtree.size - l;
        __mp_diag("read %lu symbol%s from ", l, (l == 1) ? "" : "s");
    }
    else
        __mp_diag("problem reading symbols from ");
    if (__mp_diagflags & FLG_HTML)
        __mp_diagtag("<TT>");
    __mp_diag("%s", s);
    if (__mp_diagflags & FLG_HTML)
        __mp_diagtag("</TT>");
    if (v != NULL)
        if (__mp_diagflags & FLG_HTML)
        {
            __mp_diag(" [");
            __mp_diagtag("<TT>");
            __mp_diag("%s", v);
            __mp_diagtag("</TT>");
            __mp_diag("]");
        }
        else
            __mp_diag(" [%s]", v);
    __mp_diag("\n");
    if (__mp_diagflags & FLG_HTML)
        __mp_diagtag("<BR>\n");
    return r;
}


/* Add any external or additional symbols to the symbol table.
 */

MP_GLOBAL
int
__mp_addextsymbols(symhead *y, meminfo *e)
{
#if DYNLINK == DYNLINK_AIX
    static char b[4096];
    struct ld_info *l;
    char *s;
#elif DYNLINK == DYNLINK_BSD
#if SYSTEM == SYSTEM_SUNOS
    struct link_dynamic *d;
    struct link_map *l;
#else /* SYSTEM */
    struct _dynamic *d;
    struct so_map *l;
#endif /* SYSTEM */
#elif DYNLINK == DYNLINK_HPUX
    struct shl_descriptor d;
    size_t i;
    unsigned int o;
#elif DYNLINK == DYNLINK_IRIX
    struct obj_list *l;
    struct obj *o;
    objectinfo *i;
    char *s;
    size_t b;
#elif DYNLINK == DYNLINK_OSF
    ldr_region_info_t r;
    ldr_module_info_t i;
    ldr_module_t m;
    ldr_process_t p;
    size_t c, l, n;
    size_t b;
#elif DYNLINK == DYNLINK_SVR4
#if ENVIRON == ENVIRON_64
    Elf64_Dyn *d;
#else /* ENVIRON */
    Elf32_Dyn *d;
#endif /* ENVIRON */
    dynamiclink *l;
#elif DYNLINK == DYNLINK_WINDOWS
    modinfo m;
#endif /* DYNLINK */

    /* This function liaises with the dynamic linker when a program is
     * dynamically linked in order to read symbols from any required shared
     * objects.
     */
#if DYNLINK == DYNLINK_AIX
    if (loadquery(L_GETINFO, b, sizeof(b)) != -1)
    {
        /* We skip past the first item on the list since it represents the
         * executable file, but we may wish to record the name of the file
         * if we haven't already determined it.
         */
        l = (struct ld_info *) b;
        s = l->ldinfo_filename;
        if ((e->prog == NULL) && (*s != '\0'))
            e->prog = __mp_addstring(&y->strings, s);
        while (l = l->ldinfo_next ? (struct ld_info *)
                                    ((char *) l + l->ldinfo_next) : NULL)
            if (*l->ldinfo_filename != '\0')
            {
                /* If the filename represents an archive file then the member
                 * name within the archive will follow immediately after it.
                 */
                s = l->ldinfo_filename + strlen(l->ldinfo_filename) + 1;
                if (*s == '\0')
                    s = NULL;
                if (!__mp_addsymbols(y, l->ldinfo_filename, s,
                                     (unsigned long) l->ldinfo_textorg))
                    return 0;
            }
    }
#elif DYNLINK == DYNLINK_BSD
    /* Check to see if the dynamic linker has set up the _DYNAMIC symbol
     * and also check that it is a valid structure.
     */
    if ((d = &_DYNAMIC) &&
#if SYSTEM == SYSTEM_SUNOS
        (d->ld_version > 1) && (d->ld_version <= 3) && (d->ld_un.ld_1 != NULL))
#else /* SYSTEM */
        (d->d_version == LD_VERSION_BSD) && (d->d_un.d_sdt != NULL))
#endif /* SYSTEM */
    {
#if SYSTEM == SYSTEM_SUNOS
        l = d->ld_un.ld_1->ld_loaded;
#else /* SYSTEM */
        l = d->d_un.d_sdt->sdt_loaded;
#endif /* SYSTEM */
        while (l != NULL)
        {
#if SYSTEM == SYSTEM_SUNOS
            if ((l->lm_name != NULL) && (*l->lm_name != '\0') &&
                !__mp_addsymbols(y, l->lm_name, NULL, l->lm_addr))
#else /* SYSTEM */
            if ((l->som_path != NULL) && (*l->som_path != '\0') &&
                !__mp_addsymbols(y, l->som_path, NULL, l->som_addr))
#endif /* SYSTEM */
                return 0;
#if SYSTEM == SYSTEM_SUNOS
            l = l->lm_next;
#else /* SYSTEM */
            l = l->som_next;
#endif /* SYSTEM */
        }
    }
#elif DYNLINK == DYNLINK_HPUX
    /* An index of -1 indicates the dynamic linker and an index of 0 indicates
     * the main executable program.  We are interested in all other object files
     * that the program depends on, but we may wish to record the name of the
     * executable file if we haven't already determined it.
     */
    if ((e->prog == NULL) && (shl_get_r(0, &d) != -1) &&
        (d.filename[0] != '\0'))
        e->prog = __mp_addstring(&y->strings, d.filename);
    for (i = 1; shl_get_r(i, &d) != -1; i++)
    {
        /* Determine the offset of the first text symbol in the library.  This
         * is normally 0x1000 but may be something else on later systems.  The
         * handle structure is not documented anywhere, but the fourth word
         * appears to contain the information we need, based on trial and error.
         */
        if (d.handle != NULL)
            o = ((unsigned int *) d.handle)[3];
        else
            o = 0;
        if ((d.filename[0] != '\0') &&
            !__mp_addsymbols(y, d.filename, NULL, d.tstart - o))
            return 0;
    }
#elif DYNLINK == DYNLINK_IRIX
    if (l = __rld_obj_head)
        /* Determine if the shared object list we are looking at contains O32
         * ABI object files or N32 ABI object files.
         */
        if (l->data == 0xFFFFFFFF)
        {
            /* We skip past the first item on the list since it represents the
             * executable file, but we may wish to record the name of the file
             * if we haven't already determined it.
             */
            i = (objectinfo *) l;
            s = (char *) i->name;
            if ((e->prog == NULL) && (s != NULL) && (*s != '\0'))
                e->prog = __mp_addstring(&y->strings, s);
            while (i = (objectinfo *) i->next)
            {
                s = (char *) i->name;
                b = (long) i->ehdr - (long) i->ohdr;
                if ((s != NULL) && (*s != '\0') &&
                    !__mp_addsymbols(y, s, NULL, b))
                    return 0;
            }
        }
        else
        {
            /* We skip past the first item on the list since it represents the
             * executable file, but we may wish to record the name of the file
             * if we haven't already determined it.
             */
            o = (struct obj *) l->data;
            s = o->o_path;
            if ((e->prog == NULL) && (s != NULL) && (*s != '\0'))
                e->prog = __mp_addstring(&y->strings, s);
            while (l = l->next)
            {
                o = (struct obj *) l->data;
                s = o->o_path;
                b = (long) o->o_text_start - (long) o->o_base_address;
                if ((s != NULL) && (*s != '\0') &&
                    !__mp_addsymbols(y, s, NULL, b))
                    return 0;
            }
        }
#elif DYNLINK == DYNLINK_OSF
    c = 0;
    p = ldr_my_process();
    m = LDR_NULL_MODULE;
    while ((ldr_next_module(p, &m) == 0) && (m != LDR_NULL_MODULE))
        if (ldr_inq_module(p, m, &i, sizeof(i), &l) == 0)
        {
            /* We skip past the first item on the list since it represents the
             * executable file, but we may wish to record the name of the file
             * if we haven't already determined it.
             */
            if (c++ == 0)
            {
                if ((e->prog == NULL) && (i.lmi_name[0] != '\0'))
                    e->prog = __mp_addstring(&y->strings, i.lmi_name);
                continue;
            }
            for (n = 0; n < (size_t) i.lmi_nregion; n++)
                if ((ldr_inq_region(p, m, (ldr_region_t) n, &r, sizeof(r),
                      &l) == 0) && (strcmp(r.lri_name, ".text") == 0))
                {
                    b = (long) r.lri_mapaddr - (long) r.lri_vaddr;
                    if ((i.lmi_name[0] != '\0') && !__mp_addsymbols(y,
                         i.lmi_name, NULL, b))
                        return 0;
                    break;
                }
        }
#elif DYNLINK == DYNLINK_SVR4
#if ENVIRON == ENVIRON_64
    if ((&_DYNAMIC != NULL) && (d = (Elf64_Dyn *) _DYNAMIC))
#else /* ENVIRON */
    if ((&_DYNAMIC != NULL) && (d = (Elf32_Dyn *) _DYNAMIC))
#endif /* ENVIRON */
    {
        /* Search for the DT_DEBUG tag in the _DYNAMIC symbol.
         */
        for (l = NULL; d->d_tag != DT_NULL; d++)
            if (d->d_tag == DT_DEBUG)
            {
                if (!d->d_un.d_ptr)
                    l = NULL;
                else
                    l = (dynamiclink *) *((unsigned long *) d->d_un.d_ptr + 1);
                break;
            }
        /* We skip past the first item on the list since it represents the
         * executable file, but we may wish to record the name of the file
         * if we haven't already determined it.
         */
        if (l != NULL)
        {
            if ((e->prog == NULL) && (l->name != NULL) && (*l->name != '\0'))
                e->prog = __mp_addstring(&y->strings, l->name);
            l = l->next;
        }
        while (l != NULL)
        {
            if ((l->name != NULL) && (*l->name != '\0') &&
                !__mp_addsymbols(y, l->name, NULL, l->base))
                return 0;
            l = l->next;
        }
    }
#elif DYNLINK == DYNLINK_WINDOWS
    /* We always want to initialise the imagehlp library here since we will
     * be using it to obtain the symbols from any loaded DLLs here and
     * possibly also from the executable file if we are not using any other
     * object file access library.  In any case we can set the demangling
     * option in the imagehlp library and also instruct it to load line number
     * information if the USEDEBUG option is given.
     */
    if (y->lineinfo)
        SymSetOptions(SYMOPT_UNDNAME | SYMOPT_LOAD_LINES);
    else
        SymSetOptions(SYMOPT_UNDNAME);
    SymInitialize(GetCurrentProcess(), NULL, 1);
    /* The imagehlp library allows us to locate the symbols contained in
     * all of the loaded DLLs without having to actually read the files
     * themselves.
     */
    m.syms = y;
    m.index = 0;
    m.libs = 1;
    if (!SymEnumerateModules(GetCurrentProcess(), addsyms, &m))
        return 0;
#endif /* DYNLINK */
    return 1;
}


/* Attempt to tidy up the symbol table by correcting any potential errors or
 * conflicts from the symbols that have been read.
 */

MP_GLOBAL
void
__mp_fixsymbols(symhead *y)
{
    symnode *n, *p;
    void *l, *m;
#if TARGET == TARGET_AMIGA
    unsigned long o;
#endif /* TARGET */

#if TARGET == TARGET_AMIGA
    o = 0;
#endif /* TARGET */
    l = NULL;
    for (n = (symnode *) __mp_minimum(y->dtree.root); n != NULL; n = p)
    {
        /* If a symbol has a zero size and it is closely followed by another
         * symbol then the likelihood is that the symbol really has a non-zero
         * size so we fix that here.  This commonly occurs with system startup
         * files.
         */
        p = (symnode *) __mp_successor(&n->data.node);
        if ((n->data.size == 0) && (n->data.addr >= l))
            if ((p == NULL) || (n->data.file != p->data.file))
                n->data.size = 256;
            else
                n->data.size = (char *) p->data.addr - (char *) n->data.addr;
        if ((m = (char *) n->data.addr + n->data.size) > l)
            l = m;
#if TARGET == TARGET_AMIGA
        /* On AmigaOS, sections are scatter-loaded into memory and will occupy
         * different addresses each time a program is loaded.  One easy way to
         * determine the run-time address of each function is to find the
         * offset for one function and add it to all functions, but this means
         * that this function must be compiled with symbolic information and
         * assumes that all text symbols are in the one hunk.
         */
        if ((o == 0) && (strcmp(n->data.name, "___mp_fixsymbols") == 0))
            o = (char *) __mp_fixsymbols - (char *) n->data.addr;
#endif /* TARGET */
    }
#if TARGET == TARGET_AMIGA
    if (o != 0)
    {
#if FORMAT == FORMAT_BFD
        if (y->hhead != NULL)
            ((objectfile *) y->hhead)->base = o;
#endif /* FORMAT */
        for (n = (symnode *) __mp_minimum(y->dtree.root); n != NULL;
             n = (symnode *) __mp_successor(&n->index.node))
        {
            n->data.node.key += o;
            n->data.addr = (char *) n->data.addr + o;
        }
    }
#endif /* TARGET */
}


/* Protect the memory blocks used by the symbol table with the supplied access
 * permission.
 */

MP_GLOBAL
int
__mp_protectsymbols(symhead *y, memaccess a)
{
    symnode *n;

    /* The library already knows what its protection status is so we don't
     * need to do anything if the request has already been done.
     */
    if (y->prot == a)
    {
        y->protrecur++;
        return 1;
    }
    else if (y->protrecur > 0)
    {
        y->protrecur--;
        return 1;
    }
    y->prot = a;
    for (n = (symnode *) __mp_minimum(y->itree.root); n != NULL;
         n = (symnode *) __mp_successor(&n->index.node))
        if (!__mp_memprotect(&y->heap->memory, n->index.block, n->index.size,
             a))
            return 0;
    return 1;
}


/* Attempt to find the symbol located at a particular address.
 */

MP_GLOBAL
symnode *
__mp_findsymbol(symhead *y, void *p)
{
    symnode *m, *n, *r;

    /* This function does not deal completely correctly with nested symbols
     * but that occurrence does not happen frequently so the current
     * implementation should suffice.
     */
    r = NULL;
    if (n = (symnode *) __mp_searchlower(y->dtree.root, (unsigned long) p))
    {
        while ((m = (symnode *) __mp_predecessor(&n->data.node)) &&
               (m->data.addr == n->data.addr))
            n = m;
        for (m = n; (n != NULL) && (n->data.addr == m->data.addr);
             n = (symnode *) __mp_successor(&n->data.node))
            if ((char *) n->data.addr + n->data.size > (char *) p)
            {
#if FORMAT == FORMAT_AOUT
                /* We give precedence to global symbols, then local symbols.
                 */
                if ((r == NULL) || (!(r->data.flags & N_EXT) &&
                     (n->data.flags & N_EXT)))
                    r = n;
#elif FORMAT == FORMAT_COFF
                /* We give precedence to global symbols, then local symbols.
                 */
                if ((r == NULL) || ((r->data.flags == C_STAT) &&
                     (n->data.flags == C_EXT)))
                    r = n;
#elif FORMAT == FORMAT_XCOFF
                /* We give precedence to global symbols, then hidden external
                 * symbols, then local symbols.
                 */
                if ((r == NULL) || ((r->data.flags & C_STAT) &&
                     ((n->data.flags & C_HIDEXT) || (n->data.flags & C_EXT))) ||
                    ((r->data.flags & C_HIDEXT) && (n->data.flags & C_EXT)))
                    r = n;
#elif FORMAT == FORMAT_ELF32 || FORMAT == FORMAT_ELF64
                /* We give precedence to global symbols, then weak symbols,
                 * then local symbols.
                 */
                if ((r == NULL) || ((r->data.flags == STB_LOCAL) &&
                     ((n->data.flags == STB_WEAK) ||
                      (n->data.flags == STB_GLOBAL))) ||
                    ((r->data.flags == STB_WEAK) &&
                     (n->data.flags == STB_GLOBAL)))
                    r = n;
#elif FORMAT == FORMAT_BFD
                /* We give precedence to global symbols, then weak symbols,
                 * then local symbols.
                 */
                if ((r == NULL) || ((r->data.flags & BSF_LOCAL) &&
                     ((n->data.flags & BSF_WEAK) ||
                      (n->data.flags & BSF_GLOBAL))) ||
                    ((r->data.flags & BSF_WEAK) &&
                     (n->data.flags & BSF_GLOBAL)))
                    r = n;
#else /* FORMAT */
                r = n;
                break;
#endif /* FORMAT */
            }
    }
    return r;
}


#if FORMAT == FORMAT_BFD
/* Search a BFD section for a specific virtual memory address and attempt
 * to match it up with source position information.
 */

static
void
findsource(bfd *h, asection *p, void *d)
{
    sourcepos *s;
    bfd_vma v;
    size_t l;

    s = (sourcepos *) d;
    if (!s->found && !bfd_is_und_section(p) &&
        !bfd_is_abs_section(p) && !bfd_is_com_section(p) &&
        (p->flags & SEC_ALLOC) && (p->flags & SEC_CODE))
    {
        v = bfd_section_vma(h, p) + s->base;
        l = bfd_section_size(h, p);
        if ((s->addr >= v) && (s->addr < v + l))
        {
            s->file = s->func = NULL;
            s->line = 0;
            s->found = bfd_find_nearest_line(h, p, s->symbols, s->addr - v,
                                             (const char **) &s->file,
                                             (const char **) &s->func,
                                             &s->line);
        }
    }
}
#endif /* FORMAT */


/* Attempt to find the source correspondence for a machine instruction located
 * at a particular address.
 */

MP_GLOBAL
int
__mp_findsource(symhead *y, void *p, char **s, char **t, unsigned long *u)
{
#if FORMAT == FORMAT_BFD
    objectfile *n;
    sourcepos m;
#endif /* FORMAT */
#if DYNLINK == DYNLINK_WINDOWS
    static char b[1024];
    IMAGEHLP_SYMBOL *i;
    IMAGEHLP_LINE l;
    unsigned long d;
#endif /* DYNLINK */

#if FORMAT == FORMAT_BFD
    m.addr = (bfd_vma) p;
    m.found = 0;
    for (n = (objectfile *) y->hhead; n != NULL; n = n->next)
    {
        m.symbols = n->symbols;
        m.base = n->base;
        bfd_map_over_sections(n->file, findsource, &m);
        if (m.found)
        {
            *s = m.func;
            *t = m.file;
            *u = m.line;
            return 1;
        }
    }
#endif /* FORMAT */
    *s = *t = NULL;
    *u = 0;
#if DYNLINK == DYNLINK_WINDOWS
    if (y->lineinfo)
    {
        i = (IMAGEHLP_SYMBOL *) b;
        i->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
        i->MaxNameLength = sizeof(b) - sizeof(IMAGEHLP_SYMBOL);
        if (SymGetSymFromAddr(GetCurrentProcess(), (unsigned long) p, &d, i))
            *s = i->Name;
        l.SizeOfStruct = sizeof(IMAGEHLP_LINE);
        if (SymGetLineFromAddr(GetCurrentProcess(), (unsigned long) p, &d, &l))
        {
            *t = l.FileName;
            *u = l.LineNumber;
        }
        if ((*s != NULL) || (*t != NULL))
            return 1;
    }
#endif /* DYNLINK */
    return 0;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
