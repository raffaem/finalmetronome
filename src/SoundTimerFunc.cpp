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

#include "SoundTimerFunc.h"
#include "MetronomeDlg.h"
#include "libopenmusic/wxutils.h"

void metroSoundOnTime(void* data)
{
	metronomeDlg* metro_class = (metronomeDlg*)data;

	metro_class->beatmgr++;
	unsigned int curr_beat = metro_class->beatmgr.getCurrentBeat();

    wxString beat_str;
    beat_str << toWxString(metro_class->beatmgr.getString());
    //metro_class->WxCurrentBeat->SetLabel(beat_str);

    metro_class->soundmgr->play( metro_class->beat_sound_map[curr_beat] );

    // Syncronize only if this is not a subbeat
    if( !metro_class->beatmgr.isSubBeat(curr_beat) )
        metro_class->metro_anim->syncronize(curr_beat);

    // Reset curr_measure_beat that starts from 0
    metro_class->beatmgr.normalize();

   // Post a custom main beat event
   wxCommandEvent event( MYEVT_MAINBEAT );
   event.SetString(beat_str);

   metro_class->GetEventHandler()->AddPendingEvent(event);

   //another method to post an event
   //wxPostEvent( metro_class->GetEventHandler(), event );

   return;
}
