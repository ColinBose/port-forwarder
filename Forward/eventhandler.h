#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H
#include "header.h"

struct requestInfo{
    char fileName[20] = {0};
    int retSock;
    char header[HEADERLEN];
};
class EventHandler
{
public:
    EventHandler();
    requestInfo getEvent();
    void addEvent(requestInfo r);
private:
    requestInfo theReqs[20];
    int head = 0;
    int tail = 0;
};



#endif // EVENTHANDLER_H
