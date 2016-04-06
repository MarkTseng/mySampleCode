=============================
pwd -- Unix Password Database
=============================

.. module:: pwd
    :synopsis: Unix Password Database

:Purpose: Read user data from Unix password database.
:Python Version: 1.4 and later

The pwd module can be used to read user information from the Unix
password database (usually ``/etc/passwd``).  The read-only interface
returns tuple-like objects with named attributes for the standard
fields of a password record.

===== ========= =======
Index Attribute Meaning
===== ========= =======
 0    pw_name   The user's login name
 1    pw_passwd Encrypted password (optional)
 2    pw_uid    User id (integer)
 3    pw_gid    Group id (integer)
 4    pw_gecos  Comment/full name
 5    pw_dir    Home directory
 6    pw_shell  Application started on login, usually a command interpreter
===== ========= =======

Querying All Users
==================

Suppose you need to print a report of all of the "real" users on a
system, including their home directories (for our purposes, "real" is
defined as having a name not starting with "``_``").  To load the
entire password database, you would use ``getpwall()``.  The return
value is a list with an undefined order, so you will need to sort it
before printing the report.

.. include:: pwd_getpwall.py
    :literal:
    :start-after: #end_pymotw_header

Most of the example code above deals with formatting the results
nicely.  The ``for`` loop at the end shows how to access fields from
the records by name.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'pwd_getpwall.py'))
.. }}}

::

	$ python pwd_getpwall.py

	User        UID Home Dir          Description
	---------- ---- ----------------- ------------------------------
	daemon        1 /var/root         System Services
	dhellmann   527 /Users/dhellmann  Doug Hellmann
	nobody     4294967294 /var/empty        Unprivileged User
	root          0 /var/root         System Administrator

.. {{{end}}}


Querying User By Name
=====================

If you need information about one user, it is not necessary to read
the entire password database.  Using ``getpwnam()``, you can retrieve
the information about a user by name.

.. include:: pwd_getpwnam.py
    :literal:
    :start-after: #end_pymotw_header

The passwords on my system are stored outside of the main user
database in a shadow file, so the password field, when set, is
reported as all ``*``.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'pwd_getpwnam.py dhellmann'))
.. cog.out(run_script(cog.inFile, 'pwd_getpwnam.py nobody', include_prefix=False))
.. }}}

::

	$ python pwd_getpwnam.py dhellmann

	Username: dhellmann
	Password: ********
	Comment : Doug Hellmann
	UID/GID : 527 / 501
	Home    : /Users/dhellmann
	Shell   : /bin/bash

	$ python pwd_getpwnam.py nobody

	Username: nobody
	Password: *
	Comment : Unprivileged User
	UID/GID : 4294967294 / 4294967294
	Home    : /var/empty
	Shell   : /usr/bin/false

.. {{{end}}}


Querying User By UID
====================

It is also possible to look up a user by their numerical user id.
This is useful to find the owner of a file:

.. include:: pwd_getpwuid_fileowner.py
    :literal:
    :start-after: #end_pymotw_header

.. {{{cog
.. cog.out(run_script(cog.inFile, 'pwd_getpwuid_fileowner.py'))
.. }}}

::

	$ python pwd_getpwuid_fileowner.py

	pwd_getpwuid_fileowner.py is owned by dhellmann (527)

.. {{{end}}}


The numeric user id is can also be used to find information about the
user currently running a process:

.. include:: pwd_getpwuid_process.py
    :literal:
    :start-after: #end_pymotw_header

.. {{{cog
.. cog.out(run_script(cog.inFile, 'pwd_getpwuid_process.py'))
.. }}}

::

	$ python pwd_getpwuid_process.py

	Currently running with UID=527 username=dhellmann

.. {{{end}}}


.. seealso::

    `pwd <http://docs.python.org/library/pwd.html>`_
        The standard library documentation for this module.

    :mod:`spwd`
        Secure password database access for systems using shadow passwords.

    :mod:`grp`
        The :mod:`grp` module reads Unix group information.
