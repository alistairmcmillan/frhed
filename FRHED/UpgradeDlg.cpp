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
 * @file  UpgradeDlg.cpp
 *
 * @brief Implementation of the Settings upgrading (from previous version) dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "resource.h"
#include "Constants.h"
#include "regtools.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "StringTable.h"
#include "LangString.h"

#define nibble2hex(c) ( (c) < 10 ? (c) + '0': (c) - 10 + 'a' )
#define NIBBLE2HEX(c) ( (c) < 10 ? (c) + '0': (c) - 10 + 'A' )

struct DispDataStruct
{
	int iTextColorValue; //Color values
	int iBkColorValue;
	int iSepColorValue;
	int iSelTextColorValue;
	int iSelBkColorValue;
	int iBmkColor; 
	int iBytesPerLine;
	int iOffsetLen;
	int iFontSize;
	int iAutomaticBPL;
	int bAutoOffsetLen;
	int bOpenReadOnly;
	int bMakeBackups;
	int iWindowShowCmd;
	int iCharacterSet;
} DispData;

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 */
BOOL UpgradeDlg::OnInitDialog(HWND hDlg)
{
	int i;
	HKEY hk;
	TCHAR subkeynam[MAX_PATH + 1] = {0};
	LVITEM item;
	ZeroMemory(&item, sizeof(LVITEM));
	item.mask = LVIF_TEXT ;
	item.pszText = subkeynam;

	static const DWORD exstyle = LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP;
	HWND list = GetDlgItem(hDlg, IDC_VERS);
	ListView_DeleteColumn(list, 0);
	ListView_DeleteAllItems(list);
	ListView_SetExtendedListViewStyle(list, exstyle);

	//Add a column
	LVCOLUMN col;
	ZeroMemory(&col, sizeof(col));
	col.mask = LVCF_TEXT | LVCF_WIDTH ;
	col.fmt = LVCFMT_LEFT;
	col.pszText = _T("HKCU\\Software\\Frhed");
	col.cx = 165;
	ListView_InsertColumn(list, 0, &col);

	//Fill the vers list with the various versions
	if (RegOpenKeyEx(HKEY_CURRENT_USER, OptionsRegistryPath, 0, KEY_ALL_ACCESS, &hk) == 0)
	{
		i = 0;
		while (RegEnumKey(hk, i, subkeynam, MAX_PATH + 1) != ERROR_NO_MORE_ITEMS)
		{
			item.iItem = i++;
			ListView_InsertItem(list, &item);
		}
		RegCloseKey(hk);
	}
	list = GetDlgItem(hDlg, IDC_INSTS);
	ListView_DeleteColumn(list, 0);
	ListView_DeleteAllItems(list);
	ListView_SetExtendedListViewStyle(list, exstyle);

	list = GetDlgItem(hDlg, IDC_INSTDATA);
	ListView_DeleteColumn(list, 1);
	ListView_DeleteColumn(list, 0);
	ListView_DeleteAllItems(list);
	ZeroMemory(&col, sizeof(col));
	col.mask = LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.cx = 105;
	col.pszText = GetLangString(IDS_UPDLG_OPTION);
	ListView_InsertColumn(list, 0, &col);
	ZeroMemory(&col, sizeof(col));
	col.mask = LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.cx = 105;
	col.pszText = GetLangString(IDS_UPDLG_VALUE);
	ListView_InsertColumn(list, 1, &col);

	list = GetDlgItem(hDlg, IDC_LINKS);
	ListView_DeleteColumn(list, 0);
	ListView_DeleteAllItems(list);
	ZeroMemory(&col, sizeof(col));
	col.mask = LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.cx = 155;
	col.pszText = GetLangString(IDS_UPDLG_LINKS);
	ListView_InsertColumn(list, 0, &col);

	list = GetDlgItem(hDlg, IDC_MRU);
	ListView_DeleteColumn(list, 0);
	ListView_DeleteAllItems(list);
	ZeroMemory(&col,sizeof(col));
	col.mask = LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.cx = 185;
	col.pszText = GetLangString(IDS_UPDLG_MRUFILES);
	ListView_InsertColumn(list, 0, &col);
	HICON hIcon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_FRHED));
	SendDlgItemMessage(hDlg, IDC_DISPLAY, WM_SETICON, 1, (LPARAM)hIcon);
	return TRUE;
}

/**
 * @brief Handle dialog commands.
 * @param [in] hDlg Hanle to the dialog.
 * @param [in] wParam The command to handle.
 * @param [in] lParam Optional parameter for the command.
 * @return TRUE if the command was handled, FALSE otherwise.
 */
BOOL UpgradeDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	HWND insts = GetDlgItem(hDlg, IDC_INSTS);
	TCHAR keynam[MAX_PATH];
	_tcsncpy(keynam, OptionsRegistryPath, RTL_NUMBER_OF(keynam));
	LVCOLUMN col;
	switch (wParam)
	{
	case IDCANCEL:
		EndDialog(hDlg, 0);
		return TRUE;
	case IDC_COPY:
		{
			HKEY lk;
			HWND vers = GetDlgItem(hDlg, IDC_VERS);
			int i = ListView_GetNextItem(vers, (UINT)-1, LVNI_SELECTED);
			ListView_GetItemText(vers, i, 0, &keynam[15], MAX_PATH - 15);
			if (!_tcscmp(&keynam[15], OptionsRegistrySettingsPath))
			{
				//If that key was this version don't copy
				LangString msg(IDS_UPDLG_CANT_COPY_CURR);
				MessageBox(hDlg, msg, MB_OK);
				return 0;
			}

			//Open the reg key to load from
			if (ERROR_SUCCESS == RegOpenKey(HKEY_CURRENT_USER, keynam, &lk))
			{
				TCHAR cver[_MAX_PATH+1];
				_tcsncpy(cver, OptionsRegistryPath, RTL_NUMBER_OF(cver));
				int i;
				int numi = ListView_GetItemCount(insts);
				int len;
				int lenc = _tcslen(cver);
				_tcsncat(keynam, _T("\\"), RTL_NUMBER_OF(keynam) - _tcslen(keynam));
				for (i = 0; i < numi; i++)
				{
					if (ListView_GetCheckState(insts, i))
					{
						len = _tcslen(keynam);
						ListView_GetItemText(insts, i, 0, &keynam[len], MAX_PATH + 1 - len);//get the inst
						_tcscpy(&cver[lenc], &keynam[len]);
						RegCopyValues(HKEY_CURRENT_USER, keynam, HKEY_CURRENT_USER, cver);//copy the key
						keynam[len] = cver[lenc] = 0;
					}//if cur inst checked
				}//loop insts
				RegCloseKey(lk);
				SendMessage(hDlg, WM_INITDIALOG, 0, 0);//Readd all the instances
			}
			else
			{
				LangString msg(IDS_CANT_OPEN_VER);
				MessageBox(hDlg, msg, MB_OK);
			}
		}
		return 0;
	case IDC_READ:
		{
			//Get the instance
			int i = ListView_GetNextItem(insts, (UINT)-1, LVNI_SELECTED);
			TCHAR text[MAX_PATH] = {0};
			ListView_GetItemText(insts, i, 0, text, MAX_PATH);
			//Get the version
			ZeroMemory(&col, sizeof(col));
			col.mask = LVCF_TEXT;
			col.pszText = keynam;
			col.cchTextMax = _MAX_PATH;
			ListView_GetColumn(insts, 0, &col);
			//Save the current instance
			int tmp = iInstCount;
			//Set the instance to read from
			iInstCount = _ttoi(text);
			//Read the data
			read_ini_data(keynam);
			//Reset the instance
			iInstCount = tmp;
			resize_window();
		}
		break;
	case IDC_DELETE:
		{
			HWND vers = GetDlgItem(hDlg, IDC_VERS);
			int v;
			int i;
			int numv = ListView_GetItemCount(vers);
			int numi = ListView_GetItemCount(insts);
			int len;
			for (v = 0; v < numv; v++)
			{
				if (ListView_GetCheckState(vers, v))
				{
					for (i = 0; i < numi; i++)
					{
						if (ListView_GetCheckState(insts, i))
						{
							ListView_GetItemText(vers, v, 0, &keynam[15], MAX_PATH + 1 - 15);//get the ver
							_tcsncat(keynam, _T("\\"), RTL_NUMBER_OF(keynam) - _tcslen(keynam));
							len = _tcslen(keynam);
							ListView_GetItemText(insts, i, 0, &keynam[len], MAX_PATH + 1 - len);//get the inst
							RegDeleteKey(HKEY_CURRENT_USER, keynam);//delete the key
							keynam[len - 1] = 0;//cut off the "\\<inst>"
							SHDeleteEmptyKey(HKEY_CURRENT_USER, keynam);//Delete an empty key
							if (_tcscmp(&keynam[15], OptionsRegistrySettingsPath) == 0)
								bSaveIni = 0;//If that key was this version don't save
						}//if cur inst checked
					}//loop insts
				}//if cur ver checked
			}//loop vers
			SendMessage(hDlg, WM_INITDIALOG, 0, 0);//Readd all the instances
		}
		break;
	}//switch ctrl id
	return TRUE;
}

BOOL UpgradeDlg::OnNotify(HWND hw, WPARAM w, LPARAM l)
{
	NMHDR *nmh = (NMHDR *)l;
	if (nmh->code == LVN_ITEMCHANGED)
	{
		NMLISTVIEW *nml = (NMLISTVIEW*)l;
		if (nml->uChanged == LVIF_STATE &&
			(nml->uNewState & LVIS_FOCUSED) > (nml->uOldState & LVIS_FOCUSED))
		{
			TCHAR text[_MAX_PATH+1];
			switch (nmh->idFrom)
			{
			case IDC_VERS:
				ListView_GetItemText(nmh->hwndFrom, nml->iItem, 0, text, _MAX_PATH + 1);
				ChangeSelVer(hw, text);
				break;
			case IDC_INSTS:
				ListView_GetItemText(nmh->hwndFrom, nml->iItem, 0, text, _MAX_PATH + 1);
				ChangeSelInst(hw, text);
				break;
			}
		}
	}
	return TRUE;
}

BOOL UpgradeDlg::OnDrawitem(HWND, WPARAM, LPARAM l)
{
	DRAWITEMSTRUCT *pdis = (DRAWITEMSTRUCT *)l;
	HWND hw = pdis->hwndItem;
	HDC dc = pdis->hDC;
	RECT rt;
	GetClientRect(hw, &rt);
	FillRect(dc, &rt, GetSysColorBrush(COLOR_BTNFACE));
	if (GetWindowTextLength(hw))
	{
		rt.bottom = rt.top + 18;
		DrawCaption(hw, dc, &rt, DC_ACTIVE|DC_ICON|DC_TEXT);
		rt.left = 150; rt.right = rt.left + 16; rt.bottom--; rt.top++;
		UINT type;
		switch (DispData.iWindowShowCmd)
		{
		case SW_HIDE:
		case SW_MINIMIZE:
		case SW_SHOWMINIMIZED:
		case SW_SHOWMINNOACTIVE:
			type = DFCS_CAPTIONMIN;
			break;
		case SW_RESTORE:
		case SW_SHOWNORMAL:
		case SW_SHOW:
		case SW_SHOWNA:
		case SW_SHOWNOACTIVATE:
			type = DFCS_CAPTIONRESTORE;
			break;
		case SW_SHOWMAXIMIZED:
			type = DFCS_CAPTIONMAX;
			break;
		}
		DrawFrameControl(dc, &rt, DFC_CAPTION, type);

	//-------------Draw the status bar-----------------------------------------
		GetClientRect(hw, &rt);
		rt.top = rt.bottom - 18;
		DrawEdge (dc, &rt, BDR_SUNKENOUTER, BF_RECT);
		HFONT fon = (HFONT) SendMessage(GetParent(hw), WM_GETFONT, 0, 0);
		HFONT ofon = (HFONT) SelectObject(dc, fon);
		TCHAR statusbuf[]=  _T("ANSI / READ");
		int i = 0;
		int len = 11;
		if (DispData.iCharacterSet != ANSI_FIXED_FONT)
		{
			statusbuf[1] = 'O';
			statusbuf[2] = 'E';
			statusbuf[3] = 'M';
			i++;
			len--;
		}
		if (!DispData.bOpenReadOnly)
		{
			statusbuf[7] = 0;
			_tcscat(statusbuf, _T("OVR"));
			len--;
		}
		SIZE s;
		GetTextExtentPoint32(dc, &statusbuf[i], len, &s);
		int mode = SetBkMode(dc, TRANSPARENT);
		UINT align = SetTextAlign(dc, TA_CENTER);
		TextOut(dc, (rt.left + rt.right) / 2, (rt.top + rt.bottom - s.cy) / 2, &statusbuf[i], len);
		SetTextAlign(dc, align);
		SetBkMode(dc, mode);
		SelectObject(dc, ofon);

	//-------------Draw the border---------------------------------------------
		GetClientRect(hw, &rt);
		rt.top += 19;
		rt.bottom -= 20;
		DrawEdge(dc, &rt, EDGE_SUNKEN, BF_RECT);

	//-------------Draw hex contents-------------------------------------------
		//Print 1 row unselected, 1 row selected, 2 bookmarks & separators
		rt.left += 2;
		rt.top += 2;
		rt.right -= 2;
		rt.bottom -= 2;
		//Create the font & stick in the DC
		int nHeight = -MulDiv(DispData.iFontSize, GetDeviceCaps(dc, LOGPIXELSY), 72);
		int cset = (DispData.iCharacterSet == ANSI_FIXED_FONT ? ANSI_CHARSET : OEM_CHARSET);
		fon = CreateFont(nHeight, 0, 0, 0, 0, 0, 0, 0, cset, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE, 0);
		ofon = (HFONT) SelectObject(dc, fon);

		//Set the text & back colours for non-selected
		SetBkColor(dc, DispData.iBkColorValue);
		SetTextColor(dc, DispData.iTextColorValue);

		//Create the text
		int p;
		TEXTMETRIC tm;
		GetTextMetrics(dc, &tm);
		int tmp = DispData.iBytesPerLine;
		if (DispData.iAutomaticBPL)
		{
			//Get the number of chars that will fit into the box
			tmp = len = (rt.right - rt.left) / tm.tmAveCharWidth;
			len++;//'\0'
			//Get the number of chars to print
			tmp -= DispData.iOffsetLen + 3;//3 spaces
			tmp /= 4;//"ff " & "�" = 4 chars so numchars = leftovers / 4
		}
		//length of last offset
		//Offset len, 3 spaces & '\0', numchars * 4 (2 hex 1 space & character) - \0 not printed
		int mol = 1;
		while (tmp & ~0 << 4 * mol)
			++mol;
		if (mol < DispData.iOffsetLen)
			mol = DispData.iOffsetLen;
		len = mol + 4 + 4 * tmp;
		TCHAR* linebuf = new TCHAR[len];
		if (!linebuf)
			return 0;
		//Offset & 2 spaces
		int tol = DispData.bAutoOffsetLen ? mol : DispData.iOffsetLen;
		_stprintf(linebuf, _T("%*.*x"), tol, tol, 0); // The fix caused a crash
		p = _tcslen(linebuf);
		mol += 2;
		for (i = p; i < mol - p; i++)
			linebuf[i] = ' ';
		linebuf[mol] = 0;
		p = _tcslen(linebuf);
		//numchars
		for (i = 0 ; i < tmp ; i++)
		{
			int ii = (i >> 4) & 0x0f;
			linebuf[p++] = (TCHAR)nibble2hex(ii);
			ii = i & 0x0f;
			linebuf[p++] = (TCHAR)nibble2hex(ii);
			linebuf[p++] = ' ';
		}
		linebuf[p++] = ' ';
		for (i = 0 ; i < tmp ; i++)
		{
			linebuf[p++] =
			(
				DispData.iCharacterSet == OEM_FIXED_FONT && i != 0 ||
				(i >= 32 && i <= 126 || i >= 160 && i <= 255 || i >= 145 && i <= 146)
			) ? (TCHAR)i : '.';
		}
		linebuf[p] = 0;

		SetBkMode(dc, OPAQUE);

		//Draw the non-selected text
		ExtTextOut(dc, rt.left, rt.top, ETO_CLIPPED | ETO_OPAQUE, &rt, linebuf, p, NULL);

		rt.top -= nHeight;

		//Set the text & back colours for selected text
		SetBkColor(dc, DispData.iSelBkColorValue);
		SetTextColor(dc, DispData.iSelTextColorValue);

		//Create the text
		_stprintf(linebuf, _T("%*.*x"), tol, tol, tmp); // The fix caused a crash
		p = _tcslen(linebuf);
		for (i = p; i < mol -p; i++)
			linebuf[i] = ' ';
		linebuf[mol] = 0;
		mol -= 2;
		p = _tcslen(linebuf);
		//numchars
		for (i = 0 ; i < tmp - 1 ; i++)
		{
			int ii = ((tmp + i) >> 4) & 0x0f;
			linebuf[p++] = (TCHAR)nibble2hex(ii);
			ii = (tmp+i) & 0x0f;
			linebuf[p++] = (TCHAR)nibble2hex(ii);
			linebuf[p++] = ' ';
		}
		linebuf[p++] = '_';
		linebuf[p++] = '_';
		linebuf[p++] = ' ';
		linebuf[p++] = ' ';
		for (i = 0 ; i < tmp - 1 ; i++)
		{
			int ii = tmp + i;
			linebuf[p++] =
			(
				DispData.iCharacterSet == OEM_FIXED_FONT && ii != 0 ||
				(ii >= 32 && ii <= 126 || ii >= 160 && ii <= 255 || ii >= 145 && ii <= 146)
			) ? (TCHAR)ii : '.';
		}
		linebuf[p++] = ' ';
		linebuf[p] = 0;

		//Draw the selected text
		ExtTextOut(dc, rt.left, rt.top, ETO_CLIPPED, &rt, linebuf, p, NULL);
		delete[] linebuf;

		//Kill the font
		SelectObject(dc, ofon);
		DeleteObject(fon);

		rt.top += nHeight;

		//Create the separator pen
		HPEN sp = CreatePen(PS_SOLID, 1, DispData.iSepColorValue);
		HPEN op = (HPEN) SelectObject (dc, sp);

		//Draw the separators
		int m;
		for (i = 0; i < (tmp / 4) + 1; i++)
		{
			m = (mol + 2) * tm.tmAveCharWidth - tm.tmAveCharWidth / 2 + 3 * tm.tmAveCharWidth * 4 * i;
			MoveToEx(dc, m, rt.top, NULL);
			LineTo(dc, m, rt.top - nHeight * 2);
		}
		// Separator for chars.
		m = tm.tmAveCharWidth * (mol + 3 + tmp * 3) - 2;
		MoveToEx(dc, m, rt.top, NULL);
		LineTo(dc, m, rt.top - nHeight * 2);
		// Second separator.
		MoveToEx(dc, m + 2, rt.top, NULL);
		LineTo(dc, m + 2, rt.top - nHeight * 2);

		//Kill the separator pen
		SelectObject(dc, op);
		DeleteObject(sp);

		rt.bottom = rt.top - nHeight;

		//Create a brush for bookmarks
		HBRUSH bb = CreateSolidBrush(DispData.iBmkColor);
		// Mark hex.
		rt.left += tm.tmAveCharWidth * (mol + 2);
		rt.right = rt.left + 2 * tm.tmAveCharWidth + 1;
		FrameRect(dc, &rt, bb);
		// Mark char.
		rt.left += tm.tmAveCharWidth*(tmp * 3 + 1);
		rt.right = rt.left + tm.tmAveCharWidth + 1;
		FrameRect(dc, &rt, bb);

		//Kill the brush
		DeleteObject(bb);
	}
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
INT_PTR UpgradeDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch(iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hDlg);
	case WM_COMMAND:
		return OnCommand(hDlg, wParam, lParam);
	case WM_NOTIFY:
		return OnNotify(hDlg, wParam, lParam);
	case WM_DRAWITEM:
		return OnDrawitem(hDlg, wParam, lParam);

	case WM_HELP:
		OnHelp(hDlg);
		break;
	}
	return FALSE;
}

//Delete all items from all lists
//init the insts list
void UpgradeDlg::ChangeSelVer(HWND hw, TCHAR* text)
{
	HWND insts = GetDlgItem(hw, IDC_INSTS);
	HWND instdata = GetDlgItem(hw, IDC_INSTDATA);
	HWND links = GetDlgItem(hw, IDC_LINKS);
	HWND mru = GetDlgItem(hw, IDC_MRU);

	ListView_DeleteAllItems(insts);
	ListView_DeleteColumn(insts, 0);
	ListView_DeleteAllItems(instdata);
	ListView_DeleteAllItems(links);
	ListView_DeleteAllItems(mru);

	//Init the version number on the insts list header
	LVCOLUMN col;
	ZeroMemory(&col, sizeof col);
	col.mask = LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.pszText = text;
	col.cx = 120;
	ListView_InsertColumn(insts, 0, &col);

	TCHAR keyname[MAX_PATH];
	TCHAR subkeynam[MAX_PATH];
	PathCombine(keyname, OptionsRegistryPath, text);

	LVITEM item;
	ZeroMemory(&item, sizeof item);
	item.mask = LVIF_TEXT;
	item.pszText = subkeynam;
	HKEY hk;
	LONG res;

	//Fill the instance list with the various instances of the current selected version
	if (0 == RegOpenKeyEx(HKEY_CURRENT_USER, keyname, 0,KEY_ALL_ACCESS, &hk))
	{
		for (int i = 0; ; i++)
		{
			res = RegEnumKey(hk, i, subkeynam, MAX_PATH);
			if (res == ERROR_NO_MORE_ITEMS)
				break;
			else
			{
				int instno = 0;
				if (StrToIntEx(subkeynam, STIF_DEFAULT, &instno))
				{
					item.iItem = i;
					ListView_InsertItem(insts, &item);
				}
			}
		}
		RegCloseKey(hk);
	}

	//Add all the links
	_tcsncat(keyname, _T("\\links"), RTL_NUMBER_OF(keyname) - _tcslen(keyname));
	TCHAR* valnam = subkeynam;
	TCHAR valbuf[_MAX_PATH + 1] = {0};
	DWORD valnamsize;
	DWORD valbufsize;
	DWORD typ;
	item.pszText = valbuf;
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, keyname, 0, KEY_ALL_ACCESS, &hk))
	{
		//Load all the string values
		for (DWORD i = 0; ; i++)
		{
			typ = 0;
			valnamsize = valbufsize = _MAX_PATH+1;
			valbuf[0] = valnam[0] = 0;
			res = RegEnumValue(hk, i, valnam, &valnamsize, 0, &typ,(BYTE*) valbuf, &valbufsize);
			if (typ == REG_SZ && valbuf[0] != 0 )
			{
				//Add the string
				item.iItem = i;
				ListView_InsertItem(links, &item);
			}
			if (ERROR_NO_MORE_ITEMS == res)
				break;
		}
		RegCloseKey(hk);
	}
	SetDlgItemText(hw, IDC_DISPLAY, _T(""));
}

void UpgradeDlg::ChangeSelInst(HWND hw, TCHAR* text)
{
	HWND insts = GetDlgItem(hw, IDC_INSTS);
	HWND instdata = GetDlgItem(hw, IDC_INSTDATA);
	HWND mru = GetDlgItem(hw, IDC_MRU);

	ListView_DeleteAllItems(instdata);
	ListView_DeleteAllItems(mru);

	//Assemble the keyname
	TCHAR keynam[MAX_PATH];
	_tcsncpy(keynam, OptionsRegistryPath, RTL_NUMBER_OF(keynam));
	LVCOLUMN col;
	ZeroMemory(&col, sizeof col);
	col.mask = LVCF_TEXT;
	col.pszText = PathAddBackslash(keynam);
	col.cchTextMax = keynam + MAX_PATH - col.pszText;
	ListView_GetColumn(insts, 0, &col);
	PathAppend(keynam, text);

	HKEY hk;
	if (0 == RegOpenKeyEx(HKEY_CURRENT_USER, keynam, 0, KEY_ALL_ACCESS, &hk))
	{
		//Add all the data
		BYTE databuf[_MAX_PATH + 1] = {0};
		TCHAR szText[_MAX_PATH + 1] = {0};

		int mrucount = 0;

		LVITEM item;
		item.mask = LVIF_TEXT;
		item.iSubItem = 0;

		static struct
		{
			TCHAR *name;	// registry value names
			TCHAR *text;	// names to go in the list box
			int offset;
			} dict[] = {
			// Color values
			{ _T("iTextColorValue"),	_T("Text Color"),		FIELD_OFFSET(DispDataStruct, iTextColorValue)		},
			{ _T("iBkColorValue"),		_T("Back Color"),		FIELD_OFFSET(DispDataStruct, iBkColorValue)			},
			{ _T("iSepColorValue"),		_T("Separator Color"),	FIELD_OFFSET(DispDataStruct, iSepColorValue)		},
			{ _T("iSelTextColorValue"),	_T("Selected Text Color"),	FIELD_OFFSET(DispDataStruct, iSelTextColorValue)	},
			{ _T("iSelBkColorValue"),	_T("Selected Back Color"),	FIELD_OFFSET(DispDataStruct, iSelBkColorValue)		},
			{ _T("iBmkColor"),			_T("Bookmark Color"),	FIELD_OFFSET(DispDataStruct, iBmkColor)				},
			// signed integers
			{ _T("iBytesPerLine"),		_T("Bytes Per Line"),	FIELD_OFFSET(DispDataStruct, iBytesPerLine)			},
			{ _T("iOffsetLen"),			_T("Offset Len"),		FIELD_OFFSET(DispDataStruct, iOffsetLen)			},
			{ _T("iFontSize"),			_T("Font Size"),		FIELD_OFFSET(DispDataStruct, iFontSize)				},
			{ _T("iWindowX"),			_T("Window XPos"),		-1													},
			{ _T("iWindowY"),			_T("Window YPos"),		-1													},
			{ _T("iWindowWidth"),		_T("Window Width"),		-1													},
			{ _T("iWindowHeight"),		_T("Window Height"),	-1													},
			{ _T("iMRU_count"),			_T("# MRU items"),		-1													},
			// Bool
			{ _T("iAutomaticBPL"),		_T("Automatic BPL"),	FIELD_OFFSET(DispDataStruct, iAutomaticBPL)			},
			{ _T("bAutoOffsetLen"),		_T("Auto Offset Len"),	FIELD_OFFSET(DispDataStruct, bAutoOffsetLen)		},
			{ _T("bOpenReadOnly"),		_T("Open Read Only"),	FIELD_OFFSET(DispDataStruct, bOpenReadOnly)			},
			{ _T("bMakeBackups"),		_T("Make backups"),		FIELD_OFFSET(DispDataStruct, bMakeBackups)			},
			// Multiple different values
			{ _T("iWindowShowCmd"),		_T("Window Show Cmd"),	FIELD_OFFSET(DispDataStruct, iWindowShowCmd)		},
			{ _T("iCharacterSet"),		_T("Character Set"),	FIELD_OFFSET(DispDataStruct, iCharacterSet)			},
			// Strings
			{ _T("TexteditorName"),		_T("Text Editor Name"),	-1													},
		};
		int i = 0;
		for ( ; i < RTL_NUMBER_OF(dict) ; i++)
		{
			DWORD typ;
			DWORD datasize = sizeof databuf;
			item.iItem = i;
			item.pszText = (TCHAR *)dict[i].text;
			ListView_InsertItem(instdata, &item);
			item.pszText = (TCHAR *)dict[i].name;
			ZeroMemory(databuf, sizeof databuf);
			RegQueryValueEx(hk, item.pszText, NULL, &typ, databuf, &datasize);
			int offset = dict[item.iItem].offset;
			if (offset >= 0)
				*(int *)((TCHAR *)&DispData + offset) = *(int*)databuf;
			if (i == 13)
				mrucount = *(int*)databuf;
			if (i < 6)
			{
				_sntprintf(szText, RTL_NUMBER_OF(szText) - 1, _T("RGB - %u,%u,%u"),
					(unsigned)databuf[0], (unsigned)databuf[1], (unsigned)databuf[2]);
			}
			else if (i < 6 + 8)
			{
				_sntprintf(szText, RTL_NUMBER_OF(szText) - 1, _T("%u"), *(int*)databuf);
			}
			else if (i < 6 + 8 + 4)
			{
				_tcscpy(szText, databuf[0] ? _T("True") : _T("False"));
			}
			else if (i < 6 + 8 + 4 + 1)
			{
				switch (*(int*)databuf)
				{
					case SW_HIDE:
						_tcscpy(szText, _T("Hide"));
						break;
					case SW_MINIMIZE:
						_tcscpy(szText, _T("Minimize"));
						break;
					case SW_RESTORE:
						_tcscpy(szText, _T("Restore"));
						break;
					case SW_SHOW:
						_tcscpy(szText, _T("Show"));
						break;
					case SW_SHOWMAXIMIZED:
						_tcscpy(szText, _T("Show Maximized"));
						break;
					case SW_SHOWMINIMIZED:
						_tcscpy(szText, _T("Show Minimized"));
						break;
					case SW_SHOWMINNOACTIVE:
						_tcscpy(szText, _T("Show MinNoactive"));
						break;
					case SW_SHOWNA:
						_tcscpy(szText, _T("Show NA"));
						break;
					case SW_SHOWNOACTIVATE:
						_tcscpy(szText, _T("Show Noactivate"));
						break;
					case SW_SHOWNORMAL:
						_tcscpy(szText, _T("Show Normal"));
						break;
				}
			}
			else if (i < 6 + 8 + 4 + 1 + 1)
			{
				switch (*(int*)databuf)
				{
					case ANSI_FIXED_FONT:
						_tcscpy(szText, _T("ANSI"));
						break;
					case OEM_FIXED_FONT:
						_tcscpy(szText, _T("OEM"));
						break;
				}
			}
			else if (i < 6 + 8 + 4 + 1 + 1 + 2)
				_tcscpy(szText, (TCHAR *)databuf);
			else
				_tcscpy(szText, _T("?"));
			ListView_SetItemText(instdata, item.iItem, 1, szText);
			item.iItem++;
		}
		item.pszText = (TCHAR *)databuf;
		//Add all the MRUs
		for (i = 0 ; i < mrucount ; i++ )
		{
			_sntprintf(szText, RTL_NUMBER_OF(szText) - 1, _T("MRU_File%d"), i + 1);
			DWORD datasize = sizeof databuf;
			ZeroMemory(databuf, sizeof databuf);
			RegQueryValueEx(hk, szText, NULL, NULL, databuf, &datasize);
			item.iItem = i;
			ListView_InsertItem(mru, &item);
		}
		RegCloseKey(hk);
		//Paint the display box
		SetDlgItemText(hw, IDC_DISPLAY, _T("Frhed Display"));
	}
}
