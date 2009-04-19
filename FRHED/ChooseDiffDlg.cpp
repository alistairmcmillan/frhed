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
 * @file  ChooseDiffDlg.cpp
 *
 * @brief Implementation of the difference choosing dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "resource.h"
#include "clipboard.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "StringTable.h"
#include "LangString.h"

void ChooseDiffDlg::add_diff(HWND hwndList, int diff, int lower, int upper)
{
	TCHAR buf[100];
	_stprintf(buf,
		// "%d) 0x%x=%u to 0x%x=%u (%d bytes)"
		GetLangString(IDS_DIFFLISTITEMFORMAT),
		diff, lower, lower,	upper, upper, upper - lower + 1);
	SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)buf);
}

//-------------------------------------------------------------------
// Transfer offsets of differences to pdiff.
int ChooseDiffDlg::get_diffs(HWND hwndList, TCHAR *ps, int sl, TCHAR *pd, int dl)
{
	int lower, upper;
	int i = 0, diff = 0, type = 1;
	// type=0 means differences, type=1 means equality at last char.
	while (i < sl && i < dl)
	{
		switch (type)
		{
		case 0:
			// Working on area of difference at the moment.
			if (ps[i] == pd[i])
			{
				// Chars equal again.
				add_diff(hwndList, ++diff, lower, upper);
				type = 1;
			}
			// else: chars still different.
			break;

		case 1:
			// Working on area of equality at the moment.
			if (ps[i] != pd[i])
			{
				// Start of area of difference found.
				lower = i; // Save start of area of difference.
				type = 0;
			}
			// else: chars still equal.
			break;
		}
		upper = i++;
	}
	if (type == 0) // If area of difference was at end of file.
	{
		add_diff(hwndList, ++diff, lower, upper);
	}
	return diff;
}

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to dialog.
 * @return TRUE
 */
BOOL ChooseDiffDlg::OnInitDialog(HWND hDlg)
{
	TCHAR szFileName[_MAX_PATH];
	szFileName[0] = '\0';

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof ofn);
	ofn.lStructSize = sizeof ofn;
	ofn.hwndOwner = hDlg;
	ofn.lpstrFilter = GetLangString(IDS_OPEN_ALL_FILES);
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrTitle = GetLangString(IDS_DIFF_CHOOSEFILE);
	if (!GetOpenFileName(&ofn))
		return FALSE;
	int filehandle = _topen(szFileName, _O_RDONLY|_O_BINARY);
	if (filehandle == -1)
	{
		LangString errOpen(IDS_DIFF_ERROPEN);
		MessageBox(hDlg, errOpen, MB_ICONERROR);
		return FALSE;
	}
	BOOL bDone = FALSE;
	if (int filelen = _filelength(filehandle))
	{
		int iDestFileLen = filelen;
		int iSrcFileLen = DataArray.GetLength() - iCurByte;
		if (TCHAR *cmpdata = new TCHAR[filelen])
		{
			// Read data.
			if (_read(filehandle, cmpdata, filelen) != -1)
			{
				HWND hwndList = GetDlgItem(hDlg, IDC_CHOOSEDIFF_DIFFLIST);
				if (int diff = get_diffs(hwndList, (TCHAR *)&DataArray[iCurByte], DataArray.GetLength() - iCurByte, cmpdata, filelen))
				{
					TCHAR buf[100];
					LangString diffsFound(IDS_DIFF_AREAS_FOUND);
					_stprintf(buf, diffsFound, diff);
					SetDlgItemText(hDlg, IDC_NUMDIFFS, buf);
					LangString remainingData(IDS_DIFF_REMAINING_BYTES);
					_stprintf(buf, remainingData, iSrcFileLen, iDestFileLen);
					SetDlgItemText(hDlg, IDC_CHOOSEDIFF_FSIZES, buf);
					SendMessage(hwndList, LB_SETCURSEL, 0, 0);
					bDone = TRUE;
				}
				else
				{
					// No difference.
					LangString noDiff(IDS_DIFF_NO_DIFF);
					MessageBox(hDlg, noDiff, MB_ICONINFORMATION);
				}
			}
			else
			{
				LangString readErr(IDS_ERR_READING_FILE);
				MessageBox(hDlg, readErr, MB_ICONERROR);
			}
			delete[] cmpdata;
		}
		else
		{
			LangString noMem(IDS_NO_MEMORY);
			MessageBox(hDlg, noMem, MB_ICONERROR);
		}
	}
	_close(filehandle);
	return bDone;
}

/**
 * @brief Handle dialog commands.
 * @param [in] hDlg Handle to dialog.
 * @param [in] wParam First param (depends on command).
 * @return TRUE if command (message) was handled, FALSE if not.
 */
BOOL ChooseDiffDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM)
{
	switch (wParam)
	{
	// By pabs.
	case IDCOPY:
		{//copy button was pressed
			if (!OpenClipboard(hwnd)) //open clip
			{
				LangString errClipboard(IDS_CANNOT_ACCESS_CLIPBOARD);
				MessageBox(hwnd, errClipboard, MB_ICONERROR);
				return TRUE;
			}
			EmptyClipboard(); //empty clip
			IStream *piStream = 0;
			if (SUCCEEDED(CreateStreamOnHGlobal(0, FALSE, &piStream)))
			{
				HWND hwndList = GetDlgItem(hDlg, IDC_CHOOSEDIFF_DIFFLIST);//get the list
				CLIPFORMAT cf = IsWindowUnicode(hwndList) ? CF_UNICODETEXT : CF_TEXT;
				int num = SendMessage(hwndList, LB_GETCOUNT, 0, 0);//get the # items in the list
				for (int i = 0 ; i < num ; i++)
				{
					//add '\r\n' to the end of each line - this is '\r\n' rather than '\n' so that it can be pasted into notepad & dos programs
					if (cf == CF_UNICODETEXT)
					{
						wchar_t buf[100];
						int cch = SendMessageW(hwndList, LB_GETTEXT, i, (LPARAM)buf);
						piStream->Write(buf, cch * sizeof *buf, 0);
						static const wchar_t eol[] = L"\r\n";
						piStream->Write(eol, i < num ? sizeof eol - sizeof *eol : sizeof eol, 0);
					}
					else
					{
						char buf[100];
						int cch = SendMessageA(hwndList, LB_GETTEXT, i, (LPARAM)buf);
						piStream->Write(buf, cch * sizeof *buf, 0);
						static const char eol[] = "\r\n";
						piStream->Write(eol, i < num ? sizeof eol - sizeof *eol : sizeof eol, 0);
					}
				}
				HGLOBAL hMem = 0;
				if (SUCCEEDED(GetHGlobalFromStream(piStream, &hMem)))
				{
					SetClipboardData(cf, hMem); //copy to clip
				}
				piStream->Release();
			}
			CloseClipboard(); //close clip
		}
		return TRUE;

	case IDOK:
		{
			HWND hwndList = GetDlgItem(hDlg, IDC_CHOOSEDIFF_DIFFLIST);
			int i = SendMessage(hwndList, LB_GETCURSEL, 0, 0);
			if (i != -1)
			{
				TCHAR buf[100];
				SendMessage(hwndList, LB_GETTEXT, i, (LPARAM)buf);
				i = _stscanf(buf,
					// "%d) 0x%x=%u to 0x%x=%u (%d bytes)"
					GetLangString(IDS_DIFFLISTITEMFORMAT),
					&i, &i, &iStartOfSelection, &i, &iEndOfSelection, &i);
				iStartOfSelection += iCurByte;
				iEndOfSelection += iCurByte;
				iCurByte = iStartOfSelection;
				bSelected = TRUE;
				adjust_view_for_selection();
				repaint();
			}
		}
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
INT_PTR ChooseDiffDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		if (!OnInitDialog(hDlg))
			EndDialog(hDlg, IDCANCEL);
		return TRUE;

	case WM_COMMAND:
		return OnCommand(hDlg, wParam, lParam);

	case WM_HELP:
		OnHelp(hDlg);
		break;
	}
	return FALSE;
}
