/*
 * pcidata.c --  a module that dumps the cfg registers through /proc
 *
 * $Id: pcidata.c,v 1.8 2001/03/16 21:04:49 rubini Exp $
 *
 * Copyright (C) 1997,2000   rubini@linux.it (Alessandro Rubini)
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
#include <linux/proc_fs.h>
#include <linux/pci.h>

#include "sysdep.h"


/*
 * This simple module offers in a /proc file the binary
 * configuration data of available PCI devices.
 * Each device has 256 bytes of cfg data: 16 devices
 * can fit in a page (32 on the alpha).
 * FIXME: page size is no longer an issue with 2.2/2.4
 */

int pcidata_read_proc(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{
    int i, pos=0;
    int bus, devfn;

    if (!pcibios_present())
        return sprintf(buf,"No PCI bios present\n");

    /*
     * This code is derived from "drivers/pci/pci.c". This means that
     * the GPL applies to this source file and credit is due to the
     * original authors (Drew Eckhardt, Frederic Potter, David
     * Mosberger-Tang)
     */
    for (bus=0; !bus; bus++) { /* only bus 0 :-) */
        for (devfn=0; devfn < 0x100 && pos < PAGE_SIZE/2; devfn++) {
	    struct pci_dev *dev = NULL;

	    dev = pci_find_slot(bus, devfn);
	    if (!dev) continue;

            /* Ok, we've found a device, copy its cfg space to the buffer*/
            for (i=0; i<256; i += sizeof(u32), pos += sizeof(u32))
                pci_read_config_dword(dev,i,(u32 *)(buf+pos));
	    pci_release_device(dev); /* 2.0 compatibility */
        }
    }
    *eof=1;
    return pos;
}

#ifdef USE_PROC_REGISTER

static int pcidata_old_read_proc(char *buf, char **start, off_t offset,
				 int len, int unused)
{
    int eof;
    return pcidata_read_proc(buf, start, offset, len, &eof, NULL);
}


struct proc_dir_entry pcimod_proc_entry = {
        0,                 /* low_ino: the inode -- dynamic */
        7, "pcidata",      /* len of name and name */
        S_IFREG | S_IRUGO, /* mode */
        1, 0, 0,           /* nlinks, owner, group */
        0, NULL,           /* size - unused; operations -- use default */
        &pcidata_old_read_proc,   /* function used to read data */
        /* nothing more */
    };

#endif /* USE_PROC_REGISTER */

int init_module(void)
{
#ifdef USE_PROC_REGISTER
    proc_register_dynamic(&proc_root, &pcimod_proc_entry);
#else
    create_proc_read_entry("pcidata", 0, NULL, pcidata_read_proc, NULL);
#endif
    return 0;
}

void cleanup_module(void)
{
#ifdef USE_PROC_REGISTER
    proc_unregister(&proc_root, pcimod_proc_entry.low_ino);
#else
    remove_proc_entry("pcidata", 0);
#endif
    return;
}

