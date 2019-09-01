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

//---------------------------------------------------------------------------
//
// Name:        metronomeApp.cpp
// Author:      Raffaele
// Created:     15/02/2010 4.37.57
// Description:
//
//---------------------------------------------------------------------------

#include "MetronomeApp.h"
#include "MetronomeDlg.h"
#include "libopentime/opentimeconfig.h"

#ifndef WIN32
#include "config.h"
#else
#define WINDOWS
#endif

#ifdef WINDOWS
    #include <windows.h>
    #define PACKAGE_VERSION "0.0.8"
#endif

IMPLEMENT_APP(metronomeDlgApp)

bool metronomeDlgApp::OnInit()
{
    #ifdef WINDOWS
        //SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
    #endif
	
	metronomeDlg* dialog = new metronomeDlg(
		NULL, //parent
		-1, //default ID
		wxString(wxT("FinalMetronome ")) + wxString(wxT(PACKAGE_VERSION)), //title
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxMAXIMIZE_BOX| wxMINIMIZE_BOX | wxCLIP_CHILDREN | wxRESIZE_BORDER //style
	);
	//metronomeDlg* dialog = new metronomeDlg(NULL);

	SetTopWindow(dialog);
	dialog->Show(true);
	return true;
}

int metronomeDlgApp::OnExit()
{
	return 0;
}
