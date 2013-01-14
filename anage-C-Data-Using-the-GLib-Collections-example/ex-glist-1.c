#include <glib.h>
int main(int argc, char** argv) {
 GList* list = NULL;
 list = g_list_append(list, "Austin ");
 printf("The first item is '%s'\n", list->data);
 list = g_list_insert(list, "Baltimore ", 1);
 printf("The second item is '%s'\n", g_list_next(list)->data);
 list = g_list_remove(list, "Baltimore ");
 printf("After removal of 'Baltimore', the list length is %d\n", g_list_length(list));
 GList* other_list = g_list_append(NULL, "Baltimore ");
 list = g_list_concat(list, other_list);	
 printf("After concatenation: ");
 g_list_foreach(list, (GFunc)printf, NULL);
 list = g_list_reverse(list);	
 printf("\nAfter reversal: ");
 g_list_foreach(list, (GFunc)printf, NULL);
 g_list_free(list);
 return 0;
}
