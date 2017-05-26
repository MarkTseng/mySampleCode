// Jesse Kennedy
// CPSC 322 Project 1
// Fall 2015

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

void __attribute__ ((constructor)) shim_init(void);
void __attribute__ ((destructor)) shim_cleanup(void);

void free(void *point);
void *malloc(size_t size);

void *(*original_malloc)(size_t size) = NULL;
void (*original_free)(void *point) = NULL;

struct Node {
  void *value;
  int size;
  struct Node *next;
};

struct Node *head = NULL;

void shim_init(void) {
  original_malloc = dlsym(RTLD_NEXT, "malloc");
  original_free = dlsym(RTLD_NEXT, "free");
}

void *malloc(size_t size) {
  void *block = original_malloc(size);
  struct Node *tmp = original_malloc(sizeof(struct Node));
  tmp->size = size;
  tmp->value = block;
  tmp->next = head;
  head = tmp;

  return block;
}

void free(void *ptr) {
  original_free(ptr);

  struct Node *cursor = NULL, *prev = NULL;

  for (cursor = head; cursor != NULL; prev = cursor, cursor = cursor->next) {
    if (cursor->value == ptr) {
      if (prev == NULL) {
        head = cursor->next;
      } else {
        prev->next = cursor->next;
      }
      original_free(cursor);
      return;
    }
  }

  return;
}

void shim_cleanup(void) {
  int leak_count = 0, leak_size = 0;
  struct Node *cursor = NULL;

  for (cursor = head; cursor != NULL; cursor = cursor->next) {
    leak_count++;
    leak_size += cursor->size;
    fprintf(stderr, "LEAK\t%d\n", cursor->size);
  }

  fprintf(stderr, "TOTAL\t%d\t%d\n", leak_count, leak_size);
}