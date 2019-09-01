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

#include "XmlParserConfig.h"
#include "SoundManager.h"
#include "SoundManagerFMOD.h"

#include "libopenmusic/utils.h"
#include "libopenmusic/wxutils.h"
using namespace std;

void ConfigOptions::print()
{
    wxString str;
    str << wxT("idle_frame: ") << idle_frame << wxT("\n");
    str << wxT("background_bitmap: ") << metro_back << wxT("\n");
    /*
    str << wxT("sound1: ") << sound1 << wxT("\n");
    str << wxT("sound2: ") << sound2 << wxT("\n");
    */
    str << wxT("frames_path: ") << frames_path << wxT("\n");
    str << wxT("frames_type: ") << frames_type << wxT("\n");
    wxMessageBox(str);
}

ConfigOptions::ConfigOptions()
{
    this->idle_frame = 0;
    this->metro_back = wxString(wxT(""));
    this->window_back = wxString(wxT(""));
    this->fmod_output_type = FMOD_OUTPUTTYPE_AUTODETECT;
}

XmlParserConfig::XmlParserConfig()
{
    this->user_data = NULL;
}

bool XmlParserConfig::onTag(wxXmlNode* node, vector<wxString>& path, void* user_data)
{
    wxString name = node->GetName();
    wxXmlNodeType node_type = node->GetType();
    if(node_type == wxXML_COMMENT_NODE) return true;

    wxString path_solved = XmlParser::solvePath(path, 1);
    ConfigOptions* opts = (ConfigOptions*) user_data;

    //window->background
    if(name == wxT("background") && path_solved == wxT("window"))
    {
        opts->window_back = this->datadir + node->GetNodeContent();
        return true;
    }

    //animation->idle_frame
    else if(name == wxT("idle_frame") && path_solved == wxT("animation"))
    {
        opts->idle_frame = atoi((const char*)node->GetNodeContent().c_str());
        return true;
    }

    //animation->background sub-tag
    else if(name == wxT("background") && path_solved == wxT("animation"))
    {
        opts->metro_back = this->datadir + node->GetNodeContent();
        return true;
    }

    //animation->path sub-tag
    else if(name == wxT("path") && path_solved == wxT("animation"))
    {
        opts->frames_path = this->datadir + node->GetNodeContent();
        return true;
    }

    //sound->output_type
    else if(name == wxT("output_type") &&  path_solved == wxT("sound") )
    {
        std::string str(toString(node->GetNodeContent()));
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        opts->fmod_output_type = strToFmodOutType(str);
        return true;
    }

    //sound->data->[number]
    else if(name == wxT("file") &&  path_solved == wxT("sound->data") )
    {
        wxString wildcard_str = node->GetPropVal(wxT("wildcard"), wxT("false"));
        bool wildcard;
        bool res = strToBool(toString(wildcard_str), wildcard);
        if(!res)
        {
            wxMessageBox(wxT("Error! Invalid \"wildcard\" value in sound->data->file tag. ")
                         wxT("Possibile values are \"true\" or \"false\" (case unsensitive)")
						 );
        }

        wxString sound_file = node->GetNodeContent();
        trim(sound_file);
        sound_file = this->datadir + sound_file;

        if(wildcard)
        {
            wxString curr_file = wxFindFirstFile(sound_file);
            while ( !curr_file.empty() )
            {
                // Remove spaces from the left and the right of the string, otherwise FMOD will return
                // a file not found error, if there are any spaces
                trim(curr_file);
                opts->sound_files.push_back(curr_file);

                curr_file = wxFindNextFile();
            }
        }
        else
        {
            opts->sound_files.push_back(sound_file);
        }

        return true;
    }

    //not a known tag
    return false;
}
