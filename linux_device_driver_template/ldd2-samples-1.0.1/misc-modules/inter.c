/*
 * inter.c -- A slave variant that uses inter_module functions
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
 * $Id: inter.c,v 1.8 2001/07/18 22:28:16 rubini Exp $
 */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/config.h>
#include <linux/module.h>

/* This depends on kmod.h, refuse to go on with 2.0 */
#if LINUX_VERSION_CODE < 0x020200
#  error "This module needs kmod, so it won't run with 2.0"
#else

#include <linux/init.h>
#include "sysdep.h"

#ifdef HAVE_INTER_MODULE

static char *string = "inter says 'Hello World'";

void ime_function(const char *who)
{
    printk(KERN_INFO "inter: ime_function called by %s\n", who);
}

int ime_init(void)
{
    inter_module_register("ime_string", THIS_MODULE, string);
    inter_module_register("ime_function", THIS_MODULE, ime_function);
    return 0;
}

void ime_cleanup(void)
{
    inter_module_unregister("ime_string");
}


module_init(ime_init);
module_exit(ime_cleanup);
#endif /* HAVE_INTER_MODULE */

#endif /* no 2.0 allowed */

