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
 * @file  EnterDecimalValueDlg.cpp
 *
 * @brief Implementation of the dialog for entering decimal value.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "LangString.h"

int EnterDecimalValueDlg::iDecValDlgSize = 1;

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE.
 */
BOOL EnterDecimalValueDlg::OnInitDialog(HWND hDlg)
{
	int iDecValDlgValue = 0;
	if (iCurByte >= 0 && iCurByte < DataArray.GetLength())
	{
		int t = DataArray.GetLength() - iCurByte;
		//Set the size down a bit if someone called this func with a size thats too large
		while (iDecValDlgSize > t)
			iDecValDlgSize /= 2;
		//Get the right value
		if (iDecValDlgSize == 2)
			iDecValDlgValue = *(WORD *)&DataArray[iCurByte];
		else if (iDecValDlgSize == 4)
			iDecValDlgValue = *(DWORD *)&DataArray[iCurByte];
		else
			iDecValDlgValue = (int)DataArray[iCurByte];
	}
	char buf[16] = {0};
	SetDlgItemInt(hDlg, IDC_DECIMAL_VALUE, iDecValDlgValue, TRUE);
	_snprintf(buf, RTL_NUMBER_OF(buf) - 1, "x%x", iCurByte);
	SetDlgItemText(hDlg, IDC_DECIMAL_OFFSET, buf);
	SetDlgItemInt(hDlg, IDC_DECIMAL_TIMES, 1, TRUE);
	CheckDlgButton(hDlg,
		iDecValDlgSize == 4 ? IDC_DECIMAL_DWORD :
		iDecValDlgSize == 2 ? IDC_DECIMAL_WORD :
		IDC_DECIMAL_BYTE,
		BST_CHECKED);
	return TRUE;
}

/**
 * @brief Apply values from the dialog when dialog is closed.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE if values are valid, FALSE if there are errors.
 */
BOOL EnterDecimalValueDlg::Apply(HWND hDlg)
{
	iDecValDlgSize =
		IsDlgButtonChecked(hDlg, IDC_DECIMAL_DWORD) ? 4 :
		IsDlgButtonChecked(hDlg, IDC_DECIMAL_WORD) ? 2 :
		1;
	char buf[16] = {0};
	BOOL translated;
	int iDecValDlgValue = GetDlgItemInt(hDlg, IDC_DECIMAL_VALUE, &translated,
			TRUE);
	if (!translated)
	{
		LangString app(IDS_APPNAME);
		MessageBox(hDlg, "Decimal value not recognized.", app, MB_ICONERROR);
		return FALSE;
	}
	int iDecValDlgOffset;
	if (GetDlgItemText(hDlg, IDC_DECIMAL_OFFSET, buf, 16) &&
		sscanf(buf, "%d", &iDecValDlgOffset) == 0 && 
		sscanf(buf, "x%x", &iDecValDlgOffset) == 0)
	{
		LangString app(IDS_APPNAME);
		LangString offsetErr(IDS_OFFSET_ERROR);
		MessageBox(hDlg, offsetErr, app, MB_ICONERROR);
		return FALSE;
	}
	int iDecValDlgTimes = GetDlgItemInt(hDlg, IDC_DECIMAL_TIMES, &translated,
			TRUE);
	if (!translated)
	{
		LangString app(IDS_APPNAME);
		MessageBox(hDlg, "Number of times not recognized.", app, MB_ICONERROR);
		return FALSE;
	}
	if (iDecValDlgOffset < 0 || iDecValDlgOffset > DataArray.GetUpperBound())
	{
		LangString app(IDS_APPNAME);
		MessageBox(hDlg, "Invalid start offset.", app, MB_ICONERROR);
		return FALSE;
	}
	if (iDecValDlgOffset + iDecValDlgSize * iDecValDlgTimes > DataArray.GetLength())
	{
		LangString app(IDS_APPNAME);
		MessageBox(hDlg, "Not enough space for writing decimal values.", app,
				MB_ICONERROR);
		return FALSE;
	}
	WaitCursor wc;
	while (iDecValDlgTimes)
	{
		if (iBinaryMode == ENDIAN_LITTLE)
		{
			switch (iDecValDlgSize)
			{
			case 1:
				DataArray[iDecValDlgOffset++] = (BYTE)iDecValDlgValue;
				break;

			case 2:
				DataArray[iDecValDlgOffset++] = (BYTE) (iDecValDlgValue & 0xff);
				DataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff00) >> 8);
				break;

			case 4:
				DataArray[iDecValDlgOffset++] = (BYTE) (iDecValDlgValue & 0xff);
				DataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff00) >> 8);
				DataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff0000) >> 16);
				DataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff000000) >> 24);
				break;
			}
		}
		else
		{
			switch (iDecValDlgSize)
			{
			case 1:
				DataArray[iDecValDlgOffset++] = (BYTE)iDecValDlgValue;
				break;

			case 2:
				DataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff00) >> 8);
				DataArray[iDecValDlgOffset++] = (BYTE) (iDecValDlgValue & 0xff);
				break;

			case 4:
				DataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff000000) >> 24);
				DataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff0000) >> 16);
				DataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff00) >> 8);
				DataArray[iDecValDlgOffset++] = (BYTE) (iDecValDlgValue & 0xff);
				break;
			}
		}
		--iDecValDlgTimes;
	}
	iFileChanged = TRUE;
	bFilestatusChanged = true;
	repaint();
	return TRUE;
}

/**
 * @brief Dialog message handler.
 */
INT_PTR EnterDecimalValueDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
		}
		break;

	case WM_HELP:
		OnHelp(hDlg);
		break;
	}
	return FALSE;
}
