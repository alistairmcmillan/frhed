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
 * @file  DragDropDlg.cpp
 *
 * @brief Implementation of the Drag andDrop-dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "DragDropDlg.h"
#include "LangString.h"

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 */
BOOL DragDropDlg::OnInitDialog(HWND hDlg)
{
	CheckDlgButton(hDlg, effect ? IDC_COPY : IDC_MOVE, BST_CHECKED);
	if (!(allowable_effects & DROPEFFECT_MOVE))
		EnableWindow(GetDlgItem(hDlg, IDC_MOVE), FALSE);
	if (!(allowable_effects & DROPEFFECT_COPY))
		EnableWindow(GetDlgItem(hDlg, IDC_COPY), FALSE);
	HWND list = GetDlgItem(hDlg, IDC_DAND_LIST);
	if (numformatetcs && formatetcs)
	{
		LVCOLUMN col;
		ZeroMemory(&col, sizeof col);
		ListView_InsertColumn(list, 0, &col);
		TCHAR szFormatName[100];
		UINT i;
		for (i = 0 ; i < numformatetcs ; i++)
		{
			CLIPFORMAT temp = formatetcs[i].cfFormat;
			LVITEM lvi;
			lvi.lParam = lvi.iItem = i;
			lvi.iSubItem = 0;
			lvi.pszText = szFormatName;
			// For registered formats, get the registered name.
			if (!GetClipboardFormatName(temp, szFormatName, sizeof szFormatName))
			{
				//Get the name of the standard clipboard format.
				switch (temp)
				{
					#define CASE(a) case a: lvi.pszText = _T(#a); break;
						CASE(+CF_TEXT)
						CASE(CF_BITMAP) CASE(CF_METAFILEPICT) CASE(CF_SYLK)
						CASE(CF_DIF) CASE(CF_TIFF) CASE(CF_OEMTEXT)
						CASE(CF_DIB) CASE(CF_PALETTE) CASE(CF_PENDATA)
						CASE(CF_RIFF) CASE(CF_WAVE) CASE(CF_UNICODETEXT)
						CASE(CF_ENHMETAFILE) CASE(CF_HDROP) CASE(CF_LOCALE)
						CASE(CF_OWNERDISPLAY) CASE(CF_DSPTEXT)
						CASE(CF_DSPBITMAP) CASE(CF_DSPMETAFILEPICT)
						CASE(CF_DSPENHMETAFILE) CASE(CF_PRIVATEFIRST)
						CASE(CF_PRIVATELAST) CASE(CF_GDIOBJFIRST)
						CASE(CF_GDIOBJLAST) CASE(CF_DIBV5)
					#undef CASE
					default:
						if (temp >= CF_PRIVATEFIRST && temp <= CF_PRIVATELAST)
						{
							_stprintf(szFormatName, _T("CF_PRIVATE_%d"), temp - CF_PRIVATEFIRST);
						}
						else if (temp >= CF_GDIOBJFIRST && temp <= CF_GDIOBJLAST)
						{
							_stprintf(szFormatName, _T("CF_GDIOBJ_%d"), temp - CF_GDIOBJFIRST);
						}
						//Format ideas for future: hex number, system/msdn constant, registered format, WM_ASKFORMATNAME, tickbox for delay rendered or not*/
						/*else if(temp>0xC000&&temp<0xFFFF)
						{
							sprintf(szFormatName,"CF_REGISTERED%d",temp-0xC000);
						}*/
						else
						{
							_stprintf(szFormatName, _T("0x%.8x"), temp);
						}
					break;
				}
			}
			//Insert into the list
			lvi.mask = LVIF_TEXT | LVIF_PARAM;
			if (*lvi.pszText == '+')
			{
				++lvi.pszText;
				lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;
				lvi.state = lvi.stateMask = LVIS_SELECTED;
			}
			ListView_InsertItem(list, &lvi);
		}
		ListView_SetColumnWidth(list, 0, LVSCW_AUTOSIZE_USEHEADER);
	}
	return TRUE;
}

/**
 * @brief Handle dialog commands.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] wParam The command to handle.
 * @param [in] lParam Optional parameter for the command.
 * @return TRUE if the command was handled, FALSE otherwise.
 * @todo Update such that when the IDataObject changes the list box is re-created.
 */
BOOL DragDropDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDOK:
		{
			effect = !IsDlgButtonChecked(hDlg, IDC_MOVE);
			HWND list = GetDlgItem(hDlg, IDC_DAND_LIST);
			numformats = ListView_GetSelectedCount(list);
			if (numformats)
			{
				formats = (UINT*)malloc(numformats * sizeof *formats);
				if (formats == 0)
				{
					LangString msg(IDS_NO_MEMORY);
					LangString app(IDS_APPNAME);
					MessageBox(hDlg, msg, app, MB_ICONERROR);
					return TRUE;
				}
				LVITEM temp;
				ZeroMemory(&temp, sizeof temp);
				temp.mask = LVIF_PARAM;
				temp.iItem = -1;
				for (UINT i = 0 ; i < numformats ; i++)
				{
					temp.iItem = ListView_GetNextItem(list, temp.iItem, LVNI_SELECTED);
					ListView_GetItem(list, &temp);
					formats[i] = temp.lParam;
				}
			}
			EndDialog (hDlg, 1);
		}
		return TRUE;

	case IDCANCEL:
		EndDialog(hDlg, -1);
		return TRUE;

	case IDC_UP:
	case IDC_DOWN:
		{
			HWND list = GetDlgItem(hDlg, IDC_DAND_LIST);
			LVITEM item[2];
			ZeroMemory(item, sizeof item);
			//If anyone knows a better way to swap two items please send a patch
			item[0].iItem = ListView_GetNextItem(list, (UINT)-1, LVNI_SELECTED);
			if (item[0].iItem==-1)
				item[0].iItem = ListView_GetNextItem(list, (UINT)-1, LVNI_FOCUSED);
			if (item[0].iItem==-1)
			{
				LangString app(IDS_APPNAME);
				LangString moveItem(IDS_DD_SELECT_MOVE_ITEM);
				MessageBox(hDlg, moveItem, app, MB_OK);
				SetFocus(list);
				return TRUE;
			}
			item[0].mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;
			item[0].stateMask = (UINT)-1;
			TCHAR text[2][100];
			item[0].pszText = text[0];
			item[0].cchTextMax = RTL_NUMBER_OF(text[0]);
			item[1] = item[0];
			item[1].pszText = text[1];
			if (LOWORD(wParam) == IDC_UP)
			{
				if (item[1].iItem == 0)
					item[1].iItem=numformatetcs - 1;
				else
					item[1].iItem--;
			}
			else
			{
				if ((UINT)item[1].iItem == numformatetcs - 1)
					item[1].iItem = 0;
				else
					item[1].iItem++;
			}
			ListView_GetItem(list, &item[0]);
			ListView_GetItem(list, &item[1]);
			swap(item[0].iItem, item[1].iItem);
			item[0].state |= LVIS_FOCUSED | LVIS_SELECTED;
			item[1].state &= ~(LVIS_FOCUSED | LVIS_SELECTED);
			ListView_SetItem(list, &item[0]);
			ListView_SetItem(list, &item[1]);
			SetFocus(list);
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
INT_PTR DragDropDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hDlg);

	case WM_COMMAND:
		return OnCommand(hDlg, wParam, lParam);

	case WM_HELP:
		HexEditorWindow::OnHelp(hDlg);
		break;
	}
	return FALSE;
}
