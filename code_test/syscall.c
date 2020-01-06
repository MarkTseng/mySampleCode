/*******************************************
* File Name : syscall.c
* Purpose :
* Creation Date : 14-09-2016
* Last Modified : Wed 14 Sep 2016 09:44:56 AM CST
* Created By : Mark Tseng  
**********************************************/

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)
      
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int munmap(void *addr, size_t length);

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	printf("mmap call, addr: %p, length: %d, prot: %d, flags: %d, fd: %d, offset: %lx\n", addr, length, prot, flags, fd, offset);
	return (void *)syscall(SYS_mmap2, addr, length, prot, flags, fd, offset);
}

int munmap(void *addr, size_t length)
{
	printf("munmap call\n");
	return syscall(SYS_munmap, addr, length);
}

int main(int argc, char *argv[])
{
	char *addr;
	int fd;
	struct stat sb;
	off_t offset, pa_offset;
	size_t length;
	ssize_t s;

	if (argc < 3 || argc > 4) {
		fprintf(stderr, "%s file offset [length]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
		handle_error("open");

	if (fstat(fd, &sb) == -1)           /* To obtain file size */
		handle_error("fstat");

	offset = atoi(argv[2]);
	pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);
	/* offset for mmap() must be page aligned */

	if (offset >= sb.st_size) {
		fprintf(stderr, "offset is past end of file\n");
		//exit(EXIT_FAILURE);
	}

	length = 4096;

	addr = mmap(NULL, length + offset - pa_offset, PROT_READ,
			MAP_PRIVATE, fd, pa_offset);
	if (addr == MAP_FAILED)
		handle_error("mmap");

	munmap(addr, length + offset - pa_offset);
	close(fd);

	exit(EXIT_SUCCESS);
}
