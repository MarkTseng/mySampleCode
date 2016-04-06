#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2009 Doug Hellmann All rights reserved.
#
"""
"""
#end_pymotw_header

import abc

class PluginBase(object):
    __metaclass__ = abc.ABCMeta
    
    @abc.abstractmethod
    def load(self, input):
        """Retrieve data from the input source
        and return an object.
        """
    
    @abc.abstractmethod
    def save(self, output, data):
        """Save the data object to the output."""
