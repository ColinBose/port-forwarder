#ifndef CLIENT_H
#define CLIENT_H
#include "network.h"
#include "header.h"
#include "globals.h"
#include <QString>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>
class Client
{
public:
    Client();
};
void connectClient(int port, const char *IP);
void requestUdp(QString fileName, QString destAddr, int destPort, int forwardPort, int myPort);
void requestFile(QString fileName, QString destAddr, QString fileType, int destPort, bool unknown);
void * timeoutSocket(void * args);
void * readThread(void * args);
#endif // CLIENT_H
