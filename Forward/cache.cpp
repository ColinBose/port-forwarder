#include "cache.h"

Cache::Cache()
{
    cacheList.push_front(a);
    cacheList.push_front(b);
    cacheList.push_front(c);
}
bool Cache::checkCache(QString fName){
    for(int i = 0; i < cacheList.length(); i++){
        if(cacheList[i].fileName == fName)
            return true;
    }
    return false;
}
void Cache::setCache(QString fName, int trackNum, int totalSize){
 //   if(cacheList.length() >= CACHESIZE)
   //     cacheList.removeAt(CACHESIZE -1);

   // aCache c;
    cacheList[CACHESIZE-1].curPackNo = trackNum;
    cacheList[CACHESIZE-1].fileName = fName;
    cacheList[CACHESIZE-1].fSize = totalSize;
    cacheList[CACHESIZE-1].pointer = 0;
    cacheList.move(CACHESIZE-1, 0);
}
void Cache::addToCache(char buffer[], int size, int trackNum){
    int index = -1;
    for(int i = 0; i < cacheList.length(); i++){
        if(cacheList[i].curPackNo == trackNum){
            index = i;
            break;
        }
    }
    if(index == -1){
        printf("Error adding to cache;;");
        fflush(stdout);
        return;
    }
    if((cacheList[index].pointer + DATASIZE) > cacheList[index].fSize){
        memcpy(cacheList[index].buffer+cacheList[index].pointer, buffer, cacheList[index].fSize - cacheList[index].pointer);
        cacheList[index].pointer+= cacheList[index].fSize - cacheList[index].pointer;

    }else{
        memcpy(cacheList[index].buffer+cacheList[index].pointer, buffer, size);
        cacheList[index].pointer+= size;

    }
}
bool Cache::cacheHas(QString s){
    for(int i = 0; i < cacheList.length(); i++)
        if(cacheList[i].fileName == s)
            return true;
    return false;
}
int Cache::getData(QString name, char buffer[]){
    int index = -1;
    for(int i = 0; i < cacheList.length(); i++){
        if(cacheList[i].fileName == name){
            index = i;
            break;
        }
    }
    if(index == -1)
        return index;
    memcpy(buffer, cacheList[index].buffer, CACHELENGTH);
    return cacheList[index].pointer;
}
