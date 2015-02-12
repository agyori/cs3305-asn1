#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_LENGTH 256
#define MAX_ELEMENT_LENGTH 64
#define DEFAULT_STACK_LENGTH 150
#define MAX_ARGS_AMOUNT 8

// Data structures
typedef struct Node Node;
typedef struct Stack Stack;


/**
* Stack implementation
*/
struct Stack
{
  //char** contents;
  char contents[DEFAULT_STACK_LENGTH][MAX_INPUT_LENGTH];
  int size;
};

/**
 * Global variables
 */
char* username; // Hold the username
char cmd[4][MAX_ELEMENT_LENGTH]; // Holds input for 3 possible pipes and 128 
                  // characters per command (ex. A|B|C|D)
Stack historyStack;


/** 
* Stack  functions
*/

void stackPush(Stack* stack, char* val)
{
  int n = stack->size;
  if (n != DEFAULT_STACK_LENGTH)
  {
    strcpy(stack->contents[n], val);
    stack->size++;
  }
  else
  {
    perror("Cannot push to stack: stack is full.");
  }
}



void clearCMD()
{
  memset(cmd[0], '\0', MAX_ELEMENT_LENGTH);
  memset(cmd[1], '\0', MAX_ELEMENT_LENGTH);
  memset(cmd[2], '\0', MAX_ELEMENT_LENGTH);
  memset(cmd[3], '\0', MAX_ELEMENT_LENGTH);
}

// Get the input and return it.
void receiveInput(char* input)
{
  printf("%s>", username);
  fgets(input, MAX_INPUT_LENGTH, stdin);
}
  
int pipeCount(char* input)
{
  // Check if there are greater than 3 pipes
  int i;
  int pCount = 0;
  for (i = 0; i < MAX_INPUT_LENGTH; i++)
  {
    if (input[i] == '|')
      pCount++; 
  }
  return pCount;
}


void parseInput(char* input)
{
  int i;
  // Check if there are 3 or less pipes.
  int pCount = pipeCount(input);
  if (pCount > 3) // We do not support more than 3 pipes.
  {
    perror("Shell does not support more than 3 pipes.\n");
    exit(EXIT_FAILURE);
  }

  // Load all the arguments into cmd
  char* temp = strtok(input, "|");
  for (i = 0; i < pCount+1; i++)
  {
    strcpy(cmd[i],  temp);
    temp = strtok(NULL, "|");
  }
}

// Terminate the shell.
void shellExit()
{
  exit(EXIT_SUCCESS);
}



// Print the n last lines that were executed in the shell. Default is 10.
void shellHistory(int n)
{
  int i;
  if (n > historyStack.size)
    n = historyStack.size;
  // Iterate through n times.
  for (i = historyStack.size - 1; i >= historyStack.size - n; i--)
  {
    printf("shellHistory[%d]:%s\n", i, historyStack.contents[i]);
  }
}

void shellHistoryDefault()
{
  shellHistory(10);
}

void writeToHistory(char* input)
{
  stackPush(&historyStack, input);
}



// Tokenizes a string according to spaces. This does not modify the original string.
char** tokenizeInput(char* input)
{
  printf("input:%s\n", input);
  int i, j, k, elementcount;
  char tempInput[MAX_ELEMENT_LENGTH+1]; // Used to preserve char* input param.
  char countInput[MAX_ELEMENT_LENGTH+1];
  
  // Copy the input into the temp string and replace all newline chacacters with null terminators.
  strcpy(tempInput, input);
  for(k = 0; k < MAX_ELEMENT_LENGTH+1; k++)
    if (tempInput[k] == '\n')
      tempInput[k] = '\0';
  strcpy(countInput, tempInput);

  // Count how many elements there are in the input
  elementcount = 0;
  char* tempCountToken = strtok(countInput, " ");
  while (tempCountToken)
  {
    elementcount++;
    printf("countToken[%d-1]:%s\n", elementcount,  tempCountToken);
    
    tempCountToken = strtok(NULL, " "); 
  }
  printf("There are %d elements.\n", elementcount);
  // Initialize the array we will return.
  char** tokenized = (char**)calloc(MAX_ARGS_AMOUNT, sizeof(char*));
  for (j = 0; j < elementcount; j++)
  {
    tokenized[j] = (char*)calloc((MAX_ELEMENT_LENGTH+1), sizeof(char));
  }

  // Tokenize temp.
  printf("tempInput:%s\n", tempInput);
  char* token = strtok(tempInput, " ");
  for (i = 0;  i < MAX_ARGS_AMOUNT; i++)
  {
    if (token == NULL)
      break;
    printf("token:%s\n", token);
    strcpy(tokenized[i], token);
    token = strtok(NULL, " ");
    printf("tokenized[%d]:%s\n", i, tokenized[i]);
  }
  return tokenized;
}


// The input has no pipes
void pipe0()
{
  // Tokenize the input.
  char** proc1 = tokenizeInput(cmd[0]);
  printf("proc1[0]:%s\n", proc1[0]);
  if (strcmp(proc1[0], "exit") == 0) // Check for the "exit" built-in command.
    shellExit();
  pid_t pid = fork();
  if (pid > 0) // Parent process
  {
    wait(0);
  }
  else if (pid == 0) // Child process
  {
    if (strcmp(proc1[0], "history") == 0) // Check for the "history" built-in command.
    {
      // Check the argument.
      if (strcmp(proc1[1], "") == 0)
      {
        shellHistoryDefault();
      }
      else
      {
        int x = atoi(proc1[1]);
        shellHistory(x);
      }
      exit(EXIT_SUCCESS);
    }
    else // All other commands
    {
      if (execvp(proc1[0], proc1) < 0);
      {
        perror("Could not exec.\n");
        exit(EXIT_FAILURE);
      }
    }
  }
  else // Fork failed
  {
    perror("Fork failed.\n");
  }
}

// The input has one pipe
void pipe1()
{
  int fds[2];
  pid_t pid1;
  pid_t pid2;
  // Tokenize the input
  char** proc1 = tokenizeInput(cmd[0]);
  char** proc2 = tokenizeInput(cmd[1]);

  // Pipe
  //if (pipe(fds) < 0)
  //  perror("Could not pipe");

  printf("FIRST FORK!\n");
  // Initial fork
  if ((pid1 = fork()) < 0)
  {
    perror("Could not perform initial fork");
    exit(EXIT_FAILURE);
  }
  else if (pid1 > 0) // Parent process waits for child to finish
    wait(0);
  else // Child process forks again and pipes
  {
    printf("SECOND FORK!\n");
    printf("pid1 child:%d\n", pid1);
    // Pipe
    if (pipe(fds) < 0)
      perror("Could not pipe");

    
    // Second Fork
    if ((pid2 = fork()) < 0) // Fork and check for error.
    {
      perror("Could not fork");
      exit(EXIT_FAILURE);
    }
    else if (pid2 > 0) // Parent process
    {
      printf("pid2 parent:%d\n", pid2);
      close(fds[1]); // Close read end
      // Close stdout, redirect to the writing end of the pipe.
      if (dup2(fds[0], 0) < 0)
      {
        perror("Could not dup2");
        exit(EXIT_FAILURE);
      }
      wait(0);
      execvp(proc2[0], proc2);
      perror("Could not exec");
      exit(EXIT_FAILURE);
    }
    else // Child process
    {
      printf("pid2 child:%d\n", pid2);
      close(fds[0]);
      if (dup2(fds[1], 1) < 0)
      {
        perror("Could not dup2");
        exit(EXIT_FAILURE);
      }
      execvp(proc1[0], proc1);
      perror("Could not exec");
      exit(EXIT_FAILURE);
    }
  }
}

// The input has two pipes
void pipe2()
{
  int fds1[2];
  int fds2[2];
  pid_t pid1;
  pid_t pid2;
  pid_t pid3;
  // Tokenize the input
  char** proc1 = tokenizeInput(cmd[0]);
  char** proc2 = tokenizeInput(cmd[1]);
  char** proc3 = tokenizeInput(cmd[2]);

  
  
  
  
  
  printf("FIRST FORK!\n");
  // Initial fork
  if ((pid1 = fork()) < 0)
  {
    perror("Could not perform initial fork");
    exit(EXIT_FAILURE);
  }
  else if (pid1 > 0) // Parent process waits for child to finish
    wait(0);











  else // Child process forks again and pipes
  {
    printf("SECOND FORK!\n");
    printf("pid1 child:%d\n", pid1);
    // Pipe
    if (pipe(fds1) < 0)
      perror("Could not pipe");

    
    // Second Fork
    if ((pid2 = fork()) < 0) // Fork and check for error.
    {
      perror("Could not fork");
      exit(EXIT_FAILURE);
    }
    else if (pid2 > 0) // Parent process
    {
      printf("pid2 parent:%d\n", pid2);
      close(fds1[0]); // Close read end
      // Close stdout, redirect to the writing end of the pipe.
      if (dup2(fds1[1], 1) < 0)
      {
        perror("Could not dup2");
        exit(EXIT_FAILURE);
      }
      execvp(proc1[0], proc1);
      perror("Could not exec");
      exit(EXIT_FAILURE);
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    else // Child process forks again and pipes
    {
      // Pipe
      if (pipe(fds2) < 0)
        perror("Could not pipe");
      
      printf("THIRD FORK!\n");
      printf("pid2 child:%d\n", pid2);
      // Third Fork
      if ((pid3 = fork()) < 0)
      {
        perror("Could not fork");
        exit(EXIT_FAILURE);
      }
      else if (pid3 > 0) // Parent process
      {       
        close(fds1[1]); // Close write end of first pipe
        close(fds2[0]); // Close read end of second pipe
        if (dup2(fds1[0], fds2[1]) < 0) //
        {
          perror("Could not dup2");
          exit(EXIT_FAILURE);
        } 
        execvp(proc2[0], proc2);
        perror("Could not exec");
        exit(EXIT_FAILURE);  
      }
      else // Child process
      {
        printf("pid3 child:%d\n", pid3);
        close(fds2[1]);
        if (dup2(fds2[0], 0) < 0)
        {
          perror("Could not dup2");
          exit(EXIT_FAILURE);
        }
        execvp(proc3[0], proc3);
        perror("Could not exec");
        exit(EXIT_FAILURE);
      }
    }
  }
}

// The input has three pipes
void pipe3()
{

}






// Go through each command and set the pipes where appropriate.
void setPipes()
{
  int fd[2]; // Holds the read/write ends of the pipe.
  pipe(fd);
  pid_t pid;
  pid = fork();

  if (pid < 0) // Error: could not fork
  {
    exit(EXIT_FAILURE);
  }
  else if (pid > 0) // Parent process
  {
    
  }
  else // Child process
  {
    
  }

}


/**
void parseInput(char* input)
{
  // Fork a new process.
  pid_t pid;
  int status = 0;
  pid = fork();
  if (pid < 0) // Error: could not fork
  {
    perror("Could not fork()\n");
  }
  if (pid > 0) // Parent process
  {
    wait(0); // Terminates the parent when the child terminates
  }
  else // Child process
  {
    // Check if there are any more pipes
    char* s;
    s = strchr(input, '|');
    if (s == NULL) // There are no more pipes
    {
      
    }
    else // There exists one or more pipes
    {
      // parseInput for the leftmost part of the command.
      // also parseInput for the rest of the command (excluding the
      // leftmost pipe)
    }
  }
  return;
}
*/

int main (int argc, char** argv)
{
  char input[MAX_INPUT_LENGTH+1]; // Holds the raw input
  int pCount;
  // Check if we were given a username
  if (argc == 1)
  {
    perror("Usage: asn1sh [USERNAME]\n");
    return 1;
  }
  else
  {
    username = argv[1];
  }

  // Wait for input forever. Only terminate the program when
  // there is 
  while (1)
  {
    // Clear the raw input and CMD
    memset(input, '\0', MAX_INPUT_LENGTH+1);
    clearCMD();
    // Get the raw input and store it in input
    receiveInput(input);
    // Record the raw input to the history stack.
    writeToHistory(input);
    // Count how many pipes there are in the raw input. We do this now because parseInput alters the raw input
    pCount = pipeCount(input);
    // Parse the input and store it in the global variable CMD
    parseInput(input);
    

    // TESTAN====================================================================================
    // Print the contents of cmd
    //int x;
    //for (x = 0; x < 4; x++)
    //{
    //  printf("cmd[%d]:%s\n", x, cmd[x]);
    //}
    //===========================================================================================

    // Which scenario are we in? It is dependent on how many pipes are in the raw input.
    if (pCount == 0)
      pipe0();
    else if (pCount == 1)
      pipe1();
    else if (pCount == 2)
      pipe2();
    else if (pCount == 3)
      pipe3();
    else
    {
      perror("This shell does not support more than 3 pipes.");
      exit(EXIT_FAILURE);
    }
    
    /**
    char** test = tokenizeInput(cmd[0]);
    printf("cmd[0]:%s\n", cmd[0]);
    int j;
    for (j = 0; j < 8; j++)
    {
      printf("tokenizeInput for cmd[%d]:%s\n", j, test[j]);
    }
    */


    //return 0; // TESTAN===========================================================================
  }

  return 0;
}
