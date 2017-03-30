#include "server.h"
#include <QDebug>
Server::Server()
{

}
int sock;
int retSock;
int udpP;
sem_t waitLock;
EventHandler events;
void startServer(int port, int udpPort){
    sockaddr_in  serv;
    udpP = udpPort;
    sem_init(&waitLock, 0,0);
    sock = setupListen(port, &serv);
    listen(sock, 5);
    pthread_t listener, h1,h2,h3, udpListen;
    pthread_create(&listener, NULL, listenThread, (void *)0);
    pthread_create(&h1, NULL, requestHandler, (void *)0);
    pthread_create(&h2, NULL, requestHandler, (void *)0);
    pthread_create(&h3, NULL, requestHandler, (void *)0);
    pthread_create(&udpListen, NULL, listenUdp, (void *)0);

}
void * listenUdp(void * args){
    int udpSock;
    sockaddr_in client,server;
    char buffer[TRANSFERSIZE];
    udpSock = setUdp(udpP,"", &server );
    while(1){
        readSock(udpSock, TRANSFERSIZE, buffer, &client);
    }
}

void * listenThread(void * args){
    sockaddr_in  client;
    socklen_t client_len = sizeof(client);


    if ((retSock = accept (sock, (struct sockaddr *)&client, &client_len)) == -1)
    {
        fprintf(stderr, "Can't accept client\n");
        fprintf(stderr, "socket() failed: %s\n", strerror(errno));
        exit(1);
    }
    int err;
    bool reading = true;
    char buffer[TRANSFERSIZE];
    while(reading){
        err = readSock(retSock, TRANSFERSIZE,buffer);
        if(err == 0)
            break;
        printf("READ SOME DATA AT THE SERVER:::\n");
        fflush(stdout);
        requestInfo r;
        getFileName(buffer, r.fileName);
        setSendBack(buffer);
        memcpy(r.header,buffer, HEADERLEN);
        events.addEvent(r);
        sem_post(&waitLock);

    }
}
void * requestHandler(void * args){
    requestInfo req;
    char header[HEADERLEN];
    zero(header, HEADERLEN);
    char totalPack[TRANSFERSIZE];
    long fSize;
    while(1){
        sem_wait(&waitLock);
        req = events.getEvent();
        fSize = getSize(req.fileName);
        if(fSize == -1){
            printf("No such file");
            fflush(stdout);
            continue;
        }
        memcpy(header, req.header, HEADERLEN);
        addSize(header, fSize);
        flipSrcDst(header);
        qDebug() << "SENDING BACK :: " + QString(req.fileName);
        sendFile(header, req.fileName);
        printf("idk something");

    }
}
void sendFile(char header[], char fName[]){
    FILE * f;
    char sendBuffer[TRANSFERSIZE];
    int sent = 1;
    char packIdBuff[5];
    getIdNo(packIdBuff);
    addIdNo(header, packIdBuff);
    memcpy(sendBuffer, header, HEADERLEN);
    f = fopen(fName, "r");
    int s;
    if(f == NULL)
        return;
    while((s=fread(sendBuffer+HEADERLEN, sizeof(char), TRANSFERSIZE - HEADERLEN, f))> 0){
        addPackNum(sendBuffer, sent);

        sent++;
        sendData(retSock, sendBuffer, TRANSFERSIZE);
        if(s < (TRANSFERSIZE - HEADERLEN))
            break;

    }
    fclose(f);


}

long getSize(char fileName[]){
    FILE * f;

    f = fopen(fileName, "r");
    if(f == NULL)
        return -1;
    fseek(f, 0, SEEK_END);
    return ftell(f);
}
