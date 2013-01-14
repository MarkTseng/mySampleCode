#include <asm/mach-spv9200/types.h>
#include <asm/mach-spv9200/ic-spv9200/hw_public.h>
#include <asm/mach-spv9200/regmap_9200.h>
#include <linux/types.h>
#include "drv_common.h"
#include "drv_ssd.h"
#include "include/aud_global.h"
#include "include/auddrv.h"

#include "include/dsp_mon.h"

#define strncmpi	strncasecmp

void dumpLongDataLE(UINT32 base, UINT32 len)
{

    UINT32    i, j;

	DSP_PRINT("dumpLongDataLE::base:0x%x,len:0x%x\n",base,len);

    if (len >= 0x100000)
    {
        DSP_PRINT("dumpLongDataLE::e:len is >= 0x100000,skip dump!!\n");
        return;
    }

    for (i=0; i<len*4;)
    {
        DSP_PRINT("%08x: ", base+i);
        for (j=0; j<4; j++)
        {
            DSP_PRINT(" %08x", *((UINT32 *)(base+i)));
            i+=4;
        }
        DSP_PRINT("\n");
    }

}

const UINT8 *mon_skipspace(const UINT8 * p)
{
	if (p == NULL) {
		return NULL;
	}

	while (1)
	{
		int c = p[0];
		if (c == ' ' || c == '\t' || c == '\v') {
			p++;
		} else {
			break;
		}
	}
	return p;
}

const UINT8 *mon_readhex(const UINT8 * p, int *x)
{
	int base = 10;
	int sign = 0;
	int cnt, retval;

	if (p == NULL) {
		return NULL;
	}
	if (x == NULL) {
		return p;
	}

	p = mon_skipspace(p);

	if (p[0] == '-') {
		sign = 1;
		p++;
	}
	if (p[0] == '0' && p[1] == 'x') {
		base = 16;
		p += 2;
	} else if (p[0] == '0' && p[1] == 'o') {
		base = 8;
		p += 2;
	} else if (p[0] == '0' && p[1] == 'b') {
		base = 2;
		p += 2;
	}

	retval = 0;
	for (cnt = 0; 1; cnt++) {
		int c = *p;
		int val;

		// translate 0~9, a~z, A~Z
		if (c >= '0' && c <= '9')
			val = c - '0';
		else if (c >= 'a' && c <= 'z')
			val = c - 'a' + 10;
		else if (c >= 'A' && c <= 'Z')
			val = c - 'A' + 10;
		else
			break;
		if (val >= base)
			break;

		p++;
		retval = retval * base + val;
	}

	if (cnt == 0) {
		return NULL;			// no translation is done??
	}

	if (sign) {
		retval = -retval;		// change sign
	}
	*(UINT32 *)x = retval;		// store it
	return p;
}

/*----------------------------------------------
/         AUD and DSP function
----------------------------------------------*/
void dsp_monCmd(const char* pbuf)
{

    MONTASK_PRINT("dsp_monCmd::pbuf:%s\n",pbuf);
    if(strncmpi(pbuf, "pc", 2) == 0)
    {
        // dsp pc
        DISABLE_JTAG();
        diag_printf("dsp_monCmd::DSP PC:0x%x\n",GET_DSP_PC());
        ENABLE_JTAG();
    }
    else if(strncmpi(pbuf, "fc", 2) == 0)
    {
        // dec1 sample count
        diag_printf("dsp_monCmd::dec1 sample count:0x%x\n",HWREG_R(dsp_port[15]));
    }
    else if(strncmpi(pbuf, "bsbase", 6 ) == 0)
    {
        UINT8 *DSP_BS_base;
        DSP_BS_base = (UINT8 *)AUDIF_Get_audBSbuf_start(AUDIF_BANK_DEC1);
        diag_printf("[mon]AUDIF_Get_audBSbuf_start: %x\n", (UINT32)DSP_BS_base );

        extern UINT32 DSP_GetInfo(UINT32 Info);
        BsBufInfo* pBsBufInfo = NULL;
        pBsBufInfo = (BsBufInfo*)DSP_GetInfo(INFO_BSINFO_DEC1);
        diag_printf("[ccyang]GetBsBufBase: %x\n", (UINT32)pBsBufInfo->pBsBufBase );
    }
    else if(strncmpi(pbuf, "state", 5) == 0)
    {
        // dsp state
        diag_printf("dsp_monCmd::state:0x%08x\n",HWREG_R(dsp_port[12]));
    }
    else if(strncmpi(pbuf, "sendcmd", 7) == 0)
    {
        // dsp cmd [port0] [port1]
        UINT32 port0 = 0;
        UINT32 port1 = 0;
        pbuf = (const char *)mon_readhex((const UINT8 *)(pbuf+7), (int *)&port0);
        pbuf = (const char *)mon_readhex((const UINT8 *)pbuf, (int *)&port1);
        MONTASK_PRINT("dsp_monCmd::cmd:port0:0x%08x, port1:0x%08x\n",port0,port1);
        //if ((port0>>24) <= RISC_CMD_MAX)
        DSP_SendCmd((port0>>24)&0xFF,(port0>>16)&0xFF, port0&0xFFFF, port1);
        //else
        //	ERROR_PRINT("dsp_monCmd::e:undefined command\n",port0,port1);
    }

    //
    // read/write dsp internal memory
    //
    else if(strncmpi(pbuf, "rw", 2) == 0)
    {
        // dsp rw [address]  (read word)
        int addr = 0;
        UINT32 value = 0;
        pbuf = (const char *)mon_readhex((const UINT8 *)pbuf+2, &addr);
        DSP_PRINT("dsp_monCmd::rw:address:0x%x\n",addr);
        value = DSP_ReadData(DSP_READ_SHORT, addr);
        DSP_PRINT("DSP::0x%04x:0x%08x\n",addr,value);

    }
    else if(strncmpi(pbuf, "rd", 2) == 0)
    {
        // dsp rd [address] (read double word)
        int addr = 0;
        UINT32 value = 0;
        pbuf = (const char *)mon_readhex((const UINT8 *)pbuf+2, &addr);
        DSP_PRINT("dsp_monCmd::rd:address:0x%x\n",addr);
        value = DSP_ReadData(DSP_READ_LONG, addr);
        DSP_PRINT("DSP::0x%04x:0x%08x\n",addr,value);

    }
    else if(strncmpi(pbuf, "rr", 2) == 0)
    {
        // dsp re [address] (read double word)
        int addr = 0;
        UINT32 value = 0;
        pbuf = (const char *)mon_readhex((const UINT8 *)pbuf+2, &addr);
        DSP_PRINT("dsp_monCmd::re:register:0x%x\n",addr);
        value = DSP_ReadData(DSP_READ_REG, addr);
        DSP_PRINT("DSP::0x%08x:0x%08x\n",addr,value);

    }
    else if(strncmpi(pbuf, "ri", 2) == 0)
    {
        // dsp re [address] (read double word)
        int addr = 0;
        UINT32 value = 0;
        pbuf = (const char *)mon_readhex((const UINT8 *)pbuf+2, &addr);
        DSP_PRINT("dsp_monCmd::ri:register:0x%x\n",addr);
        value = DSP_ReadData(DSP_READ_INT_REG, addr);
        DSP_PRINT("DSP::0x%08x:0x%08x\n",addr,value);

    }
    else if(strncmpi(pbuf, "ww", 2) == 0)
    {
        // dsp ww [address] [value] (write word)
        int addr = 0;
        int value = 0;
        pbuf = (const char *)mon_readhex((const UINT8 *)pbuf+2, &addr);
        pbuf = (const char *)mon_readhex((const UINT8 *)pbuf, &value);
        DSP_PRINT("dsp_monCmd::ww:address:0x%x, value:0x%x\n",addr,value);
        DSP_WriteData(DSP_WRITE_SHORT, addr, value);
    }
    else if(strncmpi(pbuf, "wd", 2) == 0)
    {
        // dsp wd2 [address] [value]
        int addr = 0;
        int value = 0;
        pbuf = (const char *)mon_readhex((const UINT8 *)pbuf+2, &addr);
        pbuf = (const char *)mon_readhex((const UINT8 *)pbuf, &value);
        DSP_PRINT("dsp_monCmd::wd:address:0x%x, value:0x%x\n",addr,value);
        DSP_WriteData(DSP_WRITE_LONG, addr, value);
    }
    else if(strncmpi(pbuf, "dw", 2) == 0)
    {
        // dsp dw [address] (dump  word)
        int addr = 0;
        int len = 0;

        pbuf = (const char *)mon_readhex((const UINT8 *)pbuf+2, &addr);
        pbuf = (const char *)mon_readhex((const UINT8 *)pbuf, &len);

        if (len >= 32*1024)
        {
            DSP_PRINT("dsp_monCmd::e:len:%d >= 32k\n",len);
            len = 32*1024-1;  // max dw len (32k-1)
        }
        DSP_PRINT("dsp_monCmd::dw:address:0x%x, len:0x%x\n",addr,len);
        DSP_DumpData(DSP_DUMP_DATA,addr,len);

    }

    else if(strncmpi(pbuf, "dram", 4) == 0)
    {
        // dsp dd [address] (dump double word)
        UINT32 offset = 0;
        UINT32 len = 0;

        pbuf = (const char *)mon_readhex((const UINT8 *)(pbuf+4), (int *)&offset);
        pbuf = (const char *)mon_readhex((const UINT8 *)pbuf, (int *)&len);
        DSP_PRINT("dsp_monCmd::dram:address offset:0x%x, len:0x%x\n",offset,len);
        //dumpByte(HWREG_R(aud_audhwya)+offset,len);
        dumpByte(DSP_GetInfo(INFO_DATA_BANK_BASE)+offset,len);

    }
    else if(strncmpi(pbuf, "fifo", 4) == 0)
    {
        // dsp dd [address] (dump double word)
        int offset = 0;
        int len = 0;

        pbuf = (const char *)mon_readhex((const UINT8 *)(pbuf+4), (int *)&offset);
        pbuf = (const char *)mon_readhex((const UINT8 *)pbuf, (int *)&len);
        DSP_PRINT("dsp_monCmd::dram:address offset:0x%x, len:0x%x\n",offset,len);
        dumpWord24(DSP_GetInfo(INFO_DATA_BANK_BASE) + offset,len);
    }

    else if(strncmpi(pbuf, "hex", 3) == 0)
    {
        // dsp dd [address] (dump double word)
        int offset = 0;
        int len = 0;

        pbuf = (const char *)mon_readhex((const UINT8 *)(pbuf+3), (int *)&offset);
        pbuf = (const char *)mon_readhex((const UINT8 *)pbuf, (int *)&len);
        DSP_PRINT("dsp_monCmd::hex:address offset:0x%x, len:0x%x\n",offset,len);
        dumpHex(DSP_GetInfo(INFO_DATA_BANK_BASE) + offset,len);
    }
    else if(strncmpi(pbuf, "info", 4) == 0)
    {
        DSP_PRINT("dsp_monCmd::info \n");
        pbuf = pbuf+5;							// skip "info " (include space)

        if(strncmpi(pbuf, "risc", 4) == 0)
        {
            DSP_PRINT("dsp_monCmd::info risc\n");
            DSP_DumpData(DSP_DUMP_RISC_PARAM,0,0);
        }
        else if(strncmpi(pbuf, "dsp", 3) == 0)
        {
            DSP_PRINT("dsp_monCmd::info dsp\n");
            DSP_DumpData(DSP_DUMP_DSP_PARAM,0,0);
        }
        if(strncmpi(pbuf, "nxp_risc", 8) == 0)
        {
            DSP_PRINT("dsp_monCmd::info nxp_risc\n");
            DSP_DumpData(DSP_DUMP_NXP_RISC_PARAM,0,0);
        }
        else if(strncmpi(pbuf, "nxp_dsp", 7) == 0)
        {
            DSP_PRINT("dsp_monCmd::info nxp_dsp\n");
            DSP_DumpData(DSP_DUMP_NXP_DSP_PARAM,0,0);
        }
        else if(strncmpi(pbuf, "main", 4) == 0)
        {
            DSP_PRINT("dsp_monCmd::info main\n");
            DSP_DumpData(DSP_DUMP_MAIN_DATA,0,0);
        }

        else if(strncmpi(pbuf, "mixer", 5) == 0)
        {
            DSP_PRINT("dsp_monCmd::info mixer\n");
            DSP_DumpData(DSP_DUMP_MIXER_DATA,0,0);
        }
        else if(strncmpi(pbuf, "swapp", 5) == 0)
        {
            DSP_PRINT("dsp_monCmd::info swapp\n");
            DSP_DumpData(DSP_DUMP_SWAPP_DATA,0,0);
        }

        else if(strncmpi(pbuf, "dec1", 4) == 0)
        {
            DSP_PRINT("dsp_monCmd::info dec1\n");
            DSP_DumpData(DSP_DUMP_DEC1_PARAM,0,0);
        }
        else if(strncmpi(pbuf, "dec2", 4) == 0)
        {
            DSP_PRINT("dsp_monCmd::info dec2\n");
            DSP_DumpData(DSP_DUMP_DEC2_PARAM,0,0);
        }
        else if(strncmpi(pbuf, "enc", 3) == 0)
        {
            DSP_PRINT("dsp_monCmd::info enc\n");
            DSP_DumpData(DSP_DUMP_ENC_PARAM,0,0);
        }
        else if(strncmpi(pbuf, "app", 3) == 0)
        {
            DSP_PRINT("dsp_monCmd::info app\n");
            DSP_DumpData(DSP_DUMP_APP_DATA,0,0);
        }
        else if(strncmpi(pbuf, "dbg", 3) == 0)
        {
            DSP_PRINT("dsp_monCmd::info dbg\n");
            DSP_DumpData(DSP_DUMP_DBG_DATA,0,0);
        }
        else if(strncmpi(pbuf, "drambase", 8) == 0)
        {
            DSP_PRINT("dsp_monCmd::info drambase\n");
            DSP_DumpData(DSP_DUMP_DRAMBASE,0,0);
        }
        else if(strncmpi(pbuf, "util", 4) == 0)
        {
            DSP_PRINT("dsp_monCmd::info util\n");
            DSP_DumpData(DSP_DUMP_UTIL_DATA,0,0);
        }
        else if(strncmpi(pbuf, "stack", 5) == 0)
        {
            DSP_PRINT("dsp_monCmd::info stack\n");
            DSP_DumpData(DSP_DUMP_STACK,0,0);
        }
        else if(strncmpi(pbuf, "isr", 3) == 0)
        {
            DSP_PRINT("dsp_monCmd::info isr\n");
            DSP_DumpData(DSP_DUMP_ISR_GLOBALS,0,0);
        }
        else if(strncmpi(pbuf, "spdifin", 7) == 0)
        {
            DSP_PRINT("dsp_monCmd::info spdifin\n");
            DSP_DumpData(DSP_DUMP_SPDIFIN_DATA,0,0);
        }
        else if(strncmpi(pbuf, "effect",6) == 0)
        {
            DSP_PRINT("dsp_monCmd::info effect\n");
            DSP_DumpData(DSP_DUMP_EFFECT_PARAM,0,0);
        }
        else if(strncmpi(pbuf, "ddp_rom",7) == 0)
        {
            DSP_PRINT("dsp_monCmd::info ddp_rom\n");
            DSP_DumpData(DSP_DUMP_ROM_DDP,0,0);
        }
        else if(strncmpi(pbuf, "src_rom",7) == 0)
        {
            DSP_PRINT("dsp_monCmd::info src_rom\n");
            DSP_DumpData(DSP_DUMP_ROM_SRC,0,0);
        }
        else if(strncmpi(pbuf, "pl2x_rom",8) == 0)
        {
            DSP_PRINT("dsp_monCmd::info pl2x_rom\n");
            DSP_DumpData(DSP_DUMP_ROM_PL2X,0,0);
        }
        else if(strncmpi(pbuf, "dtshd_rom",9) == 0)
        {
            DSP_PRINT("dsp_monCmd::info dtshd_rom\n");
            DSP_DumpData(DSP_DUMP_ROM_DTSHD,0,0);
        }
        else if(strncmpi(pbuf, "truehd_rom",10) == 0)
        {
            DSP_PRINT("dsp_monCmd::info truehd_rom\n");
            DSP_DumpData(DSP_DUMP_ROM_TRUEHD,0,0);
        }
        else if(strncmpi(pbuf, "watermark_rom",13) == 0)
        {
            DSP_PRINT("dsp_monCmd::info watermark_rom\n");
            DSP_DumpData(DSP_DUMP_ROM_WATERMARK,0,0);
        }
        else if(strncmpi(pbuf, "dtslbr_rom",10) == 0)
        {
            DSP_PRINT("dsp_monCmd::info dtslbr_rom\n");
            DSP_DumpData(DSP_DUMP_ROM_DTSLBR,0,0);
        }
        else if(strncmpi(pbuf, "dtsenc_rom",10) == 0)
        {
            DSP_PRINT("dsp_monCmd::info dtsenc_rom\n");
            DSP_DumpData(DSP_DUMP_ROM_DTSENC,0,0);
        }
        else if(strncmpi(pbuf, "wmapro_rom",10) == 0)
        {
            DSP_PRINT("dsp_monCmd::info wmapro_rom\n");
            DSP_DumpData(DSP_DUMP_ROM_WMAPRO,0,0);
        }
        else
        {
            MONTASK_PRINT("dsp_monCmd::info:unknow mon command!\n");
        }

    }
    //
    // mainprogram system command
    //
    else if(strncmpi(pbuf, "sysidle", 7) == 0)
    {
        MONTASK_PRINT("dsp_monCmd::sysidle\n");
        DSP_SetSystemIdle();
    }
    else if(strncmpi(pbuf, "sysready", 8) == 0)
    {
        MONTASK_PRINT("dsp_monCmd::sysready\n");
        DSP_SetSystemReady();
    }
    else if(strncmpi(pbuf, "sysrun", 6) == 0)
    {
        MONTASK_PRINT("dsp_monCmd::sysrun\n");
        DSP_SetSystemRun();
    }

    //
    // dsp message on/off
    //
    else if(strncmpi(pbuf, "msg", 3) == 0)
    {
        extern UINT16 gDspMsgIDFilter;
        extern UINT16 gDspMsgSubIDFilter;
        UINT32 filter;
        pbuf = (const char *)mon_readhex((const UINT8 *)(pbuf+3), (int *)&filter);
        gDspMsgIDFilter =(UINT16) filter&0xFFFF;
        pbuf = (const char *)mon_readhex((const UINT8 *)(pbuf), (int *)&filter);
        gDspMsgSubIDFilter =(UINT16) filter&0xFFFF;

        DSP_PRINT("dsp_monCmd::msg:dspMsgIDFilter:0x%x, dspMsgSubIDFilter:0x%x\n",gDspMsgIDFilter,gDspMsgSubIDFilter);

    }
    //
    // dsp register dump
    //
    else if(strncmpi(pbuf, "reg", 3) == 0)
    {
        UINT32 len = 0;
        UINT32 regbase = 0;

#define REG_G56_BASE		    0xbffe1c00
#define REG_G57_BASE   		    0xbffe1c80
#define REG_G58_BASE   		    0xbffe1d00
#define REG_G59_BASE   		    0xbffe1d80
#define REG_G60_BASE   		    0xbffe1e00
#define REG_G61_BASE   		    0xbffe1e80
#define REG_G62_BASE   		    0xbffe1f00
#define REG_G63_BASE   		    0xbffe1f80
#define REG_G64_BASE   		    0xbffe2000
#define REG_G65_BASE   		    0xbffe2080
#define REG_G66_BASE   		    0xbffe2100
#define REG_G67_BASE   		    0xbffe2180
#define REG_G68_BASE   		    0xbffe2200
#define REG_G69_BASE   		    0xbffe2280
#define REG_G70_BASE   		    0xbffe2300
#define REG_G71_BASE   		    0xbffe2380
#define REG_G72_BASE   		    0xbffe2400
#define REG_G73_BASE   		    0xbffe2480
#define REG_G74_BASE   		    0xbffe2500
#define REG_G75_BASE   		    0xbffe2580
#define REG_G76_BASE   		    0xbffe2600
#define REG_G77_BASE   		    0xbffe2680
#define REG_DSP_BASE_END        0xbffe2700

        pbuf = pbuf+4;							// skip "reg"


        if(strncmpi(pbuf, "all", 3) == 0)
        {
            regbase = REG_G56_BASE;
            len = (REG_DSP_BASE_END - REG_G56_BASE)/4;
        }
        if(strncmpi(pbuf, "g56", 3) == 0)
        {
            regbase = REG_G56_BASE;
            len = 32;
        }
        if(strncmpi(pbuf, "g57", 3) == 0)
        {
            regbase = REG_G57_BASE;
            len = 32;
        }
        if(strncmpi(pbuf, "g58", 3) == 0)
        {
            regbase = REG_G58_BASE;
            len = 32;
        }
        if(strncmpi(pbuf, "g59", 3) == 0)
        {
            regbase = REG_G59_BASE;
            len = 32;
        }
        if(strncmpi(pbuf, "g60", 3) == 0)
        {
            regbase = REG_G60_BASE;
            len = 32;
        }
        if(strncmpi(pbuf, "g61", 3) == 0)
        {
            regbase = REG_G61_BASE;
            len = 32;
        }
        if(strncmpi(pbuf, "g62", 3) == 0)
        {
            regbase = REG_G62_BASE;
            len = 32;
        }
        if(strncmpi(pbuf, "g63", 3) == 0)
        {
            regbase = REG_G63_BASE;
            len = 32;
        }
        if(strncmpi(pbuf, "g64", 3) == 0)
        {
            regbase = REG_G64_BASE;
            len = 32;
        }
        if(strncmpi(pbuf, "g65", 3) == 0)
        {
            regbase = REG_G65_BASE;
            len = 32;
        }
        if(strncmpi(pbuf, "g66", 3) == 0)
        {
            regbase = REG_G66_BASE;
            len = 32;
        }
        if(strncmpi(pbuf, "g67", 3) == 0)
        {
            regbase = REG_G67_BASE;
            len = 32;
        }
        if(strncmpi(pbuf, "g68", 3) == 0)
        {
            regbase = REG_G68_BASE;
            len = 32;
        }
        if(strncmpi(pbuf, "g69", 3) == 0)
        {
            regbase = REG_G69_BASE;
            len = 32;
        }
        if(strncmpi(pbuf, "g70", 3) == 0)
        {
            regbase = REG_G70_BASE;
            len = 32;
        }
        if(strncmpi(pbuf, "g71", 3) == 0)
        {
            regbase = REG_G71_BASE;
            len = 32;
        }
        if(strncmpi(pbuf, "g72", 3) == 0)
        {
            regbase = REG_G72_BASE;
            len = 32;
        }
        if(strncmpi(pbuf, "g73", 3) == 0)
        {
            regbase = REG_G73_BASE;
            len = 32;
        }
        if(strncmpi(pbuf, "g74", 3) == 0)
        {
            regbase = REG_G74_BASE;
            len = 32;
        }
        if(strncmpi(pbuf, "g75", 3) == 0)
        {
            regbase = REG_G75_BASE;
            len = 32;
        }
        if(strncmpi(pbuf, "g76", 3) == 0)
        {
            regbase = REG_G76_BASE;
            len = 32;
        }
        if(strncmpi(pbuf, "g77", 3) == 0)
        {
            regbase = REG_G77_BASE;
            len = 32;
        }

        dumpLongDataLE(regbase,len);

    }
    // audio FIFO pause/play
    else if(strncmpi(pbuf, "fifo", 4) == 0)
    {
        UINT32 fifos;
        pbuf = pbuf+5;							// skip "info " (include space)

        if(strncmpi(pbuf, "enable", 6) == 0)
        {
            DSP_PRINT("dsp_monCmd::fifo:enable\n");
            DSP_SendCmd(RISC_CMD0x01_FIFO_CTRL,CODE0x00_FIFO_ENABLE,0xFFFF,0x0);
        }
        else if (strncmpi(pbuf, "disable", 7) == 0)
        {
            DSP_PRINT("dsp_monCmd::fifo:disable\n");
            DSP_SendCmd(RISC_CMD0x01_FIFO_CTRL,CODE0x00_FIFO_ENABLE,0x0,0x0);
        }
        else if (strncmpi(pbuf, "reset", 5) == 0)
        {
            pbuf = (const char *)mon_readhex((const UINT8 *)(pbuf+5), (int *)&fifos);
            DSP_PRINT("dsp_monCmd::fifo:reset:0x%x\n",fifos);
            DSP_SendCmd(RISC_CMD0x01_FIFO_CTRL,CODE0x01_FIFO_RESET,fifos,0x0);
        }
        else
        {
            pbuf = (const char *)mon_readhex((const UINT8 *)(pbuf+5), (int *)&fifos);
            DSP_PRINT("dsp_monCmd::fifo:enable:0x%x\n",fifos);

            DSP_SendCmd(RISC_CMD0x01_FIFO_CTRL,CODE0x00_FIFO_ENABLE,fifos,0x0);
        }
    }
    else if(strncmpi(pbuf, "code", 4) == 0)
    {
        int codeID;

        pbuf = (const char *)mon_readhex((const UINT8 *)pbuf+4, &codeID);
        if (codeID >MAX_CODEBANK_ID)
        {
            DSP_PRINT("dsp_monCmd::e:codeID:0x%x not exist\n",codeID);
        }
        DSP_DumpDspCodeInfo(codeID);

    }
    else if(strncmpi(pbuf, "check", 5) == 0)
    {
        DSP_PRINT("dsp_monCmd::dsp check\n");
        DSP_CheckCodebank();
    }
    else if(strncmpi(pbuf, "size", 4) == 0)
    {
        DSP_DumpDspSizeInfo();
    }
    else if(strncmpi(pbuf, "prf", 3) == 0)
    {
        extern void DSP_ShowPrfResult(void);
        DSP_PRINT("dsp_monCmd::profiler results:\n");
        DSP_ShowPrfResult();
    }
    else if(strncmpi(pbuf, "test", 4) == 0)
    {


    }
    else if(strncmpi(pbuf, "adcmap", 6) == 0)
    {
        pbuf = pbuf+7;							// skip "adcmap " (include space)
        UINT8 select = AUDIF_ADC_SOURCE_ADC1;

        if(strncmpi(pbuf, "adc0", 4) == 0)
        {
            select = AUDIF_ADC_SOURCE_ADC0;
        }
        else if(strncmpi(pbuf, "adc1", 4) == 0)
        {
            select = AUDIF_ADC_SOURCE_ADC1;
        }
        else if(strncmpi(pbuf, "adc2", 4) == 0)
        {
            select = AUDIF_ADC_SOURCE_ADC2;
        }
        else if(strncmpi(pbuf, "adc3", 4) == 0)
        {
            select = AUDIF_ADC_SOURCE_ADC3;
        }
        else
        {
            diag_printf("[dylan] in-valid params! set default ADC1\n");
        }

        AUDIF_Set_AdcSourceSelect(select);

    }
    else if (strncmpi(pbuf, "dylan", 5) == 0)
    {
        extern AUDIF_Status_t AUDIF_Set_MainChannelOutput(UINT8 mode);
        extern AUDIF_Status_t AUDIF_Set_AuxChannelOutput(UINT8 aux1mode, UINT8 aux2mode);

        pbuf = pbuf+6;
        UINT16 mode;
        if(strncmpi(pbuf, "main", 4) == 0)
        {
            pbuf = (const char *)mon_readhex((const UINT8 *)(pbuf+5), (int *)&mode);
            diag_printf("[dylan] set main mode %d\n", mode);
            AUDIF_Set_MainChannelOutput(mode);
        }
        else if(strncmpi(pbuf, "aux", 3) == 0)
        {
            pbuf = (const char *)mon_readhex((const UINT8 *)(pbuf+4), (int *)&mode);
            diag_printf("[dylan] set aux mode %d\n", mode);
            AUDIF_Set_AuxChannelOutput(mode&0xFF, (mode&0xFF00)>>8);
        }
        else if(strncmpi(pbuf, "path", 4) == 0)
        {
            extern void AUDIF_Init_SifIn(void);
            extern INT32 AUDIF_Set_Volume(UINT8 x);
            AUDIF_Init_SifIn();
            AUDIF_Set_Volume(100);
        }
        else if(strncmpi(pbuf, "reenc", 5) == 0)
        {
            AUDIF_Enable_AC3TransCoding();
            AUDIF_Set_SPDIF(0x3);
        }
        else
        {
            diag_printf("[dylan] invalid command\n");
        }

    }
    else
    {
        MONTASK_PRINT("dsp_monCmd::unknow mon command!\n");
    }

    return;
}
