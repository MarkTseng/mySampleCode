#ifndef _LIBEBL_HPPA_H
#define _LIBEBL_HPPA_H 1

#include <libdw.h>

extern int parisc_return_value_location_32(Dwarf_Die *, const Dwarf_Op **locp);
extern int parisc_return_value_location_64(Dwarf_Die *, const Dwarf_Op **locp);

#endif
