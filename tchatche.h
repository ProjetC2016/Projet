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
void sendPrivateMessageClient(char* buffer);
void listUsersClient(char* buffer, int l);
void shutClient(char* buffer, int l);
void debugClient();
void sendFileClient(char* buffer);
void mainClient();
