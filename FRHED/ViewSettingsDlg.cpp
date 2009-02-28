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
 * @file  ViewSettingsDlg.cpp
 *
 * @brief Implementation of the View settings -dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "LangArray.h"

static WNDPROC NTAPI SubclassAW(HWND hWnd, WNDPROC wndproc)
{
	return (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)wndproc);
}

static WNDPROC DefWndProcDroppedComboBox = 0;

static LRESULT CALLBACK WndProcDroppedComboBox(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CTLCOLORLISTBOX:
		{
			int cxScreen = GetSystemMetrics(SM_CXSCREEN);
			HWND hLb = (HWND)lParam;
			RECT rcLb;
			::GetWindowRect(hLb, &rcLb);
			if (rcLb.right > cxScreen)
				rcLb.left -= rcLb.right - cxScreen;
			if (rcLb.left < 0)
				rcLb.left = 0;
			::SetWindowPos(hLb, 0, rcLb.left, rcLb.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
		}
		break;
	}
	return CallWindowProc(DefWndProcDroppedComboBox, hWnd, uMsg, wParam, lParam);
}

HWND ViewSettingsDlg::hCbLang;

BOOL ViewSettingsDlg::EnumLocalesProc(LPTSTR lpLocaleString)
{
	TCHAR path[MAX_PATH];
	GetModuleFileName(hMainInstance, path, MAX_PATH);
	PathRenameExtension(path, _T(".lng"));
	LPTSTR name = PathAddBackslash(path);
	LCID lcid = 0;
	if (sscanf(lpLocaleString, "%x", &lcid) == 1)
	{
		LANGID langid = (LANGID)lcid;
		if (int i = GetLocaleInfo(langid, LOCALE_SISO639LANGNAME, name, 4))
		{
			int j = GetLocaleInfo(langid, LOCALE_SISO3166CTRYNAME, name + i, 4);
			name[--i] = '-';
			BOOL f = langid == LangArray::DefLangId;
			while (f == 0)
			{
				strcpy(name + i + j, ".po");
				f = PathFileExists(path);
				if (j == 0 || SUBLANGID(langid) != SUBLANG_DEFAULT)
					break;
				j = 0;
			}
			int k = SendMessage(hCbLang, CB_ADDSTRING, 0, MAKELONG(langid, f));
			if (langid == (langArray.m_langid ? langArray.m_langid : LangArray::DefLangId))
				SendMessage(hCbLang, CB_SETCURSEL, k, 0);
		}
	}
	return TRUE;
}

int ViewSettingsDlg::FormatLangId(LPWSTR bufW, LANGID langid, int verbose)
{
	char bufA[200];
	int i = LangArray::LangCodeMajor(langid, bufA);
	if (i)
	{
		bufA[i - 1] = '-';
		i += LangArray::LangCodeMinor(langid, bufA + i);
		switch (verbose)
		{
		case sizeof(WCHAR):
			MultiByteToWideChar(CP_ACP, 0, bufA, i, bufW, i);
			bufW[i - 1] = '\t';
			i += GetLocaleInfoW(langid, LOCALE_SNATIVELANGNAME|LOCALE_USE_CP_ACP, bufW + i, 40);
			bufW[i - 1] = '\t';
			i += GetLocaleInfoW(langid, LOCALE_SNATIVECTRYNAME|LOCALE_USE_CP_ACP, bufW + i, 40);
			bufW[i - 1] = '\t';
			i += GetLocaleInfoW(langid, LOCALE_SENGLANGUAGE, bufW + i, 40);
			bufW[i - 1] = '\t';
			i += GetLocaleInfoW(langid, LOCALE_SENGCOUNTRY, bufW + i, 40);
			break;
		case sizeof(CHAR):
			bufA[i - 1] = '\t';
			i += GetLocaleInfoA(langid, LOCALE_SNATIVELANGNAME|LOCALE_USE_CP_ACP, bufA + i, 40);
			bufA[i - 1] = '\t';
			i += GetLocaleInfoA(langid, LOCALE_SNATIVECTRYNAME|LOCALE_USE_CP_ACP, bufA + i, 40);
			bufA[i - 1] = '\t';
			i += GetLocaleInfoA(langid, LOCALE_SENGLANGUAGE, bufA + i, 40);
			bufA[i - 1] = '\t';
			i += GetLocaleInfoA(langid, LOCALE_SENGCOUNTRY, bufA + i, 40);
			// fall through
		case 0:
			MultiByteToWideChar(CP_ACP, 0, bufA, i, bufW, i);
			break;
		}
		--i;
	}
	else
	{
		i = swprintf(bufW, L"%04x", langid);
	}
	return i;
}

void ViewSettingsDlg::OnDrawitemLangId(DRAWITEMSTRUCT *pdis)
{
	int iColorText = COLOR_WINDOWTEXT;
	int iColorTextBk = COLOR_WINDOW;
	if (pdis->itemState & ODS_SELECTED)
	{
		iColorText = COLOR_HIGHLIGHTTEXT;
		iColorTextBk =  COLOR_HIGHLIGHT;
	}
	if (HIWORD(pdis->itemData) == 0)
	{
		iColorText = COLOR_GRAYTEXT;
	}
	SetTextColor(pdis->hDC, GetSysColor(iColorText));
	SetBkColor(pdis->hDC, GetSysColor(iColorTextBk));
	int x = pdis->rcItem.left + 2;
	int y = pdis->rcItem.top + 2;
	static const int rgcx[] = { 50, 120, 180, 120, 220, 0 };
	const int *pcx = rgcx;
	UINT flags = ETO_OPAQUE;
	WCHAR buffer[200];
	int length = FormatLangId(buffer, LOWORD(pdis->itemData),
		IsWindowUnicode(pdis->hwndItem) ? sizeof(WCHAR) : sizeof(CHAR));
	LPWSTR p = buffer;
	while (LPWSTR q = StrChrW(p, L'\t'))
	{
		ExtTextOutW(pdis->hDC, x, y, flags, &pdis->rcItem, p, q - p, 0);
		x += *pcx ? *pcx++ : 100;
		p = q + 1;
		flags = 0;
	}
	ExtTextOutW(pdis->hDC, x, y, flags, &pdis->rcItem, p, length - (p - buffer), 0);
	if (pdis->itemState & ODS_FOCUS)
	{
		DrawFocusRect(pdis->hDC, &pdis->rcItem);
	}
}

INT_PTR ViewSettingsDlg::OnCompareitemLangId(COMPAREITEMSTRUCT *pcis)
{
	WCHAR name1[20];
	FormatLangId(name1, LOWORD(pcis->itemData1));
	WCHAR name2[20];
	FormatLangId(name2, LOWORD(pcis->itemData2));
	int cmp = StrCmpIW(name1, name2);
	return cmp < 0 ? -1 : cmp > 0 ? +1 : 0;
	//Code below would yield numeric sort order by first PRIMARYLANGID, then SUBLANGID
	/*WORD w1 = LOWORD(pcis->itemData1);
	w1 = w1 << 10 | w1 >> 10;
	WORD w2 = LOWORD(pcis->itemData2);
	w2 = w2 << 10 | w2 >> 10;
	return w1 < w2 ? -1 : w1 > w2 ? 1 : 0;*/
}

/**
 * @brief Initialize the dialog controls.
 * This function initializes dialog controls from current option values. Not
 * reading from registry since there are values calculated runtime (min.
 * offset length).
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE.
 */
BOOL ViewSettingsDlg::OnInitDialog(HWND hDlg)
{
	SetDlgItemInt(hDlg, IDC_SETTINGS_BYTESPERLINE, iBytesPerLine, TRUE);
	SetDlgItemInt(hDlg, IDC_SETTINGS_OFFSETLEN, iMinOffsetLen, TRUE);
	UINT checked = iAutomaticBPL == 0 ? BST_UNCHECKED : BST_CHECKED;
	CheckDlgButton(hDlg, IDC_SETTINGS_ADJUST_BYTELINE, checked);
	CheckDlgButton(hDlg, bUnsignedView ? IDC_SETTINGS_CARETUNSIGN :
			IDC_SETTINGS_CARETSIGN, BST_CHECKED);
	CheckDlgButton(hDlg, IDC_SETTINGS_OPENRO, bOpenReadOnly);
	checked = bAutoOffsetLen == 0 ? BST_UNCHECKED : BST_CHECKED;
	CheckDlgButton(hDlg, IDC_SETTINGS_ADJOFFSET, checked);
	SetDlgItemText(hDlg, IDC_SETTINGS_EDITOR, TexteditorName);
	hCbLang = GetDlgItem(hDlg, IDC_SETTINGS_LANGUAGE);
	SendMessage(hCbLang, CB_SETDROPPEDWIDTH, 698, 0);
	EnumSystemLocales(EnumLocalesProc, LCID_SUPPORTED);
	return TRUE;
}

/**
 * @brief Save the settings in dialog controls to registry.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE.
 */
BOOL ViewSettingsDlg::Apply(HWND hDlg)
{
	iBytesPerLine = GetDlgItemInt(hDlg, IDC_SETTINGS_BYTESPERLINE, 0, TRUE);
	if (iBytesPerLine < 1)
		iBytesPerLine = 1;
	iMinOffsetLen = GetDlgItemInt(hDlg, IDC_SETTINGS_OFFSETLEN, 0, TRUE);
	if (iMinOffsetLen < 1)
		iMinOffsetLen = 1;
	// Get the text editor path and name.
	GetDlgItemText(hDlg, IDC_SETTINGS_EDITOR, TexteditorName, RTL_NUMBER_OF(TexteditorName));
	
	UINT res = IsDlgButtonChecked(hDlg, IDC_SETTINGS_ADJUST_BYTELINE);
	if (res == BST_CHECKED)
		iAutomaticBPL = 1;
	else
		iAutomaticBPL = 0;

	res = IsDlgButtonChecked(hDlg, IDC_SETTINGS_ADJOFFSET);
	if (res == BST_CHECKED)
		bAutoOffsetLen = 1;
	else
		bAutoOffsetLen = 0;

	res = IsDlgButtonChecked(hDlg, IDC_SETTINGS_CARETUNSIGN);
	if (res == BST_CHECKED)
		bUnsignedView = true;
	else if (res == BST_UNCHECKED)
		bUnsignedView = false;

	bOpenReadOnly = IsDlgButtonChecked(hDlg, IDC_SETTINGS_OPENRO);
	int i = SendMessage(hCbLang, CB_GETCURSEL, 0, 0);
	if (i != -1)
	{
		DWORD itemData = (DWORD)SendMessage(hCbLang, CB_GETITEMDATA, i, 0);
		load_lang((LANGID)itemData);
	}
	save_ini_data();
	resize_window();
	return TRUE;
}

/**
 * @brief Show the file selection dialog to select text editor executable.
 * @param [in] hDlg Handle to the view settings dialog.
 */
void ViewSettingsDlg::SelectEditor(HWND hDlg)
{
	char szFileName[MAX_PATH];
	szFileName[0] = '\0';
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof ofn);
	ofn.lStructSize = sizeof ofn;
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "All Files (*.*)\0*.*\0\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
	if (GetOpenFileName(&ofn))
	{
		strncpy(TexteditorName, szFileName, MAX_PATH);
		SetDlgItemText(hDlg, IDC_SETTINGS_EDITOR, TexteditorName);
	}
}

INT_PTR ViewSettingsDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hDlg);
	case WM_COMMAND:
		switch (wParam)
		{
		case IDC_SETTINGS_EDITORSELECT:
			SelectEditor(hDlg);
			return TRUE;
		case IDOK:
			if (Apply(hDlg))
			{
			case IDCANCEL:
				EndDialog(hDlg, wParam);
			}
			return TRUE;
		case MAKEWPARAM(IDC_SETTINGS_LANGUAGE, CBN_DROPDOWN):
			if (DefWndProcDroppedComboBox == 0)
			{
				DefWndProcDroppedComboBox = SubclassAW((HWND)lParam, WndProcDroppedComboBox);
			}
			return TRUE;
		case MAKEWPARAM(IDC_SETTINGS_LANGUAGE, CBN_CLOSEUP):
			if (DefWndProcDroppedComboBox)
			{
				SubclassAW((HWND)lParam, DefWndProcDroppedComboBox);
				DefWndProcDroppedComboBox = 0;
			}
			return TRUE;
		}
		break;
	case WM_DRAWITEM:
		switch (wParam)
		{
		case IDC_SETTINGS_LANGUAGE:
			OnDrawitemLangId(reinterpret_cast<DRAWITEMSTRUCT *>(lParam));
			return TRUE;
		}
		break;
	case WM_COMPAREITEM:
		switch (wParam)
		{
		case IDC_SETTINGS_LANGUAGE:
			return OnCompareitemLangId(reinterpret_cast<COMPAREITEMSTRUCT *>(lParam));
		}
		break;

	case WM_HELP:
		OnHelp(hDlg);
		break;
	}
	return FALSE;
}
