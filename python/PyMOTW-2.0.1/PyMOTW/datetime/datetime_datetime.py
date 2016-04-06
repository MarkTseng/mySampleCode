#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2008 Doug Hellmann All rights reserved.
#
"""Combining dates and times into a single object.
"""

__version__ = "$Id$"
#end_pymotw_header

import datetime

print 'Now    :', datetime.datetime.now()
print 'Today  :', datetime.datetime.today()
print 'UTC Now:', datetime.datetime.utcnow()
print

FIELDS = [ 'year', 'month', 'day',
           'hour', 'minute', 'second', 'microsecond',
           ]

d = datetime.datetime.now()
for attr in FIELDS:
    print '%15s: %s' % (attr, getattr(d, attr))
