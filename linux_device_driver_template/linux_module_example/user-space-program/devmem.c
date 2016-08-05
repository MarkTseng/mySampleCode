#include<stdio.h>  
#include<unistd.h>  
#include<sys/mman.h>  
#include<sys/types.h>  
#include<sys/stat.h>  
#include<fcntl.h>  

int main()  
{  
	unsigned int * map_base;  
	FILE *f;  
	int n, fd;  

	fd = open("/dev/mem", O_RDWR|O_SYNC);  
	if (fd == -1)  
	{  
		return (-1);  
	}  

	map_base = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x22560000);  

	if (map_base == 0)  
	{  
		printf("NULL pointer!\n");  
	}  
	else  
	{  
		printf("Successfull!\n");  
	}  

	unsigned int addr;  
	unsigned int content;  

	int i = 0;  
	//memset(map_base,0,4096);
	for (;i < 0xff; ++i)  
	{  
		addr = (unsigned long)(map_base + i);  
		content = map_base[i];  
		printf("address: 0x%lx   content 0x%x\n", addr, content);  

		map_base[i] = (unsigned char)i;  
		content = map_base[i];  
		printf("updated address: 0x%lx   content 0x%x\n", addr, (unsigned int)content);  
	}  

	close(fd);  

	munmap(map_base, 4096);  

	return (1);  
}  

