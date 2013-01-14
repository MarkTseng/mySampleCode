#include <glib.h>
int main(int argc, char** argv) {
 GArray* a = g_array_new(FALSE, FALSE, sizeof(char*));
 char* first = "hello", *second = "there", *third = "world";
 g_array_append_val(a, first);
 g_array_append_val(a, second);
 g_array_append_val(a, third);
 printf("There are now %d items in the array\n", a->len);
 printf("The first item is '%s'\n", g_array_index(a, char*, 0));
 printf("The third item is '%s'\n", g_array_index(a, char*, 2));
 g_array_remove_index(a, 1);
 printf("There are now %d items in the array\n", a->len);
 g_array_free(a, FALSE);
 return 0;
}
