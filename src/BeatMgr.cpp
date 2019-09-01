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

#include "BeatMgr.h"

const double milliseconds_per_minute = 60.0 * 1000.0;

bool BeatMgr::isSubBeat(unsigned int _beat)
{
	if(this->group == ONENOTE) 
	{
		return false;
	}
    
	else if( this->getSubBeatInfo(_beat) == 1 ) 
	{
		return false;
	}
    
    return true;
}

double BeatMgr::calculateDurationOfABeatd(unsigned int bpm)
{
    // bpm : milliseconds_per_minute = 1 : x
        
    // How many milliseconds takes a beat
	double period_milliseconds_d = milliseconds_per_minute / static_cast<double>(bpm);
	
	return period_milliseconds_d;
}

unsigned int BeatMgr::calculateDurationOfABeat(unsigned int bpm)
{
	double period_milliseconds_d = BeatMgr::calculateDurationOfABeatd(bpm);
    
	// Arrotonda per difetto
	// Floor returns a double for strange reasons
	unsigned int period_milliseconds = static_cast<unsigned int>(floor(period_milliseconds_d));
	
	return period_milliseconds;
}
