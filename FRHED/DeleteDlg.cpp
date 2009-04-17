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
 * @file  DeleteDlg.cpp
 *
 * @brief Implementation of the Delete dialog.
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

/** Length of the offset buffer. */
static const int OffsetLen = 16;

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE
 */
BOOL DeleteDlg::OnInitDialog(HWND hDlg)
{
	int iStart = iGetStartOfSelection();
	int iEnd = iGetEndOfSelection();
	TCHAR buf[OffsetLen + 1] = {0};

	_stprintf(buf, _T("x%x"), iStart);
	SetDlgItemText(hDlg, IDC_DELETE_STARTOFFSET, buf);
	_stprintf(buf, _T("x%x"), iEnd);

	CheckDlgButton(hDlg, IDC_DELETE_INCLUDEOFFSET, BST_CHECKED);
	SetDlgItemText(hDlg, IDC_DELETE_ENDOFFSET, buf);
	SetDlgItemInt(hDlg, IDC_DELETE_NUMBYTES, iEnd - iStart + 1, TRUE);

	return TRUE;
}

/**
 * @brief Delete the data.
 * This function deletes the data based on values user entered to the dialog.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE if the deleting succeeded, FALSE otherwise.
 */
BOOL DeleteDlg::Apply(HWND hDlg)
{
	TCHAR buf[OffsetLen + 1] = {0};
	int iOffset;
	int iNumberOfBytes;

	if (GetDlgItemText(hDlg, IDC_DELETE_STARTOFFSET, buf, OffsetLen) &&
		_stscanf(buf, _T("x%x"), &iOffset) == 0 &&
		_stscanf(buf, _T("%d"), &iOffset) == 0)
	{
		LangString startOffsetErr(IDS_OFFSET_START_ERROR);
		MessageBox(hDlg, startOffsetErr, MB_ICONERROR);
		return FALSE;
	}

	if (IsDlgButtonChecked(hDlg, IDC_DELETE_INCLUDEOFFSET))
	{
		if (GetDlgItemText(hDlg, IDC_DELETE_ENDOFFSET, buf, OffsetLen) &&
			_stscanf(buf, _T("x%x"), &iNumberOfBytes) == 0 &&
			_stscanf(buf, _T("%d"), &iNumberOfBytes) == 0)
		{
			LangString endOffsetErr(IDS_OFFSET_END_ERROR);
			MessageBox(hDlg, endOffsetErr, MB_ICONERROR);
			return FALSE;
		}
		iNumberOfBytes = iNumberOfBytes - iOffset + 1;
	}
	else
	{// Get number of bytes.
		if (GetDlgItemText(hDlg, IDC_DELETE_NUMBYTES, buf, OffsetLen) &&
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
		LangString tooMany(IDS_DELETE_TOO_MANY);
		MessageBox(hDlg, tooMany, MB_ICONERROR);
		return FALSE;
	}

	// Delete data.
	if (!DataArray.RemoveAt(iOffset, iNumberOfBytes))
	{
		LangString deleteFailed(IDS_DELETE_FAILED);
		MessageBox(hDlg, deleteFailed, MB_ICONERROR);
		return FALSE;
	}
	iCurByte = iOffset;
	if (iCurByte > DataArray.GetUpperBound())
		iCurByte = DataArray.GetUpperBound();
	if (iCurByte < 0)
		iCurByte = 0;
	iFileChanged = TRUE;
	bFilestatusChanged = true;
	bSelected = FALSE;
	resize_window();
	return TRUE;
}

/**
 * @brief Process messages.
 * @param [in] hDlg Handle to dialog.
 * @param [in] iMsg Message ID.
 * @param [in] wParam First message parameter (depends on message).
 * @param [in] lParam Second message parameter (depends on message).
 * @return TRUE if message was handled, FALSE if ignored.
 */
INT_PTR DeleteDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
				EndDialog(hDlg, wParam);
			}
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, wParam);
			return TRUE;
		}
		
		// Check for controls
		switch (LOWORD(wParam))
		{
		case IDC_DELETE_INCLUDEOFFSET:  
		case IDC_DELETE_NUMBEROFBYTES:
			EnableWindow(GetDlgItem(hDlg, IDC_DELETE_NUMBYTES),
						IsDlgButtonChecked(hDlg, IDC_DELETE_NUMBEROFBYTES));
			EnableWindow(GetDlgItem(hDlg, IDC_DELETE_ENDOFFSET),
						IsDlgButtonChecked(hDlg, IDC_DELETE_INCLUDEOFFSET));
			return TRUE;
		}
		break;

	case WM_HELP:
		OnHelp(hDlg);
		break;
	}
	return FALSE;
}
