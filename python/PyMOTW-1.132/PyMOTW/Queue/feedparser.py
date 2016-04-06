#!/usr/bin/env python
"""Universal feed parser

Visit http://diveintomark.org/projects/feed_parser/ for the latest version

Handles RSS 0.9x, RSS 1.0, RSS 2.0, CDF, Atom feeds

Required: Python 2.1 or later
Recommended: Python 2.3 or later
Recommended: libxml2 <http://xmlsoft.org/python.html>
"""

__version__ = "3.0-fc-1"
__author__ = "Mark Pilgrim <http://diveintomark.org/>"
__copyright__ = "Copyright 2002-4, Mark Pilgrim"
__contributors__ = ["Jason Diamond <http://injektilo.org/>",
                    "John Beimler <http://john.beimler.org/>",
                    "Fazal Majid <http://www.majid.info/mylos/weblog/>"]
__license__ = "Python"
_debug = 0
_debug_never_use_libxml2 = 0

# if you are embedding feedparser in a larger application, you should change this to your application name and URL
USER_AGENT = "UniversalFeedParser/%s%s +http://diveintomark.org/projects/feed_parser/" % (__version__, _debug and "-debug" or "")

# If you want feedparser to automatically run HTML markup through HTML Tidy, set this to 1.
# This is off by default because of reports of crashing on some platforms.  If it crashes
# for you, please submit a bug report with your OS platform, Python version, and the URL
# of the feed you were attempting to parse.
# Requires mxTidy <http://www.egenix.com/files/python/mxTidy.html>
TIDY_MARKUP = 0

# ---------- required modules (should come with any Python distribution) ----------
import sgmllib, re, sys, copy, urlparse, time, rfc822, types
try:
    from cStringIO import StringIO as _StringIO
except:
    from StringIO import StringIO as _StringIO

# ---------- optional modules (feedparser will work without these, but with reduced functionality) ----------

# gzip is included with most Python distributions, but may not be available if you compiled your own
try:
    import gzip
except:
    gzip = None
    
# timeoutsocket allows feedparser to time out rather than hang forever on ultra-slow servers.
# Python 2.3 now has this functionality available in the standard socket library, so under
# 2.3 you don't need to install anything.  But you probably should anyway, because the socket
# module is buggy and timeoutsocket is better.
try:
    import timeoutsocket # http://www.timo-tasi.org/python/timeoutsocket.py
    timeoutsocket.setDefaultSocketTimeout(10)
except ImportError:
    import socket
    if hasattr(socket, 'setdefaulttimeout'):
        socket.setdefaulttimeout(10)
import urllib2

_mxtidy = None
if TIDY_MARKUP:
    try:
        from mx.Tidy import Tidy as _mxtidy
    except:
        pass

# If a real XML parser is available, feedparser will attempt to use it.  feedparser works
# with both the built-in SAX parser and PyXML SAX parser.  On platforms where the Python
# distribution does not come with an XML parser (such as Mac OS X 10.2 and some versions of
# FreeBSD), feedparser will just fall back on regex-based parsing.  If XML libraries are
# available but the feed turns out not to be well-formed XML, feedparser will fall back
# on regex-based parsing and set the "bozo" bit in the results to indicate that the feed
# author is a bozo who can't generate well-formed XML.  The two advantages of using a real
# XML parser are (1) Unicode support, and (2) to get people to stop yelling at me for not
# using one.
try:
    import xml.sax
    from xml.sax.saxutils import escape as _xmlescape
    class CharacterEncodingOverride(xml.sax.SAXException): pass
    _XML_AVAILABLE = 1
except:
    _XML_AVAILABLE = 0
    def _xmlescape(data):
        data = data.replace("&", "&amp;")
        data = data.replace(">", "&gt;")
        data = data.replace("<", "&lt;")
        return data

# base64 support for Atom feeds that contain embedded binary data
try:
    import base64, binascii
except:
    base64 = binascii = None
    
# ---------- don't touch these ----------
sgmllib.tagfind = re.compile('[a-zA-Z][-_.:a-zA-Z0-9]*')
sgmllib.special = re.compile('<!')

SUPPORTED_VERSIONS = {'': 'unknown',
                      'rss090': 'RSS 0.90',
                      'rss091n': 'RSS 0.91 (Netscape)',
                      'rss091u': 'RSS 0.91 (Userland)',
                      'rss092': 'RSS 0.92',
                      'rss093': 'RSS 0.93',
                      'rss094': 'RSS 0.94',
                      'rss20': 'RSS 2.0',
                      'rss10': 'RSS 1.0',
                      'rss': 'RSS (unknown version)',
                      'atom01': 'Atom 0.1',
                      'atom02': 'Atom 0.2',
                      'atom03': 'Atom 0.3',
                      'atom': 'Atom (unknown version)',
                      'cdf': 'CDF',
                      'hotrss': 'Hot RSS'
                      }

try:
    dict
except NameError:
    # Python 2.1 does not have a built-in dict() function
    def dict(aList):
        rc = {}
        for k, v in aList:
            rc[k] = v
        return rc

from UserDict import UserDict
class FeedParserDict(UserDict):
    def __getitem__(self, key):
        keymap = {'channel': 'feed',
                  'items': 'entries',
                  'guid': 'id',
                  'date': 'modified',
                  'date_parsed': 'modified_parsed'}
        key = keymap.get(key, key)
        return UserDict.__getitem__(self, key)

    def has_key(self, key):
        return hasattr(self, key) or UserDict.has_key(self, key)
        
    def __getattr__(self, key):
        try:
            return self.__dict__[key]
        except KeyError:
            pass
        try:
            return self.__getitem__(key)
        except:
            raise AttributeError, "object has no attribute '%s'" % key

class _FeedParserMixin:
    namespaces = {"": "",
                  "http://backend.userland.com/rss": "",
                  "http://blogs.law.harvard.edu/tech/rss": "",
                  "http://purl.org/rss/1.0/": "",
                  "http://my.netscape.com/rdf/simple/0.9/": "",
                  "http://example.com/newformat#": "",
                  "http://example.com/necho": "",
                  "http://purl.org/echo/": "",
                  "uri/of/echo/namespace#": "",
                  "http://purl.org/pie/": "",
                  "http://purl.org/atom/ns#": "",
                  "http://purl.org/rss/1.0/modules/rss091#": "",
                  
                  "http://webns.net/mvcb/":                               "admin",
                  "http://purl.org/rss/1.0/modules/aggregation/":         "ag",
                  "http://purl.org/rss/1.0/modules/annotate/":            "annotate",
                  "http://media.tangent.org/rss/1.0/":                    "audio",
                  "http://backend.userland.com/blogChannelModule":        "blogChannel",
                  "http://web.resource.org/cc/":                          "cc",
                  "http://backend.userland.com/creativeCommonsRssModule": "creativeCommons",
                  "http://purl.org/rss/1.0/modules/company":              "co",
                  "http://purl.org/rss/1.0/modules/content/":             "content",
                  "http://my.theinfo.org/changed/1.0/rss/":               "cp",
                  "http://purl.org/dc/elements/1.1/":                     "dc",
                  "http://purl.org/dc/terms/":                            "dcterms",
                  "http://purl.org/rss/1.0/modules/email/":               "email",
                  "http://purl.org/rss/1.0/modules/event/":               "ev",
                  "http://postneo.com/icbm/":                             "icbm",
                  "http://purl.org/rss/1.0/modules/image/":               "image",
                  "http://xmlns.com/foaf/0.1/":                           "foaf",
                  "http://freshmeat.net/rss/fm/":                         "fm",
                  "http://purl.org/rss/1.0/modules/link/":                "l",
                  "http://madskills.com/public/xml/rss/module/pingback/": "pingback",
                  "http://prismstandard.org/namespaces/1.2/basic/":       "prism",
                  "http://www.w3.org/1999/02/22-rdf-syntax-ns#":          "rdf",
                  "http://www.w3.org/2000/01/rdf-schema#":                "rdfs",
                  "http://purl.org/rss/1.0/modules/reference/":           "ref",
                  "http://purl.org/rss/1.0/modules/richequiv/":           "reqv",
                  "http://purl.org/rss/1.0/modules/search/":              "search",
                  "http://purl.org/rss/1.0/modules/slash/":               "slash",
                  "http://purl.org/rss/1.0/modules/servicestatus/":       "ss",
                  "http://hacks.benhammersley.com/rss/streaming/":        "str",
                  "http://purl.org/rss/1.0/modules/subscription/":        "sub",
                  "http://purl.org/rss/1.0/modules/syndication/":         "sy",
                  "http://purl.org/rss/1.0/modules/taxonomy/":            "taxo",
                  "http://purl.org/rss/1.0/modules/threading/":           "thr",
                  "http://purl.org/rss/1.0/modules/textinput/":           "ti",
                  "http://madskills.com/public/xml/rss/module/trackback/":"trackback",
                  "http://wellformedweb.org/CommentAPI/":                 "wfw",
                  "http://purl.org/rss/1.0/modules/wiki/":                "wiki",
                  "http://schemas.xmlsoap.org/soap/envelope/":            "soap",
                  "http://www.w3.org/1999/xhtml":                         "xhtml",
                  "http://www.w3.org/XML/1998/namespace":                 "xml"
}

    can_be_relative_uri = ['link', 'id', 'wfw_comment', 'wfw_commentrss', 'docs', 'url', 'comments']
    can_contain_relative_uris = ['content', 'description', 'title', 'summary', 'info', 'tagline', 'copyright']
    can_contain_dangerous_markup = ['content', 'description', 'title', 'summary', 'info', 'tagline', 'copyright']
    html_types = ['text/html', 'application/xhtml+xml']
    
    def __init__(self, baseuri=None, encoding='utf-8'):
        if _debug: sys.stderr.write("initializing FeedParser\n")
        self.feeddata = FeedParserDict() # feed-level data
        self.encoding = encoding # character encoding
        self.entries = [] # list of entry-level data
        self.version = '' # feed type/version, see SUPPORTED_VERSIONS

        # the following are used internally to track state;
        # some of this is kind of out of control and should
        # probably be refactored into a finite state machine
        self.infeed = 0
        self.inentry = 0
        self.incontent = 0
        self.intextinput = 0
        self.inimage = 0
        self.inauthor = 0
        self.incontributor = 0
        self.contentparams = FeedParserDict()
        self.namespacemap = {}
        self.elementstack = []
        self.basestack = []
        self.langstack = []
        self.baseuri = baseuri or ''
        self.lang = None

    def unknown_starttag(self, tag, attrs):
        if _debug: sys.stderr.write('start %s with %s\n' % (tag, attrs))
        # normalize attrs
        attrs = [(k.lower(), sgmllib.charref.sub(lambda m: unichr(int(m.groups()[0])), v).strip()) for k, v in attrs]
        attrs = [(k, k in ('rel', 'type') and v.lower() or v) for k, v in attrs]
        
        # track xml:base and xml:lang
        attrsD = dict(attrs)
        baseuri = attrsD.get('xml:base', attrsD.get('base'))
        if baseuri:
            if _debug: sys.stderr.write('self.baseuri=%s\n' % baseuri)
            self.baseuri = baseuri
        lang = attrsD.get('xml:lang', attrsD.get('lang'))
        if lang:
            self.lang = lang
        self.basestack.append(baseuri)
        self.langstack.append(lang)
        
        # track namespaces
        for prefix, uri in attrs:
            if prefix.startswith('xmlns:'):
                self.trackNamespace(prefix[6:], uri)
            elif prefix == 'xmlns':
                self.trackNamespace(None, uri)

        # track inline content
        if self.incontent and self.contentparams.get('mode') == 'escaped':
            # element declared itself as escaped markup, but it isn't really
            self.contentparams['mode'] = 'xml'
        if self.incontent and self.contentparams.get('mode') == 'xml':
            # Note: probably shouldn't simply recreate localname here, but
            # our namespace handling isn't actually 100% correct in cases where
            # the feed redefines the default namespace (which is actually
            # the usual case for inline content, thanks Sam), so here we
            # cheat and just reconstruct the element based on localname
            # because that compensates for the bugs in our namespace handling.
            # This will horribly munge inline content with non-empty qnames,
            # but nobody actually does that, so I'm not fixing it.
            tag = tag.split(':')[-1]
            return self.handle_data("<%s%s>" % (tag, "".join([' %s="%s"' % t for t in attrs])), escape=0)

        # match namespaces
        if tag.find(':') <> -1:
            prefix, suffix = tag.split(':', 1)
        else:
            prefix, suffix = '', tag
        prefix = self.namespacemap.get(prefix, prefix)
        if prefix:
            prefix = prefix + '_'

        # special hack for better tracking of empty textinput/image elements in illformed feeds
        if (not prefix) and tag not in ('title', 'link', 'description', 'name'):
            self.intextinput = 0
        if (not prefix) and tag not in ('title', 'link', 'description', 'url', 'width', 'height'):
            self.inimage = 0
        
        # call special handler (if defined) or default handler
        methodname = '_start_' + prefix + suffix
        try:
            method = getattr(self, methodname)
            return method(attrsD)
        except AttributeError:
            return self.push(prefix + suffix, 1)

    def unknown_endtag(self, tag):
        if _debug: sys.stderr.write('end %s\n' % tag)
        # match namespaces
        if tag.find(':') <> -1:
            prefix, suffix = tag.split(':', 1)
        else:
            prefix, suffix = '', tag
        prefix = self.namespacemap.get(prefix, prefix)
        if prefix:
            prefix = prefix + '_'

        # call special handler (if defined) or default handler
        methodname = '_end_' + prefix + suffix
        try:
            method = getattr(self, methodname)
            method()
        except AttributeError:
            self.pop(prefix + suffix)

        # track inline content
        if self.incontent and self.contentparams.get('mode') == 'escaped':
            # element declared itself as escaped markup, but it isn't really
            self.contentparams['mode'] = 'xml'
        if self.incontent and self.contentparams.get('mode') == 'xml':
            tag = tag.split(':')[-1]
            self.handle_data("</%s>" % tag, escape=0)

        # track xml:base and xml:lang going out of scope
        if self.basestack:
            self.basestack.pop()
            if self.basestack and self.basestack[-1]:
                baseuri = self.basestack[-1]
                if _debug: sys.stderr.write('self.baseuri=%s\n' % baseuri)
                self.baseuri = baseuri
        if self.langstack:
            lang = self.langstack.pop()
            if lang:
                self.lang = lang

    def handle_charref(self, ref):
        # called for each character reference, e.g. for "&#160;", ref will be "160"
        # Reconstruct the original character reference.
        if not self.elementstack: return
        text = "&#%s;" % ref
        self.elementstack[-1][2].append(text)

    def handle_entityref(self, ref):
        # called for each entity reference, e.g. for "&copy;", ref will be "copy"
        # Reconstruct the original entity reference.
        if not self.elementstack: return
        text = "&%s;" % ref
        self.elementstack[-1][2].append(text)

    def handle_data(self, text, escape=1):
        # called for each block of plain text, i.e. outside of any tag and
        # not containing any character or entity references
        if not self.elementstack: return
        if escape and self.contentparams.get('mode') == 'xml':
            text = _xmlescape(text)
        self.elementstack[-1][2].append(text)

    def handle_comment(self, text):
        # called for each comment, e.g. <!-- insert message here -->
        pass

    def handle_pi(self, text):
        # called for each processing instruction, e.g. <?instruction>
        pass

    def handle_decl(self, text):
        pass

    def parse_declaration(self, i):
        # override internal declaration handler to handle CDATA blocks
        if _debug: sys.stderr.write("entering parse_declaration\n")
        if self.rawdata[i:i+9] == '<![CDATA[':
            k = self.rawdata.find(']]>', i)
            if k == -1: k = len(self.rawdata)
            self.handle_data(_xmlescape(self.rawdata[i+9:k]), 0)
            return k+3
        else:
            k = self.rawdata.find('>', i)
            return k+1

    def trackNamespace(self, prefix, uri):
        if (prefix, uri) == (None, 'http://my.netscape.com/rdf/simple/0.9/') and not self.version:
            self.version = 'rss090'
        if (prefix, uri) == (None, 'http://purl.org/rss/1.0/') and not self.version:
            self.version = 'rss10'
        if not prefix: return
        if uri.find('backend.userland.com/rss') <> -1:
            # match any backend.userland.com namespace
            uri = 'http://backend.userland.com/rss'
        if self.namespaces.has_key(uri):
            self.namespacemap[prefix] = self.namespaces[uri]

    def resolveURI(self, uri):
        return urlparse.urljoin(self.baseuri or '', uri)
    
    def decodeEntities(self, element, data):
        if self.contentparams.get('mode') == 'escaped':
            data = data.replace('&lt;', '<')
            data = data.replace('&gt;', '>')
            data = data.replace('&amp;', '&')
            data = data.replace('&quot;', '"')
            data = data.replace('&apos;', "'")
        return data
        
    def push(self, element, expectingText):
        self.elementstack.append([element, expectingText, []])

    def pop(self, element):
        if not self.elementstack: return
        if self.elementstack[-1][0] != element: return

        element, expectingText, pieces = self.elementstack.pop()
        output = "".join(pieces)
        output = output.strip()
        if not expectingText: return output
        
        # decode base64 content
        if self.contentparams.get('mode') == 'base64' and base64:
            try:
                output = base64.decodestring(output)
            except binascii.Error:
                pass
            except binascii.Incomplete:
                pass
                
        # resolve relative URIs
        if (element in self.can_be_relative_uri) and output:
            output = self.resolveURI(output)
        
        # decode entities within embedded markup
        output = self.decodeEntities(element, output)

        # resolve relative URIs within embedded markup
        if element in self.can_contain_relative_uris:
            output = _resolveRelativeURIs(output, self.baseuri, self.encoding)
        
        # sanitize embedded markup
        if element in self.can_contain_dangerous_markup:
            output = _sanitizeHTML(output, self.encoding)

        if type(output) == types.StringType:
            try:
                output = unicode(output, self.encoding)
            except:
                pass
            
        # store output in appropriate place(s)
        if self.inentry:
            if element == 'content':
                self.entries[-1].setdefault(element, [])
                contentparams = copy.deepcopy(self.contentparams)
                contentparams['value'] = output
                self.entries[-1][element].append(contentparams)
            elif element == 'category':
                self.entries[-1][element] = output
                domain = self.entries[-1]['categories'][-1][0]
                self.entries[-1]['categories'][-1] = (domain, output)
            elif element == 'source':
                self.entries[-1]['source']['value'] = output
            elif element == 'link':
                self.entries[-1][element] = output
                if output:
                    self.entries[-1]['links'][-1]['href'] = output
            else:
                self.entries[-1][element] = output
                if self.incontent:
                    if element == 'description':
                        element = 'summary'
                    contentparams = copy.deepcopy(self.contentparams)
                    contentparams['value'] = output
                    self.entries[-1][element + '_detail'] = contentparams
        elif self.infeed and (not self.intextinput) and (not self.inimage):
            self.feeddata[element] = output
            if element == 'category':
                domain = self.feeddata['categories'][-1][0]
                self.feeddata['categories'][-1] = (domain, output)
            elif element == 'link':
                self.feeddata['links'][-1]['href'] = output
            elif self.incontent:
                if element == 'description':
                    element = 'tagline'
                contentparams = copy.deepcopy(self.contentparams)
                contentparams['value'] = output
                self.feeddata[element + '_detail'] = contentparams
        return output

    def _mapToStandardPrefix(self, name):
        colonpos = name.find(':')
        if colonpos <> -1:
            prefix = name[:colonpos]
            suffix = name[colonpos+1:]
            prefix = self.namespacemap.get(prefix, prefix)
            name = prefix + ':' + suffix
        return name
        
    def _getAttribute(self, attrsD, name):
        return attrsD.get(self._mapToStandardPrefix(name))

    def _save(self, key, value):
        if value:
            if self.inentry:
                self.entries[-1].setdefault(key, value)
            elif self.feeddata:
                self.feeddata.setdefault(key, value)

    def _start_rss(self, attrsD):
        versionmap = {'0.91': 'rss091u',
                      '0.92': 'rss092',
                      '0.93': 'rss093',
                      '0.94': 'rss094'}
        if not self.version:
            attr_version = attrsD.get('version', '')
            version = versionmap.get(attr_version)
            if version:
                self.version = version
            elif attr_version.startswith('2.'):
                self.version = 'rss20'
            else:
                self.version = 'rss'
    
    def _start_dlhottitles(self, attrsD):
        self.version = 'hotrss'

    def _start_channel(self, attrsD):
        self.infeed = 1
        self._cdf_common(attrsD)
    _start_feedinfo = _start_channel

    def _cdf_common(self, attrsD):
        if attrsD.has_key('lastmod'):
            if _debug: sys.stderr.write(attrsD['lastmod'] + '\n')
            self._start_modified({})
            self.elementstack[-1][-1] = attrsD['lastmod']
            self._end_modified()
        if attrsD.has_key('href'):
            self._start_link({})
            self.elementstack[-1][-1] = attrsD['href']
            self._end_link()
    
    def _start_feed(self, attrsD):
        self.infeed = 1
        versionmap = {'0.1': 'atom01',
                      '0.2': 'atom02',
                      '0.3': 'atom03'}
        if not self.version:
            attr_version = attrsD.get('version')
            version = versionmap.get(attr_version)
            if version:
                self.version = version
            else:
                self.version = 'atom'

    def _end_channel(self):
        self.infeed = 0
    _end_feed = _end_channel
    
    def _start_image(self, attrsD):
        self.inimage = 1
            
    def _end_image(self):
        self.inimage = 0
                
    def _start_textinput(self, attrsD):
        self.intextinput = 1
        self.push('textinput', 0)
        context = self._getContext()
        context.setdefault('textinput', FeedParserDict())
    _start_textInput = _start_textinput
    
    def _end_textinput(self):
        self.pop('textinput')
        self.intextinput = 0
    _end_textInput = _end_textinput

    def _start_author(self, attrsD):
        self.inauthor = 1
        self.push('author', 1)
    _start_managingeditor = _start_author
    _start_dc_author = _start_author
    _start_dc_creator = _start_author

    def _end_author(self):
        self.pop('author')
        self.inauthor = 0
        self._sync_author_detail()
    _end_managingeditor = _end_author
    _end_dc_author = _end_author
    _end_dc_creator = _end_author

    def _start_contributor(self, attrsD):
        self.incontributor = 1
        context = self._getContext()
        context.setdefault('contributors', [])
        context['contributors'].append(FeedParserDict())
        self.push('contributor', 0)

    def _end_contributor(self):
        self.pop('contributor')
        self.incontributor = 0
        
    def _start_name(self, attrsD):
        self.push('name', 0)

    def _end_name(self):
        value = self.pop('name')
        if self.inauthor:
            self._save_author('name', value)
        elif self.incontributor:
            self._save_contributor('name', value)
        elif self.intextinput:
            context = self._getContext()
            context['textinput']['name'] = value

    def _start_url(self, attrsD):
        self.push('url', 1)
    _start_homepage = _start_url
    _start_uri = _start_url

    def _end_url(self):
        value = self.pop('url')
        if self.inauthor:
            self._save_author('url', value)
        elif self.incontributor:
            self._save_contributor('url', value)
        elif self.inimage:
            # TODO
            pass
        elif self.intextinput:
            # TODO (map to link)
            pass
    _end_homepage = _end_url
    _end_uri = _end_url

    def _start_email(self, attrsD):
        self.push('email', 0)

    def _end_email(self):
        value = self.pop('email')
        if self.inauthor:
            self._save_author('email', value)
        elif self.incontributor:
            self._save_contributor('email', value)
            pass

    def _getContext(self):
        if self.inentry:
            context = self.entries[-1]
        else:
            context = self.feeddata
        return context

    def _save_author(self, key, value):
        context = self._getContext()
        context.setdefault('author_detail', FeedParserDict())
        context['author_detail'][key] = value
        self._sync_author_detail()

    def _save_contributor(self, key, value):
        context = self._getContext()
        context.setdefault('contributors', [FeedParserDict()])
        context['contributors'][-1][key] = value

    def _sync_author_detail(self):
        context = self._getContext()
        detail = context.get('author_detail')
        if detail:
            name = detail.get('name')
            email = detail.get('email')
            if name and email:
                context['author'] = "%s (%s)" % (name, email)
            elif name:
                context['author'] = name
            elif email:
                context['author'] = email
        else:
            author = context.get('author')
            if not author: return
            emailmatch = re.search(r"""(([a-zA-Z0-9\_\-\.\+]+)@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.)|(([a-zA-Z0-9\-]+\.)+))([a-zA-Z]{2,4}|[0-9]{1,3})(\]?))""", author)
            if not emailmatch: return
            email = emailmatch.group(0)
            # probably a better way to do the following, but it passes all the tests
            author = author.replace(email, '')
            author = author.replace('()', '')
            author = author.strip()
            if author and (author[0] == '('):
                author = author[1:]
            if author and (author[-1] == ')'):
                author = author[:-1]
            author = author.strip()
            context.setdefault('author_detail', FeedParserDict())
            context['author_detail']['name'] = author
            context['author_detail']['email'] = email
            
    def _start_tagline(self, attrsD):
        self.incontent += 1
        self.contentparams = FeedParserDict({'mode': attrsD.get('mode', 'escaped'),
                              'type': attrsD.get('type', 'text/plain'),
                              'language': attrsD.get('xml:lang', self.lang),
                              'base': attrsD.get('xml:base', self.baseuri)})
        self.push('tagline', 1)
    _start_subtitle = _start_tagline

    def _end_tagline(self):
        value = self.pop('tagline')
        self.incontent -= 1
        self.contentparams.clear()
        if self.infeed:
            self.feeddata['description'] = value
    _end_subtitle = _end_tagline
            
    def _start_copyright(self, attrsD):
        self.incontent += 1
        self.contentparams = FeedParserDict({'mode': attrsD.get('mode', 'escaped'),
                              'type': attrsD.get('type', 'text/plain'),
                              'language': attrsD.get('xml:lang', self.lang),
                              'base': attrsD.get('xml:base', self.baseuri)})
        self.push('copyright', 1)
    _start_dc_rights = _start_copyright

    def _end_copyright(self):
        self.pop('copyright')
        self.incontent -= 1
        self.contentparams.clear()
    _end_dc_rights = _end_copyright

    def _start_item(self, attrsD):
        self.entries.append(FeedParserDict())
        self.push('item', 0)
        self.inentry = 1
        self._cdf_common(attrsD)
    _start_entry = _start_item
    _start_product = _start_item

    def _end_item(self):
        self.pop('item')
        self.inentry = 0
    _end_entry = _end_item

    def _start_dc_language(self, attrsD):
        self.push('language', 1)
    _start_language = _start_dc_language

    def _end_dc_language(self):
        self.lang = self.pop('language')
    _end_language = _end_dc_language

    def _start_dc_publisher(self, attrsD):
        self.push('publisher', 1)
    _start_webmaster = _start_dc_publisher

    def _end_dc_publisher(self):
        self.pop('publisher')
    _end_webmaster = _end_dc_publisher
        
    def _start_dcterms_issued(self, attrsD):
        self.push('issued', 1)
    _start_issued = _start_dcterms_issued

    def _end_dcterms_issued(self):
        value = self.pop('issued')
        self._save('issued_parsed', _parse_date(value))
    _end_issued = _end_dcterms_issued

    def _start_dcterms_created(self, attrsD):
        self.push('created', 1)
    _start_created = _start_dcterms_created

    def _end_dcterms_created(self):
        value = self.pop('created')
        self._save('created_parsed', _parse_date(value))
    _end_created = _end_dcterms_created

    def _start_dcterms_modified(self, attrsD):
        self.push('modified', 1)
    _start_modified = _start_dcterms_modified
    _start_dc_date = _start_dcterms_modified
    _start_pubdate = _start_dcterms_modified

    def _end_dcterms_modified(self):
        value = self.pop('modified')
        if _debug: sys.stderr.write('_end_dcterms_modified, value=' + value + '\n')
        parsed_value = _parse_date(value)
        self._save('modified_parsed', parsed_value)
    _end_modified = _end_dcterms_modified
    _end_dc_date = _end_dcterms_modified
    _end_pubdate = _end_dcterms_modified

    def _start_expirationdate(self, attrsD):
        self.push('expired', 1)

    def _end_expirationdate(self):
        self._save('expired_parsed', _parse_date(self.pop('expired')))

    def _start_cc_license(self, attrsD):
        self.push('license', 1)
        value = self._getAttribute(attrsD, 'rdf:resource')
        if value:
            self.elementstack[-1][2].append(value)
        self.pop('license')
        
    def _start_creativecommons_license(self, attrsD):
        self.push('license', 1)

    def _end_creativecommons_license(self):
        self.pop('license')

    def _start_category(self, attrsD):
        self.push('category', 1)
        domain = self._getAttribute(attrsD, 'domain')
        cats = []
        if self.inentry:
            cats = self.entries[-1].setdefault('categories', [])
        elif self.infeed:
            cats = self.feeddata.setdefault('categories', [])
        cats.append((domain, None))
    _start_dc_subject = _start_category
    _start_keywords = _start_category
        
    def _end_category(self):
        self.pop('category')
    _end_dc_subject = _end_category
    _end_keywords = _end_category
        
    def _start_cloud(self, attrsD):
        self.feeddata['cloud'] = attrsD
        
    def _start_link(self, attrsD):
        attrsD.setdefault('rel', 'alternate')
        attrsD.setdefault('type', 'text/html')
        if attrsD.has_key('href'):
            attrsD['href'] = self.resolveURI(attrsD['href'])
        expectingText = self.infeed or self.inentry
        if self.inentry:
            self.entries[-1].setdefault('links', [])
            self.entries[-1]['links'].append(FeedParserDict(attrsD))
        elif self.infeed:
            self.feeddata.setdefault('links', [])
            self.feeddata['links'].append(FeedParserDict(attrsD))
        if attrsD.has_key('href'):
            expectingText = 0
            if attrsD.get('type', '') in self.html_types:
                if self.inentry:
                    self.entries[-1]['link'] = attrsD['href']
                elif self.infeed:
                    self.feeddata['link'] = attrsD['href']
        else:
            self.push('link', expectingText)
    _start_producturl = _start_link

    def _end_link(self):
        value = self.pop('link')
        if self.intextinput:
            context = self._getContext()
            context['textinput']['link'] = value
    _end_producturl = _end_link

    def _start_guid(self, attrsD):
        self.guidislink = (attrsD.get('ispermalink', 'true') == 'true')
        self.push('id', 1)

    def _end_guid(self):
        value = self.pop('id')
        if self.guidislink:
            # guid acts as link, but only if "ispermalink" is not present or is "true",
            # and only if the item doesn't already have a link element
            self._save('link', value)

    def _start_id(self, attrsD):
        self.push('id', 1)

    def _end_id(self):
        value = self.pop('id')
            
    def _start_title(self, attrsD):
        self.incontent += 1
        self.contentparams = FeedParserDict({'mode': attrsD.get('mode', 'escaped'),
                              'type': attrsD.get('type', 'text/plain'),
                              'language': attrsD.get('xml:lang', self.lang),
                              'base': attrsD.get('xml:base', self.baseuri)})
        self.push('title', self.infeed or self.inentry)
    _start_dc_title = _start_title

    def _end_title(self):
        value = self.pop('title')
        self.incontent -= 1
        self.contentparams.clear()
        if self.intextinput:
            context = self._getContext()
            context['textinput']['title'] = value
    _end_dc_title = _end_title

    def _start_description(self, attrsD, default_content_type='text/html'):
        self.incontent += 1
        self.contentparams = FeedParserDict({'mode': attrsD.get('mode', 'escaped'),
                              'type': attrsD.get('type', default_content_type),
                              'language': attrsD.get('xml:lang', self.lang),
                              'base': attrsD.get('xml:base', self.baseuri)})
        self.push('description', self.infeed or self.inentry)

    def _start_abstract(self, attrsD):
        return self._start_description(attrsD, 'text/plain')

    def _end_description(self):
        value = self.pop('description')
        self.incontent -= 1
        self.contentparams.clear()
        context = self._getContext()
        if self.intextinput:
            context['textinput']['description'] = value
        elif self.inentry:
            context['summary'] = value
        elif self.infeed:
            context['tagline'] = value
    _end_abstract = _end_description

    def _start_info(self, attrsD):
        self.incontent += 1
        self.contentparams = FeedParserDict({'mode': attrsD.get('mode', 'escaped'),
                              'type': attrsD.get('type', 'text/plain'),
                              'language': attrsD.get('xml:lang', self.lang),
                              'base': attrsD.get('xml:base', self.baseuri)})
        self.push('info', 1)

    def _end_info(self):
        self.pop('info')
        self.incontent -= 1
        self.contentparams.clear()

    def _start_generator(self, attrsD):
        if attrsD:
            if attrsD.has_key('url'):
                attrsD['url'] = self.resolveURI(attrsD['url'])
            self.feeddata['generator_detail'] = attrsD
        self.push('generator', 1)

    def _end_generator(self):
        value = self.pop('generator')
        if self.feeddata.has_key('generator_detail'):
            self.feeddata['generator_detail']['name'] = value
            
    def _start_admin_generatoragent(self, attrsD):
        self.push('generator', 1)
        value = self._getAttribute(attrsD, 'rdf:resource')
        if value:
            self.elementstack[-1][2].append(value)
        self.pop('generator')

    def _start_admin_errorreportsto(self, attrsD):
        self.push('errorreportsto', 1)
        value = self._getAttribute(attrsD, 'rdf:resource')
        if value:
            self.elementstack[-1][2].append(value)
        self.pop('errorreportsto')
        
    def _start_summary(self, attrsD):
        self.incontent += 1
        self.contentparams = FeedParserDict({'mode': attrsD.get('mode', 'escaped'),
                              'type': attrsD.get('type', 'text/plain'),
                              'language': attrsD.get('xml:lang', self.lang),
                              'base': attrsD.get('xml:base', self.baseuri)})
        self.push('summary', 1)

    def _end_summary(self):
        value = self.pop('summary')
        if self.entries:
            self.entries[-1]['description'] = value
        self.incontent -= 1
        self.contentparams.clear()
        
    def _start_enclosure(self, attrsD):
        if self.inentry:
            self.entries[-1].setdefault('enclosures', [])
            self.entries[-1]['enclosures'].append(FeedParserDict(attrsD))
            
    def _start_source(self, attrsD):
        if self.inentry:
            self.entries[-1]['source'] = attrsD
        self.push('source', 1)

    def _end_source(self):
        self.pop('source')

    def _start_content(self, attrsD):
        self.incontent += 1
        self.contentparams = FeedParserDict({'mode': attrsD.get('mode', 'xml'),
                              'type': attrsD.get('type', 'text/plain'),
                              'language': attrsD.get('xml:lang', self.lang),
                              'base': attrsD.get('xml:base', self.baseuri)})
        self.push('content', 1)

    def _start_prodlink(self, attrsD):
        self.incontent += 1
        self.contentparams = FeedParserDict({'mode': attrsD.get('mode', 'xml'),
                              'type': attrsD.get('type', 'text/html'),
                              'language': attrsD.get('xml:lang', self.lang),
                              'base': attrsD.get('xml:base', self.baseuri)})
        self.push('content', 1)

    def _start_body(self, attrsD):
        self.incontent += 1
        self.contentparams = FeedParserDict({'mode': 'xml',
                              'type': 'application/xhtml+xml',
                              'language': attrsD.get('xml:lang', self.lang),
                              'base': attrsD.get('xml:base', self.baseuri)})
        self.push('content', 1)
    _start_xhtml_body = _start_body

    def _start_content_encoded(self, attrsD):
        self.incontent += 1
        self.contentparams = FeedParserDict({'mode': 'escaped',
                              'type': 'text/html',
                              'language': attrsD.get('xml:lang', self.lang),
                              'base': attrsD.get('xml:base', self.baseuri)})
        self.push('content', 1)
    _start_fullitem = _start_content_encoded

    def _end_content(self):
        value = self.pop('content')
        if self.contentparams.get('type') in (['text/plain'] + self.html_types):
            self._save('description', value)
        self.incontent -= 1
        self.contentparams.clear()
    _end_body = _end_content
    _end_xhtml_body = _end_content
    _end_content_encoded = _end_content
    _end_fullitem = _end_content
    _end_prodlink = _end_content

if _XML_AVAILABLE:
    class _StrictFeedParser(_FeedParserMixin, xml.sax.handler.ContentHandler, xml.sax.handler.EntityResolver):#, xml.sax.handler.DTDHandler):
        def __init__(self, baseuri, encoding):
            if _debug: sys.stderr.write('trying StrictFeedParser\n')
            xml.sax.handler.ContentHandler.__init__(self)
            _FeedParserMixin.__init__(self, baseuri, encoding)
            self.bozo = 0
            self.exc = None
        
        def startPrefixMapping(self, prefix, uri):
            self.trackNamespace(prefix, uri)
        
        def startElementNS(self, name, qname, attrs):
            namespace, localname = name
            namespace = str(namespace or '')
            if namespace.find('backend.userland.com/rss') <> -1:
                # match any backend.userland.com namespace
                namespace = 'http://backend.userland.com/rss'
            prefix = self.namespaces.get(namespace, 'unknown')
            if prefix:
                localname = prefix + ':' + localname
            localname = str(localname).lower()

            # qname implementation is horribly broken in Python 2.1 (it
            # doesn't report any), and slightly broken in Python 2.2 (it
            # doesn't report the xml: namespace). So we match up namespaces
            # with a known list first, and then possibly override them with
            # the qnames the SAX parser gives us (if indeed it gives us any
            # at all).  Thanks to MatejC for helping me test this and
            # tirelessly telling me that it didn't work yet.
            attrsD = {}
            for (namespace, attrlocalname), attrvalue in attrs._attrs.items():
                prefix = self.namespaces.get(namespace, '')
                if prefix:
                    attrlocalname = prefix + ":" + attrlocalname
                attrsD[str(attrlocalname).lower()] = attrvalue
            for qname in attrs.getQNames():
                attrsD[str(qname).lower()] = attrs.getValueByQName(qname)
            self.unknown_starttag(localname, attrsD.items())

        def resolveEntity(self, publicId, systemId):
            return _StringIO()

        def characters(self, text):
            self.handle_data(text)

        def endElementNS(self, name, qname):
            namespace, localname = name
            namespace = str(namespace)
            prefix = self.namespaces.get(namespace, '')
            if prefix:
                localname = prefix + ':' + localname
            localname = str(localname).lower()
            self.unknown_endtag(localname)

        def error(self, exc):
            self.bozo = 1
            self.exc = exc
            
        def fatalError(self, exc):
            self.error(exc)
            raise exc

class _BaseHTMLProcessor(sgmllib.SGMLParser):
    elements_no_end_tag = ['area', 'base', 'basefont', 'br', 'col', 'frame', 'hr',
      'img', 'input', 'isindex', 'link', 'meta', 'param']
    
    def __init__(self, encoding):
        self.encoding = encoding
        sgmllib.SGMLParser.__init__(self)
        
    def reset(self):
        self.pieces = []
        sgmllib.SGMLParser.reset(self)

    def feed(self, data):
        data = re.compile(r'<!((?!DOCTYPE|--|\[))', re.IGNORECASE).sub(r'&lt;!\1', data)
        data = re.sub(r'<(\S+)/>', r'<\1></\1>', data)
        data = data.replace('&#39;', "'")
        data = data.replace('&#34;', '"')
        if type(data) == types.UnicodeType:
            data = data.encode(self.encoding)
        sgmllib.SGMLParser.feed(self, data)

    def normalize_attrs(self, attrs):
        # utility method to be called by descendants
        attrs = [(k.lower(), sgmllib.charref.sub(lambda m: unichr(int(m.groups()[0])), v).strip()) for k, v in attrs]
        # The previous line may have output a Unicode string.  If so we need to convert it back
        # to raw bytes in the current encoding.  If not, then just pass.
        try:
            attrs = [(k, v.encode(self.encoding)) for k, v in attrs]
        except:
            pass
        attrs = [(k, k in ('rel', 'type') and v.lower() or v) for k, v in attrs]
        return attrs

    def unknown_starttag(self, tag, attrs):
        # called for each start tag
        # attrs is a list of (attr, value) tuples
        # e.g. for <pre class="screen">, tag="pre", attrs=[("class", "screen")]
        if _debug: sys.stderr.write('_BaseHTMLProcessor, unknown_starttag, tag=%s\n' % tag)
        strattrs = "".join([' %s="%s"' % (key, value) for key, value in attrs])
        if tag in self.elements_no_end_tag:
            self.pieces.append("<%(tag)s%(strattrs)s />" % locals())
        else:
            self.pieces.append("<%(tag)s%(strattrs)s>" % locals())
        
    def unknown_endtag(self, tag):
        # called for each end tag, e.g. for </pre>, tag will be "pre"
        # Reconstruct the original end tag.
        if tag not in self.elements_no_end_tag:
            self.pieces.append("</%(tag)s>" % locals())

    def handle_charref(self, ref):
        # called for each character reference, e.g. for "&#160;", ref will be "160"
        # Reconstruct the original character reference.
        self.pieces.append("&#%(ref)s;" % locals())
        
    def handle_entityref(self, ref):
        # called for each entity reference, e.g. for "&copy;", ref will be "copy"
        # Reconstruct the original entity reference.
        self.pieces.append("&%(ref)s;" % locals())

    def handle_data(self, text):
        # called for each block of plain text, i.e. outside of any tag and
        # not containing any character or entity references
        # Store the original text verbatim.
        if _debug: sys.stderr.write('_BaseHTMLProcessor, handle_text, text=%s\n' % text)
        self.pieces.append(text)
        
    def handle_comment(self, text):
        # called for each HTML comment, e.g. <!-- insert Javascript code here -->
        # Reconstruct the original comment.
        self.pieces.append("<!--%(text)s-->" % locals())
        
    def handle_pi(self, text):
        # called for each processing instruction, e.g. <?instruction>
        # Reconstruct original processing instruction.
        self.pieces.append("<?%(text)s>" % locals())

    def handle_decl(self, text):
        # called for the DOCTYPE, if present, e.g.
        # <!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
        #     "http://www.w3.org/TR/html4/loose.dtd">
        # Reconstruct original DOCTYPE
        self.pieces.append("<!%(text)s>" % locals())
        
    _new_declname_match = re.compile(r'[a-zA-Z][-_.a-zA-Z0-9:]*\s*').match
    def _scan_name(self, i, declstartpos):
        rawdata = self.rawdata
        if _debug: sys.stderr.write("i=%s, declstartpos=%s, rawdata=%s\n" % (i, declstartpos, rawdata))
        n = len(rawdata)
        if i == n:
            return None, -1
        m = self._new_declname_match(rawdata, i)
        if m:
            s = m.group()
            name = s.strip()
            if (i + len(s)) == n:
                return None, -1  # end of buffer
            return name.lower(), m.end()
        else:
            self.handle_data(rawdata)
#            self.updatepos(declstartpos, i)
            return None, -1

    def output(self):
        """Return processed HTML as a single string"""
        if _debug:
            for p in self.pieces:
                sys.stderr.write(p)
            sys.stderr.write('\n')
        return "".join([str(p) for p in self.pieces])

class _LooseFeedParser(_FeedParserMixin, _BaseHTMLProcessor):
    def __init__(self, baseuri, encoding):
        sgmllib.SGMLParser.__init__(self)
        _FeedParserMixin.__init__(self, baseuri, encoding)

class _RelativeURIResolver(_BaseHTMLProcessor):
    relative_uris = [('a', 'href'),
                     ('applet', 'codebase'),
                     ('area', 'href'),
                     ('blockquote', 'cite'),
                     ('body', 'background'),
                     ('del', 'cite'),
                     ('form', 'action'),
                     ('frame', 'longdesc'),
                     ('frame', 'src'),
                     ('iframe', 'longdesc'),
                     ('iframe', 'src'),
                     ('head', 'profile'),
                     ('img', 'longdesc'),
                     ('img', 'src'),
                     ('img', 'usemap'),
                     ('input', 'src'),
                     ('input', 'usemap'),
                     ('ins', 'cite'),
                     ('link', 'href'),
                     ('object', 'classid'),
                     ('object', 'codebase'),
                     ('object', 'data'),
                     ('object', 'usemap'),
                     ('q', 'cite'),
                     ('script', 'src')]

    def __init__(self, baseuri, encoding):
        _BaseHTMLProcessor.__init__(self, encoding)
        self.baseuri = baseuri

    def resolveURI(self, uri):
        return urlparse.urljoin(self.baseuri, uri)
    
    def unknown_starttag(self, tag, attrs):
        attrs = self.normalize_attrs(attrs)
        attrs = [(key, ((tag, key) in self.relative_uris) and self.resolveURI(value) or value) for key, value in attrs]
        _BaseHTMLProcessor.unknown_starttag(self, tag, attrs)
        
def _resolveRelativeURIs(htmlSource, baseURI, encoding):
    if _debug: sys.stderr.write("entering _resolveRelativeURIs\n")
    p = _RelativeURIResolver(baseURI, encoding)
    if _debug: sys.stderr.write(repr(type(htmlSource)) + '\n')
    p.feed(htmlSource)
    return p.output()

class _HTMLSanitizer(_BaseHTMLProcessor):
    acceptable_elements = ['a', 'abbr', 'acronym', 'address', 'area', 'b', 'big',
      'blockquote', 'br', 'button', 'caption', 'center', 'cite', 'code', 'col',
      'colgroup', 'dd', 'del', 'dfn', 'dir', 'div', 'dl', 'dt', 'em', 'fieldset',
      'font', 'form', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6', 'hr', 'i', 'img', 'input',
      'ins', 'kbd', 'label', 'legend', 'li', 'map', 'menu', 'ol', 'optgroup',
      'option', 'p', 'pre', 'q', 's', 'samp', 'select', 'small', 'span', 'strike',
      'strong', 'sub', 'sup', 'table', 'tbody', 'td', 'textarea', 'tfoot', 'th',
      'thead', 'tr', 'tt', 'u', 'ul', 'var']

    acceptable_attributes = ['abbr', 'accept', 'accept-charset', 'accesskey',
      'action', 'align', 'alt', 'axis', 'border', 'cellpadding', 'cellspacing',
      'char', 'charoff', 'charset', 'checked', 'cite', 'class', 'clear', 'cols',
      'colspan', 'color', 'compact', 'coords', 'datetime', 'dir', 'disabled',
      'enctype', 'for', 'frame', 'headers', 'height', 'href', 'hreflang', 'hspace',
      'id', 'ismap', 'label', 'lang', 'longdesc', 'maxlength', 'media', 'method',
      'multiple', 'name', 'nohref', 'noshade', 'nowrap', 'prompt', 'readonly',
      'rel', 'rev', 'rows', 'rowspan', 'rules', 'scope', 'selected', 'shape', 'size',
      'span', 'src', 'start', 'summary', 'tabindex', 'target', 'title', 'type',
      'usemap', 'valign', 'value', 'vspace', 'width']

    unacceptable_elements_with_end_tag = ['script', 'applet']

    def reset(self):
        _BaseHTMLProcessor.reset(self)
        self.unacceptablestack = 0
        
    def unknown_starttag(self, tag, attrs):
        if not tag in self.acceptable_elements:
            if tag in self.unacceptable_elements_with_end_tag:
                self.unacceptablestack += 1
            return
        attrs = self.normalize_attrs(attrs)
        attrs = [(key, value) for key, value in attrs if key in self.acceptable_attributes]
        _BaseHTMLProcessor.unknown_starttag(self, tag, attrs)
        
    def unknown_endtag(self, tag):
        if not tag in self.acceptable_elements:
            if tag in self.unacceptable_elements_with_end_tag:
                self.unacceptablestack -= 1
            return
        _BaseHTMLProcessor.unknown_endtag(self, tag)

    def handle_pi(self, text):
        pass

    def handle_decl(self, text):
        pass

    def handle_data(self, text):
        if not self.unacceptablestack:
            _BaseHTMLProcessor.handle_data(self, text)

def _sanitizeHTML(htmlSource, encoding):
    p = _HTMLSanitizer(encoding)
    p.feed(htmlSource)
    data = p.output()
    if _mxtidy and TIDY_MARKUP:
        nerrors, nwarnings, data, errordata = _mxtidy.tidy(data, output_xhtml=1, numeric_entities=1, wrap=0)
        if data.count('<body'):
            data = data.split('<body', 1)[1]
            if data.count('>'):
                data = data.split('>', 1)[1]
        if data.count('</body'):
            data = data.split('</body', 1)[0]
    data = data.strip().replace('\r\n', '\n')
    return data

class _FeedURLHandler(urllib2.HTTPRedirectHandler, urllib2.HTTPDefaultErrorHandler):
    def http_error_default(self, req, fp, code, msg, headers):
        if ((code / 100) == 3) and (code != 304):
            return self.http_error_302(req, fp, code, msg, headers)
        from urllib import addinfourl
        infourl = addinfourl(fp, headers, req.get_full_url())
        infourl.status = code
        return infourl

    def http_error_302(self, req, fp, code, msg, headers):
        infourl = urllib2.HTTPRedirectHandler.http_error_302(self, req, fp, code, msg, headers)
        infourl.status = code
        return infourl

    def http_error_301(self, req, fp, code, msg, headers):
        infourl = urllib2.HTTPRedirectHandler.http_error_301(self, req, fp, code, msg, headers)
        infourl.status = code
        return infourl

    http_error_300 = http_error_302
    http_error_307 = http_error_302
        
def _open_resource(url_file_stream_or_string, etag=None, modified=None, agent=None, referrer=None):
    """URL, filename, or string --> stream

    This function lets you define parsers that take any input source
    (URL, pathname to local or network file, or actual data as a string)
    and deal with it in a uniform manner.  Returned object is guaranteed
    to have all the basic stdio read methods (read, readline, readlines).
    Just .close() the object when you're done with it.

    If the etag argument is supplied, it will be used as the value of an
    If-None-Match request header.

    If the modified argument is supplied, it must be a tuple of 9 integers
    as returned by gmtime() in the standard Python time module. This MUST
    be in GMT (Greenwich Mean Time). The formatted date/time will be used
    as the value of an If-Modified-Since request header.

    If the agent argument is supplied, it will be used as the value of a
    User-Agent request header.

    If the referrer argument is supplied, it will be used as the value of a
    Referer[sic] request header.
    """

    if hasattr(url_file_stream_or_string, "read"):
        return url_file_stream_or_string

    if url_file_stream_or_string == "-":
        return sys.stdin

    if urlparse.urlparse(url_file_stream_or_string)[0] in ('http', 'https', 'ftp'):
        if not agent:
            agent = USER_AGENT
        # try to open with urllib2 (to use optional headers)
        request = urllib2.Request(url_file_stream_or_string)
        request.add_header("User-Agent", agent)
        if etag:
            request.add_header("If-None-Match", etag)
        if modified:
            # format into an RFC 1123-compliant timestamp. We can't use
            # time.strftime() since the %a and %b directives can be affected
            # by the current locale, but RFC 2616 states that dates must be
            # in English.
            short_weekdays = ["Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"]
            months = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]
            request.add_header("If-Modified-Since", "%s, %02d %s %04d %02d:%02d:%02d GMT" % (short_weekdays[modified[6]], modified[2], months[modified[1] - 1], modified[0], modified[3], modified[4], modified[5]))
        if referrer:
            request.add_header("Referer", referrer)
        if gzip:
            request.add_header("Accept-encoding", "gzip")
        opener = urllib2.build_opener(_FeedURLHandler())
        opener.addheaders = [] # RMK - must clear so we only send our custom User-Agent
        try:
            try:
                return opener.open(request)
            except:
                return _StringIO('')
        finally:
            opener.close() # JohnD
    
    # try to open with native open function (if url_file_stream_or_string is a filename)
    try:
        return open(url_file_stream_or_string)
    except:
        pass

    # treat url_file_stream_or_string as string
    return _StringIO(str(url_file_stream_or_string))

# W3DTF-style date parsing adapted from PyXML xml.utils.iso8601, written by
# Drake and licensed under the Python license.  Removed all range checking
# for month, day, hour, minute, and second, since mktime will normalize
# these later
def _w3dtf_parse(s):
    def __extract_date(m):
        year = int(m.group("year"))
        if year < 100:
            year = 100 * int(time.gmtime()[0] / 100) + int(year)
        if year < 1000:
            return 0, 0, 0
        julian = m.group("julian")
        if julian:
            julian = int(julian)
            month = julian / 30 + 1
            day = julian % 30 + 1
            jday = None
            while jday != julian:
                t = time.mktime((year, month, day, 0, 0, 0, 0, 0, 0))
                jday = time.gmtime(t)[-2]
                diff = abs(jday - julian)
                if jday > julian:
                    if diff < day:
                        day = day - diff
                    else:
                        month = month - 1
                        day = 31
                elif jday < julian:
                    if day + diff < 28:
                       day = day + diff
                    else:
                        month = month + 1
            return year, month, day
        month = m.group("month")
        day = 1
        if month is None:
            month = 1
        else:
            month = int(month)
            day = m.group("day")
            if day:
                day = int(day)
            else:
                day = 1
        return year, month, day

    def __extract_time(m):
        if not m:
            return 0, 0, 0
        hours = m.group("hours")
        if not hours:
            return 0, 0, 0
        hours = int(hours)
        minutes = int(m.group("minutes"))
        seconds = m.group("seconds")
        if seconds:
            seconds = int(seconds)
        else:
            seconds = 0
        return hours, minutes, seconds

    def __extract_tzd(m):
        """Return the Time Zone Designator as an offset in seconds from UTC."""
        if not m:
            return 0
        tzd = m.group("tzd")
        if not tzd:
            return 0
        if tzd == "Z":
            return 0
        hours = int(m.group("tzdhours"))
        minutes = m.group("tzdminutes")
        if minutes:
            minutes = int(minutes)
        else:
            minutes = 0
        offset = (hours*60 + minutes) * 60
        if tzd[0] == "+":
            return -offset
        return offset

    __date_re = ("(?P<year>\d\d\d\d)"
                 "(?:(?P<dsep>-|)"
                 "(?:(?P<julian>\d\d\d)"
                 "|(?P<month>\d\d)(?:(?P=dsep)(?P<day>\d\d))?))?")
    __tzd_re = "(?P<tzd>[-+](?P<tzdhours>\d\d)(?::?(?P<tzdminutes>\d\d))|Z)"
    __tzd_rx = re.compile(__tzd_re)
    __time_re = ("(?P<hours>\d\d)(?P<tsep>:|)(?P<minutes>\d\d)"
                 "(?:(?P=tsep)(?P<seconds>\d\d(?:[.,]\d+)?))?"
                 + __tzd_re)
    __datetime_re = "%s(?:T%s)?" % (__date_re, __time_re)
    __datetime_rx = re.compile(__datetime_re)
    m = __datetime_rx.match(s)
    if m is None or m.group() != s:
        return None
    gmt = __extract_date(m) + __extract_time(m) + (0, 0, 0)
    if gmt[0] == 0: return
    return time.mktime(gmt) + __extract_tzd(m) - time.timezone

# Additional ISO-8601 date parsing routines written by Fazal Majid
# The ISO 8601 standard is very convoluted and irregular - a full ISO 8601
# parser is beyond the scope of feedparser and would be a worthwhile addition
# to the Python library
# A single regular expression cannot parse ISO 8601 date formats into groups
# as the standard is highly irregular (for instance is 030104 2003-01-04 or
# 0301-04-01), so we use templates instead
# Please note the order in templates is significant because we need a
# greedy match
_iso8601_tmpl = ['YYYY-?MM-?DD', 'YYYY-MM', 'YYYY-?OOO',
                'YY-?MM-?DD', 'YY-?OOO', 'YYYY', 
                '-YY-?MM', '-OOO', '-YY',
                '--MM-?DD', '--MM',
                '---DD',
                'CC', '']
_iso8601_re = [
    tmpl.replace(
    'YYYY', r'(?P<year>\d{4})').replace(
    'YY', r'(?P<year>\d\d)').replace(
    'MM', r'(?P<month>[01]\d)').replace(
    'DD', r'(?P<day>[0123]\d)').replace(
    'OOO', r'(?P<ordinal>[0123]\d\d)').replace(
    'CC', r'(?P<century>\d\d$)')
    + r'(T?(?P<hour>\d{2}):(?P<minute>\d{2})'
    + r'(:(?P<second>\d{2}))?'
    + r'(?P<tz>[+-](?P<tzhour>\d{2})(:(?P<tzmin>\d{2}))?|Z)?)?'
    for tmpl in _iso8601_tmpl]
del tmpl

_iso8601_matches = [re.compile(regex).match for regex in _iso8601_re]
del regex

# rfc822.py defines several time zones, but we define some extra ones.
# "ET" is equivalent to "EST", etc.
_additional_timezones = {'AT': -400, 'ET': -500, 'CT': -600, 'MT': -700, 'PT': -800}
rfc822._timezones.update(_additional_timezones)

def _parse_date(date):
    """Parses a variety of date formats into a tuple of 9 integers"""
    date = str(date)
    try:
        # try the standard rfc822 library, which handles
        # RFC822, RFC1123, RFC2822, and asctime
        tm = rfc822.parsedate_tz(date)
        if tm:
            return time.gmtime(rfc822.mktime_tz(tm))
        # not a RFC2822 date, try W3DTF profile of ISO-8601
        try:
            tm = _w3dtf_parse(date)
        except ValueError:
            tm = None
        if tm:
            return time.gmtime(tm)
        # try various non-W3DTF ISO-8601-compatible formats like 20040105
        m = None
        for _iso8601_match in _iso8601_matches:
            m = _iso8601_match(date)
            if m: break
        if not m: return
        # catch truly malformed strings
        if m.span() == (0, 0): return
        params = m.groupdict()
        ordinal = params.get("ordinal", 0)
        if ordinal:
            ordinal = int(ordinal)
        else:
            ordinal = 0
        year = params.get("year", "--")
        if not year or year == "--":
            year = time.gmtime()[0]
        elif len(year) == 2:
            # ISO 8601 assumes current century, i.e. 93 -> 2093, NOT 1993
            year = 100 * int(time.gmtime()[0] / 100) + int(year)
        else:
            year = int(year)
        month = params.get("month", "-")
        if not month or month == "-":
            # ordinals are NOT normalized by mktime, we simulate them
            # by setting month=1, day=ordinal
            if ordinal:
                month = 1
            else:
                month = time.gmtime()[1]
        month = int(month)
        day = params.get("day", 0)
        if not day:
            # see above
            if ordinal:
                day = ordinal
            elif params.get("century", 0) or \
                     params.get("year", 0) or params.get("month", 0):
                day = 1
            else:
                day = time.gmtime()[2]
        else:
            day = int(day)
        # special case of the century - is the first year of the 21st century
        # 2000 or 2001 ? The debate goes on...
        if "century" in params.keys():
            year = (int(params["century"]) - 1) * 100 + 1
        # in ISO 8601 most fields are optional
        for field in ["hour", "minute", "second", "tzhour", "tzmin"]:
            if not params.get(field, None):
                params[field] = 0
        hour = int(params.get("hour", 0))
        minute = int(params.get("minute", 0))
        second = int(params.get("second", 0))
        # weekday is normalized by mktime(), we can ignore it
        weekday = 0
        # daylight savings is complex, but not needed for feedparser's purposes
        # as time zones, if specified, include mention of whether it is active
        # (e.g. PST vs. PDT, CET). Using -1 is implementation-dependent and
        # and most implementations have DST bugs
        daylight_savings_flag = 0
        tm = [year, month, day, hour, minute, second, weekday,
              ordinal, daylight_savings_flag]
        # ISO 8601 time zone adjustments
        tz = params.get("tz")
        if tz and tz != "Z":
            if tz[0] == "-":
                tm[3] += int(params.get("tzhour", 0))
                tm[4] += int(params.get("tzmin", 0))
            elif tz[0] == "+":
                tm[3] -= int(params.get("tzhour", 0))
                tm[4] -= int(params.get("tzmin", 0))
            else:
                return None
        # Python's time.mktime() is a wrapper around the ANSI C mktime(3c)
        # which is guaranteed to normalize d/m/y/h/m/s
        # many implementations have bugs, but we'll pretend they don't
        return time.localtime(time.mktime(tm))
    except:
        return None

def _getCharacterEncoding(http_headers, xml_data):
    """Get the character encoding of the XML document

    http_headers is a dictionary
    xml_data is a raw string (not Unicode)
    
    This is so much trickier than it sounds,
    it's not even funny.  According to RFC 3023 ("XML Media Types"), if
    the HTTP Content-Type is application/xml, application/*+xml,
    application/xml-external-parsed-entity, or application/xml-dtd,
    the encoding given in the charset parameter of the HTTP Content-Type
    takes precedence over the encoding given in the XML prefix within the
    document, and defaults to "utf-8" if neither are specified.  But, if
    the HTTP Content-Type is text/xml, text/*+xml, or
    text/xml-external-parsed-entity, the encoding given in the XML prefix
    within the document is ALWAYS IGNORED and only the encoding given in
    the charset parameter of the HTTP Content-Type header should be
    respected, and it defaults to "us-ascii" if not specified.  If
    Content-Type is unspecified (input was local file or non-HTTP source)
    or unrecognized (server just got it totally wrong), then go by the
    encoding given in the XML prefix of the document and default to
    "utf-8" as per the XML specification.
    """

    def _parseHTTPContentType(content_type):
        """takes HTTP Content-Type header and returns (content type, charset)

        If no charset is specified, returns (content type, '')
        If no content type is specified, returns ('', '')
        Both return parameters are guaranteed to be lowercase strings
        """
        if not content_type:
            return '', ''
        content_type = content_type.strip()
        paramstr = content_type.split(';')[1:]
        if not paramstr:
            return content_type, ''
        content_type = content_type.split(';', 1)[0].strip().lower()
        if not paramstr[0]:
            # declaration like "text/xml;" (note ending semicolon)
            # dunno if this is malformed but it sure was hard to track down
            return content_type, ''
        import string
        params = dict([map(string.lower, map(string.strip, p.strip().split('=', 1))) for p in paramstr])
        charset = params.get('charset')
        if not charset:
            return content_type, ''
        if charset[0] in ('"', "'"):
            charset = charset[1:]
        if charset and charset[-1] in ('"', "'"):
            charset = charset[:-1]
        charset = charset.strip()
        return content_type, charset

    true_encoding = None
    http_content_type, http_encoding = _parseHTTPContentType(http_headers.get("content-type"))
    xml_encoding_match = re.compile('<\?.*encoding=[\'"](.*?)[\'"].*\?>').match(xml_data)
    xml_encoding = xml_encoding_match and xml_encoding_match.groups()[0].lower() or ''
    if (http_content_type == 'application/xml') or \
       (http_content_type == 'application/xml-dtd') or \
       (http_content_type == 'application/xml-external-parsed-entity') or \
       (http_content_type.startswith('application/') and http_content_type.endswith('+xml')):
        if http_encoding:
            true_encoding = http_encoding
        elif xml_encoding:
            true_encoding = xml_encoding
        else:
            true_encoding = 'utf-8'
    elif (http_content_type == 'text/xml') or \
         (http_content_type == 'text/xml-external-parsed-entity') or \
         (http_content_type.startswith('text/') and http_content_type.endswith('+xml')):
        if http_encoding:
            true_encoding = http_encoding
        else:
            true_encoding = 'us-ascii'
    else:
        true_encoding = xml_encoding or 'utf-8'
    return true_encoding, http_encoding, xml_encoding
    
def _changeEncodingDeclaration(data, encoding):
    """Changes an XML data stream on the fly to specify a new encoding

    data is a raw sequence of bytes (not Unicode) that is presumed to be in %encoding already
    encoding is a string recognized by encodings.aliases
    """
    if _debug: sys.stderr.write('entering _changeEncodingDeclaration\n')
    if _debug: sys.stderr.write('proposed encoding: %s\n' % encoding)
    #import cjkcodecs.aliases
    #import japanese
    data = unicode(data, encoding)
    declmatch = re.compile(u'^<\?xml[^>]*?>')
    newdecl = unicode("""<?xml version='1.0' encoding='%s'?>""" % encoding, encoding)
    if declmatch.search(data):
        data = declmatch.sub(newdecl, data)
    else:
        data = newdecl + u'\n' + data
    return data.encode(encoding)

def _stripDoctype(data):
    """Strips DOCTYPE from XML document, returns (rss_version, stripped_data)

    rss_version may be "rss091n" or None
    stripped_data is the same XML document, minus the DOCTYPE
    """
    entity_pattern = re.compile(r'<!ENTITY([^>]*?)>', re.MULTILINE)
    data = entity_pattern.sub('', data)
    doctype_pattern = re.compile(r'<!DOCTYPE([^>]*?)>', re.MULTILINE)
    doctype_results = doctype_pattern.findall(data)
    doctype = doctype_results and doctype_results[0] or ''
    if doctype.lower().count('netscape'):
        version = 'rss091n'
    else:
        version = None
    data = doctype_pattern.sub('', data)
    return version, data
    
def parse(url_file_stream_or_string, etag=None, modified=None, agent=None, referrer=None):
    """Parse a feed from a URL, file, stream, or string"""
    result = FeedParserDict()
    f = _open_resource(url_file_stream_or_string, etag=etag, modified=modified, agent=agent, referrer=referrer)
    data = f.read()
    if hasattr(f, "headers"):
        if gzip and f.headers.get('content-encoding', '') == 'gzip':
            try:
                data = gzip.GzipFile(fileobj=_StringIO(data)).read()
            except:
                # some feeds claim to be gzipped but they're not, so we get garbage
                data = ''
    if hasattr(f, "info"):
        info = f.info()
        result["etag"] = info.getheader("ETag")
        last_modified = info.getheader("Last-Modified")
        if last_modified:
            result["modified"] = _parse_date(last_modified)
    if hasattr(f, "url"):
        result["url"] = f.url
        result["status"] = 200 # default, may be overridden later
    if hasattr(f, "status"):
        result["status"] = f.status
    if hasattr(f, "headers"):
        result["headers"] = f.headers.dict
    f.close()
    if result.get("status", 0) == 304:
        result['feed'] = FeedParserDict()
        result['entries'] = []
        result['debug_message'] = "The feed has not changed since you last checked, so the server sent no data.  This is a feature, not a bug!"
        return result
    result['encoding'], http_encoding, xml_encoding = _getCharacterEncoding(result.get("headers", {}), data)
    result['version'], data = _stripDoctype(data)
    baseuri = result.get('headers', {}).get('content-location', result.get('url'))
    # try true XML parser first
    if not _XML_AVAILABLE:
        if _debug: sys.stderr.write('no xml libraries available\n')
    use_strict_parser = _XML_AVAILABLE
    if use_strict_parser:
        if _debug: sys.stderr.write('using xml library\n')
        result['bozo'] = 0
        feedparser = _StrictFeedParser(baseuri, result['encoding'])
        if _debug and _debug_never_use_libxml2:
            sys.stderr.write('not using libxml2 (even if available)\n')
            additional_parsers = []
        else:
            additional_parsers = ["drv_libxml2"]
        saxparser = xml.sax.make_parser(additional_parsers)
        saxparser.setFeature(xml.sax.handler.feature_namespaces, 1)
        saxparser.setContentHandler(feedparser)
        saxparser.setErrorHandler(feedparser)
        try:
            saxparser.setDTDHandler(feedparser)
        except xml.sax.SAXNotSupportedException:
            # libxml2 driver does not support DTDHandler
            if _debug: sys.stderr.write('using an xml library that does not support DTDHandler (not a big deal)\n')
        try:
            saxparser.setEntityResolver(feedparser)
        except xml.sax.SAXNotSupportedException:
            # libxml2 driver does not support EntityResolver
            if _debug: sys.stderr.write('using an xml library that does not support EntityResolver (not a big deal)\n')
        encoding_set = (result['encoding'] == xml_encoding)
        if not encoding_set:
            bozo_exception = None
            proposed_encodings = [result['encoding'], xml_encoding, 'utf-8', 'iso-8859-1', 'windows-1252']
            tried_encodings = []
            for proposed_encoding in proposed_encodings:
                if proposed_encoding in tried_encodings: continue
                tried_encodings.append(proposed_encoding)
                try:
                    data = _changeEncodingDeclaration(data, proposed_encoding)
                except Exception, bozo_exception:
                    if _debug: sys.stderr.write('character encoding is wrong\n')
                else:
                    if proposed_encoding != result['encoding']:
                        try:
                            raise CharacterEncodingOverride, "document declared as %s, but parsed as %s" % (result['encoding'], proposed_encoding)
                        except CharacterEncodingOverride, bozo_exception:
                            result['bozo'] = 1
                            result['bozo_exception'] = bozo_exception
                    result['encoding'] = proposed_encoding
                    encoding_set = 1
                    break
        if not encoding_set:
            result['bozo'] = 1
            result['bozo_exception'] = bozo_exception
            use_strict_parser = 0
    if use_strict_parser:
        source = xml.sax.xmlreader.InputSource()
        source.setByteStream(_StringIO(data))
        if hasattr(saxparser, '_ns_stack'):
            # work around bug in built-in SAX parser (doesn't recognize xml: namespace)
            # PyXML doesn't have this problem, and it doesn't have _ns_stack either
            saxparser._ns_stack.append({'http://www.w3.org/XML/1998/namespace':'xml'})
        try:
            saxparser.parse(source)
        except Exception, e:
            if _debug: sys.stderr.write('xml parsing failed\n')
            feedparser.bozo = 1
            feedparser.bozo_exception = feedparser.exc or e
        if feedparser.bozo:
            # feed is not well-formed XML, fall back on regex-based parser
            result['bozo'] = 1
            result['bozo_exception'] = feedparser.bozo_exception
            use_strict_parser = 0
    if not use_strict_parser:
        if _debug: sys.stderr.write('using regexes, now you have two problems\n')
        feedparser = _LooseFeedParser(baseuri, result['encoding'])
        feedparser.feed(data)
    result['feed'] = feedparser.feeddata
    result['entries'] = feedparser.entries
    result['version'] = result['version'] or feedparser.version
    return result

if __name__ == '__main__':
    if not sys.argv[1:]:
        print __doc__
        sys.exit(0)
    else:
        urls = sys.argv[1:]
    from pprint import pprint
    for url in urls:
        print url
        print
        result = parse(url)
        pprint(result['entries'])
        #for entry in result['entries']:
        #    print entry['title']
        #    if entry.has_key('enclosures'):
        #        print entry['enclosures']
        #print

#TODO
#- image
#
#REVISION HISTORY
#1.0 - 9/27/2002 - MAP - fixed namespace processing on prefixed RSS 2.0 elements,
#  added Simon Fell's test suite
#1.1 - 9/29/2002 - MAP - fixed infinite loop on incomplete CDATA sections
#2.0 - 10/19/2002
#  JD - use inchannel to watch out for image and textinput elements which can
#  also contain title, link, and description elements
#  JD - check for isPermaLink="false" attribute on guid elements
#  JD - replaced openAnything with open_resource supporting ETag and
#  If-Modified-Since request headers
#  JD - parse now accepts etag, modified, agent, and referrer optional
#  arguments
#  JD - modified parse to return a dictionary instead of a tuple so that any
#  etag or modified information can be returned and cached by the caller
#2.0.1 - 10/21/2002 - MAP - changed parse() so that if we don't get anything
#  because of etag/modified, return the old etag/modified to the caller to
#  indicate why nothing is being returned
#2.0.2 - 10/21/2002 - JB - added the inchannel to the if statement, otherwise its
#  useless.  Fixes the problem JD was addressing by adding it.
#2.1 - 11/14/2002 - MAP - added gzip support
#2.2 - 1/27/2003 - MAP - added attribute support, admin:generatorAgent.
#  start_admingeneratoragent is an example of how to handle elements with
#  only attributes, no content.
#2.3 - 6/11/2003 - MAP - added USER_AGENT for default (if caller doesn't specify);
#  also, make sure we send the User-Agent even if urllib2 isn't available.
#  Match any variation of backend.userland.com/rss namespace.
#2.3.1 - 6/12/2003 - MAP - if item has both link and guid, return both as-is.
#2.4 - 7/9/2003 - MAP - added preliminary Pie/Atom/Echo support based on Sam Ruby's
#  snapshot of July 1 <http://www.intertwingly.net/blog/1506.html>; changed
#  project name
#2.5 - 7/25/2003 - MAP - changed to Python license (all contributors agree);
#  removed unnecessary urllib code -- urllib2 should always be available anyway;
#  return actual url, status, and full HTTP headers (as result['url'],
#  result['status'], and result['headers']) if parsing a remote feed over HTTP --
#  this should pass all the HTTP tests at <http://diveintomark.org/tests/client/http/>;
#  added the latest namespace-of-the-week for RSS 2.0
#2.5.1 - 7/26/2003 - RMK - clear opener.addheaders so we only send our custom
#  User-Agent (otherwise urllib2 sends two, which confuses some servers)
#2.5.2 - 7/28/2003 - MAP - entity-decode inline xml properly; added support for
#  inline <xhtml:body> and <xhtml:div> as used in some RSS 2.0 feeds
#2.5.3 - 8/6/2003 - TvdV - patch to track whether we're inside an image or
#  textInput, and also to return the character encoding (if specified)
#2.6 - 1/1/2004 - MAP - dc:author support (MarekK); fixed bug tracking
#  nested divs within content (JohnD); fixed missing sys import (JohanS);
#  fixed regular expression to capture XML character encoding (Andrei);
#  added support for Atom 0.3-style links; fixed bug with textInput tracking;
#  added support for cloud (MartijnP); added support for multiple
#  category/dc:subject (MartijnP); normalize content model: "description" gets
#  description (which can come from description, summary, or full content if no
#  description), "content" gets dict of base/language/type/value (which can come
#  from content:encoded, xhtml:body, content, or fullitem);
#  fixed bug matching arbitrary Userland namespaces; added xml:base and xml:lang
#  tracking; fixed bug tracking unknown tags; fixed bug tracking content when
#  <content> element is not in default namespace (like Pocketsoap feed);
#  resolve relative URLs in link, guid, docs, url, comments, wfw:comment,
#  wfw:commentRSS; resolve relative URLs within embedded HTML markup in
#  description, xhtml:body, content, content:encoded, title, subtitle,
#  summary, info, tagline, and copyright; added support for pingback and
#  trackback namespaces
#2.7 - 1/5/2004 - MAP - really added support for trackback and pingback
#  namespaces, as opposed to 2.6 when I said I did but didn't really;
#  sanitize HTML markup within some elements; added mxTidy support (if
#  installed) to tidy HTML markup within some elements; fixed indentation
#  bug in _parse_date (FazalM); use socket.setdefaulttimeout if available
#  (FazalM); universal date parsing and normalization (FazalM): 'created', modified',
#  'issued' are parsed into 9-tuple date format and stored in 'created_parsed',
#  'modified_parsed', and 'issued_parsed'; 'date' is duplicated in 'modified'
#  and vice-versa; 'date_parsed' is duplicated in 'modified_parsed' and vice-versa
#2.7.1 - 1/9/2004 - MAP - fixed bug handling &quot; and &apos;.  fixed memory
#  leak not closing url opener (JohnD); added dc:publisher support (MarekK);
#  added admin:errorReportsTo support (MarekK); Python 2.1 dict support (MarekK)
#2.7.4 - 1/14/2004 - MAP - added workaround for improperly formed <br/> tags in
#  encoded HTML (skadz); fixed unicode handling in normalize_attrs (ChrisL);
#  fixed relative URI processing for guid (skadz); added ICBM support; added
#  base64 support
#2.7.5 - 1/15/2004 - MAP - added workaround for malformed DOCTYPE (seen on many
#  blogspot.com sites); added _debug variable
#2.7.6 - 1/16/2004 - MAP - fixed bug with StringIO importing
#3.0b3 - 1/23/2004 - MAP - parse entire feed with real XML parser (if available);
#  added several new supported namespaces; fixed bug tracking naked markup in
#  description; added support for enclosure; added support for source; re-added
#  support for cloud which got dropped somehow; added support for expirationDate
#3.0b4 - 1/26/2004 - MAP - fixed xml:lang inheritance; fixed multiple bugs tracking
#  xml:base URI, one for documents that don't define one explicitly and one for
#  documents that define an outer and an inner xml:base that goes out of scope
#  before the end of the document
#3.0b5 - 1/26/2004 - MAP - fixed bug parsing multiple links at feed level
#3.0b6 - 1/27/2004 - MAP - added feed type and version detection, result["version"]
#  will be one of SUPPORTED_VERSIONS.keys() or empty string if unrecognized;
#  added support for creativeCommons:license and cc:license; added support for
#  full Atom content model in title, tagline, info, copyright, summary; fixed bug
#  with gzip encoding (not always telling server we support it when we do)
#3.0b7 - 1/28/2004 - MAP - support Atom-style author element in author_detail
#  (dictionary of "name", "url", "email"); map author to author_detail if author
#  contains name + email address
#3.0b8 - 1/28/2004 - MAP - added support for contributor
#3.0b9 - 1/29/2004 - MAP - fixed check for presence of dict function; added
#  support for summary
#3.0b10 - 1/31/2004 - MAP - incorporated ISO-8601 date parsing routines from
#  xml.util.iso8601
#3.0b11 - 2/2/2004 - MAP - added 'rights' to list of elements that can contain
#  dangerous markup; fiddled with decodeEntities (not right); liberalized
#  date parsing even further
#3.0b12 - 2/6/2004 - MAP - fiddled with decodeEntities (still not right);
#  added support to Atom 0.2 subtitle; added support for Atom content model
#  in copyright; better sanitizing of dangerous HTML elements with end tags
#  (script, frameset)
#3.0b13 - 2/8/2004 - MAP - better handling of empty HTML tags (br, hr, img,
#  etc.) in embedded markup, in either HTML or XHTML form (<br>, <br/>, <br />)
#3.0b14 - 2/8/2004 - MAP - fixed CDATA handling in non-wellformed feeds under
#  Python 2.1
#3.0b15 - 2/11/2004 - MAP - fixed bug resolving relative links in wfw:commentRSS;
#  fixed bug capturing author and contributor URL; fixed bug resolving relative
#  links in author and contributor URL; fixed bug resolvin relative links in
#  generator URL; added support for recognizing RSS 1.0; passed Simon Fell's
#  namespace tests, and included them permanently in the test suite with his
#  permission; fixed namespace handling under Python 2.1
#3.0b16 - 2/12/2004 - MAP - fixed support for RSS 0.90 (broken in b15)
#3.0b17 - 2/13/2004 - MAP - determine character encoding as per RFC 3023
#3.0b18 - 2/17/2004 - MAP - always map description to summary_detail (Andrei);
#  use libxml2 (if available)
#3.0b19 - 3/15/2004 - MAP - fixed bug exploding author information when author
#  name was in parentheses; removed ultra-problematic mxTidy support; patch to
#  workaround crash in PyXML/expat when encountering invalid entities
#  (MarkMoraes); support for textinput/textInput
#3.0b20 - 4/7/2004 - MAP - added CDF support
#3.0b21 - 4/14/2004 - MAP - added Hot RSS support
#3.0b22 - 4/19/2004 - MAP - changed 'channel' to 'feed', 'item' to 'entries' in
#  results dict; changed results dict to allow getting values with results.key
#  as well as results[key]; work around embedded illformed HTML with half
#  a DOCTYPE; work around malformed Content-Type header; if character encoding
#  is wrong, try several common ones before falling back to regexes (if this
#  works, bozo_exception is set to CharacterEncodingOverride); fixed character
#  encoding issues in BaseHTMLProcessor by tracking encoding and converting
#  from Unicode to raw strings before feeding data to sgmllib.SGMLParser;
#  convert each value in results to Unicode (if possible), even if using
#  regex-based parsing
#3.0b23 - 4/21/2004 - MAP - fixed UnicodeDecodeError for feeds that contain
#  high-bit characters in attributes in embedded HTML in description (thanks
#  Thijs van de Vossen); moved guid, date, and date_parsed to mapped keys in
#  FeedParserDict; tweaked FeedParserDict.has_key to return True if asking
#  about a mapped key
#3.0fc1 - 4/23/2004 - MAP - made results.entries[0].links[0] and
#  results.entries[0].enclosures[0] into FeedParserDict; fixed typo that could
#  cause the same encoding to be tried twice (even if it failed the first time);
#  fixed DOCTYPE stripping when DOCTYPE contained entity declarations;
#  better textinput and image tracking in illformed RSS 1.0 feeds
