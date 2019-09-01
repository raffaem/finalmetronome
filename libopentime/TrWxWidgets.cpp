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

//depends on the order the compiler choose to cimple the source files, this macro may not be defined
//this is why we need to include timer.h
#ifdef UNIX

#include "TrWxWidgets.h"
using namespace Timer;

TrWxWidgets::TrWxWidgets()
{
    is_active = false;	
}

TrWxWidgets::~TrWxWidgets()
{
	this->stop();
}

void TrWxWidgets::Notify()
{
	this->callFunc();
}

void TrWxWidgets::onStart(unsigned int delay)
{
	this->Start(delay, wxTIMER_CONTINUOUS);
}

void TrWxWidgets::onStop()
{
	this->Stop();
}

#endif
