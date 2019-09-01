/*
Copyright 2010 Mancuso Raffaele

This file is part of LiveForMusic project.

LiveForMusic project is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

LiveForMusic project is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with LiveForMusic project.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef UTILS_H_5842687968
#define UTILS_H_5842687968
#pragma once

#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <typeinfo> //declare typeid
#include <cstring> //declare strcmp
#include <ctime> //declare time function
#include <stdio.h> //declare getch() function

inline void strToLower(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

inline void strToUpper(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

inline bool strToBool(std::string str, bool& result)
{
    strToLower(str);
    if(str == "true") 
    {
        result = true; 
        return true;
    }
    else if(str == "false") 
    {
        result = false; 
        return true;
    }    
    return false;
}

template<class T> inline std::string numToStr(T num)
{
	std::ostringstream s;
	const char* name = typeid(num).name();

	if( strcmp(name,"char")==0 || strcmp(name,"unsigned char")==0 )
		s << static_cast<int>(num);	
	else	
		s << num;

	return s.str();
}

template <class A, class B>
inline void printMap(const std::map<A,B>& _map)
{
	std::cout << std::endl;

	for(typename std::map<A,B>::const_iterator it = _map.begin();
		it != _map.end();
		it++)
	{
		std::cout << "First: " << (*it).first << " Second: " << (*it).second << std::endl;
	}

	std::cout << std::endl;
}

template<class A>
inline void printVec(const std::vector<A>& _vec)
{
	std::cout << std::endl;

	for(typename std::vector<A>::const_iterator it = _vec.begin();
		it != _vec.end();
		it++)
	{
		std::cout << *it << std::endl;
	}

	std::cout << std::endl;
}

inline void inirandom()
{
	srand(static_cast<unsigned int>(time(NULL)));
}

inline int random(int min, int max)
{
	int span = (max-min)+1;
	//mod is in the range [0, span]
	int mod = rand() % span;
	//mod is in the range [min, max]
	mod += min;
	return mod;
}

inline void mypause()
{
	std::cout << "Press a key... " << std::endl;
	int a;
	std::cin >> a;
}

#endif
