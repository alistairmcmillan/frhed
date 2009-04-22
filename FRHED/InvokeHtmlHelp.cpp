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
 * @file  InvokeHtmlHelp.cpp
 *
 * @brief Implementation for helper function to start HtmlHelp.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include <htmlhelp.h>
#include <tchar.h>
#include "Constants.h"
#include "paths.h"
#include "resource.h"
#include "LangString.h"
#include "StringTable.h"

/**
 * @brief Show HTML help.
 * This function shows HTML help, with given command and path.
 * @param [in] uCommand How/what to open in help (TOC etc).
 * @param [in] path Additional relative path inside help file to open.
 * @param [in] hParentWindow Window opening this help.
 */
void ShowHtmlHelp(UINT uCommand, LPCTSTR path, HWND hParentWindow)
{
	TCHAR fullHelpPath[MAX_PATH] = {0};
	
	paths_GetFullPath(HtmlHelpFile, fullHelpPath, RTL_NUMBER_OF(fullHelpPath));
	if (!paths_DoesFileExist(fullHelpPath))
	{
		TCHAR msg[500] = {0};
		LangString app(IDS_APPNAME);
		_sntprintf(msg, RTL_NUMBER_OF(msg) - 1, GetLangString(IDS_ERR_HELP_NOT_FOUND),
				fullHelpPath);
		MessageBox(hParentWindow, msg, app, MB_ICONERROR);
		return;
	}

	if (uCommand == HELP_CONTEXT)
	{
		if (path != NULL)
		{
			TCHAR fullpath[MAX_PATH] = {0};
			_sntprintf(fullpath, MAX_PATH, _T("%s::/%s"), fullHelpPath, path);
			HtmlHelp(NULL, fullpath, HH_DISPLAY_TOPIC, NULL);
		}
		else
		{
			HtmlHelp(NULL, fullHelpPath, HH_DISPLAY_TOPIC, NULL);
		}
	}
	else if (uCommand == HELP_FINDER)
	{
		HtmlHelp(NULL, fullHelpPath, HH_DISPLAY_SEARCH, NULL);
	}
	else if (uCommand == HELP_CONTENTS)
	{
		HtmlHelp(NULL, fullHelpPath, HH_DISPLAY_TOC, NULL);
	}
}
