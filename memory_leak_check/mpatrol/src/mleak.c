/*
 * mpatrol
 * A library for controlling and tracing dynamic memory allocations.
 * Copyright (C) 1997-2008 Graeme S. Roy <graemeroy@users.sourceforge.net>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


/*
 * A tool designed to read a log file produced by the mpatrol library
 * and report any unfreed memory allocations.  This should be used if
 * the mpatrol library could not finish writing the log file due to
 * abnormal program termination, but note that some of the unfreed
 * allocations might have been freed if the program had terminated
 * successfully.  Also note that no attempt is made to account for
 * resizing of memory allocations and so the total amount of memory
 * used by the resulting unfreed allocations may not be entirely accurate.
 */


#include "tree.h"
#include "getopt.h"
#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#if MP_IDENT_SUPPORT
#ident "$Id$"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *mleak_id = "$Id$";
#endif /* MP_IDENT_SUPPORT */


#define PROGVERSION "1.3" /* the current version of this program */


/* The flags used to parse the command line options.
 */

typedef enum options_flags
{
    OF_HELP     = 'h',
    OF_IGNORE   = 'i',
    OF_MAXSTACK = 'n',
    OF_VERSION  = 'V'
}
options_flags;


/* Structure containing the allocation details and log file offset for a
 * single memory allocation.
 */

typedef struct allocation
{
    treenode node;        /* tree node */
    unsigned long addr;   /* allocation address */
    unsigned long size;   /* allocation size */
    unsigned long offset; /* log file offset */
}
allocation;


/* The tree containing information about each memory allocation.
 */

static treeroot alloctree;


/* The total number of bytes currently allocated.
 */

static unsigned long alloctotal;


/* The log file produced by mpatrol.
 */

static FILE *logfile;


/* The current offset in the log file.
 */

static long fileoffset;


/* The filename used to invoke this tool.
 */

static char *progname;


/* Indicates that the unfreed memory allocation list in the log file, if it
 * exists, should be ignored.
 */

static int ignorelist;


/* Indicates the maximum stack depth to display.
 */

static unsigned long maxstack;


/* The table describing all recognised options.
 */

static option options_table[] =
{
    {"help", OF_HELP, NULL,
     "\tDisplays this quick-reference option summary.\n"},
    {"ignore", OF_IGNORE, NULL,
     "\tSpecifies that the list of unfreed allocations in the log file\n"
     "\tshould be ignored.\n"},
    {"max-stack", OF_MAXSTACK, "depth",
     "\tSpecifies the maximum stack depth to display.\n"},
    {"version", OF_VERSION, NULL,
     "\tDisplays the version number of this program.\n"},
    NULL
};


/* Create a new memory allocation and record its log file offset.
 */

static
void
newalloc(unsigned long i, unsigned long a, unsigned long l, unsigned long o)
{
    allocation *n;

    if ((n = (allocation *) malloc(sizeof(allocation))) == NULL)
    {
        fprintf(stderr, "%s: Out of memory\n", progname);
        exit(EXIT_FAILURE);
    }
    __mp_treeinsert(&alloctree, &n->node, i);
    n->addr = a;
    n->size = l;
    n->offset = o;
    alloctotal += l;
}


/* Free an existing memory allocation.
 */

static
void
freealloc(unsigned long i)
{
    allocation *n;

    if (n = (allocation *) __mp_search(alloctree.root, i))
    {
        __mp_treeremove(&alloctree, &n->node);
        alloctotal -= n->size;
        free(n);
    }
}


/* Read an input line from the log file.
 */

static
char *
getnextline(void)
{
    static char s[MP_BUFFER_SIZE + 1];
    unsigned long i;
    int c;

    i = 0;
    /* Record the file offset so that we can go back to this position during
     * the second pass of the log file.
     */
    if ((fileoffset = ftell(logfile)) == -1)
    {
        fprintf(stderr, "%s: Cannot determine file position\n", progname);
        exit(EXIT_FAILURE);
    }
    while (((c = fgetc(logfile)) != EOF) && (c != '\n'))
    {
        if (i == MP_BUFFER_SIZE)
        {
            fprintf(stderr, "%s: Buffer overflow\n", progname);
            exit(EXIT_FAILURE);
        }
        s[i++] = c;
    }
    if (c == EOF)
        return NULL;
    s[i] = '\0';
    return s;
}


/* Log the allocations and deallocations from the log file.
 */

static
void
readfile(void)
{
    char *s, *t;
    unsigned long a, l, n, o;

    while (s = getnextline())
        if (strncmp(s, "ALLOC: ", 7) == 0)
        {
            /* Parse relevant details from the memory allocation and
             * add the allocation to the allocation tree.
             */
            o = fileoffset;
            if ((s = strchr(s + 7, '(')) && (t = strchr(s + 1, ',')))
            {
                /* Get the allocation index.
                 */
                *t = '\0';
                n = strtoul(s + 1, NULL, 0);
                if ((*(s = t + 1) == ' ') && (t = strchr(s + 1, ' ')))
                {
                    /* Get the allocation size.
                     */
                    *t = '\0';
                    l = strtoul(s + 1, NULL, 0);
                    /* Don't record the allocation if the pointer returned is
                     * NULL.
                     */
                    while ((s = getnextline()) &&
                           (strncmp(s, "returns ", 8) != 0));
                    if ((n != 0) && (s != NULL) &&
                        (a = strtoul(s + 8, NULL, 0)))
                        newalloc(n, a, l, o);
                }
            }
        }
        else if (strncmp(s, "FREE: ", 6) == 0)
        {
            /* Parse relevant details from the memory deallocation and
             * remove the allocation from the allocation tree.
             */
            if ((s = strchr(s + 6, '(')) && (t = strchr(s + 1, ')')))
            {
                /* Get the allocation address.
                 */
                *t = '\0';
                if (a = strtoul(s + 1, NULL, 0))
                {
                    while ((s = getnextline()) && (*s != '\0'));
                    /* Don't record the deallocation if a warning or error
                     * occurred.
                     */
                    if ((s = getnextline()) && (strncmp(s, "    ", 4) == 0) &&
                        (s = strchr(s + 4, ':')) && (t = strchr(s + 1, ':')))
                    {
                        /* Get the allocation index.
                         */
                        *t = '\0';
                        n = strtoul(s + 1, NULL, 0);
                        freealloc(n);
                    }
                }
            }
        }
        else if (!ignorelist && (strncmp(s, "unfreed allocations: ", 21) == 0))
            /* If we get here then there is already a list of unfreed memory
             * allocations in the log file.  In this case we just parse them
             * anyway, adding any new entries to the allocation tree.
             */
            while (s = getnextline())
            {
                /* Parse relevant details from the unfreed allocation and
                 * add the allocation to the allocation tree.
                 */
                o = fileoffset;
                if ((strncmp(s, "    ", 4) == 0) && (t = strchr(s + 4, ' ')))
                {
                    /* Get the allocation address.
                     */
                    *t = '\0';
                    if ((a = strtoul(s + 4, NULL, 0)) &&
                        (*(s = t + 1) == '(') && (t = strchr(s + 1, ' ')))
                    {
                        /* Get the allocation size.
                         */
                        *t = '\0';
                        l = strtoul(s + 1, NULL, 0);
                        if ((s = strchr(t + 1, ':')) &&
                            (t = strchr(s + 1, ':')))
                        {
                            /* Get the allocation index.
                             */
                            *t = '\0';
                            n = strtoul(s + 1, NULL, 0);
                            if (!__mp_search(alloctree.root, n))
                                newalloc(n, a, l, o);
                        }
                    }
                }
                while ((s = getnextline()) && (*s != '\0'));
            }
}


/* Display all remaining memory allocations.
 */

static
void
printallocs(void)
{
    allocation *n, *p;
    char *r, *s, *t;
    size_t i;

    printf("unfreed allocations: %lu (%lu byte%s)\n", alloctree.size,
           alloctotal, (alloctotal == 1) ? "" : "s");
    for (n = (allocation *) __mp_minimum(alloctree.root); n != NULL; n = p)
    {
        p = (allocation *) __mp_successor(&n->node);
        /* Move to the position in the log file that records the original
         * allocation.
         */
        if (fseek(logfile, n->offset, SEEK_SET) == -1)
        {
            fprintf(stderr, "%s: Cannot set file position\n", progname);
            exit(EXIT_FAILURE);
        }
        /* Extract the relevant information from the allocation log so that
         * we can format it in the same way as that displayed for the
         * SHOWUNFREED option.
         */
        if (s = getnextline())
            if ((strncmp(s, "ALLOC: ", 7) == 0) &&
                (t = strchr(s + 7, '(')) && (t > s) && (*(t = t - 1) == ' '))
            {
                *t = '\0';
                r = s + 7;
                if (s = strchr(t + 2, '['))
                {
                    i = 0;
                    printf("    " MP_POINTER " (%lu byte%s) {%s:%lu:0} %s\n",
                           n->addr, n->size, (n->size == 1) ? "" : "s", r,
                           n->node.key, s);
                    while ((s = getnextline()) && (*s != '\0'))
                        if (i++ < maxstack)
                            puts(s);
                    if ((alloctree.size > 1) && (maxstack != 0))
                        putchar('\n');
                }
            }
            else
            {
                i = 0;
                puts(s);
                while ((s = getnextline()) && (*s != '\0'))
                    if (i++ < maxstack)
                        puts(s);
                if ((alloctree.size > 1) && (maxstack != 0))
                    putchar('\n');
            }
        __mp_treeremove(&alloctree, &n->node);
        free(n);
    }
}


/* Read the log file and display all unfreed memory allocations.
 */

int
main(int argc, char **argv)
{
    char b[256];
    char *f;
    int c, e, h, v;

    e = h = v = 0;
    maxstack = ~0;
    progname = __mp_basename(argv[0]);
    while ((c = __mp_getopt(argc, argv, __mp_shortopts(b, options_table),
             options_table)) != EOF)
        switch (c)
        {
          case OF_HELP:
            h = 1;
            break;
          case OF_IGNORE:
            ignorelist = 1;
            break;
          case OF_MAXSTACK:
            if (!__mp_getnum(progname, __mp_optarg, (long *) &maxstack, 1))
                e = 1;
            break;
          case OF_VERSION:
            v = 1;
            break;
          default:
            e = 1;
            break;
        }
    argc -= __mp_optindex;
    argv += __mp_optindex;
    if (v == 1)
    {
        fprintf(stdout, "%s %s\n%s %s\n\n", progname, PROGVERSION,
                __mp_copyright, __mp_author);
        fputs("This is free software, and you are welcome to redistribute it "
              "under certain\n", stdout);
        fputs("conditions; see the GNU Lesser General Public License for "
              "details.\n\n", stdout);
        fputs("For the latest mpatrol release and documentation,\n", stdout);
        fprintf(stdout, "visit %s.\n\n", __mp_homepage);
    }
    if (argc > 1)
        e = 1;
    if ((e == 1) || (h == 1))
    {
        fprintf(stdout, "Usage: %s [options] [file]\n\n", progname);
        if (h == 0)
            fprintf(stdout, "Type `%s --help' for a complete list of "
                    "options.\n", progname);
        else
            __mp_showopts(options_table);
        if (e == 1)
            exit(EXIT_FAILURE);
        exit(EXIT_SUCCESS);
    }
    if (argc == 1)
        f = argv[0];
    else
        f = MP_LOGFILE;
    __mp_newtree(&alloctree);
    alloctotal = 0;
    if (strcmp(f, "-") == 0)
        logfile = stdin;
    else if ((logfile = fopen(f, "r")) == NULL)
    {
        fprintf(stderr, "%s: Cannot open file `%s'\n", progname, f);
        exit(EXIT_FAILURE);
    }
    fileoffset = 0;
    readfile();
    printallocs();
    fclose(logfile);
    return EXIT_SUCCESS;
}
