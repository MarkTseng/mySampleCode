#include <glib.h>
int main(int argc, char** argv) {
 GSList* list = NULL;
 list = g_slist_append(list, "second");
 list = g_slist_prepend(list, "first");
 printf("The list is now %d items long\n", g_slist_length(list));
 list = g_slist_remove(list, "first");
 printf("The list is now %d items long\n", g_slist_length(list));
 g_slist_free(list);
 return 0;
}
