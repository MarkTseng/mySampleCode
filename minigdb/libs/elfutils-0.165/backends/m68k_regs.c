/* Register names and numbers for m68k DWARF.
   Copyright (C) 2007 Kurt Roeckx <kurt@roeckx.be>

   This software is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by the
   Free Software Foundation; version 2 of the License.

   This software is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this software; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301 USA.

   */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <string.h>
#include <dwarf.h>

#define BACKEND m68k_
#include "libebl_CPU.h"

ssize_t
m68k_register_info (Ebl *ebl __attribute__ ((unused)),
		   int regno, char *name, size_t namelen,
		   const char **prefix, const char **setname,
		   int *bits, int *type)
{
	if (name == NULL)
		return 25;

	if (regno < 0 || regno > 24 || namelen < 5)
		return -1;

	*prefix = "%";
	*bits = 32;
	*type = (regno < 8 ? DW_ATE_signed
		: regno < 16 ? DW_ATE_address : DW_ATE_float);

	if (regno < 8)
	{
		*setname = "integer";
	}
	else if (regno < 16)
	{
		*setname = "address";
	}
	else if (regno < 24)
	{
		*setname = "FPU";
	}
	else
	{
		*setname = "address";
		*type = DW_ATE_address;
	}

	switch (regno)
	{
	case 0 ... 7:
		name[0] = 'd';
		name[1] = regno + '0';
		namelen = 2;
		break;

	case 8 ... 13:
		name[0] = 'a';
		name[1] = regno - 8 + '0';
		namelen = 2;
		break;

	case 14:
		name[0] = 'f';
		name[1] = 'p';
   		namelen = 2;
		break;

	case 15:
		name[0] = 's';
		name[1] = 'p';
   		namelen = 2;
		break;

	case 16 ... 23:
		name[0] = 'f';
		name[1] = 'p';
		name[2] = regno - 16 + '0';
   		namelen = 3;
		break;

	case 24:
		name[0] = 'p';
		name[1] = 'c';
		namelen = 2;
	}

	name[namelen++] = '\0';
	return namelen;
}

