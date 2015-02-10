#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_LENGTH 256
#define MAX_ELEMENT_LENGTH 64
#define DEFAULT_STACK_LENGTH 150

// Data structures
typedef struct Node Node;
typedef struct Stack Stack;

/**
 * Node implementation
 */

struct Node
{
  char* value;
  Node *next;
};


/**
* Stack implementation
*/
struct Stack
{
  char contents[DEFAULT_STACK_LENGTH][MAX_INPUT_LENGTH];
  Node* top;
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

void stackInit(Stack* stack)
{
  
}

void stackDestroy(Stack* stack)
{

}

// Push to the stack
void stackPush(Stack* stack, char* val)
{
  Node* node;
  node->value = val;
  printf("val:%s\n", val);
  if (stack->size == 0)
  {
    stack->top = node;
    printf("Top of stack:%s\n", stack->top->value);
    stack->size++;
  }
  else
  {

    node->next = stack->top;
    stack->top = node;
    stack->size++;
  }
}

// Pop the stack
Node stackPop(Stack *stack)
{
  Node temp;
  if (stack->size == 0)
  {
    
    printf("Cannot pop from stack: stack is empty\n");
    exit(EXIT_FAILURE);
  }
  else if (stack->size == 1)
  {
    // Because we keep track of the size as an integer, we can just return the
    // top of the node without removing it as long as we decrement the size 
    // counter.
    temp = *stack->top;
    stack->size--;
    return temp;


  }
  else
  {
    temp = *stack->top;
    stack->top = stack->top->next;
    stack->size--;
    return temp;
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


int parseInput(char* input)
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
  char* temp = strtok(input, " | ");
  for (i = 0; i < pCount+1; i++)
  {
    strcpy(cmd[i],  temp);
    temp = strtok(NULL, " | ");
  }
}

// Terminate the shell.
void shellExit()
{
  
}

// Print the n last lines that were executed in the shell. Default is 10.
void shellHistory(int n)
{
  int i;
  if (n > historyStack.size)
    n = historyStack.size;
  // Iterate through n times.
  Node* temp = historyStack.top;
  printf("shellHistory[0]:%s\n", temp->value);
  for (i = 1; i < n; i++)
  {
    temp = temp->next;
    printf("shellHistory[%d]:%s\n", i, temp->value);
  }
}

void writeToHistory(char* input)
{
  // Create a new pointer to hold the data
  char* temp;
  temp = (char*)malloc((MAX_INPUT_LENGTH+1)*sizeof(char));
  memset(temp, '\0', MAX_INPUT_LENGTH+1);
  strcpy(temp, input);
  // Push the input to the stack
  stackPush(&historyStack, temp);
}


// The input has no pipes
void pipe0()
{

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
    printf("Size of stack: %d\n", historyStack.size);
    printf("Top of stack in main:%s\n", historyStack.top->value);
    shellHistory(5);
    //return 0; // TESTAN===========================================================================
  }

  return 0;
}
