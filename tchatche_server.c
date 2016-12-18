#include <stdio.h>
#include <stdlib.h>
#include "tchatche_server.h"

#define DIRECTORY_LENGTH 1024

int id = 1;; //id du client (augmente de 1 à chaque ajout d'un nouveau client)

/* Fonction qui crée le utbe ud server */
void createServer(){
  if(access("serverPipe", F_OK) == -1){ //si le tube server n'existe pas
    mkfifo("serverPipe", 0666); //je le crée
  }
}

/* Fonction de connexion : récupère l'intel de connexion et offre un id */
void connexion(char* buffer, int l){
  char* pseudo = malloc(((l-8)/2)*sizeof(char)); //string pour le pseudo
  strncpy(pseudo,buffer+8,(l-8)/2); //on le récupère dans le buffer
  pseudo[((l-8)/2)]='\0';
  printf("Pseudo : %s\n",pseudo);
  int client = open(pseudo, O_WRONLY); //on ouvre le tube client en écriture
  char* intel = malloc(13*sizeof(char)); //string pour l'intel envoyé
  sprintf(intel,"%4d%s%4d",12,"OKOK",id); //on crée l'intel
  intel[12]='\0';
  write(client, intel, strlen(intel));//on écrit l'intel dans le tube client
  id++; //on augmente l'id pour le prochain tour
  free(pseudo); //on free ! (on a tout compris)
  free(intel);
}
void mainServer(){
  int server = open("serverPipe", O_RDONLY); //ouverture du tube server en lecture
  char *buffer = malloc(DIRECTORY_LENGTH*sizeof(char)); //buffer
  char *type = malloc(5*sizeof(char));  //string pour le type des messages
  char* lC = malloc(5*sizeof(char)); //longueur de l'intel
  while(1){
    printf("En attente d'un message !\n");
    read(server, buffer, DIRECTORY_LENGTH); //on lit dans le tube server
    strncpy(lC,buffer,4); //on la stocke
    lC[4]='\0';
    int l = atoi(lC); //on la transforme
    buffer[l]='\0';
    printf("Message recu : %s\n",buffer);
    if(strcmp(buffer, "quit") ==0) break;
    strncpy(type,buffer+4,4); //on récupère le type du message
    type[4]='\0';
    printf("Type :%s\n",type);
    if(strcmp(type,"HELO")==0){ //si le message est une demande de connexion
      printf("Connexion en cours...\n");
      connexion(buffer,l); //on execute la fonction correspondante
      printf("Connexion terminée !\n");
    }
  }
  free(buffer); //on free !
  free(type);
  free(lC);
}

int main(int argc, char const *argv[]) {
  createServer(); //creation du servre
  mainServer(); //main principal
  return 0;
}
