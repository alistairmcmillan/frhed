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
 * @file  StringTable.cpp
 *
 * @brief Implementation of the translation Stringtable.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "StringTable.h"
#include "resource.h"

StringTable<LPWSTR> S;

StringTable<WORD> IDS =
{
	IDS_DIFFLISTITEMFORMAT,
	IDS_ABOUTFRHEDVER,
	IDS_OPEN_ALL_FILES,
};

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
