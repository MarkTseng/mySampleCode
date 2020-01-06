#include <stdio.h>
#include <time.h>   /* Needed for struct timespec */


int nsleep(long miliseconds)
{
   struct timespec req, rem;

   if(miliseconds > 999)
   {   
        req.tv_sec = (int)(miliseconds / 1000);                            /* Must be Non-Negative */
        req.tv_nsec = (miliseconds - ((long)req.tv_sec * 1000)); /* Must be in range of 0 to 999999999 */
   }   
   else
   {   
        req.tv_sec = 0;                         /* Must be Non-Negative */
        req.tv_nsec = miliseconds;    /* Must be in range of 0 to 999999999 */
   }   

   return nanosleep(&req , &rem);
}

int main()
{
   int ret = nsleep(1);
   printf("sleep result %d\n",ret);
   return 0;
}

