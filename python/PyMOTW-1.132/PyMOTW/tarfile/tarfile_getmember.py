#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2009 Doug Hellmann All rights reserved.
#
"""
"""

__version__ = "$Id$"
#end_pymotw_header

import tarfile
import time

t = tarfile.open('example.tar', 'r')
for filename in [ 'README.txt', 'notthere.txt' ]:
    try:
        info = t.getmember(filename)
    except KeyError:
        print 'ERROR: Did not find %s in tar archive' % filename
    else:
        print '%s is %d bytes' % (info.name, info.size)
