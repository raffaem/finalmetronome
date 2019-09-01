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

#include "XmlParser.h"
using namespace std;

XmlParser::XmlParser()
{
    this->user_data = NULL;
}

wxString XmlParser::solvePath(std::vector<wxString>& path, unsigned char start)
{
	wxString final;

	// Without this line it would crash
	// because we cannot add 'start' to the begin iterator of a null vector
	if(path.size() == 0) return final;    
        
    for(vector<wxString>::iterator it = path.begin()+start; it!=path.end(); it++)
    {
        //jump if string is null
        /*
        if(*it == wxT(""))
        {
            wxString extract = final.Mid(final.Len()-3);
            //wxMessageBox(extract);            
            if(extract == wxString(wxT("->")))
            {
                final = final.Mid(0, final.Len()-2);
                //wxMessageBox(final);
            }            
            break;
        }
        */
        
        final += *it;
        
        //do not put -> in the final tag
        if( it != (path.end()-1) )
            final += wxT("->");
    }    
    return final;
}

void XmlParser::removeFirstFromPath(wxString& path)
{
    const wxString tofind(wxT("->"));
    int pos = path.Find(tofind);
    // wxString::Mid returns a string starting at the parameter passed
    path = path.Mid(pos + tofind.Len());    
    return;
}

void XmlParser::subParser(
    wxXmlNode* node, 
    vector<wxString>& path
    )
{    
    if(this->onTag(node, path, this->user_data)) return;    
    
    //node is really a parent
    else
    {
        wxXmlNode* new_child = node->GetChildren();
        
        while(new_child)
        {
            // In the new path append the parent node
            path.push_back(node->GetName());
            
            this->subParser(
                new_child, //node is the new child
                path //new path from parent       
            );    
            
            path.erase(path.end()-1);        
            new_child = new_child->GetNext();
        }              
    }     
}

bool XmlParser::onTag(wxXmlNode* node, vector<wxString>& path, void* user_data)
{
    return false;
}

bool XmlParser::parseFile(wxString filename)
{
    wxXmlDocument doc;
    bool res = doc.Load(filename);
        
    if(!res)
        return false;
       
    //pointer to the first child
    wxXmlNode* root = doc.GetRoot();    
       
    vector<wxString> path;
    
    this->subParser(
        root,
        path           
    );
    
    return true;
}
