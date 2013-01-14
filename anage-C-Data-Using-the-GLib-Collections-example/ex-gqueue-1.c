#include <glib.h>
int main(int argc, char** argv) {
 GQueue* q = g_queue_new();
 printf("Is the queue empty?  %s, adding folks\n",  g_queue_is_empty(q) ? "Yes" : "No");
 g_queue_push_tail(q, "Alice");
 g_queue_push_tail(q, "Bob");
 g_queue_push_tail(q, "Fred");
 printf("First in line is %s\n", g_queue_peek_head(q));
 printf("Last in line is %s\n", g_queue_peek_tail(q));
 printf("The queue is %d people long\n",  g_queue_get_length(q));
 printf("%s just bought a ticket\n", g_queue_pop_head(q));
 printf("Now %s is first in line\n", g_queue_peek_head(q));
 printf("Someone's cutting to the front of the line\n");
 g_queue_push_head(q, "Big Jim");
 printf("Now %s is first in line\n", g_queue_peek_head(q));
 g_queue_free(q);
 return 0;
}
