#ifndef WXUTILS_H_1523
#define WXUTILS_H_1523
#pragma once

#include <wx/wx.h>
#include <string>
#include <map>

inline std::string toString(const wxString& str)
{

#ifdef UNICODE
	const wxChar* str_wch = str.c_str(); 
	size_t needed = (*wxConvCurrent).FromWChar(NULL, 0, str_wch);
	char* str_ch = new char[needed];
	(*wxConvCurrent).FromWChar(str_ch, needed, str_wch);	
	std::string stdstr( str_ch );
	delete str_ch;
	return stdstr;

#else //ASCII
	return std::string( str.c_str() );
#endif
}

//NEED TO DELETE THE RETURNED POINTER!!
inline const char* toCharArr(const wxString& str)
{

#ifdef UNICODE
	const wxChar* str_wch = str.c_str(); 
	size_t needed = (*wxConvCurrent).FromWChar(NULL, 0, str_wch);
	char* str_ch = new char[needed];
	(*wxConvCurrent).FromWChar(str_ch, needed, str_wch);	
	return str_ch;

#else //ASCII
	size_t needed = str.size();
	char* str_ch = new char[needed];
	//char * strcpy ( char * destination, const char * source );
	strcpy(str_ch, str.c_str());
	return str_ch;
#endif
}

inline wxString toWxString(const std::string& str)
{
#ifdef UNICODE
	return wxString( str.c_str(), *wxConvCurrent );
#else //ASCII
	return wxString( str.c_str() );
#endif
}

inline void trim(wxString& str)
{
    str.Trim(true); 
    str.Trim(false);
}

inline void wxprintArray(const wxArrayInt& arr)
{
	wxString str;
	for(unsigned int i=0; i<arr.size(); i++)
	{
		str << i << wxT(":") << arr[i] << wxT("\n");
	}
	wxMessageBox(str);
}

template <class A, class B>
inline void wxprintMap(const std::map<A,B>& _map)
{
	wxString str;

	for(typename std::map<A,B>::const_iterator it = _map.begin();
		it != _map.end();
		it++)
	{
		str << wxT("First: ") << (*it).first << wxT(" Second: ") << (*it).second << wxT("\n");
	}

	wxMessageBox(str);
}

inline void changeColor(wxImage* img, 
			int from_r, int from_g, int from_b, int from_a, 
			int to_r, int to_g, int to_b, int to_a)
{
		int width = img->GetWidth();
                int height = img->GetHeight();
                for(int x=0; x<width; x++)
                {
                    for(int y=0; y<height; y++)
                    {
                        int r = img->GetRed(x,y);
                        int g = img->GetGreen(x,y);
                        int b = img->GetBlue(x,y);
                        int a = img->GetAlpha(x,y);                        

                        if(r==from_r && g==from_g && b==from_b && a==from_a)
                        {
                            img->SetRGB(x,y, to_r, to_g, to_b);
			    img->SetAlpha(x,y, to_a);
                        }
                    }
                }
}

#endif
