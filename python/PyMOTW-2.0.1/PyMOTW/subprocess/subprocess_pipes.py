#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2009 Doug Hellmann All rights reserved.
#
"""
"""
#end_pymotw_header

import subprocess

cat = subprocess.Popen(['cat', 'index.rst'], 
                        stdout=subprocess.PIPE,
                        )

grep = subprocess.Popen(['grep', '.. include::'],
                        stdin=cat.stdout,
                        stdout=subprocess.PIPE,
                        )

cut = subprocess.Popen(['cut', '-f', '3', '-d:'],
                        stdin=grep.stdout,
                        stdout=subprocess.PIPE,
                        )

end_of_pipe = cut.stdout

print 'Included files:'
for line in end_of_pipe:
    print '\t', line.strip()
