#include <stdio.h>
#include <stdlib.h>
#include "tchatche_server.h"

#define DIRECTORY_LENGTH 1024

int counter = 1; //id possible pour un client (augmente de 1 à chaque ajout d'un nouveau client)
int nbUsers = 0;
int logList[DIRECTORY_LENGTH];
char* pseudoList[DIRECTORY_LENGTH];
int pipes[DIRECTORY_LENGTH];
int shutdown = 0;

/* Fonction qui crée le tube du server */
void createServer(){
  if(access("serverPipe", F_OK) == -1){ //si le tube server n'existe pas
    mkfifo("serverPipe", 0666); //je le crée
  }
}

void test(){
  printf("Liste des pseudos : ");
  int i;
  for (i=0;i<10; i++){
    printf("%s   - ", pseudoList[i]);
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
    pseudoList[counter-1]=malloc(DIRECTORY_LENGTH*sizeof(char));
    strcpy(pseudoList[counter-1],pseudo);
    counter++; //on augmente le counter pour le prochain tour
    nbUsers++; //on augmente le nombre d'utilisateurs
    free(pseudo); //on free ! (on a tout compris)
    free(intel);
    //test();
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
  free(pseudoList[idClient-1]);
  pipes[idClient-1] = 0;
  nbUsers--; //on diminue le nombre d'utilisateurs
  free(idC);//et on free
  free(intel);
}

/*Fonction d'envoi de message public : récupère le message et le transmet */
void sendPublicMessageServer(char* buffer, int l){
  printf("DEBUT\n");
  char* idC = malloc(4*sizeof(char)); //String idC envoyé par le client au serveur
  strncpy(idC,buffer+8,4); //on le récupère dans le buffer
  int idClient = atoi(idC); //on le transforme pour récupérer l'id
  char* sender = pseudoList[idClient-1]; //On récupère le pseudo de celui qui envoie le message
  printf("#### sender de taille %d : %s ####\n", (int)strlen(sender),sender);
  char* msgSize = malloc(4*sizeof(char)); //String correspondant à la taille du message
  strncpy(msgSize, buffer+12, 4);
  int messageSize = atoi(msgSize);
  char* message = malloc(messageSize*sizeof(char));
  strncpy(message, buffer+16, messageSize);
  char* intel = malloc((17+strlen(sender)+messageSize)*sizeof(char));
  sprintf(intel,"%4d%s%4d%s%4d%s", 16+(int)strlen(sender)+messageSize, "BCST", (int)strlen(sender), sender, messageSize, message);
  intel[16+(int)strlen(sender)+messageSize]='\0';
  int i;
  for(i=0;i<counter;i++){
    if(pipes[i]!=0){
      write(pipes[i], intel, strlen(intel)); //On écrit l'intel dans tous les tubes clients
    }
  }
  free(idC);
  free(msgSize);
  free(message);
  free(intel);
  printf("FIN\n");
}

/*Fonction d'envoi de message privé : récupère le message et le transmet */
void sendPrivateMessageServer(char* buffer, int l){
  printf("DEBUT\n");
  char* idC = malloc(4*sizeof(char)); //String idC envoyé par le client au serveur
  strncpy(idC,buffer+8,4); //on le récupère dans le buffer
  int idClient = atoi(idC); //on le transforme pour récupérer l'id
  char* sender = pseudoList[idClient-1]; //On récupère le pseudo de celui qui envoie le message
  printf("#### sender de taille %d : %s ####\n", (int)strlen(sender),sender);
  char* psdSize = malloc(4*sizeof(char)); //String correspondant à la taille du pseudo de celui qui recoit
  strncpy(psdSize, buffer+12, 4);
  int pseudoSize = atoi(psdSize);
  char* receiver = malloc(pseudoSize*sizeof(char));
  strncpy(receiver, buffer+16, pseudoSize);
  char* msgSize = malloc(4*sizeof(char)); //String correspondant à la taille du message
  strncpy(msgSize, buffer+16+pseudoSize, 4);
  int messageSize = atoi(msgSize);
  char* message = malloc(messageSize*sizeof(char));
  strncpy(message, buffer+20+pseudoSize, messageSize);
  char* intel = malloc((17+strlen(sender)+messageSize)*sizeof(char));
  sprintf(intel,"%4d%s%4d%s%4d%s", 16+(int)strlen(sender)+messageSize, "PRVT", (int)strlen(sender), sender, messageSize, message);
  intel[16+(int)strlen(sender)+messageSize]='\0';
  int idReceiver=-1;
  int j;
  for(j=0; j<nbUsers; j++){
    if(strcmp(pseudoList[j],receiver)==0){
      idReceiver = j;
    }
  }
  if(idReceiver!=-1){
    write(pipes[idReceiver], intel, strlen(intel)); //On écrit l'intel dans le tube de celui qui recoit
    char* intelOk = malloc((17+strlen(receiver)+messageSize)*sizeof(char)); //string pour l'intelOk envoyé
    sprintf(intelOk,"%4d%s%4d%s%4d%s",16+(int)strlen(receiver)+messageSize,"OKPV",(int)strlen(receiver),receiver, messageSize, message); //on crée l'intelOk
    intelOk[16+strlen(receiver)+messageSize]='\0';
    write(pipes[idClient-1], intelOk, strlen(intelOk)); //on informe le client que le message privé a été envoyé
    free(intelOk);
  }else{
    char* intelBad = malloc(17*sizeof(char)); //string pour l'intelBad envoyé
    sprintf(intelBad,"%4d%s%4d%s",16,"BADD",4,"WRNG"); //on crée l'intelBad
    intelBad[16]='\0';
    write(pipes[idClient-1], intelBad, strlen(intelBad));
    free(intelBad);
  }

  free(idC);
  free(psdSize);
  free(receiver);
  free(msgSize);
  free(message);
  free(intel);
  printf("FIN\n");
}

/*Fonction pour obtenir la liste des utilisateurs */
void listUsersServer(char* buffer, int l){
  //on recupere l'id de celui qui a demandé la liste :
  char* id = malloc(4*sizeof(char));
  strncpy(id, buffer+8, 4);
  // id[(int)(strlen(id))]='\0';
  int i;
  char* intel = malloc((12+(int)strlen(pseudoList[0]))*sizeof(char)); //string pour l'intel envoyé
  int idClient = atoi(id);
  int client = pipes[idClient-1];
  for(i=0;i<counter;i++){
    if(pipes[i]!=0){
      sprintf(intel,"%4d%s%4d%s",(12+(int)strlen(pseudoList[i])),"LIST",nbUsers,pseudoList[i]); //on crée l'intel
      intel[(12+(int)strlen(pseudoList[i]))]='\0';
      printf("%d J'envoie %s\n",i,intel);
      int c = write(client,intel,strlen(intel));
      sleep(2);
      printf("OK : %d\n",c);
    }
  }
  printf("J'ai fini !\n");
}
/* Fonction pour forcer la déconnexion de tous les id + shutdown du serveur */
void shutServer(){
  printf("J'éxécute shutServer\n");
  int i;
  char* intel = malloc(DIRECTORY_LENGTH*sizeof(char));
  char* pseudo = malloc(DIRECTORY_LENGTH*sizeof(char));
  for(i=0;i<counter;i++){
    if(pipes[i]!=0){
      strcpy(pseudo,pseudoList[i]);
      sprintf(intel,"%4d%s%4d%s",12+(int)strlen(pseudo),"SHUT",(int) strlen(pseudo),pseudo); //on crée l'intel
      intel[12+strlen(pseudo)]='\0';
      write(pipes[i],intel,strlen(intel));
    }
  }
  unlink("serverPipe");
    
  free(intel);
  free(pseudo);
  printf("J'ai fini !\n");
}

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
      if(nbUsers==0 && shutdown==1){
	printf("Il est mort Jim !\n");
	break;
      }
      if(c!=0){
	strncpy(lC,buffer,4); //on la stocke
	int l = atoi(lC); //on la transforme
	buffer[c]='\0';
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
	  printf("Je lance SendMessage !\n");
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
	  shutdown=1;
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
