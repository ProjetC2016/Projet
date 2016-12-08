#include "tchatche.h"
#include <stdio.h>
#include <stdlib.h>

#define DIRECTORY_LENGTH 1024

void createClient(){
  char *buffer = malloc(DIRECTORY_LENGTH*sizeof(char));
  int client = open("serverPipe", O_WRONLY);
  do{
    printf("> ");
    fgets(buffer, DIRECTORY_LENGTH, stdin);
    write(client, buffer, strlen(buffer));
  }while(strcmp(buffer, "quit") !=0);
}

int main(int argc, char const *argv[]) {
  createClient();
  return 0;
}
