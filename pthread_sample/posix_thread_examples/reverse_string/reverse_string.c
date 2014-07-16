/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * reverse_string --
 * 
 * Example showing thread-safe versus thread-unsafe ways to write 
 * a string reversal routine.
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include <pthread.h>

void reverse_string(char*);
void reverse_string_r(char *); 


pthread_mutex_t reverse_string_lock = PTHREAD_MUTEX_INITIALIZER;

#define safe_reverse_string(x) \
pthread_mutex_lock(&reverse_string_lock); \
reverse_string(x); \
pthread_mutex_unlock(&reverse_string_lock);


static char my_buffer[100];

void reverse_string(in_str)
     char *in_str;
{
  int size = 0, i = 0, j = 0;


  /* Find the end of the in_str */
  while ( (in_str[size] != '\0') && (size != 100)) {
    size++;
  }


  /* Copy from in_str into buffer, reversing it */
  for (i = size-1; i > -1; i--) {
    my_buffer[j++] = in_str[i];
  }
  my_buffer[j+1] = '\0';


  /* Copy back from buffer to in_str */
  for (i = 0; i < size+1; i++)
    in_str[i] = my_buffer[i];
}


void reverse_string_r(in_str)
     char *in_str;
{
  int size = 0, i = 0, j = 0;
  char my_auto_buffer[100];

  /* Find the end of the in_str */
  while ( (in_str[size] != '\0') && (size != 100)) {
    size++;
  }

  /* Copy from in_str into buffer, reversing it */
  for (i = size-1; i > -1; i--) {
    my_auto_buffer[j++] = in_str[i];
  }
  my_auto_buffer[j+1] = '\0';


  /* Copy back from buffer to in_str */
  for (i = 0; i < size+1; i++)
    in_str[i] = my_auto_buffer[i];

}

extern int
main(void)
{
  char s1[25], s2[25];
  
  sprintf(s1,"%s", "This is a string");
  sprintf(s2,"%s", "This is another string");

  printf("%s\n",s1);
  reverse_string(s1);
  printf("%s\n\n",s1);


  printf("%s\n",s2);
  reverse_string_r(s2);
  printf("%s\n\n",s2);

  printf("%s\n",s2);
  safe_reverse_string(s2);
  printf("%s\n",s2);

  return 0;
}

