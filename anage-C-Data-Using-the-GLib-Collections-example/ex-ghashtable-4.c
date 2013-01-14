#include <glib.h>
void value_destroyed(gpointer data) {
 printf("Got a value destroy call for %d\n", GPOINTER_TO_INT(data));
}
gboolean finder(gpointer key, gpointer value, gpointer user_data) {
 return (GPOINTER_TO_INT(key) + GPOINTER_TO_INT(value)) == 42;
}
int main(int argc, char** argv) {
 GHashTable* hash = g_hash_table_new_full(g_direct_hash, g_direct_equal, 
  NULL, 
  (GDestroyNotify)value_destroyed);
 g_hash_table_insert(hash, GINT_TO_POINTER(6), GINT_TO_POINTER(36));
 g_hash_table_insert(hash, GINT_TO_POINTER(10), GINT_TO_POINTER(12));
 g_hash_table_insert(hash, GINT_TO_POINTER(20), GINT_TO_POINTER(22));
 gpointer item_ptr = g_hash_table_find(hash, (GHRFunc)finder, NULL);
 gint item = GPOINTER_TO_INT(item_ptr);
 printf("%d + %d == 42\n", item, 42-item);
 g_hash_table_destroy(hash);
 return 0;
}
