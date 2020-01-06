/* Function return value location for Linux/m68k ABI.
   Copyright (C) 2005-2010 Red Hat, Inc.
   Copyright (c) 2011 Thorsten Glaser, Debian.
   This file is part of Red Hat elfutils.

   Red Hat elfutils is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by the
   Free Software Foundation; version 2 of the License.

   Red Hat elfutils is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with Red Hat elfutils; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301 USA.

   Red Hat elfutils is an included package of the Open Invention Network.
   An included package of the Open Invention Network is a package for which
   Open Invention Network licensees cross-license their patents.  No patent
   license is granted, either expressly or impliedly, by designation as an
   included package.  Should you wish to participate in the Open Invention
   Network licensing program, please visit www.openinventionnetwork.com
   <http://www.openinventionnetwork.com>.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <assert.h>
#include <dwarf.h>

#define BACKEND m68k_
#include "libebl_CPU.h"


/* %d0, or pair %d0, %d1, or %a0 */
static const Dwarf_Op loc_intreg[] =
  {
    { .atom = DW_OP_reg0 }, { .atom = DW_OP_piece, .number = 4 },
    { .atom = DW_OP_reg1 }, { .atom = DW_OP_piece, .number = 4 },
  };
static const Dwarf_Op loc_ptrreg[] =
  {
    { .atom = DW_OP_reg8 },
  };
#define nloc_intreg	1
#define nloc_intregpair	4
#define nloc_ptrreg	1

/* %f0 */
static const Dwarf_Op loc_fpreg[] =
  {
    { .atom = DW_OP_reg16 }
  };
#define nloc_fpreg	1

/* Structures are a bit more complicated - small structures are returned
   in %d0 / %d1 (-freg-struct-return which is enabled by default), large
   structures use %a1 (in constrast to the SYSV psABI which says %a0) as
   reentrant storage space indicator.  */
static const Dwarf_Op loc_aggregate[] =
  {
    { .atom = DW_OP_breg9, .number = 0 }
  };
#define nloc_aggregate 1

int
m68k_return_value_location (Dwarf_Die *functypedie, const Dwarf_Op **locp)
{
  Dwarf_Word size;

  /* Start with the function's type, and get the DW_AT_type attribute,
     which is the type of the return value.  */

  Dwarf_Attribute attr_mem;
  Dwarf_Attribute *attr = dwarf_attr_integrate (functypedie, DW_AT_type,
						&attr_mem);
  if (attr == NULL)
    /* The function has no return value, like a `void' function in C.  */
    return 0;

  Dwarf_Die die_mem;
  Dwarf_Die *typedie = dwarf_formref_die (attr, &die_mem);
  int tag = dwarf_tag (typedie);

  /* Follow typedefs and qualifiers to get to the actual type.  */
  while (tag == DW_TAG_typedef
	 || tag == DW_TAG_const_type || tag == DW_TAG_volatile_type
	 || tag == DW_TAG_restrict_type)
    {
      attr = dwarf_attr_integrate (typedie, DW_AT_type, &attr_mem);
      typedie = dwarf_formref_die (attr, &die_mem);
      tag = dwarf_tag (typedie);
    }

  switch (tag)
    {
    case -1:
      return -1;

    case DW_TAG_subrange_type:
      if (! dwarf_hasattr_integrate (typedie, DW_AT_byte_size))
	{
	  attr = dwarf_attr_integrate (typedie, DW_AT_type, &attr_mem);
	  typedie = dwarf_formref_die (attr, &die_mem);
	  tag = dwarf_tag (typedie);
	}
      /* Fall through.  */

    case DW_TAG_base_type:
    case DW_TAG_enumeration_type:
    case DW_TAG_pointer_type:
    case DW_TAG_ptr_to_member_type:
      if (dwarf_formudata (dwarf_attr_integrate (typedie, DW_AT_byte_size,
						 &attr_mem), &size) != 0)
	{
	  if (tag == DW_TAG_pointer_type || tag == DW_TAG_ptr_to_member_type)
	    size = 4;
	  else
	    return -1;
	}
      if (tag == DW_TAG_pointer_type || tag == DW_TAG_ptr_to_member_type)
	{
	  *locp = loc_ptrreg;
          return nloc_ptrreg;
	}
      if (tag == DW_TAG_base_type)
	{
	  Dwarf_Word encoding;
	  if (dwarf_formudata (dwarf_attr_integrate (typedie, DW_AT_encoding,
						     &attr_mem),
			       &encoding) != 0)
	    return -1;
	  if (encoding == DW_ATE_float)
	    {
	      /* XXX really 10? */
              if (size > 10)
                return -2;
              *locp = loc_fpreg;
              return nloc_fpreg;
	    }
	}
      if (size <= 8)
	{
	intreg:
	  /* XXX check endianness of dword pair, int64 vs aggregate */
	  *locp = loc_intreg;
	  return size <= 4 ? nloc_intreg : nloc_intregpair;
	}

    aggregate:
      *locp = loc_aggregate;
      return nloc_aggregate;

    case DW_TAG_structure_type:
    case DW_TAG_class_type:
    case DW_TAG_union_type:
    case DW_TAG_array_type:
      if (dwarf_aggregate_size (typedie, &size) == 0
	  && size > 0 && size <= 8)
	/* not accurate for a struct whose only member is a float */
	goto intreg;
      goto aggregate;
    }

  /* XXX We don't have a good way to return specific errors from ebl calls.
     This value means we do not understand the type, but it is well-formed
     DWARF and might be valid.  */
  return -2;
}
