#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2010 Doug Hellmann.  All rights reserved.
#
"""Match email addresses
"""
#end_pymotw_header

import re

address = re.compile('[\w\d.+-]+@([\w\d.]+\.)+(com|org|edu)',
                     re.UNICODE)

candidates = [
    u'first.last@example.com',
    u'first.last+category@gmail.com',
    u'valid-address@mail.example.com',
    u'not-valid@example.foo',
    ]

for candidate in candidates:
    match = address.search(candidate)
    print '%-30s  %s' % (candidate, 'Matches' if match else 'No match')
    
