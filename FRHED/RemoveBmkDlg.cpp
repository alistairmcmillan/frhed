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
 * @file  RemoveBmkDlg.cpp
 *
 * @brief Implementation of the Bookmark removing dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

BOOL RemoveBmkDlg::OnInitDialog(HWND hDlg)
{
	char buf[128] = {0};
	HWND hwndList = GetDlgItem(hDlg, IDC_REMOVEBMK_LIST);
	for (int i = 0 ; i < iBmkCount ; i++)
	{
		_snprintf(buf, RTL_NUMBER_OF(buf) - 1, pbmkList[i].name ? "%d) 0x%x:%s" : "%d) 0x%x",
			i + 1, pbmkList[i].offset, pbmkList[i].name);
		SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)buf);
	}
	SendMessage(hwndList, LB_SETCURSEL, 0, 0);
	return TRUE;
}

BOOL RemoveBmkDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	int i;
	switch (wParam)
	{
	case IDOK:
		i = SendDlgItemMessage(hDlg, IDC_REMOVEBMK_LIST, LB_GETCURSEL, 0, 0);
		if (char *name = pbmkList[i].name)
		{
			free(name);
			name = NULL;
		}
		--iBmkCount;
		for ( ; i < iBmkCount ; i++)
			pbmkList[i] = pbmkList[i + 1];
		repaint();
		// fall through
	case IDCANCEL:
		EndDialog(hDlg, wParam);
		return TRUE;
	}
	return FALSE;
}

INT_PTR RemoveBmkDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
