#include <glib.h>
int main(int argc, char** argv) {
 GQueue* q = g_queue_new();
 g_queue_push_tail(q, "Alice ");
 g_queue_push_tail(q, "Bob ");
 g_queue_push_tail(q, "Fred ");
 printf("Starting out, the queue is: ");
 g_queue_foreach(q, (GFunc)printf, NULL);
 g_queue_reverse(q);
 printf("\nAfter reversal, it's: ");
 g_queue_foreach(q, (GFunc)printf, NULL);
 GQueue* new_q = g_queue_copy(q);
 g_queue_reverse(new_q);
 printf("\nNewly copied and re-reversed queue is: ");
 g_queue_foreach(new_q, (GFunc)printf, NULL);
 g_queue_free(q);
 g_queue_free(new_q);
 return 0;
}
