
/*
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
 *
 * This sample driver implements USB protocol decoding for both an USB
 * keyboard and an USB mouse. You can test it using either device, provided
 * they are not already managed by the official drivers. If you want
 * to only experiment with one device, pass the nokbd=1 or the
 * no_mouse=1 option to the command line of insmod
 */


#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/config.h>
#include <linux/module.h>

/* No USB with 2.0, make an explicit error and avoid strange ones */
#if LINUX_VERSION_CODE < 0x020200
#  error "This module needs kmod, so it won't run with 2.0"
#else


#include <linux/kernel.h>
#include <linux/malloc.h>
#include <linux/init.h>
#include <linux/usb.h>

/*
 * Note: <linux/usb.h> in 2.2 includes <linux/kcomp.h> that breaks
 * our sysdep.h . You can't disable kcomp.h from entering the game,
 * so "sysdep.h" can't be included here. If you write a backward-portable
 * driver with both USB and something-else support, you need to separate
 * the USB stuff in order not to rely on sysdep.h in USB-related  files
 */
#if 0
  #include "sysdep.h"
#else
  #include "usb-sysdep.h"
#endif

/*
 * We need a local data structure, as it must be allocated for each new
 * mouse device plugged in the USB bus
 */

struct sample_device {
    unsigned char data[8];   /* enough for keyboard and mouse protocols */
    char *name;              /* either "kdb" or "mouse" */
    struct urb urb;          /* USB Request block, to get USB data*/
    int maxp;                /* packet len */
    char output[80];         /* used for printk at irq time */
};


/*
 * Handler for data sent in by the device. The function is called by
 * the USB kernel subsystem whenever a device spits out new data
 */
static void sample_irq(struct urb *urb)
{
    struct sample_device *sample = urb->context;
    char *pos = sample->output;
    int i;

    if (urb->status != USB_ST_NOERROR) return;

    pos += sprintf(pos, "usbsample: data from %-8s =",
		   sample->name);
    for (i=0; i<sample->maxp; i++) {
	pos += sprintf(pos, " %02x", sample->data[i]);
    }
    printk(KERN_INFO "%s\n", sample->output);
}

/*
 * These two callbacks are invoked when an USB device is detached or attached
 * to the bus
 */

static void sample_disconnect(struct usb_device *udev, void *clientdata)
{
    /* the clientdata is the sample_device we passed originally */
    struct sample_device *sample = clientdata;

    /* remove the URB, remove the input device, free memory */
    usb_unlink_urb(&sample->urb);
    kfree(sample);
    printk(KERN_INFO "sample: USB %s disconnected\n", sample->name);

    /*
     * here you might MOD_DEC_USE_COUNT, but only if you increment
     * the count in sample_probe() below
     */
    return;
}

static void *sample_probe(struct usb_device *udev, unsigned int ifnum,
			  const struct usb_device_id *id)
{
    /*
     * The probe procedure is pretty standard. Device matching has already
     * been performed based on the id_table structure (defined later)
     */
    struct usb_interface *iface;
    struct usb_interface_descriptor *interface;
    struct usb_endpoint_descriptor *endpoint;
    struct sample_device *sample;

    printk(KERN_INFO "usbsample: probe called for %s device\n",
	   (char *)id->driver_info /* "mouse" or "keyboard" */ );

    iface = &udev->actconfig->interface[ifnum];
    interface = &iface->altsetting[iface->act_altsetting];

    if (interface->bNumEndpoints != 1) return NULL;

    endpoint = interface->endpoint + 0;
    if (!(endpoint->bEndpointAddress & 0x80)) return NULL;
    if ((endpoint->bmAttributes & 3) != 3) return NULL;

    usb_set_protocol(udev, interface->bInterfaceNumber, 0);
    usb_set_idle(udev, interface->bInterfaceNumber, 0, 0);

    /* allocate and zero a new data structure for the new device */
    sample = kmalloc(sizeof(struct sample_device), GFP_KERNEL);
    if (!sample) return NULL; /* failure */
    memset(sample, 0, sizeof(*sample));
    sample->name = (char *)id->driver_info;

    /* fill the URB data structure using the FILL_INT_URB macro */
    {
	int pipe = usb_rcvintpipe(udev, endpoint->bEndpointAddress);
	int maxp = usb_maxpacket(udev, pipe, usb_pipeout(pipe));
	
	if (maxp > 8) maxp = 8; sample->maxp = maxp; /* remember for later */
	FILL_INT_URB(&sample->urb, udev, pipe, sample->data, maxp,
		      sample_irq, sample, endpoint->bInterval);
    }

    /* register the URB within the USB subsystem */
    if (usb_submit_urb(&sample->urb)) {
	kfree(sample);
	return NULL;
    }
    /* announce yourself */
    printk(KERN_INFO "usbsample: probe successful for %s (maxp is %i)\n",
	   sample->name, sample->maxp);

    /*
     * here you might MOD_INC_USE_COUNT; if you do, you'll need to unplug
     * the device or the devices before being able to unload the module
     */

    /* and return the new structure */
    return sample;
}


/*
 * The id_table, lists all devices that can be handled by this driver.
 * The three numbers are class, subclass, protocol. <linux/usb.h> has
 * more details about interface mathces and vendor/device matches.
 * This feature is not there in version 2.2, see below, sample_probe_22()
 * for details. Here we use a fake usb_device_id structure defined in 
 * ./usb-sysdep.h
 */
static struct usb_device_id sample_id_table [] = {
    {
	USB_INTERFACE_INFO(3, 1, 1),
	driver_info: (unsigned long)"keyboard"
    },
    {
	USB_INTERFACE_INFO(3, 1, 2),
	driver_info: (unsigned long)"mouse"
    },
    {
	0, /* no more matches */
    }
};

/*
 * The callbacks are registered within the USB subsystem using the
 * usb_driver data structure
 */
#ifdef LINUX_24
static struct usb_driver sample_usb_driver = {
        name:        "sample",
        probe:       sample_probe,
        disconnect:  sample_disconnect,
        id_table:    sample_id_table,
};

#else /* 2.2 */

/*
 * With version 2.2, there is no device_id support: the probe function
 * is called for every device being plugged, and it must select whether
 * the device is going to be handled or not. Here we extract the identification
 * phase (based on class/subclass/protocol in this case) and rely on
 * sample_probe() above for the interesting part of the game. Note
 * that a 2.4 driver can use this approach as well, by not defining an
 * id table (and achieving a 2.2 and 2.4 source with less ifdef). We think
 * the id_table way is much cleaner, so we chose to exploit it where available
 */
static void *sample_probe_22(struct usb_device *udev, unsigned int ifnum)
{
    struct usb_device_id *id;
    struct usb_interface_descriptor *interface;

    printk(KERN_INFO "sample_probe_22 called\n");
    if (udev->descriptor.bNumConfigurations != 1) return NULL;
    interface = udev->config[0].interface[ifnum].altsetting;

    for (id = sample_id_table; id->driver_info; id++) {
        if (interface->bInterfaceClass !=    id->class)    continue;
        if (interface->bInterfaceSubClass != id->subclass) continue;
        if (interface->bInterfaceProtocol != id->protocol) continue;
	break; /* found */
    }
    if (!id->driver_info)
	return NULL; /* not ours */

    return sample_probe(udev, ifnum, id);
}

static struct usb_driver sample_usb_driver = {
        name:        "sample",
        probe:       sample_probe_22,
        disconnect:  sample_disconnect,
	/* no id_table field here */
};

#endif /* 2.2 */

/*
 * Functions called at module load and unload time: only register and
 * unregister the USB callbacks
 */
int sample_init(void)
{
    /* just register it, returns 0 or error code */
    return usb_register(&sample_usb_driver);
}

void sample_exit(void)
{
    usb_deregister(&sample_usb_driver);
}

module_init(sample_init);
module_exit(sample_exit);

#endif /* no 2.0 */
