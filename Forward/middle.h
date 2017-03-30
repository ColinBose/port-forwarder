#ifndef MIDDLE_H
#define MIDDLE_H
#include <semaphore.h>
#include <pthread.h>
#include <QDebug>
#include "cache.h"
#include <stdio.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "network.h"
#include <fcntl.h>
#include <assert.h>
#include "limits.h"
#include "mainwindow.h"
#define DATASIZE (TRANSFERSIZE - HEADERLEN)

class Middle
{
public:
    Middle();
};
struct Files{
    QString fName;
    int sock;
    QString service;
    QString ip;
    int port;
    Files(QString n, int s, QString t, QString i, int p) : fName(n), sock(s), service(t), ip(i), port(p){}
};
struct passInfo{
    QString fileName;
    int sock;
    int srcPort;
    QString srcIp;
    passInfo(QString f, int s, int p, QString src) : fileName(f) , sock(s), srcPort(p), srcIp(src) {}
};

void * acceptThread(void * args);
void * pollThread(void * args);
void genFileList(char buffer[]);
int getHash(QString s, int sock);
void connectServers();
int checkForwards(char fIp[], int fPort);
void startForwarder();
void * udpThread(void * args);
void * cachedSend(void * args);
#endif // MIDDLE_H
