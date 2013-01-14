#include <glib.h>
#include <stdio.h>
int main(int argc, char** argv) {
 GPtrArray* a = g_ptr_array_new();
 g_ptr_array_add(a, g_strdup("hello "));
 g_ptr_array_add(a, g_strdup("again "));
 g_ptr_array_add(a, g_strdup("there "));
 g_ptr_array_add(a, g_strdup("world "));
 g_ptr_array_add(a, g_strdup("\n"));
 printf(">Here are the GPtrArray contents\n");
 g_ptr_array_foreach(a, (GFunc)printf, NULL);
 printf(">Removing the third item\n");
 g_ptr_array_remove_index(a, 2);
 g_ptr_array_foreach(a, (GFunc)printf, NULL);
 printf(">Removing the second and third item\n");
 g_ptr_array_remove_range(a, 1, 2);
 g_ptr_array_foreach(a, (GFunc)printf, NULL);
 printf("The first item is '%s'\n", g_ptr_array_index(a, 0));
 g_ptr_array_free(a, TRUE);
 return 0;
}
