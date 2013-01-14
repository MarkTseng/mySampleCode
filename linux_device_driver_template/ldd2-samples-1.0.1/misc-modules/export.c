/*
 * export.c -- export a symbol (maybe a versioned one) (v2.1)
 *
 * Copyright (C) 2001 Alessandro Rubini and Jonathan Corbet
 * Copyright (C) 2001 O'Reilly & Associates
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.  The citation
 * should list that the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates.   No warranty is attached;
 * we cannot take responsibility for errors or fitness for use.
 *
 */


#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

/* This stuff might go in the Makefile, but I'd better put it here */

#include <linux/config.h> /* retrieve the CONFIG_* macros */
#if defined(CONFIG_MODVERSIONS) && !defined(MODVERSIONS)
#   define MODVERSIONS
#endif

/*
 * Include the versioned definitions for both kernel symbols and our
 * symbol, *unless* we are generating checksums (__GENKSYMS__
 * defined) */
#if defined(MODVERSIONS) && !defined(__GENKSYMS__)
#    include <linux/modversions.h>
#    include "export.ver" /* redefine "export_function" to include CRC */
#endif

/*
 * Everything from now on is normal. The previous stuff can be replaced
 * by "$(CC) -D__KERNEL__ -DMODULE -DMODVERSIONS -DEXPORT_SYMTAB \
 *         -include $(INCLUDEDIR)/linux/modversions.h" if versioning
 * is enabled, and the following (simpler) cmdline for genksyms:
 *    "$(CC) -E -DCONFIG_MODVERSIONS -DEXPORT_SYMTAB"
 */

#ifndef EXPORT_SYMTAB
#  define EXPORT_SYMTAB /* need this one 'cause we export symbols */
#endif

#include <linux/module.h>

#include <linux/sched.h>
#include <linux/kernel.h> /* printk() */

#include "sysdep.h"

/* our dumb function, with two dumb arguments */
int export_function(int a, int b);

#ifdef __USE_OLD_SYMTAB__
    static struct symbol_table export_syms = {
        #include <linux/symtab_begin.h>
        X(export_function),
        #include <linux/symtab_end.h>
    };
#else
    EXPORT_SYMBOL(export_function);
#endif

int export_init(void)
{
    REGISTER_SYMTAB(&export_syms);
    return 0;
}

void export_cleanup(void)
{
}

int export_function(int a, int b)
{return a+b;}

module_init(export_init);
module_exit(export_cleanup);
