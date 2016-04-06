#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2010 Doug Hellmann.  All rights reserved.
#
"""AttributeError from class that uses __slots__
"""
#end_pymotw_header

class MyClass(object):
    __slots__ = ( 'attribute', )

o = MyClass()
o.attribute = 'known attribute'
o.not_a_slot = 'new attribute'

