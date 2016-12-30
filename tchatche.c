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
  sprintf(intel,"%4d%s%s%s",8+2*(int)(strlen(pseudo)),"HELO",pseudo,pseudo); //on crée l'intel CONNEXION correspondant
  intel[8+2*strlen(pseudo)]='\0';
  write(server, intel, 8+2*strlen(pseudo)); //on l'envoie au server
  printf("j'envoie : %s\n",intel);
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

/*Fonction de déconnexion : déconnecte le client */
void deconnexionClient(char* buffer, int l){
  //TODO: Ecrire cette fonction
}

/*Fonction d'envoi de message public : envoie un message à tous les utilisateurs */
void sendPublicMessageClient(char* buffer, int l){
  //TODO: Ecrire cette fonction
}

/*Fonction d'envoi de message privé : envoie un message à un seul utilisateur */
void sendPrivateMessageClient(char* buffer, int l){
  //TODO: Ecrire cette fonction
}

/*Fonction pour obtenir la liste des utilisateurs */
void listUsersClient(char* buffer, int l){
  //TODO: Ecrire cette fonction
}

/*Fonction pour forcer la déconnexion de tous les id + shutdown du serveur */
void shutClient(char* buffer, int l){
  //TODO: Ecrire cette fonction
}

/*Fonction pour débugger le serveur */
void debugClient(char* buffer, int l){
  //TODO: Ecrire cette fonction
}

/*Fonction pour envoyer un fichier */
void sendFileClient(char* buffer, int l){
  //TODO: Ecrire cette fonction
}

void mainClient(){
  //TODO: pour gérer l'envoi des messages/fichiers.
  //Fonction similaire à mainServer mais en plus simple ! (il y a un mode d'attente par défault)
}

int main(int argc, char const *argv[]) {
  createClient(); //fonction de connexion
  mainClient(); //fonction qui s'occupe du client
  return 0;
}
