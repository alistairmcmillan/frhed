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
 * @file  AddBmkDlg.cpp
 *
 * @brief Implementation of the Add Bookmark dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "LangString.h"
#include "offset.h"

/** @brief Max length of the offset number. */
static const int OffsetLen = 16;

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to dialog.
 * @return TRUE
 */
BOOL AddBmkDlg::OnInitDialog(HWND hDlg)
{
	TCHAR buf[OffsetLen + 1] = {0};
	_sntprintf(buf, RTL_NUMBER_OF(buf), _T("0x%x"), iCurByte);
	SetDlgItemText(hDlg, IDC_BMKADD_OFFSET, buf);
	
	// Limit edit text lengths
	HWND edit = GetDlgItem(hDlg, IDC_BMKADD_OFFSET);
	SendMessage(edit, EM_SETLIMITTEXT, OffsetLen, 0);
	edit = GetDlgItem(hDlg, IDC_BMKADD_NAME);
	SendMessage(edit, EM_SETLIMITTEXT, BMKTEXTMAX, 0);
	return TRUE;
}

/**
 * @brief Handle dialog commands.
 * @param [in] hDlg Handle to dialog.
 * @param [in] wParam First param (depends on command).
 * @param [in] lParam Second param (depends on command).
 * @return TRUE if command (message) was handled, FALSE if not.
 */
BOOL AddBmkDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	TCHAR buf[OffsetLen + 1] = {0};
	int i, offset;
	TCHAR name[BMKTEXTMAX] = {0};
	switch (wParam)
	{
	case IDOK:
		if (GetDlgItemText(hDlg, IDC_BMKADD_OFFSET, buf, OffsetLen) &&
			!offset_parse(buf, offset))
		{
			LangString app(IDS_APPNAME);
			LangString startOffsetErr(IDS_OFFSET_START_ERROR);
			MessageBox(hDlg, startOffsetErr, app, MB_ICONERROR);
			return TRUE;
		}
		if (offset < 0 || offset > m_dataArray.GetLength())
		{
			LangString app(IDS_APPNAME);
			LangString cannotSetPos(IDS_BMK_INVALID_POSITION);
			MessageBox(hDlg, cannotSetPos, app, MB_ICONERROR);
			return TRUE;
		}
		// Is there already a bookmark on this offset?
		for (i = 0 ; i < iBmkCount ; i++)
		{
			if (pbmkList[i].offset == offset)
			{
				LangString app(IDS_APPNAME);
				LangString alreadyBmk(IDS_BMK_ALREADY_THERE);
				MessageBox(hDlg, alreadyBmk, app, MB_ICONERROR);
				return TRUE;
			}
		}
		// No bookmark on that position yet.
		pbmkList[iBmkCount].offset = offset;
		pbmkList[iBmkCount].name = GetDlgItemText(hDlg, IDC_BMKADD_NAME, name,
				BMKTEXTMAX) ? _tcsdup(name) : 0;
		iBmkCount++;
		repaint();
		// fall through
	case IDCANCEL:
		EndDialog(hDlg, wParam);
		return TRUE;
	}
	return FALSE;
}

/**
 * @brief Handle dialog messages.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] iMsg The message.
 * @param [in] wParam The command in the message.
 * @param [in] lParam The optional parameter for the command.
 * @return TRUE if the message was handled, FALSE otherwise.
 */
INT_PTR AddBmkDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hDlg);

	case WM_COMMAND:
		return OnCommand(hDlg, wParam, lParam);

	case WM_HELP:
		OnHelp(hDlg);
		break;
	}
	return FALSE;
}
