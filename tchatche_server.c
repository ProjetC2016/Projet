#include <stdio.h>
#include <stdlib.h>
#include "tchatche_server.h"

#define DIRECTORY_LENGTH 1024

int id = 1; //id du client (augmente de 1 à chaque ajout d'un nouveau client)
int logList[DIRECTORY_LENGTH];
char* pseudoList[DIRECTORY_LENGTH];
int pipes[DIRECTORY_LENGTH];

/* Fonction qui crée le tube du server */
void createServer(){
  if(access("serverPipe", F_OK) == -1){ //si le tube server n'existe pas
    mkfifo("serverPipe", 0666); //je le crée
  }
}

/* Fonction de connexion : récupère l'intel de connexion et offre un id */
void connexionServer(char* buffer, int l){
  if(logList[id-1]==id){}
  else{
    char* pseudo = malloc(((l-8)/2)*sizeof(char)); //string pour le pseudo
    strncpy(pseudo,buffer+8,(l-8)/2); //on le récupère dans le buffer
    pseudo[((l-8)/2)]='\0';
    printf("Pseudo : %s\n",pseudo);
    int client = open(pseudo, O_WRONLY); //on ouvre le tube client en écriture
    pipes[id-1]=client;
    char* intel = malloc(13*sizeof(char)); //string pour l'intel envoyé
    sprintf(intel,"%4d%s%4d",12,"OKOK",id); //on crée l'intel
    intel[12]='\0';
    write(client, intel, strlen(intel));//on écrit l'intel dans le tube client
    logList[id-1]=id-1;
    pseudoList[id-1]=pseudo;
    id++; //on augmente l'id pour le prochain tour
    free(pseudo); //on free ! (on a tout compris)
    free(intel);
  }

}

/*Fonction de déconnexion : récupère l'id et le déconnecte */
void deconnexionServer(char* buffer, int l){
  //TODO: Ecrire cette fonction (similaire à connexion)
  //1) déconnecter le client (i.e) le supprimer de la liste des clients en ligne
  //2) virer toute info relative à l'utilisateur
  //3) nettoyer/supprimer les tubes
}

/*Fonction d'envoi de message public : récupère le message et le transmet */
void sendPublicMessageServer(char* buffer, int l){
  //TODO: Ecrire cette fonction
  //1) recevoir le message
  //2) le transmettre à tous les clients de la liste
}

/*Fonction d'envoi de message privé : récupère le message et le transmet */
void sendPrivateMessageServer(char* buffer, int l){
  //TODO: Ecrire cette fonction (similaire à sendPublic)
  //1) recevoir le message
  //2) le transmettre à la personne désignée
}

/*Fonction pour obtenir la liste des utilisateurs */
void listUsersServer(char* buffer, int l){
  //TODO: Ecrire cette fonction
  //liste un par un les utilisateurs
}

/*Fonction pour forcer la déconnexion de tous les id + shutdown du serveur */
void shutServer(char* buffer, int l){
  //TODO: Ecrire cette fonction
  //1) déconnecte tous les id (utiliser deconnexion)
  //2) supprimer le server
}

/*Fonction pour débugger le serveur */
void debugServer(char* buffer, int l){
  //TODO: Ecrire cette fonction
  //EUH......
}

/*Fonction pour envoyer un fichier */
void sendFileServer(char* buffer, int l){
  //TODO: Ecrire cette fonction
  //Complexe ! A faire en dernier
}

void mainServer(){
  int server = open("serverPipe", O_RDONLY); //ouverture du tube server en lecture
  char *buffer = malloc(DIRECTORY_LENGTH*sizeof(char)); //buffer
  char *type = malloc(5*sizeof(char));  //string pour le type des messages
  char* lC = malloc(5*sizeof(char)); //longueur de l'intel
  while(1){
    int c = read(server, buffer, DIRECTORY_LENGTH); //on lit dans le tube server
    if(c!=0){
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
        connexionServer(buffer,l); //on execute la fonction correspondante
        printf("Connexion terminée !\n");
      }
      //A compléter
      else if(strcmp(type,"BYEE")==0){ //si le message est une demande de déconnexion
        deconnexionServer(buffer,l); //on execute la fonction correspondante
      }
      else if(strcmp(type,"BCST")==0){ //si le message est une demande d'envoi de message public
        sendPublicMessageServer(buffer,l); //on execute la fonction correspondante
      }
      else if(strcmp(type,"PRVT")==0){ //si le message est une demande d'envoi de message privé
        sendPrivateMessageServer(buffer,l); //on execute la fonction correspondante
      }
      else if(strcmp(type,"LIST")==0){ //si le message est une demande de la liste des utilisateurs
        listUsersServer(buffer,l); //on execute la fonction correspondante
      }
      else if(strcmp(type,"SHUT")==0){ //si le message est une demande de shutdown total
        shutServer(buffer,l); //on execute la fonction correspondante
      }
      else if(strcmp(type,"DEBG")==0){ //si le message est une demande de debug
        debugServer(buffer,l); //on execute la fonction correspondante
      }
      else if(strcmp(type,"FILE")==0){ //si le message est une demande d'envoi de fichier
        sendFileServer(buffer,l); //on execute la fonction correspondante
      }
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
