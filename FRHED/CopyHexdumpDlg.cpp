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

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 */
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
	TCHAR buf[16] = {0};
	_sntprintf(buf, RTL_NUMBER_OF(buf) - 1, _T("%x"), iCopyHexdumpDlgStart);
	SetDlgItemText(hDlg, IDC_HEXDUMP_OFFSET, buf);
	_sntprintf(buf, RTL_NUMBER_OF(buf) - 1, _T("%x"), iCopyHexdumpDlgEnd);
	SetDlgItemText(hDlg, IDC_HEXDUMP_OFFSET2, buf);
	CheckDlgButton(hDlg, iCopyHexdumpMode ? IDC_HEXDUMP_EXPORTCLIPB :
			IDC_HEXDUMP_EXPORTFILE, BST_CHECKED);
	CheckDlgButton(hDlg, iCopyHexdumpType, BST_CHECKED);
	return TRUE;
}

/**
 * @brief Handle dialog commands.
 * @param [in] hDlg Hanle to the dialog.
 * @param [in] wParam The command to handle.
 * @param [in] lParam Optional parameter for the command.
 * @return TRUE if the command was handled, FALSE otherwise.
 */
BOOL CopyHexdumpDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	int iCopyHexdumpDlgStart, iCopyHexdumpDlgEnd;
	const int bufSize = 16;
	TCHAR buf[bufSize + 1] = {0};
	switch (wParam)
	{
	case IDOK:
		if (GetDlgItemText(hDlg, IDC_HEXDUMP_OFFSET, buf, bufSize) &&
			_stscanf(buf, _T("%x"), &iCopyHexdumpDlgStart) &&
			GetDlgItemText(hDlg, IDC_HEXDUMP_OFFSET2, buf, bufSize) &&
			_stscanf(buf, _T("%x"), &iCopyHexdumpDlgEnd))
		{
			iCopyHexdumpMode = IsDlgButtonChecked(hDlg, IDC_HEXDUMP_EXPORTCLIPB);
			if (IsDlgButtonChecked(hDlg, IDC_EXPORTDISPLAY))
				iCopyHexdumpType = IDC_EXPORTDISPLAY;
			else if (IsDlgButtonChecked(hDlg, IDC_EXPORTDIGITS))
				iCopyHexdumpType = IDC_EXPORTDIGITS;
			else if (IsDlgButtonChecked(hDlg, IDC_EXPORTRTF))
				iCopyHexdumpType = IDC_EXPORTRTF;

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

/**
 * @brief Handle dialog messages.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] iMsg The message.
 * @param [in] wParam The command in the message.
 * @param [in] lParam The optional parameter for the command.
 * @return TRUE if the message was handled, FALSE otherwise.
 */INT_PTR CopyHexdumpDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
