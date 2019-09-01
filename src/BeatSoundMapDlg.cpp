///-----------------------------------------------------------------
///
/// @file      BeatSoundMapDlg.cpp
/// @author    Raffaele
/// Created:   07/04/2010 7.08.47
/// @section   DESCRIPTION
///            BeatSoundMapDlg class implementation
///
///------------------------------------------------------------------

#include "BeatSoundMapDlg.h"

//Do not add custom headers
//wxDev-C++ designer will remove them
////Header Include Start
////Header Include End

#include <wx/filename.h>
#include "libopenmusic/wxutils.h"

//----------------------------------------------------------------------------
// BeatSoundMapDlg
//----------------------------------------------------------------------------
//Add Custom Events only in the appropriate block.
//Code added in other places will be removed by wxDev-C++
////Event Table Start
BEGIN_EVENT_TABLE(BeatSoundMapDlg,wxDialog)
	////Manual Code Start
	////Manual Code End

	EVT_CLOSE(BeatSoundMapDlg::OnClose)
	EVT_BUTTON(ID_WXUPDATE,BeatSoundMapDlg::WxUpdateClick)
	EVT_LISTBOX(ID_WXBEATSOUNDMAP,BeatSoundMapDlg::WxBeatSoundMapSelected)
END_EVENT_TABLE()
////Event Table End

BeatSoundMapDlg::BeatSoundMapDlg(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxDialog(parent, id, title, position, size, style)
{
	CreateGUIControls();
}

BeatSoundMapDlg::~BeatSoundMapDlg()
{
}

void BeatSoundMapDlg::CreateGUIControls()
{
	//Do not add custom code between
	//GUI Items Creation Start and GUI Items Creation End.
	//wxDev-C++ designer will remove them.
	//Add the custom code before or after the blocks
	////GUI Items Creation Start

	WxStaticText2 = new wxStaticText(this, ID_WXSTATICTEXT2, wxT("Choose one or more beats to modify it:"), wxPoint(25, 9), wxDefaultSize, 0, wxT("WxStaticText2"));

	WxStaticText1 = new wxStaticText(this, ID_WXSTATICTEXT1, wxT("Choose a sound:"), wxPoint(25, 189), wxDefaultSize, 0, wxT("WxStaticText1"));

	WxUpdate = new wxButton(this, ID_WXUPDATE, wxT("Apply"), wxPoint(92, 259), wxSize(119, 30), 0, wxDefaultValidator, wxT("WxUpdate"));

	wxArrayString arrayStringFor_WxSounds;
	WxSounds = new wxChoice(this, ID_WXSOUNDS, wxPoint(25, 215), wxSize(169, 21), arrayStringFor_WxSounds, 0, wxDefaultValidator, wxT("WxSounds"));
	WxSounds->SetSelection(-1);

	wxArrayString arrayStringFor_WxBeatSoundMap;
	WxBeatSoundMap = new wxListBox(this, ID_WXBEATSOUNDMAP, wxPoint(25, 37), wxSize(249, 128), arrayStringFor_WxBeatSoundMap, wxLB_EXTENDED);

	SetTitle(wxT("Beat -> Sound map"));
	SetIcon(wxNullIcon);
	SetSize(8,8,311,351);
	Center();

	////GUI Items Creation End
}

void BeatSoundMapDlg::OnClose(wxCloseEvent& /*event*/)
{
    this->metro_dlg->beat_map_dlg = NULL;
	Destroy();
}

void BeatSoundMapDlg::updateList()
{
    //update the list
    WxBeatSoundMap->Clear();

	for(std::map<unsigned int,unsigned int>::iterator it = this->metro_dlg->beat_sound_map.begin();
		it != this->metro_dlg->beat_sound_map.end();
		it++)
	{
		unsigned int beat = (*it).first;
		wxString to_add;
		wxFileName sound_file_name( this->metro_dlg->soundmgr->sounds[(*it).second].file );

		to_add << wxT(" Beat ") << toWxString(this->metro_dlg->beatmgr.getString(beat)) << wxT("   =>   ") << sound_file_name.GetFullName();
		WxBeatSoundMap->Append(to_add);
	}
}

void BeatSoundMapDlg::init(metronomeDlg* _metro_dlg)
{
	this->metro_dlg = _metro_dlg;

	// Update the combo box with the sound file names
	for(std::vector<Sound>::iterator sndit = this->metro_dlg->soundmgr->sounds.begin();
	   sndit != this->metro_dlg->soundmgr->sounds.end();
	   sndit++)
	{
	    wxFileName file_name( (*sndit).file );
        WxSounds->Append(file_name.GetFullName());
    }


    /*
    for(int i=0; i<this->metro_dlg->soundmgr->sounds.size(); i++)
    {
        wxFileName file_name( this->metro_dlg->soundmgr->sounds[i].file );
        WxSounds->Append(file_name.GetFullName());
    }
    */

    this->updateList();
}

/*
 * WxUpdateClick
 */
void BeatSoundMapDlg::WxUpdateClick(wxCommandEvent& event)
{
    wxArrayInt selection_arr;
    int count = WxBeatSoundMap->GetSelections(selection_arr);
    if(count == wxNOT_FOUND) return;

    /*
    printWxArray(selection_arr);
    printMapWx(this->metro_dlg->beat_sound_map);
    */

    for(int i=0; i<count; i++)
    {
        // The real beat is one unit after this
        int beat_selected = (selection_arr[i]+1);
    	this->metro_dlg->beat_sound_map[beat_selected] = WxSounds->GetSelection();
    }

	this->updateList();

	// Reselect the items that are now unselected after updating the list box
	for(int i=0; i<count; i++)
    {
    	WxBeatSoundMap->SetSelection( selection_arr[i] );
    }
}

/*
 * WxBeatSoundMapSelected
 */
void BeatSoundMapDlg::WxBeatSoundMapSelected(wxCommandEvent& event)
{
	wxArrayInt select_array;
	int count = WxBeatSoundMap->GetSelections(select_array);
	if(count == wxNOT_FOUND) return;

	// WORKAROUND FOR A BUG (?) IN WXWIDGETS THAT HAPPEN UNDER GTK ON UBUNTU 10.04
	if(select_array.size() == 0) return;

	// The real beat is one unit after the element ID in the list box
	int snd_index = this->metro_dlg->beat_sound_map[ (select_array[0]+1) ];

	WxSounds->SetSelection(snd_index);
}


