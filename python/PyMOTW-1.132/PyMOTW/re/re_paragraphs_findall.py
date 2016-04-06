#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2010 Doug Hellmann.  All rights reserved.
#
"""Splitting input based on a pattern.
"""
#end_pymotw_header

import re

text = 'Paragraph one\non two lines.\n\nParagraph two.\n\n\nParagraph three.'

for num, para in enumerate(re.findall(r'(.+?)\n{2,}', text, flags=re.DOTALL)):
    print num, repr(para)
    print
