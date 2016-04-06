#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2008 Doug Hellmann All rights reserved.
#
"""Traverse a directory tree.
"""

__version__ = "$Id$"
#end_pymotw_header

import os
import os.path
import pprint

def visit(arg, dirname, names):
    print dirname, arg
    for name in names:
        subname = os.path.join(dirname, name)
        if os.path.isdir(subname):
            print '  %s/' % name
        else:
            print '  %s' % name
    print

os.mkdir('example')
os.mkdir('example/one')
f = open('example/one/file.txt', 'wt')
f.write('contents')
f.close()
f = open('example/two.txt', 'wt')
f.write('contents')
f.close()
os.path.walk('example', visit, '(User data)')