/* **************** LPD:1.0 s_03/dynamic.c **************** */
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
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main (int argc, char *argv[])
{
    void *handle;
    void (*function) (void);

    handle = dlopen ("./libshared.so", RTLD_LAZY);
    if (!handle) {
        fprintf (stderr, "error opening library is  %s\n", dlerror ());
        exit (-1);
    }

    function = dlsym (handle, "lib_function");
    function ();
    dlclose (handle);
    exit (0);
}
