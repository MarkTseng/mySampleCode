#include <glib.h>
int main(int argc, char** argv) {
 GSList* list = g_slist_append(NULL, "Anaheim "), *iterator = NULL;
 list = g_slist_append(list, "Elkton ");
 printf("Before inserting 'Boston', second item is: '%s'\n", g_slist_nth(list, 1)->data);
 g_slist_insert(list, "Boston ", 1);
 printf("After insertion, second item is: '%s'\n", g_slist_nth(list, 1)->data);
 list = g_slist_insert_before(list, g_slist_nth(list, 2), "Chicago ");
 printf("After an insert_before, third item is: '%s'\n", g_slist_nth(list, 2)->data);
 list = g_slist_insert_sorted(list, "Denver ", (GCompareFunc)g_ascii_strcasecmp);
 printf("After inserting 'Denver', here's the final list:\n");
 g_slist_foreach(list, (GFunc)printf, NULL);
 g_slist_free(list);
 return 0;
}
