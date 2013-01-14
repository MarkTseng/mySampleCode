/*
 * master.c -- the master module of a master/slave pair using kmod
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
 */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

/*
 * Inclusions related to version problems. They *might* go after
 * <linux/module.h>, unless a symbol table is exported. All in all,
 * better putting them first anyways.
 */
#include <linux/config.h> /* retrieve the CONFIG_* macros */
#if defined(CONFIG_MODVERSIONS) && !defined(MODVERSIONS)
#  define MODVERSIONS /* force it on */
#endif

#ifdef MODVERSIONS
#  include <linux/modversions.h>
#endif

#include <linux/module.h>
#include <linux/version.h>

/* This depends on kmod.h, refuse to go on with 2.0 */
#if LINUX_VERSION_CODE < 0x020200
#  error "This module needs kmod, so it won't run with 2.0"
#else

#include <linux/sched.h>
#include <linux/kernel.h> /* printk() */
#include <linux/string.h>
#include <linux/init.h>

#ifdef HAVE_INTER_MODULE                
static const char *ime_string = NULL;
static void master_test_inter();
#endif                                  


#include <linux/kmod.h>
#include "sysdep.h"


int master_init_module(void)
{
    int r[2]; /* results */
    
    r[0]=request_module("slave");
    r[1]=request_module("nonexistent");
    printk(KERN_INFO "master: loading results are %i, %i\n", r[0],r[1]);
#ifdef HAVE_INTER_MODULE                
    master_test_inter();		
#endif                                  
    return 0; /* success */
}

#ifndef HAVE_INTER_MODULE                
void master_cleanup_module(void)
{ }
#endif                                  



#ifdef HAVE_INTER_MODULE

void master_test_inter()
{
    void (*ime_func)();
    ime_string = inter_module_get_request("ime_string", "inter");
    if (ime_string)
        printk(KERN_INFO "master: got ime_string '%s'\n", ime_string);
    else
        printk(KERN_INFO "master: inter_module_get failed");
    ime_func = inter_module_get("ime_function");
    if (ime_func) {
	(*ime_func)("master");
	inter_module_put("ime_function");
    }
}	

void master_cleanup_module(void)
{
    if (ime_string)
        inter_module_put("ime_string");
}
#endif

module_init(master_init_module);
module_exit(master_cleanup_module);

#endif /* no 2.0 allowed */
