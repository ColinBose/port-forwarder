#include "header.h"
#include <QDebug>
Header::Header()
{

}
void addHeader(char buffer[], const char src[], const char dst[], const char fileName[], bool request, int length, int destPort){
    memcpy(buffer, src, IPSIZE);
    memcpy(buffer + IPSIZE, dst, IPSIZE);
    memcpy(buffer + FILEOFFSET, fileName, FILELENGTH);
    char len[9];
    sprintf(len,"%d",length);
    if(request){
        memcpy(buffer + REQUESTOFFSET, "R", 1);

    }
    memcpy(buffer + LENGTHOFFSET, len, 9);
    sprintf(len,"%d",destPort);
    memcpy(buffer + PORTOFFSET, len, 5);


}
void fillSrcIp(char buffer[], char srcIp[]){
    memcpy(buffer, srcIp, IPSIZE);

}
void setUnknown(char buffer[]){
    buffer[REQUESTOFFSET] = 'M';
}
bool isUnknown(char buffer[]){
    return buffer[REQUESTOFFSET] == 'M';
}

int getRetNo(char buffer[]){
    char numBuffer[5];
    memcpy(numBuffer, buffer+PACKID, 5);
    return atoi(numBuffer);
}
int getHeaderSize(char buffer[]){
    char numBuffer[9];
    memset(numBuffer,'\0', 9);
    memcpy(numBuffer, buffer+LENGTHOFFSET, 9);
    return atoi(numBuffer);
}

void addSize(char buffer[], long length){
    char lenHold[9];
    sprintf(lenHold, "%u", length);
    memcpy(buffer+LENGTHOFFSET, lenHold, 9);
}

bool fromClient(char buffer[]){

    return buffer[REQUESTOFFSET] == 'R';
}
void fillSrcPort(char buffer[], int port){
    char portThing[5];
    memset(portThing, '/0', 5);
    sprintf(portThing, "%d", port);
    memcpy(buffer + PORTOFFSET, portThing, 5);

}

int getForwardInfo(char buffer[],char ip[]){
    strcpy(ip, buffer + IPSIZE);
    char portHold[5];
    strcpy(portHold, buffer+PORTOFFSET);

    return atoi(portHold);
}
void getFileName(char buffer[], char fileFill[]){
    memcpy(fileFill, buffer+FILEOFFSET, FILELENGTH);
}
void flipSrcDst(char buffer[]){
    char holdSrc[15];
    char holdDst[15];
    memcpy(holdSrc, buffer, 15);
    memcpy(holdDst, buffer+15, 15);
    memcpy(buffer, holdDst, 15);
    memcpy(buffer+ 15, holdSrc, 15);


}
void setSendBack(char buffer[]){
    buffer[REQUESTOFFSET] = 'Z';
}
bool checkDataReturn(char buffer[]){
    return buffer[REQUESTOFFSET] == 'Z';
}
QString getFileName(char buffer[]){
    QString ret(buffer + FILEOFFSET);
 //   qDebug() << "The file Name: " + ret;
    return ret;
}

void getIdNo(char buff[]){
    srand(time(NULL));
    int c;
    for(int i = 0; i < 5; i++){
        c = rand() %10;
        c+= 48;
        buff[i] = (char)c;
    }

}
void addPackNum(char buff[], int no){
    char pNum[5];
    sprintf(pNum, "%d", no);
    memcpy(buff + RETNO, pNum, 5);
}

void addIdNo(char buff[], char idNo[]){
    memcpy(buff+PACKID,idNo, 5 );
}
