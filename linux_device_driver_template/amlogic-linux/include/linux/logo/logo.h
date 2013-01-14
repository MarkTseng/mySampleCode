#ifndef LOGO_H
#define LOGO_H
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/ctype.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <mach/am_regs.h>
#include <linux/ioport.h>
#include <linux/device.h>
#include <linux/osd/osd.h>
#include <linux/osd/osd_hw.h>
#include <linux/vout/vinfo.h>
#include <linux/vout/vout_notify.h>
#include "err_code.h"
#include "logo_base_type.h"
#include "logo_dev.h"
#include "logo_parser.h"


#undef  MODULE
/**************************************************************
************************ type define part ***************************
***************************************************************/
//loader 






/**************************************************************
************************ macro define part ************************
***************************************************************/
extern	logo_object_t  aml_logo; 
#define   TOLOWER(x) ((x) | 0x20)

#define	LOGO_NAME	"aml_logo"
#define   dev_to_platformdev(dev)   (container_of((dev), struct platform_device,dev) )
extern struct bus_type platform_bus_type;
  	


/***************************************************************
*********************  para define part  *****************************
****************************************************************/






/**************************************************************
************************ macro define part ************************
***************************************************************/

extern  logo_object_t* get_current_logo_obj(void);
extern int  register_logo_parser(logo_parser_t* logo_parser) ;
extern int  register_logo_output_dev(logo_output_dev_t* new_dev);


#endif
