# mpatrol
# A library for controlling and tracing dynamic memory allocations.
# Copyright (C) 1997-2002 Graeme S. Roy <graeme@epc.co.uk>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public
# License along with this library; if not, write to the Free
# Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307, USA.


# csh/tcsh shell script for use with mpatrol


# $Id: .cshrc,v 1.2 2002/01/08 20:30:57 graeme Exp $


# Set mpatrol library options in the current shell environment.
# Note that there is a known problem when using option arguments
# that contain spaces, so try to avoid using such arguments.

alias mallopt 'setenv MPATROL_OPTIONS "`mpatrol --show-env \!*`"; echo "$MPATROL_OPTIONS"'
