#!/usr/bin/env python
#
# Copyright 2007 Doug Hellmann.
#
#
#                         All Rights Reserved
#
# Permission to use, copy, modify, and distribute this software and
# its documentation for any purpose and without fee is hereby
# granted, provided that the above copyright notice appear in all
# copies and that both that copyright notice and this permission
# notice appear in supporting documentation, and that the name of Doug
# Hellmann not be used in advertising or publicity pertaining to
# distribution of the software without specific, written prior
# permission.
#
# DOUG HELLMANN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
# INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN
# NO EVENT SHALL DOUG HELLMANN BE LIABLE FOR ANY SPECIAL, INDIRECT OR
# CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
# OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
# NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#

"""Writing data to a new archive with writestr().

"""

__version__ = "$Id$"
#end_pymotw_header

from zipfile_infolist import print_info
import zipfile

msg = 'This data did not exist in a file before being added to the ZIP file'
zf = zipfile.ZipFile('zipfile_writestr.zip', 
                     mode='w',
                     compression=zipfile.ZIP_DEFLATED, 
                     )
try:
    zf.writestr('from_string.txt', msg)
finally:
    zf.close()

print_info('zipfile_writestr.zip')

zf = zipfile.ZipFile('zipfile_writestr.zip', 'r')
print zf.read('from_string.txt')
