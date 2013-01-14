#include <glib.h>
gboolean iter(GNode* n, gpointer data) {
 printf("%s ", n->data);
 return FALSE; 
}
int main(int argc, char** argv) {
 GNode* root = g_node_new("Atlanta");
 g_node_append(root, g_node_new("Detroit"));
 GNode* portland = g_node_prepend(root, g_node_new("Portland"));
 printf(">Some cities to start with\n");
 g_node_traverse(root, G_PRE_ORDER, G_TRAVERSE_ALL, -1, iter, NULL);
 printf("\n>Inserting Coos Bay before Portland\n");
 g_node_insert_data_before(root, portland, "Coos Bay");
 g_node_traverse(root, G_PRE_ORDER, G_TRAVERSE_ALL, -1, iter, NULL);
 printf("\n>Reversing the child nodes\n");
 g_node_reverse_children(root);
 g_node_traverse(root, G_PRE_ORDER, G_TRAVERSE_ALL, -1, iter, NULL);
 printf("\n>Root node is %s\n", g_node_get_root(portland)->data);
 printf(">Portland node index is %d\n", g_node_child_index(root, "Portland"));
 g_node_destroy(root);
 return 0;
}
