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

#include "ChTickCount.h"

Chronometer::ChTickCount::ChTickCount(void) { }

void Chronometer::ChTickCount::start(void)
{
    this->start_time = GetTickCount();
}

double Chronometer::ChTickCount::getElapsed(void)
{
    return static_cast<double>(GetTickCount() - this->start_time) / 1000.0;
} 

double Chronometer::ChTickCount::reset(void)
{
    double elapsed = this->getElapsed();
    this->start();
    return elapsed;
}

#endif
