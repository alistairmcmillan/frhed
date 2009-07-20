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
#include "offset.h"

int EnterDecimalValueDlg::iDecValDlgSize = 1;
bool EnterDecimalValueDlg::bSigned = false;

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE.
 */
BOOL EnterDecimalValueDlg::OnInitDialog(HWND hDlg)
{
	// Handle value as unsigned, convert to signed (if needed) when sending
	// the value to GUI.
	UINT iDecValDlgValue = 0;
	if (iCurByte >= 0 && iCurByte < m_dataArray.GetLength())
	{
		int t = m_dataArray.GetLength() - iCurByte;
		//Set the size down a bit if someone called this func with a size thats too large
		while (iDecValDlgSize > t)
			iDecValDlgSize /= 2;
		//Get the right value
		if (iDecValDlgSize == 2)
			iDecValDlgValue = *(WORD *)&m_dataArray[iCurByte];
		else if (iDecValDlgSize == 4)
			iDecValDlgValue = *(DWORD *)&m_dataArray[iCurByte];
		else
			iDecValDlgValue = (int)m_dataArray[iCurByte];
	}
	TCHAR buf[16] = {0};
	SetDlgItemInt(hDlg, IDC_DECIMAL_VALUE, iDecValDlgValue, bSigned ? TRUE : FALSE);
	_sntprintf(buf, RTL_NUMBER_OF(buf) - 1, _T("0x%x"), iCurByte);
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
	TCHAR buf[16] = {0};
	BOOL translated;
	UINT iDecValDlgValue = GetDlgItemInt(hDlg, IDC_DECIMAL_VALUE, &translated,
			bSigned ? TRUE : FALSE);
	if (!translated)
	{
		LangString notRecognized(IDS_DECI_UNKNOWN);
		MessageBox(hDlg, notRecognized, MB_ICONERROR);
		return FALSE;
	}
	int iDecValDlgOffset;
	if (GetDlgItemText(hDlg, IDC_DECIMAL_OFFSET, buf, 16) &&
		!offset_parse(buf, iDecValDlgOffset))
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
		LangString unknownTimes(IDS_DECI_UNKNOWN_TIMES);
		MessageBox(hDlg, unknownTimes, MB_ICONERROR);
		return FALSE;
	}
	if (iDecValDlgOffset < 0 || iDecValDlgOffset > m_dataArray.GetUpperBound())
	{
		LangString invalidStart(IDS_DECI_INVALID_START);
		MessageBox(hDlg, invalidStart, MB_ICONERROR);
		return FALSE;
	}
	if (iDecValDlgOffset + iDecValDlgSize * iDecValDlgTimes > m_dataArray.GetLength())
	{
		LangString noSpace(IDS_DECI_NO_SPACE);
		MessageBox(hDlg, noSpace, MB_ICONERROR);
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
				m_dataArray[iDecValDlgOffset++] = (BYTE)iDecValDlgValue;
				break;

			case 2:
				m_dataArray[iDecValDlgOffset++] = (BYTE) (iDecValDlgValue & 0xff);
				m_dataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff00) >> 8);
				break;

			case 4:
				m_dataArray[iDecValDlgOffset++] = (BYTE) (iDecValDlgValue & 0xff);
				m_dataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff00) >> 8);
				m_dataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff0000) >> 16);
				m_dataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff000000) >> 24);
				break;
			}
		}
		else
		{
			switch (iDecValDlgSize)
			{
			case 1:
				m_dataArray[iDecValDlgOffset++] = (BYTE)iDecValDlgValue;
				break;

			case 2:
				m_dataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff00) >> 8);
				m_dataArray[iDecValDlgOffset++] = (BYTE) (iDecValDlgValue & 0xff);
				break;

			case 4:
				m_dataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff000000) >> 24);
				m_dataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff0000) >> 16);
				m_dataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff00) >> 8);
				m_dataArray[iDecValDlgOffset++] = (BYTE) (iDecValDlgValue & 0xff);
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
 * @brief Handle dialog messages.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] iMsg The message.
 * @param [in] wParam The command in the message.
 * @param [in] lParam The optional parameter for the command.
 * @return TRUE if the message was handled, FALSE otherwise.
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
