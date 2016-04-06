#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2008 Doug Hellmann All rights reserved.
#
"""Demonstrate WeakValueDictionary.
"""
#end_pymotw_header

import gc
from pprint import pprint
import weakref

gc.set_debug(gc.DEBUG_LEAK)

class ExpensiveObject(object):
    def __init__(self, name):
        self.name = name
    def __repr__(self):
        return 'ExpensiveObject(%s)' % self.name
    def __del__(self):
        print '    (Deleting %s)' % self
        
def demo(cache_factory):
    # hold objects so any weak references 
    # are not removed immediately
    all_refs = {}
    # create the cache using the factory
    print 'CACHE TYPE:', cache_factory
    cache = cache_factory()
    for name in [ 'one', 'two', 'three' ]:
        o = ExpensiveObject(name)
        cache[name] = o
        all_refs[name] = o
        del o # decref

    print '  all_refs =',
    pprint(all_refs)
    print '\n  Before, cache contains:', cache.keys()
    for name, value in cache.items():
        print '    %s = %s' % (name, value)
        del value # decref
        
    # Remove all references to the objects except the cache
    print '\n  Cleanup:'
    del all_refs
    gc.collect()

    print '\n  After, cache contains:', cache.keys()
    for name, value in cache.items():
        print '    %s = %s' % (name, value)
    print '  demo returning'
    return

demo(dict)
print

demo(weakref.WeakValueDictionary)
