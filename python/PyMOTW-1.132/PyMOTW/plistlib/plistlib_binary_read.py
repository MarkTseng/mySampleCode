#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2009 Doug Hellmann.  All rights reserved.
#
"""
"""
#end_pymotw_header

import plistlib
import pprint

DATA = """<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
        <key>binary_data</key>
        <data>
        VGhpcyBkYXRhIGhhcyBhbiBlbWJlZGRlZCBudWxsLiAA
        </data>
</dict>
</plist>
"""

d = plistlib.readPlistFromString(DATA)

print repr(d['binary_data'].data)
