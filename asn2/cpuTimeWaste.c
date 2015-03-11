#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

void main(void)
{

  float x = 0.1;
  while( x != 1.05) {
     x++;
  }
}
