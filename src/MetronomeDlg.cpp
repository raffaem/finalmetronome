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

///-----------------------------------------------------------------
///
/// @file      metronomeDlg.cpp
/// @author    Raffaele Mancuso
/// Created:   15/02/2010 4.37.58
/// @section   DESCRIPTION
///            metronomeDlg class implementation
///
///------------------------------------------------------------------

#include <ctime>
#include <cmath>
#include <string>
#include <iostream>
#include <algorithm>

#include <wx/dcclient.h> //define wxPaintDC
#include <wx/dcmemory.h> //define wxMemoryDC
#include <wx/stdpaths.h> //define wxStandardPaths
#include <wx/file.h>
#include <wx/ffile.h>

#include "MetronomeDlg.h"
#include "BeatSoundMapDlg.h"
#include "SoundTimerFunc.h"
#include "XmlParserConfig.h"
#include "SoundManagerFMOD.h"
#include "libopenmusic/utils.h"

#ifdef __DEBUG__
	#include <fstream>
	std::ofstream evt_file("metrodlg_evt.txt");
#endif

using namespace std;

bool winback_need_update = false;
wxImage window_background_img;
wxBitmap* window_background_bmp;

//Do not add custom headers
//wxDev-C++ designer will remove them
////Header Include Start
////Header Include End

//----------------------------------------------------------------------------
// metronomeDlg
//----------------------------------------------------------------------------
const wxEventType MYEVT_MAINBEAT = wxNewEventType();

//Add Custom Events only in the appropriate block.
//Code added in other places will be removed by wxDev-C++
////Event Table Start
BEGIN_EVENT_TABLE(metronomeDlg,wxDialog)
    ////Manual Code Start
    EVT_IDLE(metronomeDlg::OnIdle)
    EVT_ERASE_BACKGROUND(metronomeDlg::OnEraseBackground)
    MYEVT_ONMAINBEAT( wxID_ANY, metronomeDlg::OnMainBeat )
    ////Manual Code End

    EVT_CLOSE(metronomeDlg::OnClose)
    EVT_SIZE(metronomeDlg::metronomeDlgSize)
    EVT_PAINT(metronomeDlg::metronomeDlgPaint)

    EVT_BUTTON(ID_WXTAPTEMPO,metronomeDlg::WxTapTempoClick)
    EVT_BUTTON(ID_WXSHOWBEATSOUNDMAP,metronomeDlg::WxShowBeatSoundMapClick)
    EVT_COMBOBOX(ID_WXBEATSUBDIVISION,metronomeDlg::WxBeatSubdivisionSelected)
    EVT_SPINCTRL(ID_WXBEATSINAMEASURE,metronomeDlg::WxBeatsInAMeasureUpdated)
    EVT_COMMAND_SCROLL(ID_WXVOLUME,metronomeDlg::WxVolumeScroll)
    EVT_SPINCTRL(ID_WXBPMCTRL,metronomeDlg::WxBPMUpdated)
    EVT_BUTTON(ID_WXPLAYBUTTON,metronomeDlg::WxPlayClick)
	
END_EVENT_TABLE()
////Event Table End

void metronomeDlg::updateBeatSoundMap()
{
    // Update the beat->sound map
    size_t snd_vec_size = soundmgr->sounds.size();
    unsigned int beats_per_measure = this->beatmgr.getTotalBeatsCount();

    // Get the element with the highest key in the map
    std::pair<int,int> last_key = *this->beat_sound_map.rbegin();
    if (last_key.first == beats_per_measure) return;

    // If key's beat is lower than the current beat .....
    if (last_key.first < beats_per_measure)
    {
        // .....we need to make the map bigger
        inirandom();
        unsigned int diff = beats_per_measure - last_key.first;
        for (int i=1; i<=diff; i++)
        {
            unsigned int beat = last_key.first + i;
            unsigned int snd_index;

            if (snd_vec_size >= beat)
                snd_index = beat-1;
            else
                snd_index = random(0, snd_vec_size-1);

            this->beat_sound_map[beat] = snd_index;
        }
    }

    // If the ebat_map window is open, then update it
    if (this->beat_map_dlg != NULL)
        this->beat_map_dlg->updateList();
}

metronomeDlg::metronomeDlg(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
        : wxDialog(parent, id, title, position, size, style)
{
    //initialize this BEFORE calling CreateGUIControls to avoid all possible errors
    this->is_playing = false;
    this->beatmgr.setMainBeats(4);
    this->metro_anim = NULL;
    this->beat_map_dlg = NULL;
    this->beat_sound_map[0] = 0;
    this->soundmgr = new SoundMgrFMOD();

    CreateGUIControls();
}

metronomeDlg::~metronomeDlg()
{
    delete this->soundmgr;
}

void metronomeDlg::CreateGUIControls()
{
    //Do not add custom code between
    //GUI Items Creation Start and GUI Items Creation End.
    //wxDev-C++ designer will remove them.
    //Add the custom code before or after the blocks
    ////GUI Items Creation Start

    WxPlay = new wxButton(this, ID_WXPLAYBUTTON, wxT("Play"), wxPoint(13, 159), wxSize(137, 70), 0, wxDefaultValidator, wxT("WxPlay"));

    WxBPM = new wxSpinCtrl(this, ID_WXBPMCTRL, wxT("80"), wxPoint(145, 38), wxSize(59, 22), wxSP_ARROW_KEYS, 0, 100, 80);

    WxVolume = new wxSlider(this, ID_WXVOLUME, 0, 0, 100, wxPoint(21, 107), wxSize(198, 27), wxSL_HORIZONTAL | wxSL_SELRANGE , wxDefaultValidator, wxT("WxVolume"));
    WxVolume->SetRange(0,100);
    WxVolume->SetValue(0);

    WxBeatsInAMeasure = new wxSpinCtrl(this, ID_WXBEATSINAMEASURE, wxT("0"), wxPoint(12, 38), wxSize(46, 22), wxSP_ARROW_KEYS, 0, 100, 0);

    WxCurrentBeat = new wxStaticText(this, ID_WXCURRENTBEAT, wxT("0"), wxPoint(396, 10), wxDefaultSize, 0, wxT("WxCurrentBeat"));

    WxHostWindow = new wxScrolledWindow(this, ID_WXHOSTWINDOW, wxPoint(285, 61), wxSize(300, 300), wxCLIP_CHILDREN);
    WxHostWindow->SetBackgroundColour(wxColour(000,000,000));

    WxStaticText1 = new wxStaticText(this, ID_WXSTATICTEXT1, wxT("Volume:"), wxPoint(21, 80), wxDefaultSize, 0, wxT("WxStaticText1"));

    WxStaticText2 = new wxStaticText(this, ID_WXSTATICTEXT2, wxT("Tempo:"), wxPoint(145, 8), wxDefaultSize, 0, wxT("WxStaticText2"));

    WxStaticText3 = new wxStaticText(this, ID_WXSTATICTEXT3, wxT("Beats in a measure:"), wxPoint(12, 8), wxDefaultSize, 0, wxT("WxStaticText3"));

    wxArrayString arrayStringFor_WxBeatSubdivision;
    WxBeatSubdivision = new wxBitmapComboBox(this, ID_WXBEATSUBDIVISION, wxT("WxBeatSubdivision"), wxPoint(17, 290), wxSize(142, 21), arrayStringFor_WxBeatSubdivision, wxCB_READONLY, wxDefaultValidator, wxT("WxBeatSubdivision"));

    WxShowBeatSoundMap = new wxButton(this, ID_WXSHOWBEATSOUNDMAP, wxT("Choose sounds"), wxPoint(166, 184), wxSize(103, 42), 0, wxDefaultValidator, wxT("WxShowBeatSoundMap"));

    WxStaticText4 = new wxStaticText(this, ID_WXSTATICTEXT4, wxT("How to divide a beat:"), wxPoint(17, 263), wxDefaultSize, 0, wxT("WxStaticText4"));

    WxTapTempo = new wxButton(this, ID_WXTAPTEMPO, wxT("Tap tempo"), wxPoint(139, 68), wxSize(87, 20), 0, wxDefaultValidator, wxT("WxTapTempo"));

    SetIcon(wxNullIcon);
    SetSize(8,8,624,446);
    Center();

    ////GUI Items Creation End	

    // Build data directory path
    wxString datadir;
#ifdef DATADIR
    datadir += wxT(DATADIR);
#else
    wxStandardPaths stdpaths;
    wxString std_data_path = stdpaths.GetDataDir();
    wxString guess_data_dir = std_data_path + wxString(wxT("./data"));
    bool data_dir_exists = wxDir::Exists(guess_data_dir);
    if(data_dir_exists) datadir = guess_data_dir;
    else
    {
        wxString data_loc_file_path = std_data_path + wxString(wxT("/data.loc"));
        if(wxFile::Exists(data_loc_file_path))
        {
            wxFFile data_loc_file(data_loc_file_path);
            data_loc_file.ReadAll(&datadir);
            data_loc_file.Close();
        }
        else datadir = wxString(wxT(""));
    }    
#endif

    // Set host window background color
    //WxHostWindow->SetBackgroundColour(wxColour(000,000,000,wxALPHA_TRANSPARENT));

    // Beat subdivision
    WxBeatSubdivision->SetWindowStyle(wxCB_READONLY);
    ::wxInitAllImageHandlers();

    WxBeatSubdivision->Append(wxT("one note"), wxBitmap(datadir+wxT("/subdivisions/minima.png"),wxBITMAP_TYPE_PNG)  );
    WxBeatSubdivision->Append(wxT("duplet"),   wxBitmap(datadir+wxT("/subdivisions/duplet.png"),wxBITMAP_TYPE_PNG)  );
    WxBeatSubdivision->Append(wxT("triplet"),  wxBitmap(datadir+wxT("/subdivisions/triplet.png"),wxBITMAP_TYPE_PNG) );

    WxBeatSubdivision->SetSelection(0);

    // Load config.xml
    XmlParserConfig config_parser;
    ConfigOptions opts;
    config_parser.user_data = &opts;
    config_parser.datadir = datadir;
    if (! config_parser.parseFile(datadir+wxT("/config.xml")) )
    {
        wxMessageBox(wxT("Config.xml file could not be parsed. Program ends here."));
        exit(-1);
    }

    // Load window background
    int window_width, window_height;
    int host_x, host_y, host_width, host_height;

    this->GetClientSize(&window_width, &window_height);
    WxHostWindow->GetPosition(&host_x, &host_y);
    WxHostWindow->GetClientSize(&host_width, &host_height);

    if (opts.window_back != wxString(wxT("")))
    {
        window_background_img.LoadFile(opts.window_back);
        if (window_background_img.Ok())
        {
            // Adapt window size to background image
            int image_x = window_background_img.GetWidth();
            int image_y = window_background_img.GetHeight();
            // win_x : win y = img_x : img_y
            // win_x = (img_x * win_y) / img_y
            window_width = (image_x*window_height) / image_y;
            this->SetClientSize(window_width, window_height);

            // Rescale the image
            wxImage winback_scaled = window_background_img.Scale(window_width, window_height, wxIMAGE_QUALITY_HIGH);
            window_background_bmp = new wxBitmap(winback_scaled);
        }
        else
        {
            window_background_bmp = NULL;
        }
    }
    else
        window_background_bmp = NULL;

    // Set values for GUI	
    WxBPM->SetRange(20, 400);
    WxBPM->SetValue(80);
    WxVolume->SetRange(0, 100);
    WxVolume->SetValue(100);
    WxBeatsInAMeasure->SetRange(1, 20);
    WxBeatsInAMeasure->SetValue(this->beatmgr.getMainBeats());

    wxFont myfont = wxFont(
                        30, //point size
                        wxFONTFAMILY_MODERN, //font family
                        wxFONTSTYLE_NORMAL, //style
                        wxFONTWEIGHT_BOLD, //font boldness
                        false, //underline
                        wxT("") //typeface, default if empty
                    );
    WxCurrentBeat->SetFont(myfont);
    WxCurrentBeat->SetLabel(wxT("0"));

    // Load audio
    soundmgr->ini(opts);

    // Add a silent sound for pauses
    soundmgr->addSilentSound();

    // Update the beat->sound map
    this->updateBeatSoundMap();
    // Remove element with key 0 from the map
    this->beat_sound_map.erase(0);

    // Set correct volume
    wxScrollEvent screvt;
    this->WxVolumeScroll(screvt);

    // Initialize sound timer
    this->sound_timer = Timer::create();
    this->sound_timer->setCallback(metroSoundOnTime, (void*)this);

    // Initialize tap tempo chronometer
    this->taptempo_chrono = Chronometer::create();

    // Metronome animation		
    this->metro_anim = new WindowAnimation();
    this->metro_anim->setHost(WxHostWindow);	
    this->metro_anim->Init(opts.frames_path, wxT("output"));	
    this->metro_anim->setIdleFrame(opts.idle_frame);

    if (!opts.metro_back.IsEmpty())
        this->metro_anim->setBackground(opts.metro_back);
    else if (window_background_bmp != NULL)
    {
        this->metro_anim->setBackground(*window_background_bmp);
    }	
}

/*
 * WxPlayClick
 */
void metronomeDlg::WxPlayClick(wxCommandEvent& event)
{
    this->is_playing = !this->is_playing;

    if (this->is_playing)
    {
        wxSpinEvent evt;
        this->WxBPMUpdated(evt);

        WxPlay->SetLabel(wxT("Stop"));
        this->WxBeatSubdivision->Disable();
    }
    else
    {
        this->sound_timer->stop();
        this->metro_anim->stop();
        this->beatmgr.resetCurrentBeat();

        this->WxCurrentBeat->SetLabel(wxT("0"));
        WxPlay->SetLabel(wxT("Play"));
        this->WxBeatSubdivision->Enable();
    }
}

/*
 * WxVolumeScroll
 */
void metronomeDlg::WxVolumeScroll(wxScrollEvent& event)
{
    unsigned int value_int = WxVolume->GetValue();

    //value_int : 100 = value_float : 1
    float value_float = static_cast<float>(value_int) / 100.0f;

    soundmgr->setVolume(value_float);
    return;
}

/*
 * WxBPMUpdated
 */
void metronomeDlg::WxBPMUpdated(wxSpinEvent& event )
{
    if (!this->is_playing)
    {
        event.Skip();
        return;
    }

    // Reset sound
    this->sound_timer->stop();
    this->beatmgr.resetCurrentBeat();

    unsigned int bpm = static_cast<unsigned int>(WxBPM->GetValue());
    unsigned int period_milliseconds = BeatMgr::calculateDurationOfABeat(bpm);

    // Divide per subbeats
    this->sound_timer->start( (unsigned int)(period_milliseconds / (double)(this->beatmgr.getNoteGroup())) );

    // Calling sound callback function for the first time
    this->sound_timer->callFunc();

    //--------------metronome animation------------------------------------
    this->metro_anim->stopTimer();

    double frame_numbers_2 = static_cast<double>(this->metro_anim->frames.size()) / 2.0;

    /*
    (frames_number/2) : period_milliseconds = 1 : x
    x = period_milliseconds / (frames_number/2)
    */

    double frame_period_d = static_cast<double>(period_milliseconds) / frame_numbers_2;
    // Floor arrotonda per difetto, ceil per eccesso
    unsigned int frame_period = static_cast<unsigned int>(ceil(frame_period_d));

    this->metro_anim->startTimer(frame_period);

    //----------------------end of metronome animation---------------------
}

/*
 * WxBeatsInAMeasureUpdated
 */
void metronomeDlg::WxBeatsInAMeasureUpdated(wxSpinEvent& event )
{
    // The first time this function is called, it is by the
    // CreateGUIControls() function when it call the constructor for WxBeatsInAMeasure.
    // So we need to do nothing and return the first time the function is called, to avoid assign
    // this->beats_in_measure an invalid value
    static bool first_time = true;
    if (first_time) {
        first_time = false;
        event.Skip();
        return;
    }

    unsigned int beats_in_measure = WxBeatsInAMeasure->GetValue();

    this->beatmgr.setMainBeats(beats_in_measure);
    this->updateBeatSoundMap();
}

void metronomeDlg::OnClose(wxCloseEvent& /*event*/)
{
    if (window_background_bmp != NULL)
	{
		delete window_background_bmp;
		window_background_bmp = NULL;
	}        

    delete this->metro_anim;
    delete this->sound_timer;

    this->metro_anim = NULL;
    this->sound_timer = NULL;

    // We MUST call the destroy function
    Destroy();
}

void metronomeDlg::OnEraseBackground(wxEraseEvent& evt)
{
#ifdef __DEBUG__
	evt_file << "OnEraseBackground called" << std::endl;
	evt_file.flush();
#endif
	// We must skip the event or the background is not cleaned correctly
	// if we not skip it, the new window will have the background of the windows that already exist
	evt.Skip();
}

/*
 * metronomeDlgPaint
 */
void metronomeDlg::metronomeDlgPaint(wxPaintEvent& evt)
{
#ifdef __DEBUG__
	evt_file << "OnPaint called" << std::endl;
	evt_file.flush();
#endif

	wxPaintDC dc(this);
	evt.Skip();

    // If window_background_bmp is NULL, do nothing
    if (window_background_bmp == NULL) {
        evt.Skip();
        return;
    }    

    wxPaintDC host_dc(this->WxHostWindow);

    // Draw bitmap on the main window
    dc.DrawBitmap(*window_background_bmp, 0, 0, false);

    // Creates a memory dc for the blit function
    wxMemoryDC winback_dc;
    winback_dc.SelectObjectAsSource(*window_background_bmp);

    int host_x, host_y, host_width, host_height;
    this->WxHostWindow->GetPosition(&host_x, &host_y);
    this->WxHostWindow->GetClientSize(&host_width, &host_height);

    host_dc.Blit(
        (wxCoord)0,
        (wxCoord)0,
        (wxCoord)host_width,
        (wxCoord)host_height,
        &winback_dc,
        (wxCoord)host_x,
        (wxCoord)host_y
    );
}

/*
 * metronomeDlgSize
 */
void metronomeDlg::metronomeDlgSize(wxSizeEvent& evt)
{

#ifdef __DEBUG__
	evt_file << "OnSize called" << std::endl;
	evt_file.flush();
#endif

    // If window_background_bmp is NULL, do nothing
    if (window_background_bmp == NULL) {
        evt.Skip();
        return;
    }

    wxClientDC win_dc(this);
    wxClientDC host_dc(this->WxHostWindow);

    wxBitmap* old_winbackground = window_background_bmp;

    // Rescale the image
    // These will contain the client size of the main window
    int window_width, window_height;
    // These will contain the position and the client size of the hosted window that contains the animation
    int host_x, host_y, host_width, host_height;

    this->GetClientSize(&window_width, &window_height);
    this->WxHostWindow->GetPosition(&host_x, &host_y);
    this->WxHostWindow->GetClientSize(&host_width, &host_height);

    wxImage winback_scaled = window_background_img.Scale(window_width, window_height, wxIMAGE_QUALITY_NORMAL);
    window_background_bmp = new wxBitmap(winback_scaled);

    // Draw back bitmap on main window
    win_dc.DrawBitmap(*window_background_bmp, 0, 0, false);

    // Set new background image to the host window
    if (this->metro_anim != NULL)
    {
        this->metro_anim->setBackground(*window_background_bmp);
        this->metro_anim->updateWindow();
    }

    if (old_winbackground != NULL)
        delete old_winbackground;

    winback_need_update = true;
	evt.Skip();
}

void metronomeDlg::OnIdle(wxIdleEvent& event)
{
    // Update sound
    // This MUST be done or an error will raise after some times. It has nothing to do with the update of the window
    if (soundmgr != NULL)
    {
        soundmgr->update();
    }

    if (!winback_need_update || window_background_bmp == NULL) return;

    delete window_background_bmp;

    int window_width, window_height;
    this->GetClientSize(&window_width, &window_height);

    wxImage winback_scaled = window_background_img.Scale(window_width, window_height, wxIMAGE_QUALITY_HIGH);
    window_background_bmp = new wxBitmap(winback_scaled);

    //update window
    this->Refresh();
    this->Update();
    this->metro_anim->setBackground(*window_background_bmp);
    this->metro_anim->updateWindow();

    winback_need_update = false;
}

/*
 * WxShowBeatSoundMapClick
 */
void metronomeDlg::WxShowBeatSoundMapClick(wxCommandEvent& event)
{
    // If the beat window is already open, we simply set the focus on it
    if (this->beat_map_dlg != NULL)
    {
        // Need to call also this to prepare the window to receive events?
        //beat_map->SetFocus();
        this->beat_map_dlg->Raise();
        event.Skip();
        return;
    }

    this->beat_map_dlg = new BeatSoundMapDlg(NULL);
    beat_map_dlg->init(this);
    beat_map_dlg->Show(true);
}

/*
 * WxBeatSubdivisionSelected
 */
void metronomeDlg::WxBeatSubdivisionSelected(wxCommandEvent& event )
{
    if (this->is_playing)
    {
        wxMessageBox(wxT("You need to stop the metronome for doing this"));
        return;
    }

    else
    {
        this->beatmgr.setNoteGroup( (ENOTEGROUP)(this->WxBeatSubdivision->GetSelection()+1) );
        this->updateBeatSoundMap();
    }
}

/*
 * WxTapTempoClick
 */
void metronomeDlg::WxTapTempoClick(wxCommandEvent& event)
{
    //wxMessageBox(wxT("Taptempo clicked!"));

    // Get elapsed time in seconds
    double elapsed = this->taptempo_chrono->reset();

    double bpm_double = (1.0/elapsed) * 60.0;
    unsigned int bpm = static_cast<unsigned int>(ceil(bpm_double));

    this->WxBPM->SetValue(bpm);
}

void metronomeDlg::OnMainBeat( wxCommandEvent &event )
{
    wxString str = event.GetString();
    this->WxCurrentBeat->SetLabel(str);
}
