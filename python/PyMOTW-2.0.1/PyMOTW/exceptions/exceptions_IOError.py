#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2008 Doug Hellmann All rights reserved.
#
"""
"""

__version__ = "$Id$"
#end_pymotw_header

try:
    f = open('/does/not/exist', 'r')
except IOError as err:
    print 'Formatted   :', str(err)
    print 'Filename    :', err.filename
    print 'Errno       :', err.errno
    print 'String error:', err.strerror
