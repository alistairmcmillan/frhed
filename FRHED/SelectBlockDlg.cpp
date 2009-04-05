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

BOOL SelectBlockDlg::OnInitDialog(HWND hDlg)
{
	char buf[128] = {0};
	_snprintf(buf, RTL_NUMBER_OF(buf) - 1, "x%x", bSelected ? iStartOfSelection : iCurByte);
	SetDlgItemText(hDlg, IDC_BLOCKSEL_OFFSET, buf);
	_snprintf(buf, RTL_NUMBER_OF(buf) - 1, "x%x", bSelected ? iEndOfSelection : iCurByte);
	SetDlgItemText(hDlg, IDC_BLOCKSEL_OFFSETEND, buf);
	return TRUE;
}

BOOL SelectBlockDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	char buf[128] = {0};
	int iStartOfSelSetting;
	int iEndOfSelSetting;
	int maxb;
	switch (wParam)
	{
	case IDOK:
		if (GetDlgItemText(hDlg, IDC_BLOCKSEL_OFFSET, buf, 128) &&
			sscanf(buf, "x%x", &iStartOfSelSetting) == 0 &&
			sscanf(buf, "%d", &iStartOfSelSetting) == 0)
		{
			LangString app(IDS_APPNAME);
			LangString startOffsetErr(IDS_OFFSET_START_ERROR);
			MessageBox(hDlg, startOffsetErr, app, MB_ICONERROR);
			return TRUE;
		}
		if (GetDlgItemText(hDlg, IDC_BLOCKSEL_OFFSETEND, buf, 128) &&
			sscanf(buf, "x%x", &iEndOfSelSetting) == 0 &&
			sscanf(buf, "%d", &iEndOfSelSetting) == 0)
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
		bSelected = TRUE;
		adjust_view_for_selection();
		repaint();
		// fall through
	case IDCANCEL:
		EndDialog(hDlg, wParam);
		return TRUE;
	}
	return FALSE;
}

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
