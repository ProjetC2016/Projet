#include "tchatche.h"
#include <stdio.h>
#include <stdlib.h>

#define DIRECTORY_LENGTH 1024

void createClient(){
  char *buffer = malloc(DIRECTORY_LENGTH*sizeof(char));
  char *pseudo = malloc(DIRECTORY_LENGTH*sizeof(char));

  int client = open("serverPipe", O_WRONLY);
  do{
    printf("Pseudo> ");
    fgets(buffer, DIRECTORY_LENGTH, stdin);
    if(access(buffer, F_OK) == -1){
      mkfifo(buffer, 0666);
    }
    char* intel = malloc((12+2*strlen(buffer))*sizeof(char));
    //if(8+2*strlen(buffer)<=9999){
      //sprintf(intel,"%4d%s%s%s",8+)
    }

    write(client, buffer, strlen(buffer));
  }while(strcmp(buffer, "quit") !=0);
  strcpy(pseudo, buffer)
}

int main(int argc, char const *argv[]) {
  createClient();
  return 0;
}

//sprintf(message, "%4d,...,...,...", l, )

// int i;
// char* number = malloc(8*sizeof(char));
// char* l = malloc
// int a = 8 + 2*strlen(buffer);
// sprintf(number,"%d", a);
// for(i=3; i>=strlen(number); i--){
//   l[i] = number[strlen(number)-4+i];
// }
// for(i=0; i<strlen(number); i++){
//   l[i] = 0 ;
// }
