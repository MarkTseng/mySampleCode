/*
 * pcidump.c --  a tool to decode /proc/pcidata
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
 * $Id: pcidump.c,v 1.5 2001/07/18 22:28:16 rubini Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <linux/pci.h>
#include <asm/page.h>

#include <linux/version.h>

/* A structure to declare how data is decoded */
struct pcidump_info {
    int offset; /* the data item to retrieve */
    unsigned long bitmask;
    int bool; /* true or false */
    char *string;
};

#define ONEBYTE 0xFF
#define TWOBYTES 0xFFFF
#define FOURBYTES 0xFFFFFFFF

struct pcidump_info dumpinfo[] = {
    {0,                    0,                       0, "\tCompulsory registers:\n"},
    {PCI_VENDOR_ID,        TWOBYTES,                0, "Vendor id: %04x\n"},
    {PCI_DEVICE_ID,        TWOBYTES,                0, "Device id: %04x\n"},
    {PCI_COMMAND,          PCI_COMMAND_IO,          1, "I/O space enabled: %c\n"},
    {PCI_COMMAND,          PCI_COMMAND_MEMORY,      1, "Memory enabled: %c\n"},
    {PCI_COMMAND,          PCI_COMMAND_MASTER,      1, "Master enabled: %c\n"},
    {PCI_COMMAND,          PCI_COMMAND_SPECIAL,     1, "Special command enabled: %c\n"},
    {PCI_COMMAND,          PCI_COMMAND_INVALIDATE,  1, "Write-invalidate enabled: %c\n"},
    {PCI_COMMAND,          PCI_COMMAND_VGA_PALETTE, 1, "Palette-snoop enabled: %c\n"},
    {PCI_COMMAND,          PCI_COMMAND_PARITY,      1, "Parity enabled: %c\n"},
    {PCI_COMMAND,          PCI_COMMAND_WAIT,        1, "Wait-cycle enabled: %c\n"},
    {PCI_COMMAND,          PCI_COMMAND_SERR,        1, "System-error enabled: %c\n"},
    {PCI_COMMAND,          PCI_COMMAND_FAST_BACK,   1, "Fast-back-to-back enabled: %c\n"},
    {PCI_STATUS,           PCI_STATUS_66MHZ,        1, "Can do 66MHz: %c\n"},
    {PCI_STATUS,           PCI_STATUS_UDF,          1, "Supports User Defined Features: %c\n"},
    {PCI_STATUS,           PCI_STATUS_FAST_BACK,    1, "Fast-back-to-back capable: %c\n"},
    {PCI_STATUS,           PCI_STATUS_PARITY,        1, "Data Parity Reported: %c\n"},
    {PCI_STATUS,           PCI_STATUS_DEVSEL_MASK,    0, "Device Select Timing bits are %x\n"},
    {PCI_STATUS,           PCI_STATUS_SIG_TARGET_ABORT,1, "Signalled abort as target: %c\n"},
    {PCI_STATUS,           PCI_STATUS_REC_TARGET_ABORT, 1, "Received abort from target: %c\n"},
    {PCI_STATUS,           PCI_STATUS_REC_MASTER_ABORT, 1, "Aborted transaction as master: %c\n"},
    {PCI_STATUS,           PCI_STATUS_SIG_SYSTEM_ERROR, 1, "Did send a system error: %c\n"},
    {PCI_STATUS,           PCI_STATUS_DETECTED_PARITY, 1, "Detected a parity error: %c\n"},
    {PCI_REVISION_ID,      ONEBYTE,                   0, "Revision id (decimal): %i\n"},
    {PCI_CLASS_PROG,       ONEBYTE,                  0, "Programmer Interface: %02x\n"},
    {PCI_CLASS_DEVICE,     TWOBYTES,                0, "Class of device: %04x\n"},
    {PCI_HEADER_TYPE,      0x7f,                    0, "Header type: %02x\n"},
    {PCI_HEADER_TYPE,      0x80,                    1, "Multi function device: %c\n"},
    {0,                    0,                       0, "\tOptional registers:\n"},
    {PCI_CACHE_LINE_SIZE,  ONEBYTE,                 0, "Cache line size (decimal): %i\n"},
    {PCI_LATENCY_TIMER,    ONEBYTE,                 0, "Latency timer (decimal): %i\n"},
    {PCI_BIST,             PCI_BIST_CAPABLE,        1, "Is Built-In-Self-Test available: %c\n"},
    {PCI_BIST,             PCI_BIST_CODE_MASK,      1, "Did Built-In-Self-Test fail: %c\n"},
    {PCI_BASE_ADDRESS_0, PCI_BASE_ADDRESS_MEM_MASK,  0, "Base Address 0: %08x\n"},
    {PCI_BASE_ADDRESS_0, PCI_BASE_ADDRESS_SPACE,      1, "Base Address 0 Is I/O: %c\n"},
    {PCI_BASE_ADDRESS_0, PCI_BASE_ADDRESS_MEM_TYPE_64, 1, "Base Address 0 is 64-bits: %c\n"},
    {PCI_BASE_ADDRESS_0, PCI_BASE_ADDRESS_MEM_TYPE_1M,  1, "Base Address 0 is below-1M: %c\n"},
    {PCI_BASE_ADDRESS_0, PCI_BASE_ADDRESS_MEM_PREFETCH, 1, "Base Address 0 is prefetchable: %c\n"},
    {PCI_BASE_ADDRESS_1, PCI_BASE_ADDRESS_MEM_MASK,     0, "Base Address 1: %08x\n"},
    {PCI_BASE_ADDRESS_1, PCI_BASE_ADDRESS_SPACE,        1, "Base Address 1 Is I/O: %c\n"},
    {PCI_BASE_ADDRESS_1, PCI_BASE_ADDRESS_MEM_TYPE_64,  1, "Base Address 1 is 64-bits: %c\n"},
    {PCI_BASE_ADDRESS_1, PCI_BASE_ADDRESS_MEM_TYPE_1M,  1, "Base Address 1 is below-1M: %c\n"},
    {PCI_BASE_ADDRESS_1, PCI_BASE_ADDRESS_MEM_PREFETCH, 1, "Base Address 1 is prefetchable: %c\n"},

#if 0
#define PCI_BASE_ADDRESS_2      0x18    /* 32 bits */
#define PCI_BASE_ADDRESS_3      0x1c    /* 32 bits */
#define PCI_BASE_ADDRESS_4      0x20    /* 32 bits */
#define PCI_BASE_ADDRESS_5      0x24    /* 32 bits */
#define PCI_CARDBUS_CIS         0x28
#endif

    {PCI_SUBSYSTEM_ID,        TWOBYTES,                0, "Subsystem id: %04x\n"},
    {PCI_SUBSYSTEM_VENDOR_ID, TWOBYTES,                0, "Subsystem vendor: %04x\n"},
    {PCI_ROM_ADDRESS,         ~0x7FF,                  0, "Rom address: %08x\n"},
    {PCI_ROM_ADDRESS,         PCI_ROM_ADDRESS_ENABLE,  1, "Rom is enabled: %c\n"},
    {PCI_INTERRUPT_PIN,       ONEBYTE,                 1, "Does generate interrupts: %c\n"},
    {PCI_INTERRUPT_LINE,      ONEBYTE,                 0, "Interrupt line (decimal): %i\n"},
    {PCI_INTERRUPT_PIN,       ONEBYTE,                 0, "Interrupt pin (decimal): %i\n"},
    {PCI_MIN_GNT,             ONEBYTE,                 0, "Min bus grant time (decimal): %i\n"},
    {PCI_MAX_LAT,             ONEBYTE,                 0, "Max bus latency acceptable (decimal): %i\n"},

    {0x00,                 0,                       0, NULL}
};



unsigned char buffer[PAGE_SIZE];

int main(int argc, char **argv)
{
    int fd, len;
    char *fname="/proc/pcidata";
    
    unsigned char *curr, *end;

    if (argc > 2 || (argc == 2 && argv[1][0] == '-')) { 
        fprintf(stderr,"%s: Usage: \"%s <filename>\"\n"
                "\tfilename should embed binary pci configuration data\n"
                "\tif filename is missing, %s is used\n",
                argv[0], argv[0], fname);
        exit(1);
    }
    if (argc>1) {
	fname = argv[1];
    }
    fd=open(fname, O_RDONLY);
    if (fd<0) {
	fprintf(stderr,"%s: %s: %s\n", argv[0], fname, strerror(errno));
	exit(1);
    }

    switch (len=read(fd,buffer,PAGE_SIZE)) {
      case 0:
        fprintf(stderr,"%s: %s: no data\n", argv[0], fname);
        exit(1);
      case -1:
        fprintf(stderr,"%s: %s: %s\n", argv[0], fname, strerror(errno));
        exit(1);
      default: break;
    }
    if (len < 256) {
        buffer[len]='\0';
        fprintf(stderr," %s: %s: %s\n", argv[0], fname, buffer);
        exit(1);
    }
    if (len % 256) {
        fprintf(stderr," %s: %s: incorrect data size: %i\n", 
		argv[0], fname, len);
        exit(1);
    }

    for (end = buffer+len, curr = buffer; curr < end; curr += 256) {
        struct pcidump_info *ptr;
        unsigned int datum;

        for (ptr = dumpinfo; ptr->string; ptr++) {
            /*
             * Perform a little-endian read of the item
             */
            datum = curr[ptr->offset]
                | (curr[ptr->offset+1]<<8)
                    | (curr[ptr->offset+2]<<16)
                        | (curr[ptr->offset+3]<<24);
            datum &= ptr->bitmask;
            printf(ptr->string, ptr->bool ? (datum ? 'y' : 'n') : datum);
        }
        printf("\n");
    }
    return 0;
}
