#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2010 Doug Hellmann.  All rights reserved.
#
"""Multiline input
"""
#end_pymotw_header

import re

text = 'This is some text -- with punctuation.\nAnd a second line.'
pattern = r'(^\w+)|(\w+\S*$)'
single_line = re.compile(pattern)
multiline = re.compile(pattern, re.MULTILINE)

print 'Text        :', repr(text)
print 'Pattern     :', pattern
print 'Single Line :', single_line.findall(text)
print 'Multline    :', multiline.findall(text)

