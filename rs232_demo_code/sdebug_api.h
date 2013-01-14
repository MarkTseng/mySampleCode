#ifndef SDEBUG_API

#define	SDEBUG_API

#define TTYNAME	"/dev/ttyS0"
#define BAUDRATE B115200
#define PRINTDEBUGNAME printf("fun: %s - line: %d\n",__func__,__LINE__);

/*
 BASE_PHY_ADDR = 0xbffe0000 in CPU view, but in CBUS view is 0x1FFE0000 (/dev/uio0)
 example: access Probe 17 sel register (Bank0 + C0) => 0xbffe8000 + (C0*4) = 0xbffe8300
*/
#define BASE_PHY_ADDR 0xbffe0000

#define BANK00 0xbffe8000
#define BANK01 0xbffe8400
#define BANK02 0xbffe8800
#define BANK03 0xbffe8c00
#define BANK04 0xbffe9000
#define BANK05 0xbffe9400
#define BANK06 0xbffe9800 
#define BANK07 0xbffe9c00
#define BANK08 0xbffeA000
#define BANK09 0xbffeA400
#define BANK0A 0xbffeA800
#define BANK0B 0xbffeAc00
#define BANK0C 0xbffeB000
#define BANK0D 0xbffeB400
#define BANK0E 0xbffeB800
#define BANK0F 0xbffeBc00
#define BANK10 0xbffeC000
#define BANK11 0xbffeC400

//Start-byte:
#define CMDSTART				0xFF	//define the start of command package

//Total package length:
//a. Command Package
#define CONTROLCMD				4		//Control command
#define IRCMD					5		//IR control command
#define CMDTINY					8		//Byte command (read/write)
#define CMDSMALL				10		//Word command (read/write)
#define CMDLARGE				14		//Double-Word command (read/write)
#define ReadTableCmdLen			6   	//Read Gamma/UV/BW table command
#define WriteTableCmdLen		21  	//Write Gamma/UV/BW table command
#define WriteDramCmdLen			71  	//Write Dram command

//b. Respond Package
#define RESPONSION				1   	//Control/IR/Write command respond
#define RECEIVEBYTE				4		//Read byte command respond

#define RECEIVEWORD				5		//Read word command respond
#define RECEIVEDWORD			7		//Read double-word command respond
#define RespondTableLen			19  	//Read Gamma/BW/UV table respond
#define RespondDramLen			67  	//Read Dram respond


//Read or Write control
#define READ		   			0x01
#define WRITE		  			0x00
#define REGTYPE					0x02 	//choose BANK or GROUP

//CTRL+C
#define CTRLC					0x03
// Chip address
#define SPV9200ADDR				0x68

#define BANK2REGADDR (BankNum,Offset) BankNum + (Offset * 4) 
#define GROUP2REGADDR (GroupNum,Offset) GroupNum + (Offset * 4) 

/*
typedef struct Packet 
{
	unsigned char RequestData[128]; // receive from sdebug 
	unsigned char RegisterValue[128];// = {0xFF,0x04,0x84,0x79};
}Packet_t;
*/

#define UIONAME "/dev/uio0"
#define UIOMAPSIZE	0xF000
#define MAP_SIZE (getpagesize())
#define MAP_MASK (MAP_SIZE - 1)

typedef struct UIOmmap
{
	int		uioFD;				// uio file description
	void 	*uioVirtualAddr;	// uio virtual address
	int		uioReadCount;		// avoid uio driver bug
}__attribute__((packed)) UIOmmap_t;

typedef struct SerialPortConfigProfile{
	struct	termios	*SerialPortSetting;
	struct	termios	*TerminalSetting;
	char	ttyName[128];
	int		DebugLevel;
	int		ttyFD;
	//Packet_t	*SDebug_SendBuf;	

	// cmd packet info
	unsigned char 	RequestPacket[128]; // receive from sdebug 
	unsigned char 	RespondPacket[128]; // send to sdebug = {0xFF,0x04,0x84,0x79};
	int				RequestType; 	// Read / Write / BankorGroup
	unsigned int	RegBankAddr;		// addr_byte
	// end of cmd packet info

	// uio struct
	struct UIOmmap	*UioDevice;
	//end of uio struct

}__attribute__((packed)) SerialPortConfigProfile_t;


int SDEBUG_PacketChecksum(unsigned char *aData, int DataLen);
int SDEBUG_Init(struct SerialPortConfigProfile *pSPConfig);
int SDEBUG_Release(struct SerialPortConfigProfile *pSPConfig);
int SDEBUG_CMDParser(struct SerialPortConfigProfile *pSPConfig);
int SDEBUG_ReadRequestCMD(struct SerialPortConfigProfile *pSPConfig, int ttyFD);
int SDEBUG_CMDTINYParser(struct SerialPortConfigProfile *pSPConfig);
int SDEBUG_CMDSMALLParser(struct SerialPortConfigProfile *pSPConfig);
int SDEBUG_CMDLARGEParser(struct SerialPortConfigProfile *pSPConfig);
int SDEBUG_WriteRegValueCMD(struct SerialPortConfigProfile *pSPConfig, int ttyFD, int len, unsigned int value);
int SDEBUG_WriteFF01CMD(int ttyFD);
int SDEBUG_SetTTYConfig(struct SerialPortConfigProfile *pSPConfig);
int SDEBUG_GetTTYConfig(struct SerialPortConfigProfile *pSPConfig);
int SDEBUG_OpenDevice(struct SerialPortConfigProfile *pSPConfig);
int SDEBUG_CloseDevice(struct SerialPortConfigProfile *pSPConfig);
int SDEBUG_RestoreTTYSetting(struct SerialPortConfigProfile * pSPConfig);
int SDEBUG_UioOpen(struct SerialPortConfigProfile * pSPConfig);
int SDEBUG_UioClose(struct SerialPortConfigProfile * pSPConfig);
int SDEBUG_UioRead(struct SerialPortConfigProfile * pSPConfig, int len, unsigned int RegAddr,unsigned long *buf);
int SDEBUG_UioWrite(struct SerialPortConfigProfile * pSPConfig, int len, unsigned int RegAddr, int value);
void SDEBUG_Usage();

#endif
