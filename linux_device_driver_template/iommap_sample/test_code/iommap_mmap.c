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

#define DEVNAME	"/dev/iommap"
#define MAPSIZE	0x1000000
#define OFFSET	0x0FFFFF0

int main(int argc, char *argv[])
{ 
	int mem_fd,i=0;
	void *uio; 
	unsigned char *pVaddr;
	mem_fd=open(DEVNAME,O_RDWR|O_SYNC);
	if (mem_fd <0) 
	{ 
		printf("open device error\n");
		return -1;
	}

	uio=(unsigned char *)mmap(NULL,MAPSIZE, PROT_READ | PROT_WRITE, MAP_SHARED,mem_fd, 0);

	if (uio == MAP_FAILED) 
	{ 
		printf("iommap == MAP_FAILED \n");
		close(mem_fd);
		exit(-1);
	} 
	
	printf("iommap virtual addr = %p \n",uio);
	pVaddr = (unsigned char*)uio;

	//memset(uio,0,sizeof(unsigned char)*0xF000);

	for(i=0; i< 30 ; i++)
	{
		printf("iommap offset %02d, value = 0x%02lx \n",i,*(unsigned char*)(pVaddr+i+OFFSET));
	}	
	
	munmap(uio, MAPSIZE);

	close(mem_fd);

	return 0;
}
