#include <glib.h>
int main(int argc, char** argv) {
 GRelation* r = g_relation_new(2);
 g_relation_index(r, 0, g_str_hash, g_str_equal);
 g_relation_index(r, 1, g_str_hash, g_str_equal);
 g_relation_insert(r, "Virginia", "Richmond");
 g_relation_insert(r, "New Jersey", "Trenton");
 g_relation_insert(r, "New York", "Albany");
 g_relation_insert(r, "Virginia", "Farmville");
 g_relation_insert(r, "Wisconsin", "Madison");
 g_relation_insert(r, "Virginia", "Keysville");
 GTuples* t = g_relation_select(r, "Virginia", 0);
 printf("Some cities in Virginia:\n");
 int i;
 for (i=0; i < t->len; i++) {
 	printf("%d) %s\n", i, g_tuples_index(t, i, 1));
 }
 g_tuples_destroy(t);
 t = g_relation_select(r, "Vermont", 0);
 printf("Number of Vermont cities in the GRelation: %d\n", t->len);
 g_tuples_destroy(t);
 g_relation_destroy(r);
 return 0;
}
