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
 * @file  GotoDlg.cpp
 *
 * @brief Implementation of the Go To-dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "LangString.h"

/** Size of offset edit and buffer. */
static const UINT EditLen = 16;

/**
 * Initialize the dialog.
 * This function initializes the dialog by setting the initial value for the
 * offset edit box and setting text lenhgt limit for it.
 * @param [in] hDlg Handle to dialog to initialize.
 * @return TRUE.
 */
BOOL GoToDlg::OnInitDialog(HWND hDlg)
{
	// Get current offset
	TCHAR buffer[EditLen + 1];
	_itot(iCurByte, buffer, 10);

	HWND edit = GetDlgItem(hDlg, IDC_GOTO_OFFSET);
	SetDlgItemText(hDlg, IDC_GOTO_OFFSET, buffer);

	// Limit edit field char amount
	SendMessage(edit, EM_SETLIMITTEXT, EditLen, 0);
	return TRUE;
}

/**
 * @brief Go to offset user gave to the dialog.
 * @param [in] hDlg Handle of Goto-dialog.
 * @return TRUE if new offset was applied, FALSE for invalid offset.
 */
BOOL GoToDlg::Apply(HWND hDlg)
{
	TCHAR buffer[EditLen + 1];
	int offset, i = 0, r = 0;
	GetDlgItemText(hDlg, IDC_GOTO_OFFSET, buffer, RTL_NUMBER_OF(buffer));
	// For a relative jump, read offset from 2nd character on.
	if (buffer[0] == '+' || buffer[0] == '-')
		r = 1;
	if (_stscanf(buffer + r, _T("x%x"), &offset) == 0 &&
		_stscanf(buffer + r, _T("%d"), &offset) == 0)
	{
		LangString offsetErr(IDS_OFFSET_ERROR);
		MessageBox(hDlg, offsetErr, MB_ICONERROR);
		return FALSE;
	}
	if (r)
	{
		// Relative jump.
		if (buffer[0] == '-' )
			offset = -offset;
		offset += iCurByte;
	}
	
	// Absolute jump.
	// Check limits and jump to begin/end if out of limits
	if (offset < 0)
		offset = 0;
	if (offset >= DataArray.GetLength())
		offset = DataArray.GetLength() - 1;

	iCurByte = offset;
	snap_caret();
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
INT_PTR GoToDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hDlg);
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
