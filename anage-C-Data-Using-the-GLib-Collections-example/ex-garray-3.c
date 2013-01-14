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
 printf("Array is empty, so appending some values\n");
 int x[2] = {4,5};
 g_array_append_vals(a, &x, 2);
 prt(a);
 printf("Now to prepend some values\n");
 int y[2] = {2,3};
 g_array_prepend_vals(a, &y, 2);
 prt(a);
 printf("And one more prepend\n");
 int z = 1;
 g_array_prepend_val(a, z);
 prt(a);
 g_array_free(a, FALSE);
 return 0;
}
