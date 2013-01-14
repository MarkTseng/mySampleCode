#include <glib.h>
int main(int argc, char** argv) {
 GHashTable* hash = g_hash_table_new(g_str_hash, g_str_equal);
 g_hash_table_insert(hash, "Texas", "Austin");
 g_hash_table_insert(hash, "Virginia", "Richmond");
 g_hash_table_insert(hash, "Ohio", "Columbus");
 char* state = NULL;
 char* capital = NULL;
 char** key_ptr = &state;
 char** value_ptr = &capital;
 gboolean result = g_hash_table_lookup_extended(hash, "Ohio", (gpointer*)key_ptr, (gpointer*)value_ptr);
 if (result) {
  printf("Found that the capital of %s is %s\n", capital, state);
 }
 if (!g_hash_table_lookup_extended(hash, "Vermont", (gpointer*)key_ptr, (gpointer*)value_ptr)) {
  printf("Couldn't find Vermont in the hash table\n");
 } 
 g_hash_table_destroy(hash);
 return 0;
}
