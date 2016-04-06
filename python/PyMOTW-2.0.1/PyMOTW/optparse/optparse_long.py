#!/usr/bin/env python
"""Using optparse with longer option names.
"""
#end_pymotw_header

import optparse

parser = optparse.OptionParser()
parser.add_option('--noarg', action="store_true", default=False)
parser.add_option('--witharg', action="store", dest="witharg")
parser.add_option('--witharg2', action="store",
                  dest="witharg2", type="int")

print parser.parse_args([ '--noarg',
                          '--witharg', 'val',
                          '--witharg2=3' ])
