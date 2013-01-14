/*
 * pciregions.c --  a module that prints address regions of PCI devices
 *
 * $Id: pciregions.c,v 1.9 2001/03/16 11:30:23 rubini Exp $
 *
 * Copyright (C) 1997,2001   rubini@linux.it (Alessandro Rubini)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */


#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/config.h>
#include <linux/module.h>

#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/pci.h>

#include "sysdep.h"

static u32 addresses[] = {
    PCI_BASE_ADDRESS_0,
    PCI_BASE_ADDRESS_1,
    PCI_BASE_ADDRESS_2,
    PCI_BASE_ADDRESS_3,
    PCI_BASE_ADDRESS_4,
    PCI_BASE_ADDRESS_5,
    0
};

int pciregions_read_proc(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{
    int i, pos=0;
    int bus, devfn, is_multi = 0;
    unsigned char headertype, pribus, secbus;
    u16 vendorid, deviceid = 0;

    /* to print information about several buses, keep an array of them */
    #define MAXNBUS 8
    int buses[MAXNBUS] = {0,};
    int lastbus = 0; /* only one bus, by default, bus 0 */
    int busindex = 0;

    /* this macro helps keeping the following lines short */
    #define PRINTF(fmt, args...) sprintf(buf+len, fmt, ## args)
    len=0;

    /* Loop through the devices (code not printed in the book) */

    if (!pci_present())
        return sprintf(buf, "PCI not available in this computer\n");

    bus = buses[busindex]; /* first bus (bus 0) */
    for (devfn=0; pos < PAGE_SIZE; devfn++) {
	struct pci_dev *dev = NULL;
	/*
	 * A clean implementation
	 * would have a separate function to dump a single bus, but i
	 * preferred to keep it in one function to include part of it
	 * in the book (the printed code is automagically extracted from
	 * this file).
	 *
	 * Instead, I use a dirty trick to fold two loops in one.
	 */
	if (devfn > 0xff) { /* end of this bus */
	    if (busindex == lastbus) break; 
	    /* loop over to the next bus */
	    bus = buses[++busindex];
	    devfn = 0;
	}

	/*
	 * This code is derived from "drivers/pci/pci.c" in version
	 * 2.0, although it has been modified to work with the 2.4 interface.
	 * This means that the GPL applies to this source file
	 * and credit is due to the original authors
	 * (Drew Eckhardt, Frederic Potter, David Mosberger-Tang)
	 */

        if (PCI_FUNC(devfn) && !is_multi) /* not multi-function */
	    continue;

	dev = pci_find_slot(bus, devfn);
	if (!dev) {
	    if (!PCI_FUNC(devfn)) is_multi = 0; /* no first implies no other */
	    continue; /* no such device */
	}
	pci_read_config_byte(dev, PCI_HEADER_TYPE, &headertype);
        if (!PCI_FUNC(devfn)) /* first function */
	    is_multi = headertype & 0x80;
	headertype &= 0x7f; /* mask multi-function bit */

	/* FIXME: should get rid of the PAGE_SIZE limit */
        if (len > PAGE_SIZE / 2) { /* a big margin, just to be sure */
	    *eof = 1; return len;
	}
	vendorid = dev->vendor;
	deviceid = dev->device;

        len += PRINTF("Bus %i, device %2i, devfn %2i (id %04x-%04x,"
		      " headertype 0x%02x)\n",
		      bus, devfn>>3, devfn & 7,
		      vendorid, deviceid, headertype);
	if (headertype == PCI_HEADER_TYPE_BRIDGE) {
	    /* This is a bridge, print what it does */
	    pci_read_config_byte(dev, PCI_PRIMARY_BUS, &pribus);
	    pci_read_config_byte(dev, PCI_SECONDARY_BUS, &secbus);
	    len += PRINTF("\tbridge connecting PCI bus %i to PCI bus %i\n",
			  secbus, pribus);
	    /* remember about this bus, to dump it later */
	    if (lastbus <= MAXNBUS-1) {
		lastbus++;
		buses[lastbus] = secbus;
		len += PRINTF("\t(bus %i is dumped below)\n", secbus);
	    } else {
		len += PRINTF("\t(bus %i won't be dumped)\n", secbus);
	    }
	    pci_release_device(dev); /* 2.0 compatibility */
	    continue;
	} else if (headertype == PCI_HEADER_TYPE_CARDBUS) {
	    /* This is a CardBus bridge, print what it does */
	    pci_read_config_byte(dev, PCI_CB_PRIMARY_BUS,&pribus);
	    pci_read_config_byte(dev, PCI_CB_CARD_BUS,&secbus);
	    len += PRINTF("\tbridge connecting CardBus %i to PCI bus %i\n",
			  secbus, pribus);
	    pci_release_device(dev); /* 2.0 compatibility */
	    continue;
	} else if (headertype != PCI_HEADER_TYPE_NORMAL) {
	    len += PRINTF("\tunknown header type, skipping\n");
	    pci_release_device(dev); /* 2.0 compatibility */
	    continue;
	}
		
        /* Print the address regions of this device */
        for (i=0; addresses[i]; i++) {
            u32 curr, mask, size;
            char *type;

            pci_read_config_dword(dev, addresses[i],&curr);
            cli();
            pci_write_config_dword(dev, addresses[i],~0);
            pci_read_config_dword(dev, addresses[i],&mask);
            pci_write_config_dword(dev, addresses[i],curr);
            sti();

            if (!mask)
		continue; /* there may be other regions */

	    /*
	     * apply the I/O or memory mask to current position
	     * note that I/O is limited to 0xffff, and 64-bit is not
	     * supported by this simple imeplementation
	     */
	    if (curr & PCI_BASE_ADDRESS_SPACE_IO) {
		curr &= PCI_BASE_ADDRESS_IO_MASK;
	    } else {
		curr &= PCI_BASE_ADDRESS_MEM_MASK;
	    }
		
            len += PRINTF("\tregion %i: mask 0x%08lx, now at 0x%08lx\n", i,
                           (unsigned long)mask,
                           (unsigned long)curr);
            /* extract the type, and the programmable bits */
            if (mask & PCI_BASE_ADDRESS_SPACE_IO) {
                type = "I/O"; mask &= PCI_BASE_ADDRESS_IO_MASK;
		size = (~mask + 1) & 0xffff; /* Bleah */
            } else {
                type = "mem"; mask &= PCI_BASE_ADDRESS_MEM_MASK;
		size = ~mask + 1;
            }
            len += PRINTF("\tregion %i: type %s, size %i (%i%s)\n", i,
                          type, size,
			  (size & 0xfffff) == 0 ? size >> 20 :
			    (size & 0x3ff) == 0 ? size >> 10 : size,
			  (size & 0xfffff) == 0 ? "MB" :
			    (size & 0x3ff) == 0 ? "KB" : "B");
	    if (len > PAGE_SIZE / 2) {
		len += PRINTF("... more info skipped ...\n");
		*eof = 1; return len;
	    }
        }
	pci_release_device(dev); /* 2.0 compatibility */
    } /* devfn */
    *eof = 1;
    return len;
}

#ifdef USE_PROC_REGISTER

static int pciregions_old_read_proc(char *buf, char **start, off_t offset,
				 int len, int unused)
{
    int eof;
    return pciregions_read_proc(buf, start, offset, len, &eof, NULL);
}


struct proc_dir_entry pciregions_proc_entry = {
        0,                 /* low_ino: the inode -- dynamic */
       10, "pciregions",   /* len of name and name */
        S_IFREG | S_IRUGO, /* mode */
        1, 0, 0,           /* nlinks, owner, group */
        0, NULL,           /* size - unused; operations -- use default */
        &pciregions_old_read_proc,   /* function used to read data */
        /* nothing more */
    };

#endif /* USE_PROC_REGISTER */


int init_module(void)
{
#ifdef USE_PROC_REGISTER
    proc_register_dynamic(&proc_root, &pciregions_proc_entry);
#else
    create_proc_read_entry("pciregions", 0, NULL, pciregions_read_proc, NULL);
#endif
    return 0;
}

void cleanup_module(void)
{
#ifdef USE_PROC_REGISTER
    proc_unregister(&proc_root, pciregions_proc_entry.low_ino);
#else
    remove_proc_entry("pciregions", 0);
#endif
}

