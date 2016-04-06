.. _xml.etree.ElementTree.parsing:

=======================
 Parsing XML Documents
=======================

Parsed XML documents are represented in memory by :class:`ElementTree`
and :class:`Element` objects connected into a tree structure based on
the way the nodes in the XML document are nested.

Parsing an Entire Document
==========================

Parsing an entire document with :func:`parse()` returns an
:class:`ElementTree` instance.  The tree knows about all of the data
in the input document, and the nodes of the tree can be searched or
manipulated in place.  While this flexibility can make working with
the parsed document a little easier, it typically takes more memory
than an event-based parsing approach since the entire document must be
loaded at one time.

The memory footprint of small, simple documents such as this list of
podcasts represented as an OPML_ outline is not significant:

.. literalinclude:: podcasts.opml
   :language: xml

To parse the file, pass an open file handle to :func:`parse()`.

.. include:: ElementTree_parse_opml.py
   :literal:
   :start-after: #end_pymotw_header

It will read the data, parse the XML, and return an
:class:`ElementTree` object.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'ElementTree_parse_opml.py'))
.. }}}

::

	$ python ElementTree_parse_opml.py

	<xml.etree.ElementTree.ElementTree object at 0x10048b390>

.. {{{end}}}

Traversing the Parsed Tree
==========================

To visit all of the children in order, use :func:`iter` to create a
generator that iterates over the :class:`ElementTree` instance.

.. include:: ElementTree_dump_opml.py
   :literal:
   :start-after: #end_pymotw_header

This example prints the entire tree, one tag at a time.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'ElementTree_dump_opml.py'))
.. }}}

::

	$ python ElementTree_dump_opml.py

	opml {'version': '1.0'}
	head {}
	title {}
	dateCreated {}
	dateModified {}
	body {}
	outline {'text': 'Science and Tech'}
	outline {'xmlUrl': 'http://www.publicradio.org/columns/futuretense/podcast.xml', 'text': 'APM: Future Tense', 'type': 'rss', 'htmlUrl': 'http://www.publicradio.org/columns/futuretense/'}
	outline {'xmlUrl': 'http://www.npr.org/rss/podcast.php?id=510030', 'text': 'Engines Of Our Ingenuity Podcast', 'type': 'rss', 'htmlUrl': 'http://www.uh.edu/engines/engines.htm'}
	outline {'xmlUrl': 'http://www.nyas.org/Podcasts/Atom.axd', 'text': 'Science & the City', 'type': 'rss', 'htmlUrl': 'http://www.nyas.org/WhatWeDo/SciencetheCity.aspx'}
	outline {'text': 'Books and Fiction'}
	outline {'xmlUrl': 'http://feeds.feedburner.com/podiobooks', 'text': 'Podiobooker', 'type': 'rss', 'htmlUrl': 'http://www.podiobooks.com/blog'}
	outline {'xmlUrl': 'http://web.me.com/normsherman/Site/Podcast/rss.xml', 'text': 'The Drabblecast', 'type': 'rss', 'htmlUrl': 'http://web.me.com/normsherman/Site/Podcast/Podcast.html'}
	outline {'xmlUrl': 'http://www.tor.com/rss/category/TorDotStories', 'text': 'tor.com / category / tordotstories', 'type': 'rss', 'htmlUrl': 'http://www.tor.com/'}
	outline {'text': 'Computers and Programming'}
	outline {'xmlUrl': 'http://leo.am/podcasts/mbw', 'text': 'MacBreak Weekly', 'type': 'rss', 'htmlUrl': 'http://twit.tv/mbw'}
	outline {'xmlUrl': 'http://leo.am/podcasts/floss', 'text': 'FLOSS Weekly', 'type': 'rss', 'htmlUrl': 'http://twit.tv'}
	outline {'xmlUrl': 'http://www.coreint.org/podcast.xml', 'text': 'Core Intuition', 'type': 'rss', 'htmlUrl': 'http://www.coreint.org/'}
	outline {'text': 'Python'}
	outline {'xmlUrl': 'http://advocacy.python.org/podcasts/pycon.rss', 'text': 'PyCon Podcast', 'type': 'rss', 'htmlUrl': 'http://advocacy.python.org/podcasts/'}
	outline {'xmlUrl': 'http://advocacy.python.org/podcasts/littlebit.rss', 'text': 'A Little Bit of Python', 'type': 'rss', 'htmlUrl': 'http://advocacy.python.org/podcasts/'}
	outline {'xmlUrl': 'http://djangodose.com/everything/feed/', 'text': 'Django Dose Everything Feed', 'type': 'rss'}
	outline {'text': 'Miscelaneous'}
	outline {'xmlUrl': 'http://www.castsampler.com/cast/feed/rss/dhellmann/', 'text': "dhellmann's CastSampler Feed", 'type': 'rss', 'htmlUrl': 'http://www.castsampler.com/users/dhellmann/'}

.. {{{end}}}

To print only the groups of names and feed URLs for the podcasts,
leaving out of all of the data in the header section by iterating over
only the ``outline`` nodes and print the *text* and *xmlUrl*
attributes.

.. include:: ElementTree_show_feed_urls.py
   :literal:
   :start-after: #end_pymotw_header

The ``'outline'`` argument to :func:`iter` means processing is limited
to only nodes with the tag ``'outline'``.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'ElementTree_show_feed_urls.py'))
.. }}}

::

	$ python ElementTree_show_feed_urls.py

	Science and Tech
	  APM: Future Tense :: http://www.publicradio.org/columns/futuretense/podcast.xml
	  Engines Of Our Ingenuity Podcast :: http://www.npr.org/rss/podcast.php?id=510030
	  Science & the City :: http://www.nyas.org/Podcasts/Atom.axd
	Books and Fiction
	  Podiobooker :: http://feeds.feedburner.com/podiobooks
	  The Drabblecast :: http://web.me.com/normsherman/Site/Podcast/rss.xml
	  tor.com / category / tordotstories :: http://www.tor.com/rss/category/TorDotStories
	Computers and Programming
	  MacBreak Weekly :: http://leo.am/podcasts/mbw
	  FLOSS Weekly :: http://leo.am/podcasts/floss
	  Core Intuition :: http://www.coreint.org/podcast.xml
	Python
	  PyCon Podcast :: http://advocacy.python.org/podcasts/pycon.rss
	  A Little Bit of Python :: http://advocacy.python.org/podcasts/littlebit.rss
	  Django Dose Everything Feed :: http://djangodose.com/everything/feed/
	Miscelaneous
	  dhellmann's CastSampler Feed :: http://www.castsampler.com/cast/feed/rss/dhellmann/

.. {{{end}}}

Finding Nodes in a Document
===========================

Walking the entire tree like this searching for relevant nodes can be
error prone.  The example above had to look at each outline node to
determine if it was a group (nodes with only a :attr:`text` attribute)
or podcast (with both :attr:`text` and :attr:`xmlUrl`).  To produce a
simple list of the podcast feed URLs, without names or groups, for a
podcast downloader application, the logic could be simplified using
:func:`findall()` to look for nodes with more descriptive search
characteristics.

As a first pass at converting the above example, we can construct an
XPath_ argument to look for all outline nodes.

.. include:: ElementTree_find_feeds_by_tag.py
   :literal:
   :start-after: #end_pymotw_header

The logic in this version is not substantially different than the
version using :func:`getiterator()`.  It still has to check for the
presence of the URL, except that it does not print the group name when
the URL is not found.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'ElementTree_find_feeds_by_tag.py'))
.. }}}

::

	$ python ElementTree_find_feeds_by_tag.py

	http://www.publicradio.org/columns/futuretense/podcast.xml
	http://www.npr.org/rss/podcast.php?id=510030
	http://www.nyas.org/Podcasts/Atom.axd
	http://feeds.feedburner.com/podiobooks
	http://web.me.com/normsherman/Site/Podcast/rss.xml
	http://www.tor.com/rss/category/TorDotStories
	http://leo.am/podcasts/mbw
	http://leo.am/podcasts/floss
	http://www.coreint.org/podcast.xml
	http://advocacy.python.org/podcasts/pycon.rss
	http://advocacy.python.org/podcasts/littlebit.rss
	http://djangodose.com/everything/feed/
	http://www.castsampler.com/cast/feed/rss/dhellmann/

.. {{{end}}}

Another version can take advantage of the fact that the outline nodes
are only nested two levels deep.  Changing the search path to
``.//outline/outline`` mean the loop will process only the second
level of outline nodes.

.. include:: ElementTree_find_feeds_by_structure.py
   :literal:
   :start-after: #end_pymotw_header

All of those outline nodes nested two levels deep in the input are
expected to have the *xmlURL* attribute refering to the podcast feed,
so the loop can skip checking for for the attribute before using it.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'ElementTree_find_feeds_by_structure.py'))
.. }}}

::

	$ python ElementTree_find_feeds_by_structure.py

	http://www.publicradio.org/columns/futuretense/podcast.xml
	http://www.npr.org/rss/podcast.php?id=510030
	http://www.nyas.org/Podcasts/Atom.axd
	http://feeds.feedburner.com/podiobooks
	http://web.me.com/normsherman/Site/Podcast/rss.xml
	http://www.tor.com/rss/category/TorDotStories
	http://leo.am/podcasts/mbw
	http://leo.am/podcasts/floss
	http://www.coreint.org/podcast.xml
	http://advocacy.python.org/podcasts/pycon.rss
	http://advocacy.python.org/podcasts/littlebit.rss
	http://djangodose.com/everything/feed/
	http://www.castsampler.com/cast/feed/rss/dhellmann/

.. {{{end}}}

This version is limited to the existing structure, though, so if the
outline nodes are ever rearranged into a deeper tree it will stop
working.

Parsed Node Attributes
======================

The items returned by :func:`findall()` and :func:`iter()` are
:class:`Element` objects, each representing a node in the XML parse
tree.  Each :class:`Element` has attributes for accessing data pulled
out of the XML.  This can be illustrated with a somewhat more
contrived example input file, ``data.xml``:

.. literalinclude:: data.xml
   :language: xml
   :linenos:

The "attributes" of a node are available in the :attr:`attrib`
property, which acts like a dictionary.

.. include:: ElementTree_node_attributes.py
   :literal:
   :start-after: #end_pymotw_header

The node on line five of the input file has two attributes,
:attr:`name` and :attr:`foo`.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'ElementTree_node_attributes.py'))
.. }}}

::

	$ python ElementTree_node_attributes.py

	with_attributes
	  foo  = "bar"
	  name = "value"

.. {{{end}}}

The text content of the nodes is available, along with the "tail" text
that comes after the end of a close tag.

.. include:: ElementTree_node_text.py
   :literal:
   :start-after: #end_pymotw_header

The ``child`` node on line three contains embedded text, and the node
on line four has text with a tail (including any whitespace).

.. {{{cog
.. cog.out(run_script(cog.inFile, 'ElementTree_node_text.py'))
.. }}}

::

	$ python ElementTree_node_text.py

	child
	  child node text: This child contains text.
	  and tail text  : 
	  
	child_with_tail
	  child node text: This child has regular text.
	  and tail text  : And "tail" text.
	  

.. {{{end}}}

XML entity references embedded in the document are conveniently
converted to the appropriate characters before values are returned.

.. include:: ElementTree_entity_references.py
   :literal:
   :start-after: #end_pymotw_header

The automatic conversion mean the implementation detail of
representing certain characters in an XML document can be ignored.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'ElementTree_entity_references.py'))
.. }}}

::

	$ python ElementTree_entity_references.py

	entity_expansion
	  in attribute: This & That
	  in text     : That & This

.. {{{end}}}


Watching Events While Parsing
=============================

The other API useful for processing XML documents is event-based.  The
parser generates ``start`` events for opening tags and ``end`` events
for closing tags.  Data can be extracted from the document during the
parsing phase by iterating over the event stream, which is convenient
if it is not necessary to manipulate the entire document afterwards
and there is no need to hold the entire parsed document in memory.

:func:`iterparse()` returns an iterable that produces tuples
containing the name of the event and the node triggering the event.
Events can be one of:

``start``
  A new tag has been encountered.  The closing angle bracket of the
  tag was processed, but not the contents.
``end``
  The closing angle bracket of a closing tag has been processed.  All
  of the children were already processed.
``start-ns``
  Start a namespace declaration.
``end-ns``
  End a namespace declaration.

.. include:: ElementTree_show_all_events.py
   :literal:
   :start-after: #end_pymotw_header

By default, only ``end`` events are generated.  To see other events,
pass the list of desired event names to :func:`iterparse()`, as in
this example:

.. {{{cog
.. cog.out(run_script(cog.inFile, 'ElementTree_show_all_events.py'))
.. }}}

::

	$ python ElementTree_show_all_events.py

	start            opml         4299733776
	..start          head         4299733840
	....start        title        4299733904
	....end          title        4299733904
	....start        dateCreated  4299734096
	....end          dateCreated  4299734096
	....start        dateModified 4299734288
	....end          dateModified 4299734288
	..end            head         4299733840
	..start          body         4299734672
	....start        outline      4299734736
	......start      outline      4299734800
	......end        outline      4299734800
	......start      outline      4299734864
	......end        outline      4299734864
	......start      outline      4299734928
	......end        outline      4299734928
	....end          outline      4299734736
	....start        outline      4299734992
	......start      outline      4299759760
	......end        outline      4299759760
	......start      outline      4299759696
	......end        outline      4299759696
	......start      outline      4299759824
	......end        outline      4299759824
	....end          outline      4299734992
	....start        outline      4299759888
	......start      outline      4299760016
	......end        outline      4299760016
	......start      outline      4299760208
	......end        outline      4299760208
	......start      outline      4299760144
	......end        outline      4299760144
	....end          outline      4299759888
	....start        outline      4299760336
	......start      outline      4299760400
	......end        outline      4299760400
	......start      outline      4299760464
	......end        outline      4299760464
	......start      outline      4299760592
	......end        outline      4299760592
	....end          outline      4299760336
	....start        outline      4299760720
	......start      outline      4299760848
	......end        outline      4299760848
	....end          outline      4299760720
	..end            body         4299734672
	end              opml         4299733776

.. {{{end}}}

The event-style of processing is more natural for some operations,
such as converting XML input to some other format.  This technique can
be used to convert list of podcasts from the earlier examples from an
XML file to a CSV file, so they can be loaded into a spreadsheet or
database application.

.. include:: ElementTree_write_podcast_csv.py
   :literal:
   :start-after: #end_pymotw_header

This conversion program does not need to hold the entire parsed input
file in memory, and processing each node as it is encountered in the
input is more efficient.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'ElementTree_write_podcast_csv.py'))
.. }}}

::

	$ python ElementTree_write_podcast_csv.py

	"Science and Tech","APM: Future Tense","http://www.publicradio.org/columns/futuretense/podcast.xml","http://www.publicradio.org/columns/futuretense/"
	"Science and Tech","Engines Of Our Ingenuity Podcast","http://www.npr.org/rss/podcast.php?id=510030","http://www.uh.edu/engines/engines.htm"
	"Science and Tech","Science & the City","http://www.nyas.org/Podcasts/Atom.axd","http://www.nyas.org/WhatWeDo/SciencetheCity.aspx"
	"Books and Fiction","Podiobooker","http://feeds.feedburner.com/podiobooks","http://www.podiobooks.com/blog"
	"Books and Fiction","The Drabblecast","http://web.me.com/normsherman/Site/Podcast/rss.xml","http://web.me.com/normsherman/Site/Podcast/Podcast.html"
	"Books and Fiction","tor.com / category / tordotstories","http://www.tor.com/rss/category/TorDotStories","http://www.tor.com/"
	"Computers and Programming","MacBreak Weekly","http://leo.am/podcasts/mbw","http://twit.tv/mbw"
	"Computers and Programming","FLOSS Weekly","http://leo.am/podcasts/floss","http://twit.tv"
	"Computers and Programming","Core Intuition","http://www.coreint.org/podcast.xml","http://www.coreint.org/"
	"Python","PyCon Podcast","http://advocacy.python.org/podcasts/pycon.rss","http://advocacy.python.org/podcasts/"
	"Python","A Little Bit of Python","http://advocacy.python.org/podcasts/littlebit.rss","http://advocacy.python.org/podcasts/"
	"Python","Django Dose Everything Feed","http://djangodose.com/everything/feed/",""
	"Miscelaneous","dhellmann's CastSampler Feed","http://www.castsampler.com/cast/feed/rss/dhellmann/","http://www.castsampler.com/users/dhellmann/"

.. {{{end}}}


Creating a Custom Tree Builder
==============================

A potentially more efficient means of handling parse events is to
replace the standard tree builder behavior with a custom version.  The
:class:`ElementTree` parser uses an :class:`XMLTreeBuilder` to process
the XML and call methods on a target class to save the results.  The
usual output is an :class:`ElementTree` instance created by the
default :class:`TreeBuilder` class.  Replacing :class:`TreeBuilder`
with another class allows it to receive the events before the
:class:`Element` nodes are instantiated, saving that portion of the
overhead.

The XML-to-CSV converter from the previous section can be translated
to a tree builder.

.. include:: ElementTree_podcast_csv_treebuilder.py
   :literal:
   :start-after: #end_pymotw_header

:class:`PodcastListToCSV` implements the :class:`TreeBuilder`
protocol.  Each time a new XML tag is encountered, :func:`start()` is
called with the tag name and attributes.  When a closing tag is seen
:func:`end()` is called with the name.  In between, :func:`data()` is
called when a node has content (the tree builder is expected to keep
up with the "current" node).  When all of the input is processed,
:func:`close()` is called.  It can return a value, which will be
returned to the user of the :class:`XMLTreeBuilder`.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'ElementTree_podcast_csv_treebuilder.py'))
.. }}}

::

	$ python ElementTree_podcast_csv_treebuilder.py

	"Science and Tech","APM: Future Tense","http://www.publicradio.org/columns/futuretense/podcast.xml","http://www.publicradio.org/columns/futuretense/"
	"Science and Tech","Engines Of Our Ingenuity Podcast","http://www.npr.org/rss/podcast.php?id=510030","http://www.uh.edu/engines/engines.htm"
	"Science and Tech","Science & the City","http://www.nyas.org/Podcasts/Atom.axd","http://www.nyas.org/WhatWeDo/SciencetheCity.aspx"
	"Books and Fiction","Podiobooker","http://feeds.feedburner.com/podiobooks","http://www.podiobooks.com/blog"
	"Books and Fiction","The Drabblecast","http://web.me.com/normsherman/Site/Podcast/rss.xml","http://web.me.com/normsherman/Site/Podcast/Podcast.html"
	"Books and Fiction","tor.com / category / tordotstories","http://www.tor.com/rss/category/TorDotStories","http://www.tor.com/"
	"Computers and Programming","MacBreak Weekly","http://leo.am/podcasts/mbw","http://twit.tv/mbw"
	"Computers and Programming","FLOSS Weekly","http://leo.am/podcasts/floss","http://twit.tv"
	"Computers and Programming","Core Intuition","http://www.coreint.org/podcast.xml","http://www.coreint.org/"
	"Python","PyCon Podcast","http://advocacy.python.org/podcasts/pycon.rss","http://advocacy.python.org/podcasts/"
	"Python","A Little Bit of Python","http://advocacy.python.org/podcasts/littlebit.rss","http://advocacy.python.org/podcasts/"
	"Python","Django Dose Everything Feed","http://djangodose.com/everything/feed/",""
	"Miscelaneous","dhellmann's CastSampler Feed","http://www.castsampler.com/cast/feed/rss/dhellmann/","http://www.castsampler.com/users/dhellmann/"

.. {{{end}}}



Parsing Strings
===============

To work with smaller bits of XML text, especially string literals as
might be embedded in the source of a program, use :func:`XML()` and
the string containing the XML to be parsed as the only argument.

.. include:: ElementTree_XML.py
   :literal:
   :start-after: #end_pymotw_header

Notice that unlike with :func:`parse()`, the return value is an
:class:`Element` instance instead of an :class:`ElementTree`.  An
:class:`Element` supports the iterator protocol directly, so there is
no need to call :func:`getiterator`.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'ElementTree_XML.py'))
.. }}}

::

	$ python ElementTree_XML.py

	parsed = <Element 'root' at 0x10048ba90>
	group
	
	group
	

.. {{{end}}}

For structured XML that uses the :attr:`id` attribute to identify
unique nodes of interest, :func:`XMLID()` is a convenient way to
access the parse results.

.. include:: ElementTree_XMLID.py
   :literal:
   :start-after: #end_pymotw_header

:func:`XMLID()` returns the parsed tree as an :class:`Element` object,
along with a dictionary mapping the :attr:`id` attribute strings to
the individual nodes in the tree.

.. {{{cog
.. cog.out(run_script(cog.inFile, 'ElementTree_XMLID.py'))
.. }}}

::

	$ python ElementTree_XMLID.py

	a = <Element 'child' at 0x10048bbd0>
	b = <Element 'child' at 0x10048bc90>
	c = <Element 'child' at 0x10048bed0>

.. {{{end}}}


.. seealso::

   Outline Processor Markup Language, OPML_
       Dave Winer's OPML specification and documentation.

   XML Path Language, XPath_
       A syntax for identifying parts of an XML document.

   `XPath Support in ElementTree <http://effbot.org/zone/element-xpath.htm>`_
       Part of Fredrick Lundh's original documentation for ElementTree.

   :mod:`csv`
       Read and write comma-separated-value files

.. _OPML: http://www.opml.org/

.. _XPath: http://www.w3.org/TR/xpath/
