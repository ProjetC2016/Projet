#include <stdio.h>
#include <stdlib.h>
#include "tchatche.h"

#define DIRECTORY_LENGTH 1024

int client = 0;
int id = 0;
int server;

void createClient(){
  server = open("serverPipe", O_WRONLY); //on ouvre le tube server en écriture
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
void deconnexionClient(){
  //TODO: Ecrire cette fonction
}

/*Fonction d'envoi de message public : envoie un message à tous les utilisateurs */
void sendPublicMessageClient(char* buffer){
  //TODO: Ecrire cette fonction
}

/*Fonction d'envoi de message privé : envoie un message à un seul utilisateur */
void sendPrivateMessageClient(char* buffer){
  //TODO: Ecrire cette fonction
}

/*Fonction pour obtenir la liste des utilisateurs */
void listUsersClient(){
  //TODO: Ecrire cette fonction
}

/*Fonction pour forcer la déconnexion de tous les id + shutdown du serveur */
void shutClient(){
  char* intel = malloc(13*sizeof(char)); //infos envoyées au serveur
  sprintf(intel,"%4d%s%4d",12,"SHUT",id,); //on crée l'intel CONNEXION correspondant
  intel[12]='\0';
  write(server, intel, 13); //on l'envoie au server
  printf("j'envoie : %s\n",intel);
  char* recu = malloc(8+strlen()*sizeof(char)); //message recu du serveur
  read(client, recu, DIRECTORY_LENGTH); //on lit la tube
  strncpy(idC,recu+8,4); //on le stocke dans idC
  id = atoi(idC); //on le transforme pour récupérer l'id
  printf("Connected. ID : %d\n",id);
  free(buffer); //et on free !
  free(pseudo);
  free(intel);
  free(recu);
  free(idC);
  //TODO: Ecrire cette fonction
}

/*Fonction pour débugger le serveur */
void debugClient(){
  //TODO: Ecrire cette fonction
}

/*Fonction pour envoyer un fichier */
void sendFileClient(char* buffer){
  //TODO: Ecrire cette fonction
}

void mainClient(){
  //TODO: faire un help des commandes
  char *buffer = malloc(DIRECTORY_LENGTH*sizeof(char));
  char *envoi = malloc(DIRECTORY_LENGTH*sizeof(char));
  printf("> ");
  fgets(buffer, DIRECTORY_LENGTH, stdin); //on récupère l'envoi
  strncpy(envoi,buffer,strlen(buffer)-1); //on le stocke dans la variable correspondante
  char* private = malloc(11*sizeof(char));
  memcpy(private,&envoi[0],11);
  char* users = malloc(6*sizeof(char));
  memcpy(users,&envoi[0],6);
  char* shut = malloc(5*sizeof(char));
  memcpy(shut,&envoi[0],5);
  char* debug = malloc(6*sizeof(char));
  memcpy(debug,&envoi[0],6);
  char* send = malloc(10*sizeof(char));
  memcpy(send,&envoi[0],10);
  if(strcmp("!quit",envoi)==0){
    deconnexionClient();
  }
  else if(strcmp(private,"!private to")==0){
    sendPrivateMessageClient(envoi);
  }
  else if(strcmp(users,"!users")==0){
    listUsersClient();
  }
  else if(strcmp(shut,"!shut")==0){
    shutClient();
  }
  else if(strcmp(debug,"!debug")==0){
    debugClient();
  }
  else if(strcmp(send,"!send file")==0){
    sendFileClient(envoi);
  }
  else{
    sendPublicMessageClient(envoi);
  }
}

int main(int argc, char const *argv[]) {
  createClient(); //fonction de connexion
  mainClient(); //fonction qui s'occupe du client
  return 0;
}
