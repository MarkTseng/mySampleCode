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
	unsigned int write_enable = 0;
	unsigned int dump_file_enable = 0;
	unsigned int page_size = getpagesize();
	unsigned int page_size_mask = getpagesize() - 1;
	int fd,i;
	char * buffer;
	FILE *writeData;

	if(argc < 4) {
		printf("usage: devmem phys_addr r length [dump_file]\n");
		printf("       devmem phys_addr w data_file\n");
		exit(-1);
	}
	fd = open("/dev/mem", O_RDWR|O_SYNC);  
	if (fd == -1)  
	{  
		return (-1);  
	}  
	phys_addr = strtol(argv[1],NULL,16);
	// write 
	if(strncmp(argv[2],"w",1)==0) {
		write_enable = 1;
		if(argv[3] == NULL) {
			printf("no write data file\n");
			exit(-1);
		}
	}else{ // read
		phys_len = strtol(argv[3],NULL,16);
		if(argv[4] != NULL) {
			printf("dump file : %s\n", argv[4]);
			dump_file_enable = 1;
		}
	}
	// cal write data length
	if(write_enable == 1)
	{
		writeData = fopen(argv[3],"r");
		if(writeData == NULL){ 
			printf("open file fail\n");
			exit(-1);
		}
		fseek (writeData , 0 , SEEK_END);
		phys_len = ftell (writeData);
		rewind (writeData);
		buffer = (char*) malloc (sizeof(char)* phys_len);
		if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}
		fread (buffer,1,phys_len,writeData);
		fclose(writeData);
		printf("read file: %s, size: %#x\n", argv[3], phys_len);
	}
	
	// align PAGE_SIZE
	phys_offset = phys_addr & (page_size_mask);
	if(phys_offset != 0) {
		phys_addr = phys_addr & (~page_size_mask); // page size alignment
		phys_len = phys_len + phys_offset + 1;
	}

	printf("start mmap phys addr: %#x, offset: %#x, len: %#x, write: %#x\n", phys_addr, phys_offset, phys_len, write_enable);
	printf("page size = %#x\n",page_size);
	printf("page mask = %#x\n",page_size_mask);

	if(phys_len == 0) {
		printf("length could not be 0\n");
		exit(3);
	}
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

	// write data to phys memory addr
	if(write_enable == 1)
	{
		for (i=phys_offset;i < phys_len; ++i)  
		{ 
			map_base[i] = (unsigned char)buffer[i];  
			printf("updated address: 0x%p, content: 0x%x, buffer: %#x\n", (map_base+i), map_base[i], buffer[i]);  
		}
		free(buffer); 
	} else { // read data from phys memory addr
		if(dump_file_enable == 1)
		{
			buffer = (char*) malloc (sizeof(char)* (phys_len - phys_offset));
			if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}
		}

		for (i=phys_offset;i < phys_len; ++i)  
		{ 
			if(dump_file_enable == 1)
			{
				buffer[i] = map_base[i];
				printf("buffer: 0x%p   content %#x\n", (buffer +i), buffer[i]);  
			}
			printf("address: 0x%p   content %#x\n", (map_base +i), map_base[i]);  
		}

		if(dump_file_enable == 1){
			writeData = fopen(argv[4],"wb");
			if(writeData == NULL){ 
				printf("open file fail\n");
				exit(-1);
			}
			fwrite (buffer , 1, (phys_len - phys_offset), writeData);
			fflush(writeData);
			fclose(writeData);
			free(buffer);
		}
	}
	close(fd);  

	munmap(map_base, phys_len);  

	return (1);  
}  

