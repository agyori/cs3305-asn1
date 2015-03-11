/* Program to compute Pi using Monte Carlo methods */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <sched.h>
#define SEED 35791246



/*This function finds the difference between two times */

double computeElapsedTime(struct timeval start_time,
                          struct timeval finish_time)

{
  
  double start_count, end_count, elapsed_time;

  start_count =
     (double)start_time.tv_sec+1.e-6*(double)start_time.tv_usec;
  end_count =
     (double)finish_time.tv_sec+1.e-6*(double)finish_time.tv_usec;

  elapsed_time = (end_count - start_count);

  return elapsed_time;
}



void computePi(int niter)
{
   int count, i;
   double x, y, z, pi;
   /* initialize random numbers */
   srand(SEED);
   count=0;
   for ( i=0; i<niter; i++) {
      x = (double)rand()/RAND_MAX;
      y = (double)rand()/RAND_MAX;
      z = x*x+y*y;
      if (z<=1) count++;
      }
   pi=(double)count/niter*4;
   printf("#trials = %d, pi = %g\n",niter,pi);
}


void main(int argc, char **argv)
{
   int niter;
   double x,y;
   int i,count=0; /* # of points in the 1st quadrant of unit circle */
   double z;
   double pi;
   int numChildren, policy;
   struct sched_param param;
   pid_t tempID;

   struct timeval start_time, finish_time;
   double elapsed_time;
   if (argc != 3){
     perror("incorrect arguments");
     exit(0);
   }



   niter = atoi(argv[1]);  
   numChildren = atoi(argv[2]);

   
   // for loop to create children
   for (i = 0; i < numChildren; i++)
   {
     if (i % 2 == 0) // i is even
     {
       tempID = fork();
       // SCHED_OTHER
       if (tempID > 0) // Parent
       {
         // Do nothing
         //wait(0);
       }
       else // Child
       {
         // Set up
         policy = SCHED_OTHER;
         param.sched_priority = sched_get_priority_max(policy);
         if (sched_setscheduler(0, policy, &param))
         {
           perror("Error setting schedukler policy");
           exit(EXIT_FAILURE);
         }
         // Compute pi and time how long it took.
         gettimeofday(&start_time, NULL);
         computePi(niter);
         gettimeofday(&finish_time, NULL);
         elapsed_time = computeElapsedTime(start_time, finish_time);
         printf("\t\t\tOTHER: %fs\n", elapsed_time);
         return;
       }
     }
     else // i is odd
     {
       tempID = fork();
       // SCHED_FIFO
       if (tempID > 0) // Parent
       {
         // Do nothing
         //wait(0);
       }
       else // Child
       {
         // Set up
         policy = SCHED_FIFO;
         param.sched_priority = sched_get_priority_max(policy);
         if (sched_setscheduler(0, policy, &param))
         {
           perror("Error setting scheduler policy");
           exit(EXIT_FAILURE);
         }
         // Compute pi and see how long it took.
         gettimeofday(&start_time, NULL);
         computePi(niter);
         gettimeofday(&finish_time, NULL);
         elapsed_time = computeElapsedTime(start_time, finish_time);
         printf("\t\t\tFIFO:  %fs\n", elapsed_time);
         return;
       }
     }
   }
   return;
}


