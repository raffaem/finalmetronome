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
#ifdef WINDOWS

#include "TrWinMultimedia.h"
using namespace Timer;

#include <mmsystem.h>
#include <windows.h>

void error(const char* message)
{
	MessageBoxA(NULL, message, "Error in WinMultimediaTimer", MB_OK);
}

TrWinMultimedia::TrWinMultimedia()
{
    this->is_active = false;
    
	if( timeGetDevCaps(&tcaps, sizeof(TIMECAPS)) != MMSYSERR_NOERROR)
	{
		error("timeGetDevCaps");
	}
	timeBeginPeriod(tcaps.wPeriodMin);
	//cout << "Min period: " << tcaps.wPeriodMin << " - Max period: " << tcaps.wPeriodMax << endl;
}

TrWinMultimedia::~TrWinMultimedia()
{
	this->stop();
	timeEndPeriod(tcaps.wPeriodMin);
}

void TrWinMultimedia::onStart(unsigned int delay)
{
	/*
	MMRESULT timeSetEvent(
	UINT uDelay,
	UINT uResolution,
	LPTIMECALLBACK lpTimeProc,
	DWORD_PTR dwUser,
	UINT fuEvent
	);
	*/

	timerID = timeSetEvent(
		delay,
		tcaps.wPeriodMin,
		TrWinMultimedia::winMultimediaTimeProc,
		(DWORD_PTR)(&this->callback_data), //user data passed to function
		TIME_PERIODIC /*| TIME_KILL_SYNCHRONOUS*/
		);
	if(timerID == NULL)
	{
		error("in timeSetEvent");
	}
}

void TrWinMultimedia::onStop()
{
	if(timerID != NULL)
	{
		timeKillEvent(timerID);
		timerID = NULL;
	}
}

void CALLBACK TrWinMultimedia::winMultimediaTimeProc(
		UINT uID,
		UINT uMsg,
		DWORD dwUser,
		DWORD dw1,
		DWORD dw2
		)
{
	TimerCallbackData* ptr = (TimerCallbackData*)dwUser;
	(*ptr->usr_func)(ptr->usr_data);
}

#endif
