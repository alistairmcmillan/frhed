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
 * @file  OpenPartiallyDlg.cpp
 *
 * @brief Implementation of the dialog for opening files partially.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "LangArray.h"
#include "LangString.h"
#include "offset.h"

int OpenPartiallyDlg::filehandle = 0;
bool OpenPartiallyDlg::bShowFileStatsPL = false;

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE.
 */
BOOL OpenPartiallyDlg::OnInitDialog(HWND hDlg)
{
	INT64 iPLFileLen = _filelengthi64(filehandle);
	TCHAR buf[128] = {0};
	SetDlgItemText(hDlg, IDC_OPENPARTIAL_OFFSET, _T("x0"));
	_sntprintf(buf, RTL_NUMBER_OF(buf) - 1, GetLangString(IDS_PARTIAL_LOAD_BYTES), iPLFileLen);
	SetDlgItemText(hDlg, IDC_OPENPARTIAL_BYTECOUNT, buf);
	_sntprintf(buf, RTL_NUMBER_OF(buf) - 1, _T("%lld"), iPLFileLen);
	SetDlgItemText(hDlg, IDC_OPENPARTIAL_BYTES, buf);
	CheckDlgButton(hDlg, IDC_OPENPARTIAL_BEGINOFF, BST_CHECKED);
	const UINT state = bShowFileStatsPL ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(hDlg, IDC_OPENPARTIAL_RELOFFSET, state);
	return TRUE;
}

/**
 * @brief Apply user's selections and open the file.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE if file was successfully opened, FALSE otherwise.
 */
BOOL OpenPartiallyDlg::Apply(HWND hDlg)
{
	const INT64 iPLFileLen = _filelengthi64(filehandle);
	const UINT state = IsDlgButtonChecked(hDlg, IDC_OPENPARTIAL_RELOFFSET);
	bShowFileStatsPL = state == BST_CHECKED;
	const int bufLen = 128;
	TCHAR buf[bufLen + 1] = {0};
	UINT numBytesPl = 0; // Bytes to read
	
	// Only complain about wrong offset in start offset editbox if loading from start.
	if (GetDlgItemText(hDlg, IDC_OPENPARTIAL_BYTES, buf, bufLen) &&
		_stscanf(buf, _T("%u"), &numBytesPl) == 0)
	{
		LangString notRecognized(IDS_BYTES_NOT_KNOWN);
		MessageBox(hDlg, notRecognized, MB_ICONERROR);
		return FALSE;
	}

	if (numBytesPl >= INT_MAX)
	{
		LangString tooBig(IDS_PARTIAL_TOO_BIG);
		MessageBox(hDlg, tooBig, MB_ICONERROR);
		return FALSE;
	}

	INT64 iStartPL = 0;
	if (IsDlgButtonChecked(hDlg, IDC_OPENPARTIAL_ENDBYTES))
	{
		// Load from end of file: arguments must be adapted.
		iStartPL = iPLFileLen - numBytesPl;
		if (iStartPL < 0)
		{
			LangString biggerThan(IDS_PARTIAL_BIGGER);
			MessageBox(hDlg, biggerThan, MB_ICONERROR);
			return FALSE;
		}
	}
	else if (GetDlgItemText(hDlg, IDC_OPENPARTIAL_OFFSET, buf, bufLen) &&
		!offset_parse64(buf, iStartPL))
	{
		LangString app(IDS_APPNAME);
		LangString startOffsetErr(IDS_OFFSET_START_ERROR);
		MessageBox(hDlg, startOffsetErr, app, MB_ICONERROR);
		return FALSE;
	}
	if (iStartPL + numBytesPl > iPLFileLen)
	{
		LangString manyBytes(IDS_PARTIAL_TOO_MANY_BYTES);
		MessageBox(hDlg, manyBytes, MB_ICONERROR);
		return FALSE;
	}

	BOOL done = FALSE;
	if (DataArray.SetSize(numBytesPl))
	{
		DataArray.ExpandToSize();
		_lseeki64(filehandle, iStartPL, 0);
		iPartialOffset = iStartPL;
		iPartialOpenLen = (int) numBytesPl;
		iPartialFileLen = iPLFileLen;
		bPartialStats = bShowFileStatsPL;

		// DataArray restricts max size to 2 GB.
		if (_read(filehandle, DataArray, numBytesPl) != -1)
		{
			done = TRUE;
		}
		else
		{
			DataArray.ClearAll();
			LangString readErr(IDS_ERR_READING_FILE);
			MessageBox(hDlg, readErr, MB_ICONERROR);
		}
	}
	else
	{
		LangString noMem(IDS_NO_MEM_FOR_FILE);
		MessageBox(hDlg, noMem, MB_ICONERROR);
	}
	return done;
}

/**
 * @brief Handle dialog messages.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] iMsg The message.
 * @param [in] wParam The command in the message.
 * @param [in] lParam The optional parameter for the command.
 * @return TRUE if the message was handled, FALSE otherwise.
 */
INT_PTR OpenPartiallyDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
		case IDC_OPENPARTIAL_BEGINOFF:
		case IDC_OPENPARTIAL_ENDBYTES:
			EnableWindow(GetDlgItem(hDlg, IDC_OPENPARTIAL_OFFSET),
					IsDlgButtonChecked(hDlg, IDC_OPENPARTIAL_BEGINOFF));
			return TRUE;
		}
		break;

	case WM_HELP:
		OnHelp(hDlg);
		break;
	}
	return FALSE;
}
