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

//#define __DEBUG__

#include "WindowAnimation.h"
#include <iostream>
#include <fstream>
#include "libopenmusic/wxutils.h"

#ifdef __DEBUG__
    #include <fstream>
    using namespace std;
    std::ofstream debug_file("debug.txt");
    std::ofstream loaded_dbgfile("loaded.txt");
    std::ofstream evt_file("winanim_evt.txt");
	std::ofstream winanimevt_file("bkg_evt.txt");
#endif

// Event table
BEGIN_EVENT_TABLE(WinAnimEvtHandler, wxEvtHandler)
    EVT_ERASE_BACKGROUND(WinAnimEvtHandler::onEraseBackground)
    EVT_PAINT(WinAnimEvtHandler::onPaint)
END_EVENT_TABLE()

void WinAnimEvtHandler::onEraseBackground(wxEraseEvent& evt)
{
#ifdef __DEBUG__
	winanimevt_file << "OnEraseBackground Fire!!!" << std::endl;
	winanimevt_file.flush();
#endif

	// We must skip the event or the background is not cleaned correctly
	// if we not skip it, the new window will have the background of the windows that already exist
	evt.Skip();
}

void WinAnimEvtHandler::onPaint(wxPaintEvent& evt)
{
#ifdef __DEBUG__
	winanimevt_file << "OnPaint Fire!!!" << std::endl;
	winanimevt_file.flush();
#endif

	wxPaintDC dc(this->host);
	//dc.DrawCircle(0,0,10);

	// We must skip the event or strange grapichs behaviour will happen
    evt.Skip();
}

// Files must be in the format prefix_number.extention
static int SortFunc(const wxString& first, const wxString& second)
{
    int pos1_1 = first.Find('_');
    int pos1_2 = first.Find('.');

    int pos2_1 = second.Find('_');
    int pos2_2 = second.Find('.');

    if(pos1_1 == -1 ||
    pos1_2 == -1 ||
    pos2_1 == -1 ||
    pos2_2 == -1 )
        return 1;

    wxString sub1 = first.Mid(pos1_1+1, pos1_2 - pos1_1 - 1);
    wxString sub2 = second.Mid(pos2_1+1, pos2_2 - pos2_1 - 1);

    int sub1_int = wxAtoi(sub1);
    int sub2_int = wxAtoi(sub2);

    /*
    wxString message = wxString("Substring 1: ") + sub1 + wxString("\nSub2:") + sub2;
    wxMessageBox(message);
    */

    if(sub1_int < sub2_int) return -1;
    if(sub1_int > sub2_int) return 1;
    return 0;

    return 1;
}

WinAnimEvtHandler::WinAnimEvtHandler()
{
	this->host = NULL;
}

WinAnimEvtHandler::~WinAnimEvtHandler()
{
}

//------------------------------------------------------------------------------------------

WindowAnimation::WindowAnimation()
{
	this->evt_handler = NULL;
}

WindowAnimation::WindowAnimation(wxString directory_name, wxString file_prefix)
{
    this->Init(directory_name, file_prefix);
}

bool WindowAnimation::Init(wxString directory_name, wxString file_prefix)
{
    wxArrayString files_arr;
    wxDir::GetAllFiles(directory_name, &files_arr);
    files_arr.Sort(SortFunc);

    for(unsigned int i=0; i<files_arr.GetCount(); i++)
    {
        if(files_arr[i].Find(file_prefix) != wxNOT_FOUND)
        {
            //wxString file_str = directory_name + wxString("/") + files_arr[i];
            wxString file_str = files_arr[i];

            wxBitmap* bitmap = NULL;
            wxImage* img = new wxImage();
            bool res = img->LoadFile(file_str);

            if(res == false)
            {
                #ifdef __DEBUG__
                    loaded_dbgfile << "File not a valid bitmap: \"";
                    loaded_dbgfile << file_str;
                    loaded_dbgfile << "\". Last system message: ";
                    loaded_dbgfile << wxSysErrorMsg();
                    loaded_dbgfile << endl;
                #endif

                delete img;
                continue;
            }
            else
            {
                // Delete alpha channel
				//changeColor(img, 0, 0, 0, 0, 255, 255, 255, 255);
                bitmap = new wxBitmap(*img);

                #ifdef __DEBUG__
                    loaded_dbgfile << "\"" << file_str << "\" loaded"<< endl;
                #endif
            }

            this->frames.push_back(bitmap);
        }
        else
        {
            #ifdef __DEBUG__
                loaded_dbgfile << "File does not contain prefix output_: \"";
                loaded_dbgfile << files_arr[i];
                loaded_dbgfile << "\"";
                loaded_dbgfile << endl;
            #endif
        }
    }

    // Initialize variables
    this->curr_frame = 0;
    this->idle_frame = 0;
    this->need_syncronization = false;
    this->total_lateness = 0;
    this->last_syncro_frame = 1;
    this->evt_handler = new WinAnimEvtHandler();	

    // Choose the best implementation for chronometer and timer
    this->chronometer = Chronometer::create();

    //this->timer = new Timer::TrWxWidgets();
    this->timer = Timer::create();

    this->timer->setCallback(this->timerCallback, (void*)(this));

    #ifdef __DEBUG__
        loaded_dbgfile.close();
    #endif

	// Connect the on erase background event to avoid flickering
	this->evt_handler->host = this->host;
	this->host->PushEventHandler(this->evt_handler);

	// DO NOT apply the idle frame because we DO NOT HAVE A WINDOW ATTACHED YET!!!
	//this->idle();

    return true;
}

bool WindowAnimation::startTimer(unsigned int milliseconds)
{
    // Avoid bugs
    this->timer->start(milliseconds);
    this->normal_delay = milliseconds;
    this->chronometer->start();

    return true;
}

bool WindowAnimation::stopTimer()
{
    this->timer->stop();
    // Reset syncrinization
    this->last_syncro_frame = 1;
    return true;
}

bool WindowAnimation::stop()
{
    this->stopTimer();
    this->idle();
    return true;
}

void WindowAnimation::timerCallback(void* usrdata)
{
    #ifdef __DEBUG__
        wxString debugstr;
    #endif

    WindowAnimation* mainclass = (WindowAnimation*) usrdata;

    double elapsed_time = mainclass->chronometer->reset();
    //reset() returns seconds, not milliseconds!!!
    elapsed_time *= static_cast<double>(1000.0);

    //wxString str("Elapsed time: ");
    //str << elapsed_time;
    //wxMessageBox(str);

    if(elapsed_time > static_cast<double>(mainclass->normal_delay))
    {
        #ifdef __DEBUG__
            debugstr << wxT("\n\nWe are late. Normal delay was: ");
            debugstr << mainclass->normal_delay;
            debugstr << wxT("\nCurrent one is: ");
            debugstr << elapsed_time;
            debugstr << wxT("\nLateness is: ");
            debugstr << elapsed_time - static_cast<double>(mainclass->normal_delay);
            debugstr << wxT("\nTotal lateness before is: ");
            debugstr << mainclass->total_lateness;
        #endif

        //add to lateness
        mainclass->total_lateness += (elapsed_time - (double)(mainclass->normal_delay));

        #ifdef __DEBUG__
            debugstr << wxT("\nTotal lateness after is: ");
            debugstr << mainclass->total_lateness;
        #endif
    }

    else if(elapsed_time < static_cast<double>(mainclass->normal_delay))
    {
        #ifdef __DEBUG__
            debugstr << wxT("\n\nWe are early. Normal delay was: ");
            debugstr << mainclass->normal_delay;
            debugstr << wxT("\nCurrent one is: ");
            debugstr << elapsed_time;
            debugstr << wxT("\nLateness is: ");
            debugstr << elapsed_time - static_cast<double>(mainclass->normal_delay);
            debugstr << wxT("\nTotal lateness before is: ");
            debugstr << mainclass->total_lateness;
        #endif

        //add to lateness
        mainclass->total_lateness += elapsed_time;

        //return because it's really too early to change frame
        if(mainclass->total_lateness < (double)(mainclass->normal_delay))
        {
            #ifdef __DEBUG__
                debugstr << wxT("\nFRAMES SKIPPED");
                debug_file << debugstr;
                debug_file.flush();
            #endif

            return;
        }
        else
        {
            #ifdef __DEBUG__
                debugstr << wxT("\nTotal lateness before substracting normal_delay: ");
                debugstr << mainclass->total_lateness;
            #endif

            mainclass->total_lateness -= (double)(mainclass->normal_delay);

             #ifdef __DEBUG__
                debugstr << wxT("\nTotal lateness after substracting normal_delay ");
                debugstr << mainclass->total_lateness;
            #endif
        }
    }

    // After getting the correct strings and voided too early changes, increment curr_frame
    mainclass->curr_frame++;

    int frames_to_recover = static_cast<int>( floor(mainclass->total_lateness/(double)mainclass->normal_delay) );
    mainclass->total_lateness -= frames_to_recover * mainclass->normal_delay;

    mainclass->curr_frame += frames_to_recover;
    if(mainclass->curr_frame >= mainclass->frames.size()) mainclass->curr_frame = (mainclass->curr_frame % mainclass->frames.size());

    // SYNCRONIZATION
    if(mainclass->need_syncronization)
    {
		if(mainclass->last_syncro_frame == 0)
		{
			mainclass->curr_frame = (mainclass->frames.size() / 2);
		}
		else
		{
			mainclass->curr_frame = 0;
		}

		mainclass->last_syncro_frame = mainclass->curr_frame;
        mainclass->need_syncronization = false;
    }
    // END OF SYNCRONIZATION

    mainclass->updateWindow();

    #ifdef __DEBUG__
        debugstr << wxT("\nFrames recovered: ");
        debugstr << frames_to_recover;
        debugstr << wxT("\nTotal lateness after recover is: ");
        debugstr << mainclass->total_lateness;
        debug_file << debugstr;
        debug_file.flush();
    #endif

    mainclass->chronometer->start();

    return;
}

WindowAnimation::~WindowAnimation()
{
    for(std::vector<wxBitmap*>::iterator it = this->frames.begin(); it != this->frames.end(); it++)
    {
        delete (*it);
    }

    delete this->chronometer;
    delete this->timer;

    // Pop the event handler before deleting it. Otherwise, the program will crash
    this->host->PopEventHandler();
    delete this->evt_handler;
}

void WindowAnimation::syncronize(unsigned int beat)
{
    this->need_syncronization=true;
}

void WindowAnimation::setIdleFrame(unsigned int frame)
{
    this->idle_frame = frame;
}

void WindowAnimation::setHost(wxWindow* _host)
{
	this->host = _host;

	if(this->evt_handler != NULL)
	{
		this->evt_handler->host = _host;
	}
}

void WindowAnimation::idle()
{
    this->applyFrame(this->idle_frame);
}

bool WindowAnimation::setBackground(wxString file)
{
    bool res = this->background.LoadFile(file, wxBITMAP_TYPE_PNG);
    return res;
}

void WindowAnimation::setBackground(wxBitmap& bmp)
{
    this->background = bmp;
}

void WindowAnimation::applyFrame(unsigned int frame)
{
	if(frame >= this->frames.size() && this->frames[frame] == NULL) return;

    static wxBitmap buffer_bitmap(300,300);

    wxClientDC client_dc(this->host);
    wxBufferedDC buff_dc(&client_dc, buffer_bitmap);
    //wxBufferedDC buff_dc(&client_dc);
    //buff_dc.Clear();

    /*
    if(this->background.Ok())
    {
       wxMemoryDC source_dc;
       source_dc.SelectObjectAsSource(this->background);

	   int host_x, host_y, host_width, host_height;
	   this->host->GetPosition(&host_x, &host_y);
	   this->host->GetClientSize(&host_width, &host_height);

       buff_dc.Blit(
	       (wxCoord)0,
	       (wxCoord)0,
	       (wxCoord)host_width,
	       (wxCoord)host_height,
	       &source_dc,
	       (wxCoord)host_x,
	       (wxCoord)host_y
        );
    }
    */

    //void DrawBitmap(const wxBitmap&  bitmap, wxCoord x, wxCoord y, bool  transparent)
    buff_dc.DrawBitmap(*this->frames[frame], 0, 0 , false);
	
    // go slower with this
    //this->host->Update();
}
