#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2010 Doug Hellmann.  All rights reserved.
#
"""Repetition of patterns
"""
#end_pymotw_header

from re_test_patterns import test_patterns

test_patterns(
    'This is some text -- with punctuation.',
    [ ('[a-z]+', 'sequences of lowercase letters'),
      ('[A-Z]+', 'sequences of uppercase letters'),
      ('[a-zA-Z]+', 'sequences of lowercase or uppercase letters'),
      ('[A-Z][a-z]+', 'one uppercase followed by lowercase'),
      ])
