/* bps - Breakpoint Shenanigans - K Sheldrake
** bps_cmdline.c - command line argument processing functions for bps
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

#include "bps_cmdline.h"

/*
** command line argument functions
*/

/* isnextval checks if there is a : in the string and changes it to a null */
int isnextval(char *buff)
{
	char *ptr = NULL;

	if (!buff) return 0;
	
	/* find the : and change it to a null */
	ptr = strchr(buff, ':');
	if (!ptr) return 0;

	*ptr = 0x00;

	return 1;
}


/* nextval returns a pointer to the next string (after the next null) */
char *nextval(char *buff)
{
	if (!buff) return NULL;
	
	return strchr(buff, 0x00) + 1;
}


/* processptrarg deals with -R register:format[:size][:offset] arguments */
int processptrarg(char *arg, struct ptrlist *ptr)
{
	char *argptr = arg;

	if (!arg || !ptr) return 0;
	
	if (isnextval(argptr)) {

		if (!isvalidreg(argptr)) {
			usage("invalid register");
		}
		ptr->reg = argptr;
		argptr = nextval(argptr);

		ptr->format = argptr[0];

		if (ptr->format <= 'Z') {
			ptr->deref = 1;
			ptr->format += 0x20;
		}

		if (ptr->format == 's') {
			/* no size, optional offset */

			if (isnextval(argptr)) {
				argptr = nextval(argptr);

				ptr->offset = strtoll(argptr, NULL, 0);

			}

		} else {
			/* get the size, optional offset */

			if (isnextval(argptr)) {
				argptr = nextval(argptr);

				if (isnextval(argptr)) {

					ptr->size = strtol(argptr, NULL, 0);
					argptr = nextval(argptr);
					ptr->offset = strtoll(argptr, NULL, 0);

				} else {

					ptr->size = strtol(argptr, NULL, 0);

				}

			} else {
				return 0;
			}
		}

	} else {
		return 0;
	}

	return 1;

}


/* processaddrarg deals with -A register:format[:size][:offset] arguments */
int processaddrarg(char *arg, struct addrlist *addr)
{
	char *argptr = arg;

	if (!arg || !addr) return 0;
	
	if (isnextval(argptr)) {

		addr->address = (unsigned long) strtoll(argptr, NULL, 0);
		argptr = nextval(argptr);

		addr->format = argptr[0];

		if (addr->format <= 'Z') {
			addr->deref = 1;
			addr->format += 0x20;
		}

		if (addr->format == 's') {
			/* no size, optional offset */

			if (isnextval(argptr)) {
				argptr = nextval(argptr);

				addr->offset = strtoll(argptr, NULL, 0);

			}

		} else {
			/* get the size, optional offset */

			if (isnextval(argptr)) {
				argptr = nextval(argptr);

				if (isnextval(argptr)) {

					addr->size = strtol(argptr, NULL, 0);
					argptr = nextval(argptr);
					addr->offset = strtoll(argptr, NULL, 0);

				} else {

					addr->size = strtol(argptr, NULL, 0);

				}

			} else {
				return 0;
			}
		}

	} else {
		return 0;
	}

	return 1;

}


