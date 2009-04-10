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
 * @file  AppendDlg.cpp
 *
 * @brief Implementation of the Append dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "LangString.h"

/**
 * @brief Append the data to current file.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE if appending succeeded, FALSE if failed.
 */
BOOL AppendDlg::Apply(HWND hDlg)
{
	char buf[64];
	int iAppendbytes;
	if (GetDlgItemText(hDlg, IDC_APPEND_BYTES, buf, 64) &&
		sscanf(buf, "%d", &iAppendbytes) == 0)
	{
		LangString app(IDS_APPNAME);
		LangString msg(IDS_APPEND_UNKNOWN_AMOUNT);
		MessageBox(hDlg, msg, app, MB_ICONERROR);
		return FALSE;
	}
	int oldupbound = DataArray.GetLength();
	if (!DataArray.SetSize(DataArray.GetSize() + iAppendbytes))
	{
		LangString app(IDS_APPNAME);
		LangString msg(IDS_APPEND_NO_MEM);
		MessageBox(hDlg, msg, app, MB_ICONERROR);
		return FALSE;
	}
	DataArray.ExpandToSize();
	WaitCursor wc;
	for (int i = 0 ; i < iAppendbytes ; i++)
		DataArray[oldupbound + i] = 0;
	iFileChanged = TRUE;
	bFilestatusChanged = true;
	resize_window();
	return TRUE;
}

/**
 * @brief Handle dialog messages.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] iMsg The message.
 * @param [in] wParam The command in the message.
 * @param [in] lParam The optional parameter for the command.
 * @return TRUE if the message was handled, FALSE otherwise.
 */
INT_PTR AppendDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemInt(hDlg, IDC_APPEND_BYTES, 1, TRUE);
		return TRUE;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			if (Apply(hDlg))
			{
			case IDCANCEL:
				EndDialog(hDlg, wParam);
			}
			return TRUE;
		}
		break;

	case WM_HELP:
		OnHelp(hDlg);
		break;
	}
	return FALSE;
}
