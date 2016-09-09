/*
 * mpatrol
 * A library for controlling and tracing dynamic memory allocations.
 * Copyright (C) 1997-2002 Graeme S. Roy <graeme.roy@analog.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307, USA.
 */


/*
 * Mathematical support routines.  The signed and unsigned LEB128 support
 * routines are derived from algorithms printed in the DWARF 2 debugging
 * information format specification.
 */


#include "utils.h"


#if MP_IDENT_SUPPORT
#ident "$Id: utils.c,v 1.11 2002/01/08 20:13:59 graeme Exp $"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *utils_id = "$Id: utils.c,v 1.11 2002/01/08 20:13:59 graeme Exp $";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* Return the base-two logarithm of an unsigned integer.
 */

MP_GLOBAL
unsigned char
__mp_logtwo(unsigned long n)
{
    unsigned char l;

    for (l = 0; n > 0; l++, n >>= 1);
    return (unsigned char) ((l == 0) ? 0 : l - 1);
}


/* Return the truncated square root of an unsigned integer.
 */

MP_GLOBAL
unsigned long
__mp_squareroot(unsigned long n)
{
    unsigned long r, t;

    for (r = 0, t = 1; n >= t; n -= t, r++, t += 2);
    return r;
}


#if !MP_MACROROUTINES
/* Determine if an unsigned integer is a power of two.
 */

MP_GLOBAL
int
__mp_ispoweroftwo(unsigned long n)
{
    return ((n > 0) && ((n & (n - 1)) == 0));
}
#endif /* MP_MACROROUTINES */


/* Round an unsigned integer up to the nearest power of two.
 */

MP_GLOBAL
unsigned long
__mp_poweroftwo(unsigned long n)
{
    if ((n == 0) || __mp_ispoweroftwo(n))
        return n;
    return (unsigned long) (2 << __mp_logtwo(n));
}


#if !MP_MACROROUTINES
/* Round an unsigned integer down to a specified power of two alignment.
 */

MP_GLOBAL
unsigned long
__mp_rounddown(unsigned long n, unsigned long a)
{
    return n & ~(a - 1);
}
#endif /* MP_MACROROUTINES */


#if !MP_MACROROUTINES
/* Round an unsigned integer up to a specified power of two alignment.
 */

MP_GLOBAL
unsigned long
__mp_roundup(unsigned long n, unsigned long a)
{
    return ((n - 1) & ~(a - 1)) + a;
}
#endif /* MP_MACROROUTINES */


/* Convert a signed integer to a signed LEB128 number.
 */

MP_GLOBAL
void *
__mp_encodesleb128(long n, size_t *l)
{
    static unsigned char b[32];
    size_t i;
    int p, s;

    i = 0;
    p = (n >= 0);
    do
    {
        b[i] = n & 0x7F;
        n >>= 7;
        if (!p)
            n |= -(1UL << ((sizeof(long) << 3) - 7));
        s = b[i] & 0x40;
        if (((n != 0) || (s != 0)) && ((n != -1) || (s == 0)))
            b[i] |= 0x80;
    }
    while (b[i++] & 0x80);
    *l = i;
    return b;
}


/* Convert an unsigned integer to an unsigned LEB128 number.
 */

MP_GLOBAL
void *
__mp_encodeuleb128(unsigned long n, size_t *l)
{
    static unsigned char b[32];
    size_t i;

    i = 0;
    do
    {
        b[i] = n & 0x7F;
        if (n >>= 7)
            b[i] |= 0x80;
    }
    while (b[i++] & 0x80);
    *l = i;
    return b;
}


/* Convert a signed LEB128 number to a signed integer.
 */

MP_GLOBAL
long
__mp_decodesleb128(void *d, size_t *l)
{
    unsigned char *b;
    long n;
    unsigned char s;

    b = (unsigned char *) d;
    n = 0;
    s = 0;
    do
    {
        n |= (*b & 0x7F) << s;
        s += 7;
    }
    while (*b++ & 0x80);
    if ((s < sizeof(long) << 3) && (*(b - 1) & 0x40))
        n |= -(1 << s);
    *l = (size_t) (b - (unsigned char *) d);
    return n;
}


/* Convert an unsigned LEB128 number to an unsigned integer.
 */

MP_GLOBAL
unsigned long
__mp_decodeuleb128(void *d, size_t *l)
{
    unsigned char *b;
    unsigned long n;
    unsigned char s;

    b = (unsigned char *) d;
    n = 0;
    s = 0;
    do
    {
        n |= (*b & 0x7F) << s;
        s += 7;
    }
    while (*b++ & 0x80);
    *l = (size_t) (b - (unsigned char *) d);
    return n;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
