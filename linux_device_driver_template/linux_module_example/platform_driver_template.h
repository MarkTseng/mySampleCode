#ifndef __CRT_API__
#define __CRT_API__
#include <linux/spinlock.h>
#include <linux/hardirq.h>

/* rtk_crt char device data struct and macro  */
#ifndef CRT_MAJOR
#define CRT_MAJOR 0   /* dynamic major by default */
#endif

#ifndef CRT_NR_DEVS
#define CRT_NR_DEVS 1
#endif

#define CRT_NAME		"crt device"

/* ioctl cmd package struct  */
typedef struct crt_clk_onoff {
	void *private_data;
} crt_clk_onoff_cmd;

/*
 * Ioctl definitions
 */
#define CRT_IOC_MAGIC  'x'
#define CRT_IOC_CLK_ONOFF 		_IOW(CRT_IOC_MAGIC,  0, crt_clk_onoff_cmd)

#endif // __CRT_API__
