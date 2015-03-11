#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

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

int simpleFunction1()
{

  FILE *fd1;
  int x;
 
  x = 0;
  x++;

}


int simpleFunction2()
{
  FILE *fd1;
  int x;
 
  x = 0;
  x++;

  fd1 =  fopen("foobar.txt","w");
  fprintf(fd1,"x is %d\n",x);

}

main()
{
  pid_t p;

  struct timeval start_time,finish_time;
  double elapsed_time;


  gettimeofday(&start_time,NULL);
 
      simpleFunction1();

  gettimeofday(&finish_time,NULL);


  elapsed_time = computeElapsedTime(start_time,finish_time);
  
  printf("You have taken %f seconds to compute simpleFunction1()\n",elapsed_time);

  gettimeofday(&start_time,NULL);
 
      simpleFunction2();

  gettimeofday(&finish_time,NULL);

  elapsed_time = computeElapsedTime(start_time,finish_time);
  
  printf("You have taken %f seconds to compute simpleFunction2()\n",elapsed_time);

  gettimeofday(&start_time,NULL);
 
      printf("tadah\n");

  gettimeofday(&finish_time,NULL);

  elapsed_time = computeElapsedTime(start_time,finish_time);
 
  printf("You have taken %f seconds to print\n",elapsed_time);


  gettimeofday(&start_time,NULL);

      p = getpid();

  gettimeofday(&finish_time,NULL);

  printf("You have taken %f seconds to get the process identifier\n",elapsed_time);
}
