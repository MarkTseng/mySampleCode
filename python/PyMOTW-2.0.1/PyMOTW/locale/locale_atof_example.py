#!/usr/bin/env python
#
# Copyright 2007 Doug Hellmann.
#
"""Converting strings to floating point numbers.
"""
#end_pymotw_header

import locale

sample_data = [ ('USA',      'en_US', '1,234.56'),
                ('France',   'fr_FR', '1234,56'),
                ('Spain',    'es_ES', '1234,56'),
                ('Portugal', 'pt_PT', '1234.56'),
                ('Poland',   'pl_PL', '1 234,56'),
                ]

for name, loc, a in sample_data:
    locale.setlocale(locale.LC_ALL, loc)
    f = locale.atof(a)
    print '%20s: %9s => %f' % (name, a, f)
    
