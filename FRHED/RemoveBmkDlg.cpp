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

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 */
BOOL RemoveBmkDlg::OnInitDialog(HWND hDlg)
{
	TCHAR buf[128] = {0};
	HWND hwndList = GetDlgItem(hDlg, IDC_REMOVEBMK_LIST);
	for (int i = 0 ; i < iBmkCount ; i++)
	{
		if (pbmkList[i].name)
			_stprintf(buf, _T("%d %s (0x%x)"), i + 1, pbmkList[i].name,
					pbmkList[i].offset);
		else
			_stprintf(buf, _T("%d 0x%x"), i + 1, pbmkList[i].offset,
					pbmkList[i].name);
		SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)buf);
	}
	SendMessage(hwndList, LB_SETCURSEL, 0, 0);
	return TRUE;
}

/**
 * @brief Handle dialog commands.
 * @param [in] hDlg Hanle to the dialog.
 * @param [in] wParam The command to handle.
 * @param [in] lParam Optional parameter for the command.
 * @return TRUE if the command was handled, FALSE otherwise.
 */
BOOL RemoveBmkDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	int i;
	switch (wParam)
	{
	case IDOK:
		i = SendDlgItemMessage(hDlg, IDC_REMOVEBMK_LIST, LB_GETCURSEL, 0, 0);
		if (TCHAR *name = pbmkList[i].name)
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

/**
 * @brief Handle dialog messages.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] iMsg The message.
 * @param [in] wParam The command in the message.
 * @param [in] lParam The optional parameter for the command.
 * @return TRUE if the message was handled, FALSE otherwise.
 */
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
