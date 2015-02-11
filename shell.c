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
  int i, j;
  char tempInput[MAX_ELEMENT_LENGTH+1]; // Used to preserve char* input param.
  //char tokenized[MAX_ARGS_AMOUNT][MAX_ELEMENT_LENGTH+1]; // Max of 8 args per command.
  char** tokenized = (char**)malloc(MAX_ARGS_AMOUNT * sizeof(char*));
  for (j = 0; j < MAX_ARGS_AMOUNT; j++)
  {
    tokenized[j] = (char*)malloc((MAX_ELEMENT_LENGTH+1) * sizeof(char));
  }
  strcpy(tempInput, input);
  // Tokenize temp.
  printf("tempInput:%s\n", tempInput);
  char* token = strtok(tempInput, " ");
  for (i = 0;  i < MAX_ARGS_AMOUNT; i++)
  {
    if (token == NULL)
      break;
    printf("token:%s\n", token);
    strcpy(tokenized[i], token);
    printf("a\n");
    token = strtok(NULL, " ");
    printf("tokenized[%d]:%s\n", i, tokenized[i]);
  }
  return tokenized;
}


// The input has no pipes
void pipe0()
{
  // Check if the command is history or exit
  pid_t pid = fork();
  if (pid > 0) // Parent process
  {
    wait(0);
    if (strcmp(cmd[0], "exit") == 0)
      shellExit();
  }
  else if (pid == 0) // Child process
  {
    if (strcmp(cmd[0], "history") == 0) // FIX
    {
      //shellHistory();
      shellExit();
    }
    else if (strcmp(cmd[0], "exit") == 0)
    {
      shellExit();
    }
    else // All other commands
    {

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

}

// The input has two pipes
void pipe2()
{

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
    int x;
    for (x = 0; x < 4; x++)
    {
      printf("cmd[%d]:%s\n", x, cmd[x]);
    }
    //===========================================================================================

    // Which scenario are we in? It is dependent on how many pipes are in the raw input.
    if (pCount == 0);
      //pipe0();
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
    
    char** test = tokenizeInput(cmd[0]);
    printf("cmd[0]:%s\n", cmd[0]);
    int j;
    for (j = 0; j < 8; j++)
    {
      printf("tokenizeInput for cmd[%d]:%s\n", j, test[j]);
    }
    


    //return 0; // TESTAN===========================================================================
  }

  return 0;
}
