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
 * @file  ReverseDlg.cpp
 *
 * @brief Implementation of the Data reversing dialog.
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

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 */
BOOL ReverseDlg::OnInitDialog(HWND hDlg)
{
	TCHAR buf[32];
	_sntprintf(buf, RTL_NUMBER_OF(buf) - 1, _T("x%x"), iGetStartOfSelection());
	SetDlgItemText (hDlg, IDC_REVERSE_OFFSET, buf);
	_sntprintf(buf, RTL_NUMBER_OF(buf) - 1, _T("x%x"), iGetEndOfSelection());
	SetDlgItemText(hDlg, IDC_REVERSE_OFFSETEND, buf);
	return TRUE;
}

/**
 * @brief Called when user closes the dialog with OK-button.
 * @param [in] hDlg Hanle to the dialog.
 * @param [in] wParam The command to handle.
 * @param [in] lParam Optional parameter for the command.
 * @return TRUE if the command was handled, FALSE otherwise.
 */
BOOL ReverseDlg::OnOK(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	TCHAR buf[32];
	int iStartOfSelSetting = 0;
	int iEndOfSelSetting = 0;
	int maxb;

	if (GetDlgItemText(hDlg, IDC_REVERSE_OFFSET, buf, RTL_NUMBER_OF(buf)) &&
		!offset_parse(buf, iStartOfSelSetting))
	{
		LangString startOffsetErr(IDS_OFFSET_START_ERROR);
		MessageBox(hDlg, startOffsetErr, MB_ICONERROR);
		return TRUE;
	}
	if (GetDlgItemText(hDlg, IDC_REVERSE_OFFSETEND, buf, RTL_NUMBER_OF(buf)) &&
		!offset_parse(buf, iEndOfSelSetting))
	{
		LangString endOffsetErr(IDS_OFFSET_END_ERROR);
		MessageBox(hDlg, endOffsetErr, MB_ICONERROR);
		return TRUE;
	}
	if (iEndOfSelSetting == iStartOfSelSetting)
	{
		LangString singleByte(IDS_REVERSE_ONE_BYTE);
		MessageBox(hDlg, singleByte, MB_ICONERROR);
		return TRUE;
	}

	SetCursor (LoadCursor (NULL, IDC_WAIT));
	if (iEndOfSelSetting < iStartOfSelSetting)
		swap(iEndOfSelSetting, iStartOfSelSetting);

	maxb = m_dataArray.GetUpperBound();
	if (iStartOfSelSetting < 0 || iEndOfSelSetting > maxb)
	{
		LangString extendBlock(IDS_REVERSE_BLOCK_EXTEND);
		MessageBox(hDlg, extendBlock, MB_ICONERROR);
	}

	if (iStartOfSelSetting < 0)
		iStartOfSelSetting = 0;
	if (iEndOfSelSetting > maxb)
		iEndOfSelSetting = maxb;
	reverse_bytes(&m_dataArray[iStartOfSelSetting], &m_dataArray[iEndOfSelSetting]);
	if (bSelected)
	{
		//If the selection was inside the bit that was reversed, then reverse it too
		if (iStartOfSelSetting <= iStartOfSelection &&
			iStartOfSelSetting <= iEndOfSelection &&
			iEndOfSelSetting >= iStartOfSelection &&
			iEndOfSelSetting >= iEndOfSelection)
		{
			iStartOfSelection = iEndOfSelSetting - iStartOfSelection + iStartOfSelSetting;
			iEndOfSelection = iEndOfSelSetting - iEndOfSelection + iStartOfSelSetting;
		}
		else
		{
			bSelected = false;
		}//If the above is not true deselect - this may change when multiple selections are allowed
	}
	//Or if the current byte was in the reversed bytes reverse it too
	else if (iCurByte >= iStartOfSelSetting &&
		iCurByte <= iEndOfSelSetting)
	{
		iCurByte = iEndOfSelSetting - iCurByte + iStartOfSelSetting;
		iCurNibble = !iCurNibble;
	}
	SetCursor (LoadCursor (NULL, IDC_ARROW));
	iFileChanged = TRUE;
	bFilestatusChanged = true;
	repaint();
	return TRUE;
}

/**
 * @brief Handle dialog commands.
 * @param [in] hDlg Hanle to the dialog.
 * @param [in] wParam The command to handle.
 * @param [in] lParam Optional parameter for the command.
 * @return TRUE if the command was handled, FALSE otherwise.
 */
BOOL ReverseDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case IDOK:
		return OnOK(hDlg, wParam, lParam);

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
 */
INT_PTR ReverseDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
