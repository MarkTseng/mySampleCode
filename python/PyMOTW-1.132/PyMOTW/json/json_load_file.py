#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2009 Doug Hellmann All rights reserved.
#
"""
"""
#end_pymotw_header

import json
import tempfile

f = tempfile.NamedTemporaryFile(mode='w+')
f.write('[{"a": "A", "c": 3.0, "b": [2, 4]}]')
f.flush()
f.seek(0)

print json.load(f)
