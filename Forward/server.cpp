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

// OpenSSL
SSL_CTX *sslctx;
SSL *ssl;
BIO *sbio;

void startServer(int port, int udpPort){
    // Build the SSL context
    sslctx = initialize_ctx(SVR_KEYFILE, PASSWORD);
    SSL_CTX_set_options(sslctx, SSL_OP_SINGLE_DH_USE);
    generate_eph_rsa_key (sslctx);
    SSL_CTX_set_session_id_context (
        sslctx,
        &s_server_session_id_context,
        sizeof s_server_session_id_context);

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

    QString fileName;

    socklen_t client_len = sizeof(client);
    udpSock = setUdp(udpP,"", &server );
    int err;
    while(1){
        readSock(udpSock, TRANSFERSIZE, buffer, &client,0);
       // recvfrom(udpSock, buffer, TRANSFERSIZE, 0, (struct sockaddr *)&client, &client_len);
       // sendto (udpSock, buffer, err, 0,(struct sockaddr *)&client, client_len);

        //endDataTo(udpSock, TRANSFERSIZE, buffer, &client );
       // continue;
        udpSender send;
        flipSrcDst(buffer);

        swapPort(buffer);
        memcpy(send.header, buffer, HEADERLEN);
        getFileName(buffer, send.fileName);

        send.sock = udpSock;
        send.sender = client;
        pthread_t udpFile;
        pthread_create(&udpFile, NULL, sendFileUdp, (void *)&send);

    }
}
void * sendFileUdp(void * args){
    udpSender * s = (udpSender *) args;
    FILE * f;
    char sendBuffer[TRANSFERSIZE];
    int sent = 1;
    char packIdBuff[5];
    char headerBuff[HEADERLEN];
    memcpy(headerBuff, s->header, HEADERLEN);
    getIdNo(packIdBuff);
    addIdNo(headerBuff, packIdBuff);
    setSendBack(headerBuff);
    memcpy(sendBuffer, headerBuff, HEADERLEN);
    f = fopen(s->fileName, "r");
    int ss;
    if(f == NULL)
        return(void*)0;
    while((ss=fread(sendBuffer+HEADERLEN, sizeof(char), TRANSFERSIZE - HEADERLEN, f))> 0){
        addPackNum(sendBuffer, sent);

        sent++;
        sendDataTo(s->sock,  TRANSFERSIZE,sendBuffer, &s->sender);

        if(ss < (TRANSFERSIZE - HEADERLEN))
            break;

    }
    fclose(f);

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

    sbio = BIO_new_socket (retSock, BIO_NOCLOSE);
    ssl = SSL_new(sslctx);
    SSL_set_bio (ssl, sbio, sbio);

    int sslerr;
    if (sslerr = SSL_accept (ssl) <= 0)
        berr_exit("SSL Accept Error");

    int err;
    bool reading = true;
    char buffer[TRANSFERSIZE];
    // TODO: might have to do specific SSL error checking instead of
    // relying on regular socket errors?
    while(reading){
        err = readSockSSL(retSock, TRANSFERSIZE,buffer, ssl);
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
        sendDataSSL(retSock, sendBuffer, TRANSFERSIZE, ssl);
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
