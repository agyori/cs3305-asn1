#include <string.h>
#include <stdio.h>
#include <stdlib.h>



void receiveInput(char* a)
{
  //char input[512];
  //memset(input, '\0', 512);
  fgets(a, 512, stdin);
}




int main()
{
  /*
  //char* a = (char*)malloc(512*sizeof(char));
  char a[512];
  memset(a, '\0', 512);
  receiveInput(a);
  printf("a:%s\n", a);
  */





  
  char s[256];
  strcpy(s, "AAAA a | BBBB b | CCCC c | DDDD d");
  //char* token = strtok(s, "|");
  /*
  while (token) {
    printf("token:%s\n", token);
    printf("s:%s\n", s);
    token = strtok(NULL, "|");
  }
  */
  
  int i;
  char  cmd[4][64];
  //char* temp = (char*)malloc(64*sizeof(char));
  char* temp = strtok(s, "|");
  for (i = 0; i < 4; i++)
  {
    strcpy(cmd[i], temp);
    temp = strtok(NULL, "|");
    printf("cmd[%d]:%s\n", i, cmd[i]);
  }
  
}
