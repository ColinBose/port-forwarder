#include "middle.h"
#define EPOLLMAX 10000
#define DEFAULTPORT 555
#define UDPPORT 8000
#define MAXLIST 5000
#define STDPORT 7000
QList<Files> fileLookup;
Middle::Middle()
{

}
struct forwardPorts{
    int val = 0;
    int sock = 0;
    char ip[15] = {0};
    int port;
    SSL *ssl;
    BIO *sbio;
};
struct sockData{
    int sock;
    char ip[15];
    int val;
    int srcPort;
};
Cache theCache;
forwardPorts portList[MAXLIST];
int totalPorts = 0;
int epoll_fd;
static struct epoll_event events[EPOLLMAX], event;


void startForwarder(){
    pthread_t accepterThread, pollerThread, udpReader;
    pthread_create(&accepterThread, NULL, acceptThread, (void *)0);
    pthread_create(&pollerThread, NULL, pollThread, (void *)0);
    pthread_create(&udpReader, NULL, udpThread, (void *)0);
    connectServers();
}
void connectServers(){
    FILE * f;
    size_t len;
    int read;
    f = fopen("portConfig", "r");
    if(f == NULL){
        printf("FILE CANT OPEN");
        fflush(stdout);
        exit(1);
    }

    char * line = NULL;
    while ((read = getline(&line, &len, f)) != -1) {
       QString liner(line);
       QStringList parts = liner.split(" ");
       if(parts.length() == 1)
           continue;

       // OpenSSL specific variables
       SSL_CTX *sslctx;
       SSL *ssl;
       BIO *sbio;

       // Build the SSL context
       sslctx = initialize_ctx(CLT_KEYFILE, PASSWORD);
       ssl = SSL_new(sslctx);

       int sock = connectTCPSocketSSL(parts.at(1).toInt(), parts[0].toStdString().c_str(), ssl, sbio);
       if(sock != -1){

           portList[sock%MAXLIST].sock = sock;
           portList[sock%MAXLIST].val = getHash(parts[0], parts[1].toInt());
           portList[sock%MAXLIST].ssl = ssl;
           portList[sock%MAXLIST].sbio = sbio;
           if(parts[1][parts.length()-1] == '\n')
               parts[1] = parts[1].left(parts[1].length() - 1);
           QString wtf = parts[1];
           int ok = wtf.toInt();
           portList[sock%MAXLIST].port = ok;
           memcpy(portList[sock%MAXLIST].ip,parts[0].toStdString().c_str(),15);

           if (fcntl (sock, F_SETFL, O_NONBLOCK | fcntl(sock, F_GETFL, 0)) == -1)
               fprintf(stderr, "socket() failed: %s %d\n", strerror(errno), errno);

           event.data.fd = sock;
           event.events = EPOLLIN;
           if (epoll_ctl (epoll_fd, EPOLL_CTL_ADD, sock, &event) == -1)
               fprintf(stderr, "socket() failed: %s\n", strerror(errno));

           QMetaObject::invokeMethod(mw, "addNetwork", Q_ARG(QString,parts[0]), Q_ARG(int,parts[1].toInt()));

            QString fs, s;
            fs = parts[0];
            s = parts[1];
           for(int i = 1; i < parts.length(); i++){
               QStringList adds = parts[i].split('-');
               if(adds.length() != 2)
                   continue;
               if(adds[1][(adds[1].length() -1 )] == '\n')
                   adds[1] = adds[1].left(adds[1].length() -1);

               Files f(adds[0], sock, adds[1], fs, s.toInt());
               fileLookup.push_back(f);
           }
       }

    }
}
int getHash(QString s, int port){
    QStringList parts;
    parts = s.split(".");
    int total = 0;
    int facts[5] = { 3, 5, 7, 9, 11};
    for(int i = 0; i < parts.length(); i++){
        total += parts[i].toInt() * facts[i];
    }
    total += port * 13;
    return total % 1000;
}
void * udpThread(void * args){
    int udpSock;
    sockaddr_in read, clientInfo, clientSend;
    int fPort, forwardSock;
    char fIp[15];
    char buffer[TRANSFERSIZE];
    udpSock = setUdp(UDPPORT, "",&read);
    while(1){
        zero(buffer, TRANSFERSIZE);
        readSock(udpSock, TRANSFERSIZE, buffer,&clientInfo );
        fPort = getForwardInfo(buffer, fIp);
        fillSrcIp(buffer, inet_ntoa(clientInfo.sin_addr));
        fillSrcPort(buffer, STDPORT);
        forwardSock = setForwardUDP(fPort, fIp, &clientSend);
        sendDataTo(forwardSock, TRANSFERSIZE, buffer, &clientSend);




    }
}

void * acceptThread(void * args){

    epoll_fd = epoll_create(EPOLLMAX);
    if (epoll_fd == -1)
        qDebug() << "Error creating EPOLL";
    qDebug() << "Thread up";
    struct	sockaddr_in server, client;
    int sd;
    socklen_t client_len;
       sd = setupListen(PORT+1, &server);

    listen(sd, 5);
    client_len = sizeof(client);
    int someSock;
    char fileBuffer[TRANSFERSIZE];
    char srcIp[15];
    while (1){

        if ((someSock = accept (sd, (struct sockaddr *)&client, &client_len)) == -1)
        {
            fprintf(stderr, "Can't accept client\n");
            fprintf(stderr, "socket() failed: %s\n", strerror(errno));
            exit(1);
        }

        if (fcntl (someSock, F_SETFL, O_NONBLOCK | fcntl(someSock, F_GETFL, 0)) == -1)
            fprintf(stderr, "socket() failed: %s %d\n", strerror(errno), errno);

        // Add the new socket descriptor to the epoll loop

        event.events = EPOLLIN;
        //event.data.fd = someSock;
        //sockData d;
        //d.sock = someSock;

        strcpy(portList[someSock%MAXLIST].ip,inet_ntoa(client.sin_addr));
        event.data.fd = someSock;
        portList[someSock%MAXLIST].val = getHash(QString(portList[someSock%MAXLIST].ip), DEFAULTPORT);
        portList[someSock%MAXLIST].sock = someSock;
        portList[someSock%MAXLIST].port = DEFAULTPORT;
        if (epoll_ctl (epoll_fd, EPOLL_CTL_ADD, someSock, &event) == -1)
            fprintf(stderr, "socket() failed: %s\n", strerror(errno));

        QMetaObject::invokeMethod(mw, "addClient", Q_ARG(QString, inet_ntoa(client.sin_addr)), Q_ARG(int, DEFAULTPORT));
        genFileList(fileBuffer);
        sendData(someSock, fileBuffer, TRANSFERSIZE);


    }
}
void genFileList(char buffer[]){
    QString total = "";
    for(int i  = 0 ; i < fileLookup.length(); i++){
        total += fileLookup[i].fName + "-" + fileLookup[i].service + " ";

    }
    zero(buffer, TRANSFERSIZE);
    memcpy(buffer, total.toStdString().c_str(), total.length());
}

void * pollThread(void * args){
    int num_fds;
    char buffer[TRANSFERSIZE];
    int err;
    int retNumber;
    int fPort;
    int fSize;
    char fIp[15];
    int passSock;
    while (1){
        num_fds = epoll_wait (epoll_fd, events, EPOLLMAX, -1);
        if (num_fds < 0){
           continue;

        }
        for (int i = 0; i < num_fds; i++){

                // Case 1: Error condition
            if (events[i].events & (EPOLLHUP | EPOLLERR)){
                close(events[i].data.fd);
                printf("Closing socket");
                fflush(stdout);
                continue;
            }
            if(events[i].events & EPOLLIN){
                zero(buffer, TRANSFERSIZE);

                err = readSockSSL(events[i].data.fd, TRANSFERSIZE, buffer, portList[events[i].data.fd%MAXLIST].ssl);
                if(err <= 0){
                    QMetaObject::invokeMethod(mw, "removeNetwork", Q_ARG(QString, portList[events[i].data.fd%MAXLIST].ip), Q_ARG(int,portList[events[i].data.fd%MAXLIST].port ));
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, events);
                    printf("removing socket");
                    fflush(stdout);
                    continue;
                }
                if(isUnknown(buffer)){
                    bool found = false;
                    QString fileName = getFileName(buffer);
                    for(int i = 0; i < fileLookup.length(); i++){
                        if(fileLookup[i].fName == fileName){
                            found = true;
                            passSock = fileLookup[i].sock;
                            memcpy(fIp,fileLookup[i].ip.toStdString().c_str(), 15);
                            fPort = fileLookup[i].port;
                            break;
                        }
                    }
                    if(!found)
                        continue;

                }else{
                    fPort = getForwardInfo(buffer, fIp);
                    if(!fromClient(buffer)){
                        fPort = DEFAULTPORT;
                    }
                    passSock = checkForwards(fIp, fPort);
                    //HANDLE UNKNOWN FORWARD ADDRESS/PORT
                    if(passSock == -1){
                        continue;
                    }
                }
                printf("THE SOCKET IS: %d\n", events[i].data.fd);
                fflush(stdout);
                if(checkDataReturn(buffer)){

                    QString fileName = getFileName(buffer);
                    QMetaObject::invokeMethod(mw, "addPacket", Q_ARG(QString, portList[events[i].data.fd%MAXLIST].ip), Q_ARG(QString, fIp),Q_ARG(int, portList[events[i].data.fd%MAXLIST].port)
                            , Q_ARG(int,fPort), Q_ARG(bool, false));

                    fSize = getHeaderSize(buffer);
                    printf("%d\n", fSize);
                    if(fSize < CACHELENGTH){
                        retNumber = getRetNo(buffer);
                        if(theCache.checkCache(fileName)){
                            theCache.addToCache(buffer+HEADERLEN, TRANSFERSIZE - HEADERLEN, retNumber);
                        }
                        else{
                            theCache.setCache(fileName, retNumber, getHeaderSize(buffer));
                            theCache.addToCache(buffer+HEADERLEN, TRANSFERSIZE - HEADERLEN, retNumber);

                        }
                    }

                }
                else{
                    //File request from client//

                    QString fileName = getFileName(buffer);
                    if(theCache.cacheHas(fileName)){
                        QMetaObject::invokeMethod(mw, "addPacket", Q_ARG(QString, portList[events[i].data.fd%MAXLIST].ip), Q_ARG(QString, fIp),Q_ARG(int, portList[events[i].data.fd%MAXLIST].port)
                                , Q_ARG(int,fPort), Q_ARG(bool, true));
                        pthread_t sCache;
                        pthread_create(&sCache, NULL, cachedSend, new passInfo(fileName, events[i].data.fd, fPort, fIp));
                        continue;
                    }
                    QMetaObject::invokeMethod(mw, "addPacket", Q_ARG(QString, portList[events[i].data.fd%MAXLIST].ip), Q_ARG(QString, fIp), Q_ARG(int, portList[events[i].data.fd%MAXLIST].port)
                            , Q_ARG(int,fPort),Q_ARG(bool, false));



                }


                fillSrcIp(buffer, portList[events[i].data.fd%MAXLIST].ip);
                sendDataSSL(passSock, buffer, TRANSFERSIZE, portList[events[i].data.fd%MAXLIST].ssl);


                    //sem_post(&readWait);
            }
        }
    }
}
int checkForwards(char fIp[], int fPort){
    int t = getHash(fIp, fPort);
    for(int i =0 ; i < MAXLIST; i++){
        if(portList[i].val == t)
            return portList[i].sock;
    }
    return -1;
}
void * cachedSend(void * args){
    passInfo * p = (passInfo *)args;
    QString fName;
    int fSock;
    QString srcIp = p->srcIp;
    fName = p->fileName;
    fSock = p->sock;
    int srcPort = p->srcPort;
    char buffer[CACHELENGTH];
    char sendBuffer[TRANSFERSIZE];
    int len, sLen;
    int point = 0;
    len = theCache.getData(fName, buffer);
    bool done = true;
    sLen = len;
    if(len == -1){
        printf("Error getting data from cache;;;");
        fflush(stdout);
        return(void *)0;
    }
    while(done){
        zero(sendBuffer,TRANSFERSIZE);
        addSize(sendBuffer,len);
        if(sLen >= DATASIZE){
            memcpy(sendBuffer+HEADERLEN, buffer+point, DATASIZE );
            point += DATASIZE;
            sLen -= DATASIZE;

        }
        else{
            memcpy(sendBuffer+HEADERLEN, buffer+point, sLen);
            done = false;
        }
        sendData(fSock, sendBuffer, TRANSFERSIZE);
        QMetaObject::invokeMethod(mw, "addPacket",  Q_ARG(QString, srcIp), Q_ARG(QString, portList[fSock].ip), Q_ARG(int, srcPort)
                , Q_ARG(int,DEFAULTPORT), Q_ARG(bool, true));


    }

}
