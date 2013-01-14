#include <glib.h>
int main(int argc, char** argv) {
 GQueue* q = g_queue_new();
 g_queue_push_tail(q, "Alice ");
 g_queue_push_tail(q, "Bob ");
 g_queue_push_tail(q, "Fred ");
 g_queue_push_tail(q, "Jim ");
 printf("Starting out, the queue is: ");
 g_queue_foreach(q, (GFunc)printf, NULL);
 GList* fred_link = g_queue_peek_nth_link(q, 2);
 printf("\nThe link at index 2 contains %s\n", fred_link->data);
 g_queue_unlink(q, fred_link);
 g_list_free(fred_link);
 GList* jim_link = g_queue_peek_nth_link(q, 2);
 printf("Now index 2 contains %s\n", jim_link->data);
 g_queue_delete_link(q, jim_link);
 printf("Now the queue is: ");
 g_queue_foreach(q, (GFunc)printf, NULL);
 g_queue_free(q);
 return 0;
}
