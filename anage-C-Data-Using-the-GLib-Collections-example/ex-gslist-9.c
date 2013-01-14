#include <glib.h>
gint my_comparator(gconstpointer item1, gconstpointer item2) {
 return g_ascii_strcasecmp(item1, item2);
}
int main(int argc, char** argv) {
 GSList* list = g_slist_append(NULL, "Chicago");
 list = g_slist_append(list, "Boston");
 list = g_slist_append(list, "Albany");
 list = g_slist_sort(list, (GCompareFunc)my_comparator);
 printf("The first item is now '%s'\n", list->data);
 printf("The last item is now '%s'\n", g_slist_last(list)->data);
 g_slist_free(list);
 return 0;
}
