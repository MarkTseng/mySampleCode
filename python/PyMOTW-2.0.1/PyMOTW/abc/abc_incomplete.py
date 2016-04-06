#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2009 Doug Hellmann All rights reserved.
#
"""
"""
#end_pymotw_header

import abc
from abc_base import PluginBase

class IncompleteImplementation(PluginBase):
    
    def save(self, output, data):
        return output.write(data)

PluginBase.register(IncompleteImplementation)

if __name__ == '__main__':
    print 'Subclass:', issubclass(IncompleteImplementation,
                                  PluginBase)
    print 'Instance:', isinstance(IncompleteImplementation(),
                                  PluginBase)
