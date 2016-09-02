#include <stdio.h>
/* -*- mode: C; c-file-style: "linux" -*- */

/* MemProf -- memory profiler and leak detector
 * Copyright 1999, 2000, 2001, Red Hat, Inc.
 * Copyright 2002, Kristian Rietveld
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
/*====*/

#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>		/* For send() */
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "memintercept-utils.h"

static int
write_all (int         fd,
	   const void *buf,
	   int         total)
{
	int count;
	int written = 0;

	while (written < total) {
		/* Use send() to avoid EPIPE errors */
		count = write (fd, buf + written, total - written);
		if (count < 0) {
			if (errno != EINTR)
				return 0;
		} else {
			if (count == 0)
				return 0;
			written += count;
		}
	}

	return 1;
}

static void
write_unsigned (int           fd,
		unsigned long num,
		unsigned int  radix)
{
	char buffer[64];
	unsigned long tmp;
	char c;
	int i, n;

	if (!num) {
		write_all (fd, "0", 1);
		return;
	} 

	if (radix == 16)
		write_all (fd, "0x", 2);
	else if (radix == 8)
		write_all (fd, "0", 2);
	
	n = 0;
	tmp = num;
	while (tmp) {
		tmp /= radix;
		n++;
	}

	i = n;
	while (num) {
		i--;
		c = (num % radix);
		if (c < 10)
			buffer[i] = c + '0';
		else
			buffer[i] = c + 'a' - 10;
		num /= radix;
	}

	write_all (fd, buffer, n);
}


static void
write_signed (int          fd,
	      long         num,
	      unsigned int radix)
{
	if (num < 0) {
		write_all (fd, "-", 1);
		num = -num;
	}

	write_unsigned (fd, num, radix);
}

static void
mi_vprintf (int         fd,
	    const char *format,
	    va_list     va)
{
	const char *p, *q;
	int argi;
	size_t argsize;
	const void *argp;
	const char *args = NULL;
	long argl = 0;
	
	p = q = format;
	while ((p = strchr (p, '%'))) {
		int is_size = 0;
			
		write_all (fd, q, p - q);
		q = p + 2;

	again:
		switch (*(p + 1)) {
		case 'z':
			is_size = 1;
			q++;
			p++;
			goto again;
		case 'd':
		case 'u':
		case 'x':
			if (is_size) {
				argsize = va_arg (va, size_t);
				argl = argsize;
			} else {
				argi = va_arg (va, int);
				argl = argi;
			}
			break;
		case 'p':
			argp = va_arg (va, void *);
			argl = (long)argp;
			break;
		case 's':
			args = va_arg (va, const char *);
			break;
		}

		switch (*(p + 1)) {
		case '%':
			write_all (fd, "%", 1);
			break;
		case 'z':
			is_size = 1;
			q++;
			p++;
			goto again;
			break;
		case 'P':
			write_signed (fd, getpid(), 10);
			break;
		case 'd':
			write_signed (fd, argl, 10);
			break;
		case 'u':
			write_unsigned (fd, argl, 10);
			break;
		case 'p':
		case 'x':
			write_unsigned (fd, argl, 16);
			break;
		case 's':
			if (!args)
				args = "(null)";
			write_all (fd, args, strlen (args));
			break;
		case 0:
			q--;
			break;
		}
		p = q;
	}

	write_all (fd, q, strlen (q));
}

void
mi_printf (int         fd,
	   const char *format,
	   ...)
{
	va_list va;
	
	va_start (va, format);
	mi_vprintf (fd, format, va);
	va_end (va);
}

void
mi_debug (const char *format,
	  ...)
{
	static int fd = -1;
	va_list va;

	if (fd < 0) {
#if 0
		fd = 2;
#else
		fd = open ("/dev/tty", O_WRONLY);
#endif		
	}
	

	mi_printf (fd, "memintercept (%P): ");

	va_start (va, format);
	mi_vprintf (fd, format, va);
	va_end (va);
}

void
mi_perror (const char *message)
{
	if (message && *message)
		mi_debug ("%s: %s (%d)\n", message, strerror (errno), errno);
	else
		mi_debug ("%s (%d)\n", strerror (errno), errno);
}

int
mi_write (int         fd,
	  const void *buf,
	  int         total)
{
	int count;
	int written = 0;

	while (written < total) {
		/* Use send() to avoid EPIPE errors */
		count = send (fd, buf + written, total - written, MSG_NOSIGNAL);
		if (count < 0) {
			if (errno != EINTR)
			{
				mi_debug ("mi_write error %d\n", errno);
				return 0;
			}
		} else {
			if (count == 0)
				return 0;
			written += count;
		}
	}

	return 1;
}

uint32_t
mi_atomic_increment(uint32_t *addr)
{
#if defined (__GNUC__) && defined (__i386__)
	uint32_t result;
	__asm__ __volatile__("lock; xaddl %0, %1; incl %0"
			     : "=r"(result), "=m"(*(addr))
			     : "0" (1), "m"(*(addr)) : "memory");
	return result;
#else
	/* Hope for the best */
	return *++addr;
#endif
}

uint32_t
mi_atomic_decrement(uint32_t *addr)
{
#if defined (__GNUC__) && defined (__i386__)
	uint32_t result;
	__asm__ __volatile__("lock; xaddl %0, %1; decl %0"
			     : "=r"(result), "=m"(*(addr))
			     : "0" (-1), "m"(*(addr)) : "memory");
	return result;
#else
	/* Hope for the best */
	return *--addr;
#endif
}

