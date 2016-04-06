#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2008 Doug Hellmann All rights reserved.
#
"""
"""

__version__ = "$Id$"
#end_pymotw_header

import os

for i in range(10):
    try:
        print i, os.ttyname(i)
    except OSError as err:
        print
        print '  Formatted   :', str(err)
        print '  Errno       :', err.errno
        print '  String error:', err.strerror
        break
