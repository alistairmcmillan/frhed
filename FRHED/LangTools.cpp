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
 * @file  LangTools.cpp
 *
 * @brief A few functions related to UI translation.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: LangTools.cpp 0 0000-00-00 00:00:00Z jtuc $
#include "precomp.h"
#include "hexwnd.h"
#include "AnsiConvert.h"
#include "LangArray.h"

LangArray langArray;

void NTAPI TranslateDialog(HWND hwnd)
{
	if (IsWindowUnicode(hwnd))
		langArray.TranslateDialogW(hwnd);
	else
		langArray.TranslateDialogA(hwnd);
}

INT_PTR NTAPI ShowModalDialog(UINT idd, HWND hwnd, DLGPROC dlgproc, LPVOID param)
{
	HINSTANCE hinst = langArray.m_hLangDll ? langArray.m_hLangDll : hMainInstance;
	return IsWindowUnicode(hwnd)
	? DialogBoxParamW(hinst, MAKEINTRESOURCEW(idd), hwnd, dlgproc, (LPARAM)param)
	: DialogBoxParamA(hinst, MAKEINTRESOURCEA(idd), hwnd, dlgproc, (LPARAM)param);
}

void NTAPI SetWindowText(HWND hwnd, PCWSTR text)
{
	if (IsWindowUnicode(hwnd))
		SetWindowTextW(hwnd, text);
	else
		SetWindowTextA(hwnd, AnsiConvert(text, langArray.m_codepage));
}

void NTAPI SetDlgItemText(HWND hwnd, int id, PCWSTR text)
{
	if (IsWindowUnicode(hwnd))
		SetDlgItemTextW(hwnd, id, text);
	else
		SetDlgItemTextA(hwnd, id, AnsiConvert(text, langArray.m_codepage));
}
