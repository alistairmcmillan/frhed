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
 * @file  LangString.h
 *
 * @brief  Class for converting translated strings from wide char to ANSI.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _LANG_STRING_H_
#define _LANG_STRING_H_

LPWSTR GetLangString(WORD id);

/**
 * @brief Helper class for using translated strings.
 * This class helps using translated strings by:
 * - hiding ANSI and UNICODE build differences (does conversions when needed)
 * - getting string by resource ID
 */
class LangString
{
private:
#ifdef UNICODE
	PCWSTR m_str; /**< Resource string pointer in UNICODE build. */
#else
	BSTR m_str; /**< Resource string pointer in ANSI build. */
#endif

protected:
	void Convert(PCWSTR text);

public:
	LangString(PCWSTR text);
	LangString(WORD id);
	~LangString();

#ifdef UNICODE
	operator PCWSTR() { return m_str; }
#else
	operator PCSTR() { return (PCSTR)m_str; }
#endif
};

#endif // _LANG_STRING_H_
