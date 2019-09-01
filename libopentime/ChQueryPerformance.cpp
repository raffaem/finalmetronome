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

#ifdef WINDOWS

#include "ChQueryPerformance.h"
#define NO_THREAD_AFFINITY

Chronometer::ChQueryPerformance::ChQueryPerformance(void) 
{
	this->frequency = this->getFrequency();
}

long double Chronometer::ChQueryPerformance::getFrequency(void)
{
    LARGE_INTEGER proc_freq;
    if (!::QueryPerformanceFrequency(&proc_freq))
    { 
        //throw exception(TEXT("QueryPerformanceFrequency() failed"));
        MessageBoxA(NULL, "High resolution timer not supported by this system!!", "Error!", MB_OK);
    }
    return static_cast<long double>(proc_freq.QuadPart);
}

void Chronometer::ChQueryPerformance::start(void)
{
#ifndef NO_THREAD_AFFINITY
    DWORD_PTR oldmask = ::SetThreadAffinityMask(::GetCurrentThread(), 0);
#endif

    ::QueryPerformanceCounter(&this->start_time);

#ifndef NO_THREAD_AFFINITY
    ::SetThreadAffinityMask(::GetCurrentThread(), oldmask);
#endif
}

double Chronometer::ChQueryPerformance::getElapsed(void)
{
    LARGE_INTEGER stop_time;
    
#ifndef NO_THREAD_AFFINITY
    DWORD_PTR oldmask = ::SetThreadAffinityMask(::GetCurrentThread(), 0);
#endif

    ::QueryPerformanceCounter(&stop_time);

#ifndef NO_THREAD_AFFINITY
    ::SetThreadAffinityMask(::GetCurrentThread(), oldmask);
#endif

    return static_cast<double>( (long double)(stop_time.QuadPart - start_time.QuadPart) / this->frequency);
} 

double Chronometer::ChQueryPerformance::reset(void)
{
    double elapsed = this->getElapsed();
    this->start();
    return elapsed;
}

#endif //#ifdef WINDOWS
