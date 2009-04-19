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
 * @file  CopyDlg.cpp
 *
 * @brief Copy dialog implementation.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "BinTrans.h"
#include "LangString.h"

/** @brief Maximum text length in dialog edit boxes. */
static const int MaxTextLen = 16;

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE.
 */
BOOL CopyDlg::OnInitDialog(HWND hDlg)
{
	int iStart = iGetStartOfSelection();
	int iEnd = iGetEndOfSelection();
	TCHAR buf[32] = {0};
	_sntprintf(buf, RTL_NUMBER_OF(buf) - 1, _T("x%x"), iStart);
	SetDlgItemText(hDlg, IDC_COPY_STARTOFFSET, buf);
	_sntprintf(buf, RTL_NUMBER_OF(buf) - 1, _T("x%x"), iEnd);
	SetDlgItemText(hDlg, IDC_COPY_OFFSETEDIT, buf);
	SetDlgItemInt(hDlg, IDC_COPY_BYTECOUNT, iEnd - iStart + 1, TRUE);
	CheckDlgButton(hDlg, IDC_COPY_OFFSET, BST_CHECKED);

	// Limit edit text lengths
	HWND edit = GetDlgItem(hDlg, IDC_COPY_STARTOFFSET);
	SendMessage(edit, EM_SETLIMITTEXT, MaxTextLen, 0);
	edit = GetDlgItem(hDlg, IDC_COPY_OFFSETEDIT);
	SendMessage(edit, EM_SETLIMITTEXT, MaxTextLen, 0);
	edit = GetDlgItem(hDlg, IDC_COPY_BYTECOUNT);
	SendMessage(edit, EM_SETLIMITTEXT, MaxTextLen, 0);
	return TRUE;
}

/**
 * @brief Copy the bytes.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE if bytes were copied, FALSE otherwise.
 */
BOOL CopyDlg::Apply(HWND hDlg)
{
	const int bufSize = 64;
	TCHAR buf[bufSize + 1] = {0};
	int iOffset;
	int iNumberOfBytes;
	if (GetDlgItemText(hDlg, IDC_COPY_STARTOFFSET, buf, bufSize) &&
		_stscanf(buf, _T("x%x"), &iOffset) == 0 &&
		_stscanf(buf, _T("%d"), &iOffset) == 0)
	{
		LangString app(IDS_APPNAME);
		LangString startOffsetErr(IDS_OFFSET_START_ERROR);
		MessageBox(hDlg, startOffsetErr, app, MB_ICONERROR);
		return FALSE;
	}
	if (IsDlgButtonChecked(hDlg, IDC_COPY_OFFSET))
	{
		if (GetDlgItemText(hDlg, IDC_COPY_OFFSETEDIT, buf, bufSize) &&
			_stscanf(buf, _T("x%x"), &iNumberOfBytes) == 0 &&
			_stscanf(buf, _T("%d"), &iNumberOfBytes) == 0)
		{
			LangString app(IDS_APPNAME);
			LangString endOffsetErr(IDS_OFFSET_END_ERROR);
			MessageBox(hDlg, endOffsetErr, app, MB_ICONERROR);
			return FALSE;
		}
		iNumberOfBytes = iNumberOfBytes - iOffset + 1;
	}
	else
	{// Get number of bytes.
		if (GetDlgItemText(hDlg, IDC_COPY_BYTECOUNT, buf, bufSize) &&
			_stscanf(buf, _T("%d"), &iNumberOfBytes) == 0)
		{
			LangString notRecognized(IDS_BYTES_NOT_KNOWN);
			MessageBox(hDlg, notRecognized, MB_ICONERROR);
			return FALSE;
		}
	}
	// Can requested number be cut?
	// DataArray.GetLength ()-iCutOffset = number of bytes from current pos. to end.
	if (DataArray.GetLength() - iOffset < iNumberOfBytes)
	{
		LangString copyTooMany(IDS_COPY_TOO_MANY);
		MessageBox(hDlg, copyTooMany, MB_ICONERROR);
		return FALSE;
	}
	// Transfer to clipboard.
	int destlen = Text2BinTranslator::iBytes2BytecodeDestLen((TCHAR*) &DataArray[iOffset], iNumberOfBytes);
	HGLOBAL hGlobal = GlobalAlloc(GHND, destlen);
	if (hGlobal == 0)
	{
		// Not enough memory for clipboard.
		LangString noMem(IDS_COPY_NO_MEM);
		MessageBox(hDlg, noMem, MB_ICONERROR);
		return FALSE;
	}
	WaitCursor wc;
	TCHAR *pd = (TCHAR *)GlobalLock(hGlobal);
	Text2BinTranslator::iTranslateBytesToBC(pd, &DataArray[iOffset], iNumberOfBytes);
	GlobalUnlock(hGlobal);
	OpenClipboard(hwnd);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hGlobal);
	CloseClipboard();
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
INT_PTR CopyDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
		case IDC_COPY_OFFSET:
		case IDC_COPY_BYTES:
			EnableWindow(GetDlgItem(hDlg, IDC_COPY_OFFSETEDIT),
					IsDlgButtonChecked(hDlg, IDC_COPY_OFFSET));
			EnableWindow(GetDlgItem(hDlg, IDC_COPY_BYTECOUNT),
					IsDlgButtonChecked(hDlg, IDC_COPY_BYTES));
			return TRUE;
		}
		break;

	case WM_HELP:
		OnHelp(hDlg);
		break;
	}
	return FALSE;
}
