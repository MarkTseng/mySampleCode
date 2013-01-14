#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

#include "sdebug_api.h"

/*******************************************************************
 *@brief    SDEBUG_Init() 
 *@param    void 
 *@return   void
********************************************************************/
int SDEBUG_Init(struct SerialPortConfigProfile *pSPConfig)
{

	SerialPortConfigProfile_t *SP = (struct SerialPortConfigProfile *)pSPConfig;
	
	// initial Serial Port setting
	SP->SerialPortSetting = malloc(sizeof(struct  termios));
	if(SP->SerialPortSetting == NULL)
		return -1;
	memset(SP->SerialPortSetting,0,sizeof(struct  termios));

	// initial Terminal setting
	SP->TerminalSetting = malloc(sizeof(struct  termios));
	if(SP->TerminalSetting == NULL)
		return -1;
	memset(SP->TerminalSetting,0,sizeof(struct  termios));

	/*
	// initial Sdebug Packet
	SP->SDebug_SendBuf = malloc(sizeof(Packet_t));
	memset(SP->SDebug_SendBuf,0,sizeof(Packet_t));

	SP->SDebug_SendBuf->RegisterValue[0] = CMDSTART;
	SP->SDebug_SendBuf->RegisterValue[1] = 0x04;
	SP->SDebug_SendBuf->RegisterValue[2] = 0x00;
	SP->SDebug_SendBuf->RegisterValue[3] = 0xFD;
	*/

	//initial uio struct
	SP->UioDevice = malloc(sizeof(struct UIOmmap));
	if(SP->UioDevice == NULL)
		return -1;
	memset(SP->UioDevice,0,sizeof(struct UIOmmap));

	return 0;
}

int SDEBUG_Release(struct SerialPortConfigProfile *pSPConfig)
{

	SerialPortConfigProfile_t *SP = (struct SerialPortConfigProfile *)pSPConfig;
	SP->ttyFD = open(SP->ttyName,O_RDWR | O_NONBLOCK);
	free(SP->SerialPortSetting);
	free(SP->TerminalSetting);
	//free(SP->SDebug_SendBuf);
	free(SP->UioDevice);

	return 0;
}

int SDEBUG_OpenDevice(struct SerialPortConfigProfile *pSPConfig)
{
	SerialPortConfigProfile_t *SP = (struct SerialPortConfigProfile *)pSPConfig;
	
	SP->ttyFD = open(SP->ttyName,O_RDWR | O_NOCTTY);
	if(SP->ttyFD == -1)
		return -1;

	return 0;
}

int SDEBUG_CloseDevice(struct SerialPortConfigProfile *pSPConfig)
{
	int ret;
	SerialPortConfigProfile_t *SP = (struct SerialPortConfigProfile *)pSPConfig;
	
	ret = close(SP->ttyFD);
	if(ret == -1)
	{
		return -1;
	}

	return 0;
}

/*
 *
 */
int SDEBUG_PacketChecksum(unsigned char *aData, int DataLen)
{
    int i;
    unsigned char checksum = 0;

    for(i = 0; i < DataLen - 1 ;i++)
        checksum += *(aData + i);

    checksum = ~(checksum) + 1;
	//aData[DataLen - 1] = checksum;

#ifdef DEBUG
	PRINTDEBUGNAME
    printf("aData checksum = 0x%x\n",checksum);
#endif
	return checksum;

}


int SDEBUG_CMDParser(struct SerialPortConfigProfile *pSPConfig)
{
	SerialPortConfigProfile_t *SP = (struct SerialPortConfigProfile *)pSPConfig;

	if(SP->RequestPacket[0] == CMDSTART)
	{

		switch(SP->RequestPacket[1])
		{
			case CMDTINY:
				//checksum check
				if(SDEBUG_PacketChecksum(SP->RequestPacket, CMDTINY) != SP->RequestPacket[CMDTINY-1])
				{
					printf("checksum error: %d\n",SDEBUG_PacketChecksum(SP->RequestPacket, CMDTINY));
					return -1;
				}
				SDEBUG_CMDTINYParser(SP);
				break;
			case CMDSMALL:
				//checksum check
				if(SDEBUG_PacketChecksum(SP->RequestPacket, CMDSMALL) != SP->RequestPacket[CMDSMALL-1])
				{
					printf("checksum error: %d\n",SDEBUG_PacketChecksum(SP->RequestPacket, CMDSMALL));
					return -1;
				}

				SDEBUG_CMDSMALLParser(SP);
				break;
			case CMDLARGE:
				//checksum check
				if(SDEBUG_PacketChecksum(SP->RequestPacket, CMDLARGE) != SP->RequestPacket[CMDLARGE-1])
				{
					printf("checksum error: %d\n",SDEBUG_PacketChecksum(SP->RequestPacket, CMDLARGE));
					return -1;
				}

				SDEBUG_CMDLARGEParser(SP);
				break;
			default:
				printf("unknoew CMD type\n");
				break;
		}

		return 0;
	}else{
		return -1;
	}
	
}

int SDEBUG_CMDTINYParser(struct SerialPortConfigProfile *pSPConfig)
{
	SerialPortConfigProfile_t *SP = (struct SerialPortConfigProfile *)pSPConfig;
	unsigned long value = 0;
	unsigned int RegAddr = 0;

	switch(SP->RequestPacket[2])
	{
		case READ:
			if(SP->RequestPacket[3] == SPV9200ADDR)
			{
				RegAddr = SP->RegBankAddr + SP->RequestPacket[5] * 4;
				if(SP->UioDevice->uioReadCount > 256)
				{
					SDEBUG_UioClose(SP);
					SDEBUG_UioOpen(SP);	
					SP->UioDevice->uioReadCount = 0;
				}
				SDEBUG_UioRead(SP, CMDTINY, RegAddr, &value);
				SP->UioDevice->uioReadCount++;
				printf("RegAddr : 0x%x\n",RegAddr + BASE_PHY_ADDR);
				printf("read value: 0x%lX\n",value);
				printf("read count: %u\n",SP->UioDevice->uioReadCount);
				SDEBUG_WriteRegValueCMD(SP, SP->ttyFD, CMDTINY, value);
			}
					
			break;
		case WRITE:
			if(SP->RequestPacket[3] == SPV9200ADDR)
			{
				RegAddr = SP->RegBankAddr + SP->RequestPacket[5] * 4;
				if(SP->UioDevice->uioReadCount > 256)
				{
					SDEBUG_UioClose(SP);
					SDEBUG_UioOpen(SP);	
					SP->UioDevice->uioReadCount = 0;
				}
				value = SP->RequestPacket[6];
				SDEBUG_UioWrite(SP, CMDTINY, RegAddr, value);
				SP->UioDevice->uioReadCount++;
				printf("RegAddr : 0x%x\n",RegAddr + BASE_PHY_ADDR);
				printf("write value: 0x%lX\n",value);
				printf("read count: %u\n",SP->UioDevice->uioReadCount);
				SDEBUG_WriteRegValueCMD(SP, SP->ttyFD, CMDTINY, value);
				SDEBUG_WriteFF01CMD(SP->ttyFD); 
			}
	
			break;
		case REGTYPE:
			//FF 08 02 68 "00" FF "00" 90 => 00->Bank type, 00-> bank number
			//FF 08 02 68 "05" FF "00" 8B =>  05->group, 00-> group number
			if(SP->RequestPacket[3] == SPV9200ADDR)
			{
				if(SP->RequestPacket[4] == 0x00) //Bank00 ~ Bank11
				{
					// Uio map is from 0xBFFE0000 to 0xBFFEF000 
					SP->RegBankAddr = SP->RequestPacket[6] * 0x400 + (BANK00 - BASE_PHY_ADDR) ;
				}
				else if(SP->RequestPacket[4] == 0x05) //Group00 ~ Group255
				{
					SP->RegBankAddr = SP->RequestPacket[6] * 0x80;
				}	
				SDEBUG_WriteFF01CMD(SP->ttyFD); // write 0xff 0x01 to ask next packet
			}
			break;
	}
	

	return 0;
}

// for sdebug 4.5.1.1
int SDEBUG_CMDSMALLParser(struct SerialPortConfigProfile *pSPConfig)
{
	SerialPortConfigProfile_t *SP = (struct SerialPortConfigProfile *)pSPConfig;
	unsigned long value = 0;
	unsigned int RegAddr = 0;

	switch(SP->RequestPacket[2])
	{
		case READ:
			//FF 0A 00 68 0C "00 1A" "00 FF" 6A => offset "00 1A" ,write value "00 FF"
			if(SP->RequestPacket[3] == SPV9200ADDR)
			{
				RegAddr = SP->RegBankAddr +  (SP->RequestPacket[6] * 4);
				if(SP->UioDevice->uioReadCount > 256)
				{
					SDEBUG_UioClose(SP);
					SDEBUG_UioOpen(SP);	
					SP->UioDevice->uioReadCount = 0;
				}
				SDEBUG_UioRead(SP, CMDLARGE, RegAddr, &value);
				SP->UioDevice->uioReadCount++;
				printf("RegAddr : 0x%x\n",RegAddr + BASE_PHY_ADDR);
				printf("read value: 0x%lX\n",value);
				//printf("read count: %u\n",SP->UioDevice->uioReadCount);
				SDEBUG_WriteRegValueCMD(SP, SP->ttyFD, CMDLARGE, value);
			}
					
			break;
		case WRITE:
			//FF 0A 00 68 0C "00 1A" "00 FF" 6A => offset "00 1A" ,write value "00 FF"
			if(SP->RequestPacket[3] == SPV9200ADDR)
			{
				RegAddr = SP->RegBankAddr + SP->RequestPacket[6] * 4;
				if(SP->UioDevice->uioReadCount > 256)
				{
					SDEBUG_UioClose(SP);
					SDEBUG_UioOpen(SP);	
					SP->UioDevice->uioReadCount = 0;
				}
				value = (SP->RequestPacket[7] << 8) +SP->RequestPacket[8];
				SDEBUG_UioWrite(SP, CMDSMALL, RegAddr, value);
				SP->UioDevice->uioReadCount++;
				//printf("RegAddr : 0x%x\n",RegAddr + BASE_PHY_ADDR);
				//printf("write value: 0x%2lX\n",value);
				//printf("read count: %u\n",SP->UioDevice->uioReadCount);
				//SDEBUG_WriteRegValueCMD(SP, SP->ttyFD, CMDSMALL, value);
				SDEBUG_WriteFF01CMD(SP->ttyFD); 
			}
			break;
		case REGTYPE:
			//FF 0A 02 68 0C 00 FF "01 89" F8  => group 393
			if(SP->RequestPacket[3] == SPV9200ADDR)
			{
				if(SP->RequestPacket[4] == 0x00) //Bank00 ~ Bank11
				{
					// Uio map is from 0xBFFE0000 to 0xBFFEF000 
					SP->RegBankAddr = ((SP->RequestPacket[7] << 8) + SP->RequestPacket[8]) * 0x400 + (BANK00 - BASE_PHY_ADDR) ;
				}
				else if(SP->RequestPacket[4] == 0x0C) //Group00 ~ Group255
				{
					SP->RegBankAddr = ((SP->RequestPacket[7] << 8)  + SP->RequestPacket[8]) * 0x80;
				}	
				SDEBUG_WriteFF01CMD(SP->ttyFD); // write 0xff 0x01 to ask next packet
			}
			break;
		default:
			printf("no this type\n");
			break;
	}
	

	return 0;
}

int SDEBUG_CMDLARGEParser(struct SerialPortConfigProfile *pSPConfig)
{
	SerialPortConfigProfile_t *SP = (struct SerialPortConfigProfile *)pSPConfig;
	unsigned long value = 0;
	unsigned int RegAddr = 0;

	switch(SP->RequestPacket[2])
	{
		//FF 0E 01 68 05 00 00 00 "00" 00 00 03 31 51   00->0ffset
		//FF 0E 00 68 05 00 00 00 "1B" 00 00 00 22 49   00->Offset
		case READ:
			if(SP->RequestPacket[3] == SPV9200ADDR)
			{
				RegAddr = SP->RegBankAddr + SP->RequestPacket[8] * 4;
				if(SP->UioDevice->uioReadCount > 256)
				{
					SDEBUG_UioClose(SP);
					SDEBUG_UioOpen(SP);	
					SP->UioDevice->uioReadCount = 0;
				}
				SDEBUG_UioRead(SP, CMDLARGE, RegAddr, &value);
				SP->UioDevice->uioReadCount++;
				printf("RegAddr : 0x%x\n",RegAddr + BASE_PHY_ADDR);
				printf("read value: 0x%lX\n",value);
				printf("read count: %u\n",SP->UioDevice->uioReadCount);
				SDEBUG_WriteRegValueCMD(SP, SP->ttyFD, CMDLARGE, value);
			}
					
			break;
		case WRITE:
			if(SP->RequestPacket[3] == SPV9200ADDR)
			{
				RegAddr = SP->RegBankAddr + SP->RequestPacket[8] * 4;
				if(SP->UioDevice->uioReadCount > 256)
				{
					SDEBUG_UioClose(SP);
					SDEBUG_UioOpen(SP);	
					SP->UioDevice->uioReadCount = 0;
				}
				value = (SP->RequestPacket[9] << 24) + (SP->RequestPacket[10] << 16) + (SP->RequestPacket[11] << 8) +SP->RequestPacket[12];
				SDEBUG_UioWrite(SP, CMDLARGE, RegAddr, value);
				SP->UioDevice->uioReadCount++;
				printf("RegAddr : 0x%x\n",RegAddr + BASE_PHY_ADDR);
				printf("write value: 0x%2lX\n",value);
				printf("read count: %u\n",SP->UioDevice->uioReadCount);
				//SDEBUG_WriteRegValueCMD(SP, SP->ttyFD, CMDLARGE, value);
				SDEBUG_WriteFF01CMD(SP->ttyFD); 
			}
			break;
		case REGTYPE:
			// don't need to implement this type
			break;
	}
	

	return 0;
}

int SDEBUG_ReadRequestCMD(struct SerialPortConfigProfile *pSPConfig, int ttyFD)
{
	int nByte;
	SerialPortConfigProfile_t *SP = (struct SerialPortConfigProfile *)pSPConfig;
	
	memset(SP->RequestPacket,0,sizeof(SP->RequestPacket));	
	// block in here
	nByte = read(ttyFD, SP->RequestPacket, sizeof(SP->RequestPacket));

	/* capture ctrl + c */
	if(SP->RequestPacket[0] == CTRLC)
		return -1;

	return 0;
}


int SDEBUG_WriteFF01CMD(int ttyFD)
{
	unsigned char FF01[2] = {0xFF,0x01};
	
	write(ttyFD,FF01 ,sizeof(FF01));
	
	return 0;
}

int SDEBUG_WriteRegValueCMD(struct SerialPortConfigProfile *pSPConfig, int ttyFD, int len, unsigned int value)
{
	SerialPortConfigProfile_t *SP = (struct SerialPortConfigProfile *)pSPConfig;

	// clear RespondPacket variable
	memset(SP->RespondPacket,0,sizeof(SP->RespondPacket));
	
	SP->RespondPacket[0] = CMDSTART;
	switch(len)
	{
		case CMDTINY:
			SP->RespondPacket[1] = RECEIVEBYTE;
			SP->RespondPacket[2] = (unsigned char)(value & 0x000000FF); 
			SP->RespondPacket[3] = SDEBUG_PacketChecksum(SP->RespondPacket, RECEIVEBYTE); 
			write(ttyFD, SP->RespondPacket, RECEIVEBYTE);
			break;
		case CMDSMALL:
			SP->RespondPacket[1] = RECEIVEWORD;
			SP->RespondPacket[2] = (unsigned char)(value >>  8); 
			SP->RespondPacket[3] = (unsigned char)(value & 0x000000FF); 
			SP->RespondPacket[4] = SDEBUG_PacketChecksum(SP->RespondPacket, RECEIVEWORD); 
			write(ttyFD, SP->RespondPacket, RECEIVEWORD);
			break;
		case CMDLARGE:
			SP->RespondPacket[1] = RECEIVEDWORD;
			SP->RespondPacket[2] = (unsigned char)(value >> 24); 
			SP->RespondPacket[3] = (unsigned char)(value >> 16); 
			SP->RespondPacket[4] = (unsigned char)(value >>  8); 
			SP->RespondPacket[5] = (unsigned char)(value & 0x000000FF); 
			SP->RespondPacket[6] = SDEBUG_PacketChecksum(SP->RespondPacket, RECEIVEDWORD); 
			write(ttyFD, SP->RespondPacket, RECEIVEDWORD);
			break;
	}
	
	return 0;
}

int SDEBUG_SetTTYConfig(struct SerialPortConfigProfile *pSPConfig)
{	
	SerialPortConfigProfile_t *SP = (struct SerialPortConfigProfile *)pSPConfig;

    SP->SerialPortSetting->c_iflag=0;
    SP->SerialPortSetting->c_oflag=0;
    SP->SerialPortSetting->c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
    SP->SerialPortSetting->c_lflag=0;
    SP->SerialPortSetting->c_cc[VMIN]=1;
    SP->SerialPortSetting->c_cc[VTIME]=10;
    cfsetospeed(SP->SerialPortSetting,B115200);            // 115200 baud
    cfsetispeed(SP->SerialPortSetting,B115200);            // 115200 baud
    tcsetattr(SP->ttyFD,TCSANOW,SP->SerialPortSetting);

#ifdef DEBUG
	PRINTDEBUGNAME
	printf("open %s device, fd = %d\n", SP->ttyName, SP->ttyFD);
	printf("Terminal cflag=%08x\n", SP->TerminalSetting->c_cflag);
	printf("Terminal oflag=%08x\n", SP->TerminalSetting->c_oflag);
	printf("Terminal iflag=%08x\n", SP->TerminalSetting->c_iflag);
	printf("Terminal lflag=%08x\n", SP->TerminalSetting->c_lflag);
	printf("Terminal line=%02x\n", SP->TerminalSetting->c_line);
#endif

	return 0;
}

int SDEBUG_GetTTYConfig(struct SerialPortConfigProfile *pSPConfig) 
{
	SerialPortConfigProfile_t *SP = (struct SerialPortConfigProfile *)pSPConfig;
	tcgetattr(SP->ttyFD,SP->TerminalSetting);

#ifdef DEBUG
	PRINTDEBUGNAME
	printf("open %s device, fd = %d\n", SP->ttyName, SP->ttyFD);
	printf("Terminal cflag=%08x\n", SP->TerminalSetting->c_cflag);
	printf("Terminal oflag=%08x\n", SP->TerminalSetting->c_oflag);
	printf("Terminal iflag=%08x\n", SP->TerminalSetting->c_iflag);
	printf("Terminal lflag=%08x\n", SP->TerminalSetting->c_lflag);
	printf("Terminal line=%02x\n", SP->TerminalSetting->c_line);
#endif

	return 0;
}

int SDEBUG_RestoreTTYSetting(struct SerialPortConfigProfile * pSPConfig)
{
	SerialPortConfigProfile_t *SP = (struct SerialPortConfigProfile *)pSPConfig;

    tcsetattr(SP->ttyFD,TCSANOW,SP->TerminalSetting);
	return 0;
}

void SDEBUG_Usage()
{
	printf("Usage:\n");
	printf("-n\tttyname, default /dev/ttyS0\n");
	printf("-d\tdaemon mode\n");
	printf("example ./sdebug -n /dev/ttys1 -d\n");
	exit(0);
}

int SDEBUG_UioOpen(struct SerialPortConfigProfile * pSPConfig)
{
	SerialPortConfigProfile_t *SP = (struct SerialPortConfigProfile *)pSPConfig;
	SP->UioDevice->uioFD = open(UIONAME,O_RDWR|O_SYNC);
	if(SP->UioDevice->uioFD == -1)
	{
		perror("open uio fail\n");
		return -1;
	}

	SP->UioDevice->uioVirtualAddr = mmap(NULL,UIOMAPSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, SP->UioDevice->uioFD, 0);
	if (SP->UioDevice->uioVirtualAddr == MAP_FAILED) 
	{ 
		printf("uio MAP_FAILED \n");
		close(SP->UioDevice->uioFD);
		return -1;
	} 
	
	return 0;
}

int SDEBUG_UioClose(struct SerialPortConfigProfile * pSPConfig)
{
	SerialPortConfigProfile_t *SP = (struct SerialPortConfigProfile *)pSPConfig;
	munmap(SP->UioDevice->uioVirtualAddr,UIOMAPSIZE);
	close(SP->UioDevice->uioFD);
	
	return 0;
}

int SDEBUG_UioRead(struct SerialPortConfigProfile * pSPConfig, int len, unsigned int RegAddr, unsigned long *buf)
{
	SerialPortConfigProfile_t *SP = (struct SerialPortConfigProfile *)pSPConfig;
	void *VirtualAddr;

	//VirtualAddr = SP->UioDevice->uioVirtualAddr + (RegAddr & MAP_MASK);
	VirtualAddr = SP->UioDevice->uioVirtualAddr + RegAddr;

	switch(len)
	{
		case CMDTINY:
				*buf = *((unsigned char *)VirtualAddr);
			break;
		case CMDSMALL:
				*buf = *((unsigned short *)VirtualAddr);
			break;
		case CMDLARGE:
				*buf = *((unsigned long *)VirtualAddr);
			break;
	}
	return 0;
}

int SDEBUG_UioWrite(struct SerialPortConfigProfile * pSPConfig, int len, unsigned int RegAddr, int value)
{	
	SerialPortConfigProfile_t *SP = (struct SerialPortConfigProfile *)pSPConfig;
	void *VirtualAddr;

	//VirtualAddr = SP->UioDevice->uioVirtualAddr + (RegAddr & MAP_MASK);
	VirtualAddr = SP->UioDevice->uioVirtualAddr + RegAddr;

	switch(len)
	{
		case CMDTINY:
				*((unsigned char *)VirtualAddr) = value;
			break;
		case CMDSMALL:
				*((unsigned short *)VirtualAddr) = value;
			break;
		case CMDLARGE:
				*((unsigned long *)VirtualAddr) = value;
			break;
	}
	return 0;
}
