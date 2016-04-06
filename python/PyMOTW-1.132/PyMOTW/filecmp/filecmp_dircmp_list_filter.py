#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2008 Doug Hellmann All rights reserved.
#
"""
"""

__version__ = "$Id$"
#end_pymotw_header

import filecmp

dc = filecmp.dircmp('example/dir1', 'example/dir2', ignore=['common_file'])
print 'Left :', dc.left_list
print 'Right:', dc.right_list