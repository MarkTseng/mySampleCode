#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2008 Doug Hellmann All rights reserved.
#
"""
"""

__version__ = "$Id$"
#end_pymotw_header

import tempfile

with tempfile.NamedTemporaryFile(
    suffix='_suffix', prefix='prefix_', dir='/tmp',
    ) as temp:
    print 'temp:'
    print '  ', temp
    print 'temp.name:'
    print '  ', temp.name
