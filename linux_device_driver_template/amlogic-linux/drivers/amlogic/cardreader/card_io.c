#include <linux/slab.h>
#include <linux/cardreader/cardreader.h>
#include <linux/cardreader/card_block.h>
#include <linux/cardreader/sdio_hw.h>
#include "sd/sd_protocol.h"
#include <mach/am_regs.h>
#include <mach/irqs.h>
#include <mach/card_io.h>
    
int i_GPIO_timer;
int ATA_MASTER_DISABLED = 0;
int ATA_SLAVE_ENABLED = 0;
unsigned ATA_EIGHT_BIT_ENABLED = 1;
unsigned char card_share_ins_pwr_flag[MAX_CARD_UNIT];
unsigned sdio_timeout_int_times = 1;
unsigned sdio_timeout_int_num = 0;

unsigned sdio_command_err = SDIO_NONE_ERR;
unsigned sdio_command_int_num = 0;

struct completion sdio_int_complete;

/* */ void (*sd_mmc_power_register) (int power_on) = NULL;
/* */ int (*sd_mmc_ins_register) (void) = NULL;
/* */ int (*sd_mmc_wp_register) (void) = NULL;
/* */ void (*sd_mmc_io_release_register) (void) = NULL;
/**/ void (*cf_power_register) (int power_on) = NULL;
/**/ void (*cf_reset_register) (int reset_high) = NULL;
/**/ int (*cf_ins_register) (void) = NULL;
/**/ void (*cf_io_release_register) (void) = NULL;
/* */ void (*ms_mspro_power_register) (int power_on) = NULL;
/* */ int (*ms_mspro_ins_register) (void) = NULL;
/* */ int (*ms_mspro_wp_register) (void) = NULL;
/* */ void (*ms_mspro_io_release_register) (void) = NULL;

extern int using_sdxc_controller;

static void sdxc_open_host_interrupt(unsigned int_resource) 
{
	unsigned long sdxc_ctrl;
	SDXC_Ctrl_Reg_t *sdxc_ctrl_reg = (void *)&sdxc_ctrl;

	sdxc_ctrl = READ_CBUS_REG(SDHC_CTRL);

	switch (int_resource)
	{
		case SDIO_IF_INT :
			sdxc_ctrl_reg->sdio_dat1_int_en = 1;
			break;
			
		case SDIO_CMD_INT :
			sdxc_ctrl_reg->data_complete_int_en = 1;
			sdxc_ctrl_reg->res_ok_int_en = 1;
			break;
			
		case SDIO_TIMEOUT_INT:	//???
			sdxc_ctrl_reg->pack_crc_err_int_en = 1;
			sdxc_ctrl_reg->pack_timeout_int_en = 1;
			sdxc_ctrl_reg->res_crc_err_int_en = 1;
			sdxc_ctrl_reg->res_timeout_int_en = 1;
			sdio_command_err = SDIO_NONE_ERR;
			break;

		default :
			break;
	}

	WRITE_CBUS_REG(SDHC_CTRL, sdxc_ctrl);
}

static void sd_open_host_interrupt(unsigned int_resource) 
{	
	unsigned long irq_config, status_irq;	
	MSHW_IRQ_Config_Reg_t * irq_config_reg;	
	SDIO_Status_IRQ_Reg_t * status_irq_reg;	
	irq_config = READ_CBUS_REG(SDIO_IRQ_CONFIG);	
	irq_config_reg = (void *)&irq_config;
	status_irq = READ_CBUS_REG(SDIO_STATUS_IRQ);
	status_irq_reg = (void *)&status_irq;

	switch (int_resource) {
		case SDIO_IF_INT:	
			irq_config_reg->arc_if_int_en = 1;	
			status_irq_reg->if_int = 1;	
			break;

		case SDIO_CMD_INT:
			irq_config_reg->arc_cmd_int_en = 1;	
			status_irq_reg->cmd_int = 1;	
			break;

		case SDIO_SOFT_INT:	
			irq_config_reg->arc_soft_int_en = 1;	
			status_irq_reg->soft_int = 1;
			break;

		case SDIO_TIMEOUT_INT:	
			status_irq_reg->arc_timing_out_int_en = 1;	
			status_irq_reg->timing_out_int = 1;	
			sdio_timeout_int_num = 0;
			break;

		default:	
			break;	
	}

	WRITE_CBUS_REG(SDIO_STATUS_IRQ, status_irq);	
	WRITE_CBUS_REG(SDIO_IRQ_CONFIG, irq_config);
}

void sdio_open_host_interrupt(unsigned int_resource) 
{
	if (using_sdxc_controller)
		sdxc_open_host_interrupt(int_resource);
	else
		sd_open_host_interrupt(int_resource);
}


static void sdxc_clear_host_interrupt(unsigned int_resource) 
{
	unsigned long sdxc_status;
	SDXC_Status_Reg_t *sdxc_status_reg = (void *)&sdxc_status;

	sdxc_status = READ_CBUS_REG(SDHC_STAT);

	switch (int_resource)
	{
		case SDIO_IF_INT :
			sdxc_status_reg->dat0_ready_int = 1;
			break;
		case SDIO_CMD_INT :
			sdxc_status_reg->data_complete_int = 1;
			sdxc_status_reg->res_ok_int = 1;
			break;
		default :
			break;
	}

	WRITE_CBUS_REG(SDHC_CTRL, sdxc_status);
}

static void sd_clear_host_interrupt(unsigned int_resource) 
{
	unsigned long status_irq;
	SDIO_Status_IRQ_Reg_t * status_irq_reg;
	status_irq = READ_CBUS_REG(SDIO_STATUS_IRQ);
	status_irq_reg = (void *)&status_irq;

	switch (int_resource) {
		case SDIO_IF_INT:	
			status_irq_reg->if_int = 1;	
			break;
	

		case SDIO_CMD_INT:	
			status_irq_reg->cmd_int = 1;	
			break;
	
		case SDIO_SOFT_INT:	
			status_irq_reg->soft_int = 1;	
			break;
	

		case SDIO_TIMEOUT_INT:
			status_irq_reg->timing_out_int = 1;
			status_irq_reg->timing_out_count = 0x1FFF;	
			break;
	
		default:	
			break;	
	}
	
	WRITE_CBUS_REG(SDIO_STATUS_IRQ, status_irq);
}

void sdio_clear_host_interrupt(unsigned int_resource) 
{
	if (using_sdxc_controller)
		sdxc_clear_host_interrupt(int_resource);
	else
		sd_clear_host_interrupt(int_resource);
}


static void sdxc_close_host_interrupt(unsigned int_resource) 
{
	unsigned long sdxc_ctrl;
	SDXC_Ctrl_Reg_t *sdxc_ctrl_reg = (void *)&sdxc_ctrl;

	sdxc_ctrl = READ_CBUS_REG(SDHC_CTRL);

	switch (int_resource)
	{
		case SDIO_IF_INT :
			sdxc_ctrl_reg->sdio_dat1_int_en = 0;
			break;
			
		case SDIO_CMD_INT :
			sdxc_ctrl_reg->data_complete_int_en = 0;
			sdxc_ctrl_reg->res_ok_int_en = 0;
			break;

		case SDIO_TIMEOUT_INT:	//???
			sdxc_ctrl_reg->pack_crc_err_int_en = 0;
			sdxc_ctrl_reg->pack_timeout_int_en = 0;
			sdxc_ctrl_reg->res_crc_err_int_en = 0;
			sdxc_ctrl_reg->res_timeout_int_en = 0;
			break;

		default :
			break;
	}

	WRITE_CBUS_REG(SDHC_CTRL, sdxc_ctrl);
}

static void sd_close_host_interrupt(unsigned int_resource) 
{
	unsigned long irq_config, status_irq;
	MSHW_IRQ_Config_Reg_t * irq_config_reg;
	SDIO_Status_IRQ_Reg_t * status_irq_reg;
	irq_config = READ_CBUS_REG(SDIO_IRQ_CONFIG);
	status_irq = READ_CBUS_REG(SDIO_STATUS_IRQ);
	irq_config_reg = (void *)&irq_config;
	status_irq_reg = (void *)&status_irq;

	switch (int_resource) {
		case SDIO_IF_INT:	
			irq_config_reg->arc_if_int_en = 0;		
			status_irq_reg->if_int = 1;		
			break;
	
		case SDIO_CMD_INT:
			irq_config_reg->arc_cmd_int_en = 0;
			status_irq_reg->cmd_int = 1;	
			break;
	
		case SDIO_SOFT_INT:
			irq_config_reg->arc_soft_int_en = 0;
			status_irq_reg->soft_int = 1;
			break;
	
		case SDIO_TIMEOUT_INT:	
			status_irq_reg->arc_timing_out_int_en = 0;	
			status_irq_reg->timing_out_int = 1;	
			break;
	
		default:	
			break;	
	}

	WRITE_CBUS_REG(SDIO_IRQ_CONFIG, irq_config);
	WRITE_CBUS_REG(SDIO_STATUS_IRQ, status_irq);
}

void sdio_close_host_interrupt(unsigned int_resource) 
{
	if (using_sdxc_controller)
		sdxc_close_host_interrupt(int_resource);
	else
		sd_close_host_interrupt(int_resource);
}


static unsigned sdxc_check_interrupt(void) 
{
	unsigned long sdxc_status;
	SDXC_Status_Reg_t *sdxc_status_reg = (void *)&sdxc_status;

	unsigned long sdxc_ctrl;
	SDXC_Ctrl_Reg_t *sdxc_ctrl_reg = (void *)&sdxc_ctrl;

	unsigned ret = SDIO_NO_INT;

	sdxc_status = READ_CBUS_REG(SDHC_STAT);
	sdxc_ctrl = READ_CBUS_REG(SDHC_CTRL);

	if (sdxc_status_reg->res_ok_int) {
		sdxc_ctrl_reg->res_ok_int_en = 0;
		if (sdio_command_int_num > 0)
			--sdio_command_int_num;
		ret = SDIO_CMD_INT;		//sdxx
	}
	else if (sdxc_status_reg->data_complete_int) {
		sdxc_ctrl_reg->data_complete_int_en = 0;
		if (sdio_command_int_num > 0)
			--sdio_command_int_num;
		ret = SDIO_CMD_INT;		//sdxx
	}
	else if (sdxc_status_reg->sdio_dat1_int) {
		sdxc_ctrl_reg->sdio_dat1_int_en = 0;
		ret = SDIO_IF_INT;
	}
	/*
	else if (sdxc_status_reg->pack_crc_err_int || sdxc_status_reg->pack_timeout_int || 
		sdxc_status_reg->res_crc_err_int || sdxc_status_reg->res_timeout_int) {
		sdxc_ctrl_reg->pack_crc_err_int_en = 0;
		sdxc_ctrl_reg->pack_timeout_int_en = 0;
		sdxc_ctrl_reg->res_crc_err_int_en = 0;
		sdxc_ctrl_reg->res_timeout_int_en = 0;
		ret = SDIO_TIMEOUT_INT;
	}
	*/
	else if (sdxc_status_reg->pack_crc_err_int) {
		sdxc_ctrl_reg->pack_crc_err_int_en = 0;
		sdio_command_err = SDIO_PACK_CRC_ERR;
		ret = SDIO_TIMEOUT_INT;
	}
	else if (sdxc_status_reg->pack_timeout_int) {
		sdxc_ctrl_reg->pack_timeout_int_en = 0;
		sdio_command_err = SDIO_PACK_TIMEOUT_ERR;
		ret = SDIO_TIMEOUT_INT;
	}
	else if (sdxc_status_reg->res_crc_err_int) {
		sdxc_ctrl_reg->res_crc_err_int_en = 0;
		sdio_command_err = SDIO_RES_CRC_ERR;
		ret = SDIO_TIMEOUT_INT;
	}
	else if (sdxc_status_reg->res_timeout_int) {
		sdxc_ctrl_reg->res_timeout_int_en = 0;
		sdio_command_err = SDIO_RES_TIMEOUT_ERR;
		ret = SDIO_TIMEOUT_INT;
	}

	if (ret != SDIO_NO_INT)
	{
		WRITE_CBUS_REG(SDHC_STAT, sdxc_status);
		WRITE_CBUS_REG(SDHC_CTRL, sdxc_ctrl);
	}

	return ret;
}

static unsigned sd_check_interrupt(void) 
{
	unsigned long status_irq;
	SDIO_Status_IRQ_Reg_t * status_irq_reg;
	status_irq = READ_CBUS_REG(SDIO_STATUS_IRQ);
	status_irq_reg = (void *)&status_irq;
	
	if (status_irq_reg->cmd_int) {
		status_irq_reg->cmd_int = 1;
		status_irq_reg->arc_timing_out_int_en = 0;	
		status_irq_reg->timing_out_int = 1;	
		status_irq_reg->timing_out_count = 0;
		WRITE_CBUS_REG(SDIO_STATUS_IRQ, status_irq);	
		return SDIO_CMD_INT;	
	}
	else if (status_irq_reg->timing_out_int) {
		status_irq_reg->timing_out_int = 1;	
		status_irq_reg->timing_out_count = 0x1FFF;	
		WRITE_CBUS_REG(SDIO_STATUS_IRQ, status_irq);	
		return SDIO_TIMEOUT_INT;	
	}
	else if (status_irq_reg->if_int) {	
		/*close IF INT before clear if int, avoid IF INT twice*/
		sdio_close_host_interrupt(SDIO_IF_INT);
		status_irq_reg->if_int = 1;		
		WRITE_CBUS_REG(SDIO_STATUS_IRQ, status_irq);		
		return SDIO_IF_INT;		
	}
	else if (status_irq_reg->soft_int)
		return SDIO_SOFT_INT;
	else	
		return SDIO_NO_INT;
}

unsigned sdio_check_interrupt(void) 
{
	if (using_sdxc_controller)
		return sdxc_check_interrupt();
	else
		return sd_check_interrupt();
}


void sdio_if_int_handler(struct card_host *host) 
{
	sdio_irq_handled = 0;
	if (host->caps & CARD_CAP_SDIO_IRQ){
		//sdio_close_host_interrupt(SDIO_IF_INT);
		if(host->sdio_irq_thread)
			wake_up_process(host->sdio_irq_thread);
	}
	return;
} 

extern int sdxc_int_param;

void sdio_cmd_int_handle(struct memory_card *card) 
{
	if (using_sdxc_controller)	//sdxx
	{
		sdxc_int_param += 1;
		
		if (sdio_command_int_num == 0)
			complete(&sdio_int_complete);
	}
	else
	{
		sdio_timeout_int_num = 0;
		complete(&sdio_int_complete);
	}
	return;
} 

void sdio_timeout_int_handle(struct memory_card *card) 
{
	if (using_sdxc_controller)	//sdxx
	{
		sdxc_int_param += 100;
		
		complete(&sdio_int_complete);
	}
	else
	{
		if (card == NULL)
		{
			printk("sdio null\n");
			sdio_close_host_interrupt(SDIO_TIMEOUT_INT);
			complete(&sdio_int_complete);
			return;
		}

		card->card_io_init(card);
		card->card_detector(card);
		if ((card->card_status == CARD_REMOVED) || (++sdio_timeout_int_num >= sdio_timeout_int_times)) {
			sdio_close_host_interrupt(SDIO_TIMEOUT_INT);
			sdio_timeout_int_num = 0;
			sdio_timeout_int_times = 0;
			complete(&sdio_int_complete);		
		}
	}
	return;
}


