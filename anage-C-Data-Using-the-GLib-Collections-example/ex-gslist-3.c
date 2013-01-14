#include <glib.h>
int main(int argc, char** argv) {
 GSList* list = NULL;
 list = g_slist_append(list, "first");
 list = g_slist_append(list, "second");
 list = g_slist_append(list, "second");
 list = g_slist_append(list, "third");
 list = g_slist_append(list, "third");
 printf("The list is now %d items long\n", g_slist_length(list));
 list = g_slist_remove(list, "second");
 list = g_slist_remove_all(list, "third");
 printf("The list is now %d items long\n", g_slist_length(list));
 g_slist_free(list);
 return 0;
}
