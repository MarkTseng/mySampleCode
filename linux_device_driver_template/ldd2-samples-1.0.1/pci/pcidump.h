/*
 * This header declares a table to decode the first 64 bytes od cfg space
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

#include <linux/pci.h>

struct pcidump_info {
    int offset, size; /* the data item to retrieve */
    int bitmask;
    int bool; /* true or false */
    char *string;
}

struct pcidump_info dumpinfo[] = {
    {0, 2, ~0, 0, "Vendor id: %04x\n"},
    {2, 2, ~0, 0, "Device id: %04x\n"},
    {4, 2, PCI_COMMAND_IO, 1, "I/O space enabled: %c\n"},
    {0, 0, 0, 0, NULL}
};

#if 0
/*
 * Under PCI, each device has 256 bytes of configuration address space,
 * of which the first 64 bytes are standardized as follows:
 */
#define PCI_VENDOR_ID           0x00    /* 16 bits */
#define PCI_DEVICE_ID           0x02    /* 16 bits */
#define PCI_COMMAND             0x04    /* 16 bits */
#define  PCI_COMMAND_IO         0x1     /* Enable response in I/O space */
#define  PCI_COMMAND_MEMORY     0x2     /* Enable response in Memory space */
#define  PCI_COMMAND_MASTER     0x4     /* Enable bus mastering */
#define  PCI_COMMAND_SPECIAL    0x8     /* Enable response to special cycles */
#define  PCI_COMMAND_INVALIDATE 0x10    /* Use memory write and invalidate */
#define  PCI_COMMAND_VGA_PALETTE 0x20   /* Enable palette snooping */
#define  PCI_COMMAND_PARITY     0x40    /* Enable parity checking */
#define  PCI_COMMAND_WAIT       0x80    /* Enable address/data stepping */
#define  PCI_COMMAND_SERR       0x100   /* Enable SERR */
#define  PCI_COMMAND_FAST_BACK  0x200   /* Enable back-to-back writes */

#define PCI_STATUS              0x06    /* 16 bits */
#define  PCI_STATUS_66MHZ       0x20    /* Support 66 Mhz PCI 2.1 bus */
#define  PCI_STATUS_UDF         0x40    /* Support User Definable Features */

#define  PCI_STATUS_FAST_BACK   0x80    /* Accept fast-back to back */
#define  PCI_STATUS_PARITY      0x100   /* Detected parity error */
#define  PCI_STATUS_DEVSEL_MASK 0x600   /* DEVSEL timing */
#define  PCI_STATUS_DEVSEL_FAST 0x000
#define  PCI_STATUS_DEVSEL_MEDIUM 0x200
#define  PCI_STATUS_DEVSEL_SLOW 0x400
#define  PCI_STATUS_SIG_TARGET_ABORT 0x800 /* Set on target abort */
#define  PCI_STATUS_REC_TARGET_ABORT 0x1000 /* Master ack of " */
#define  PCI_STATUS_REC_MASTER_ABORT 0x2000 /* Set on master abort */
#define  PCI_STATUS_SIG_SYSTEM_ERROR 0x4000 /* Set when we drive SERR */
#define  PCI_STATUS_DETECTED_PARITY 0x8000 /* Set on parity error */

#define PCI_CLASS_REVISION      0x08    /* High 24 bits are class, low 8
                                           revision */
#define PCI_REVISION_ID         0x08    /* Revision ID */
#define PCI_CLASS_PROG          0x09    /* Reg. Level Programming Interface */
#define PCI_CLASS_DEVICE        0x0a    /* Device class */

#define PCI_CACHE_LINE_SIZE     0x0c    /* 8 bits */
#define PCI_LATENCY_TIMER       0x0d    /* 8 bits */
#define PCI_HEADER_TYPE         0x0e    /* 8 bits */
#define PCI_BIST                0x0f    /* 8 bits */
#define PCI_BIST_CODE_MASK      0x0f    /* Return result */
#define PCI_BIST_START          0x40    /* 1 to start BIST, 2 secs or less */
#define PCI_BIST_CAPABLE        0x80    /* 1 if BIST capable */


#endif
