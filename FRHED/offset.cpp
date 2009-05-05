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
 * @file  Offset.cpp
 *
 * @brief Implementation of the offset helper functions.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "offset.h"

/**
 * @brief Parse offset number from string.
 * @param [in] str String to parse.
 * @param [out] offset Number to return (parsed offset)
 * @return true if parsing succeeded, false if failed.
 */
bool offset_parse(LPCTSTR str, int &offset)
{
	if (str == NULL || _tcslen(str) == 0)
		return false;

	int value = 0;
	if ((_stscanf(str, _T("x%x"), &value) == 0) &&
		(_stscanf(str, _T("%d"), &value) == 0))
	{
		offset = value;
		return false;
	}
	offset = value;
	return true;
}
