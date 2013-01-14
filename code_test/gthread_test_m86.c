#include <glib.h>

void *print_message_function( void *ptr );

int main()
{
  GThread          *Thread1, *Thread2;
  char *message1 = "Thread 1";
  char *message2 = "Thread 2";
  GError           *err1 = NULL ;
  GError           *err2 = NULL ;

  if( !g_thread_supported() )
  {
     g_thread_init(NULL);
     printf("g_thread supported\n");
  }
  else
  {
     printf("g_thread NOT supported\n");
  }

  if( (Thread1 = g_thread_create((GThreadFunc)print_message_function, (void *)message1, TRUE, &err1)) == NULL)
  {
     printf("Thread create failed: %s!!\n", err1->message );
     g_error_free ( err1 ) ;
  }

  if( (Thread2 = g_thread_create((GThreadFunc)print_message_function, (void *)message2, TRUE, &err2)) == NULL)
  {
     printf("Thread create failed: %s!!\n", err2->message );
     g_error_free ( err2 ) ;
  }

  g_thread_join(Thread1);
  g_thread_join(Thread2);

  return 0;
}

void *print_message_function( void *ptr )
{
     char *message;
     g_usleep(1000000);
     message = (char *) ptr;
     printf("%s \n", message);
     g_usleep(1000000);
     printf("%s \n", message);
}

