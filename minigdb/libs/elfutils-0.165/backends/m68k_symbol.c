/* m68k specific symbolic name handling.
   Copyright (C) 2007 Kurt Roeckx <kurt@roeckx.be>

   This software is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by the
   Free Software Foundation; version 2 of the License.

   This software distributed in the hope that it will be useful, but
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

#include <elf.h>
#include <stddef.h>

#define BACKEND		m68k_
#include "libebl_CPU.h"

/* Check for the simple reloc types.  */
Elf_Type
m68k_reloc_simple_type (Ebl *ebl __attribute__ ((unused)), int type)
{
  switch (type)
    {
    case R_68K_32:
      return ELF_T_SWORD;
    case R_68K_16:
      return ELF_T_HALF;
    case R_68K_8:
      return ELF_T_BYTE;
    default:
      return ELF_T_NUM;
    }
}
