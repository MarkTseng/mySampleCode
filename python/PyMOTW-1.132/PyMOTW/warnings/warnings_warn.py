#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2008 Doug Hellmann All rights reserved.
#
"""
"""
#end_pymotw_header

import warnings

print 'Before the warning'
warnings.warn('This is a warning message')
print 'After the warning'
