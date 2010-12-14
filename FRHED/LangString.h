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

#ifndef FRHED_LANG_STRING_H_1358A9E6F4A2455DB2C1F36955A44907
#define FRHED_LANG_STRING_H_1358A9E6F4A2455DB2C1F36955A44907

#include "StringTable.h"

/**
 * @brief Helper class for using translated strings.
 * This class helps using translated strings by:
 * - getting string by resource ID
 */
class LangString
{
private:
	PCTSTR m_str; /**< Resource string pointer. */

public:
	LangString(PCTSTR text);
	LangString(WORD id);

	operator PCTSTR() { return m_str; }
};

#endif
