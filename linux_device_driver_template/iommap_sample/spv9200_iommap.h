/* DEBUG message define */
#ifndef DEBUG
#define DEBUG 1
#endif

#ifdef DEBUG
#define IOMMAP_DEBUGMSG(string, args...) printk(KERN_ALERT "IOMMAP: " string, ##args)
#else
#define IOMMAP_DEBUGMSG(string, args...)
#endif

/*iommap device structure */
typedef struct IOMMAP_DEV {
	void __iomem *virtual_mem_base;
	unsigned long virtual_size;	
	unsigned long phys_addr;
	struct resource mem;
} IOMMAP_DEV;


/* char device info */
#define IOMMAP_MAJOR			220
#define IOMMAP_DEVICE_NAME		"iommap"


/* define to use readb and writeb to read/write data */
#define IOMMAP_BYTE_WISE



/*
 * Ioctl definitions
 */

/* Use 'I' as magic number of iommap device */
#define IOMMAP_IOC_MAGIC  'I'

#define IOMMAP_GET						_IOR(IOMMAP_IOC_MAGIC, 0, IOMMAP_DEV)
#define IOMMAP_SET						_IOW(IOMMAP_IOC_MAGIC, 1, IOMMAP_DEV)

