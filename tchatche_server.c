#include <stdio.h>
#include <stdlib.h>
#include "tchatche_server.h"

#define DIRECTORY_LENGTH 1024

int counter = 1; //id possible pour un client (augmente de 1 à chaque ajout d'un nouveau client)
int nbUsers = 0;
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
  if(logList[counter-1]!=counter){
    char* pseudo = malloc(((l-16)/2)*sizeof(char)); //string pour le pseudo
    strncpy(pseudo,buffer+12,(l-16)/2); //on le récupère dans le buffer
    pseudo[((l-16)/2)]='\0';
    printf("Pseudo : %s\n",pseudo);
    int client = open(pseudo, O_WRONLY); //on ouvre le tube client en écriture
    pipes[counter-1]=client;
    char* intel = malloc(13*sizeof(char)); //string pour l'intel envoyé
    sprintf(intel,"%4d%s%4d",12,"OKOK",counter); //on crée l'intel
    intel[12]='\0';
    write(client, intel, strlen(intel));//on écrit l'intel dans le tube client
    logList[counter-1]=counter;
    pseudoList[counter-1]=pseudo;
    counter++; //on augmente le counter pour le prochain tour
    nbUsers++; //on augmente le nombre d'utilisateurs
    free(pseudo); //on free ! (on a tout compris)
    free(intel);
  }

}

/*Fonction de déconnexion : récupère l'id et le déconnecte */
void deconnexionServer(char* buffer, int l){
  char* idC = malloc(4*sizeof(char)); //String idC envoyé par le client au serveur
  int idClient; //int correspondant au string de l'idC
  strncpy(idC,buffer+8,4); //on le récupère dans le buffer
  idClient = atoi(idC); //on le transforme pour récupérer l'id
  char* intel = malloc(13*sizeof(char)); //string pour l'intel envoyé
  sprintf(intel,"%4d%s%4d",12,"BYEE",idClient); //on crée l'intel
  intel[12]='\0';
  int client = pipes[idClient-1];
  write(client, intel, strlen(intel));//on écrit l'intel dans le tube client
  logList[idClient-1] = 0;
  pseudoList[idClient-1] = NULL;
  pipes[idClient-1] = 0;
  nbUsers--; //on diminue le nombre d'utilisateurs
  free(idC);
  free(intel);
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
    char* intel = malloc((12+(int)strlen(pseudoList[0]))*sizeof(char)); //string pour l'intel envoyé
    //on recupere l'id de celui qui a demandé la liste :
    char* id = malloc(4*sizeof(char));    
    strncpy(id, buffer+8, 4);
   /* int index=0;
    while (pseudoList[index]){
      sprintf(intel,"%4d%s%4d",12,"OKOK",id); //on crée l'intel
      index++;
    } */
    printf("###### %s ### \n",pseudoList[0]);
    sprintf(intel,"%4d%s%4d%s",(12+(int)strlen(pseudoList[0])),"LIST",nbUsers,pseudoList[0]); //on crée l'intel
    write(atoi(id),intel,strlen(intel));
    printf ("-------------INTEL DU SERV %s \n ", intel);

  }

/*Fonction pour forcer la déconnexion de tous les id + shutdown du serveur 
void shutServer(){
  int i;
  for(i=0;i<id-1;i++){
    if(pipes[i]=0){
      char* pseudo = pseudoList[i];
      char* intel = malloc((9+strlen(pseudo))*sizeof(char));
      sprintf(intel,"%4d%s%4d%s",8+strlen(pseudo),"SHUT",(int) strlen(pseudo),pseudo); //on crée l'intel
      intel[8+strlen(pseudo)]='\0';
      write(pipes[i],intel,strlen(intel));
      free(pseudo);
      free(intel);
    }
  }
  free(pseudoList);
  //TODO: Ecrire cette fonction
  //1) déconnecte tous les id (utiliser deconnexion)
  //2) supprimer le server
}

 */

/*Fonction pour débugger le serveur */
void debugServer(){
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
        shutServer(); //on execute la fonction correspondante
        break;
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
