#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2010 Doug Hellmann.  All rights reserved.
#

class MyObj(object):

    def __init__(self, num_loops):
        self.count = num_loops

    def go(self):
        for i in range(self.num_loops):
            print i
        return
