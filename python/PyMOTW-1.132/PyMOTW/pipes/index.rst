==============================================
pipes -- Unix shell command pipeline templates
==============================================

.. module:: pipes
    :synopsis: Unix shell command pipeline templates

:Purpose: Create repeatable Unix shell command pipelines.
:Python Version: Python 1.4

The :mod:`pipes` module implements a class to create arbitrarily
complex Unix command pipelines.  Inputs and outputs of the commands
can be chained together as with the shell ``|`` operator, even if the
individual commands need to write to or read from files instead of
stdin/stdout.


Passing Standard I/O Through a Pipe
===================================

A very simple example, passing standard input through a pipe and
receiving the results in a file looks like this:

.. include:: pipes_simple_write.py
    :literal:
    :start-after: #end_pymotw_header

The pipeline Template is created and then a single command, ``cat -``
is added.  The command reads standard input and writes it to standard
output, without modification.  The second argument to ``append()``
encodes the input and output sources for the command in two characters
(input, then output).  Using ``-`` means the command uses standard
I/O.  Using ``f`` means the command needs to read from a file (as may
be the case with an image processing pipeline).

The ``debug()`` method toggles debugging output on and off.  When
debugging is enabled, the commands being run are printed and the shell
is given ``set -x`` so it runs verbosely.

After the pipeline is set up, a NamedTemporaryFile is created to give
the pipeline somewhere to write its output.  A file must always be
specified as argument to ``open()``, whether reading or writing.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'pipes_simple_write.py'))
.. }}}

::

	$ python pipes_simple_write.py

	+ cat -
	cat - >/var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmpRmHZrz
	Some text

.. {{{end}}}

Reading from a pipeline works basically the same way, with a few
changes to the arguments.  For our example, we need to set up the
contents of the input file before opening the pipline.  Then we can
pass that filename as input to ``open()``.

.. include:: pipes_simple_read.py
    :literal:
    :start-after: #end_pymotw_header

We can read the results from the pipeline directly.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'pipes_simple_read.py'))
.. }}}

::

	$ python pipes_simple_read.py

	+ cat -
	cat - </var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmp1EA4kT
	Some text

.. {{{end}}}

Using Files Instead of Streams
==============================

Some commands need to work on files on the filesystem instead of input
streams.  Commands that process a large amount of data might perform
better in this mode, since they will not block on the next command
reading their output.  Anything that works on non-stream-based data
requires this capability as well (e.g., databases or other binary file
manipulation tools).  To support this mode of operation, ``append()``
lets you specify a *kind* of ``f``, and the pipeline code will create
the needed temporary files.  The filenames are passed to the shell as
``$IN`` and ``$OUT``, so those variable names need to appear in your
command string.

.. include:: pipes_file_kind.py
    :literal:
    :start-after: #end_pymotw_header

As you see, several intermediate temporary files are created to hold
the input and output of the step.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'pipes_file_kind.py'))
.. }}}

::

	$ python pipes_file_kind.py

	+ trap 'rm -f /var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmplUuJ9T; exit' 1 2 3 13 14 15
	+ cat
	+ IN=/var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmplUuJ9T
	+ OUT=/var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmpKl905J
	+ cat /var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmplUuJ9T
	+ rm -f /var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmplUuJ9T
	trap 'rm -f /var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmplUuJ9T; exit' 1 2 3 13 14 15
	cat >/var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmplUuJ9T
	IN=/var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmplUuJ9T; OUT=/var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmpKl905J; cat $IN > $OUT
	rm -f /var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmplUuJ9T
	Some text

.. {{{end}}}

And the input and output *kind* values can be mixed, so that different
steps of the pipeline use files or standard I/O as needed.

.. include:: pipes_mixed_kinds.py
    :literal:
    :start-after: #end_pymotw_header

The trap statements visible in the output here ensure that the
temporary files created by the pipeline are cleaned up even if a task
fails in the middle or the shell is killed.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'pipes_mixed_kinds.py'))
.. }}}

::

	$ python pipes_mixed_kinds.py

	+ trap 'rm -f /var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmpmV2Cfg; exit' 1 2 3 13 14 15
	+ cat
	+ IN=/var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmpmV2Cfg
	+ cat /var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmpmV2Cfg
	+ OUT=/var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmpZf9ElN
	+ cat -
	+ rm -f /var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmpmV2Cfg
	trap 'rm -f /var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmpmV2Cfg; exit' 1 2 3 13 14 15
	cat >/var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmpmV2Cfg
	IN=/var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmpmV2Cfg; cat $IN |
	{ OUT=/var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmpZf9ElN; cat - > $OUT; }
	rm -f /var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmpmV2Cfg
	Some text

.. {{{end}}}


A More Complex Example
======================

All of the examples up to this point have been fairly trivial.  They
were constructed to illustrate how to use ``pipes.Template()`` without
depending on deep knowledge of shell scripting in general.  This
example is more complex, and shows how several commands can be
combined to manipulate data before bringing it into Python.

My `virtualenvwrapper
<http://www.doughellmann.com/projects/virtualenvwrapper/>`_ script
includes a shell function for listing all of the virtual environments
you have created.  The function is used for tab-completion and can be
called directly to list the environments, in case you forget a name.
The heart of that function is a small pipeline that looks in
``$WORKON_HOME`` for directories that look like virtual environments
(i.e., they have an ``activate`` script).  That pipeline is:

::

    (cd "$WORKON_HOME"; for f in */bin/activate; do echo $f; done) \
        | sed 's|^\./||' \
        | sed 's|/bin/activate||' \
        | sort

Implemented using :mod:`pipes`, the pipeline looks like:

.. include:: pipes_multistep.py
    :literal:
    :start-after: #end_pymotw_header

Since each sandbox name is written to a separate line, parsing the
output is easy:

.. {{{cog
.. cog.out(run_script(cog.inFile, 'pipes_multistep.py'))
.. }}}

::

	$ python pipes_multistep.py

	SANDBOXES:
	['CastSampler',
	 'bbtest',
	 'bill',
	 'blogbackup',
	 'cursive.tools',
	 'docket',
	 'docket-new',
	 'doughellmann',
	 'email_recovery',
	 'ical2org',
	 'libcloud',
	 'mytweets',
	 'nose',
	 'odtwritertest',
	 'paramiko',
	 'pearson',
	 'personal',
	 'psf-sprints',
	 'psfblog',
	 'psfboard',
	 'pulse',
	 'puzzles',
	 'pyatl',
	 'pymag',
	 'pymotw',
	 'pymotw-ja',
	 'pymotw-ja-2.6',
	 'pymotw.save',
	 'pymotw26',
	 'pymotw27',
	 'python-dev',
	 'racemi',
	 'racemi_status',
	 'reporting_server',
	 'rst2marsedit',
	 'sphinx-contrib',
	 'sphinx-contrib.old',
	 'venvwtest',
	 'virtualenvwrapper']

.. {{{end}}}


Passing Files Through Pipelines
===============================

If the input to your pipeline already exists in a file on disk,
there's no need to read it into Python simply to pass it to the
pipeline.  You can use the ``copy()`` method to pass the file directly
through the pipeline and create an output file for reading.

.. include:: pipes_copy.py
    :literal:
    :start-after: #end_pymotw_header

.. {{{cog
.. cog.out(run_script(cog.inFile, 'pipes_copy.py'))
.. }}}

::

	$ python pipes_copy.py

	+ IN=lorem.txt
	+ grep -n tortor lorem.txt
	IN=lorem.txt; grep -n tortor $IN >/var/folders/9R/9R1t+tR02Raxzk+F71Q50U+++Uw/-Tmp-/tmplmpcEA
	3:elementum elit tortor eu quam. Duis tincidunt nisi ut ante. Nulla
	6:lacus. Praesent placerat tortor sed nisl. Nunc blandit diam egestas
	11:eget velit auctor tortor blandit sollicitudin. Suspendisse imperdiet
	

.. {{{end}}}


Cloning Templates
=================

Once you have a pipeline template, you may want to use it multiple
times or create variants without re-constructing the entire object.
The ``clone()`` method makes both of these operations easy.  This
example constructs a simple word-counter pipeline, then prepends
commands to a couple of clones to make it look for different words.

.. include:: pipes_clone.py
    :literal:
    :start-after: #end_pymotw_header

By prepending a custom command to each clone, we can create separate
pipelines that perform the same basic function with small variations.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'pipes_clone.py'))
.. }}}

::

	$ python pipes_clone.py

	  "py":  1379
	"perl":    71

.. {{{end}}}



.. seealso::

    `pipes <http://docs.python.org/library/pipes.html>`_
        The standard library documentation for this module.

    :mod:`tempfile`
        The tempfile module includes classes for managing temporary files.
    
    :mod:`subprocess`
        The subprocess module also supports chaining the inputs and
        outputs of processes together.
