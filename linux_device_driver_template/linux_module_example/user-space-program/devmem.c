#include <stdio.h>  
#include <unistd.h>  
#include <sys/mman.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) 
{  
	unsigned char * map_base;  
	unsigned int phys_addr;
	unsigned int phys_offset;
	unsigned int phys_len;
	unsigned int page_size = getpagesize();
	unsigned int page_size_mask = getpagesize() - 1;
	int fd,i;  
	unsigned int content;  

	if(argc < 3) {
		printf("usage: devmem phys_addr len\n");
		exit(-1);
	}
	fd = open("/dev/mem", O_RDWR|O_SYNC);  
	if (fd == -1)  
	{  
		return (-1);  
	}  
	phys_addr = strtol(argv[1],NULL,16);
	phys_len = strtol(argv[2],NULL,16);
	phys_offset = phys_addr & (page_size_mask);
	if(phys_offset != 0) {
		phys_addr = phys_addr & (~page_size_mask); // page size alignment
		phys_len = phys_len + phys_offset;
	}
	printf("start mmap phys addr: %#x, offset: %#x, len: %#x\n", phys_addr, phys_offset, phys_len);
	printf("page size = %#x\n",page_size);
	printf("page mask = %#x\n",page_size_mask);

	map_base = mmap(NULL, phys_len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, phys_addr);  

	if (map_base == MAP_FAILED)  
	{  
		perror("map fail\n");  
		exit(-1);
	}  
	else  
	{  
		printf("mmap base: %p\n",map_base);
	}  

	for (i=phys_offset;i < phys_len; ++i)  
	{  
		content = map_base[i];  
		printf("address: 0x%p   content 0x%u\n", (map_base +i), content);  

		//map_base[i] = (unsigned char)i;  
		//content = map_base[i];  
		//printf("updated address: 0x%lx   content 0x%x\n", addr, (unsigned int)content);  
	}  

	close(fd);  

	munmap(map_base, phys_len);  

	return (1);  
}  

