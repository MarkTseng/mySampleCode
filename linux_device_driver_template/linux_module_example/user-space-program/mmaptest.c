#include <stdio.h>  
#include <unistd.h>  
#include <sys/mman.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <stdlib.h>  

#define NPAGES 16  

/* this is a test program that opens the mmap_drv. 
   It reads out values of the kmalloc() and vmalloc() 
   allocated areas and checks for correctness. 
   You need a device special file to access the driver. 
   The device special file is called 'node' and searched 
   in the current directory. 
   To create it 
   - load the driver 
   'insmod mmap_mod.o' 
   - find the major number assigned to the driver 
   'grep mmapdrv /proc/devices' 
   - and create the special file (assuming major number 254) 
   'mknod node c 254 0' 
   */  

int main(void)  
{  
	int fd;  
	unsigned int *vadr;  
	unsigned int *kadr;  

	int len = NPAGES * getpagesize();  
	int i;
	if ((fd=open("/dev/mmap", O_RDWR|O_SYNC))<0)  
	{  
		perror("open");  
		exit(-1);  
	}  

	kadr = mmap(0, len, PROT_READ|PROT_WRITE, MAP_SHARED| MAP_LOCKED, fd, len);  

	if (kadr == MAP_FAILED)  
	{  
		perror("mmap");  
		exit(-1);  
	}  

	if ((kadr[0]!=0xdead0000) || (kadr[1]!=0xbeef0000)  
			|| (kadr[len / sizeof(int) - 2] != (0xdead0000 + len / sizeof(int) - 2))  
			|| (kadr[len / sizeof(int) - 1] != (0xbeef0000 + len / sizeof(int) - 2)))  
	{  
		printf("0x%x 0x%x\n", kadr[0], kadr[1]);  
		printf("0x%x 0x%x\n", kadr[len / sizeof(int) - 2], kadr[len / sizeof(int) - 1]);  
	}  

	close(fd);  
	return(0);  
}

