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
#include "BinTrans.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "LangString.h"

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
	if (bPasteAsText)
		CheckDlgButton(hDlg, IDC_PASTE_BINARY, BST_CHECKED);
	else
		CheckDlgButton(hDlg, IDC_PASTE_BINARY, BST_UNCHECKED);

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
	bPasteAsText = (IsDlgButtonChecked(hDlg, IDC_PASTE_BINARY) == BST_CHECKED);
	iPasteTimes = GetDlgItemInt(hDlg, IDC_PASTE_TIMES, 0, TRUE);
	if (iPasteTimes <= 0)
	{
		LangString atleastOnce(IDS_PASTE_ATLEAST_ONCE);
		MessageBox(hDlg, atleastOnce, MB_ICONERROR);
		return FALSE;
	}
	iPasteSkip = GetDlgItemInt(hDlg, IDC_PASTE_SKIPBYTES, 0, TRUE);
	HWND hwndEdit1 = GetDlgItem(hDlg, IDC_PASTE_CLIPBOARD);
	int destlen = GetWindowTextLength(hwndEdit1) + 1;
	TCHAR *pcPastestring = new TCHAR[destlen];
	destlen = GetWindowText(hwndEdit1, pcPastestring, destlen);
	if (!bPasteAsText)
	{
		TCHAR *pc = 0;
		destlen = create_bc_translation(&pc, pcPastestring,
			_tcslen(pcPastestring), iCharacterSet, iBinaryMode);
		delete [] pcPastestring;
		pcPastestring = pc;
	}
	if (destlen == 0)
	{
		LangString zeroLenArray(IDS_PASTE_WAS_EMPTY);
		MessageBox(hDlg, zeroLenArray, MB_ICONERROR);
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
			m_dataArray.RemoveAt(iCurByte, iEndByte - iCurByte + 1);//Remove extraneous data
			bSelected = false; // Deselect
		}
		int i = iCurByte;
		for (int k = 0 ; k < iPasteTimes ; k++)
		{
			if (!m_dataArray.InsertAtGrow(i, (BYTE*)pcPastestring, 0, destlen))
			{
				LangString noMem(IDS_PASTE_NO_MEM);
				MessageBox(hDlg, noMem, MB_ICONERROR);
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
		// m_dataArray.GetLength()-iCurByte = number of bytes from including curbyte to end.
		if (m_dataArray.GetLength() - iCurByte < (iPasteSkip + destlen) * iPasteTimes)
		{
			LangString noSpace(IDS_PASTE_NO_SPACE);
			MessageBox(hDlg, noSpace, MB_ICONERROR);
			delete [] pcPastestring;
			return TRUE;
		}
		// Overwrite data.
		for (int k = 0 ; k < iPasteTimes ; k++)
		{
			for (int i = 0 ; i < destlen ; i++)
			{
				m_dataArray[iCurByte + k * (iPasteSkip + destlen) + i] = pcPastestring[i];
			}
		}
		iFileChanged = TRUE;
		bFilestatusChanged = true;
		repaint();
	}
	delete [] pcPastestring;
	return TRUE;
}

/**
 * @brief Dialog control message handler.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] wParam Control ID.
 */
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

/**
 * @brief Handle dialog messages.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] iMsg The message.
 * @param [in] wParam The command in the message.
 * @param [in] lParam The optional parameter for the command.
 * @return TRUE if the message was handled, FALSE otherwise.
 */
INT_PTR PasteDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
