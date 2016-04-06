===========================
pdb -- Interactive Debugger
===========================

.. module:: pdb
    :synopsis: Interactive Debugger

:Purpose: Python's Interactive Debugger
:Python Version: 1.4 and later

:mod:`pdb` implements an interactive debugging environment for Python
programs.  It includes features to let you pause your program, look at
the values of variables, and watch program execution step-by-step, so
you can understand what your program actually does and find bugs in
the logic.

Starting the Debugger
=====================

The first step to using :mod:`pdb` is causing the interpreter to enter
the debugger when you want it to.  There are a few different ways to
do that, depending on your starting conditions and what you need to
debug.

From the Command Line
---------------------

The most straightforward way to use the debugger is to run it from the
command line, giving it your own program as input so it knows what to
run.

.. literalinclude:: pdb_script.py
   :linenos: 

Running the debugger from the command line causes it to load your
source file and stop execution on the first statement it finds.  In
this case, it stops before evaluating the definition of the class
:class:`MyObj` on line 7.

::

    $ python -m pdb pdb_script.py 
    > .../pdb_script.py(7)<module>()
    -> """
    (Pdb) 

.. note::

    Normally :mod:`pdb` includes the full path to each module in the
    output when printing a filename.  In order to maintain clear
    examples, the sample output in this section replaces the path with
    ``...``.

Within the Interpreter
----------------------

Many Python developers work with the interactive interpreter while
developing early versions of modules because it lets them experiment
more iteratively without the save/run/repeat cycle needed when
creating standalone scripts.  To run the debugger from within an
interactive interpreter, use :func:`run` or :func:`runeval`.

::

    $ python
    Python 2.7 (r27:82508, Jul  3 2010, 21:12:11) 
    [GCC 4.0.1 (Apple Inc. build 5493)] on darwin
    Type "help", "copyright", "credits" or "license" for more information.
    >>> import pdb_script
    >>> import pdb
    >>> pdb.run('pdb_script.MyObj(5).go()')
    > <string>(1)<module>()
    (Pdb) 

The argument to :func:`run` is a string expression that can be
evaluated by the Python interpreter.  The debugger will parse it, then
pause execution just before the first expression evaluates.  You can
use the debugger commands described below to navigate and control the
execution.

From Within Your Program
------------------------

Both of the previous examples assume you want to start the debugger at
the beginning of your program.  For a long-running process where the
problem appears much later in the program execution, it will be more
convenient to start the debugger from inside your program using
:func:`set_trace`.

.. literalinclude:: pdb_set_trace.py
   :linenos: 

Line 16 of the sample script triggers the debugger at that point in
execution.

::

    $ python ./pdb_set_trace.py 
    > .../pdb_set_trace.py(17)go()
    -> print i
    (Pdb) 

:func:`set_trace` is just a Python function, so you can call it at any
point in your program.  This lets you enter the debugger based on
conditions inside your program, including from an exception handler or
via a specific branch of a control statement.

After a Failure
---------------

Debugging a failure after a program terminates is called *post-mortem*
debugging.  :mod:`pdb` supports post-mortem debugging through the
:func:`pm` and :func:`post_mortem` functions.

.. literalinclude:: pdb_post_mortem.py
   :linenos:

Here the incorrect attribute name on line 13 triggers an
:ref:`AttributeError <exceptions-AttributeError>` exception, causing
execution to stop. :func:`pm` looks for the active traceback and
starts the debugger at the point in the call stack where the exception
occurred.

::

    $ python
    Python 2.7 (r27:82508, Jul  3 2010, 21:12:11) 
    [GCC 4.0.1 (Apple Inc. build 5493)] on darwin
    Type "help", "copyright", "credits" or "license" for more information.
    >>> from pdb_post_mortem import MyObj
    >>> MyObj(5).go()
    Traceback (most recent call last):
      File "<stdin>", line 1, in <module>
      File "pdb_post_mortem.py", line 13, in go
        for i in range(self.num_loops):
    AttributeError: 'MyObj' object has no attribute 'num_loops'
    >>> import pdb
    >>> pdb.pm()
    > .../pdb_post_mortem.py(13)go()
    -> for i in range(self.num_loops):
    (Pdb) 
    

Controlling the Debugger
========================

You interact with the debugger using a small command language that
lets you move around the call stack, examine and change the values of
variables, and control how the debugger executes your program.  The
interactive debugger uses :mod:`readline` to accept commands.
Entering a blank line re-runs the previous command again, unless it
was a :command:`list` operation.

Navigating the Execution Stack
------------------------------

At any point while the debugger is running you can use
:command:`where` (abbreviated :command:`w`) to find out exactly what
line is being executed and where on the call stack you are.  In this
case, the module ``pdb_set_trace.py`` line 17 in the :func:`go`
method.

::

    $ python pdb_set_trace.py 
    > .../pdb_set_trace.py(17)go()
    -> print i
    (Pdb) where
      .../pdb_set_trace.py(21)<module>()
    -> MyObj(5).go()
    > .../pdb_set_trace.py(17)go()
    -> print i

To add more context around the current location, use :command:`list`
(:command:`l`).

::

    (Pdb) list
     12             self.count = num_loops
     13  
     14         def go(self):
     15             for i in range(self.count):
     16                 pdb.set_trace()
     17  ->             print i
     18             return
     19  
     20     if __name__ == '__main__':
     21         MyObj(5).go()
    [EOF]
    (Pdb) 

The default is to list 11 lines around the current line (five before
and five after).  Using :command:`list` with a single numerical
argument lists 11 lines around that line instead of the current line.

::

    (Pdb) list 14
      9     class MyObj(object):
     10  
     11         def __init__(self, num_loops):
     12             self.count = num_loops
     13  
     14         def go(self):
     15             for i in range(self.count):
     16                 pdb.set_trace()
     17  ->             print i
     18             return
     19  

If :command:`list` receives two arguments, it interprets them as the
first and last lines to include in its output.

::

    (Pdb) list 5, 19
      5     #
      6  
      7     import pdb
      8  
      9     class MyObj(object):
     10  
     11         def __init__(self, num_loops):
     12             self.count = num_loops
     13  
     14         def go(self):
     15             for i in range(self.count):
     16                 pdb.set_trace()
     17  ->             print i
     18             return
     19  

Move between frames within the current call stack using :command:`up`
and down.  :command:`up` (abbreviated :command:`u`) moves towards
older frames on the stack.  :command:`down` (abbreviated :command:`d`)
moves towards newer frames.

::

    (Pdb) up
    > .../pdb_set_trace.py(21)<module>()
    -> MyObj(5).go()

    (Pdb) down
    > .../pdb_set_trace.py(17)go()
    -> print i

Each time you move up or down the debugger prints the current location
in the same format as produced by :command:`where`.

Examining Variables on the Stack
--------------------------------

Each frame on the stack maintains a set of variables, including values
local to the function being executed and global state information.
:mod:`pdb` provides several ways to examine the contents of those
variables.

.. literalinclude:: pdb_function_arguments.py
   :linenos: 

The :command:`args` command (abbreviated :command:`a`) prints all of
the arguments to the function active in the current frame.  This
example also uses a recursive function to show what a deeper stack
looks like when printed by :command:`where`.

::

    $ python pdb_function_arguments.py 
    > .../pdb_function_arguments.py(14)recursive_function()
    -> return
    (Pdb) where
      .../pdb_function_arguments.py(17)<module>()
    -> recursive_function()
      .../pdb_function_arguments.py(11)recursive_function()
    -> recursive_function(n-1)
      .../pdb_function_arguments.py(11)recursive_function()
    -> recursive_function(n-1)
      .../pdb_function_arguments.py(11)recursive_function()
    -> recursive_function(n-1)
      .../pdb_function_arguments.py(11)recursive_function()
    -> recursive_function(n-1)
      .../pdb_function_arguments.py(11)recursive_function()
    -> recursive_function(n-1)
    > .../pdb_function_arguments.py(14)recursive_function()
    -> return

    (Pdb) args
    n = 0
    output = to be printed

    (Pdb) up
    > .../pdb_function_arguments.py(11)recursive_function()
    -> recursive_function(n-1)

    (Pdb) args
    n = 1
    output = to be printed

    (Pdb) 

The :command:`p` command evaluates an expression given as argument and
prints the result.  You can also use Python's ``print`` statement, but
that is passed through to the interpreter to be executed rather than
running as a command in the debugger.

::

    (Pdb) p n
    1

    (Pdb) print n
    1

Similarly, prefixing an expression with :command:`!` passes it to the
Python interpreter to be evaluated.  You can use this feature to
execute arbitrary Python statements, including modifying variables.
This example changes the value of *output* before letting the debugger
continue running the program.  The next statement after the call to
:func:`set_trace` prints the value of *output*, showing the modified
value.

::

    $ python pdb_function_arguments.py 
    > .../pdb_function_arguments.py(14)recursive_function()
    -> print output

    (Pdb) !output
    'to be printed'

    (Pdb) !output='changed value'

    (Pdb) continue
    changed value

For more complicated values such as nested or large data structures,
use :command:`pp` to "pretty print" them.  This program reads several
lines of text from a file.  

.. literalinclude:: pdb_pp.py
   :linenos: 

Printing the variable ``lines`` with :command:`p` results in output
that is difficult to read because it wraps awkwardly.  :command:`pp`
uses :mod:`pprint` to format the value for clean printing.

::

    $ python pdb_pp.py 
    --Return--
    > .../pdb_pp.py(12)<module>()->None
    -> pdb.set_trace()
    (Pdb) p lines
    ['Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Donec\n', 'egestas, enim 
    et consectetuer ullamcorper, lectus ligula rutrum leo, a\n', 'elementum elit tortor 
    eu quam.\n']

    (Pdb) pp lines
    ['Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Donec\n',
     'egestas, enim et consectetuer ullamcorper, lectus ligula rutrum leo, a\n',
     'elementum elit tortor eu quam.\n']

    (Pdb) 


Stepping Through Your Program
-----------------------------

In addition to navigating up and down the call stack when the program
is paused, you can also step through execution of the program past the
point where it enters the debugger.

.. literalinclude:: pdb_step.py
   :linenos: 

Use :command:`step` to execute the current line and then stop at the
next execution point -- either the first statement inside a function
being called or the next line of the current function.

::

    $ python pdb_step.py 
    > /Users/dhellmann/Documents/PyMOTW/src.pdb/PyMOTW/pdb/pdb_step.py(17)<module>()
    -> f(5)

The interpreter pauses at the call to :func:`set_trace` and gives
control to the debugger.  The first step causes the execution to enter
:func:`f`.


::

    (Pdb) step
    --Call--
    > .../pdb_step.py(9)f()
    -> def f(n):

One more step moves execution to the first line of :func:`f` and
starts the loop.

::

    (Pdb) step
    > /Users/dhellmann/Documents/PyMOTW/src.pdb/PyMOTW/pdb/pdb_step.py(10)f()
    -> for i in range(n):

Stepping again moves to the first line inside the loop where ``j`` is
defined.

::

    (Pdb) step
    > /Users/dhellmann/Documents/PyMOTW/src.pdb/PyMOTW/pdb/pdb_step.py(11)f()
    -> j = i * n
    (Pdb) p i
    0

The value of ``i`` is ``0``, so after one more step the value of ``j``
should also be ``0``.

::

    (Pdb) step
    > /Users/dhellmann/Documents/PyMOTW/src.pdb/PyMOTW/pdb/pdb_step.py(12)f()
    -> print i, j

    (Pdb) p j
    0

    (Pdb) 

Stepping one line at a time like this can become tedious if there is a
lot of code to cover before the point where the error occurs, or if
the same function is called repeatedly.

.. literalinclude:: pdb_next.py
   :linenos: 

In this example, there is nothing wrong with :func:`calc`, so stepping
through it each time it is called in the loop in :func:`f` obscures
the useful output by showing all of the lines of :func:`calc` as they
are executed.

::

    $ python pdb_next.py 
    > .../pdb_next.py(21)<module>()
    -> f(5)
    (Pdb) step
    --Call--
    > .../pdb_next.py(13)f()
    -> def f(n):

    (Pdb) step
    > .../pdb_next.py(14)f()
    -> for i in range(n):

    (Pdb) step
    > .../pdb_next.py(15)f()
    -> j = calc(i, n)

    (Pdb) step
    --Call--
    > .../pdb_next.py(9)calc()
    -> def calc(i, n):

    (Pdb) step
    > .../pdb_next.py(10)calc()
    -> j = i * n

    (Pdb) step
    > .../pdb_next.py(11)calc()
    -> return j

    (Pdb) step
    --Return--
    > .../pdb_next.py(11)calc()->0
    -> return j

    (Pdb) step
    > .../pdb_next.py(16)f()
    -> print i, j

    (Pdb) step
    0 0


The :command:`next` command is like step, but does not enter functions
called from the statement being executed.  In effect, it steps all the
way through the function call to the next statement in the current
function in a single operation.

::

    > .../pdb_next.py(14)f()
    -> for i in range(n):
    (Pdb) step
    > .../pdb_next.py(15)f()
    -> j = calc(i, n)

    (Pdb) next
    > .../pdb_next.py(16)f()
    -> print i, j

    (Pdb) 

The :command:`until` command is like :command:`next`, except it
explicitly continues until execution reaches a line in the same
function with a line number higher than the current value.  That
means, for example, that :command:`until` can be used to step past the
end of a loop.

::

    $ python pdb_next.py 
    > .../pdb_next.py(21)<module>()
    -> f(5)
    (Pdb) step
    --Call--
    > .../pdb_next.py(13)f()
    -> def f(n):

    (Pdb) step
    > .../pdb_next.py(14)f()
    -> for i in range(n):

    (Pdb) step
    > .../pdb_next.py(15)f()
    -> j = calc(i, n)

    (Pdb) next
    > .../pdb_next.py(16)f()
    -> print i, j

    (Pdb) until
    0 0
    1 5
    2 10
    3 15
    4 20
    > .../pdb_next.py(17)f()
    -> return

    (Pdb) 

Before :command:`until` was run, the current line was 16, the last
line of the loop.  After :command:`until` ran, execution was on line
17, and the loop had been exhausted.

:command:`return` is another short-cut for bypassing parts of a
function.  It continues executing until the function is about to
execute a ``return`` statement, and then it pauses.  This gives you
time to look at the return value before the function returns.

::

    $ python pdb_next.py 
    > .../pdb_next.py(21)<module>()
    -> f(5)
    (Pdb) step
    --Call--
    > .../pdb_next.py(13)f()
    -> def f(n):

    (Pdb) step
    > .../pdb_next.py(14)f()
    -> for i in range(n):

    (Pdb) return
    0 0
    1 5
    2 10
    3 15
    4 20
    --Return--
    > .../pdb_next.py(17)f()->None
    -> return

    (Pdb) 

Breakpoints
===========

As programs grow even longer, even using :command:`next` and
:command:`until` will become slow and cumbersome.  Instead of stepping
through the program by hand, a better solution is to let it run
normally until it reaches a point where you want the debugger to
interrupt it.  You could use :func:`set_trace` to start the debugger,
but that only works if there is only one point you want to pause the
program.  It is more convenient to run the program through the
debugger, but tell the debugger where to stop in advance using
*breakpoints*.  The debugger monitors your program, and when it
reaches the location described by a breakpoint the program is paused
before the line is executed.

.. literalinclude:: pdb_break.py
   :linenos: 

There are several options to the :command:`break` command used for
setting break points.  You can specify the line number, file, and
function where processing should pause.  To set a breakpoint on a
specific line of the current file, use ``break lineno``:

::

    $ python -m pdb pdb_break.py 
    > .../pdb_break.py(7)<module>()
    -> def calc(i, n):
    (Pdb) break 11
    Breakpoint 1 at .../pdb_break.py:11

    (Pdb) continue
    i = 0
    j = 0
    i = 1
    j = 5
    > .../pdb_break.py(11)calc()
    -> print 'Positive!'

    (Pdb) 

The command :command:`continue` tells the debugger to keep running
your program until the next breakpoint.  In this case, it runs through
the first iteration of the ``for`` loop in :func:`f` and stops inside
:func:`calc` during the second iteration.

Breakpoints can also be set to the first line of a function by
specifying the function name instead of a line number.  This example
shows what happens if a breakpoint is added for the :func:`calc`
function.

::

    $ python -m pdb pdb_break.py 
    > .../pdb_break.py(7)<module>()
    -> def calc(i, n):
    (Pdb) break calc
    Breakpoint 1 at .../pdb_break.py:7

    (Pdb) continue
    i = 0
    > .../pdb_break.py(8)calc()
    -> j = i * n

    (Pdb) where
      .../pdb_break.py(21)<module>()
    -> f(5)
      .../pdb_break.py(17)f()
    -> j = calc(i, n)
    > .../pdb_break.py(8)calc()
    -> j = i * n

    (Pdb) 

To specify a breakpoint in another file, prefix the line or function
argument with a filename.

.. literalinclude:: pdb_break_remote.py
   :linenos: 

Here a breakpoint is set for line 11 of ``pdb_break.py`` after
starting the main program ``pdb_break_remote.py``.

::

    $ python -m pdb pdb_break_remote.py 
    > .../pdb_break_remote.py(7)<module>()
    -> from pdb_break import f
    (Pdb) break pdb_break.py:11
    Breakpoint 1 at .../pdb_break.py:11

    (Pdb) continue
    i = 0
    j = 0
    i = 1
    j = 5
    > .../pdb_break.py(11)calc()
    -> print 'Positive!'

    (Pdb) 

The filename can be a full path to the source file, or a relative path
to a file available on ``sys.path``.

To list the breakpoints currently set, use :command:`break` without
any arguments.  The output includes the file and line number of each
break point, as well as information about how many times it has been
encountered.

::

    $ python -m pdb pdb_break.py
    > .../pdb_break.py(7)<module>()
    -> def calc(i, n):
    (Pdb) break 11
    Breakpoint 1 at .../pdb_break.py:11

    (Pdb) break
    Num Type         Disp Enb   Where
    1   breakpoint   keep yes   at .../pdb_break.py:11

    (Pdb) continue
    i = 0
    j = 0
    i = 1
    j = 5
    > .../pdb/pdb_break.py(11)calc()
    -> print 'Positive!'

    (Pdb) continue
    Positive!
    i = 2
    j = 10
    > .../pdb_break.py(11)calc()
    -> print 'Positive!'

    (Pdb) break
    Num Type         Disp Enb   Where
    1   breakpoint   keep yes   at .../pdb_break.py:11
            breakpoint already hit 2 times

    (Pdb) 

Managing Breakpoints
--------------------

As each new breakpoint is added, it is assigned a numerical
identifier.  These ID numbers are used to enable, disable, and remove
the breakpoints interactively.

Turning off a breakpoint with :command:`disable` tells the debugger
not to stop when that line is reached.  The breakpoint is remembered,
but ignored.

::

    $ python -m pdb pdb_break.py
    > .../pdb_break.py(7)<module>()
    -> def calc(i, n):
    (Pdb) break calc
    Breakpoint 1 at .../pdb_break.py:7

    (Pdb) break 11
    Breakpoint 2 at .../pdb_break.py:11

    (Pdb) break
    Num Type         Disp Enb   Where
    1   breakpoint   keep yes   at .../pdb_break.py:7
    2   breakpoint   keep yes   at .../pdb_break.py:11

    (Pdb) disable 1

    (Pdb) break
    Num Type         Disp Enb   Where
    1   breakpoint   keep no    at .../pdb_break.py:7
    2   breakpoint   keep yes   at .../pdb_break.py:11

    (Pdb) continue
    i = 0
    j = 0
    i = 1
    j = 5
    > .../pdb_break.py(11)calc()
    -> print 'Positive!'

    (Pdb) 

The debugging session below sets two breakpoints in the program, then
disables one.  The program is run until the remaining breakpoint is
encountered, and then the other breakpoint is turned back on with
:command:`enable` before execution continues.

::

    $ python -m pdb pdb_break.py
    > .../pdb_break.py(7)<module>()
    -> def calc(i, n):
    (Pdb) break calc
    Breakpoint 1 at .../pdb_break.py:7

    (Pdb) break 16
    Breakpoint 2 at .../pdb_break.py:16

    (Pdb) disable 1

    (Pdb) continue
    > .../pdb_break.py(16)f()
    -> print 'i =', i

    (Pdb) list
     11             print 'Positive!'
     12         return j
     13  
     14     def f(n):
     15         for i in range(n):
     16 B->         print 'i =', i
     17             j = calc(i, n)
     18         return
     19  
     20     if __name__ == '__main__':
     21         f(5)

    (Pdb) continue
    i = 0
    j = 0
    > .../pdb_break.py(16)f()
    -> print 'i =', i

    (Pdb) list
     11             print 'Positive!'
     12         return j
     13  
     14     def f(n):
     15         for i in range(n):
     16 B->         print 'i =', i
     17             j = calc(i, n)
     18         return
     19  
     20     if __name__ == '__main__':
     21         f(5)

    (Pdb) p i
    1

    (Pdb) enable 1

    (Pdb) continue
    i = 1
    > .../pdb_break.py(8)calc()
    -> j = i * n

    (Pdb) list
      3     #
      4     # Copyright (c) 2010 Doug Hellmann.  All rights reserved.
      5     #
      6  
      7 B   def calc(i, n):
      8  ->     j = i * n
      9         print 'j =', j
     10         if j > 0:
     11             print 'Positive!'
     12         return j
     13  
    
    (Pdb) 

The lines prefixed with ``B`` in the output from :command:`list` show
where the breakpoints are set in the program (lines 9 and 18).

Use :command:`clear` to delete a breakpoint entirely.

::

    $ python -m pdb pdb_break.py
    > .../pdb_break.py(7)<module>()
    -> def calc(i, n):
    (Pdb) break calc
    Breakpoint 1 at .../pdb_break.py:7

    (Pdb) break 11
    Breakpoint 2 at .../pdb_break.py:11

    (Pdb) break 16
    Breakpoint 3 at .../pdb_break.py:16

    (Pdb) break
    Num Type         Disp Enb   Where
    1   breakpoint   keep yes   at .../pdb_break.py:7
    2   breakpoint   keep yes   at .../pdb_break.py:11
    3   breakpoint   keep yes   at .../pdb_break.py:16

    (Pdb) clear 2
    Deleted breakpoint 2

    (Pdb) break
    Num Type         Disp Enb   Where
    1   breakpoint   keep yes   at .../pdb_break.py:7
    3   breakpoint   keep yes   at .../pdb_break.py:16

    (Pdb) 

The other breakpoints retain their original identifiers and are not
renumbered.

Temporary Breakpoints
---------------------

A temporary breakpoint is automatically cleared the first time program
execution hits it.  Using a temporary breakpoint lets you reach a
particular spot in the program flow quickly, just as with a regular
breakpoint, but since it is cleared immediately it does not interfere
with subsequent progress if that part of the program is run
repeatedly.

::

    $ python -m pdb pdb_break.py
    > .../pdb_break.py(7)<module>()
    -> def calc(i, n):
    (Pdb) tbreak 11
    Breakpoint 1 at .../pdb_break.py:11

    (Pdb) continue
    i = 0
    j = 0
    i = 1
    j = 5
    Deleted breakpoint 1
    > .../pdb_break.py(11)calc()
    -> print 'Positive!'

    (Pdb) break

    (Pdb) continue
    Positive!
    i = 2
    j = 10
    Positive!
    i = 3
    j = 15
    Positive!
    i = 4
    j = 20
    Positive!
    The program finished and will be restarted
    > .../pdb_break.py(7)<module>()
    -> def calc(i, n):

    (Pdb) 

After the program reaches line 11 the first time, the breakpoint is
removed and execution does not stop again until the program finishes.

Conditional Breakpoints
-----------------------

Rules can be applied to breakpoints so that execution only stops when
the conditions are met.  Using conditional breakpoints gives you finer
control over how the debugger pauses your program than manually
enabling and disabling breakpoints yourself.

Conditional breakpoints can be set in two ways.  The first is to
specify the condition when the breakpoint is set using
:command:`break`.

::

    $ python -m pdb pdb_break.py
    > .../pdb_break.py(7)<module>()
    -> def calc(i, n):
    (Pdb) break 9, j>0
    Breakpoint 1 at .../pdb_break.py:9

    (Pdb) break
    Num Type         Disp Enb   Where
    1   breakpoint   keep yes   at .../pdb_break.py:9
            stop only if j>0

    (Pdb) continue
    i = 0
    j = 0
    i = 1
    > .../pdb_break.py(9)calc()
    -> print 'j =', j

    (Pdb) 

The condition argument must be an expression using values visible in
the stack frame where the breakpoint is defined.  If the expression
evaluates as true, execution stops at the breakpoint.

A condition can also be applied to an existing breakpoint using the
:command:`condition` command.  The arguments are the breakpoint ID and
the expression.

::

    $ python -m pdb pdb_break.py
    > .../pdb_break.py(7)<module>()
    -> def calc(i, n):
    (Pdb) break 9
    Breakpoint 1 at .../pdb_break.py:9

    (Pdb) break
    Num Type         Disp Enb   Where
    1   breakpoint   keep yes   at .../pdb_break.py:9

    (Pdb) condition 1 j>0

    (Pdb) break
    Num Type         Disp Enb   Where
    1   breakpoint   keep yes   at .../pdb_break.py:9
            stop only if j>0

    (Pdb) 

Ignoring Breakpoints
--------------------

Programs with a lot of looping or recursive calls to the same function
are often easier to debug by "skipping ahead" in the execution,
instead of watching every call or breakpoint.  The :command:`ignore`
command tells the debugger to pass over a breakpoint without stopping.
Each time processing encounteres the breakpoint, it decrements the
ignore counter.  When the counter is zero, the breakpoint is
re-activated.

::

    $ python -m pdb pdb_break.py
    > .../pdb_break.py(7)<module>()
    -> def calc(i, n):
    (Pdb) break 17
    Breakpoint 1 at .../pdb_break.py:17

    (Pdb) continue
    i = 0
    > .../pdb_break.py(17)f()
    -> j = calc(i, n)

    (Pdb) next
    j = 0
    > .../pdb_break.py(15)f()
    -> for i in range(n):

    (Pdb) ignore 1 2
    Will ignore next 2 crossings of breakpoint 1.

    (Pdb) break
    Num Type         Disp Enb   Where
    1   breakpoint   keep yes   at .../pdb_break.py:17
            ignore next 2 hits
            breakpoint already hit 1 time

    (Pdb) continue
    i = 1
    j = 5
    Positive!
    i = 2
    j = 10
    Positive!
    i = 3
    > .../pdb_break.py(17)f()
    -> j = calc(i, n)

    (Pdb) break
    Num Type         Disp Enb   Where
    1   breakpoint   keep yes   at .../pdb_break.py:17
            breakpoint already hit 4 times

Explicitly resetting the ignore count to zero re-enables the
breakpoint immediately.

::

    $ python -m pdb pdb_break.py
    > .../pdb_break.py(7)<module>()
    -> def calc(i, n):
    (Pdb) break 17
    Breakpoint 1 at .../pdb_break.py:17

    (Pdb) ignore 1 2
    Will ignore next 2 crossings of breakpoint 1.

    (Pdb) break
    Num Type         Disp Enb   Where
    1   breakpoint   keep yes   at .../pdb_break.py:17
            ignore next 2 hits

    (Pdb) ignore 1 0
    Will stop next time breakpoint 1 is reached.

    (Pdb) break
    Num Type         Disp Enb   Where
    1   breakpoint   keep yes   at .../pdb_break.py:17

Triggering Actions on a Breakpoint
----------------------------------

In addition to the purely interactive mode, :mod:`pdb` supports basic
scripting.  Using :command:`commands`, you can define a series of
interpreter commands, including Python statements, to be executed when
a specific breakpoint is encountered.  After running
:command:`commands` with the breakpoint number as argument, the
debugger prompt changes to ``(com)``.  Enter commands one a time, and
finish the list with ``end`` to save the script and return to the main
debugger prompt.

::

    $ python -m pdb pdb_break.py
    > .../pdb_break.py(7)<module>()
    -> def calc(i, n):
    (Pdb) break 9
    Breakpoint 1 at .../pdb_break.py:9

    (Pdb) commands 1
    (com) print 'debug i =', i
    (com) print 'debug j =', j
    (com) print 'debug n =', n
    (com) end

    (Pdb) continue
    i = 0
    debug i = 0
    debug j = 0
    debug n = 5
    > .../pdb_break.py(9)calc()
    -> print 'j =', j

    (Pdb) continue
    j = 0
    i = 1
    debug i = 1
    debug j = 5
    debug n = 5
    > .../pdb_break.py(9)calc()
    -> print 'j =', j

    (Pdb) 

This feature is especially useful for debugging code that uses a lot
of data structures or variables, since you can have the debugger print
out all of the values automatically, instead of doing it manually each
time the breakpoint is encountered.

Changing Execution Flow
=======================

The :command:`jump` command lets you alter the flow of your program at
runtime, without modifying the code.  You can skip forwards to avoid
running some code, or backwards to run it again.  This sample program
generates a list of numbers.

.. literalinclude:: pdb_jump.py
   :linenos: 

When run without interference the output is a sequence of increasing
numbers divisible by ``5``.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'pdb_jump.py'))
.. }}}

::

	$ python pdb_jump.py

	[5, 15, 30, 50, 75]

.. {{{end}}}

Jump Ahead
----------

Jumping ahead moves the point of execution past the current location
without evaluating any of the statements in between.  By skipping over
line 13 in the example below, the value of ``j`` is not incremented
and all of the subsequent values that depend on it are a little
smaller.

::

    $ python -m pdb pdb_jump.py  
    > .../pdb_jump.py(7)<module>()
    -> def f(n):
    (Pdb) break 12
    Breakpoint 1 at .../pdb_jump.py:12

    (Pdb) continue
    > .../pdb_jump.py(12)f()
    -> j += n

    (Pdb) p j
    0

    (Pdb) step
    > .../pdb_jump.py(13)f()
    -> result.append(j)

    (Pdb) p j
    5

    (Pdb) continue
    > .../pdb_jump.py(12)f()
    -> j += n

    (Pdb) jump 13
    > .../pdb_jump.py(13)f()
    -> result.append(j)

    (Pdb) p j
    10

    (Pdb) disable 1

    (Pdb) continue
    [5, 10, 25, 45, 70]

    The program finished and will be restarted
    > .../pdb_jump.py(7)<module>()
    -> def f(n):
    (Pdb) 

Jump Back
---------

Jumps can also move the program execution to a statement that has
already been executed, to run it again.  Here, the value of ``j`` is
incremented an extra time, so the numbers in the result sequence are
all larger than they would otherwise be.

::

    $ python -m pdb pdb_jump.py 
    > .../pdb_jump.py(7)<module>()
    -> def f(n):
    (Pdb) break 13
    Breakpoint 1 at .../pdb_jump.py:13

    (Pdb) continue
    > .../pdb_jump.py(13)f()
    -> result.append(j)

    (Pdb) p j
    5

    (Pdb) jump 12
    > .../pdb_jump.py(12)f()
    -> j += n

    (Pdb) continue
    > .../pdb_jump.py(13)f()
    -> result.append(j)

    (Pdb) p j
    10

    (Pdb) disable 1

    (Pdb) continue
    [10, 20, 35, 55, 80]

    The program finished and will be restarted
    > .../pdb_jump.py(7)<module>()
    -> def f(n):
    (Pdb) 

Illegal Jumps
-------------

Jumping in and out of certain flow control statements is dangerous or
undefined, and therefore prevented by the debugger.

.. literalinclude:: pdb_no_jump.py
   :linenos: 

You can jump into a function, but if you do the arguments are not
defined and the code is unlikely to work.

::

    $ python -m pdb pdb_no_jump.py 
    > .../pdb_no_jump.py(7)<module>()
    -> def f(n):
    (Pdb) break 21
    Breakpoint 1 at .../pdb_no_jump.py:21

    (Pdb) jump 8
    > .../pdb_no_jump.py(8)<module>()
    -> if n < 0:

    (Pdb) p n
    *** NameError: NameError("name 'n' is not defined",)

    (Pdb) args

    (Pdb) 

You cannot jump into the middle of a block such as a ``for`` loop or
``try:except`` statement.

::

    $ python -m pdb pdb_no_jump.py 
    > .../pdb_no_jump.py(7)<module>()
    -> def f(n):
    (Pdb) break 21
    Breakpoint 1 at .../pdb_no_jump.py:21

    (Pdb) continue
    > .../pdb_no_jump.py(21)<module>()
    -> print f(5)

    (Pdb) jump 26
    *** Jump failed: can't jump into the middle of a block

    (Pdb) 

The code in a ``finally`` block must all be executed, so you cannot
jump out of the block.

::

    $ python -m pdb pdb_no_jump.py 
    > .../pdb_no_jump.py(7)<module>()
    -> def f(n):
    (Pdb) break 23
    Breakpoint 1 at .../pdb_no_jump.py:23

    (Pdb) continue
    [5, 15, 30, 50, 75]
    > .../pdb_no_jump.py(23)<module>()
    -> print 'Always printed'

    (Pdb) jump 25
    *** Jump failed: can't jump into or out of a 'finally' block

    (Pdb) 

And the most basic restriction is that jumping is constrained to the
bottom frame on the call stack.  If you move up the stack to examine
variables, you cannot change the execution flow at that point.

::

    $ python -m pdb pdb_no_jump.py 
    > .../pdb_no_jump.py(7)<module>()
    -> def f(n):
    (Pdb) break 11
    Breakpoint 1 at .../pdb_no_jump.py:11

    (Pdb) continue
    > .../pdb_no_jump.py(11)f()
    -> j = 0

    (Pdb) where
      /Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/bdb.py(379)run()
    -> exec cmd in globals, locals
      <string>(1)<module>()
      .../pdb_no_jump.py(21)<module>()
    -> print f(5)
    > .../pdb_no_jump.py(11)f()
    -> j = 0

    (Pdb) up
    > .../pdb_no_jump.py(21)<module>()
    -> print f(5)

    (Pdb) jump 25
    *** You can only jump within the bottom frame

    (Pdb) 


Restarting Your Program
-----------------------

When the debugger reaches the end of your program, it automatically
starts it over, but you can also restart it explicitly without leaving
the debugger and losing your breakpoints or other settings.

.. literalinclude:: pdb_run.py
   :linenos: 

Running the above program to completion within the debugger prints the
name of the script file, since no other arguments were given on the
command line.

::

    $ python -m pdb pdb_run.py 
    > .../pdb_run.py(7)<module>()
    -> import sys
    (Pdb) continue

    Command line args: ['pdb_run.py']
    The program finished and will be restarted
    > .../pdb_run.py(7)<module>()
    -> import sys
    
    (Pdb)

The program can be restarted using :command:`run`.  Arguments passed
to :command:`run` are parsed with :mod:`shlex` and passed to the
program as though they were command line arguments, so you can restart
the program with different settings.

::

    (Pdb) run a b c "this is a long value"
    Restarting pdb_run.py with arguments:
            a b c this is a long value
    > .../pdb_run.py(7)<module>()
    -> import sys
    
    (Pdb) continue
    Command line args: ['pdb_run.py', 'a', 'b', 'c', 'this is a long value']
    The program finished and will be restarted
    > .../pdb_run.py(7)<module>()
    -> import sys

    (Pdb) 

:command:`run` can also be used at any other point in processing to
restart the program.

::

    $ python -m pdb pdb_run.py 
    > .../pdb_run.py(7)<module>()
    -> import sys
    (Pdb) break 10
    Breakpoint 1 at .../pdb_run.py:10

    (Pdb) continue
    > .../pdb_run.py(10)f()
    -> print 'Command line args:', sys.argv

    (Pdb) run one two three
    Restarting pdb_run.py with arguments:
            one two three
    > .../pdb_run.py(7)<module>()
    -> import sys

    (Pdb) 


Customizing the Debugger with Aliases
=====================================

You can avoid typing complex commands repeatedly by using
:command:`alias` to define a shortcut.  Alias expansion is applied to
the first word of each command.  The body of the alias can consist of
any command that is legal to type at the debugger prompt, including
other debugger commands and pure Python expressions.  Recursion is
allowed in alias definitions, so one alias can even invoke another.

::

    $ python -m pdb pdb_function_arguments.py 
    > .../pdb_function_arguments.py(7)<module>()
    -> import pdb
    (Pdb) break 10
    Breakpoint 1 at .../pdb_function_arguments.py:10

    (Pdb) continue
    > .../pdb_function_arguments.py(10)recursive_function()
    -> if n > 0:

    (Pdb) pp locals().keys()
    ['output', 'n']

    (Pdb) alias pl pp locals().keys()

    (Pdb) pl
    ['output', 'n']

Running :command:`alias` without any arguments shows the list of
defined aliases.  A single argument is assumed to be the name of an
alias, and its definition is printed.

::

    (Pdb) alias
    pl = pp locals().keys()

    (Pdb) alias pl
    pl = pp locals().keys()
    (Pdb) 

Arguments to the alias are referenced using ``%n`` where *n* is
replaced with a number indicating the position of the argument,
starting with ``1``.  To consume all of the arguments, use ``%*``.

::

    $ python -m pdb pdb_function_arguments.py 
    > .../pdb_function_arguments.py(7)<module>()
    -> import pdb
    (Pdb) alias ph !help(%1)

    (Pdb) ph locals
    Help on built-in function locals in module __builtin__:
        
    locals(...)
        locals() -> dictionary
        
        Update and return a dictionary containing the current scope's local variables.

Clear the definition of an alias with :command:`unalias`.

::

    (Pdb) unalias ph

    (Pdb) ph locals
    *** SyntaxError: invalid syntax (<stdin>, line 1)

    (Pdb) 


Saving Configuration Settings
=============================

Debugging a program involves a lot of repetition; running the code,
observing the output, adjusting the code or inputs, and running it
again.  :mod:`pdb` attempts to cut down on the amount of repetition
you need to use to control the debugging experience, to let you
concentrate on your code instead of the debugger.  To help reduce the
number of times you issue the same commands to the debugger,
:mod:`pdb` lets you save configuration using text files read and
interpreted on startup.  

The file ``~/.pdbrc`` is read first, allowing you to set global
personal preferences for all of your debugging sessions.  Then
``./.pdbrc`` is read from the current working directory, so you can
set local preferences for a particular project.

::

    $ cat ~/.pdbrc
    # Show python help
    alias ph !help(%1)
    # Overridden alias
    alias redefined p 'home definition'

    $ cat .pdbrc
    # Breakpoints
    break 10
    # Overridden alias
    alias redefined p 'local definition'

    $ python -m pdb pdb_function_arguments.py 
    Breakpoint 1 at .../pdb_function_arguments.py:10
    > .../pdb_function_arguments.py(7)<module>()
    -> import pdb
    (Pdb) alias
    ph = !help(%1)
    redefined = p 'local definition'

    (Pdb) break
    Num Type         Disp Enb   Where
    1   breakpoint   keep yes   at .../pdb_function_arguments.py:10

    (Pdb) 

Any configuration commands that can be typed at the debugger prompt
can be saved in one of the startup files, but most commands that
control the execution (:command:`continue`, :command:`jump`, etc.)
cannot.  The exception is :command:`run`, which means you can set the
command line arguments for a debugging session in ``./.pdbrc`` so they
are consistent across several runs.

.. seealso::

    `pdb <http://docs.python.org/library/pdb.html>`_
        The standard library documentation for this module.

    :mod:`readline`
        Interactive prompt editing library.

    :mod:`cmd`
        Build interactive programs.

    :mod:`shlex`
        Shell command line parsing.
