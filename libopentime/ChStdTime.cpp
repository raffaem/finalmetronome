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
#include "ChStdTime.h"

Chronometer::ChStdTime::ChStdTime(void) { }

void Chronometer::ChStdTime::start(void)
{
    this->start_time = time(NULL);
}

double Chronometer::ChStdTime::getElapsed(void)
{
    //difftime returns seconds
    return difftime(time(NULL), this->start_time);
} 

double Chronometer::ChStdTime::reset(void)
{
    double elapsed = this->getElapsed();
    this->start();
    return elapsed;
}
