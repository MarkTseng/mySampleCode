#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2009 Doug Hellmann All rights reserved.
#
"""
"""

__version__ = "$Id$"
#end_pymotw_header

import asyncore

if not hasattr(asyncore.file_wrapper, 'getsockopt'):
    # Python 2.7 and early versions of Python 3.x are missing the
    # getsockopt() method of asyncore.file_wrapper, so an
    # implementation needs to be added.  This version comes from the
    # patch on issue 9354 (http://bugs.python.org/issue9354) that
    # fixes the problem in Python 3.2.
    import socket
    def monkey_patch_getsockopt(self, level, optname, buflen=None):
        if (level == socket.SOL_SOCKET and
            optname == socket.SO_ERROR and
            not buflen):
            return 0
        raise NotImplmentedError('Only asyncore-specific behavior implemented')
    asyncore.file_wrapper.getsockopt = monkey_patch_getsockopt

class FileReader(asyncore.file_dispatcher):
    
    def writable(self):
        return False
    
    def handle_read(self):
        data = self.recv(64)
        print 'READ: (%d)\n%r' % (len(data), data)
        
    def handle_expt(self):
        # Ignore events that look like out of band data
        pass
    
    def handle_close(self):
        self.close()

reader = FileReader(open('lorem.txt', 'r'))
asyncore.loop()
