/*
 * usb-sysdep.h -- macros to help using the 2.4 approach to usb in 2.2
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
 * $Id: usb-sysdep.h,v 1.3 2001/07/18 22:28:18 rubini Exp $
 */

#ifndef _USB_SYSDEP_H_
#define _USB_SYSDEP_H_


/*
 * provide the basic version check, don't bother with 2.0 as it
 * will have already barfed on <linux/usb.h>
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0) /* not > 2.4, by now */
#  error "This kernel is too recent: not supported by this file"
#endif
#if (LINUX_VERSION_CODE & 0xff00) == 3 /* not 2.3 */
#  error "Please don't use linux-2.3, use 2.4 instead"
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,3,0)
#  define LINUX_22
#else
#  define LINUX_24
#endif


#ifdef LINUX_22
/*
 * Define a device_id structure, to use the better approach of 2.4. This
 * for of the structure is used for class/subclass/protocol matching;
 * if you need to match on vendor/device identifiers you need a different
 * compatibility structure (and associated match code). Or you could
 * backport the one defined in linux-2.4/include/linux/usb.h
 */
struct usb_device_id {
    int 	    class;
    int 	    subclass;
    int 	    protocol;   
    unsigned long   driver_info;
};

/* This macro is used to fill the fields (2.4 is much more powerful here) */
#define USB_INTERFACE_INFO(cl,sc,pr) \
        class: (cl), subclass: (sc), protocol: (pr)

#endif /* 2.2 */

#endif /* _USB_SYSDEP_H_ */
