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
import pprint

dc = filecmp.dircmp('example/dir1', 'example/dir2')
print 'Common:'
pprint.pprint(dc.common)

print '\nLeft:'
pprint.pprint(dc.left_only)

print '\nRight:'
pprint.pprint(dc.right_only)

