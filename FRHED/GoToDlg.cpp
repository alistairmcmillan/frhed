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

char GoToDlg::buffer[16];

BOOL GoToDlg::Apply(HWND hDlg)
{
	int offset, i = 0, r = 0;
	GetDlgItemText(hDlg, IDC_GOTO_OFFSET, buffer, sizeof buffer);
	// For a relative jump, read offset from 2nd character on.
	if (buffer[0] == '+' || buffer[0] == '-')
		r = 1;
	if (sscanf(buffer + r, "x%x", &offset) == 0 &&
		sscanf(buffer + r, "%d", &offset) == 0)
	{
		MessageBox(hDlg, "Offset not recognized.", "Go to", MB_ICONERROR);
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
	if (offset < 0 || offset >= DataArray.GetLength())
	{
		MessageBox(hDlg, "Invalid offset.", "Go to", MB_ICONERROR);
		return FALSE;
	}
	iCurByte = offset;
	if (snap_caret())
		repaint();
	return TRUE;
}

INT_PTR GoToDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDC_GOTO_OFFSET, buffer);
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
	}
	return FALSE;
}
