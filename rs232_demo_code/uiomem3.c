/*
 * uiomem3.c: Simple program to read/write from/to maps exported by UIO drivers
 *
 * Copyright (C) 2011, Nanometrics Inc.
 * Ben Gardiner <bengardiner@nanometrics.ca>
 *
 * Based on:
 * devmem2.c: Simple program to read/write from/to any location in memory.
 *
 *  Copyright (C) 2000, Jan-Derk Bakker (J.D.Bakker@its.tudelft.nl)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>

#define FATAL do { fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", \
  __LINE__, __FILE__, errno, strerror(errno)); exit(1); } while(0)

//Treat all UIO maps as page-sized maps
#define MAP_SIZE (getpagesize())
#define MAP_MASK (MAP_SIZE - 1)

int main(int argc, char **argv) {
	int fd;
	void *map_base, *virt_addr;
	unsigned long read_result, writeval, map_number;
	off_t offset;
	int access_type = 'w';

	if(argc < 4) {
		fprintf(stderr,
			"Usage: %s UIO-DEVICE MAP-NUMBER OFFSET [ TYPE [ DATA ] ]\n"
			"Read-from or write-to the MAP-NUMBER of UIO-DEVICE at OFFSET.\n"
			"\n"
			"\tUIO-DEVICE\tthe UIO device to open e.g. /dev/uio0\n"
			"\tMAP-NUMBER\tthe number of the UIO map in which to operate\n"
			"\tOFFSET\t\tmemory offset in the map upon which to act\n"
			"\tTYPE\t\taccess operation type : [b]yte, [h]alfword, [w]ord (default) \n"
			"\tDATA\t\tdata to be written\n\n"
			"When DATA is not given OFFSET is read, otherwise DATA is written to OFFSET\n",
			argv[0]);
		exit(1);
	}
	map_number = strtoul(argv[2], 0, 0);
	offset = strtoul(argv[3], 0, 0);

	if(argc > 4)
		access_type = tolower(argv[4][0]);

	if((fd = open(argv[1], O_RDWR | O_SYNC)) == -1) FATAL;
	printf("%s opened.\n", argv[1]);
	fflush(stdout);

	map_base = mmap(NULL, MAP_SIZE,
			PROT_READ | (argc > 5) ? PROT_WRITE : 0,
			MAP_SHARED, fd,
			map_number * getpagesize());

	if(map_base == MAP_FAILED) FATAL;
	fflush(stdout);

	virt_addr = map_base + offset ;
	switch(access_type) {
		case 'b':
			read_result = *((unsigned char *) virt_addr);
			break;
		case 'h':
			read_result = *((unsigned short *) virt_addr);
			break;
		case 'w':
			read_result = *((unsigned long *) virt_addr);
			break;
		default:
			fprintf(stderr, "Illegal data type '%c'.\n", access_type);
			exit(2);
	}
	printf("Value at offset 0x%lX in map #%lu of %s (%p): 0x%lX\n", offset,
	       map_number, argv[1],
	       virt_addr, read_result);
	fflush(stdout);

	if(argc > 5) {
		writeval = strtoul(argv[5], 0, 0);
		switch(access_type) {
			case 'b':
				*((unsigned char *) virt_addr) = writeval;
				read_result = *((unsigned char *) virt_addr);
				break;
			case 'h':
				*((unsigned short *) virt_addr) = writeval;
				read_result = *((unsigned short *) virt_addr);
				break;
			case 'w':
				*((unsigned long *) virt_addr) = writeval;
				read_result = *((unsigned long *) virt_addr);
				break;
		}
		printf("Written 0x%lX; readback 0x%lX\n", writeval, read_result);
		fflush(stdout);
	}

	if(munmap(map_base, MAP_SIZE) == -1) FATAL;
	close(fd);
	return 0;

}
