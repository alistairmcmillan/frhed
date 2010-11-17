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
 * @file  AboutDlg.cpp
 *
 * @brief Implementation of the About dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "resource.h"
#include "Constants.h"
#include "paths.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "LangString.h"

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 */
BOOL AboutDlg::OnInitDialog(HWND hDlg)
{
	// Set the version information.
	TCHAR buf[4096];
	buf[RTL_NUMBER_OF(buf) - 1] = _T('\0');
	_sntprintf(buf, RTL_NUMBER_OF(buf) - 1, GetLangString(IDS_ABOUTFRHEDVER),
		FRHED_MAJOR_VERSION, FRHED_MINOR_VERSION, FRHED_SUB_RELEASE_NO);
#if defined(_DEBUG)
	_tcscat(buf, _T(" (Unicode Debug)"));
#endif
	SetDlgItemText(hDlg, IDC_ABOUT_VER, buf);
	// Set the homepage URL.
	SetDlgItemText(hDlg, IDC_ABOUT_URL, FrhedHomepageURL);
	// Set the icon.
	if (HWND hwndParent = GetParent(hDlg))
		if (DWORD dwIcon = GetClassLong(hwndParent, GCLP_HICON))
			SendDlgItemMessage(hDlg, IDC_APPICON, STM_SETICON, dwIcon, 0);
	return TRUE;
}

/**
 * @brief Handle dialog commands.
 * @param [in] hDlg Hanle to the dialog.
 * @param [in] wParam The command to handle.
 * @param [in] lParam Optional parameter for the command.
 * @return TRUE if the command was handled, FALSE otherwise.
 */
BOOL AboutDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case IDOK:
	case IDCANCEL:
		EndDialog(hDlg, wParam);
		return TRUE;

	case IDC_ABOUT_OPENURL:
		{
			HINSTANCE hi = ShellExecute(hDlg, _T("open"), FrhedHomepageURL,
					0, NULL, SW_SHOWNORMAL);
			if ((UINT)hi <= HINSTANCE_ERROR)
			{
				LangString app(IDS_APPNAME);
				LangString browserErr(IDS_ABOUT_BROWSER_ERR);
				MessageBox(hDlg, browserErr, app, MB_ICONERROR);
			}
		}
		return TRUE;

	case IDC_ABOUTCONTRIBS:
		{
			TCHAR contrList[MAX_PATH] = {0};
			paths_GetFullPath(ContributorsList, contrList,
					RTL_NUMBER_OF(contrList));
			if (!paths_DoesFileExist(contrList))
			{
				LangString app(IDS_APPNAME);
				LangString fileNotFound(IDS_ABOUT_FILENOTFOUND);
				TCHAR buf[4096] = {0};
				_sntprintf(buf, RTL_NUMBER_OF(buf) - 1, fileNotFound,
						ContributorsList);
				MessageBox(hDlg, buf, app, MB_ICONERROR);
			}
			else
				paths_OpenFile(hDlg, ContributorsList);
		}
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
INT_PTR AboutDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
