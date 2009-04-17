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
 * @file  clipboard.cpp
 *
 * @brief Clipboard helper functions implementation.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "clipboard.h"
#include "resource.h"
#include "LangString.h"

/**
 * @brief Copy text to clipboard.
 * @param [in] hwnd Parent window handle.
 * @param [in] text Text to copy.
 * @param [in] len Lenght of the text in characters.
 */
void TextToClipboard(HWND hwnd, TCHAR *text, int len)
{
	const int size = (len + 1) * sizeof(TCHAR);
	if (HGLOBAL hGlobal = GlobalAlloc(GHND, size))
	{
		SetCursor(LoadCursor (NULL, IDC_WAIT));
		if (TCHAR *pd = (TCHAR *)GlobalLock(hGlobal))
		{
			// Succesfully got pointer
			_tcsncpy(pd, text, len);
			GlobalUnlock(hGlobal);
			if (OpenClipboard(hwnd))
			{
				// Open clip
				EmptyClipboard();
				SetClipboardData(CF_TEXT, hGlobal);
				CloseClipboard();
			}
			else // Failed to open clip
			{
				LangString app(IDS_APPNAME);
				LangString noAccess(IDS_CANNOT_ACCESS_CLIPBOARD);
				MessageBox(hwnd, noAccess, app, MB_ICONERROR);
			}
		}
		else
		{
			// Failed to get pointer to global mem
			GlobalFree(hGlobal);
			LangString app(IDS_APPNAME);
			LangString cannotLock(IDS_CANNOT_LOCK_CLIPBOARD);
			MessageBox(hwnd, cannotLock, app, MB_ICONERROR);
		}
		SetCursor (LoadCursor (NULL, IDC_ARROW));
	}
	else
	{
		// Failed to allocate global mem
		LangString app(IDS_APPNAME);
		LangString noMem(IDS_CLIPBOARD_NO_MEM);
		MessageBox(hwnd, noMem, app, MB_ICONERROR);
	}
}

/**
 * @brief Copy text to clipboard.
 * @param [in] hwnd Parent window handle.
 * @param [in] text Text to copy.
 */
void TextToClipboard(HWND hwnd, TCHAR *text)
{
	int len = _tcslen(text);
	TextToClipboard(hwnd, text, len);
}

void MessageCopyBox(HWND hwnd, LPTSTR text, LPCTSTR caption, UINT type)
{
	int len = _tcslen(text);
	LangString copyTo(IDS_CLIPBOARD_COPY_TO);
	_tcscat(text, copyTo);
	if (IDYES == MessageBox(hwnd, text, caption, MB_YESNO | type))
	{
		// User wants to copy output
		text[len] = '\0'; // Remove the line added above
		TextToClipboard(hwnd, text, len);
	}
}
