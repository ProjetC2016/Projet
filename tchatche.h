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
void deconnexionClient(char* buffer, int l);
void sendPublicMessageClient(char* buffer, int l);
void sendPrivateMessageClient(char* buffer, int l);
void listUsersClient(char* buffer, int l);
void shutClient(char* buffer, int l);
void debugClient(char* buffer, int l);
void sendFileClient(char* buffer, int l);
void mainClient();
