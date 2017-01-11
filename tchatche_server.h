#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

void createServer();
void connexionServer(char* buffer, int l);
void deconnexionServer(char* buffer, int l);
void sendPublicMessageServer(char* buffer, int l);
void sendPrivateMessageServer(char* buffer, int l);
void listUsersServer(char* buffer, int l);
void shutServer();
void debugServer();
void sendFileServer(char* buffer, int l);
void mainServer();
