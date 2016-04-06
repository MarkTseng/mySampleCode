.. _sys-limits:

============================
Memory Management and Limits
============================

:mod:`sys` includes several functions for understanding and
controlling memory usage.

Reference Counts
================

Python uses *reference counting* and *garbage collection* for
automatic memory management.  An object is automatically marked to be
collected when its reference count drops to zero.  To examine the
reference count of an existing object, use :func:`getrefcount`.

.. include:: sys_getrefcount.py
    :literal:
    :start-after: #end_pymotw_header

The count is actually one higher than expected because there is a
temporary reference to the object held by :func:`getrefcount` itself.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'sys_getrefcount.py'))
.. }}}

::

	$ python sys_getrefcount.py

	At start         : 2
	Second reference : 3
	After del        : 2

.. {{{end}}}

.. seealso::

    :mod:`gc`
        Control the garbage collector via the functions exposed in :mod:`gc`.

Object Size
===========

Knowing how many references an object has may help find cycles or a
memory leak, but it isn't enough to determine what objects are
consuming the *most* memory.  That requires knowledge about how big
objects are.

.. include:: sys_getsizeof.py
    :literal:
    :start-after: #end_pymotw_header

:func:`getsizeof` reports the size in bytes.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'sys_getsizeof.py'))
.. }}}

::

	$ python sys_getsizeof.py

	      list : 72
	     tuple : 56
	      dict : 280
	       str : 38
	       str : 43
	       int : 24
	     float : 24
	  classobj : 104
	  instance : 72
	      type : 904
	  NewStyle : 64

.. {{{end}}}

The reported size for a custom class does not include the size of the
attribute values.

.. include:: sys_getsizeof_object.py
    :literal:
    :start-after: #end_pymotw_header

This can give a false impression of the amount of memory being
consumed.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'sys_getsizeof_object.py'))
.. }}}

::

	$ python sys_getsizeof_object.py

	WithoutAttributes: 64
	WithAttributes: 64

.. {{{end}}}


For a more complete estimate of the space used by a class, provide a
:func:`__sizeof__` method to compute the value by aggregating the
sizes of attributes of an object.

.. include:: sys_getsizeof_custom.py
    :literal:
    :start-after: #end_pymotw_header

This version adds the base size of the object to the sizes of all of
the attributes stored in the internal :data:`__dict__`.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'sys_getsizeof_custom.py'))
.. }}}

::

	$ python sys_getsizeof_custom.py

	140

.. {{{end}}}

Recursion
=========

Allowing infinite recursion in a Python application may introduce a
stack overflow in the interpreter itself, leading to a crash. To
eliminate this situation, the interpreter provides a way to control
the maximum recursion depth using :func:`setrecursionlimit` and
:func:`getrecursionlimit`.

.. include:: sys_recursionlimit.py
    :literal:
    :start-after: #end_pymotw_header

Once the recursion limit is reached, the interpreter raises a
:ref:`RuntimeError <exceptions-RuntimeError>` exception so the
program has an opportunity to handle the situation.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'sys_recursionlimit.py'))
.. }}}

::

	$ python sys_recursionlimit.py

	Initial limit: 1000
	Modified limit: 10
	generate_recursion_error(1)
	generate_recursion_error(2)
	generate_recursion_error(3)
	generate_recursion_error(4)
	generate_recursion_error(5)
	generate_recursion_error(6)
	generate_recursion_error(7)
	generate_recursion_error(8)
	Caught exception: maximum recursion depth exceeded while getting the str of an object

.. {{{end}}}


Maximum Values
==============

Along with the runtime configurable values, :mod:`sys` includes
variables defining the maximum values for types that vary from system
to system.

.. include:: sys_maximums.py
    :literal:
    :start-after: #end_pymotw_header

:const:`maxint` is the largest representable regular integer.
:const:`maxsize` is the maximum size of a list, dictionary, string, or
other data structure dictated by the C interpreter's size type.
:const:`maxunicode` is the largest integer Unicode point supported by
the interpreter as currently configured.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'sys_maximums.py'))
.. }}}

::

	$ python sys_maximums.py

	maxint    : 9223372036854775807
	maxsize   : 9223372036854775807
	maxunicode: 65535

.. {{{end}}}

Floating Point Values
=====================

The structure :data:`float_info` contains information about the
floating point type representation used by the interpreter, based on
the underlying system's float implementation.

.. include:: sys_float_info.py
    :literal:
    :start-after: #end_pymotw_header

.. note:: 

    These values depend on the compiler and underlying system.  These
    examples were produced on OS X 10.6.4.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'sys_float_info.py'))
.. }}}

::

	$ python sys_float_info.py

	Smallest difference (epsilon): 2.22044604925e-16
	
	Digits (dig)              : 15
	Mantissa digits (mant_dig): 53
	
	Maximum (max): 1.79769313486e+308
	Minimum (min): 2.22507385851e-308
	
	Radix of exponents (radix): 2
	
	Maximum exponent for radix (max_exp): 1024
	Minimum exponent for radix (min_exp): -1021
	
	Maximum exponent for power of 10 (max_10_exp): 308
	Minimum exponent for power of 10 (min_10_exp): -307
	
	Rounding for addition (rounds): 1

.. {{{end}}}

.. seealso::

    The ``float.h`` C header file for the local compiler contains more
    details about these settings.

Byte Ordering
=============

:const:`byteorder` is set to the native byte order.

.. include:: sys_byteorder.py
   :literal:
   :start-after: #end_pymotw_header

The value is either ``big`` for big-endian or ``little`` for
little-endian.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'sys_byteorder.py'))
.. }}}

::

	$ python sys_byteorder.py

	little

.. {{{end}}}

.. seealso::

    `Wikipedia: Endianness <http://en.wikipedia.org/wiki/Byte_order>`__
        Description of big and little endian memory systems.

    :mod:`array`, :mod:`struct`
        Other modules that depend on the byte order.
