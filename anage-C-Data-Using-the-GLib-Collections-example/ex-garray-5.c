#include <glib.h>
void prt(GArray* a) {
 int i;
 printf("Array holds: ");
 for (i = 0; i < a->len; i++) 
  printf("%d ", g_array_index(a, int, i));
 printf("\n");
}
int main(int argc, char** argv) {
 GArray* a = g_array_new(FALSE, FALSE, sizeof(int));
 int x[6] = {1,2,3,4,5,6};
 g_array_append_vals(a, &x, 6);
 prt(a);
 printf("Removing the first item\n");
 g_array_remove_index(a, 0);
 prt(a);
 printf("Removing the first two items\n");
 g_array_remove_range(a, 0, 2);
 prt(a);
 printf("Removing the first item very quickly\n");
 g_array_remove_index_fast(a, 0);
 prt(a);
 g_array_free(a, FALSE);
 return 0;
}
