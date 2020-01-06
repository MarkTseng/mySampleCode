#ifndef _LIBEBL_ARM_H
#define _LIBEBL_ARM_H 1

#include <libdw.h>

extern int arm_return_value_location_soft(Dwarf_Die *, const Dwarf_Op **locp);
extern int arm_return_value_location_hard(Dwarf_Die *, const Dwarf_Op **locp);

#endif
