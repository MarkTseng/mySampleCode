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
 gboolean found = g_relation_exists(r, "New York", "Albany");
 printf("New York %s found in the relation\n", found ? "was" : "was not");
 gint count = g_relation_count(r, "Virginia", 0);
 printf("Virginia appears in the relation %d times\n", count);
 g_relation_destroy(r);
 return 0;
}
