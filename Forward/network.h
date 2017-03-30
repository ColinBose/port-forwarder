#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
//#define TRANSFERSIZE 1000
#define STARTERLENGTH 200
#define LISTSIZE 10000
#define PORT 7000
#define TRANSFERSIZE 2000
class Network
{
public:
    Network();
};
void setUdp(int port, const char * servIp);
int sendData(int socket, char * message, int buffsize);
int  setUdp(int port, const char * servIp, sockaddr_in * serveraddr);
int connectTCPSocket(int port, const char *ip);
int readSock(int sd, int buffSize, char * buff);
int sendDataTo(int sd, int buffSize, char * buff, sockaddr_in * serveraddr);
int setupListen(int port, sockaddr_in * server);
int readSock(int sd, int buffSize, char * buff, sockaddr_in * serveraddr);
void zero(char * buffer, int len);
int setForwardUDP(int port, const char fIp[], sockaddr_in * client);
#endif // NETWORK_H
