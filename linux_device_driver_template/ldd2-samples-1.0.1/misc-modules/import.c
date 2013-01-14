/*
 * import.c -- a module using a symbol from export.c
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

/*
 * Use versioning if needed
 */
#include <linux/config.h> /* retrieve the CONFIG_* macros */
#ifdef CONFIG_MODVERSIONS
#   undef MODVERSIONS /* it might be defined */
#   define MODVERSIONS
#endif

#ifdef MODVERSIONS
#  include <linux/modversions.h>
#  include "export.ver"
#endif


#include <linux/module.h>
#include <linux/kernel.h>
#include "sysdep.h"

extern int export_function(int, int);

int import_init(void)
{
    int i = export_function(2,2);
    printk("import: my mate tells that 2+2 = %i\n",i);
    return 0;
}

void import_cleanup(void)
{}

module_init(import_init);
module_exit(import_cleanup);
