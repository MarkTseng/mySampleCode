#include <glib.h>
void prt(GArray* a) {
 printf("Array holds: ");
 int i;
 for (i = 0; i < a->len; i++) 
  printf("%d ", g_array_index(a, int, i));
 printf("\n");
}
int main(int argc, char** argv) {
 GArray* a = g_array_new(FALSE, FALSE, sizeof(int));
 int x[2] = {1,5};
 g_array_append_vals(a, &x, 2);
 prt(a);
 printf("Inserting a '2'\n");
 int b = 2;
 g_array_insert_val(a, 1, b);
 prt(a);
 printf("Inserting multiple values\n");
 int y[2] = {3,4};
 g_array_insert_vals(a, 2, y, 2);
 prt(a);
 g_array_free(a, FALSE);
 return 0;
}
