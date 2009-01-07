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

/** @brief Max length of the offset number. */
static const int OffsetLen = 16;

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to dialog.
 * @return TRUE
 */
BOOL AddBmkDlg::OnInitDialog(HWND hDlg)
{
	char buf[OffsetLen + 1] = {0};
	_sntprintf(buf, RTL_NUMBER_OF(buf), _T("x%x"), iCurByte);
	SetDlgItemText(hDlg, IDC_BMKADD_OFFSET, buf);
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
	char buf[OffsetLen + 1] = {0};
	int i, offset;
	char name[BMKTEXTMAX] = {0};
	switch (wParam)
	{
	case IDOK:
		if (GetDlgItemText(hDlg, IDC_BMKADD_OFFSET, buf, OffsetLen) &&
			sscanf(buf, "x%x", &offset) == 0 &&
			sscanf(buf, "%d", &offset) == 0)
		{
			MessageBox(hDlg, "Start offset not recognized.", "Add bookmark",
					MB_ICONERROR);
			return TRUE;
		}
		if (offset < 0 || offset > DataArray.GetLength())
		{
			MessageBox(hDlg, "Can not set bookmark at that position.",
					"Add bookmark", MB_ICONERROR);
			return TRUE;
		}
		// Is there already a bookmark on this offset?
		for (i = 0 ; i < iBmkCount ; i++)
		{
			if (pbmkList[i].offset == offset)
			{
				MessageBox(hDlg, "There already is a bookmark on that position.",
						"Add bookmark", MB_ICONERROR);
				return TRUE;
			}
		}
		// No bookmark on that position yet.
		pbmkList[iBmkCount].offset = offset;
		pbmkList[iBmkCount].name = GetDlgItemText(hDlg, IDC_BMKADD_NAME, name,
					BMKTEXTMAX) ? strdup(name) : 0;
		iBmkCount++;
		repaint();
		// fall through
	case IDCANCEL:
		EndDialog(hDlg, wParam);
		return TRUE;
	}
	return FALSE;
}

INT_PTR AddBmkDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hDlg);
	case WM_COMMAND:
		return OnCommand(hDlg, wParam, lParam);
	}
	return FALSE;
}
