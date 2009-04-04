/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  LangString.cpp
 *
 * @brief Implementation of the LangString class.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "StringTable.h"
#include "LangString.h"

/**
 * @brief Get a string from resource.
 * This function loads a string from resource string table by resource ID.
 * @param [in] id Resource ID of the string to get.
 * @return Pointer to string, or NULL if not found.
 */
LPWSTR GetLangString(WORD id)
{
	for (int i = 0 ; i < RTL_NUMBER_OF(IDS) ; ++i)
	{
		if (IDS[i] == id)
		{
			return S[i];
		}
	}
	return NULL;
};

/**
 * @brief Construct the string from given string.
 * In ANSI build the wide string needs to be converted to ANSI string.
 * In UNICODE build we just store the pointer (no new copy).
 * @param [in] text Text to use.
 */
LangString::LangString(PCWSTR text)
: m_str(0)
{
#ifdef UNICODE
	m_str = text;
#else
	Convert(text);
#endif
}

/**
 * @brief Construct the string from given resoure ID.
 * In ANSI build the wide string needs to be converted to ANSI string.
 * In UNICODE build we just store the pointer (no new copy).
 * @param [in] text Text to use.
 */
LangString::LangString(WORD id)
: m_str(0)
{
	LPWSTR str = GetLangString(id);
#ifdef UNICODE
	m_str = str;
#else
	Convert(str);
#endif
}

/**
 * @brief Release resources (if any reserved).
 */
LangString::~LangString()
{
#ifndef UNICODE
	SysFreeString(m_str);
#endif
}

/**
 * @brief Convert wide string to ANSI string.
 * @param [in] text Wide string to convert.
 */
void LangString::Convert(PCWSTR text)
{
	int len = WideCharToMultiByte(CP_ACP, 0, text, -1, 0, 0, 0, 0);
	if (len)
	{
		m_str = SysAllocStringByteLen(0, len - 1);
		WideCharToMultiByte(CP_ACP, 0, text, -1, (PSTR)m_str, len, 0, 0);
	}
}
