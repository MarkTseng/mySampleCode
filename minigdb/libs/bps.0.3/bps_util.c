/* bps - Breakpoint Shenanigans - K Sheldrake
** bps_util.c - utility functions for bps
**
** Copyright (C) 2015  Kevin Sheldrake
**
** This file is part of bps.
**
** Bps is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**
*/

#include "bps_util.h"

/* sad but true; verbosity is global */
int verbose = 0;

/* output is the output stream for messages */
int output = 1;


/*
** generic support functions
*/

/* printablechar converts a char to a . if it isn't printable */
unsigned char printablechar(unsigned char c)
{
	if (c < 0x20) return '.';
	if (c >= 0x7f) return '.';
	return c;
}


/* complex hexdump */
/* format is b[yte], h[alfword], w[ord]. data_len is number of values not number of bytes */
void hexdump(unsigned char *data, unsigned char format, int data_len)
{
	int i;
	int linelen = (4 * HEX_PER_ROW) + 32;
	char line[linelen];
	char *ptr = line;
	char *ptr2 = NULL;
	int offset = 0; 		/* offset is in terms of number of values, not number of bytes */
	int address = 0; 		/* address is a byte offset from data */
	uint16_t *datahalf = (uint16_t *)data; 
	uint32_t *dataword = (uint32_t *)data; 

	if (!data || (data_len <= 0)) {
		myprintf(0, "hexdump: invalid parameters\n");
		return;
	}
	
	myprintf(2, "\t\thexdump from (own memory) %p:\n", data);

	while (offset < data_len) {
		/* empty the output line */
		memset(line, 0x20, linelen - 1);
		line[linelen - 1] = 0x00;

		/* write the address */
		sprintf(line, "\t\t%08x  ", address);
		ptr = strchr(line, 0x00);

		switch(format) {
			case 'w':
				for (i=0; ((i < HEX_PER_ROW / 4) && (offset + i < data_len)); i++) {
					if (i == (HEX_PER_ROW / 8)) {
						sprintf(ptr, " ");
						ptr = strchr(ptr, 0x00);
					}
					sprintf(ptr, "%08x ", dataword[offset + i]);
					ptr = strchr(ptr, 0x00);
				}
				offset += (HEX_PER_ROW / 4);
				break;
			case 'h':
				for (i=0; ((i < HEX_PER_ROW / 2) && (offset + i < data_len)); i++) {
					if (i == (HEX_PER_ROW / 4)) {
						sprintf(ptr, " ");
						ptr = strchr(ptr, 0x00);
					}
					sprintf(ptr, "%04x ", datahalf[offset + i]);
					ptr = strchr(ptr, 0x00);
				}
				offset += (HEX_PER_ROW / 2);
				break;
			case 'b':
				ptr2 = ptr + (3 * HEX_PER_ROW) + 2;
				*ptr2 = '|';
				ptr2++;
				for (i=0; ((i < HEX_PER_ROW) && (offset + i < data_len)); i++) {
					if (i == (HEX_PER_ROW / 2)) {
						sprintf(ptr, " ");
						ptr = strchr(ptr, 0x00);
					}
					sprintf(ptr, "%02x ", data[offset + i]);
					ptr = strchr(ptr, 0x00);
					*ptr2 = printablechar(data[offset + i]);
					ptr2++;
				}
				*ptr = ' ';
				*ptr2 = '|';
				ptr2++;
				*ptr2 = 0x00;
				offset += HEX_PER_ROW;
				break;
			default:
				myprintf(0, "hexdump unknown format\n");
				exit(E_UNKNOWN);
		}



		myprintf(0, "%s\n", line);

		address += HEX_PER_ROW;
	}
	if (verbose > 1) {
		switch(format) {
			case 'b':
				shexdump(data, data_len);
				break;
			case 'h':
				shexdump(data, data_len * 2);
				break;
			case 'w':
				shexdump(data, data_len * 4);
				break;
		}
	}
}


/* simple hexdump for testing purposes */
void shexdump(unsigned char *data, int data_len)
{
	int i;

	if (!data || (data_len <= 0)) {
		myprintf(0, "shexdump: invalid parameters\n");
		return;
	}
	
	myprintf(1, "\t\tHexdump from %p:\n", data);

	for (i=0; i<data_len; i++) {
		if ((i % HEX_PER_ROW) == 0) {
			myprintf(0, "\n\t\t0x%04x: ", i);
		}
		myprintf(0, "%02x ", data[i]);
	}
	myprintf(0, "\n\n");
}


/* myprintf does a printf to the output file descriptor */
int myprintf(unsigned int vlevel, const char *fmt, ... )
{
    char buffer[S_MYPRINTF];
    int n;
    va_list ap;

    memset(buffer, ' ', 17);

    if (vlevel <= verbose) {
        va_start(ap, fmt);
        if (vlevel <= 16) {
            n = vsnprintf(buffer + vlevel, S_MYPRINTF, fmt, ap) + vlevel;
        } else {
            n = vsnprintf(buffer + 16, S_MYPRINTF, fmt, ap) + 16;
        }
        
        va_end(ap);
        
        if (write(output, buffer, n) <= 0) { 
            printf("error writing to output; reverting to stdout\n");
            output = 1;
        }
        return n;
    } else {
        return 0;
    }
}


/* openpipe */
int openpipe(char *pipename)
{
	int fd = 0;

	if ((!pipename) || (pipename[0] == 0x00)) {
		usage("empty pipe name");
	}

	fd = open(pipename, O_RDWR, 0);

	if (fd < 0) {
		usage("cannot open pipe");
	}

	return fd;
}



