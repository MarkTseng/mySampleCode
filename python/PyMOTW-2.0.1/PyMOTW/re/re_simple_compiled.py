#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2010 Doug Hellmann.  All rights reserved.
#
"""Simple pattern examples.
"""
#end_pymotw_header

import re

# Precompile the patterns
regexes = [ re.compile(p)
            for p in [ 'this', 'that' ]
            ]
text = 'Does this text match the pattern?'

print 'Text: %r\n' % text

for regex in regexes:
    print 'Seeking "%s" ->' % regex.pattern,

    if regex.search(text):
        print 'match!'
    else:
        print 'no match'
