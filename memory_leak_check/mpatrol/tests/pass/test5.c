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
 * Creates 16 threads, each of which allocate 16 blocks of varying
 * sizes and then reallocate and free them in a different order from
 * which they were allocated.
 */


#include "mpatrol.h"
#include <stdio.h>
#if defined(AMIGA)
#error this test does not run on this system
#elif defined(_WIN32)
#include <windows.h>
#include <winbase.h>
#elif defined(_NETWARE)
#include <nwthread.h>
#else /* AMIGA && _WIN32 && _NETWARE */
#include <pthread.h>
#endif /* AMIGA && _WIN32 && _NETWARE */


#if defined(_WIN32)
unsigned long WINAPI thread(void *d)
#elif defined(_NETWARE)
void thread(void *d)
#else /* _WIN32 && _NETWARE */
void *thread(void *d)
#endif /* _WIN32 && _NETWARE */
{
    void *a[16];
    size_t i, n;

    for (i = 0; i < 16; i++)
    {
        n = (size_t) ((rand() * 16.0) / (RAND_MAX + 1.0)) + 1;
        a[i] = malloc(n);
    }
    for (i = 16; i > 0; i--)
    {
        n = (size_t) ((rand() * 16.0) / (RAND_MAX + 1.0)) + 1;
        a[i - 1] = realloc(a[i - 1], n);
    }
    for (i = 0; i < 16; i += 2)
        free(a[i]);
    for (i = 16; i > 0; i -= 2)
        free(a[i - 1]);
#if defined(_WIN32)
    return 0;
#elif !defined(_NETWARE)
    return NULL;
#endif /* _WIN32 && !_NETWARE */
}


int main(void)
{
#if defined(_WIN32)
    HANDLE t[16];
#elif defined(_NETWARE)
    int t[16];
#else /* _WIN32 && _NETWARE */
    pthread_t t[16];
    void *r;
#endif /* _WIN32 && _NETWARE */
    size_t i;

    for (i = 0; i < 16; i++)
#if defined(_WIN32)
        if ((t[i] = CreateThread(NULL, 0, thread, NULL, 0, NULL)) == NULL)
#elif defined(_NETWARE)
        if ((t[i] = BeginThread(thread, NULL, 0, NULL)) == EFAILURE)
#else /* _WIN32 && _NETWARE */
        if (pthread_create(&t[i], NULL, thread, NULL) == -1)
#endif /* _WIN32 && _NETWARE */
        {
            fprintf(stderr, "error creating thread %lu\n", i);
            exit(EXIT_FAILURE);
        }
#if !defined(_NETWARE)
    for (i = 0; i < 16; i++)
#if defined(_WIN32)
        if (WaitForSingleObject(t[i], INFINITE) == -1)
#else /* _WIN32 */
        if (pthread_join(t[i], &r) == -1)
#endif /* _WIN32 */
        {
            fprintf(stderr, "error waiting for thread %lu to finish\n", i);
            exit(EXIT_FAILURE);
        }
#endif /* !_NETWARE */
    return EXIT_SUCCESS;
}
