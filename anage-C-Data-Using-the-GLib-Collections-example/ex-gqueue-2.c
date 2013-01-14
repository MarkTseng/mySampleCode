#include <glib.h>
int main(int argc, char** argv) {
 GQueue* q = g_queue_new();
 g_queue_push_tail(q, "Alice");
 g_queue_push_tail(q, "Bob");
 g_queue_push_tail(q, "Fred");
 printf("Queue is Alice, Bob, and Fred; removing Bob\n");
 int fred_pos = g_queue_index(q, "Fred");
 g_queue_remove(q, "Bob");
 printf("Fred moved from %d to %d\n", fred_pos, g_queue_index(q, "Fred"));
 printf("Bill is cutting in line\n");
 GList* fred_ptr = g_queue_peek_tail_link(q);
 g_queue_insert_before(q, fred_ptr, "Bill");
 printf("Middle person is now %s\n", g_queue_peek_nth(q, 1));
 printf("%s is still at the end\n", g_queue_peek_tail(q));
 g_queue_free(q);
 return 0;
}
