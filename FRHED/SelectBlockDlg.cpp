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
 * @file  SelectBlockDlg.cpp
 *
 * @brief Implementation of the Block selecting dialog.
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

/** @brief Max length of the offset number. */
static const int OffsetLen = 16;

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 */
BOOL SelectBlockDlg::OnInitDialog(HWND hDlg)
{
	TCHAR buf[128] = {0};
	_sntprintf(buf, RTL_NUMBER_OF(buf) - 1, _T("x%x"), bSelected ? iStartOfSelection : iCurByte);
	SetDlgItemText(hDlg, IDC_BLOCKSEL_OFFSET, buf);
	_sntprintf(buf, RTL_NUMBER_OF(buf) - 1, _T("x%x"), bSelected ? iEndOfSelection : iCurByte);
	SetDlgItemText(hDlg, IDC_BLOCKSEL_OFFSETEND, buf);

	// Limit edit text lengths
	HWND edit = GetDlgItem(hDlg, IDC_BLOCKSEL_OFFSET);
	SendMessage(edit, EM_SETLIMITTEXT, OffsetLen, 0);
	edit = GetDlgItem(hDlg, IDC_BLOCKSEL_OFFSETEND);
	SendMessage(edit, EM_SETLIMITTEXT, OffsetLen, 0);
	return TRUE;
}

/**
 * @brief Handle dialog commands.
 * @param [in] hDlg Hanle to the dialog.
 * @param [in] wParam The command to handle.
 * @param [in] lParam Optional parameter for the command.
 * @return TRUE if the command was handled, FALSE otherwise.
 */
BOOL SelectBlockDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	TCHAR buf[128] = {0};
	int iStartOfSelSetting = 0;
	int iEndOfSelSetting =  0;
	int maxb;
	switch (wParam)
	{
	case IDOK:
		if (GetDlgItemText(hDlg, IDC_BLOCKSEL_OFFSET, buf, 128) &&
			!offset_parse(buf, iStartOfSelSetting))
		{
			LangString app(IDS_APPNAME);
			LangString startOffsetErr(IDS_OFFSET_START_ERROR);
			MessageBox(hDlg, startOffsetErr, app, MB_ICONERROR);
			return TRUE;
		}
		if (GetDlgItemText(hDlg, IDC_BLOCKSEL_OFFSETEND, buf, 128) &&
			!offset_parse(buf, iEndOfSelSetting))
		{
			LangString app(IDS_APPNAME);
			LangString endOffsetErr(IDS_OFFSET_END_ERROR);
			MessageBox(hDlg, endOffsetErr, app, MB_ICONERROR);
			return TRUE;
		}
		//make the selection valid if it is not
		maxb = DataArray.GetUpperBound();
		if (iStartOfSelSetting < 0)
			iStartOfSelSetting = 0;
		if (iStartOfSelSetting > maxb)
			iStartOfSelSetting = maxb;
		if (iEndOfSelSetting < 0)
			iEndOfSelSetting = 0;
		if (iEndOfSelSetting > maxb)
			iEndOfSelSetting = maxb;
		iStartOfSelection = iStartOfSelSetting;
		iEndOfSelection = iEndOfSelSetting;
		bSelected = true;
		adjust_view_for_selection();
		repaint();
		// fall through
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
INT_PTR SelectBlockDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
