/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * llist_test.c --
 *
 * Exercizer program for linked list
 */
#include "llist.h"
#include <stdio.h>
#include <stdlib.h>

#define MAXDATALEN 40

extern int
main(void) {
  
  llist_t ll;
  char *string;
  int index;

  llist_init(&ll);

  while (TRUE) {
    printf ("Input index number (or 0 to quit): ");
    scanf ("%d", &index);
    if (index == 0) 
      break;
    string = malloc(MAXDATALEN);
    if (string == NULL) 
      perror("malloc");
    printf ("Input data char: ");
    scanf ("%s", string);
    llist_insert_data(index, string, &ll);
  }

  llist_show(&ll);

  return 0;
}
