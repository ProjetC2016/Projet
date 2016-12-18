#include <stdio.h>
#include <stdlib.h>
#include "tchatche.h"

#define DIRECTORY_LENGTH 1024

int client = 0;
int id = 0;

void createClient(){
  int server = open("serverPipe", O_WRONLY); //on ouvre le tube server en écriture
  char *buffer = malloc(DIRECTORY_LENGTH*sizeof(char));
  char *pseudo = malloc(DIRECTORY_LENGTH*sizeof(char));
  printf("Pseudo> ");
  fgets(buffer, DIRECTORY_LENGTH, stdin); //on récupère le pseudo
  strncpy(pseudo,buffer,strlen(buffer)-1); //on le stocke dans la variable correspondante
  pseudo[strlen(buffer)-1]='\0';
  if(access(pseudo, F_OK) == -1){ //si le fichier n'est pas crée
    mkfifo(pseudo, 0666); //je le crée
  }
  char* intel = malloc((8+2*strlen(pseudo)+1)*sizeof(char)); //infos envoyées au serveur
  sprintf(intel,"%4d%s%s%s",8+2*strlen(pseudo),"HELO",pseudo,pseudo); //on crée l'intel CONNEXION correspondant
  intel[8+2*strlen(pseudo)]='\0';
  printf("J'envoie : %s\n",intel);
  write(server, intel, 8+2*strlen(pseudo)); //on l'envoie au server
  client = open(pseudo, O_RDONLY); //on ouvre son propre tube en lecture

  char* recu = malloc(12*sizeof(char)); //message recu du serveur
  char* idC = malloc(4*sizeof(char)); //String id
  read(client, recu, DIRECTORY_LENGTH); //on lit la tube
  strncpy(idC,recu+8,4); //on le stocke dans idC
  id = atoi(idC); //on le transforme pour récupérer l'id
  printf("Connected. ID : %d\n",id);
  free(buffer); //et on free !
  free(pseudo);
  free(intel);
  free(recu);
  free(idC);
}

int main(int argc, char const *argv[]) {
  createClient(); //fonction de connexion
  while(1){} //TODO: A remplacer avec un mainClient qui gère l'envoi des messages
  return 0;
}
