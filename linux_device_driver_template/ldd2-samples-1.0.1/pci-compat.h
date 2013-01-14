
/* This header only makes sense when included in a 2.0 compile */

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
 */

#ifndef _PCI_COMPAT_H_
#define _PCI_COMPAT_H_

#ifdef __KERNEL__

/*
 * This only makes sense if <linux/pci.h> is already included, *and*
 * we are using 2.0.
*/
#if defined(LINUX_PCI_H) && (LINUX_VERSION_CODE & 0xffff00) == 0x020000

#include <linux/bios32.h> /* pcibios_* */
#include <linux/malloc.h> /* kmalloc */

/* fake the new pci interface based on the old one: encapsulate bus/devfn */
struct pci_fake_dev {
  int index;
  unsigned short vendor, device;
  void *driver_data; /* net i.f. drivers make it point to net_device */
  u8 bus;
  u8 devfn;
};
#define pci_dev pci_fake_dev /* the other pci_dev is unused by 2.0 drivers */


#ifndef PCI_HEADER_TYPE_NORMAL /* These definitions are missing from 2.0 */
#  define PCI_HEADER_TYPE_NORMAL 0
#  define PCI_HEADER_TYPE_BRIDGE 1
#  define   PCI_PRIMARY_BUS         0x18    /* Primary bus number */
#  define   PCI_SECONDARY_BUS       0x19    /* Secondary bus number */
#  define   PCI_SUBORDINATE_BUS     0x1a    /* Highest bus behind the bridge */
#  define PCI_HEADER_TYPE_CARDBUS 2
#  define   PCI_CB_PRIMARY_BUS      0x18    /* PCI bus number */
#  define   PCI_CB_CARD_BUS         0x19    /* CardBus bus number */
#  define   PCI_CB_SUBORDINATE_BUS  0x1a    /* Subordinate bus number */
#endif

extern inline struct pci_dev *pci_find_device(unsigned int vendorid,
					      unsigned int devid,
					      struct pci_dev *from)
{
    struct pci_dev *pptr = kmalloc(sizeof(*pptr), GFP_KERNEL);
    int index = 0;
    int ret;

    if (!pptr) return NULL;
    if (from) index = from->index + 1;
    pptr->index = index;
    ret = pcibios_find_device(vendorid, devid, index,
			      &pptr->bus, &pptr->devfn);
    if (ret) { kfree(pptr); return NULL; }
    /* fill other fields */
    pcibios_read_config_word(pptr->bus, pptr->devfn,
			     PCI_VENDOR_ID, &pptr->vendor);
    pcibios_read_config_word(pptr->bus, pptr->devfn,
			     PCI_DEVICE_ID, &pptr->device);
    return pptr;
}

#if 0
/* this used to be only the base class, Hmm... better not offer it*/
extern inline struct pci_dev *pci_find_class(unsigned int class,
					     struct pci_dev *from)
{
    struct pci_dev *pptr = kmalloc(sizeof(*pptr), GFP_KERNEL);
    int index = 0;
    int ret;

    if (!pptr) return NULL;
    if (from) index = from->index + 1;
    pptr->index = index;
    ret = pcibios_find_class(class, index,
			      &pptr->bus, &pptr->devfn);
    if (ret) { kfree(pptr); return NULL; }
    /* fill other fields */
    pcibios_read_config_word(pptr->bus, pptr->devfn,
			     PCI_VENDOR_ID, &pptr->vendor);
    pcibios_read_config_word(pptr->bus, pptr->devfn,
			     PCI_DEVICE_ID, &pptr->device);
    return pptr;
}
#endif

/* this is used by pciregions instead */
extern inline struct pci_dev *pci_find_slot (unsigned int bus,
					     unsigned int devfn)
{
    struct pci_dev *pptr = kmalloc(sizeof(*pptr), GFP_KERNEL);
    int index = 0;
    unsigned short vendor;
    int ret;

    if (!pptr) return NULL;
    pptr->index = index; /* 0 */
    ret = pcibios_read_config_word(bus, devfn, PCI_VENDOR_ID, &vendor);
    if (ret /* == PCIBIOS_DEVICE_NOT_FOUND or whatever error */
	|| vendor==0xffff || vendor==0x0000) {
        kfree(pptr); return NULL;
    }
    printk("ok (%i, %i %x)\n", bus, devfn, vendor);
    /* fill other fields */
    pptr->bus = bus;
    pptr->devfn = devfn;
    pcibios_read_config_word(pptr->bus, pptr->devfn,
			     PCI_VENDOR_ID, &pptr->vendor);
    pcibios_read_config_word(pptr->bus, pptr->devfn,
			     PCI_DEVICE_ID, &pptr->device);
    return pptr;
}



/* this is not used in the real (2.2, 2.4) implementation, but we need it */
extern inline void pci_release_device(struct pci_dev *dev)
{
    kfree(dev);
}

/* struct pci_dev *pci_find_slot (unsigned int bus, unsigned int devfn); */

#define pci_present pcibios_present

extern inline int
pci_read_config_byte(struct pci_dev *dev, u8 where, u8 *val)
{
    return pcibios_read_config_byte(dev->bus, dev->devfn, where, val);
}

extern inline int
pci_read_config_word(struct pci_dev *dev, u8 where, u16 *val)
{
    return pcibios_read_config_word(dev->bus, dev->devfn, where, val);
}

extern inline int
pci_read_config_dword(struct pci_dev *dev, u8 where, u32 *val)
{
    return pcibios_read_config_dword(dev->bus, dev->devfn, where, val);
}

extern inline int
pci_write_config_byte(struct pci_dev *dev, u8 where, u8 val)
{
    return pcibios_write_config_byte(dev->bus, dev->devfn, where, val);
}

extern inline int
pci_write_config_word(struct pci_dev *dev, u8 where, u16 val)
{
    return pcibios_write_config_word(dev->bus, dev->devfn, where, val);
}

extern inline int
pci_write_config_dword(struct pci_dev *dev, u8 where, u32 val)
{
    return pcibios_write_config_dword(dev->bus, dev->devfn, where, val);
}

extern inline void pci_set_master(struct pci_dev *dev)
{
    u16 cmd;
    pcibios_read_config_word(dev->bus, dev->devfn, PCI_COMMAND, &cmd);
    cmd |= PCI_COMMAND_MASTER;
    pcibios_write_config_word(dev->bus, dev->devfn, PCI_COMMAND, cmd);
}

#endif /* version 2.0 and pci.h included */
#endif /* __KERNEL__ */
#endif /* _PCI_COMPAT_H_ */
