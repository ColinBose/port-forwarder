#include "client.h"
#include "middle.h"
Client::Client()
{

}
int connectSock;

void connectClient(int port, const char * IP){
    connectSock = connectTCPSocket(port, IP);
    char retBuffer[TRANSFERSIZE];
    readSock(connectSock, TRANSFERSIZE, retBuffer);
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
void requestUdp(QString fileName, QString destAddr, int destPort, int forwardPort){
    char buffer[TRANSFERSIZE];
    zero(buffer, TRANSFERSIZE);
    char src[15];
    char dst[15];
    zero(src, 15);
    zero(dst, 15);
    strcpy(src, "127.0.0.1");
    strcpy(dst, destAddr.toStdString().c_str());
    addHeader(buffer, src, dst, fileName.toStdString().c_str(), true, 0, forwardPort);


    sockaddr_in client;
    int sendUdpSock;
    sendUdpSock = setForwardUDP(destPort, destAddr.toStdString().c_str(),  &client);


    sendDataTo(sendUdpSock, TRANSFERSIZE, buffer, &client);

}

void requestFile(QString fileName, QString destAddr, int destPort, bool unknown){
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
    sendData(connectSock, buffer, TRANSFERSIZE);
    pthread_t reader;
    pthread_create(&reader, NULL, readThread, (void*)0);
  //  readSock(connectSock,  TRANSFERSIZE, buffer);
    printf("Received a response\n");
    fflush(stdout);
}
void * readThread(void * args){
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

    readSock(connectSock, TRANSFERSIZE, retBuffer);
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
        readSock(connectSock, TRANSFERSIZE, retBuffer);
        totalWrite += DATASIZE;
        if(totalWrite > fSize){
            fwrite(retBuffer+HEADERLEN, sizeof(char), fSize -(totalWrite-DATASIZE), outFile);
            break;
        }
        fwrite(retBuffer+HEADERLEN, sizeof(char), DATASIZE, outFile);


    }
    fclose(outFile);
    return(void*)0;
}
