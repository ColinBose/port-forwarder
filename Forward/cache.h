#ifndef CACHE_H
#define CACHE_H
#define CACHESIZE 3
#define CACHELENGTH 10000
#include <QString>
#include <QStringList>
#include <QList>
#include "middle.h"
struct aCache{
    int pointer = 0;
    QString fileName;
    int curPackNo = 0;
    char buffer[CACHELENGTH] = {0};
    int fSize;

};

class Cache
{
public:
    Cache();
    bool checkCache(QString fName);
    void setCache(QString fName, int trackNum, int totalSize);
    void addToCache(char buffer[], int size, int trackNum);
    bool cacheHas(QString s);
    int getData(QString name, char buffer[]);

private:
   QList<aCache> cacheList;
   aCache a, b,c;
};

#endif // CACHE_H
