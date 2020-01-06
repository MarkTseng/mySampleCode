# mpatrol
# A library for controlling and tracing dynamic memory allocations.
# Copyright (C) 1997-2008 Graeme S. Roy <graemeroy@users.sourceforge.net>
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or (at
# your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
# General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


# GDB command file for use with mpatrol


# $Id$


# Set mpatrol library options in the current process environment.
# GDB doesn't support commands with variable arguments so we have
# to make use of the shell in order to set the new environment.
# Note that there is a known problem when using option arguments
# that contain spaces, so try to avoid using such arguments.

define mallopt
printf "Enter mpatrol library options: "
shell read arg; echo set environment MPATROL_OPTIONS `mpatrol --show-env $arg` >/tmp/mpatrol.gdb
source /tmp/mpatrol.gdb
shell rm -f /tmp/mpatrol.gdb
show environment MPATROL_OPTIONS
end
document mallopt
Sets mpatrol library options in the current process environment.
end


# Display information about an address in the heap.

define printalloc
call __mp_printinfo($arg0)
end
document printalloc
Displays information about an address in the heap.
end


# Break at a specific allocation index.

define allocstop
call __mp_setoption(-3, $arg0)
end
document allocstop
Breaks at a specific allocation index.
end


# Break at a specific reallocation index.

define reallocstop
call __mp_setoption(-4, $arg0)
end
document reallocstop
Breaks at a specific reallocation index.
end


# Break when a specific allocation is freed.

define freestop
call __mp_setoption(-5, $arg0)
end
document freestop
Breaks when a specific allocation is freed.
end
