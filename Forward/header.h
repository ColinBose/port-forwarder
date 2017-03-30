#ifndef HEADER_H
#define HEADER_H
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <QString>
#define HEADERLEN 75
#define IPSIZE 15
#define REQUESTOFFSET 50
#define FILEOFFSET IPSIZE * 2
#define FILELENGTH 20
#define LENGTHOFFSET 51
#define PORTOFFSET 60
#define RETNO 65
#define PACKID 70
class Header
{
public:
    Header();
};
void addHeader(char buffer[], const char src[], const char dst[], const char fileName[], bool request, int length, int destPort);
int getRetNo(char buffer[]);
void fillSrcPort(char buffer[], int port);
void fillSrcIp(char buffer[], char srcIp[]);
bool fromClient(char buffer[]);
void addSize(char buffer[], long length);
QString getFileName(char buffer[]);
void flipSrcDst(char buffer[]);
bool checkDataReturn(char buffer[]);
int getForwardInfo(char buffer[], char ip[]);
void getFileName(char buffer[], char fileFill[]);
void setSendBack(char buffer[]);
void setUnknown(char buffer[]);
bool isUnknown(char buffer[]);
void getIdNo(char buff[]);
void addIdNo(char buff[], char idNo[]);
void addPackNum(char buff[], int no);
int getHeaderSize(char buffer[]);
#endif // HEADER_H
