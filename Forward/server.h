#ifndef SERVER_H
#define SERVER_H

#include <semaphore.h>
#include "network.h"
#include "eventhandler.h"
class Server
{
public:
    Server();
};
struct udpSender{
    char fileName[30] = {0};
    int sock;
    sockaddr_in sender;
    char header[HEADERLEN];

};

void startServer(int port, int udpPort);
void * listenThread(void * args);
void * requestHandler(void * args);
long getSize(char fileName[]);
void sendFile(char header[], char fName[]);
void * listenUdp(void * args);
void * sendFileUdp(void * args);
#endif // SERVER_H

