/* Initialization of m68k specific backend library.
   Copyright (C) 2007 Kurt Roeckx <kurt@roeckx.be>

   This software is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by the
   Free Software Foundation; version 2 of the License.

   This softare is distributed in the hope that it will be useful, but
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

#define BACKEND		m68k_
#define RELOC_PREFIX	R_68K_
#include "libebl_CPU.h"

/* This defines the common reloc hooks based on m68k_reloc.def.  */
#include "common-reloc.c"


const char *
m68k_init (Elf *elf __attribute__ ((unused)),
     GElf_Half machine __attribute__ ((unused)),
     Ebl *eh,
     size_t ehlen)
{
  /* Check whether the Elf_BH object has a sufficent size.  */
  if (ehlen < sizeof (Ebl))
    return NULL;

  /* We handle it.  */
  eh->name = "m68k";
  m68k_init_reloc (eh);
  HOOK (eh, reloc_simple_type);
  HOOK (eh, return_value_location);
  HOOK (eh, register_info);

  return MODVERSION;
}
