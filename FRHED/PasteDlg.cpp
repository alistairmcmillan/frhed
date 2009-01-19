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
 * @file  PasteDlg.cpp
 *
 * @brief Implementation of the Paste dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to dialog.
 * @return TRUE.
 */
BOOL PasteDlg::OnInitDialog(HWND hDlg)
{
	if (bSelected) // iPasteMode = 0
	{
		EnableWindow(GetDlgItem(hDlg, IDC_PASTE_OVERWRITE), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_PASTE_INSERT), FALSE);
	}
	else if (bInsertMode) // iPasteMode = 2
	{
		CheckDlgButton(hDlg, IDC_PASTE_INSERT, BST_CHECKED);
	}
	else // iPasteMode = 1
	{
		CheckDlgButton(hDlg, IDC_PASTE_OVERWRITE, BST_CHECKED);
	}
	SendDlgItemMessage(hDlg, IDC_PASTE_CLIPBOARD, WM_PASTE, 0, 0);
	SetDlgItemInt(hDlg, IDC_PASTE_TIMES, iPasteTimes, TRUE);
	SetDlgItemInt(hDlg, IDC_PASTE_SKIPBYTES, iPasteSkip, TRUE);
	CheckDlgButton(hDlg, IDC_PASTE_BINARY, iPasteAsText);

	// Limit edit text lengths
	HWND edit = GetDlgItem(hDlg, IDC_PASTE_TIMES);
	SendMessage(edit, EM_SETLIMITTEXT, 16, 0);
	edit = GetDlgItem(hDlg, IDC_PASTE_SKIPBYTES);
	SendMessage(edit, EM_SETLIMITTEXT, 16, 0);

	return TRUE;
}

/**
 * @brief Paste the bytes.
 * @param [in] hDlg Handle to dialog.
 * @return TRUE if paste succeeded, FALSE if failed.
 */
BOOL PasteDlg::Apply(HWND hDlg)
{
	iPasteAsText = IsDlgButtonChecked(hDlg, IDC_PASTE_BINARY);
	iPasteTimes = GetDlgItemInt(hDlg, IDC_PASTE_TIMES, 0, TRUE);
	if (iPasteTimes <= 0)
	{
		MessageBox(hDlg, "Number of times to paste must be at least 1.", "Paste", MB_ICONERROR);
		return FALSE;
	}
	iPasteSkip = GetDlgItemInt(hDlg, IDC_PASTE_SKIPBYTES, 0, TRUE);
	HWND hwndEdit1 = GetDlgItem(hDlg, IDC_PASTE_CLIPBOARD);
	int destlen = GetWindowTextLength(hwndEdit1) + 1;
	char *pcPastestring = new char[destlen];
	destlen = GetWindowText(hwndEdit1, pcPastestring, destlen);
	if (iPasteAsText == 0)
	{
		char *pc = 0;
		destlen = create_bc_translation(&pc, pcPastestring, strlen(pcPastestring));
		delete [] pcPastestring;
		pcPastestring = pc;
	}
	if (destlen == 0)
	{
		MessageBox(hDlg, "Tried to paste zero-length array.", "Paste", MB_ICONERROR);
		delete [] pcPastestring;
		return FALSE;
	}
	WaitCursor wc1;
	if (bSelected || IsDlgButtonChecked(hDlg, IDC_PASTE_INSERT))
	{
		// Insert at iCurByte. Bytes there will be pushed up.
		if (bSelected)
		{
			iCurByte = iGetStartOfSelection();
			int iEndByte = iGetEndOfSelection();
			DataArray.RemoveAt(iCurByte, iEndByte - iCurByte + 1);//Remove extraneous data
			bSelected = FALSE;//Deselect
		}
		int i = iCurByte;
		for (int k = 0 ; k < iPasteTimes ; k++)
		{
			if (!DataArray.InsertAtGrow(i, (unsigned char*)pcPastestring, 0, destlen))
			{
				MessageBox(hDlg, "Not enough memory for inserting.", "Paste", MB_ICONERROR);
				break;
			}
			i += destlen + iPasteSkip;
		}
		iFileChanged = TRUE;
		bFilestatusChanged = true;
		resize_window();
	}
	else
	{
		// Overwrite.
		// Enough space for writing?
		// DataArray.GetLength()-iCurByte = number of bytes from including curbyte to end.
		if (DataArray.GetLength() - iCurByte < (iPasteSkip + destlen) * iPasteTimes)
		{
			MessageBox(hDlg, "Not enough space for overwriting.", "Paste", MB_ICONERROR);
			delete [] pcPastestring;
			return TRUE;
		}
		// Overwrite data.
		for (int k = 0 ; k < iPasteTimes ; k++)
		{
			for (int i = 0 ; i < destlen ; i++)
			{
				DataArray[iCurByte + k * (iPasteSkip + destlen) + i] = pcPastestring[i];
			}
		}
		iFileChanged = TRUE;
		bFilestatusChanged = true;
		repaint();
	}
	delete [] pcPastestring;
	return TRUE;
}

BOOL PasteDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM)
{
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
	return FALSE;
}

INT_PTR PasteDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
