#include <stdio.h>
#include <stdlib.h>
#include "tchatche.h"

#define MAX(x,y) ((x)>(y)?(x):(y))
#define LENGTH_MAX 1024

int client = 0;
int id = 0;
int server;
char *pseudo;
char* standin;
size_t l;
fd_set readers;

/*Fonction de connexion : connecte le client */
void createClient(){
  server = open("serverPipe", O_WRONLY); //on ouvre le tube server en écriture
  standin = malloc(LENGTH_MAX*sizeof(char));
  char *buffer = malloc(LENGTH_MAX*sizeof(char));
  pseudo = malloc(LENGTH_MAX*sizeof(char));
  printf("Pseudo> ");
  fgets(buffer, LENGTH_MAX, stdin); //on récupère le pseudo
  strncpy(pseudo,buffer,strlen(buffer)-1); //on le stocke dans la variable correspondante
  pseudo[strlen(buffer)-1]='\0';
  if(access(pseudo, F_OK) == -1){ //si le fichier n'est pas crée
    mkfifo(pseudo, 0666); //je le crée
  }
  char* intel = malloc((16+2*strlen(pseudo)+1)*sizeof(char)); //infos envoyées au serveur
  sprintf(intel,"%4d%s%4d%s%4d%s",16+2*(int)(strlen(pseudo)),"HELO",(int)strlen(pseudo),pseudo,(int)strlen(pseudo),pseudo); //on crée l'intel CONNEXION correspondant
  intel[16+2*strlen(pseudo)]='\0';
  write(server, intel, 16+2*strlen(pseudo)); //on l'envoie au server
  client = open(pseudo, O_RDONLY); //on ouvre son propre tube en lecture
  char* recu = malloc(12*sizeof(char)); //message recu du serveur
  char* idC = malloc(4*sizeof(char)); //String id
  read(client, recu, LENGTH_MAX); //on lit la tube
  strncpy(idC,recu+8,4); //on le stocke dans idC
  id = atoi(idC); //on le transforme pour récupérer l'id
  printf("Vous êtes connecté(e)\n");
  printf("(!help pour la liste des commandes disponibles)\n" );
  free(standin);
  free(buffer); //et on free !
  free(pseudo);
  free(intel);
  free(recu);
  free(idC);
}

/*Fonction de déconnexion : déconnecte le client */
void deconnexionClient(){
  char* intel = malloc(13*sizeof(char)); //string pour l'intel envoyé au serveur
  sprintf(intel,"%4d%s%4d",12,"BYEE",id); //on crée l'intel DECONNEXION
  intel[12]='\0';
  write(server, intel, strlen(intel)); //on l'envoie au serveur
  char* recu = malloc(12*sizeof(char)); //message recu du serveur
  read(client, recu, LENGTH_MAX); //on lit le tube
  client = 0;
  id = 0;
  unlink(pseudo); //suppression du tube client
  printf("A bientôt %s !\n", pseudo);
  free(intel);
  free(recu);
  exit(0);
}

/*Fonction d'envoi de message public : envoie un message à tous les utilisateurs */
void sendPublicMessageClient(char* buffer){
  char* intel = malloc(17+strlen(buffer)*sizeof(char)); //string pour l'intel envoyé au serveur
  sprintf(intel,"%4d%s%4d%4d%s",17+(int)strlen(buffer),"BCST",id,(int)strlen(buffer),buffer); //on crée l'intel MESSAGE PUBLIC
  intel[16+strlen(buffer)] = '\0';
  write(server,intel,strlen(intel)); //on l'envoie au server
  free(intel);
}

/*Fonction de lecture de message */
void readMessage(char* recu, char* type){
  if(strcmp(type, "WRNG")!=0){ //si le message n'est pas une erreur
    char* psdSize = malloc(4*sizeof(char));
    int pseudoSize = atoi(strncpy(psdSize, recu+8, 4)); //on récupère la taille du pseudo du sender
    char* sender = malloc(pseudoSize*sizeof(char));
    strncpy(sender, recu+12, pseudoSize); //on récupère le pseudo du sender
    sender[pseudoSize]='\0';
    char* msgSize = malloc(4*sizeof(char));
    int messageSize = atoi(strncpy(msgSize, recu+12+pseudoSize, 4)); //on récupère la taille du message
    char* message = malloc(messageSize+sizeof(char));
    strncpy(message, recu+16+pseudoSize, messageSize); //on récupère le message
    message[messageSize] = '\0';
    if(strcmp(type,"BCST")==0){ //affichage du message
      printf("[%s] %s\n", sender, message);
    }
    if(strcmp(type,"PRVT")==0){
      printf("[%s-->%s] %s\n", sender,pseudo, message);
    }
    if(strcmp(type,"OKPV")==0){
      printf("[%s-->%s] %s\n", pseudo,sender, message); //ici le nom sender n'est pas bon, c'est en fait le receiver du message privé
    }
    free(psdSize); //free !
    free(sender);
    free(msgSize);
    free(message);
  }else{ //s'il y a erreur
    printf("Le destinataire choisi n'existe pas :(\n");
  }
}

/*Fonction d'envoi de message privé : envoie un message à un seul utilisateur */
void sendPrivateMessageClient(char* buffer){
  char private[8];
  char to[2];
  char* receiver = malloc(strlen(buffer)*sizeof(char));
  sscanf(buffer, "%s %s %s", private,to,receiver); //recupération des arguments
  char* message = malloc(strlen(buffer)*sizeof(char));
  strncpy(message, buffer+12+strlen(receiver),strlen(buffer)-strlen(receiver)-12); //récupération du message
  char* intel = malloc(21+strlen(buffer)*sizeof(char)); //string pour l'intel envoyé au serveur
  sprintf(intel,"%4d%s%4d%4d%s%4d%s", 21+(int)strlen(buffer),"PRVT",id,(int)strlen(receiver),receiver,(int)strlen(message),message); //construction de l'intel
  intel[20+strlen(buffer)]='\0';
  write(server,intel,strlen(intel)); //envoi au server
  free(receiver); //free !
  free(message);
  free(intel);
}

/*Fonction pour obtenir la liste des utilisateurs présents sur le chat */
void listUsersClient(){
  char* intel = malloc(13*sizeof(char)); //string pour l'intel envoyé
  sprintf(intel,"%4d%s%4d",12,"LIST",id); //on crée l'intel
  intel[12]='\0';
  write(server, intel, strlen(intel));//on écrit l'intel dans le tube server
  printf("Liste des utilisateurs connectés : \n");
  free(intel);
}

/*Fonction pour forcer la déconnexion de tous les id + shutdown du serveur  */
void shutClient(){
  char* intel = malloc(13*sizeof(char)); //infos envoyées au serveur
  sprintf(intel,"%4d%s%4d",12,"SHUT",id); //on crée l'intel correspondant
  intel[12]='\0';
  write(server, intel, 13); //on l'envoie au server
  free(intel); //et on free !
}

/*Fonction pour débugger le serveur */
void debugClient(){
  char* intel = malloc(9*sizeof(char)); //infos envoyées au serveur
  sprintf(intel,"%4d%s",8,"DEBG"); //on crée l'intel correspondant
  intel[8]='\0';
  write(server, intel, 8); //on l'envoie au server
}

/*Fonction pour envoyer un fichier */
void sendFileClient(char* buffer){
  char* name = malloc(LENGTH_MAX*sizeof(char)); //élements du message recu, on stocke tout
  char* send = malloc(5*sizeof(char));
  char* file = malloc(5*sizeof(char));
  char* to = malloc(2*sizeof(char));
  char* destinataire = malloc(LENGTH_MAX*sizeof(char));
  sscanf(buffer, "%s %s %s %s %s",send,file,name,to,destinataire); // on range tout là où il faut
  name[(int)strlen(name)]='\0';
  if(access(name, F_OK) == -1){
    printf("Aie, votre fichier n'existe pas ! :(\n");
  }else{
    destinataire[(int)strlen(destinataire)]='\0';
    struct stat buffer2; //pour avoir des infos sur le fichier
    stat(name,&buffer2); //on prend son nom
    int longueurF = buffer2.st_size; //= wc fichier : nombre de caractères
    if(longueurF>=0){ // si la longueur est négative le fichier n'existe pas
      char* intel = malloc(LENGTH_MAX*sizeof(char)); //infos envoyées au serveur
      sprintf(intel,"%4d%s%4d%4d%4d%s%8d%4d%s",24+(int)strlen(pseudo)+3,"FILE",0,id,(int) strlen(destinataire),destinataire,longueurF,(int)strlen(name),name); //on crée l'intel correspondant
      intel[(int) strlen(intel)]='\0';
      write(server, intel, strlen(intel)); //on l'envoie au server
      char* answer = malloc(LENGTH_MAX*sizeof(char)); //infos recues du serveur
      read(client,answer, LENGTH_MAX);
      char* type = malloc(5*sizeof(char));
      strncpy(type,answer+4,4);
      type[4]='\0';
      if(strcmp(type,"OKOK")==0){ //si le server valide
        printf("Votre fichier a bien été envoyé\n");
        char* idTransfertC = malloc(4*sizeof(char));
        int idTransfert; //id du transfert
        strncpy(idTransfertC,buffer+8,4);
        idTransfert = atoi(idTransfertC);
        int nbMessages = longueurF/256 + 1; //on calcule le nombre de messages nécessaires
        int i;
        FILE* fichier = fopen(name,"r"); //le fichier à envoyer
        char* parcours = malloc(257*sizeof(char));
        char* datas = malloc(LENGTH_MAX*sizeof(char));
        for(i=0;i<nbMessages;i++){
  	       fread(parcours,sizeof(char),256,fichier); //on va chercher les données dans le fichier
           parcours[(int) strlen(parcours)]='\0';
  	       sprintf(datas,"%4d%s%4d%4d%4d%s",20+(int)strlen(parcours),"FIOK",i+1,idTransfert,(int)strlen(parcours),parcours); //on crée les données
  	       datas[strlen(datas)]='\0';
  	       write(server, datas, 256); //on les envoie au server
        }
        fclose(fichier); //à la fin : fermeture du fichier
      }
      else{
        printf("SNIF, erreur d'envoi :(\n");
      }
    }
    else printf("Aie, votre fichier n'existe pas ! :(\n");
  }
}

/* Main Client principal */
void mainClient(){
  char* buffer = malloc(LENGTH_MAX*sizeof(char));
  char* private = malloc(11*sizeof(char));
  char* users = malloc(6*sizeof(char));
  char* shut = malloc(5*sizeof(char));
  char* debug = malloc(6*sizeof(char));
  char* send = malloc(10*sizeof(char));
  char* help = malloc(6*sizeof(char));
  char* type = malloc(5*sizeof(char));
//  int fic = 0;
  while(1){
    sleep(1);
    FD_ZERO(&readers);
    FD_SET(0,&readers);
    FD_SET(client,&readers);
    int n = select(MAX(0,client) + 1,&readers,NULL, NULL, NULL);
    if(n>0){
      if(FD_ISSET(client, &readers)){
      	l = read(client,buffer, LENGTH_MAX);
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
	  if(strcmp("SHUT",type)==0){
	    free(buffer);
	    free(private);
	    free(users);
	    free(shut);
	    free(debug);
	    free(send);
	    free(type);
	    deconnexionClient();
	    exit(0);
	  }
	  else if(strcmp("BCST",type)==0){
	    readMessage(buffer,"BCST");
	  }
	  else if(strcmp("PRVT",type)==0){
	    readMessage(buffer,"PRVT");
	  }
	  else if(strcmp("OKPV",type)==0){
	    readMessage(buffer,"OKPV");
	  }
	  else if(strcmp("BADD",type)==0){
	    readMessage(buffer,"WRNG");
	  }
	  else if(strcmp("FIOK",type)==0){
      //TO DO
	    char* recu = malloc(256*sizeof(char));
	    strncpy(recu,buffer+20,256);
	    printf("%s",recu);
	  }
	  else if(strcmp("LIST",type)==0){
	    char* pseudo = malloc(LENGTH_MAX*sizeof(char));
	    char* i = malloc(4*sizeof(char));
	    char* len = malloc(4*sizeof(char));
	    int rlen = 0;
	    strncpy(len,buffer,4);
	    rlen = atoi(len);
	    strncpy(pseudo, buffer+12, rlen-12);
	    pseudo[rlen-12]='\0';
	    printf("#-  %s #\n",pseudo);
	    free(pseudo);
	    free(i);
	    free(len);
	  }
	  break;
	}
      }
      if(FD_ISSET(0, &readers)){
      	fgets(buffer,LENGTH_MAX,stdin);
      	l = strlen(buffer)-1;
      	buffer[l]='\0';
      	switch (l) {
      	case 0 | 1:
      	  printf("Aie, chaine vide\n");
      	  break;
      	case -1:
      	  perror("Mauvaise lecture stdin\n");
      	  break;
      	default:
      	  memcpy(private,&buffer[0],11);
	        private[11]='\0';
      	  memcpy(users,&buffer[0],6);
	        users[6]='\0';
      	  memcpy(shut,&buffer[0],5);
	        shut[5]='\0';
      	  memcpy(debug,&buffer[0],6);
	        debug[6]='\0';
      	  memcpy(send,&buffer[0],10);
	        send[10]='\0';
          memcpy(help,&buffer[0],5);
	        help[5]='\0';
      	  if(strcmp("!quit",buffer)==0){
      	    free(buffer);
      	    free(private);
      	    free(users);
      	    free(shut);
      	    free(debug);
      	    free(send);
            free(help);
      	    free(type);
            deconnexionClient();
      	    exit(0);
      	  }
      	  else if(strcmp(private,"!private to")==0){
      	    sendPrivateMessageClient(buffer);
      	  }
      	  else if(strcmp(users,"!users")==0){
      	    listUsersClient();
      	  }
      	  else if(strcmp(shut,"!shut")==0){
      	    printf("Blackout !\n");
      	    shutClient();
      	  }
      	  else if(strcmp(debug,"!debug")==0){
      	    debugClient();
      	  }
      	  else if(strcmp(send,"!send file")==0){
      	    sendFileClient(buffer);
      	  }
          else if(strcmp(help,"!help")==0){
            printf("\t!quit : Pour vous déconnecter\n\n" );
            printf("\t!private to <destinataire> : Pour envoyer un message privé à <destinataire>\n\n" );
            printf("\t!users : Pour connaitre la liste des utilisateurs\n\n" );
            printf("\t!shut : Pour éteindre le serveur et déconnecter tous les clients\n\n" );
            printf("\t!debug : Pour obtenir des informations techniques sur le serveur\n\n" );
            printf("\t!send file <nom fichier> to <destinataire : Pour envoyer le fichier <nom fichier> à <destinataire>\n\n" );
          }
      	  else{
      	    sendPublicMessageClient(buffer);
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
