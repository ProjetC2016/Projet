#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

void createClient();
void deconnexionClient();
void sendPublicMessageClient(char* buffer);
void readMessage(char* recu, char* type);
void sendPrivateMessageClient(char* buffer);
void listUsersClient();
void shutClient();
void debugClient();
void sendFileClient(char* buffer);
void mainClient();
