#include "OpenChronometer.h"
#include "OpenTimer.h"

Chronometer::Generic* Chronometer::create()
{
    Chronometer::Generic* ch = NULL;
    
    #ifdef WINDOWS
    ch =  new ChStdClock();
    #endif
    
    #ifdef UNIX
    ch =  new ChUnix();
    #endif
    
    return ch;        
}

Timer::TrGeneric* Timer::create()
{
    Timer::TrGeneric* tr = NULL;
    
    #ifdef WINDOWS
    tr =  new TrWinMultimedia();
    #endif
    
    #ifdef UNIX
    tr =  new TrWxWidgets();
    #endif
    
    return tr;        
}

