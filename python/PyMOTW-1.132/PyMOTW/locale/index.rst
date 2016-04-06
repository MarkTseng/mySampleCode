=========================================
locale -- POSIX cultural localization API
=========================================

.. module:: locale
    :synopsis: POSIX cultural localization API

:Purpose: Format and parse values that depend on location or language.
:Python Version: 1.5 and later

The :mod:`locale` module is part of Python's internationalization and
localization support library. It provides a standard way to handle
operations that may depend on the language or location of a user. For
example, it handles formatting numbers as currency, comparing strings
for sorting, and working with dates. It does not cover translation
(see the :mod:`gettext` module) or Unicode encoding.

.. note::

  Changing the locale can have application-wide ramifications, so the
  recommended practice is to avoid changing the value in a library and
  to let the application set it one time. In the examples below, the
  locale is changed several times within a short program to highlight
  the differences in the settings of various locales. It is far more
  likely that your application will set the locale once at startup and
  not change it.

Probing the Current Locale
==========================

The most common way to let the user change the locale settings for an
application is through an environment variable (:data:`LC_ALL`,
:data:`LC_CTYPE`, :data:`LANG`, or :data:`LANGUAGE`, depending on the
platform). The application then calls :func:`setlocale` without a
hard-coded value, and the environment value is used.

.. include:: locale_env_example.py
    :literal:
    :start-after: #end_pymotw_header

The :func:`localeconv` method returns a dictionary containing the
locale's conventions.  The full list of value names and definitions is
covered in the standard library documentation.

A Mac running OS X 10.6 with all of the variables unset produces this output:

.. {{{cog
.. cog.out(run_script(cog.inFile, 'export LANG=; export LC_CTYPE=; python locale_env_example.py', interpreter=None))
.. }}}

::

	$ export LANG=; export LC_CTYPE=; python locale_env_example.py

	Environment settings:
		LC_ALL = 
		LC_CTYPE = 
		LANG = 
		LANGUAGE = 
	
	Locale from environment: (None, None)
	
	Numeric formatting:
	
	  Decimal point      : "."
	  Grouping positions : [3, 3, 0]
	  Thousands separator: ","
	
	Monetary formatting:
	
	  International currency symbol             : "'USD '"
	  Local currency symbol                     : '$' ($)
	  Symbol precedes positive value            : 1
	  Symbol precedes negative value            : 1
	  Decimal point                             : "."
	  Digits in fractional values               : 2
	  Digits in fractional values, international: 2
	  Grouping positions                        : [3, 3, 0]
	  Thousands separator                       : ","
	  Positive sign                             : ""
	  Positive sign position                    : Before value and symbol
	  Negative sign                             : "-"
	  Negative sign position                    : Before value and symbol
	
	

.. {{{end}}}


Running the same script with the :data:`LANG` variable set shows how
the locale and default encoding change:

France (``fr_FR``):

.. {{{cog
.. cog.out(run_script(cog.inFile, 'LANG=fr_FR LC_CTYPE=fr_FR LC_ALL=fr_FR python locale_env_example.py', interpreter=None))
.. }}}

::

	$ LANG=fr_FR LC_CTYPE=fr_FR LC_ALL=fr_FR python locale_env_example.py

	Environment settings:
		LC_ALL = fr_FR
		LC_CTYPE = fr_FR
		LANG = fr_FR
		LANGUAGE = 
	
	Locale from environment: ('fr_FR', 'ISO8859-1')
	
	Numeric formatting:
	
	  Decimal point      : ","
	  Grouping positions : [127]
	  Thousands separator: ""
	
	Monetary formatting:
	
	  International currency symbol             : "'EUR '"
	  Local currency symbol                     : 'Eu' (Eu)
	  Symbol precedes positive value            : 0
	  Symbol precedes negative value            : 0
	  Decimal point                             : ","
	  Digits in fractional values               : 2
	  Digits in fractional values, international: 2
	  Grouping positions                        : [3, 3, 0]
	  Thousands separator                       : " "
	  Positive sign                             : ""
	  Positive sign position                    : Before value and symbol
	  Negative sign                             : "-"
	  Negative sign position                    : After value and symbol
	
	

.. {{{end}}}


Spain (``es_ES``):


.. {{{cog
.. cog.out(run_script(cog.inFile, 'LANG=es_ES LC_CTYPE=es_ES LC_ALL=es_ES python locale_env_example.py', interpreter=None))
.. }}}

::

	$ LANG=es_ES LC_CTYPE=es_ES LC_ALL=es_ES python locale_env_example.py

	Environment settings:
		LC_ALL = es_ES
		LC_CTYPE = es_ES
		LANG = es_ES
		LANGUAGE = 
	
	Locale from environment: ('es_ES', 'ISO8859-1')
	
	Numeric formatting:
	
	  Decimal point      : ","
	  Grouping positions : [127]
	  Thousands separator: ""
	
	Monetary formatting:
	
	  International currency symbol             : "'EUR '"
	  Local currency symbol                     : 'Eu' (Eu)
	  Symbol precedes positive value            : 1
	  Symbol precedes negative value            : 1
	  Decimal point                             : ","
	  Digits in fractional values               : 2
	  Digits in fractional values, international: 2
	  Grouping positions                        : [3, 3, 0]
	  Thousands separator                       : "."
	  Positive sign                             : ""
	  Positive sign position                    : Before value and symbol
	  Negative sign                             : "-"
	  Negative sign position                    : Before value and symbol
	
	

.. {{{end}}}

Portgual (``pt_PT``):


.. {{{cog
.. cog.out(run_script(cog.inFile, 'LANG=pt_PT LC_CTYPE=pt_PT LC_ALL=pt_PT python locale_env_example.py', interpreter=None))
.. }}}

::

	$ LANG=pt_PT LC_CTYPE=pt_PT LC_ALL=pt_PT python locale_env_example.py

	Environment settings:
		LC_ALL = pt_PT
		LC_CTYPE = pt_PT
		LANG = pt_PT
		LANGUAGE = 
	
	Locale from environment: ('pt_PT', 'ISO8859-1')
	
	Numeric formatting:
	
	  Decimal point      : ","
	  Grouping positions : []
	  Thousands separator: " "
	
	Monetary formatting:
	
	  International currency symbol             : "'EUR '"
	  Local currency symbol                     : 'Eu' (Eu)
	  Symbol precedes positive value            : 0
	  Symbol precedes negative value            : 0
	  Decimal point                             : "."
	  Digits in fractional values               : 2
	  Digits in fractional values, international: 2
	  Grouping positions                        : [3, 3, 0]
	  Thousands separator                       : "."
	  Positive sign                             : ""
	  Positive sign position                    : Before value and symbol
	  Negative sign                             : "-"
	  Negative sign position                    : Before value and symbol
	
	

.. {{{end}}}


Poland (``pl_PL``):


.. {{{cog
.. cog.out(run_script(cog.inFile, 'LANG=pl_PL LC_CTYPE=pl_PL LC_ALL=pl_PL python locale_env_example.py', interpreter=None))
.. }}}

::

	$ LANG=pl_PL LC_CTYPE=pl_PL LC_ALL=pl_PL python locale_env_example.py

	Environment settings:
		LC_ALL = pl_PL
		LC_CTYPE = pl_PL
		LANG = pl_PL
		LANGUAGE = 
	
	Locale from environment: ('pl_PL', 'ISO8859-2')
	
	Numeric formatting:
	
	  Decimal point      : ","
	  Grouping positions : [3, 3, 0]
	  Thousands separator: " "
	
	Monetary formatting:
	
	  International currency symbol             : "'PLN '"
	  Local currency symbol                     : 'z\xc5\x82' (zł)
	  Symbol precedes positive value            : 1
	  Symbol precedes negative value            : 1
	  Decimal point                             : ","
	  Digits in fractional values               : 2
	  Digits in fractional values, international: 2
	  Grouping positions                        : [3, 3, 0]
	  Thousands separator                       : " "
	  Positive sign                             : ""
	  Positive sign position                    : After value
	  Negative sign                             : "-"
	  Negative sign position                    : After value
	
	

.. {{{end}}}


Currency
========

The example output above shows that changing the locale updates the
currency symbol setting and the character to separate whole numbers
from decimal fractions.  This example loops through several different
locales to print a positive and negative currency value formatted for
each locale:

.. include:: locale_currency_example.py
    :literal:
    :start-after: #end_pymotw_header

The output is this small table:

.. {{{cog
.. cog.out(run_script(cog.inFile, 'locale_currency_example.py'))
.. }}}

::

	$ python locale_currency_example.py

	                 USA:   $1234.56   -$1234.56
	              France: 1234,56 Eu  1234,56 Eu-
	               Spain: Eu 1234,56  -Eu 1234,56
	            Portugal: 1234.56 Eu  -1234.56 Eu
	              Poland: zł 1234,56  zł 1234,56-

.. {{{end}}}

Formatting Numbers
==================

Numbers not related to currency are also formatted differently
depending on the locale.  In particular, the *grouping* character used
to separate large numbers into readable chunks is changed:

.. include:: locale_grouping.py
   :literal:
   :start-after: #end_pymotw_header

To format numbers without the currency symbol, use :func:`format`
instead of :func:`currency`.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'locale_grouping.py'))
.. }}}

::

	$ python locale_grouping.py

	              Locale         Integer                Float
	                 USA         123,456           123,456.78
	              France          123456            123456,78
	               Spain          123456            123456,78
	            Portugal          123456            123456,78
	              Poland         123 456           123 456,78

.. {{{end}}}



Parsing Numbers
===============

Besides generating output in different formats, the :mod:`locale`
module helps with parsing input. It includes :func:`atoi` and
:func:`atof` functions for converting the strings to integer and
floating point values based on the locale's numerical formatting
conventions.

.. include:: locale_atof_example.py
    :literal:
    :start-after: #end_pymotw_header

The grouping and decimal separator values 

.. {{{cog
.. cog.out(run_script(cog.inFile, 'locale_atof_example.py'))
.. }}}

::

	$ python locale_atof_example.py

	                 USA:  1,234.56 => 1234.560000
	              France:   1234,56 => 1234.560000
	               Spain:   1234,56 => 1234.560000
	            Portugal:   1234.56 => 1234.560000
	              Poland:  1 234,56 => 1234.560000

.. {{{end}}}


Dates and Times
===============

Another important aspect of localization is date and time formatting:

.. include:: locale_date_example.py
    :literal:
    :start-after: #end_pymotw_header

.. {{{cog
.. cog.out(run_script(cog.inFile, 'locale_date_example.py'))
.. }}}

::

	$ python locale_date_example.py

	                 USA: Sun Oct 24 08:53:42 2010
	              France: Dim 24 oct 08:53:42 2010
	               Spain: dom 24 oct 08:53:42 2010
	            Portugal: Dom 24 Out 08:53:42 2010
	              Poland: ndz 24 paź 08:53:42 2010

.. {{{end}}}

This discussion only covers some of the high-level functions in the
:mod:`locale` module. There are others which are lower level
(:func:`format_string`) or which relate to managing the locale for
your application (:func:`resetlocale`).

.. seealso::

    `locale <http://docs.python.org/library/locale.html>`_
        The standard library documentation for this module.

    :mod:`gettext`
        Message catalogs for translations.
