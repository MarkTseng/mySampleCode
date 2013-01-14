/***********************************************************************
 *  2009.07 / chuancheng.yang
 *  This file is all about audio harware initialization.
 *  include ADC/DAC/SPDIF/PLL...etc
 ***********************************************************************/
#include "include/aud_global.h"
//#include "stypes.h"
#include "include/aud_hw.h"
#include "include/audclk.h"
#include "include/auddrv.h"
#include "include/gpio_pin_map.h"

///#include <gsl.h>

#ifdef HW_SUPPORT_AUDHW_USE_DIGITAL_PLL
static void digital_plla_setting(int index);
#endif


/***********************************************************************
 *  Code Session
 ***********************************************************************/
void AUDHW_Init_AudHW_CLK(void)
{
    UINT32 value;

    HWREG_W(pcm_cfg, aud_param.PCM_CFG);

#ifdef QAE468_I2S_RECORD
    HWREG_W(gpio_first[0], 0x0000); // set Ext-DAC GPIO
#endif

    // PCM_CFG_LRSEL, bit[8]=0, config0_LRCK/config0_BCK
    HWREG_W(pcm_cfg, HWREG_R(pcm_cfg) & 0xfffffeff );

    // ADC_CFG_LRSEL, bit[8]=0, config0_LRCK/config0_BCK
    HWREG_W(ext_adc_cfg, HWREG_R(ext_adc_cfg) & 0xfffffeff );

    // DAC setting
    HWREG_W(sft_cfg_39_dtv, 0xa3c);	//0xa1c?

    // PLLA enable
    HWREG_W(sft_cfg_36_dtv, HWREG_R(sft_cfg_36_dtv) |= (1 << 12) );

    // AD clock
    //HWREG_W(clk_sel_atv_dtv, HWREG_R(clk_sel_atv_dtv)|((0x1)<<4) );
    value = HWREG_R(clk_sel_atv_dtv) | ((0x1) << 4); // AD clock
    //value |= (0x1)<<12;                             // set bck inverse
    HWREG_W(clk_sel_atv_dtv, value);

    //============================================//
    //			pin mux setting					  //
    //============================================//

    //SFT_CFG_I2S_SEL=1
    HWREG_W(sft_cfg_4_dtv, HWREG_R(sft_cfg_4_dtv) |= (1 << 18));
    HWREG_W(sft_cfg_4_dtv, HWREG_R(sft_cfg_4_dtv) &= ~(1 << 4));
    HWREG_W(sft_cfg_4_dtv, HWREG_R(sft_cfg_4_dtv) &= ~(1 << 7));

    //SFT_CFG_IEC_TX_SEL = 2'b1
    HWREG_W(sft_cfg_4_dtv, HWREG_R(sft_cfg_4_dtv) &= ~(0xc000));
    HWREG_W(sft_cfg_4_dtv, HWREG_R(sft_cfg_4_dtv) |= (0x4000));

    //SFT_CFG_SPDIF_RX_CT_SEL = 1 ( for certification used )
    //HWREG_W(sft_cfg_4_dtv, HWREG_R(sft_cfg_4_dtv) |= (1 << 26));
    //HWREG_W(sft_cfg_4_dtv, HWREG_R(sft_cfg_4_dtv) &= ~(1 << 5));

    //SFT_CFG_IEC_RX_SEL = 2'b1
    //HWREG_W(sft_cfg_4_dtv, HWREG_R(sft_cfg_4_dtv) &= ~(0x30000));
    //HWREG_W(sft_cfg_4_dtv, HWREG_R(sft_cfg_4_dtv) |= (0x10000));

    return;
}

// FUNCTION DESCRIPTION :
//      Set Audio Hardware Clock
//
//#define HW_SUPPORT_HDMI_I2S_SPDIF

void AUDHW_Set_AudHW_SmpRate(unsigned fs)
{
    int index = 0;

    while ((fs & 0x1) == 0)
    {
        fs >>= 1;
        index++;
        if (index >= 12)
            break;
    }

    PLLA_Clock_Setting(index);
}
void AUDHW_Set_IEC0_CLK(unsigned fs)
{
    return;
}

void AUDHW_Set_IEC1_CLK(unsigned fs)
{
    return;
}

void AUDHW_Set_IntDAC_CLK(unsigned fs)
{
    return;
}

#ifdef INTERNAL_AUDIO_DAC
void internal_dac_setting( void )
{
    return;
}

void internal_dac_reset( void )
{
    return;
}
#endif

void PLLA_Clock_Setting(int index)
{
    UINT32 value;

    /*
    *-----------------------------------------------------------*
    *           G2.4 sft_cfg_36_dtv PLLA configuration
    *   Bit15       0:FCKOUT = 147.576MHz
    *               1:FCKOUT = 135.4752MHz
    *
    *           G0.21 clk_sel_atv_dtv
    *   Bit22:17    Ext DAC XCK clock divider control
    *               0:clock not divided 
    *               other settings:divided by 2*setting
    *
    *   Bit11:6     Int DAC XCK clock divider control
    *               0:clock not divided 
    *               other settings:divided by 2*setting    
    *-----------------------------------------------------------*
    */

    switch(index)
    {
        case 0://32k
            HWREG_0(sft_cfg_36_dtv, 0xffff7fff);            // select PLLA output 147.576MHz
            value = HWREG_R(clk_sel_atv_dtv) & 0xff81f03f;  // clean bit22:17 and bit11:6
            value |= AUDCLK_128XCK_04096 << 6;              // 32*128 = 4096
            value |= audHW_param.xck_clk_fs_table[index] << 17;
            HWREG_W(clk_sel_atv_dtv, value);
            break;
        case 2: //48k
            HWREG_0(sft_cfg_36_dtv, 0xffff7fff);            // select PLLA output 147.576MHz
            value = HWREG_R(clk_sel_atv_dtv) & 0xff81f03f;  // clean bit22:17 and bit11:6
            value |= AUDCLK_128XCK_06144 << 6;              // 48*128 = 6144
            value |= audHW_param.xck_clk_fs_table[index] << 17;
            HWREG_W(clk_sel_atv_dtv, value);
            break;
        case 6: //96k
            HWREG_0(sft_cfg_36_dtv, 0xffff7fff);            // select PLLA output 147.576MHz
            value = HWREG_R(clk_sel_atv_dtv) & 0xff81f03f;  // clean bit22:17 and bit11:6
            value |= AUDCLK_128XCK_12288 << 6;              // 96*128 = 12288
            value |= audHW_param.xck_clk_fs_table[index] << 17;
            HWREG_W(clk_sel_atv_dtv, value);
            break;
        case 10://192k
            HWREG_0(sft_cfg_36_dtv, 0xffff7fff);            // select PLLA output 147.576MHz
            value = HWREG_R(clk_sel_atv_dtv) & 0xff81f03f;  // clean bit22:17 and bit11:6
            value |= AUDCLK_128XCK_24576 << 6;              // 192*128 = 24576
            value |= audHW_param.xck_clk_fs_table[index] << 17;
            HWREG_W(clk_sel_atv_dtv, value);
            break;

        case 1://44.1k
            HWREG_1(sft_cfg_36_dtv, (1 << 15) );            // select PLLA output 135.4752MHz
            value = HWREG_R(clk_sel_atv_dtv) & 0xff81f03f;  // clean bit22:17 and bit11:6
            value |= AUDCLK_128XCK_05644 << 6;              // 44.1*128 = 5644
            value |= audHW_param.xck_clk_fs_table[index] << 17;
            HWREG_W(clk_sel_atv_dtv, value);
            break;
        case 5://88.2k
            HWREG_1(sft_cfg_36_dtv, (1 << 15) );            // select PLLA output 135.4752MHz
            value = HWREG_R(clk_sel_atv_dtv) & 0xff81f03f;  // clean bit22:17 and bit11:6
            value |= AUDCLK_128XCK_11290 << 6;              // 88.2*128 = 11290
            value |= audHW_param.xck_clk_fs_table[index] << 17;
            HWREG_W(clk_sel_atv_dtv, value);
            break;
        case 9://176.4k
            HWREG_1(sft_cfg_36_dtv, (1 << 15) );            // select PLLA output 135.4752MHz
            value = HWREG_R(clk_sel_atv_dtv) & 0xff81f03f;  // clean bit22:17 and bit11:6
            value |= AUDCLK_128XCK_22579 << 6;              //176.4*128 = 22579
            value |= audHW_param.xck_clk_fs_table[index] << 17;
            HWREG_W(clk_sel_atv_dtv, value);
            break;

        default:
            HWREG_0(sft_cfg_36_dtv, 0xffff7fff);            // select PLLA output 147.576MHz
            value = HWREG_R(clk_sel_atv_dtv) & 0xff81f03f;  // clean bit22:17 and bit11:6
            value |= AUDCLK_128XCK_06144 << 6;
            value |= audHW_param.xck_clk_fs_table[2] << 17; // set default 48kHz
            HWREG_W(clk_sel_atv_dtv, value);
            break;
    }


}
#ifdef HW_SUPPORT_AUDHW_USE_DIGITAL_PLL
void digital_plla_setting(int index)
{
    return;
}
#endif


//[TODO] : check otp or stamp later, then we can remove this definition
#define AUDHW_DETECTION_CFG     (1)

#if(AUDHW_DETECTION_CFG==1) //[TODO] : check otp or stamp if we know how to identify IC version D
#define AUDHW_PIN_NULL  (0xff)
//PIN G_MX xx
static UINT8 abAUDHWPinTab[][4] =
{
    //Output, Input,      Switch,         Switch Level for Output
    {12,    10,         AUDHW_PIN_NULL, AUDHW_PIN_NULL},        // 1st pair for EV board, out/in
    {12,    11,         AUDHW_PIN_NULL, AUDHW_PIN_NULL},        // 2nd pair for demo board, out/in
    {12,    101,        AUDHW_PIN_NULL, AUDHW_PIN_NULL},        // 3rd pair for orion 214 board, out/in
    {12,    101,         102, 0},                               // 4th pair for orion 210 board, out/in/switchs
    {AUDHW_PIN_NULL,    AUDHW_PIN_NULL, AUDHW_PIN_NULL},        // end symbol for this table
};
static UINT8 abAUDHWPinGPIOValBakeUpTab[][3] =
{
    //Output,           Input,                  Switch
    {AUDHW_PIN_NULL,    AUDHW_PIN_NULL,         AUDHW_PIN_NULL}, //GPIO_First
    {AUDHW_PIN_NULL,    AUDHW_PIN_NULL,         AUDHW_PIN_NULL}, //GPIO_First
    {AUDHW_PIN_NULL,    AUDHW_PIN_NULL,         AUDHW_PIN_NULL}, //GPIO_First
    {AUDHW_PIN_NULL,    AUDHW_PIN_NULL,         AUDHW_PIN_NULL}, //GPIO_First
    {AUDHW_PIN_NULL,    AUDHW_PIN_NULL,         AUDHW_PIN_NULL}, // end symbol for this table
};

//---------- duplicate GPIO macro for B version work-arround ---------- begin
//---------- can't include plf_gpio_select => #include "../platform/plf_gpio_select.h"
#define GPIO_F_SET(a,d) do {if (d){HWREG_W(gpio_first[a/32], HWREG_R(gpio_first[a/32])|(1<<(a%32))); \
                        }else{HWREG_W(gpio_first[a/32], HWREG_R(gpio_first[a/32])& ~(1<<(a%32)));}}while (0)

// william: A dirty work arround for spv9200 GPIO
// http://psweb.sunplus.com/HE1200/view.php?id=652
#define GPIO_M_SET(a,d) do {volatile unsigned int  temp = HWREG_R(gpio_master[a/32]);\
if (d) {HWREG_W(gpio_master[a/32], HWREG_R(gpio_master[a/32])|(1<<(a%32))); \
                            } else  {HWREG_W(gpio_master[a/32], HWREG_R(gpio_master[a/32])&~(1<<(a%32))); } \
							temp = HWREG_R(gpio_master[a/32]);} while (0)
// william: A dirty work arround for spv9200 GPIO
// http://psweb.sunplus.com/HE1200/view.php?id=652
#define GPIO_E_SET(a,d) do { volatile unsigned int  temp = HWREG_R(gpio_oe[a/32]);  \
if (d) {HWREG_W(gpio_oe[a/32],HWREG_R(gpio_oe[a/32])|(1<<(a%32))); \
       } else  {HWREG_W(gpio_oe[a/32], HWREG_R(gpio_oe[a/32])&~(1<<(a%32))); }\
							temp = HWREG_R(gpio_oe[a/32]);} while (0)
// william: A dirty work arround for spv9200 GPIO
// http://psweb.sunplus.com/HE1200/view.php?id=652
#define GPIO_O_SET(a,d) do {volatile unsigned int  temp = HWREG_R(gpio_out[a/32]);  \
		if (d) { \
        HWREG_W(gpio_out[a/32], HWREG_R(gpio_out[a/32])|(1<<(a%32))); \
        } else  { \
        HWREG_W(gpio_out[a/32], HWREG_R(gpio_out[a/32])&~(1<<(a%32))); } \
		temp = HWREG_R(gpio_out[a/32]); } while (0)

#define GPIO_F_GET(a) ((HWREG_R(gpio_first[a/32]) >> (a%32)) & 0x01)
#define GPIO_M_GET(a) ((HWREG_R(gpio_master[a/32]) >> (a%32)) & 0x01)
#define GPIO_E_GET(a) ((HWREG_R(gpio_oe[a/32]) >> (a%32)) & 0x01)
#define GPIO_O_GET(a) ((HWREG_R(gpio_out[a/32]) >> (a%32)) & 0x01)
#define GPIO_I_GET(a)   ((HWREG_R(gpio_in[a/32]) >> (a%32)) & 0x01)
//---------- duplicate GPIO macro for B version work-arround ---------- end
#define set_GpioPinAsOutput(GpioNum, OutputValue)  \
    {\
        if ( GpioNum != AUDHW_PIN_NULL ) \
        {\
            GPIO_F_SET(GpioNum, 1);\
            GPIO_M_SET(GpioNum, 1);\
            GPIO_E_SET(GpioNum, 1);\
            GPIO_O_SET(GpioNum, OutputValue);\
        }\
    }
#define set_GpioPinAsInput(GpioNum)  \
    {\
        if ( GpioNum != AUDHW_PIN_NULL ) \
        {\
            GPIO_F_SET(GpioNum, 1);\
            GPIO_M_SET(GpioNum, 1);\
            GPIO_E_SET(GpioNum, 0);\
        }\
    }
#define get_GpioPinInput(GpioNum, InputValue)  \
    {\
        if ( GpioNum != AUDHW_PIN_NULL ) \
        {\
            GPIO_F_SET(GpioNum, 1);\
            GPIO_M_SET(GpioNum, 1);\
            GPIO_E_SET(GpioNum, 0);\
            InputValue = GPIO_I_GET(GpioNum);\
        }\
    }
#define set_GpioPinFirst2BackupTab(bRowIdx, bColIdx, GpioNum) \
    {\
        if ( GpioNum != AUDHW_PIN_NULL ) \
        {\
            abAUDHWPinGPIOValBakeUpTab[bRowIdx][bColIdx] = GPIO_F_GET(GpioNum);\
        }\
    }

#define set_GpioFirstByTab(row,col) \
    {\
        if ( abAUDHWPinTab[row][col] != AUDHW_PIN_NULL ) \
        {\
            GPIO_F_SET(abAUDHWPinTab[row][col], abAUDHWPinGPIOValBakeUpTab[row][col]);\
        }\
    }

static inline int AUDHW_RecoverGPIOSetting(void)
{
    UINT8 bIdx = 0;

    while(abAUDHWPinTab[bIdx][0] != AUDHW_PIN_NULL)
    {
        set_GpioFirstByTab(bIdx, 0);
        set_GpioFirstByTab(bIdx, 1);
        set_GpioFirstByTab(bIdx, 2);
        (bIdx)++;
    }

    return 0;
}

static inline int AUDHW_CheckCorrectBoardIdx(UINT8 *pbIdx)
{
    UINT8 Ret = 0;
    UINT8 bInputVal = 0;
    *pbIdx = 0;

    while(abAUDHWPinTab[*pbIdx][0] != AUDHW_PIN_NULL)
    {
        set_GpioPinFirst2BackupTab(*pbIdx, 0, abAUDHWPinTab[*pbIdx][0]);
        set_GpioPinFirst2BackupTab(*pbIdx, 1, abAUDHWPinTab[*pbIdx][1]);
        set_GpioPinFirst2BackupTab(*pbIdx, 2, abAUDHWPinTab[*pbIdx][2]);

        set_GpioPinAsOutput(abAUDHWPinTab[*pbIdx][2], abAUDHWPinTab[*pbIdx][3]); //switch
        set_GpioPinAsInput(abAUDHWPinTab[*pbIdx][1]); //input

        /* set output as high */
        set_GpioPinAsOutput(abAUDHWPinTab[*pbIdx][0], 1); //output
        get_GpioPinInput(abAUDHWPinTab[*pbIdx][1], bInputVal)

        if(bInputVal == 1)
        {
            /* set output as low */
            set_GpioPinAsOutput(abAUDHWPinTab[*pbIdx][0], 0);
            get_GpioPinInput(abAUDHWPinTab[*pbIdx][1], bInputVal);

            if(bInputVal == 0)
            {
                /* PASS */
                diag_printf("Output Pin = %d, Input Pin = %d, Success\n", abAUDHWPinTab[*pbIdx][0], abAUDHWPinTab[*pbIdx][1]);
                Ret |= 1;
                break;
            }
        }
        else
        {
            diag_printf("Output Pin = %d, Input Pin = %d, Fail\n", abAUDHWPinTab[*pbIdx][0], abAUDHWPinTab[*pbIdx][1]);
            Ret |= 0;
        }

        /* FAIL */
        (*pbIdx)++;
    }

    return Ret;
}
#endif

void AUDHW_AutoCalibration(void)
{
#if(AUDHW_DETECTION_CFG == 1) //[TODO] : check otp or stamp if we know how to identify IC version D

    UINT8 bBoardIdx;
    UINT32 dao = 0;
    UINT32 val = 0;
    UINT32 g11_backup;
    int i;

    /* Get correct board number */
    if(AUDHW_CheckCorrectBoardIdx(&bBoardIdx))
    {
        AUDDRV_PRINT("[AUDHW] bBoardIdx[%d], out/in = [%d/%d]\n", bBoardIdx, abAUDHWPinTab[bBoardIdx][0], abAUDHWPinTab[bBoardIdx][1]);
    }
    else
    {
        AUDHW_RecoverGPIOSetting();
        AUDDRV_PRINT("[AUDHW] FAIL to locate bBoardIdx!!!!!\n");
        return;
    }

    /* G1.1 clean bit 6:5, need to backup???? */
    g11_backup = HWREG_R(sft_cfg_1);
    HWREG_W(sft_cfg_1, HWREG_R(sft_cfg_1) &= ~(0x60));

    /* Enable DAC debug pin (set G1.10[0] = 1) */
    val = HWREG_R(sft_cfg_10_dtv) | (0x1);
    HWREG_W(sft_cfg_10_dtv, val);

    /* Dis DAC test mode (set G1.0[0] = 0) */
    val = HWREG_R(sft_cfg_0) & (~0x1);
    HWREG_W(sft_cfg_0, val);

    /* set ADAC_SEL_N = 2'b00 (set DAO[4:0] as output, G2.7[16:15] = 0x2)*/
    /* set ADAC_TDA = 2'b10 (set DSM test mode, G2.7[7:6] = 0x2)*/
    val = HWREG_R(sft_cfg_39_dtv);
    val &= ~(0x3 << 15);                    // clean bit 16:15, 7:6
    val |= (0 << 15);                       // ADAC_SEL_N = 2'b00
    val &= ~(0x3 << 6);                     // clean bit 7:6
    val |= (2 << 6);                        // ADAC_TDA = 2'b10
    HWREG_W(sft_cfg_39_dtv, val);


    UINT32 back_up_sft_cfg_4_dtv = HWREG_R(sft_cfg_4_dtv);
    /* set global pin-mux G1.4[0], G1.4[2], G1.4[6:5], G1.4[18] = 0*/
    val = HWREG_R(sft_cfg_4_dtv);
    val &= ~(0x1 << 18);
    val &= ~(0x1 << 0);
    val &= ~(0x1 << 2);
    val &= ~(0x3 << 5);//This one let PC BUILDIN_EDID function WORK
    HWREG_W(sft_cfg_4_dtv, val);

    /* set global pin-mux G1.10[24] = 0  test mode */
    val = HWREG_R(sft_cfg_10_dtv);
    val &= ~(0x1 << 24);
    HWREG_W(sft_cfg_10_dtv, val);

    /* set pin-mux G1.1[14:13] = 0 */
    val = HWREG_R(sft_cfg_1);
    val &= ~(0x3 << 13);
    HWREG_W(sft_cfg_1, val);

    /* disable GPIO first for output PIN => this should be output from IC, not GPIO */
    GPIO_F_SET(abAUDHWPinTab[bBoardIdx][0], 0);

    /* set GPIO master */
    val = HWREG_R(gpio_master[0]);
    val &= ~(0x80003c00);     // set bit 10,11,12,13,31 ctrl by IOP reg
    //val |= 0x23880000;         // set bit 19,23,24,25,29 ctrl by RISC reg
    HWREG_W(gpio_master[0], val);

    /* gpio oe set to 0(29, 23, 25, 24, 19) => already set in AUDHW_CheckCorrectBoardIdx  */
    //val = HWREG_R(gpio_oe);
    //val &= ~(0x23880000);
    //HWREG_W(gpio_oe, val);

    //val = HWREG_R(gpio_first[0]);
    //val |= 0x23880000;
    //HWREG_W(gpio_first[0], val);

    //delay_1ms(500);
	msleep(500);

    //dao |= ((val>>29)&0x1)<<0;
    //dao |= ((val>>23)&0x1)<<1;
    //dao |= ((val>>25)&0x1)<<2;
    //dao |= ((val>>24)&0x1)<<3;

    dao = 0;

    for (i = 0; i < 1024; i++)
    {
        //val = HWREG_R(gpio_in[0]);
        get_GpioPinInput(abAUDHWPinTab[bBoardIdx][1], val);
        //if((val >> (abAUDHWPinTab[bBoardIdx][1] % 32)) & 0x1)
        if(val)
            dao++;
    }

    //if (dao&(0x1<<2))
    if (dao)
    {
        AUDDRV_PRINT("[AUDHW] Need to set CLK inverse, DAO count %d\n", dao);
        val = HWREG_R(clk_sel_atv_dtv);
        if (val&(0x1 << 12))
            val &= ~(0x1 << 12);
        else
            val |= (0x1) << 12;                           // set bck inverse

        HWREG_W(clk_sel_atv_dtv, val);
    }
    else
    {
        AUDDRV_PRINT("[AUDHW] It's OK. NO need to set CLK inverse\n");
    }

    //recover PIN MUX
    HWREG_W(sft_cfg_4_dtv, back_up_sft_cfg_4_dtv);
    HWREG_W(sft_cfg_1, g11_backup);

    /* recover GPIO switch value for I2S and detection function */
    set_GpioPinAsOutput(abAUDHWPinTab[bBoardIdx][2], (~(abAUDHWPinTab[bBoardIdx][3]) & 0x1));
    AUDHW_RecoverGPIOSetting();
#else

    AUDDRV_PRINT("[AUDHW] Don't need to do calibration in other IC version\n");

#endif
}

void AUDHW_Enable_Mute( int pin_number )
{
	/* GPIO_AMUTE_PIN: 0 -> mute, 1 -> unmute */

	GPIO_E_SET(pin_number, 0);
}

void AUDHW_Disable_Mute(int pin_number)
{
	/* GPIO_AMUTE_PIN: 0 -> mute, 1 -> unmute */
	GPIO_E_SET(pin_number, 1);
}
