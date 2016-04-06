=================================
 socket -- Network Communication
=================================

.. module:: socket
    :synopsis: Network communication

:Purpose: Provides access to network communication
:Python Version: 1.4 and later

The :mod:`socket` module exposes the low-level C API for communicating
over a network using the BSD socket interface.  It includes the
:class:`socket` class, for handling the actual data channel, and
functions for network-related tasks such as converting a server's name
to an address and formatting data to be sent across the network.

.. toctree::

   addressing
   tcp
   udp
   uds
   multicast
   binary
   nonblocking


.. seealso::

    `socket <http://docs.python.org/library/socket.html>`_
        The standard library documentation for this module.

    `Socket Programming HOWOTO <http://docs.python.org/howto/sockets.html>`__
        An instructional guide by Gordon McMillan, included in the
        standard library documentation.

    :mod:`select`
        Testing a socket to see if it is ready for reading or writing.

    :mod:`SocketServer`
        Framework for creating network servers.

    :mod:`asyncore` and :mod:`asynchat`
        Frameworks for asynchronous communication.

    :mod:`urllib` and :mod:`urllib2`
        Most network clients should use the more convenient libraries
        for accessing remote resoruces through a URL.

    *Unix Network Programming, Volume 1: The Sockets Networking API, 3/E*
        By W. Richard Stevens, Bill Fenner, and Andrew
        M. Rudoff. Published by Addison-Wesley Professional, 2004.
        ISBN-10: 0131411551
