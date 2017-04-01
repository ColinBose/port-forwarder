#include "client.h"
#include "middle.h"
Client::Client()
{

}
int connectSock;

SSL_CTX *sslctx2;
SSL *ssl2;
BIO *sbio2;

void connectClient(int port, const char * IP){
    connectSock = connectTCPSocket(port, IP);

    if (connectSock < 0) {
        exit(0);
    }
    sslctx2 = initialize_ctx(CLT_KEYFILE, PASSWORD);
    ssl2 = SSL_new(sslctx2);

    sbio2 = BIO_new_socket (connectSock, BIO_NOCLOSE);
    SSL_set_bio (ssl2, sbio2, sbio2);
    if (SSL_connect (ssl2) < 0) {
        fprintf(stderr, "first failed: %s %d \n", strerror(errno),errno);

        berr_exit ("SSL Connect Error!");
    }

    char retBuffer[TRANSFERSIZE];
    readSockSSL(connectSock, TRANSFERSIZE, retBuffer, ssl2);
    QString fList(retBuffer);
    QStringList parts = fList.split(" ");
    for(int i = 0; i < parts.length(); i++){
        qDebug() << parts[i];
        QStringList s = parts[i].split("-");
        if(s.length() != 2)
            continue;
        QMetaObject::invokeMethod(mw, "addFileRow", Q_ARG(QString, s[0]), Q_ARG(QString, s[1]));
    }

}
void requestUdp(QString fileName, QString destAddr, QString forwardIp, int destPort, int forwardPort, int myPort){
    char buffer[TRANSFERSIZE];
    zero(buffer, TRANSFERSIZE);
    char src[15];
    char dst[15];
    zero(src, 15);
    zero(dst, 15);
    strcpy(src, "127.0.0.1");
    strcpy(dst, forwardIp.toStdString().c_str());
    addHeader(buffer, src, dst, fileName.toStdString().c_str(), true, 0, forwardPort);
    fillSrcPort(buffer, 7777);

    sockaddr_in client, server;
    int sendUdpSock;
    sendUdpSock = setUdp(myPort,"", &server );
    setForwardUDP(destPort, destAddr.toStdString().c_str(),  &client);


    sendDataTo(sendUdpSock, TRANSFERSIZE, buffer, &client);

    int e;
    FILE * f;
    f = fopen("outFile", "w");
    if(f == NULL){
        exit(1);
    }
    while(1){

        e = readSock(sendUdpSock, TRANSFERSIZE, buffer,&server, 100000 );
        if(e == 0)
            break;
        printf("Read something\n");
        fflush(stdout);
        fwrite(buffer+HEADERLEN, sizeof(char), DATASIZE, f);

    }
    printf("DONE READING");
    fflush(stdout);
    fclose(f);
    close(sendUdpSock);

}
void * timeoutSocket(void * args){
    int * sock = (int * )args;
    long curTime;
    long getTime = clockC() + 100000;
    while(1){
        curTime = clockC();
        if(curTime > getTime){
            close(*sock);
            printf("Sock closed");
            fflush(stdout);
            exit(1);
        }

    }
}

void requestFile(QString fileName, QString destAddr, QString fileType, int destPort, bool unknown){
    char buffer[TRANSFERSIZE];
    zero(buffer, TRANSFERSIZE);
    char src[15];
    char dst[15];
    zero(src, 15);
    zero(dst, 15);
    strcpy(src, "127.0.0.1");
    strcpy(dst, destAddr.toStdString().c_str());
    addHeader(buffer, src, dst, fileName.toStdString().c_str(), true, 0, destPort);
    if(unknown){
        setUnknown(buffer);
    }
    sendDataSSL(connectSock, buffer, TRANSFERSIZE, ssl2);
    pthread_t reader;
    pthread_create(&reader, NULL, readThread, (void*)&fileType);
  //  readSock(connectSock,  TRANSFERSIZE, buffer);
    printf("Received a response\n");
    fflush(stdout);
}
void * readThread(void * args){
    QString& fileType = *(QString *)args;
    int fSize;
    char retBuffer[TRANSFERSIZE];
    FILE * outFile;
    int totalWrite = 0;
    outFile = fopen("outFile", "w");
    if(outFile == NULL){
        printf("ERROR OPENING FILE;;;QUITING");
        fflush(stdout);
        exit(1);
    }

    readSockSSL(connectSock, TRANSFERSIZE, retBuffer, ssl2);
    fSize = getHeaderSize(retBuffer);
    if(fSize < DATASIZE){
        fwrite(retBuffer+HEADERLEN, sizeof(char), fSize, outFile);
        fclose(outFile);
        QMetaObject::invokeMethod(mw, "setTextView", Q_ARG(QString, retBuffer+ HEADERLEN));
        return(void*)0;
    }
    else{
        fwrite(retBuffer+HEADERLEN, sizeof(char), DATASIZE, outFile);
    }
    totalWrite += DATASIZE;
    while(1){
        readSockSSL(connectSock, TRANSFERSIZE, retBuffer, ssl2);
        totalWrite += DATASIZE;
        if(totalWrite > fSize){
            fwrite(retBuffer+HEADERLEN, sizeof(char), fSize -(totalWrite-DATASIZE), outFile);
            break;
        }
        fwrite(retBuffer+HEADERLEN, sizeof(char), DATASIZE, outFile);
    }
    fclose(outFile);

    if (QString::compare(fileType, QString("image"), Qt::CaseInsensitive) == 0) {
        QMetaObject::invokeMethod(mw, "setImageView", Q_ARG(QString, retBuffer+ HEADERLEN));
    }

    return(void*)0;
}
