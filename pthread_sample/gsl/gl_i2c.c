#include "gsl.h"

#if 0
#define I2C_DEBUG(fmt, args...) diag_printf("Sunplus I2C debug: " fmt, ## args)
#else
#define I2C_DEBUG(fmt, args...)
#endif

#if (IC_VER == QAE377)
static int i2c_otp_do_once = 0;
#elif (IC_VER == QCE491)
#elif (IC_VER == QCE483)
#else
#error "Please define correct IC_VER!"
#endif

// turn on I2C hand-shake (do once time before setting AD/DA or MAC)
void GL_TurnONI2cOtp(void)
{
#if (IC_VER == QAE377)
	if (0 != i2c_otp_do_once) {
		return;
	}

    // turn on ACODEC_IF and OTP_IF
    HWREG_1(sft_cfg_1, 0x18);			// ONEHOT_B03 | ONEHOT_B04

    // de-assert IC9079's reset signal
    HWREG_1(sft_cfg_8, 0x08);			// ONEHOT_B03

    I2C_DEBUG("\nWait for OTP information finish\n");

	HWREG_1(reset[0], 0x02080000);
    HWREG_0(reset[0], ~0x02080000);
    while ((HWREG_R(cps_otp_cmd_st) & 0x0004) != 0x0004) {
		continue;
    }
    HWREG_W(cps_otp_cmd_st, 0);
    I2C_DEBUG("\nOTP information finished\n");

    i2c_otp_do_once = 1;
#elif (IC_VER == QCE491)
#elif (IC_VER == QCE483)
#else
#error "Please define correct IC_VER!"
#endif
}

int GL_ReadI2cReg(int offset)
{
#if (IC_VER == QAE377)
	HWREG_W(otp_i2c_macro_offset_addr, offset);
	HWREG_W(otp_i2c_macro_cmd, 1);

	while (HWREG_R(otp_i2c_macro_cmd) & 0x01) {
		continue;
	}

	return HWREG_R(otp_i2c_macro_data);
#elif (IC_VER == QCE491) || (IC_VER == QCE483)
	diag_printf("%s:%d: Not implemented yet!\n", __FUNCTION__, __LINE__);
	return 0;
#else
#error "Please define correct IC_VER!"
#endif
}

void GL_WriteI2cReg(int offset, int data)
{
#if (IC_VER == QAE377)
	HWREG_W(otp_i2c_macro_offset_addr, offset);
	HWREG_W(otp_i2c_macro_data, data);
	HWREG_W(otp_i2c_macro_cmd, 2);

	while (HWREG_R(otp_i2c_macro_cmd) & 0x02) {
		continue;
	}

	I2C_DEBUG("write_i2c_reg finished, offset: %x\n" , offset);
#elif (IC_VER == QCE491)
#elif (IC_VER == QCE483)
#else
#error "Please define correct IC_VER!"
#endif
}

