#include <glib.h>
static char* texas_1, *texas_2;
void key_destroyed(gpointer data) {
 printf("Got a key destroy call for %s\n", data == texas_1 ? "texas_1" : "texas_2");
}
int main(int argc, char** argv) {
 GHashTable* hash = g_hash_table_new_full(g_str_hash, g_str_equal, 
  (GDestroyNotify)key_destroyed, NULL);
 texas_1 = g_strdup("Texas");
 texas_2 = g_strdup("Texas");
 g_hash_table_insert(hash, texas_1, "Austin");
 printf("Calling insert with the texas_2 key\n");
 g_hash_table_insert(hash, texas_2, "Houston");
 printf("Calling replace with the texas_2 key\n");
 g_hash_table_replace(hash, texas_2, "Houston");
 printf("Destroying hash, so goodbye texas_2\n");
 g_hash_table_destroy(hash);
 g_free(texas_1);
 g_free(texas_2);
 return 0;
}
