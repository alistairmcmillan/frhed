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

int OpenPartiallyDlg::filehandle = 0;
int OpenPartiallyDlg::bShowFileStatsPL = 0;

BOOL OpenPartiallyDlg::OnInitDialog(HWND hDlg)
{
	__int64 iPLFileLen = _filelengthi64(filehandle);
	char buf[128] = {0};
	SetDlgItemText(hDlg, IDC_OPENPARTIAL_OFFSET, "x0");
	_snprintf(buf, RTL_NUMBER_OF(buf) - 1, "Size of file: %lld. Load how many bytes:", iPLFileLen);
	SetDlgItemText(hDlg, IDC_STATIC2, buf);
	_snprintf(buf, RTL_NUMBER_OF(buf) - 1, "%lld", iPLFileLen);
	SetDlgItemText(hDlg, IDC_OPENPARTIAL_BYTES, buf);
	CheckDlgButton(hDlg, IDC_OPENPARTIAL_BEGINOFF, BST_CHECKED);
	CheckDlgButton(hDlg, IDC_OPENPARTIAL_RELOFFSET, bShowFileStatsPL);
	return TRUE;
}

BOOL OpenPartiallyDlg::Apply(HWND hDlg)
{
	__int64 iPLFileLen = _filelengthi64(filehandle);
	bShowFileStatsPL = IsDlgButtonChecked(hDlg, IDC_OPENPARTIAL_RELOFFSET);
	char buf[128] = {0};
	__int64 iNumBytesPl;
	
	// Only complain about wrong offset in start offset editbox if loading from start.
	if (GetDlgItemText(hDlg, IDC_OPENPARTIAL_BYTES, buf, 128) &&
		sscanf(buf, "%lld", &iNumBytesPl) == 0)
	{
		MessageBox(hDlg, "Number of bytes not recognized.", "Open partially", MB_ICONERROR);
		return FALSE;
	}

	if (iNumBytesPl >= INT_MAX)
	{
		MessageBox(hDlg, "Cannot open more than 2 GB of data.",
				"Open partially", MB_ICONERROR);
		return FALSE;
	}

	__int64 iStartPL;
	if (IsDlgButtonChecked(hDlg, IDC_OPENPARTIAL_ENDBYTES))
	{
		// Load from end of file: arguments must be adapted.
		iStartPL = iPLFileLen - iNumBytesPl;
		if (iStartPL < 0)
		{
			MessageBox(hDlg, "Specified number of bytes to load\ngreater than file size.", "Open partially", MB_ICONERROR);
			return FALSE;
		}
	}
	else if (GetDlgItemText(hDlg, IDC_OPENPARTIAL_OFFSET, buf, 128) &&
		sscanf(buf, "x%x", &iStartPL) == 0 &&
		sscanf(buf, "%lld", &iStartPL) == 0)
	{
		MessageBox(hDlg, "Start offset not recognized.", "Open partially", MB_ICONERROR);
		return FALSE;
	}
	if (iStartPL + iNumBytesPl > iPLFileLen)
	{
		MessageBox(hDlg, "Too many bytes to load.", "Open partially", MB_ICONERROR);
		return FALSE;
	}

	BOOL done = FALSE;
	if (DataArray.SetSize((int)iNumBytesPl))
	{
		DataArray.ExpandToSize();
		_lseeki64(filehandle, iStartPL, 0);
		iPartialOffset = iStartPL;
		iPartialOpenLen = (int)iNumBytesPl;
		iPartialFileLen = iPLFileLen;
		bPartialStats = bShowFileStatsPL;
		if (_read(filehandle, DataArray, iNumBytesPl) != -1)
		{
			done = TRUE;
		}
		else
		{
			DataArray.ClearAll();
			MessageBox(hDlg, "Error while reading from file.", "Open partially", MB_ICONERROR);
		}
	}
	else
	{
		MessageBox(hDlg, "Not enough memory to load file.", "Open partially", MB_ICONERROR);
	}
	return done;
}

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
	}
	return FALSE;
}
