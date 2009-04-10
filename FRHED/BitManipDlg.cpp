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
 * @file  BitManipDlg.cpp
 *
 * @brief Implementation of bit manipulation dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "LangString.h"

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 */
BOOL BitManipDlg::OnInitDialog(HWND hDlg)
{
	TCHAR buf[64];
	LangString manip(IDS_BITMANIP_AT_OFFSET);
	sprintf(buf, manip, iCurByte, iCurByte);
	SetDlgItemText(hDlg, IDC_MANIPBITS, buf);
	BYTE cBitValue = DataArray[iCurByte];
	if (cBitValue & 1)
		CheckDlgButton(hDlg, IDC_MANIPBITS_BIT1, BST_CHECKED);
	if (cBitValue & 2)
		CheckDlgButton(hDlg, IDC_MANIPBITS_BIT2, BST_CHECKED);
	if (cBitValue & 4)
		CheckDlgButton(hDlg, IDC_MANIPBITS_BIT3, BST_CHECKED);
	if (cBitValue & 8)
		CheckDlgButton(hDlg, IDC_MANIPBITS_BIT4, BST_CHECKED);
	if (cBitValue & 16)
		CheckDlgButton(hDlg, IDC_MANIPBITS_BIT5, BST_CHECKED);
	if (cBitValue & 32)
		CheckDlgButton(hDlg, IDC_MANIPBITS_BIT6, BST_CHECKED);
	if (cBitValue & 64)
		CheckDlgButton(hDlg, IDC_MANIPBITS_BIT7, BST_CHECKED);
	if (cBitValue & 128)
		CheckDlgButton(hDlg, IDC_MANIPBITS_BIT8, BST_CHECKED);
	Apply(hDlg, 0);
	return TRUE;
}

/**
 * @brief Apply the manipulation.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] wParam Command user selected.
 * @return TRUE if bytes were copied, FALSE otherwise.
 */
BOOL BitManipDlg::Apply(HWND hDlg, WPARAM wParam)
{
	BYTE cBitValue = 0;
	if (IsDlgButtonChecked(hDlg, IDC_MANIPBITS_BIT8))
		cBitValue |= 128;
	if (IsDlgButtonChecked(hDlg, IDC_MANIPBITS_BIT7))
		cBitValue |= 64;
	if (IsDlgButtonChecked(hDlg, IDC_MANIPBITS_BIT6))
		cBitValue |= 32;
	if (IsDlgButtonChecked(hDlg, IDC_MANIPBITS_BIT5))
		cBitValue |= 16;
	if (IsDlgButtonChecked(hDlg, IDC_MANIPBITS_BIT4))
		cBitValue |= 8;
	if (IsDlgButtonChecked(hDlg, IDC_MANIPBITS_BIT3))
		cBitValue |= 4;
	if (IsDlgButtonChecked(hDlg, IDC_MANIPBITS_BIT2))
		cBitValue |= 2;
	if (IsDlgButtonChecked(hDlg, IDC_MANIPBITS_BIT1))
		cBitValue |= 1;
	if (wParam == IDOK)
	{
		DataArray[iCurByte] = cBitValue;
		iFileChanged = TRUE;
		bFilestatusChanged = true;
		repaint();
		return TRUE;
	}
	TCHAR buf[64];
	LangString value(IDS_BITMANIP_VALUE);
	sprintf(buf, value,	(unsigned char)cBitValue,
			(signed char)cBitValue, (unsigned char)cBitValue);
	SetDlgItemText(hDlg, IDC_MANIPBITS_VALUE, buf);
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
INT_PTR BitManipDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hDlg);
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
		case IDC_MANIPBITS_BIT1: case IDC_MANIPBITS_BIT2: case IDC_MANIPBITS_BIT3:
		case IDC_MANIPBITS_BIT4: case IDC_MANIPBITS_BIT5: case IDC_MANIPBITS_BIT6:
		case IDC_MANIPBITS_BIT7: case IDC_MANIPBITS_BIT8:
			if (Apply(hDlg, wParam))
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
