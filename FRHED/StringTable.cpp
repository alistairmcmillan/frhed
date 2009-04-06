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
	// General strings
	IDS_APPNAME,
	IDS_DIFFLISTITEMFORMAT,
	IDS_ABOUTFRHEDVER,
	IDS_OPEN_ALL_FILES,
	IDS_UNTITLED,
	IDS_FONT_ANSI,
	IDS_FONT_OEM,
	IDS_MODE_READONLY,
	IDS_MODE_OVERWRITE,
	IDS_MODE_INSERT,
	IDS_STATUS_LITTLE_ENDIAN,
	IDS_STATUS_BIG_ENDIAN,
	IDS_OFFSET_START_ERROR,
	IDS_OFFSET_END_ERROR,
	IDS_OFFSET_ERROR,
	// About-dialog
	IDS_ABOUT_BROWSER_ERR,
	IDS_ABOUT_FILENOTFOUND,
	// Bookmarks
	IDS_BMK_INVALID_POSITION,
	IDS_BMK_ALREADY_THERE,
	IDS_BMK_EMPTY_FILE,
	IDS_BMK_MAX_AMOUNT,
	IDS_BMK_IS_INVALID,
	IDS_BMK_NONE_TOREMOVE,
	IDS_BMK_REMOVE_ALL,
};
