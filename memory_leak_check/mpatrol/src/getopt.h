#ifndef MP_GETOPT_H
#define MP_GETOPT_H


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
 * Option parsing.  Provides a common interface for the mpatrol tools to parse
 * command line options.
 */


/*
 * $Id$
 */


#include "config.h"


#define SHORTOPT_MIN 1   /* minimum value for short options */
#define SHORTOPT_MAX 255 /* maximum value for short options */


/* Contains all of the details about long options.
 */

typedef struct option
{
    char *name; /* option name */
    int value;  /* option value */
    char *arg;  /* option argument */
    char *desc; /* option description */
}
option;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


MP_EXPORT unsigned long __mp_optindex;
MP_EXPORT char *__mp_optarg;


MP_EXPORT char *__mp_basename(char *);
MP_EXPORT int __mp_getnum(char *, char *, long *, int);
MP_EXPORT int __mp_getopt(unsigned long, char **, char *, option *);
MP_EXPORT char *__mp_shortopts(char *, option *);
MP_EXPORT void __mp_showopts(option *);
MP_EXPORT int __mp_match(char *, char *);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MP_GETOPT_H */
