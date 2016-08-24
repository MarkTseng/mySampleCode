#include <stdio.h>  
#include <unistd.h>  
#include <sys/mman.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

#define REGISTER_BASE_ADDR (0x1F000000)
#define MAX_READ_LINE (0x1000)
#define BANK_SIZE (0x200)
#define MAX_BANK_NUM (0x3000)

static int register_flag;
static int bank_flag;
static int file_flag;
static int write_flag;
static int length_flag;
static int dump_flag;
static int image_flag;
static int memaddr_flag;
char *fileName = NULL;
char *memAddr = NULL;
char *bankAddr = NULL;
char *registerAddr = NULL;
char *writeVal = NULL;
char *length = NULL;
unsigned int * mmapBase = NULL;
unsigned int const page_size_mask = 0xFFF;
unsigned int phys_offset;

unsigned int getRegAddrVal(unsigned int regAddr) {
	unsigned int val;
	regAddr -= REGISTER_BASE_ADDR;
	if(mmapBase != NULL) {
		val = *(mmapBase + (regAddr/sizeof(unsigned int)));  
	}
	return val;
}

unsigned int setRegAddrVal(unsigned int regAddr, unsigned int val) {
	regAddr -= REGISTER_BASE_ADDR;
	if(mmapBase != NULL) {
		*(mmapBase + (regAddr/sizeof(unsigned int))) = val;  
	}
	return 0;
}

unsigned int getTvtoolBankAddrVal(unsigned int bank, unsigned char offset) {
	unsigned int phys_addr;
	unsigned int val;
	phys_addr = ((bank*BANK_SIZE) + ((offset)*0x04));
	if(mmapBase != NULL) {
		val = *(mmapBase + (phys_addr/sizeof(unsigned int)));  
	}
	return val;
}

unsigned int setTvtoolBankAddrVal(unsigned int bank, unsigned char offset, short val) {
	unsigned int phys_addr;
	phys_addr = ((bank*BANK_SIZE) + ((offset>>1)*0x04));
	
	if(mmapBase != NULL) {
		*(mmapBase + (phys_addr/sizeof(unsigned int))) = val;  
	}
	return 0;
}

int devMemMMap(unsigned int phys_addr, unsigned int length) 
{  
	int fd;

	fd = open("/dev/mem", O_RDWR|O_SYNC);  
	if (fd == -1)  
	{  
		perror("open /dev/mem fail\n");
		exit(1);  
	}  

	phys_offset = phys_addr & (page_size_mask);
	phys_addr &= ~(page_size_mask);
	mmapBase = mmap(NULL, length + phys_offset , PROT_READ|PROT_WRITE, MAP_SHARED, fd, phys_addr);  

	printf("phys_addr: %#x\n", phys_addr);
	printf("phys_offset: %#x\n", phys_offset);
	if (mmapBase == MAP_FAILED)  
	{  
		perror("mmap fail\n");  
		close(fd);  
		return (-ENOMEM);
	}  
	else  
	{  
		printf("mmap base: %p\n",mmapBase);
	}  
	close(fd);  
	return 0;
}  

int devMemUmap(unsigned int *map_base, unsigned int length) 
{
	int ret = 0;

	ret = munmap(map_base, length + phys_offset);  
	if(ret != 0)
		printf("munmap fail\n");
	return ret;
}

int doCMD(char *cmd,char *op,char *addr,char *value){
	unsigned int phys_addr;
	unsigned int val;
	if(strncmp(cmd,"wriu",4)==0) {
		phys_addr = strtol(addr,NULL,16);
		val = strtol(value,NULL,16);
		setTvtoolBankAddrVal(phys_addr>>8,(phys_addr&0xff),val);
	}else if (strncmp(cmd,"wait",4)==0){
		val = strtol(value,NULL,10);
		printf("wait %d\n", val);
		usleep(val*1000);
	}
	return 0;
}

int doFileParser()
{
	unsigned long fileSize;
	char * buffer;
	char cmd[16];
	char op[16];
	char addr[16];
	char value[16];
	FILE *writeData;
	
	writeData = fopen(fileName,"r");
	if(writeData == NULL){ 
		printf("open file fail\n");
		exit(-1);
	}
	fseek(writeData , 0 , SEEK_END);
	fileSize = ftell(writeData);
	rewind(writeData);
	buffer = (char*) malloc(sizeof(char)* MAX_READ_LINE);
	if (buffer == NULL) {
		fputs ("Memory error",stderr); 
		exit (2);
	}

	// start parser tvTool cmd : wriu , //, wait 
	do{
		memset(buffer, 0 , (sizeof(char)* MAX_READ_LINE));
		fgets(buffer,MAX_READ_LINE, writeData);
		if(buffer[0]=='\n')
			continue;
  		if(strncmp(buffer,"//",2) == 0) {
			printf("skip parser: %s\n",buffer);
			continue;
		}else if(strncmp(buffer,"wriu",4) == 0) {
			sscanf(buffer,"%s %s %s %s\n",cmd,op,addr,value);
			//printf("line : %s",buffer);
			//printf("cmd : %s\n",cmd);
			//printf("op : %s\n",op);
			//printf("addr : %s\n",addr);
			//printf("vlaue : %s\n",value);
			//printf("\n");
		}else{
			if(strncmp(buffer,"wait",4) == 0) {
			sscanf(buffer,"%s %s\n",cmd,value);
			//printf("line : %s",buffer);
			//printf("cmd : %s\n",cmd);
			//printf("value : %s\n",value);
			}
		}
		doCMD(cmd,op,addr,value);
	}while(!feof(writeData));

	printf("do file: %s done, size: %#lx\n", fileName, fileSize);
	fclose(writeData);
	if(fileName != NULL) {
		free(fileName);
		fileName = NULL;
	}
	free(buffer);

	return 0; 
}

void showBankVal(unsigned int bank)
{
	int i;
	printf("Bank %#x\n=====================================================================================\n\t", bank );
	for(i=0;i<0x10;i++) {
		printf("%x    ",i);
	}
	printf("\n");
	for(i=0;i<=0x7f;i++) {
		if(i%0x10 == 0){
			printf("%05x: ",i);
		}
		printf("%04x ", getTvtoolBankAddrVal(bank, i));
		if(i%0x10 == 15) {
			printf("\n");
		}
	}
	printf("\n");

}

int doDumpMemData(char *fileName, unsigned int imem_addr, unsigned int ilength) 
{
	int i;
	FILE *writeData;
	unsigned char *remmapBase; 
	printf("file: %s, imem_add: %#x, ilength: %#x\n", fileName, imem_addr, ilength); 

	writeData = fopen(fileName,"wb");
	if(writeData == NULL){ 
		printf("open file fail\n");
		exit(-1);
	}
	remmapBase = (unsigned char *)(mmapBase);
	remmapBase += phys_offset;
	fwrite ((remmapBase) , 1, ilength, writeData);
#if 0
	for(i=0; i<0x10; i++)
	{
		printf("%p data:%#x\n", (remmapBase+i) ,*(remmapBase+i));
	}
#endif
	fflush(writeData);
	fclose(writeData);

	return 0;
}

int doLoadFileToMem(char *fileName, unsigned int imem_addr)
{
	int i;
	FILE *readData;
	unsigned char *remmapBase; 
	unsigned int fileSize;
	readData = fopen(fileName,"rb");
	fseek (readData , 0 , SEEK_END);
	fileSize = ftell (readData);
	rewind (readData);
	printf("file: %s, imem_add: %#x, size:%#x\n", fileName, imem_addr, fileSize); 

	devMemMMap(imem_addr , fileSize);
	if(readData == NULL){ 
		printf("open file fail\n");
		exit(-1);
	}
	remmapBase = (unsigned char *)(mmapBase);
	remmapBase += phys_offset;
	fread (remmapBase,1,fileSize,readData);
#if 0
	for(i=0; i<0x10; i++)
	{
		printf("%p data:%#x\n", (remmapBase+i) ,*(remmapBase+i));
	}
#endif
	fclose(readData);
	devMemUmap(mmapBase, fileSize);


	return 0;
}

int option(int argc, char **argv)
{
	int c,i;
	long int ireg_addr, ilength,imem_addr;

	while (1)
	{
		static struct option long_options[] =
		{
			{"register",	required_argument, &register_flag, 1},
			{"bank",  		required_argument, &bank_flag, 1},
			{"file",  		required_argument, &file_flag, 1},
			{"write",  		required_argument, &write_flag, 1},
			{"length",  	required_argument, NULL, 'l'},
			{"memaddr",  	required_argument, NULL, 'm'},
			{"image",  	required_argument, NULL, 'i'},
			{"dump",  	required_argument, NULL, 'd'},
			{0, 0, 0, 0}
		};
		int option_index = 0;

		c = getopt_long (argc, argv, "r:b:f:w:l:m:i:d:", long_options, &option_index);

		if (c == -1)
			break;

		switch (c)
		{
			// debug -- option , -r long option is --register
			case 0:
				printf ("option %s", long_options[option_index].name);
				if (optarg)
					printf (" with arg %s", optarg);
				printf ("\n");
				break;

			case 'r':
				registerAddr = (char *)malloc(strlen(optarg)+1);
				strncpy(registerAddr, optarg, strlen(optarg)+1);
				register_flag = 1;
				break;

			case 'b':
				bankAddr = (char *)malloc(strlen(optarg)+1);
				strncpy(bankAddr, optarg, strlen(optarg)+1);
				bank_flag = 1;
				break;

			case 'l':
				length = (char *)malloc(strlen(optarg)+1);
				strncpy(length, optarg, strlen(optarg)+1);
				length_flag = 1;
				break;

			case 'd':
				fileName = (char *)malloc(strlen(optarg)+1);
				strncpy(fileName, optarg, strlen(optarg)+1);
				dump_flag = 1;
				break;

			case 'm':
				memAddr = (char *)malloc(strlen(optarg)+1);
				strncpy(memAddr, optarg, strlen(optarg)+1);
				memaddr_flag = 1;
				break;

			case 'i':
				fileName = (char *)malloc(strlen(optarg)+1);
				strncpy(fileName, optarg, strlen(optarg)+1);
				image_flag = 1;
				break;

			case 'f':
				fileName = (char *)malloc(strlen(optarg)+1);
				strncpy(fileName, optarg, strlen(optarg)+1);
				file_flag = 1;
				doFileParser();
				break;

			case 'w':
				writeVal = (char *)malloc(strlen(optarg)+1);
				strncpy(writeVal, optarg, strlen(optarg)+1);
				//strncpy()
				write_flag = 1;
				break;

			case '?':
				/* getopt_long already printed an error message. */
				break;

			default:
				abort();
		}
	}

	/* show usage  */
	if (argc<2)
	{
		printf("Usage:\n");
		printf("\t./tvTool -r reg_addr\n");
		printf("\t./tvTool -r reg_addr -l length\n");
		printf("\t./tvTool -r reg_addr -w val\n");
		printf("\t./tvTool -b bank , ISP bank start from 0x1302\n");
		printf("\t./tvTool -f tvTool_script\n");
		printf("\t./tvTool -d dumpfile -m memory_addr -l length\n");
		printf("\t./tvTool -i image_bin_file -m memory_addr \n");
		exit(1);
	}

	//dommap first
	if((register_flag ==1)||(bank_flag==1)||(file_flag==1))
	{
		devMemMMap(REGISTER_BASE_ADDR, BANK_SIZE*MAX_BANK_NUM);
	}

	if(bank_flag ==1)
	{
		showBankVal(strtol(bankAddr,NULL,16));
	}

	if((write_flag == 1) && (register_flag = 1))
	{
		ireg_addr = strtol(registerAddr, NULL, 16);
		setRegAddrVal(ireg_addr, strtol(writeVal,NULL,16));
		printf("set reg %#lx : %#x\n", ireg_addr, getRegAddrVal(ireg_addr));
		return 0;
	}
	
	if((dump_flag ==1) && (memaddr_flag==1) && (length_flag == 1)) {
		printf("dump mem data\n");
		imem_addr = strtol(memAddr, NULL, 16);
		ilength = strtol(length, NULL, 16);
		devMemMMap(imem_addr , ilength);
		doDumpMemData(fileName, imem_addr, ilength);
		devMemUmap(mmapBase, ilength);
		return 0;
	}

	if((image_flag ==1) && (memaddr_flag==1)) {
		printf("load image to mem\n");
		imem_addr = strtol(memAddr, NULL, 16);
		doLoadFileToMem(fileName, imem_addr);
		return 0;
	}

	if ((register_flag ==1)&&(length_flag==1))
	{
		ireg_addr = strtol(registerAddr, NULL, 16);
		ilength = strtol(length, NULL, 16);
		for(i=0;i<ilength;i+=4)
			printf("reg %#lx : %#x\n", ireg_addr+i, getRegAddrVal(ireg_addr +i));
		return 0;
	}

	if((register_flag ==1))
	{
		ireg_addr = strtol(registerAddr, NULL, 16);
		printf("reg %#lx : %#x\n", ireg_addr, getRegAddrVal(ireg_addr));
		return 0;
	}

	//doUnmmap
	if((register_flag ==1)||(bank_flag==1)||(file_flag==1))
	{
		devMemUmap(mmapBase, BANK_SIZE*MAX_BANK_NUM);
	}
	return 0;
}

int main(int argc, char *argv[]) 
{  
	option(argc, argv);

	// memory free check
	if(fileName != NULL)
		free(fileName);
	if(registerAddr != NULL)
		free(registerAddr);
	if(writeVal != NULL)
		free(writeVal);
	if(length != NULL)
		free(length);
	if(bankAddr != NULL)
		free(bankAddr);
	if(memAddr != NULL)
		free(memAddr);

	return 0;  
}  

