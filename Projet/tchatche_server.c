#include "tchatche.h"
#include <stdio.h>
#include <stdlib.h>

#define DIRECTORY_LENGTH 1024

void createPipe(){
  if(access("serverPipe", F_OK) == -1){
    mkfifo("serverPipe", 0666);
  }
}

void createServer(){
  createPipe();
  char *buffer = malloc(DIRECTORY_LENGTH*sizeof(char));
  int server = open("serverPipe", O_RDONLY);
  do{
    read(server, buffer, DIRECTORY_LENGTH);
    printf("%s\n", buffer);
  }while(strcmp(buffer, "quit") != 0);
}

int main(int argc, char const *argv[]) {
  createServer();
  return 0;
}
