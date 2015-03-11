#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>
#include <math.h> 


int main(void)
{

   float x = 0.1;
   int policy;
   struct sched_param param;

   policy = SCHED_FIFO;
   param.sched_priority = sched_get_priority_max(policy);

  if(sched_setscheduler(0,policy,&param)){
    perror("Error setting scheduler policy");
    exit(EXIT_FAILURE);
  }
  while( x != 1.05) {
     x++;
  }
}
