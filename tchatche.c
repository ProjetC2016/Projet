#include <stdio.h>
#include <stdlib.h>
#include "tchatche.h"

#define MAX(x,y) ((x)>(y)?(x):(y))
#define DIRECTORY_LENGTH 1024

int client = 0;
int id = 0;
int server;
char *pseudo;
char* standin;
size_t l;
fd_set readers;

void createClient(){
  server = open("serverPipe", O_WRONLY); //on ouvre le tube server en écriture
  standin = malloc(DIRECTORY_LENGTH*sizeof(char));
  char *buffer = malloc(DIRECTORY_LENGTH*sizeof(char));
  pseudo = malloc(DIRECTORY_LENGTH*sizeof(char));
  printf("Pseudo> ");
  fgets(buffer, DIRECTORY_LENGTH, stdin); //on récupère le pseudo
  strncpy(pseudo,buffer,strlen(buffer)-1); //on le stocke dans la variable correspondante
  pseudo[strlen(buffer)-1]='\0';
  if(access(pseudo, F_OK) == -1){ //si le fichier n'est pas crée
    mkfifo(pseudo, 0666); //je le crée
  }
  char* intel = malloc((16+2*strlen(pseudo)+1)*sizeof(char)); //infos envoyées au serveur
  sprintf(intel,"%4d%s%4d%s%4d%s",16+2*(int)(strlen(pseudo)),"HELO",(int)strlen(pseudo),pseudo,(int)strlen(pseudo),pseudo); //on crée l'intel CONNEXION correspondant
  intel[16+2*strlen(pseudo)]='\0';
  write(server, intel, 16+2*strlen(pseudo)); //on l'envoie au server
  printf("j'envoie : %s\n",intel);
  client = open(pseudo, O_RDONLY); //on ouvre son propre tube en lecture
  char* recu = malloc(12*sizeof(char)); //message recu du serveur
  char* idC = malloc(4*sizeof(char)); //String id
  read(client, recu, DIRECTORY_LENGTH); //on lit la tube
  strncpy(idC,recu+8,4); //on le stocke dans idC
  id = atoi(idC); //on le transforme pour récupérer l'id
  printf("Connected. ID : %d\n",id);
  free(buffer); //et on free !
  free(intel);
  free(recu);
  free(idC);
}

/*Fonction de déconnexion : déconnecte le client */
void deconnexionClient(){
  printf("Je lance la déconnexion !\n");
  char* intel = malloc(13*sizeof(char)); //string pour l'intel envoyé au serveur
  sprintf(intel,"%4d%s%4d",12,"BYEE",id); //on crée l'intel DECONNEXION
  intel[12]='\0';
  write(server, intel, strlen(intel)); //on l'envoie au serveur
  char* recu = malloc(12*sizeof(char)); //message recu du serveur
  read(client, recu, DIRECTORY_LENGTH); //on lit le tube
  client = 0;
  id = 0;
  unlink(pseudo);
  printf("A bientôt %s !\n", pseudo);
  free(pseudo);
  free(intel);
  free(recu);
  free(standin);
  printf("Déconnexion OK\n");
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
  char* intel = malloc(13*sizeof(char)); //string pour l'intel envoyé
  char* buffer = malloc(13*sizeof(char));
  sprintf(intel,"%4d%s%4d",12,"LIST",id); //on crée l'intel
  write(server, intel, strlen(intel));//on écrit l'intel dans le tube client
  read(server,buffer,strlen(buffer));
  printf ("-------------INTEL DU CLIENT %s \n ", intel);

  //TODO : le faire autant de fois qu'il y a de pseudos

  printf("%s",buffer);
}


/*Fonction pour forcer la déconnexion de tous les id + shutdown du serveur  */
void shutClient(){
  printf("Je lance ShutClient\n");
  char* intel = malloc(13*sizeof(char)); //infos envoyées au serveur
  sprintf(intel,"%4d%s%4d",12,"SHUT",id); //on crée l'intel CONNEXION correspondant
  intel[12]='\0';
  write(server, intel, 13); //on l'envoie au server
  printf("j'envoie : %s\n",intel);
  char* recu = malloc((8+strlen(pseudo))*sizeof(char)); //message recu du serveur
  read(client, recu, DIRECTORY_LENGTH); //on lit le tube
  deconnexionClient();
  free(intel); //et on free !
  free(recu);
  exit(0);
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
  char* buffer = malloc(DIRECTORY_LENGTH*sizeof(char));
  char* envoi = malloc(DIRECTORY_LENGTH*sizeof(char));
  char* private = malloc(11*sizeof(char));
  char* users = malloc(6*sizeof(char));
  char* shut = malloc(5*sizeof(char));
  char* debug = malloc(6*sizeof(char));
  char* send = malloc(10*sizeof(char));
  char* type = malloc(5*sizeof(char));
  while(1){
    FD_ZERO(&readers);
    FD_SET(0,&readers);
    FD_SET(client,&readers);
    int n = select(MAX(0,client) + 1,&readers,NULL, NULL, NULL);
    if(n>0){
        if(FD_ISSET(client, &readers)){
          printf("Je capte qqch dans le tube !\n");
          l = read(client,buffer, DIRECTORY_LENGTH);
          memcpy(type,&buffer[4],4);
          type[4]='\0';
          buffer[l]='\0';
          switch (l) {
            case 0 | 1:
              printf("Aie, chaine vide\n");
              break;
            case -1:
              perror("Mauvaise lecture stdin\n");
              break;
            default:
              printf("Buffer recu : %s de type %s\n",buffer,type);
              if(strcmp("SHUT",type)==0){
                free(buffer);
                free(envoi);
                free(private);
                free(users);
                free(shut);
                free(debug);
                free(send);
                free(type);
                deconnexionClient();
                exit(0);
              }
              break;
          }
        }
        if(FD_ISSET(0, &readers)){
          printf("Je capte qqch sur stdin !\n");
          fgets(buffer,DIRECTORY_LENGTH,stdin);
          l = strlen(buffer);
          buffer[l-1]='\0';
          switch (l) {
            case 0 | 1:
              printf("Aie, chaine vide\n");
              break;
            case -1:
              perror("Mauvaise lecture stdin\n");
              break;
            default:
              memcpy(private,&buffer[0],11);
              memcpy(users,&buffer[0],6);
              memcpy(shut,&buffer[0],5);
              memcpy(debug,&buffer[0],6);
              memcpy(send,&buffer[0],10);
              if(strcmp("!quit",buffer)==0){
                deconnexionClient();
                free(buffer);
                free(envoi);
                free(private);
                free(users);
                free(shut);
                free(debug);
                free(send);
                free(type);
                exit(0);
              }
              else if(strcmp(private,"!private to")==0){
                sendPrivateMessageClient(envoi);
              }
              else if(strcmp(users,"!users")==0){
                listUsersClient();
              }
              else if(strcmp(shut,"!shut")==0){
                printf("Blackout !\n");
                free(buffer);
                free(envoi);
                free(private);
                free(users);
                free(shut);
                free(debug);
                free(send);
                free(type);
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
              break;
          }
      }
    }
  }
}

int main(int argc, char const *argv[]) {
  createClient(); //fonction de connexion
  mainClient(); //fonction qui s'occupe du client
  return 0;
}
