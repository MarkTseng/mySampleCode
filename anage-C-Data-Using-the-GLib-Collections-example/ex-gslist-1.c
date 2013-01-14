#include <glib.h>
int main(int argc, char** argv) {
 GSList* list = NULL;
 printf("The list is now %d items long\n", g_slist_length(list));
 list = g_slist_append(list, "first");
 list = g_slist_append(list, "second");
 printf("The list is now %d items long\n", g_slist_length(list));
 g_slist_free(list);
 return 0;
}
