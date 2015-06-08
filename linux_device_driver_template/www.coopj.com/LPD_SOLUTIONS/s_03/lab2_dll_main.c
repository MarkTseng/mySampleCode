/* **************** LPD:1.0 s_03/lab2_dll_main.c **************** */
/*
 * The code herein is: Copyright Jerry Cooperstein, 2009
 *
 * This Copyright is retained for the purpose of protecting free
 * redistribution of source.
 *
 *     URL:    http://www.coopj.com
 *     email:  coop@coopj.com
 *
 * The primary maintainer for this code is Jerry Cooperstein
 * The CONTRIBUTORS file (distributed with this
 * file) lists those known to have contributed to the source.
 *
 * This code is distributed under Version 2 of the GNU General Public
 * License, which you should have received with the source.
 *
 */
/*
 * Dyanmical Loading (main program)
 *
 * Make a simple shared library (libhello_dl.so) that
 * has a "hello world" print function, and one or more constructor and
 * destructor routines.
 *
 * Make a test program that dynamically loads the library and executes
 * the function in it.
 @*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

#define LIBNAME "./libhello_dl.so"

int main (int argc, char *argv[])
{
    char *err;
    void *handle;
    void (*printit) (void);

    handle = dlopen (LIBNAME, RTLD_LAZY);
    err = dlerror ();

    if (err) {
        fprintf (stderr, "error opening library is  %s\n", err);
        exit (-1);
    }

    printit = dlsym (handle, "hello_dl");

    err = dlerror ();
    if (err) {
        fprintf (stderr, "error getting symbol is  %s\n", err);
        exit (-1);
    }

    printit ();

    dlclose (handle);

    exit (0);
}
