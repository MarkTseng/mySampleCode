/*
 * wmapper.c -- simple file that mmap()s a file region and writes to it
 *
 * Copyright (C) 1998,2000,2001 Alessandro Rubini
 * 
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

int main(int argc, char **argv)
{
    char *fname;
    FILE *f;
    unsigned int offset, len;
    unsigned char *address;
    int i, c;

    if (argc !=4
       || sscanf(argv[2],"%i", &offset) != 1
       || sscanf(argv[3],"%i", &len) != 1) {
        fprintf(stderr, "%s: Usage \"%s <file> <offset> <len>\"\n", argv[0],
                argv[0]);
        exit(1);
    }
    fname=argv[1];

    if (!(f=fopen(fname,"r+"))) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], fname, strerror(errno));
        exit(1);
    }

    address=mmap(0, len, PROT_READ | PROT_WRITE,
		 MAP_FILE | MAP_SHARED, fileno(f), offset);

    if (address == (void *)-1) {
        fprintf(stderr,"%s: mmap(): %s\n",argv[0],strerror(errno));
        exit(1);
    }
    fclose(f);
    for (i=0; i<len; i++) {
        c = getchar();
        if (c==-1)
            return 0;
        address[i]=c;
    }
    return 0;
}
        
