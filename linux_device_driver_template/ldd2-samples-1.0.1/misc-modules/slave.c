/*
 * slave.c -- the slave module of a master/slave pair using kmod
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

#include <linux/module.h>
#include "sysdep.h"

int slave_init(void)
{
	MOD_INC_USE_COUNT;
	MOD_DEC_USE_COUNT;  /* Make it look "used once" */
	return 0;
}

void slave_cleanup(void)
{}

module_init(slave_init);
module_exit(slave_cleanup);


