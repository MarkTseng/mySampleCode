#include <glib.h>
int main(int argc, char** argv) {
 GSList* list = NULL, *iterator = NULL;
 list = g_slist_append(list, "first");
 list = g_slist_append(list, "second");
 list = g_slist_append(list, "third");
 for (iterator = list; iterator; iterator = iterator->next) {
  printf("Current item is '%s'\n", iterator->data);
 }
 g_slist_free(list);
 return 0;
}
