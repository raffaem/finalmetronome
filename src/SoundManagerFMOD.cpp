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

#include "SoundManagerFMOD.h"
#include "libopenmusic/wxutils.h"

//use FMOD C interface, and not C++ interface, because the c++ interface is supported
//only under VisualC++

SoundMgrFMOD::SoundMgrFMOD()
{
    this->last_channel = NULL;
    this->system = NULL;
}

SoundMgrFMOD::~SoundMgrFMOD()
{
    if(this->system != NULL)
    {
        FMOD_System_Release(this->system);
        this->system = NULL;
    }
}

void SoundMgrFMOD::setVolume(float value)
{
    this->volume = value;
    if(this->last_channel = NULL)
        FMOD_Channel_SetVolume(this->last_channel, value);
}

void SoundMgrFMOD::ini(ConfigOptions opts)
{
    FMOD_RESULT result;

	// Create the main system object.
    result = FMOD_System_Create(&this->system);
    fmodCheckError(result);

    // Set output type
    result = FMOD_System_SetOutput(this->system, opts.fmod_output_type);
    if(result != FMOD_OK)
    {
        wxString errorstr;
        errorstr << wxT("The audio output type ")<< opts.fmod_output_type << wxT(" \"") << toWxString(fmodOutTypeToStr(opts.fmod_output_type)) << wxT("\" you have selected is not supported. Switching back to default output type");
        customError(errorstr);
        result = FMOD_System_SetOutput(this->system, FMOD_OUTPUTTYPE_AUTODETECT);
        fmodCheckError(result);
    }

    // Check FMOD version
    unsigned int version;
    result = FMOD_System_GetVersion(this->system, &version);
    fmodCheckError(result);
    if (version < FMOD_VERSION)
        customFatalError( wxString::Format(wxT("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n"), version, FMOD_VERSION) );

    // Check drivers
    int numdrivers;
    result = FMOD_System_GetNumDrivers(this->system, &numdrivers);
    fmodCheckError(result);
    if (numdrivers == 0)
    {
        customFatalError( wxString::Format(wxT("Error! Cannot detect any sound driver.")) );
        return;
    }

    FMOD_SPEAKERMODE speakermode;
    FMOD_CAPS        caps;
    result = FMOD_System_GetDriverCaps(this->system, 0, &caps, 0, 0, &speakermode);
    fmodCheckError(result, wxT("Error occured after FMOD_System_GetDriverCaps"));

    /*
    // May implement this!
    result = system->setSpeakerMode(speakermode);       // Set the user selected speaker mode.
    ERRCHECK(result);
    */

    // Check for hardware accelleration
    if (caps & FMOD_CAPS_HARDWARE_EMULATED)
    {
        // The user has the 'Acceleration' slider set to off!  This is really bad for latency!
        customError( wxT("You have the 'Acceleration' slider set to off. Audio hardware acceleration ")
                         wxT("will be emulated. It is highly suggested that you set it to on and restart the ")
                         wxT("program") );
        result = FMOD_System_SetDSPBufferSize(this->system, 1024, 10);
        fmodCheckError(result, wxT("Error occured after FMOD_System_SetDSPBufferSize"));
    }

    // Check for driver info
    char driver_name[256];
    result = FMOD_System_GetDriverInfo(this->system, 0, driver_name, 256, 0);
    fmodCheckError(result);

	// Sigmatel sound devices crackle for some reason if the format is PCM 16bit.  PCM floating point output seems to solve it.
    if (strstr(driver_name, "SigmaTel"))
    {
        customError( wxT("You have a Sigmatel sound device that crackle for some reason if the format is PCM 16bit. I will switch to PCM floating point output, it should solve it.") );
        result = FMOD_System_SetSoftwareFormat(this->system, 48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD_DSP_RESAMPLER_LINEAR);
        fmodCheckError(result, wxT("Error occured after FMOD_System_SetSoftwareFormat"));
    }

    // Initialize FMOD
    result = FMOD_System_Init(this->system, 100, FMOD_INIT_NORMAL, NULL);
    if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)
    {
        // Ok, the speaker mode selected isn't supported by this soundcard.  Switch it back to stereo...
        result = FMOD_System_SetSpeakerMode(this->system, FMOD_SPEAKERMODE_STEREO);
        fmodCheckError(result, wxT("Error occured after FMOD_System_SetSpeakerMode"));
        // ... and re-init.
        result = FMOD_System_Init(this->system, 100, FMOD_INIT_NORMAL, NULL);
        fmodCheckError(result, wxT("Error occured after the second call to FMOD_System_Init. The first returned FMOD_ERR_OUTPUT_CREATEBUFFER"));
    }
    else
        fmodCheckError(result, wxT("Error occured after FMOD_System_Init"));

    this->iniSound(opts);
}

void SoundMgrFMOD::iniSound(ConfigOptions opts)
{
    FMOD_RESULT result;

	for(std::vector<wxString>::iterator it = opts.sound_files.begin();
		it != opts.sound_files.end();
		it++)
	{

		Sound tmpsound;
		tmpsound.file = *it;
		const char* filename = toCharArr(*it);
		FMOD_SOUND* sndptr;

		result = FMOD_System_CreateSound(
					this->system,
					filename,
					FMOD_DEFAULT, //mode
					NULL, //extended information
					&sndptr //sound object
				);
		fmodCheckError(result, 
            wxString(wxT("\nFilename: ")) + toWxString(filename) + wxString(wxT("\nError occured after FMOD_System_CreateSound")));

		this->sounds.push_back(tmpsound);
		this->snd_ptrs.push_back(sndptr);

		delete filename;
	}
}

void SoundMgrFMOD::update()
{
    // We need to check for 'system' being NULL
    // because some wxWidgets ports (like wxGTK2) mess up with function ordering call
    // and so metronomeDlg::OnIdle() is called BEFORE the sound system is initialized
    // wich causes passing a NULL value to FMOD_System_Update with the related crash
    if(this->system != NULL)
    {
        FMOD_RESULT result = FMOD_System_Update(this->system);
        //fmodCheckError(result, wxT("Error occured after FMOD_System_Update"));
    }
}

void SoundMgrFMOD::play(unsigned int snd_index)
{
    if(last_channel != NULL)
        FMOD_Channel_Stop(last_channel);

    // If the sound is NULL, it means this is a pause
    if( this->snd_ptrs[snd_index] == NULL) return;

    FMOD_RESULT result;

    result = FMOD_System_PlaySound(
                 this->system,
                 FMOD_CHANNEL_FREE, //channel id
                 this->snd_ptrs[snd_index], //sound
                 true, //true for paused
                 &this->last_channel //channel handle
             );
    fmodCheckError(result, wxT("Error occured after FMOD_System_PlaySound"));

    result = FMOD_Channel_SetVolume(this->last_channel, this->volume);
    fmodCheckError(result, wxT("Error occured after setVolume"));

    result = FMOD_Channel_SetPaused(this->last_channel, false);
    fmodCheckError(result, wxT("Error occured after setPaused"));
}

void SoundMgrFMOD::addSilentSound()
{
    Sound snd;
    snd.file = wxString( wxT("No Sound") );
    this->sounds.push_back(snd);
    snd_ptrs.push_back(NULL);
}

