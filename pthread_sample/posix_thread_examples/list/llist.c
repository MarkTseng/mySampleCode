/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * llist.c --
 * 
 * Linked list for single threaded application
 */
#include "llist.h"
#include <stdio.h>
#include <stdlib.h>

int llist_init (llist_t *llistp) 
{
  *llistp = NULL;

  return 0;
}
int llist_insert_data (int index, void *datap, llist_t *llistp) 
{
  llist_node_t *cur, *prev, *new;
  int found = FALSE;

  for (cur=prev=*llistp; cur != NULL; prev=cur, cur=cur->nextp) {
    if (cur->index == index) {
      free(cur->datap);
      cur->datap = datap;
      found=TRUE;
      break;
    }
    else if (cur->index > index){
      break;
    }
  }
  if (!found) {
    new = (llist_node_t *)malloc(sizeof(llist_node_t));
    new->index = index;
    new->datap = datap;
    new->nextp = cur;
    if (cur==*llistp)
      *llistp = new;
    else
      prev->nextp = new;
  }
  return 0;
}
int llist_remove_data(int index, void **datapp, llist_t *llistp) 
{
  llist_node_t *cur, *prev;

  /* Initialize to "not found" */
  *datapp = NULL;

  for (cur=prev=*llistp; cur != NULL; prev=cur, cur=cur->nextp) {
    if (cur->index == index) {
      *datapp = cur->datap;
      prev->nextp = cur->nextp;
      free(cur);
      break;
    }
    else if (cur->index > index){
      break;
    }
  }

  return 0;
}
int llist_find_data(int index, void **datapp, llist_t *llistp) 
{
  llist_node_t *cur, *prev;

  /* Initialize to "not found" */
  *datapp = NULL;

  /* Look through index for our entry */
  for (cur=prev=*llistp; cur != NULL; prev=cur, cur=cur->nextp) {
    if (cur->index == index) {
      *datapp = cur->datap; 
      break;
    }
    else if (cur->index > index){
      break;
    }
  }

  return 0;
}
int llist_change_data(int index, void *datap, llist_t *llistp)
{
  llist_node_t *cur, *prev;
  int status = -1; /* assume failure */

  for (cur=prev=*llistp; cur != NULL; prev=cur, cur=cur->nextp) {
    if (cur->index == index) {
      cur->datap = datap;
      prev->nextp = cur->nextp;
      free(cur);
      status = 0;
      break;
    }
    else if (cur->index > index){
      break;
    }
  }

  return status;
}
int llist_show(llist_t *llistp)
{
  llist_node_t *cur;

  printf (" Linked list contains : \n");
  for (cur=*llistp; cur != NULL; cur=cur->nextp) {
    printf ("Index: %d\tData: %s \n", cur->index, (char *)cur->datap);    
  }

  return 0;
}

