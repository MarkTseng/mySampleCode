#include <glib.h>
int main(int argc, char** argv) {
 GArray* a = g_array_sized_new(TRUE, TRUE, sizeof(int), 16);
 printf("Array preallocation is hidden, so array size == %d\n", a->len);
 printf("Array was init'd to zeros, so 3rd item is = %d\n", g_array_index(a, int, 2));
 g_array_free(a, FALSE);

 // this creates an empty array, then resizes it to 16 elements
 a = g_array_new(FALSE, FALSE, sizeof(char));
 g_array_set_size(a, 16);
 g_array_free(a, FALSE);

 a = g_array_new(FALSE, FALSE, sizeof(char));
 char* x = g_strdup("hello world");
 g_array_append_val(a, x);
 g_array_free(a, TRUE);

 return 0;
}
