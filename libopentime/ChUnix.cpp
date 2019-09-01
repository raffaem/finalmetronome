/*
Copyright 2010 Mancuso Raffaele

This file is part of LiveForMusic project.

LiveForMusic project is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

LiveForMusic project is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with LiveForMusic project.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "opentimeconfig.h"

#ifdef UNIX

#include "ChUnix.h"
#include <iostream>
using namespace std;

Chronometer::ChUnix::ChUnix(void) { }

void Chronometer::ChUnix::start(void)
{
    gettimeofday(&this->start_time, NULL);
}

double Chronometer::ChUnix::getElapsed(void)
{
    timeval end_time;
    gettimeofday(&end_time, NULL);
    
    //tv_usec are microseconds (1 microsecond = 1/1000000 seconds)
    const double divisor = 1000000.0;

    /*
    // DEBUG ONLY!!!    
    cout<<"\nStart tv_sec: " << this->start_time.tv_sec <<" tv_usec: " << this->start_time.tv_usec;
    cout<<"\nEnd tv_sec: " << end_time.tv_sec <<" tv_usec: " <<end_time.tv_usec;
    */
         
    double start_seconds = static_cast<double>(this->start_time.tv_sec) + (static_cast<double>(this->start_time.tv_usec) / divisor);
    double end_seconds = static_cast<double>(end_time.tv_sec)  + (static_cast<double>(end_time.tv_usec) / divisor);
   
    return (end_seconds - start_seconds);
} 

double Chronometer::ChUnix::reset(void)
{
    double elapsed = this->getElapsed();
    this->start();
    return elapsed;
}

#endif //#ifdef UNIX
