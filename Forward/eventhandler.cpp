#include "eventhandler.h"

EventHandler::EventHandler()
{

}
void EventHandler::addEvent(requestInfo r){
    theReqs[head%20] = r;
    head++;
}
requestInfo EventHandler::getEvent(){


    return theReqs[tail++%20];
}
