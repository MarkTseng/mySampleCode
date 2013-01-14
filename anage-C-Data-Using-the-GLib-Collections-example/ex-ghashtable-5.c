#include <glib.h>
gboolean wide_open(gpointer key, gpointer value, gpointer user_data) {
 return TRUE;
}
void key_destroyed(gpointer data) {
 printf("Got a GDestroyNotify callback\n");
}
int main(int argc, char** argv) {
 GHashTable* hash = g_hash_table_new_full(g_str_hash, g_str_equal, 
  (GDestroyNotify)key_destroyed, 
  (GDestroyNotify)key_destroyed);
 g_hash_table_insert(hash, "Texas", "Austin");
 g_hash_table_insert(hash, "Virginia", "Richmond");
 g_hash_table_insert(hash, "Ohio", "Columbus");
 g_hash_table_insert(hash, "Oregon", "Salem");
 g_hash_table_insert(hash, "New York", "Albany");
 printf("Removing New York, you should see two callbacks\n");
 g_hash_table_remove(hash, "New York");
 if (g_hash_table_steal(hash, "Texas")) {
  printf("Texas has been stolen, %d items remaining\n", g_hash_table_size(hash));
 }
 printf("Stealing remaining items\n");
 g_hash_table_foreach_steal(hash, (GHRFunc)wide_open, NULL);
 printf("Destroying the GHashTable, but it's empty, so no callbacks\n");
 g_hash_table_destroy(hash);
 return 0;
}
