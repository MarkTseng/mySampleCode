#include <glib.h>
gint my_finder(gconstpointer item) {
 return g_ascii_strcasecmp(item, "second");
}
int main(int argc, char** argv) {
 GSList* list = g_slist_append(NULL, "first");
 list = g_slist_append(list, "second");
 list = g_slist_append(list, "third");
 GSList* item = g_slist_find(list, "second");
 printf("This should be the 'second' item: '%s'\n", item->data);
 item = g_slist_find_custom(list, NULL, (GCompareFunc)my_finder);
 printf("Again, this should be the 'second' item: '%s'\n", item->data);
 item = g_slist_find(list, "delta");
 printf("'delta' is not in the list, so we get: '%s'\n", item ? item->data : "(null)");
 g_slist_free(list);
 return 0;
}
