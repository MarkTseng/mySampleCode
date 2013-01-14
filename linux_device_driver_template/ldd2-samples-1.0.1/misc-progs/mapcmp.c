/*
 * Simple program to compare two mmap'd areas.
 *
 * Copyright (C) 2001 Alessandro Rubini and Jonathan Corbet
 * Copyright (C) 2001 O'Reilly & Associates
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.  The citation
 * should list that the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates.   No warranty is attached;
 * we cannot take responsibility for errors or fitness for use.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/errno.h>
#include <fcntl.h>

static char *mapdev (const char *, int, int);

/*
 * memcmp dev1 dev2 offset pages
 */
int main (int argc, char **argv)
{
	unsigned int offset, size, i;
	char *addr1, *addr2;
/*
 * Sanity check.
 */
	if (argc != 5)
	{
		fprintf (stderr, "Usage: mapcmp dev1 dev2 offset pages\n");
		exit (1);
	}
/*
 * Map the two devices.
 */
	offset = atoi (argv[3]);
	size = atoi (argv[4]);
	addr1 = mapdev (argv[1], offset, size);
	addr2 = mapdev (argv[2], offset, size);
/*
 * Do the comparison.
 */
	printf ("Comparing...");
	fflush (stdout);
	for (i = 0; i < size; i++)
		if (*addr1++ != *addr2++)
		{
			printf ("areas differ at byte %d\n", i);
			exit (0);
		}
	printf ("areas are identical.\n");
	exit (0);
}



static char *mapdev (const char *dev, int offset, int size)
{
	char *addr;
	int fd = open (dev, O_RDONLY);

	if (fd < 0)
	{
		perror (dev);
		exit (1);
	}
	addr = mmap (0, size, PROT_READ, MAP_PRIVATE, fd, offset);
	if (addr == MAP_FAILED)
	{
		perror (dev);
		exit (1);
	}
	printf ("Mapped %s (%d @ %d) at 0x%p\n", dev, size, offset, addr);
	return (addr);
}
