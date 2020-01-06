#!/usr/bin/perl -w
# log-malloc2 / config module
#	Config module
#
# Author: Samuel Behan <_samuel_._behan_(at)_dob_._sk> (C) 2013-2015
#
# License: GNU GPLv3 (http://www.gnu.org/licenses/gpl.html)
#
# Web:
#	http://devel.dob.sk/log-malloc2
#	http://blog.dob.sk/category/devel/log-malloc2 (howto, tutorials)
#	https://github.com/samsk/log-malloc2 (git repo)
#
#
package log_malloc::config;

use strict;

my $prefix	= "/usr/local";
my $exec_prefix	= "${prefix}";
my $includedir	= "${prefix}/include";
my $libdir	= "${exec_prefix}/lib";
my $libexecdir	= "${exec_prefix}/libexec";

#
# shared funcs
#

# LD_PRELOAD
sub LD_PRELOAD()
{
	return "$libdir/liblog-malloc2.so";
}

1;
# EOF
