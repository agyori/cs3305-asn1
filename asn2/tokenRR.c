
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>
#include <sched.h>
#include <math.h> 



#define MAX 256
#define CMD_MAX 10

/*
   This function takes as input the following:
     buf: This represents the string for which the tokens are to be determined
          for

     tokens: This represents the array that the found tokens are tbe put into

   The function strtok() is used to find the tokens.  The delimiter used
   to distinguish tokens is a space

*/

int make_tokenlist(char *buf, char *tokens[])
{

 char input_line[MAX];
 char *line;
 int i,n;

 i = 0;

 line =   buf;
 tokens[i] = strtok(line, " ");
 do  {
    i++;
    line = NULL;
    tokens[i] = strtok(line, " ");
 } while(tokens[i] != NULL);

 return i;
}


/*
  The purpose of the main program is to illustrate the usage of
  the make_tokenlist function

*/

void main(void) 
{

   char input_line[MAX], *tokens[CMD_MAX];
   int i, n; 
  
   // Set the policy to be using SCHED_RR
   int policy;
   struct sched_param param;

   policy = SCHED_RR;
   param.sched_priority = sched_get_priority_max(policy);

  if(sched_setscheduler(0,policy,&param)){
    perror("Error setting scheduler policy");
    exit(EXIT_FAILURE);
  }








   printf("hanan> ");
   if (fgets(input_line,MAX,stdin) != NULL) 
     n= make_tokenlist(input_line, tokens);
   else 
     printf("huh?\n");

   for (i = 0; i < n; i++)
     printf("extracted token is %s\n", tokens[i]);

  
}
