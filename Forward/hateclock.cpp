#include "hateclock.h"
/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: hateclock
--
-- PROGRAM: Protocol
--
-- FUNCTIONS:   long clockC()
--
-- DATE: 10/29/2016
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Colin Bose
--
-- PROGRAMMER: Colin Bose
--
-- NOTES: Clock class. Returns the current time in microseconds
----------------------------------------------------------------------------------------------------------------------*/

long firstSec;
bool set = false;
hateClock::hateClock()
{

}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: clockC()
-- DATE: 29/10/2016
-- REVISIONS: (Date and Description)
-- DESIGNER: Colin Bose
-- PROGRAMMER: Colin Bose
-- INTERFACE:
-- RETURNS: long - current time in microseconds
--
-- NOTES: Call to get current global time in microseconds
----------------------------------------------------------------------------------------------------------------------*/

long clockC(){
    if(!set){
        set = true;
        struct timeval tv;
        gettimeofday(&tv,NULL);
        firstSec = tv.tv_sec;
    }

    struct timeval tv;
    gettimeofday(&tv,NULL);
    long dif = tv.tv_sec - firstSec;
    return 1000000 * dif + tv.tv_usec;
}
