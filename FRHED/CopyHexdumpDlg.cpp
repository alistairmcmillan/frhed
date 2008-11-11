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
 * @file  CopyHexdumpDlg.cpp
 *
 * @brief Implementation of the Hexdump copying dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

int CopyHexdumpDlg::iCopyHexdumpMode = 0;
int CopyHexdumpDlg::iCopyHexdumpType = IDC_EXPORTDISPLAY;

BOOL CopyHexdumpDlg::OnInitDialog(HWND hDlg)
{
	int iCopyHexdumpDlgStart, iCopyHexdumpDlgEnd;
	if (!bSelected)
	{
		// Assume whole file is to be hexdumped. (except the last line (if incomplete))
		iCopyHexdumpDlgStart = 0;
		iCopyHexdumpDlgEnd = DataArray.GetUpperBound() / iBytesPerLine * iBytesPerLine;
	}
	else
	{
		// Assume selected area is to be hexdumped.
		// Make sure end of selection is greater than start of selection.
		iCopyHexdumpDlgStart = iGetStartOfSelection();
		iCopyHexdumpDlgEnd = iGetEndOfSelection();
	}
	char buf[16] = {0};
	_snprintf(buf, RTL_NUMBER_OF(buf) - 1, "%x", iCopyHexdumpDlgStart);
	SetDlgItemText(hDlg, IDC_EDIT1, buf);
	_snprintf(buf, RTL_NUMBER_OF(buf) - 1, "%x", iCopyHexdumpDlgEnd);
	SetDlgItemText(hDlg, IDC_EDIT2, buf);
	CheckDlgButton(hDlg, iCopyHexdumpMode ? IDC_RADIO2 : IDC_RADIO1, BST_CHECKED);
//Pabs changed - line insert
	CheckDlgButton(hDlg, iCopyHexdumpType, BST_CHECKED);
//end
	return TRUE;
}

BOOL CopyHexdumpDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	int iCopyHexdumpDlgStart, iCopyHexdumpDlgEnd;
	char buf[16] = {0};
	switch (wParam)
	{
	case IDOK:
		if (GetDlgItemText(hDlg, IDC_EDIT1, buf, 16) &&
			sscanf(buf, "%x", &iCopyHexdumpDlgStart) &&
			GetDlgItemText(hDlg, IDC_EDIT2, buf, 16) &&
			sscanf(buf, "%x", &iCopyHexdumpDlgEnd))
		{
			iCopyHexdumpMode = IsDlgButtonChecked(hDlg, IDC_RADIO2);
//Pabs changed - line insert
			if (IsDlgButtonChecked(hDlg, IDC_EXPORTDISPLAY))
				iCopyHexdumpType = IDC_EXPORTDISPLAY;
			else if (IsDlgButtonChecked(hDlg, IDC_EXPORTDIGITS))
				iCopyHexdumpType = IDC_EXPORTDIGITS;
			else if (IsDlgButtonChecked(hDlg, IDC_EXPORTRTF))
				iCopyHexdumpType = IDC_EXPORTRTF;
//end
			EndDialog(hDlg, wParam);
			WaitCursor w1;
			CMD_copy_hexdump(iCopyHexdumpMode, iCopyHexdumpType, iCopyHexdumpDlgStart, iCopyHexdumpDlgEnd);
			return TRUE;
		}
		break;
	case IDCANCEL:
		EndDialog(hDlg, wParam);
		return TRUE;
	}
	return FALSE;
}
//-------------------------------------------------------------------
INT_PTR CopyHexdumpDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
